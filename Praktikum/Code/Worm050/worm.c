// ============================================================================
//  Worm050 - modularisierte Version mit Extras - Fabian Ausmann
//
//  Enthaltene Erweiterungen:
//   - diagonale Steuerung (Tasten '1'–'4')
//   - Ringpuffer-Wurm (Positionsspeicherung im Kreis-Puffer)
//   - Erkennung von Selbstkollisionen
//   - Game-Over-Ausgabe (jetzt in der Message Area)
//   - Hintergrundfarbe für das Spielfeld
//   - Pausenmodus mit Einzelschritten über Pfeiltasten und 1–4
//
//  Ab Aufgabenblatt 7:
//   - Verwendung von struct worm (statt globaler Wurm-Variablen)
//   - Positionen als struct pos
//   - Alle Wurm-Operationen arbeiten mit struct worm* aworm
//   - Verwendung einer Message Area am unteren Fensterrand
//     (Trennlinie + Statusanzeige des Wurms)
// ============================================================================

#include <curses.h>    // ncurses-Funktionen und -Konstanten
#include <stdio.h>     // printf, Standard-Ein-/Ausgabe (nur noch für Fehlermeldungen)
#include <stdlib.h>    // allgemeine Hilfsfunktionen
#include <stdbool.h>   // bool-Typ (true/false)
#include <time.h>      // ggf. für Zeitfunktionen
#include <string.h>    // String-Funktionen (Reserve)
#include <unistd.h>    // POSIX-Funktionen, z. B. sleep

#include "prep.h"         // Initialisierung und Aufräumen von ncurses
#include "worm.h"         // globale Konstanten und Aufzählungstypen
#include "worm_model.h"   // Wurm-Datenstruktur und Bewegungsfunktionen
#include "board_model.h"  // Funktionen für Spielfeld und Bildschirmgrenzen
#include "messages.h"     // Funktionen für Message Area (Status, Dialoge)

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
//
//  Hier werden die in worm.h definierten Farbpaare mit konkreten Farben
//  verknüpft. Zusätzlich wird der Hintergrund des Fensters gesetzt.
// ============================================================================

void initializeColors(void) {
    start_color();   // Farbmodus in ncurses aktivieren

    //                Vordergrund , Hintergrund
    init_pair(COLP_USER_WORM, COLOR_GREEN, COLOR_WHITE);
    init_pair(COLP_FREE_CELL, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLP_BARRIER,   COLOR_BLACK, COLOR_WHITE); // Barriere/Trennlinie
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
//   - ändert ggf. die Bewegungsrichtung des übergebenen Wurms
//   - setzt den Spielzustand (Quit)
//   - setzt den Pausenstatus
//
//  Parameter:
//   - aworm       : Zeiger auf den Wurm, dessen Richtung geändert wird
//   - agame_state : Zeiger auf den aktuellen Spielzustand
//
//  Rückgabewert:
//   - true  : Im PAUSENMODUS soll ein EINZELSCHRITT ausgeführt werden
//             (Pfeiltaste oder 1–4)
//   - false : kein Einzelschritt (entweder keine Taste oder andere Taste)
//
//  Im Laufmodus (paused == false) wird der Rückgabewert ignoriert,
//  da der Wurm sowieso in jedem Frame einen Schritt macht.
// ============================================================================

static bool readUserInput(struct worm* aworm, enum GameStates* agame_state) {
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
                setWormHeading(aworm, WORM_UP);
                step_request = true;
                break;
            case KEY_DOWN:
                setWormHeading(aworm, WORM_DOWN);
                step_request = true;
                break;
            case KEY_LEFT:
                setWormHeading(aworm, WORM_LEFT);
                step_request = true;
                break;
            case KEY_RIGHT:
                setWormHeading(aworm, WORM_RIGHT);
                step_request = true;
                break;

            // diagonale Bewegung über Zifferntasten
            case '1':
                setWormHeading(aworm, WORM_UP_LEFT);
                step_request = true;
                break;
            case '2':
                setWormHeading(aworm, WORM_UP_RIGHT);
                step_request = true;
                break;
            case '3':
                setWormHeading(aworm, WORM_DOWN_RIGHT);
                step_request = true;
                break;
            case '4':
                setWormHeading(aworm, WORM_DOWN_LEFT);
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
//
//  Zeigt das Spielergebnis in der MESSAGE AREA an, wartet auf eine Taste
//  und beendet DANN erst ncurses.
//
//  Unterschied zur alten Version:
//   - Es wird NICHT mehr auf der Kommandozeile mit printf ausgegeben.
//   - Stattdessen wird showDialog() aus messages.c benutzt.
// ============================================================================

void showGameOverMessage(enum GameStates agame_state) {

    const char* msg1 = NULL;   // Hauptzeile
    const char* msg2 = "Bitte eine beliebige Taste drücken, um das Spiel zu beenden.";

    switch (agame_state) {

        case WORM_OUT_OF_BOUNDS:
            msg1 = "GAME OVER - Der Wurm hat das Spielfeld verlassen.";
            break;

        case WORM_CROSSING:
            msg1 = "GAME OVER - Der Wurm ist mit sich selbst kollidiert.";
            break;

        case WORM_GAME_QUIT:
            msg1 = "GAME OVER - Das Spiel wurde vom Benutzer beendet.";
            break;

        default:
            msg1 = "GAME OVER - Spiel beendet.";
            break;
    }

    // Dialog in der Message Area anzeigen und auf eine Taste warten
    // (showDialog kümmert sich um das Löschen und Wiederherstellen der Zeilen)
    showDialog((char*)msg1, (char*)msg2);

    // Jetzt ncurses sauber beenden
    cleanupCursesApp();

    // KEINE zusätzliche printf-Ausgabe mehr –
    // der Benutzer hat die Meldung bereits in der Message Area gesehen.
}

// ============================================================================
//  STARTBILDSCHIRM
// ============================================================================
//
//  Zeigt beim Programmstart eine Meldung in der Mitte des Fensters an und
//  wartet, bis der Benutzer eine beliebige Taste drückt.
//
//  Parameter:
//   - aworm : Zeiger auf den Wurm, der bereits initialisiert und gezeichnet ist
//
//  Zweck:
//   - Der Wurm wird bereits initial angezeigt, bewegt sich aber noch nicht.
//   - Erst nach Tastendruck beginnt die eigentliche Spielschleife.
//
//  Hinweis:
//   - Diese Funktion verwendet den gesamten Bildschirm. Die Message Area
//     (Barriere + Status) wird erst NACH dem Startscreen wieder aufgebaut.
// ============================================================================

static void showStartScreen(struct worm* aworm) {
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
    showWorm(aworm);
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
//
//  Ab Aufgabenblatt 7:
//    - Es wird ein lokaler Wurm (struct worm userworm) angelegt,
//      der an alle Wurm-Funktionen übergeben wird.
//    - Unter dem Spielfeld befindet sich eine Message Area, die durch
//      eine Barriere getrennt wird und Statusinformationen anzeigt.
// ============================================================================

enum ResCodes doLevel(void) {

    enum GameStates game_state = WORM_GAME_ONGOING;
    paused = false;               // Level startet im Laufmodus (für die Schleife)
    nodelay(stdscr, TRUE);        // getch() nicht-blockierend (für den Laufmodus)

    // Lokale Wurm-Instanz für dieses Level
    struct worm userworm;

    // Startposition des Wurmkopfes: links unten
    struct pos headpos;
    headpos.y = getLastRow();
    headpos.x = 0;

    // Initialer Wurm: links unten, bewegt sich nach rechts
    initializeWorm(&userworm,
                   WORM_LENGTH,
                   headpos,
                   WORM_RIGHT,
                   COLP_USER_WORM);

    showWorm(&userworm);
    refresh();

    // Startbildschirm anzeigen:
    // Das Spiel wartet hier zunächst auf eine beliebige Taste,
    // bevor der Wurm sich tatsächlich zu bewegen beginnt.
    showStartScreen(&userworm);

    // Nach dem Startscreen: Message Area einrichten
    //  - Barriere zeichnen
    //  - Statuszeile mit aktueller Kopfposition ausgeben
    showBorderLine();
    showStatus(&userworm);
    refresh();

    while (game_state == WORM_GAME_ONGOING) {

        // Eingaben verarbeiten (Richtung, Quit, Pause/Fortsetzen)
        bool step_request = readUserInput(&userworm, &game_state);

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
        cleanWormTail(&userworm);
        moveWorm(&userworm, &game_state);

        // Nach der Bewegung prüfen, ob das Spiel zu Ende ist
        if (game_state != WORM_GAME_ONGOING)
            break;

        // Wurm in neuer Position zeichnen
        showWorm(&userworm);

        // Statuszeile aktualisieren (z. B. Kopfposition)
        showStatus(&userworm);

        napms(NAP_TIME);
        refresh();
    }

    // Nach dem Verlassen der Spielschleife: Game-Over-Nachricht in der
    // MESSAGE AREA anzeigen und dann ncurses beenden.
    showGameOverMessage(game_state);
    return RES_OK;
}

// ============================================================================
//  MAIN
// ============================================================================
//
//  Startpunkt des Programms.
//  Hier wird ncurses initialisiert, die Fenstergröße geprüft und
//  anschließend doLevel() aufgerufen.
//
//  WICHTIG ab Blatt 7:
//   - Für ein gültiges Spielfeld muss die Fensterhöhe sowohl
//     den Wurm-Spielbereich (MIN_NUMBER_OF_ROWS) als auch die
//     reservierten Zeilen für die Message Area (ROWS_RESERVED)
//     enthalten.
// ============================================================================

int main(void) {

    initializeCursesApplication();
    initializeColors();

    // Fenstergröße prüfen:
    // - Höhe: mindestens MIN_NUMBER_OF_ROWS für das Spielfeld
    //         plus ROWS_RESERVED für die Message Area
    // - Breite: mindestens MIN_NUMBER_OF_COLS
    if (LINES < MIN_NUMBER_OF_ROWS + ROWS_RESERVED ||
        COLS  < MIN_NUMBER_OF_COLS) {

        endwin();
        printf("\n Das Fenster ist zu klein! Mindestgröße: %dx%d\n",
               MIN_NUMBER_OF_COLS,
               MIN_NUMBER_OF_ROWS + ROWS_RESERVED);
        return RES_FAILED;
    }

    doLevel();

    return RES_OK;
}



