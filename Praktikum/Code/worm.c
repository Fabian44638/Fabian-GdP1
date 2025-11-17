// Blatt 4 mit Erweiterungen für den 18.11.2025
// Dieses Programm steuert einen einfachen Wurm, der sich im Terminal bewegt.
// Erweiterungen:
// - diagonale Steuerung (Tasten 1–4)
// - farbiger Kopf (rot) und grüner Körper
// - Prüfung der Mindestfenstergröße vor Start des Spiels

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

// ********************************************************************************************
// Konstante Werte und Strukturen
// ********************************************************************************************

// Rückgabecodes für Funktionen (Erfolg / Fehler)
enum ResCodes {
    RES_OK,
    RES_FAILED,
};

// Wartezeit zwischen Bewegungen des Wurms in Millisekunden
#define NAP_TIME    100

// Mindestgröße des Terminalfensters
// Wenn diese unterschritten wird, darf das Spiel nicht starten
#define MIN_NUMBER_OF_ROWS  3
#define MIN_NUMBER_OF_COLS 10

// Farbpaar-Definitionen
// Diese werden später in initializeColors() mit curses initialisiert
enum ColorPairs {
    COLP_USER_WORM = 1,   // Körper: grün
    COLP_WORM_HEAD = 2    // Kopf: rot
};

// Zeichen, das für jedes Wurmelement verwendet wird
#define SYMBOL_WORM_INNER_ELEMENT '0'

// Spielzustände zur Kontrolle der Hauptschleife
enum GameStates {
    WORM_GAME_ONGOING,
    WORM_OUT_OF_BOUNDS,
    WORM_GAME_QUIT,
};

// Bewegungsrichtungen des Wurms
// Erweiterung: diagonale Bewegungen hinzugefügt
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

// ********************************************************************************************
// Global variables
// ********************************************************************************************

// aktuelle Kopfposition
int theworm_headpos_y;
int theworm_headpos_x;

// alte Kopfposition → wird automatisch zum nächsten Körpersegment
int old_headpos_y;
int old_headpos_x;

// Bewegung pro Tick in X- und Y-Richtung
int theworm_dx;
int theworm_dy;

// Farbe des Körpers (der Kopf hat eine eigene Farbe)
enum ColorPairs theworm_wcolor;

// ********************************************************************************************
// Forward declarations
// ********************************************************************************************

void initializeColors(void);
void readUserInput(enum GameStates* agame_state);
enum ResCodes doLevel(void);

void initializeCursesApplication(void);
void cleanupCursesApp(void);

void placeItem(int y, int x, chtype symbol, enum ColorPairs color_pair);
int getLastRow(void);
int getLastCol(void);

enum ResCodes initializeWorm(int headpos_y, int headpos_x,
                             enum WormHeading dir, enum ColorPairs color);

void showWorm(void);
void moveWorm(enum GameStates* agame_state);
void setWormHeading(enum WormHeading dir);

// ********************************************************************************************
// Functions
// ********************************************************************************************

void initializeColors(void) {
    start_color();

    init_pair(COLP_USER_WORM, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLP_WORM_HEAD, COLOR_RED,   COLOR_BLACK);
}

void readUserInput(enum GameStates* agame_state) {
    int ch;

    if ((ch = getch()) > 0) {
        switch(ch) {
            case 'q':
                *agame_state = WORM_GAME_QUIT;
                break;
            // normale Bewegungsrichtungen
            case KEY_UP:    setWormHeading(WORM_UP); break;
            case KEY_DOWN:  setWormHeading(WORM_DOWN); break;
            case KEY_LEFT:  setWormHeading(WORM_LEFT); break;
            case KEY_RIGHT: setWormHeading(WORM_RIGHT); break;

            // diagonale Erweiterung
            case '1': setWormHeading(WORM_UP_LEFT); break;
            case '2': setWormHeading(WORM_UP_RIGHT); break;
            case '3': setWormHeading(WORM_DOWN_RIGHT); break;
            case '4': setWormHeading(WORM_DOWN_LEFT); break;

            case 's': nodelay(stdscr, FALSE); break;
            case ' ': nodelay(stdscr, TRUE);  break;
        }
    }
}

enum ResCodes doLevel(void) {
    enum GameStates game_state = WORM_GAME_ONGOING;
    enum ResCodes res_code;
    bool end_level_loop = false;

    // Startposition: untere linke Ecke
    int bottomLeft_y = getLastRow();
    int bottomLeft_x = 0;

    res_code = initializeWorm(bottomLeft_y, bottomLeft_x,
                              WORM_RIGHT, COLP_USER_WORM);

    if (res_code != RES_OK)
        return res_code;

    showWorm();
    refresh();

    while (!end_level_loop) {

        readUserInput(&game_state);

        if (game_state == WORM_GAME_QUIT) {
            end_level_loop = true;
            continue;
        }

        moveWorm(&game_state);

        if (game_state != WORM_GAME_ONGOING) {
            end_level_loop = true;
            continue;
        }

        showWorm();
        napms(NAP_TIME);
        refresh();
    }

    return RES_OK;
}

void initializeCursesApplication(void) {
    initscr();
    noecho();
    cbreak();
    nonl();
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);
}

// ********************************************************************************************
// Funktion: curses sauber beenden
// ********************************************************************************************

void cleanupCursesApp(void) {
    standend();
    refresh();
    curs_set(1);
    endwin();
}

// ********************************************************************************************
// Hilfsfunktionen zur Spielfeldgrenze
// ********************************************************************************************

void placeItem(int y, int x, chtype symbol, enum ColorPairs color_pair) {
    move(y, x);
    attron(COLOR_PAIR(color_pair));
    addch(symbol);
    attroff(COLOR_PAIR(color_pair));
}

int getLastRow(void) {
    return LINES - 1;
}

int getLastCol(void) {
    return COLS - 1;
}

// ********************************************************************************************
// FEHLENDE FUNKTION: placeItem()
// Diese Funktion wurde im Originalcode entfernt und erzeugte den Fehler.
// ********************************************************************************************

// ********************************************************************************************
// Funktion: Wurm initialisieren
// ********************************************************************************************

enum ResCodes initializeWorm(int headpos_y, int headpos_x,
                             enum WormHeading dir, enum ColorPairs color)
{
    theworm_headpos_y = headpos_y;
    theworm_headpos_x = headpos_x;

    old_headpos_y = headpos_y;
    old_headpos_x = headpos_x;

    setWormHeading(dir);
    theworm_wcolor = color;

    return RES_OK;
}

void showWorm(void) {
    // Körper zeichnen
    placeItem(old_headpos_y, old_headpos_x,
              SYMBOL_WORM_INNER_ELEMENT,
              COLP_USER_WORM);

    // Kopf rot
    placeItem(theworm_headpos_y, theworm_headpos_x,
              SYMBOL_WORM_INNER_ELEMENT,
              COLP_WORM_HEAD);
}

void moveWorm(enum GameStates* agame_state) {

    old_headpos_y = theworm_headpos_y;
    old_headpos_x = theworm_headpos_x;

    theworm_headpos_y += theworm_dy;
    theworm_headpos_x += theworm_dx;

    if (theworm_headpos_x < 0 ||
        theworm_headpos_x > getLastCol() ||
        theworm_headpos_y < 0 ||
        theworm_headpos_y > getLastRow()) {

        *agame_state = WORM_OUT_OF_BOUNDS;
    }
}

void setWormHeading(enum WormHeading dir) {
    switch(dir) {
        case WORM_UP:         theworm_dx = 0;  theworm_dy = -1; break;
        case WORM_DOWN:       theworm_dx = 0;  theworm_dy =  1; break;
        case WORM_LEFT:       theworm_dx = -1; theworm_dy =  0; break;
        case WORM_RIGHT:      theworm_dx =  1; theworm_dy =  0; break;

        case WORM_UP_LEFT:    theworm_dx = -1; theworm_dy = -1; break;
        case WORM_UP_RIGHT:   theworm_dx =  1; theworm_dy = -1; break;
        case WORM_DOWN_RIGHT: theworm_dx =  1; theworm_dy =  1; break;
        case WORM_DOWN_LEFT:  theworm_dx = -1; theworm_dy =  1; break;
    }
}

// ********************************************************************************************
// MAIN
// ********************************************************************************************

int main(void) {
    enum ResCodes res_code;

    initializeCursesApplication();
    initializeColors();

    // *** FEHLER BEHOBEN: cleanupCursesApp DARF NICHT vor printf() sein ***
    if (LINES < MIN_NUMBER_OF_ROWS || COLS < MIN_NUMBER_OF_COLS) {

        endwin();   // << NUR curses ausschalten — nicht das ganze Terminal resetten

        printf("Das Fenster ist zu klein: mindestens %d x %d nötig.\n",
               MIN_NUMBER_OF_COLS, MIN_NUMBER_OF_ROWS);

        return RES_FAILED;
    }

    res_code = doLevel();

    cleanupCursesApp();
    return res_code;
}


