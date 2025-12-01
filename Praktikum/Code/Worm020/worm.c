// ============================================================================
// Fabian Ausmann - Aufgabenblatt 5
// Version mit:
//  - diagonaler Steuerung (1–4)
//  - Ringpuffer-Wurm
//  - Selbstkollision
//  - Game-Over-Ausgabe
//  - Hintergrundfarbe
// ============================================================================

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

// ============================================================================
//  KONFIGURATION
// ============================================================================

// Pausenzeit zwischen Frames (ms)
#define NAP_TIME 100

// Mindestgröße des Spielfensters
#define MIN_NUMBER_OF_ROWS 15
#define MIN_NUMBER_OF_COLS 20

// Länge des Wurms
#define WORM_LENGTH 8

// Marker für unbenutzte Wurm-Segmente
#define UNUSED_POS_ELEM -1

// Zeichen auf dem Spielfeld
#define SYMBOL_FREE_CELL           ' '   // freies Feld
#define SYMBOL_WORM_INNER_ELEMENT  '0'   // Körper-Segment
#define SYMBOL_WORM_HEAD           'X'   // Kopf
#define SYMBOL_WORM_TAIL           'X'   // Schwanz

// ============================================================================
//  ENUMS
// ============================================================================

// Rückgabewerte
enum ResCodes {
    RES_OK,
    RES_FAILED
};

// Farbpaare für ncurses
enum ColorPairs {
    COLP_USER_WORM = 1,
    COLP_FREE_CELL,
    COLP_WORM_HEAD
};

// Mögliche Spielzustände
enum GameStates {
    WORM_GAME_ONGOING,
    WORM_OUT_OF_BOUNDS,
    WORM_CROSSING,
    WORM_GAME_QUIT
};

// Bewegungsrichtungen des Wurms
enum WormHeading {
    WORM_UP,
    WORM_DOWN,
    WORM_LEFT,
    WORM_RIGHT,
    WORM_UP_LEFT,
    WORM_UP_RIGHT,
    WORM_DOWN_RIGHT,
    WORM_DOWN_LEFT
};

// ============================================================================
//  GLOBALE VARIABLEN
// ============================================================================

// Ringpuffer-Indizes
int theworm_maxindex;
int theworm_headindex;

// Positionsarrays des Wurms
int theworm_wormpos_y[WORM_LENGTH];
int theworm_wormpos_x[WORM_LENGTH];

// Bewegungsdelta pro Tick
int theworm_dx;
int theworm_dy;

// Farbe des Wurms
enum ColorPairs theworm_wcolor;

// ============================================================================
//  PROTOTYPEN
// ============================================================================

void initializeColors(void);
void initializeCursesApplication(void);
void cleanupCursesApp(void);

int getLastRow(void);
int getLastCol(void);

void placeItem(int y, int x, chtype symbol, enum ColorPairs color_pair);

enum ResCodes initializeWorm(int len_max, int y, int x,
                             enum WormHeading dir,
                             enum ColorPairs color);

void readUserInput(enum GameStates* agame_state);
void setWormHeading(enum WormHeading dir);

void showWorm(void);
void cleanWormTail(void);
void moveWorm(enum GameStates* agame_state);

bool isInUseByWorm(int y, int x);

enum ResCodes doLevel(void);

void showGameOverMessage(enum GameStates agame_state);

// ============================================================================
//  INITIALISIERUNG VON NCURSES
// ============================================================================

// Startet ncurses + Grundeinstellungen
void initializeCursesApplication(void) {
    initscr();              // Terminal initialisieren
    noecho();               // keine Zeichenecho-Ausgabe
    cbreak();               // direkte Tastenverarbeitung
    nonl();
    keypad(stdscr, TRUE);  // Pfeiltasten aktivieren
    curs_set(0);           // Cursor unsichtbar machen
    nodelay(stdscr, TRUE); // getch() blockiert nicht
}

// Beendet sauberes ncurses
void cleanupCursesApp(void) {
    standend();
    refresh();
    curs_set(1);
    endwin();
}

// ============================================================================
//  FARBEN
// ============================================================================

void initializeColors(void) {
    start_color();

    //                Vordergrund , Hintergrund
    init_pair(COLP_USER_WORM, COLOR_GREEN, COLOR_WHITE);
    init_pair(COLP_FREE_CELL, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLP_WORM_HEAD, COLOR_RED,   COLOR_WHITE);

    // gesamtes Fenster weiß einfärben
    wbkgd(stdscr, COLOR_PAIR(COLP_FREE_CELL));
    refresh();
}

// Zeichnet ein einzelnes Zeichen an Position (y,x)
void placeItem(int y, int x, chtype symbol, enum ColorPairs color_pair) {
    move(y, x);
    attron(COLOR_PAIR(color_pair));
    addch(symbol);
    attroff(COLOR_PAIR(color_pair));
}

// Letzte sichtbare Zeile / Spalte
int getLastRow(void) { return LINES - 1; }
int getLastCol(void) { return COLS  - 1; }

// ============================================================================
//  WURM-INITIALISIERUNG
// ============================================================================

enum ResCodes initializeWorm(int len_max, int y, int x,
                             enum WormHeading dir,
                             enum ColorPairs color)
{
    // Ringpuffer vorbereiten
    theworm_maxindex  = len_max - 1;
    theworm_headindex = 0;

    // Alle Positionen zurücksetzen
    for (int i = 0; i <= theworm_maxindex; i++) {
        theworm_wormpos_x[i] = UNUSED_POS_ELEM;
        theworm_wormpos_y[i] = UNUSED_POS_ELEM;
    }

    // Kopfstartposition setzen
    theworm_wormpos_x[0] = x;
    theworm_wormpos_y[0] = y;

    // Startrichtung einstellen
    setWormHeading(dir);

    // Farbe speichern
    theworm_wcolor = color;

    return RES_OK;
}

// ============================================================================
//  EINGABEVERARBEITUNG
// ============================================================================

void readUserInput(enum GameStates* agame_state) {
    int ch;

    if ((ch = getch()) > 0) {

        switch(ch) {

            // Spiel beenden
            case 'q': 
                *agame_state = WORM_GAME_QUIT;
                break;

            // Pfeiltasten
            case KEY_UP:    setWormHeading(WORM_UP);    break;
            case KEY_DOWN:  setWormHeading(WORM_DOWN);  break;
            case KEY_LEFT:  setWormHeading(WORM_LEFT);  break;
            case KEY_RIGHT: setWormHeading(WORM_RIGHT); break;

            // diagonale Bewegung
            case '1': setWormHeading(WORM_UP_LEFT);    break;
            case '2': setWormHeading(WORM_UP_RIGHT);   break;
            case '3': setWormHeading(WORM_DOWN_RIGHT); break;
            case '4': setWormHeading(WORM_DOWN_LEFT);  break;

            // Pause ein / aus
            case 's': nodelay(stdscr, FALSE); break;
            case ' ': nodelay(stdscr, TRUE);  break;
        }
    }
}

// ============================================================================
//  BEWEGUNGSLOGIK
// ============================================================================

// Setzt dx/dy entsprechend der Richtung
void setWormHeading(enum WormHeading dir) {
    switch(dir) {
        case WORM_UP:          theworm_dx =  0; theworm_dy = -1; break;
        case WORM_DOWN:        theworm_dx =  0; theworm_dy =  1; break;
        case WORM_LEFT:        theworm_dx = -1; theworm_dy =  0; break;
        case WORM_RIGHT:       theworm_dx =  1; theworm_dy =  0; break;

        case WORM_UP_LEFT:     theworm_dx = -1; theworm_dy = -1; break;
        case WORM_UP_RIGHT:    theworm_dx =  1; theworm_dy = -1; break;
        case WORM_DOWN_RIGHT:  theworm_dx =  1; theworm_dy =  1; break;
        case WORM_DOWN_LEFT:   theworm_dx = -1; theworm_dy =  1; break;
    }
}

// ============================================================================
//  ZEICHNEN DES WURMS
// ============================================================================

void showWorm(void) {

    // Schwanz im Ringpuffer finden
    int tailindex = theworm_headindex;
    int prev = (theworm_headindex == 0)
               ? theworm_maxindex
               : theworm_headindex - 1;

    while (theworm_wormpos_x[prev] != UNUSED_POS_ELEM &&
           prev != theworm_headindex) {

        tailindex = prev;
        prev = (prev == 0) ? theworm_maxindex : prev - 1;
    }

    // Alle Segmente ausgeben
    int idx = theworm_headindex;

    while (1) {

        int y = theworm_wormpos_y[idx];
        int x = theworm_wormpos_x[idx];

        if (y != UNUSED_POS_ELEM) {

            chtype sym;
            enum ColorPairs col;

            if (idx == theworm_headindex) {
                sym = SYMBOL_WORM_HEAD;
                col = COLP_WORM_HEAD;
            }
            else if (idx == tailindex) {
                sym = SYMBOL_WORM_TAIL;
                col = theworm_wcolor;
            }
            else {
                sym = SYMBOL_WORM_INNER_ELEMENT;
                col = theworm_wcolor;
            }

            placeItem(y, x, sym, col);
        }

        if (idx == tailindex)
            break;

        idx = (idx == 0) ? theworm_maxindex : idx - 1;
    }
}

// Löscht das ehemalige Schwanzende (setzt leeres Feld)
void cleanWormTail(void) {

    int tailindex =
        (theworm_headindex + 1) % (theworm_maxindex + 1);

    if (theworm_wormpos_y[tailindex] != UNUSED_POS_ELEM) {

        placeItem(
            theworm_wormpos_y[tailindex],
            theworm_wormpos_x[tailindex],
            SYMBOL_FREE_CELL,
            COLP_FREE_CELL
        );
    }
}

// ============================================================================
//  BEWEGUNG UND KOLLISION
// ============================================================================

// Prüft, ob ein Feld vom Wurm belegt ist
bool isInUseByWorm(int y, int x) {

    int i = theworm_headindex;

    do {
        i = (i + 1) % (theworm_maxindex + 1);

        if (theworm_wormpos_y[i] == y &&
            theworm_wormpos_x[i] == x)
            return true;

    } while (i != theworm_headindex &&
             theworm_wormpos_x[i] != UNUSED_POS_ELEM);

    return false;
}

// Bewegt den Wurm 1 Schritt
void moveWorm(enum GameStates* agame_state) {

    int newx =
        theworm_wormpos_x[theworm_headindex] + theworm_dx;
    int newy =
        theworm_wormpos_y[theworm_headindex] + theworm_dy;

    // Randkollision
    if (newx < 0 || newx > getLastCol()
     || newy < 0 || newy > getLastRow()) {

        *agame_state = WORM_OUT_OF_BOUNDS;
        return;
    }

    // Selbstkollision
    if (isInUseByWorm(newy, newx)) {

        *agame_state = WORM_CROSSING;
        return;
    }

    // Kopf vorwärts bewegen
    theworm_headindex++;
    theworm_headindex %= (theworm_maxindex + 1);

    theworm_wormpos_x[theworm_headindex] = newx;
    theworm_wormpos_y[theworm_headindex] = newy;
}

// ============================================================================
//  GAME LOOP
// ============================================================================

enum ResCodes doLevel(void) {

    enum GameStates game_state = WORM_GAME_ONGOING;

    // Initialer Wurm links unten, nach rechts
    initializeWorm(WORM_LENGTH, getLastRow(), 0,
                    WORM_RIGHT, COLP_USER_WORM);

    showWorm();
    refresh();

    while (game_state == WORM_GAME_ONGOING) {

        readUserInput(&game_state);
        cleanWormTail();
        moveWorm(&game_state);

        if (game_state != WORM_GAME_ONGOING)
            break;

        showWorm();
        napms(NAP_TIME);
        refresh();
    }

    showGameOverMessage(game_state);
    return RES_OK;
}

// ============================================================================
//  GAME-OVER AUSGABE
// ============================================================================

void showGameOverMessage(enum GameStates agame_state) {

    cleanupCursesApp();

    printf("\nGAME OVER\n");

    switch (agame_state) {

        case WORM_OUT_OF_BOUNDS:
            printf("Der Wurm hat das Spielfeld verlassen.\n");
            break;

        case WORM_CROSSING:
            printf("Der Wurm ist mit sich selbst kollidiert.\n");
            break;

        case WORM_GAME_QUIT:
            printf("Das Spiel wurde durch den Benutzer beendet.\n");
            break;

        default:
            printf("Spiel beendet.\n");
            break;
    }

    printf("TRY AGAIN\n");
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
        printf("\nDas Fenster ist zu klein! Mindestgröße: %dx%d\n",
               MIN_NUMBER_OF_COLS, MIN_NUMBER_OF_ROWS);
        return RES_FAILED;
    }

    doLevel();

    return RES_OK;
}

