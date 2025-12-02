// ============================================================================
//  Worm030 - modularisierte Version mit Extras - Fabian Ausmann
//
//  Enthaltene Erweiterungen:
//   - diagonale Steuerung (Tasten '1'–'4')
//   - Ringpuffer-Wurm (Positionsspeicherung im Kreis-Puffer)
//   - Erkennung von Selbstkollisionen
//   - Game-Over-Ausgabe im Konsolenmodus (nach ncurses)
//   - Hintergrundfarbe für das Spielfeld
//   - Pausenmodus mit Einzelschritten über Pfeiltasten und 1–4
// ============================================================================

#include <curses.h>    // ncurses-Funktionen und -Konstanten
#include <stdio.h>     // printf, Standard-Ein-/Ausgabe
#include <stdlib.h>    // allgemeine Hilfsfunktionen
#include <stdbool.h>   // bool-Typ (true/false)
#include <time.h>      // ggf. für Zeitfunktionen
#include <string.h>    // String-Funktionen (Reserve)
#include <unistd.h>    // POSIX-Funktionen, z. B. sleep

#include "prep.h"         // Initialisierung und Aufräumen von ncurses
#include "worm.h"         // globale Konstanten und Aufzählungstypen
#include "worm_model.h"   // Wurm-Datenstruktur und Bewegungsfunktionen
#include "board_model.h"  // Funktionen für Spielfeld und Bildschirmgrenzen

// ============================================================================
//  GLOBALER SPIELZUSTAND (nur in diesem Modul)
// ============================================================================
//
//  paused:
//   - false : Spiel läuft, Wurm bewegt sich kontinuierlich
//   - true  : Spiel pausiert, Wurm bewegt sich nur bei Richtungs-Taste
// ============================================================================

static bool paused = false;

// ============================================================================
//  FARBEN
// ============================================================================

void initializeColors(void) {
    start_color();   // Farbmodus in ncurses aktivieren

    //                Vordergrund , Hintergrund
    init_pair(COLP_USER_WORM, COLOR_GREEN, COLOR_WHITE);
    init_pair(COLP_FREE_CELL, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLP_WORM_HEAD, COLOR_RED,   COLOR_WHITE);

    // gesamtes Fenster mit der Hintergrundfarbe für freie Zellen füllen
    wbkgd(stdscr, COLOR_PAIR(COLP_FREE_CELL));
    refresh();   // Änderungen sofort sichtbar machen
}

// ============================================================================
//  EINGABEVERARBEITUNG
// ============================================================================
//
//  Liest die Benutzereingabe ein und:
//   - ändert ggf. die Bewegungsrichtung
//   - setzt den Spielzustand (Quit)
//   - setzt den Pausenstatus
//
//  Rückgabewert:
//   - true  : Im PAUSENMODUS soll ein EINZELSCHRITT ausgeführt werden
//             (Pfeiltaste oder 1–4)
//   - false : kein Einzelschritt (entweder keine Taste oder andere Taste)
//
//  Im Laufmodus (paused == false) wird der Rückgabewert ignoriert,
//  da der Wurm sowieso in jedem Frame einen Schritt macht.
// ============================================================================

static bool readUserInput(enum GameStates* agame_state) {
    int ch;
    bool step_request = false;   // Einzelschritt im Pausenmodus gewünscht?

    if ((ch = getch()) > 0) {

        switch(ch) {

            // Spiel explizit beenden
            case 'q':
                *agame_state = WORM_GAME_QUIT;
                break;

            // Pfeiltasten für Bewegung in Haupt-Richtungen
            case KEY_UP:
                setWormHeading(WORM_UP);
                step_request = true;
                break;
            case KEY_DOWN:
                setWormHeading(WORM_DOWN);
                step_request = true;
                break;
            case KEY_LEFT:
                setWormHeading(WORM_LEFT);
                step_request = true;
                break;
            case KEY_RIGHT:
                setWormHeading(WORM_RIGHT);
                step_request = true;
                break;

            // diagonale Bewegung über Zifferntasten
            case '1':
                setWormHeading(WORM_UP_LEFT);
                step_request = true;
                break;
            case '2':
                setWormHeading(WORM_UP_RIGHT);
                step_request = true;
                break;
            case '3':
                setWormHeading(WORM_DOWN_RIGHT);
                step_request = true;
                break;
            case '4':
                setWormHeading(WORM_DOWN_LEFT);
                step_request = true;
                break;

            // Spiel pausieren:
            //  - paused = true  -> Wurm bewegt sich nur noch bei Richtungs-Tasten
            //  - getch() blockiert (nodelay = FALSE)
            case 's':
                paused = true;
                nodelay(stdscr, FALSE);
                break;

            // Spiel fortsetzen:
            //  - paused = false -> Wurm bewegt sich wieder kontinuierlich
            //  - getch() nicht-blockierend (nodelay = TRUE)
            case ' ':
                paused = false;
                nodelay(stdscr, TRUE);
                break;

            default:
                // andere Tasten ändern weder Richtung noch lösen sie einen Schritt aus
                break;
        }
    }

    return step_request;
}

// ============================================================================
//  GAME-OVER AUSGABE
// ============================================================================

void showGameOverMessage(enum GameStates agame_state) {

    cleanupCursesApp();

    printf("\n -= GAME OVER =-\n");

    switch (agame_state) {

        case WORM_OUT_OF_BOUNDS:
            printf(" Der Wurm hat das Spielfeld verlassen.\n");
            break;

        case WORM_CROSSING:
            printf(" Der Wurm ist mit sich selbst kollidiert.\n");
            break;

        case WORM_GAME_QUIT:
            printf(" Das Spiel wurde durch den Benutzer beendet.\n");
            break;

        default:
            printf(" Spiel beendet.\n");
            break;
    }

    printf("    - TRY AGAIN -\n");
}

// ============================================================================
//  STARTBILDSCHIRM
// ============================================================================
//
//  Zeigt beim Programmstart eine Meldung in der Mitte des Fensters an und
//  wartet, bis der Benutzer eine beliebige Taste drückt.
//
//  Zweck:
//   - Der Wurm wird bereits initial angezeigt, bewegt sich aber noch nicht.
//   - Erst nach Tastendruck beginnt die eigentliche Spielschleife.
// ============================================================================

static void showStartScreen(void) {
    const char* msg = "Zum starten bitte eine beliebige Taste drücken!";

    // Position so berechnen, dass die Meldung etwa mittig erscheint
    int y = getLastRow() / 2;
    int x = (getLastCol() - (int)strlen(msg)) / 2;

    // getch() blockierend machen: wir wollen explizit WARTEN
    nodelay(stdscr, FALSE);

    mvprintw(y, x, "%s", msg);
    refresh();

    // Auf eine beliebige Taste warten
    getch();

    // Bildschirm leeren und Wurm neu zeichnen,
    // damit die Meldung verschwindet
    clear();
    showWorm();
    refresh();

    // getch() wieder nicht-blockierend einstellen
    nodelay(stdscr, TRUE);
}

// ============================================================================
//  GAME LOOP
// ============================================================================
//
//  Im Laufmodus (paused == false):
//    - In jedem Schleifendurchlauf wird der Wurm einen Schritt bewegt.
//
//  Im Pausenmodus (paused == true):
//    - Der Wurm bewegt sich nur, wenn readUserInput()
//      einen step_request == true liefert (Pfeiltasten oder 1–4).
// ============================================================================

enum ResCodes doLevel(void) {

    enum GameStates game_state = WORM_GAME_ONGOING;
    paused = false;               // Level startet im Laufmodus (für die Schleife)
    nodelay(stdscr, TRUE);        // getch() nicht-blockierend (für den Laufmodus)

    // Initialer Wurm: links unten, bewegt sich nach rechts
    initializeWorm(WORM_LENGTH, getLastRow(), 0,
                   WORM_RIGHT, COLP_USER_WORM);

    showWorm();
    refresh();

    // Startbildschirm anzeigen:
    // Das Spiel wartet hier zunächst auf eine beliebige Taste,
    // bevor der Wurm sich tatsächlich zu bewegen beginnt.
    showStartScreen();

    while (game_state == WORM_GAME_ONGOING) {

        // Eingaben verarbeiten (Richtung, Quit, Pause/Fortsetzen)
        bool step_request = readUserInput(&game_state);

        // Falls das Spiel durch Eingabe bereits beendet wurde
        if (game_state != WORM_GAME_ONGOING)
            break;

        // --- Verhalten im Pausenmodus --------------------------------------
        if (paused) {
            // Nur wenn eine Richtungs-Taste gedrückt wurde (Pfeil / 1–4)
            // soll ein Einzelschritt ausgeführt werden.
            if (!step_request) {
                // keine Schritt-Anfrage -> keine Bewegung
                continue;   // nächste Eingabe abwarten
            }
            // step_request == true -> genau ein Schritt ausführen
        }
        // --------------------------------------------------------------------

        // Wurmschwanz löschen und Wurm ein Feld weiter bewegen
        cleanWormTail();
        moveWorm(&game_state);

        // Nach der Bewegung prüfen, ob das Spiel zu Ende ist
        if (game_state != WORM_GAME_ONGOING)
            break;

        // Wurm in neuer Position zeichnen
        showWorm();
        napms(NAP_TIME);
        refresh();
    }

    // Nach dem Verlassen der Spielschleife: Game-Over-Nachricht ausgeben
    showGameOverMessage(game_state);
    return RES_OK;
}

// ============================================================================
//  MAIN
// ============================================================================

int main(void) {

    initializeCursesApplication();
    initializeColors();

    // Fenstergröße prüfen
    if (LINES < MIN_NUMBER_OF_ROWS ||
        COLS  < MIN_NUMBER_OF_COLS) {

        endwin();
        printf("\n Das Fenster ist zu klein! Mindestgröße: %dx%d\n",
               MIN_NUMBER_OF_COLS, MIN_NUMBER_OF_ROWS);
        return RES_FAILED;
    }

    doLevel();

    return RES_OK;
}

