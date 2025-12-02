// ============================================================================
// Fabian Ausmann - Aufgabenblatt 5
//
// Version mit:
//  - diagonaler Steuerung (Tasten "1" – "4")
//  - Ringpuffer-Wurm
//  - Selbstkollision
//  - Game-Over-Ausgabe
//  - Hintergrundfarbe
// ============================================================================

#include <curses.h>   // ncurses: Bildschirmsteuerung, getch(), Farben, etc.
#include <stdio.h>    // printf() für Textausgabe nach Spielende
#include <stdlib.h>   // allgemeine Hilfsfunktionen (z. B. exit)
#include <stdbool.h>  // bool-Typ (true/false)
#include <unistd.h>   // napms() / usleep() (Zeitverzögerungen)

// ============================================================================
//  KONFIGURATION DES SPIELS
// ============================================================================

// Pausenzeit zwischen zwei Frame-Aktualisierungen (in Millisekunden)
#define NAP_TIME 100

// Mindestgröße des Spielfensters (Terminal)
#define MIN_NUMBER_OF_ROWS 15
#define MIN_NUMBER_OF_COLS 20

// Maximale Länge des Wurms (Größe des Ringpuffers)
#define WORM_LENGTH 8

// Markerwert für unbenutzte Elemente im Positionsarray
#define UNUSED_POS_ELEM -1

// Zeichen für die Darstellung auf dem Spielfeld
#define SYMBOL_FREE_CELL           ' '   // freies Feld / Hintergrund
#define SYMBOL_WORM_INNER_ELEMENT  '0'   // Wurmkörper
#define SYMBOL_WORM_HEAD           'X'   // Wurm-Kopf
#define SYMBOL_WORM_TAIL           'X'   // Wurm-Schwanz

// ============================================================================
//  ENUMS (Aufzählungstypen)
// ============================================================================

// Rückgabewerte von Funktionen (vereinfachte Fehlerbehandlung)
enum ResCodes {
    RES_OK,
    RES_FAILED
};

// Farbpaare (Indices für COLOR_PAIR())
enum ColorPairs {
    COLP_USER_WORM = 1,  // Körper und Schwanz
    COLP_FREE_CELL,      // Hintergrundfarbe (freies Feld)
    COLP_WORM_HEAD       // Kopf des Wurms
};

// Mögliche Spielzustände
enum GameStates {
    WORM_GAME_ONGOING,   // Spiel läuft
    WORM_OUT_OF_BOUNDS,  // Wurm hat Spielfeld verlassen
    WORM_CROSSING,       // Wurm ist mit sich selbst kollidiert
    WORM_GAME_QUIT       // Benutzer hat das Spiel beendet
};

// Bewegungsrichtungen des Wurms (inkl. Diagonalen)
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
//
//  Diese Variablen repräsentieren den Zustand des Wurms und werden von
//  mehreren Funktionen gemeinsam benutzt.
//
//  Hinweis: In späteren Aufgaben (Blatt 6/Modularisierung) werden solche
//  globalen Variablen in ein eigenes Modul ausgelagert (worm_model).
// ============================================================================

// Ringpuffer-Indizes: maxindex = letzte gültige Position, headindex = Kopf
int theworm_maxindex;
int theworm_headindex;

// Arrays mit den y- und x-Koordinaten aller Wurmsegmente
int theworm_wormpos_y[WORM_LENGTH];
int theworm_wormpos_x[WORM_LENGTH];

// Bewegungsrichtung (Delta pro Schritt)
int theworm_dx;   // Änderung in x-Richtung
int theworm_dy;   // Änderung in y-Richtung

// Farbe des Wurms
enum ColorPairs theworm_wcolor;

// Pausenstatus des Spiels:
//   false → Wurm bewegt sich kontinuierlich
//   true  → Wurm ist pausiert, nur Einzelschritte über Richtungs-Tasten
bool paused = false;

// ============================================================================
//  FUNKTIONS-PROTOTYPEN
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

// WICHTIG: readUserInput liefert jetzt ein bool zurück
//   -> true  = Einzelschritt im Pausenmodus ausführen
//   -> false = kein Schritt (z. B. andere Taste oder keine Eingabe)
bool readUserInput(enum GameStates* agame_state);

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

// Startet ncurses und stellt sinnvolle Grundeinstellungen ein
void initializeCursesApplication(void) {
    initscr();              // ncurses-Bildschirm initialisieren
    noecho();               // Tastatureingaben nicht direkt anzeigen
    cbreak();               // Eingaben sofort an das Programm weitergeben
    nonl();                 // Enter-Taste nicht als Newline in Ausgabe behandeln
    keypad(stdscr, TRUE);   // Pfeiltasten und Funktionstasten erlauben
    curs_set(0);            // Cursor unsichtbar machen
    nodelay(stdscr, TRUE);  // getch() blockiert nicht (für "Echtzeit"-Steuerung)
}

// Beendet ncurses sauber und stellt das Terminal wieder her
void cleanupCursesApp(void) {
    standend();   // Alle Textattribute zurücksetzen
    refresh();
    curs_set(1);  // Cursor wieder sichtbar machen
    endwin();     // ncurses-Modus beenden
}

// ============================================================================
//  FARBEN
// ============================================================================

// Konfiguration der Farbpaare und Setzen des Hintergrunds
void initializeColors(void) {
    start_color();   // Farbmodus aktivieren

    //                  Vordergrund        Hintergrund
    init_pair(COLP_USER_WORM, COLOR_GREEN, COLOR_WHITE);
    init_pair(COLP_FREE_CELL, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLP_WORM_HEAD, COLOR_RED,   COLOR_WHITE);

    // gesamten Bildschirm mit Hintergrundfarbe für freie Zellen füllen
    wbkgd(stdscr, COLOR_PAIR(COLP_FREE_CELL));
    refresh();
}

// Zeichnet ein Zeichen an Position (y, x) mit dem angegebenen Farbpair
void placeItem(int y, int x, chtype symbol, enum ColorPairs color_pair) {
    move(y, x);                    // Cursor an Position setzen
    attron(COLOR_PAIR(color_pair));
    addch(symbol);                 // Zeichen ausgeben
    attroff(COLOR_PAIR(color_pair));
}

// Letzte sichtbare Zeile / Spalte im aktuellen Terminal
int getLastRow(void) { return LINES - 1; }
int getLastCol(void) { return COLS  - 1; }

// ============================================================================
//  WURM-INITIALISIERUNG
// ============================================================================

// Setzt den Wurm in seinen Startzustand (Ringpuffer, Position, Farbe, Richtung)
enum ResCodes initializeWorm(int len_max, int y, int x,
                             enum WormHeading dir,
                             enum ColorPairs color)
{
    // Ringpuffer vorbereiten
    theworm_maxindex  = len_max - 1;  // letzter gültiger Index
    theworm_headindex = 0;            // Kopf befindet sich zunächst bei Index 0

    // Alle Positionen im Ringpuffer auf "unbenutzt" setzen
    for (int i = 0; i <= theworm_maxindex; i++) {
        theworm_wormpos_x[i] = UNUSED_POS_ELEM;
        theworm_wormpos_y[i] = UNUSED_POS_ELEM;
    }

    // Kopfstartposition setzen
    theworm_wormpos_x[0] = x;
    theworm_wormpos_y[0] = y;

    // Startrichtung des Wurms einstellen
    setWormHeading(dir);

    // Farbe speichern
    theworm_wcolor = color;

    return RES_OK;
}

// ============================================================================
//  EINGABEVERARBEITUNG
// ============================================================================
//
//  Diese Funktion liest eine Taste ein (falls vorhanden) und:
//   - aktualisiert ggf. die Bewegungsrichtung des Wurms
//   - setzt den Spielzustand auf WORM_GAME_QUIT bei 'q'
//   - schaltet Pause/Weiter um
//
//  Rückgabewert:
//   - true  : es wurde eine Bewegungs-Taste (Pfeile / 1–4) gedrückt
//             -> im Pausenmodus soll EIN Schritt ausgeführt werden
//   - false : keine relevante Taste / andere Taste
//
bool readUserInput(enum GameStates* agame_state) {
    int ch;
    bool step_request = false;   // Einzelschritt angefordert?

    // getch() ist wegen nodelay() entweder:
    //   - negative Zahl, wenn keine Taste gedrückt wurde
    //   - oder der Code der gedrückten Taste (> 0)
    if ((ch = getch()) > 0) {

        switch(ch) {

            // Spiel beenden
            case 'q':
                *agame_state = WORM_GAME_QUIT;
                break;

            // Pfeiltasten → Richtungswechsel + Einzelschritt im Pausenmodus
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

            // diagonale Bewegung
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

            // Pause ein:
            //  - paused = true  → Wurm bewegt sich nur noch bei step_request
            //  - getch() blockiert jetzt (nodelay = FALSE)
            case 's':
                paused = true;
                nodelay(stdscr, FALSE);
                break;

            // Pause aus:
            //  - paused = false → Wurm bewegt sich wieder kontinuierlich
            //  - getch() nicht-blockierend (nodelay = TRUE)
            case ' ':
                paused = false;
                nodelay(stdscr, TRUE);
                break;

            // alle anderen Tasten:
            //  - keine Richtungsänderung
            //  - kein Einzelschritt
            default:
                break;
        }
    }

    return step_request;
}

// ============================================================================
//  BEWEGUNGSLOGIK
// ============================================================================

// Setzt dx/dy entsprechend der gewünschten Bewegungsrichtung
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

// Zeichnet den kompletten Wurm basierend auf dem Ringpuffer
void showWorm(void) {

    // Schwanz im Ringpuffer finden:
    // wir laufen rückwärts vom Kopf, bis ein UNUSED_POS_ELEM auftaucht
    int tailindex = theworm_headindex;
    int prev = (theworm_headindex == 0)
               ? theworm_maxindex
               : theworm_headindex - 1;

    while (theworm_wormpos_x[prev] != UNUSED_POS_ELEM &&
           prev != theworm_headindex) {

        tailindex = prev;
        prev = (prev == 0) ? theworm_maxindex : prev - 1;
    }

    // Alle Segmente zwischen Kopf und Schwanz zeichnen
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

// Löscht das ehemalige Schwanzende (setzt dort wieder ein freies Feld)
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

// Prüft, ob ein Feld bereits vom Wurm belegt ist (für Selbstkollision)
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

// Bewegt den Wurm um genau ein Segment weiter
void moveWorm(enum GameStates* agame_state) {

    int newx =
        theworm_wormpos_x[theworm_headindex] + theworm_dx;
    int newy =
        theworm_wormpos_y[theworm_headindex] + theworm_dy;

    // Randkollision: Wurm verlässt das Spielfeld
    if (newx < 0 || newx > getLastCol()
     || newy < 0 || newy > getLastRow()) {

        *agame_state = WORM_OUT_OF_BOUNDS;
        return;
    }

    // Selbstkollision: neuer Kopf berührt den eigenen Körper
    if (isInUseByWorm(newy, newx)) {

        *agame_state = WORM_CROSSING;
        return;
    }

    // Kopfposition im Ringpuffer weiterschieben
    theworm_headindex++;
    theworm_headindex %= (theworm_maxindex + 1);

    theworm_wormpos_x[theworm_headindex] = newx;
    theworm_wormpos_y[theworm_headindex] = newy;
}

// ============================================================================
//  GAME LOOP
// ============================================================================
//
//  Laufmodus (paused == false):
//    - Wurm bewegt sich in jedem Schleifendurchlauf (automatisch)
//
//  Pausenmodus (paused == true):
//    - Wurm bleibt stehen
//    - Nur wenn readUserInput() step_request == true zurückgibt,
//      wird EIN EINZELSCHRITT ausgeführt.
//
//  Dadurch wird verhindert, dass "beliebige Tasten" (z. B. 'l')
//  im Pausenmodus ebenfalls einen Schritt auslösen.
// ============================================================================

enum ResCodes doLevel(void) {

    enum GameStates game_state = WORM_GAME_ONGOING;
    paused = false;  // Spiel startet im Laufmodus

    // Initialer Wurm: unten links, Richtung rechts
    initializeWorm(WORM_LENGTH, getLastRow(), 0,
                    WORM_RIGHT, COLP_USER_WORM);

    showWorm();
    refresh();

    while (game_state == WORM_GAME_ONGOING) {

        // Benutzereingabe verarbeiten
        bool step_request = readUserInput(&game_state);

        // Spiel wurde ggf. durch 'q' beendet
        if (game_state != WORM_GAME_ONGOING)
            break;

        // Im Pausenmodus:
        // Nur bewegen, wenn eine Richtungs-Taste gedrückt wurde
        if (paused && !step_request) {
            napms(NAP_TIME);  // kleine Pause, um CPU zu schonen
            continue;         // keine Bewegung, nächste Eingabe abwarten
        }

        // Wurm bewegen (alte Schwanzposition löschen, Kopf vorwärts)
        cleanWormTail();
        moveWorm(&game_state);

        // Eventuelles Spielende durch Kollision prüfen
        if (game_state != WORM_GAME_ONGOING)
            break;

        // Wurm in neuer Position anzeigen
        showWorm();
        napms(NAP_TIME);
        refresh();
    }

    // Nach dem Verlassen der Schleife: Game-Over-Meldung ausgeben
    showGameOverMessage(game_state);
    return RES_OK;
}

// ============================================================================
//  GAME-OVER AUSGABE
// ============================================================================

// Gibt nach Ende des Spiels eine passende Meldung auf der Konsole aus
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
//  MAIN
// ============================================================================
//
//  Einstiegspunkt des Programms:
//   - ncurses initialisieren
//   - Farben setzen
//   - Fenstergröße prüfen
//   - Spiellogik starten
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

