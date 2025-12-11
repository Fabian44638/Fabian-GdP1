// Worm070 - Aufgabenblatt 8
// ============================================================================
//  Hauptprogramm für das Worm-Spiel
//
//  Diese Datei enthält den zentralen Steuerablauf des Spiels:
//    - Initialisierung der ncurses-Umgebung und Farben
//    - Auswertung der Tastatureingaben
//    - Darstellung des Spielfelds und des Wurms
//    - Spielschleife mit Bewegungslogik
//    - Behandlung aller Game-Over-Situationen
//
//  Die Datei bildet den Rahmen des gesamten Spiels und nutzt alle anderen
//  Module. Die Spiellogik selbst befindet sich in worm_model.c, das Board
//  wird in board_model.c verwaltet, Meldungen erscheinen über messages.c.
// ============================================================================

#include <curses.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "prep.h"
#include "worm.h"
#include "worm_model.h"
#include "board_model.h"
#include "messages.h"

// ---------------------------------------------------------------------------
// Globale Variable zur Steuerung des Pausenmodus.
//
// paused = false   Der Wurm bewegt sich normal.
// paused = true    Das Spiel steht still und reagiert nur auf bestimmte Tasten.
// ---------------------------------------------------------------------------
static bool paused = false;


// ---------------------------------------------------------------------------
// initializeColors
// ---------------------------------------------------------------------------
// Aufgabe:
//    Definiert alle Farbkombinationen, die im Spiel verwendet werden.
//    Die Darstellung erfolgt über ncurses-Farbpaare.
//
// Vorgehen:
//    - start_color() aktiviert die Farbdarstellung
//    - init_pair erzeugt ein Farbpaar (Vordergrund, Hintergrund)
//    - wbkgd setzt den einheitlichen Hintergrund für das gesamte Fenster
//
// Die Farben werden in worm.h über das Enum ColorPairs referenziert.
// ---------------------------------------------------------------------------

void initializeColors(void)
{
    start_color();

    init_pair(COLP_FREE_CELL, COLOR_BLACK,   COLOR_WHITE);   // neutrale Spielfelder
    init_pair(COLP_USER_WORM, COLOR_GREEN,   COLOR_WHITE);   // Körper des Wurms
    init_pair(COLP_WORM_HEAD, COLOR_RED,     COLOR_WHITE);   // Kopf des Wurms

    init_pair(COLP_FOOD_1,    COLOR_BLUE,    COLOR_WHITE);   // Futtertyp 1
    init_pair(COLP_FOOD_2,    COLOR_MAGENTA, COLOR_WHITE);   // Futtertyp 2
    init_pair(COLP_FOOD_3,    COLOR_CYAN,    COLOR_WHITE);   // Futtertyp 3

    init_pair(COLP_BARRIER,   COLOR_RED,     COLOR_WHITE);   // Barrieren

    // Einheitlicher weißer Hintergrund für das Spielfeld
    wbkgd(stdscr, COLOR_PAIR(COLP_FREE_CELL));
    refresh();
}


// ---------------------------------------------------------------------------
// readUserInput
// ---------------------------------------------------------------------------
// Aufgabe:
//    Verarbeitet eine Tastatureingabe und reagiert darauf:
//
//    - Richtungsänderungen des Wurms
//    - Spiel beenden
//    - diagonale Bewegungen (Sondertasten 1 bis 4)
//    - manuelles Wachstum
//    - Pause ein und aus
//
// Rückgabewert:
//    true  wenn ein Bewegungsschritt durchgeführt werden soll
//    false wenn keine Bewegung stattfinden soll (zum Beispiel bei Pause)
//
// Hinweis:
//    Das Spiel nutzt standardmäßig non-blocking Eingabe. Nur Dialoge arbeiten
//    blockierend. Das Abfangen von ERR bedeutet, dass keine Taste gedrückt wurde.
// ---------------------------------------------------------------------------

static bool readUserInput(struct worm* aworm,
                          enum GameStates* agame_state)
{
    int ch;
    bool do_step = false;

    ch = getch();
    if (ch == ERR) {
        return false;    // Keine Eingabe vorhanden
    }

    switch(ch) {

        case 'q':
            *agame_state = WORM_GAME_QUIT;
            break;

        // Bewegungsrichtungen
        case KEY_UP:
            setWormHeading(aworm, WORM_UP);
            do_step = true;
            break;

        case KEY_DOWN:
            setWormHeading(aworm, WORM_DOWN);
            do_step = true;
            break;

        case KEY_LEFT:
            setWormHeading(aworm, WORM_LEFT);
            do_step = true;
            break;

        case KEY_RIGHT:
            setWormHeading(aworm, WORM_RIGHT);
            do_step = true;
            break;

        // Diagonale Bewegungen
        case '1':
            setWormHeading(aworm, WORM_UP_LEFT);
            do_step = true;
            break;

        case '2':
            setWormHeading(aworm, WORM_UP_RIGHT);
            do_step = true;
            break;

        case '3':
            setWormHeading(aworm, WORM_DOWN_RIGHT);
            do_step = true;
            break;

        case '4':
            setWormHeading(aworm, WORM_DOWN_LEFT);
            do_step = true;
            break;

        // Manuelles Wachstum durch Taste g
        case 'g':
            growWorm(aworm, BONUS_MANUAL);
            break;

        // Pause einschalten
        case 's':
            paused = true;
            nodelay(stdscr, FALSE);
            break;

        // Pause beenden
        case ' ':
            paused = false;
            nodelay(stdscr, TRUE);
            break;

        default:
            break;
    }

    return do_step;
}


// ---------------------------------------------------------------------------
// showGameOverMessage
// ---------------------------------------------------------------------------
// Aufgabe:
//    Gibt abhängig von der Endursache eine passende Game-Over-Meldung aus.
//
// Vorgehen:
//    - Text auswählen je nach GameStates
//    - Dialog anzeigen
//    - ncurses korrekt beenden
// ---------------------------------------------------------------------------

void showGameOverMessage(enum GameStates gamestate)
{
    const char* line1 = NULL;
    const char* line2 =
        "Bitte eine beliebige Taste drücken, um das Spiel zu beenden.";

    switch (gamestate) {

        case WORM_OUT_OF_BOUNDS:
            line1 = "GAME OVER - Spielfeld verlassen";
            break;

        case WORM_CROSSING:
            line1 = "GAME OVER - Selbstkollision";
            break;

        case WORM_CRASH:
            line1 = "GAME OVER - Barriere getroffen";
            break;

        case WORM_GAME_QUIT:
            line1 = "Der Spieler hat das Spiel beendet.";
            break;

        default:
            line1 = "GAME OVER";
            break;
    }

    showDialog((char*)line1, (char*)line2);
    cleanupCursesApp();
}


// ---------------------------------------------------------------------------
// showStartScreen
// ---------------------------------------------------------------------------
// Aufgabe:
//    Zeigt einen Startbildschirm an, der den Spieler auffordert,
//    eine Taste zu drücken.
//
// Ablauf:
//    - Text in der Mitte des Bildschirms ausgeben
//    - auf Tastendruck warten
//    - Spielfeld neu aufbauen
//    - Wurm zeichnen
// ---------------------------------------------------------------------------

static void showStartScreen(struct board* aboard,
                            struct worm* aworm)
{
    const char* msg =
        "Bitte eine beliebige Taste drücken, um das Spiel zu starten.";

    nodelay(stdscr, FALSE);

    clear();

    int y = LINES / 2;
    int x = (COLS - (int)strlen(msg)) / 2;

    mvprintw(y, x, "%s", msg);
    refresh();
    getch();

    clear();

    initializeLevel(aboard);
    showWorm(aboard, aworm);
    refresh();

    nodelay(stdscr, TRUE);
}


// ---------------------------------------------------------------------------
// doLevel
// ---------------------------------------------------------------------------
// Aufgabe:
//    Führt ein vollständiges Spiellevel aus. Dies ist die eigentliche
//    Hauptspielschleife.
//
// Schritte:
//
//    1. Board und Wurm initialisieren
//    2. Startbildschirm anzeigen
//    3. Status und Trennlinie zeigen
//    4. Endlosschleife:
//       - Eingaben auswerten
//       - Wurm bewegen
//       - Futter, Kollisionen, Wachstum verarbeiten
//       - Spielfeld neu darstellen
//    5. Bei Game Over letzten Zustand darstellen und Meldung anzeigen
// ---------------------------------------------------------------------------

enum ResCodes doLevel(void)
{
    struct board theBoard;
    struct worm  userWorm;

    enum GameStates game_state = WORM_GAME_ONGOING;

    paused = false;
    nodelay(stdscr, TRUE);

    initializeLevel(&theBoard);

    struct pos headpos;
    headpos.y = getLastRowOnBoard(&theBoard);
    headpos.x = 0;

    initializeWorm(&userWorm,
                   WORM_LENGTH,
                   WORM_INITIAL_LENGTH,
                   headpos,
                   WORM_RIGHT,
                   COLP_USER_WORM);

    showWorm(&theBoard, &userWorm);
    refresh();

    showStartScreen(&theBoard, &userWorm);

    showBorderLine();
    showStatus(&theBoard, &userWorm);

    while (game_state == WORM_GAME_ONGOING) {

        bool step = readUserInput(&userWorm, &game_state);

        if (game_state != WORM_GAME_ONGOING)
            break;

        if (paused && !step)
            continue;

        cleanWormTail(&theBoard, &userWorm);

        moveWorm(&theBoard,
                 &userWorm,
                 &game_state);

        if (game_state != WORM_GAME_ONGOING)
            break;

        showWorm(&theBoard, &userWorm);

        showStatus(&theBoard, &userWorm);

        napms(NAP_TIME);
        refresh();
    }

    if (game_state != WORM_GAME_QUIT) {
        showWorm(&theBoard, &userWorm);
        showStatus(&theBoard, &userWorm);
        refresh();
    }

    showGameOverMessage(game_state);
    return RES_OK;
}


// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
// Aufgabe:
//    Programmanfang. Initialisiert ncurses und Farben,
//    prüft die Fenstergröße und startet das Level.
//
// Rückgabe:
//    RES_OK bei Erfolg
//    RES_FAILED wenn das Fenster zu klein ist
// ---------------------------------------------------------------------------

int main(void)
{
    initializeCursesApplication();
    initializeColors();

    if (LINES < MIN_NUMBER_OF_ROWS + ROWS_RESERVED ||
        COLS  < MIN_NUMBER_OF_COLS)
    {
        endwin();
        printf("\nDas Fenster ist zu klein! Mindestgröße: %dx%d\n",
                MIN_NUMBER_OF_COLS,
                MIN_NUMBER_OF_ROWS + ROWS_RESERVED);
        return RES_FAILED;
    }

    doLevel();
    return RES_OK;
}






