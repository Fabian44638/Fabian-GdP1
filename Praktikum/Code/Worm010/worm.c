// Fabian Ausmann - Aufgabenblatt 4
// Dieses Programm steuert einen einfachen Wurm, der sich im Terminal bewegt.
// Erweiterungen für den 18.11.2025:
// - diagonale Steuerung über die Tasten 1 bis 4
// - roter Kopf und grüner Körper

#include <curses.h>   // curses-Bibliothek für Terminal-Grafik und Tastatureingaben
#include <stdio.h>    // Standard-Ein-/Ausgabe (printf usw.)
#include <stdlib.h>   // Standardfunktionen (z.B. EXIT-Codes, malloc, free)
#include <time.h>     // Zeitfunktionen (hier aktuell nicht genutzt, aber eingebunden)
#include <string.h>   // String-Funktionen (hier aktuell nicht genutzt)
#include <unistd.h>   // POSIX-Funktionen (sleep, usw.; hier nur wegen napms/Usages üblich)
#include <stdbool.h>  // bool, true, false

// ********************************************************************************************
// Konstante Werte und Strukturen
// ********************************************************************************************

// Rückgabecodes für Funktionen (Erfolg / Fehler)
// Wird z.B. von doLevel() oder initializeWorm() verwendet, um Erfolg oder Fehlschlag zu signalisieren.
enum ResCodes {
    RES_OK,
    RES_FAILED,
};

// Wartezeit zwischen Bewegungen des Wurms in Millisekunden
// Wird in doLevel() an napms() übergeben, bestimmt also die "Geschwindigkeit" des Wurms.
#define NAP_TIME    100

// Mindestgröße des Terminalfensters
// Wenn diese unterschritten wird, darf das Spiel nicht starten
// (Prüfung erfolgt in main() anhand von LINES und COLS von curses)
#define MIN_NUMBER_OF_ROWS 15
#define MIN_NUMBER_OF_COLS 20

// Farbpaar-Definitionen
// Diese werden später in initializeColors() mit curses initialisiert
// und dienen als "IDs", um bestimmte Vorder-/Hintergrundfarben zuzuweisen.
enum ColorPairs {
    COLP_USER_WORM = 1,   // Körper: grün
    COLP_WORM_HEAD = 2    // Kopf: rot
};

// Zeichen, das für jedes Wurmelement verwendet wird
// Sowohl Kopf als auch Körper bestehen aus diesem Zeichen, die Farbe unterscheidet sie.
#define SYMBOL_WORM_INNER_ELEMENT '0'

// Spielzustände zur Kontrolle der Hauptschleife
// Diese Werte steuern, ob das Level weiterläuft oder beendet wird.
enum GameStates {
    WORM_GAME_ONGOING,   // Spiel läuft normal weiter
    WORM_OUT_OF_BOUNDS,  // Wurm hat den Rand des Spielfeldes verlassen
    WORM_GAME_QUIT,      // Spieler hat aufgehört (Taste 'q')
};

// Bewegungsrichtungen des Wurms
// Erweiterung: diagonale Bewegungen hinzugefügt
// Diese Werte beschreiben nur die Richtung, die eigentliche Umsetzung in dx/dy
// erfolgt in setWormHeading().
enum WormHeading {
    WORM_UP,
    WORM_DOWN,
    WORM_LEFT,
    WORM_RIGHT,
    WORM_UP_LEFT,       // Erweiterung
    WORM_UP_RIGHT,      // Erweiterung
    WORM_DOWN_RIGHT,    // Erweiterung
    WORM_DOWN_LEFT      // Erweiterung
};

// ********************************************************************************************
// Global variables
// ********************************************************************************************

// aktuelle Kopfposition
// y = Zeile (oben nach unten), x = Spalte (links nach rechts)
int theworm_headpos_y;
int theworm_headpos_x;

// alte Kopfposition → wird automatisch zum nächsten Körpersegment
// showWorm() zeichnet hier den Körper, während der neue Kopf an der neuen Position gezeichnet wird
int old_headpos_y;
int old_headpos_x;

// Bewegung pro Tick in X- und Y-Richtung
// Diese Werte werden von setWormHeading() aus der Richtung berechnet.
// Beispiel: nach rechts → dx=+1, dy=0
int theworm_dx;
int theworm_dy;

// Farbe des Körpers (der Kopf hat eine eigene Farbe)
// Wird in initializeWorm() gesetzt und in showWorm() verwendet.
enum ColorPairs theworm_wcolor;

// ********************************************************************************************
// Forward declarations
// ********************************************************************************************
// Funktionsprototypen, damit der Compiler alle Funktionen kennt,
// bevor sie in anderen Funktionen aufgerufen werden.

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

// Initialisiert die Farbpaar-Einstellungen für curses.
// Muss nach initializeCursesApplication() aufgerufen werden.
void initializeColors(void) {
    start_color();  // Farbfähigkeit von curses aktivieren

    // Farbpaar für den Körper des Wurms:
    // Vordergrund: grün, Hintergrund: schwarz
    init_pair(COLP_USER_WORM, COLOR_GREEN, COLOR_BLACK);

    // Farbpaar für den Kopf des Wurms:
    // Vordergrund: rot, Hintergrund: schwarz
    init_pair(COLP_WORM_HEAD, COLOR_RED,   COLOR_BLACK);
}

// Liest die Benutzereingabe ein und setzt entsprechende Spielzustände oder Wurmrichtung.
// agame_state wird als Pointer übergeben, damit der Zustand außerhalb der Funktion
// verändert werden kann.
void readUserInput(enum GameStates* agame_state) {
    int ch;

    // getch() holt ein Zeichen von der Tastatur.
    // Durch nodelay(stdscr, TRUE) wird kein Warten erzwungen; bei keiner Eingabe kommt ERR zurück.
    if ((ch = getch()) > 0) {
        // Wenn ein gültiges Zeichen gelesen wurde, auswerten:
        switch(ch) {
            case 'q':
                // 'q' beendet das Spiel
                *agame_state = WORM_GAME_QUIT;
                break;

            // normale Bewegungsrichtungen über Pfeiltasten
            case KEY_UP:    setWormHeading(WORM_UP);    break;
            case KEY_DOWN:  setWormHeading(WORM_DOWN);  break;
            case KEY_LEFT:  setWormHeading(WORM_LEFT);  break;
            case KEY_RIGHT: setWormHeading(WORM_RIGHT); break;

            // diagonale Erweiterung
            // 1: hoch-links
            // 2: hoch-rechts
            // 3: runter-rechts
            // 4: runter-links
            case '1': setWormHeading(WORM_UP_LEFT);    break;
            case '2': setWormHeading(WORM_UP_RIGHT);   break;
            case '3': setWormHeading(WORM_DOWN_RIGHT); break;
            case '4': setWormHeading(WORM_DOWN_LEFT);  break;

            // 's' schaltet nodelay aus → getch() blockiert, Spiel "stoppt"
            case 's': nodelay(stdscr, FALSE); break;
            // Leertaste aktiviert nodelay → Timer-gesteuerte Bewegung ohne Blockieren
            case ' ': nodelay(stdscr, TRUE);  break;
        }
    }
}

// Führt ein komplettes Level aus (im Moment gibt es nur eines).
// Hier ist die eigentliche Spielschleife, die wiederholt Eingaben liest, den Wurm bewegt
// und ihn zeichnet.
enum ResCodes doLevel(void) {
    enum GameStates game_state = WORM_GAME_ONGOING;  // Startzustand
    enum ResCodes res_code;
    bool end_level_loop = false;                     // Flag zum Verlassen der while-Schleife

    // Startposition: untere linke Ecke
    // getLastRow() liefert die maximale Zeile (0-basiert), also untere Zeile.
    // Spalte 0 ist ganz links.
    int bottomLeft_y = getLastRow();
    int bottomLeft_x = 0;

    // Wurm an der Startposition initialisieren, Blickrichtung nach rechts.
    res_code = initializeWorm(bottomLeft_y, bottomLeft_x,
                              WORM_RIGHT, COLP_USER_WORM);

    if (res_code != RES_OK)
        return res_code;  // Falls Initialisierung fehlschlägt, Level mit Fehler abbrechen.

    // Anfangszustand des Wurms anzeigen
    showWorm();
    refresh();  // Bildschirm aktualisieren

    // Haupt-Spielschleife des Levels
    while (!end_level_loop) {

        // Tastatur abfragen und ggf. Spielzustand / Richtung anpassen
        readUserInput(&game_state);

        // Prüfen, ob der Spieler das Spiel beenden wollte
        if (game_state == WORM_GAME_QUIT) {
            end_level_loop = true;
            continue;
        }

        // Wurm eine "Einheit" weiterbewegen (abhängig von theworm_dx/theworm_dy)
        moveWorm(&game_state);

        // Wenn der Wurm den Rand verlassen hat, ist das Level vorbei
        if (game_state != WORM_GAME_ONGOING) {
            end_level_loop = true;
            continue;
        }

        // Wurm an neuer Position anzeigen (alter Kopf wird Körperstück, neuer Kopf rot)
        showWorm();

        // kurze Pause, um die Bewegung sichtbar zu machen (Geschwindigkeit)
        napms(NAP_TIME);

        // Bildschirmaufbau in das Terminal "schreiben"
        refresh();
    }

    // Level normal beendet
    return RES_OK;
}

// Initialisiert die curses-Umgebung.
// Muss vor allen curses-Funktionen aufgerufen werden.
void initializeCursesApplication(void) {
    initscr();            // curses starten, Bildschirm übernehmen
    noecho();             // Tastatureingaben nicht automatisch anzeigen
    cbreak();             // Eingaben sofort an das Programm weitergeben (kein Zeilenpuffer)
    nonl();               // Enter-Taste nicht in CR-LF umsetzen
    keypad(stdscr, TRUE); // Funktionstasten (z.B. Pfeiltasten) aktivieren
    curs_set(0);          // Cursor unsichtbar machen
    nodelay(stdscr, TRUE);// getch() blockiert nicht, wenn keine Eingabe anliegt
}

// ********************************************************************************************
// Funktion: curses sauber beenden
// ********************************************************************************************

// Stellt Terminaleinstellungen nach der Benutzung von curses wieder her.
void cleanupCursesApp(void) {
    standend();   // Alle Attribute (Farben etc.) zurücksetzen
    refresh();    // Anzeige noch einmal aktualisieren
    curs_set(1);  // Cursor wieder sichtbar machen
    endwin();     // curses-Session beenden
}

// ********************************************************************************************
// Hilfsfunktionen zur Spielfeldgrenze
// ********************************************************************************************

// Zeichnet ein einzelnes Symbol an die Position (y, x) mit einer bestimmten Farbe.
void placeItem(int y, int x, chtype symbol, enum ColorPairs color_pair) {
    move(y, x);                        // Cursor an Position setzen
    attron(COLOR_PAIR(color_pair));    // gewünschtes Farbpaar aktivieren
    addch(symbol);                     // Zeichen ausgeben
    attroff(COLOR_PAIR(color_pair));   // Farbpaar wieder deaktivieren
}

// Liefert die letzte angezeigte Zeile (maximaler Index) zurück.
// curses stellt die globale Variable LINES zur Verfügung (Anzahl der Zeilen).
int getLastRow(void) {
    return LINES - 1;
}

// Liefert die letzte angezeigte Spalte (maximaler Index) zurück.
// curses stellt die globale Variable COLS zur Verfügung (Anzahl der Spalten).
int getLastCol(void) {
    return COLS - 1;
}

// ********************************************************************************************
// FEHLENDE FUNKTION: placeItem()
// Diese Funktion wurde im Originalcode entfernt und erzeugte den Fehler.
// Hinweis: In dieser Version ist placeItem() wieder vorhanden (siehe oben).
// ********************************************************************************************

// ********************************************************************************************
// Funktion: Wurm initialisieren
// ********************************************************************************************

// Setzt die Anfangsposition und -richtung des Wurms.
// headpos_y / headpos_x: Startposition des Wurmkopfes
// dir: Start-Richtung (enum WormHeading)
// color: Farbpaar für den Körper
enum ResCodes initializeWorm(int headpos_y, int headpos_x,
                             enum WormHeading dir, enum ColorPairs color)
{
    // Aktuelle Kopfposition setzen
    theworm_headpos_y = headpos_y;
    theworm_headpos_x = headpos_x;

    // Alte Kopfposition initial auf aktuelle setzen
    // (zu Beginn gibt es praktisch nur ein Segment)
    old_headpos_y = headpos_y;
    old_headpos_x = headpos_x;

    // Richtung des Wurms setzen → berechnet dx/dy
    setWormHeading(dir);

    // Farbe des Körpers merken (Kopf bekommt später eigene Farbe)
    theworm_wcolor = color;

    return RES_OK;
}

// Zeichnet den Wurm auf dem Spielfeld.
// Im aktuellen, sehr einfachen Modell besteht der Wurm nur aus "Kopf"
// und dem vorherigen Kopf als Körpersegment.
void showWorm(void) {
    // Körper zeichnen (frühere Kopfposition) mit Körperfarbe (grün)
    placeItem(old_headpos_y, old_headpos_x,
              SYMBOL_WORM_INNER_ELEMENT,
              COLP_USER_WORM);

    // Kopf rot zeichnen an der aktuellen Kopfposition
    placeItem(theworm_headpos_y, theworm_headpos_x,
              SYMBOL_WORM_INNER_ELEMENT,
              COLP_WORM_HEAD);
}

// Bewegt den Wurm anhand von theworm_dx und theworm_dy um eine Position.
// Prüft danach, ob der Wurm noch innerhalb des Spielfelds ist.
void moveWorm(enum GameStates* agame_state) {

    // Alte Kopfposition sichern (wird beim Zeichnen zum Körperteil)
    old_headpos_y = theworm_headpos_y;
    old_headpos_x = theworm_headpos_x;

    // Neue Kopfposition berechnen
    // y nimmt dy hinzu (nach unten positiv), x nimmt dx hinzu (nach rechts positiv)
    theworm_headpos_y += theworm_dy;
    theworm_headpos_x += theworm_dx;

    // Prüfen, ob die neue Position noch im erlaubten Bereich liegt.
    if (theworm_headpos_x < 0 ||
        theworm_headpos_x > getLastCol() ||
        theworm_headpos_y < 0 ||
        theworm_headpos_y > getLastRow()) {

        // Falls nicht → Wurm ist "aus dem Spielfeld gefallen"
        *agame_state = WORM_OUT_OF_BOUNDS;
    }
}

// Setzt die Richtung des Wurms und berechnet daraus die Schrittweite dx/dy.
// dx = Änderung in x-Richtung; dy = Änderung in y-Richtung.
void setWormHeading(enum WormHeading dir) {
    switch(dir) {
        // nur vertikale Bewegung
        case WORM_UP:         theworm_dx = 0;  theworm_dy = -1; break;
        case WORM_DOWN:       theworm_dx = 0;  theworm_dy =  1; break;

        // nur horizontale Bewegung
        case WORM_LEFT:       theworm_dx = -1; theworm_dy =  0; break;
        case WORM_RIGHT:      theworm_dx =  1; theworm_dy =  0; break;

        // diagonale Bewegung: Kombination von horizontaler und vertikaler Änderung
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

    // curses initialisieren (Terminal übernehmen)
    initializeCursesApplication();
    // Farbsystem initialisieren (muss nach initscr()/initializeCursesApplication() kommen)
    initializeColors();

    // *** FEHLER BEHOBEN: cleanupCursesApp DARF NICHT vor printf() sein ***
    // Prüfen, ob das Terminal groß genug ist, um das Spiel sinnvoll darzustellen.
    if (LINES < MIN_NUMBER_OF_ROWS || COLS < MIN_NUMBER_OF_COLS) {

        // curses beenden, damit printf wieder "normal" auf dem Terminal funktioniert
        endwin();   // << NUR curses ausschalten — nicht das ganze Terminal resetten

        // Meldung auf normalen STDOUT ausgeben
        printf("Das Fenster ist zu klein: mindestens %d x %d nötig.\n",
               MIN_NUMBER_OF_COLS, MIN_NUMBER_OF_ROWS);

        // mit Fehlercode beenden
        return RES_FAILED;
    }

    // Level starten und Spielschleife ausführen
    res_code = doLevel();

    // curses sauber beenden und Terminal zurücksetzen
    cleanupCursesApp();
    return res_code;
}


