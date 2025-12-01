// ============================================================================
//  Modul: worm_model.c - Fabian Ausmann
//
//  Dieses Modul kapselt die komplette Datenstruktur und Bewegungslogik
//  des Wurms. Andere Teile des Programms greifen nur über die hier
//  definierten Funktionen auf den Wurm zu und kennen die internen
//  Implementierungsdetails (Ringpuffer, Arrays etc.) nicht.
//
//  Aufgaben dieses Moduls:
//   - Verwaltung der Wurm-Positionen im Ringpuffer
//   - Initialisierung von Startposition, Richtung und Farbe
//   - Aktualisierung der Bewegungsrichtung (Heading)
//   - Zeichnen des Wurms auf dem Spielfeld
//   - Löschen des alten Schwanzsegments
//   - Erkennung von Randkollision und Selbstkollision
//   - Fortschreiben der Position (ein Schritt pro Tick)
// ============================================================================

#include <curses.h>        // Für den Typ chtype und Bildschirmfunktionen
#include "worm.h"          // Gemeinsame Konstanten und Aufzählungstypen
#include "board_model.h"   // placeItem(), getLastRow(), getLastCol()
#include "worm_model.h"    // Öffentliche Schnittstelle des Wurmmodells

// ============================================================================
//  GLOBALE VARIABLEN – WURM (gekapselt in diesem Modul)
// ============================================================================
//
//  Diese Variablen sind nur in diesem Modul sichtbar (sofern nicht als
//  extern deklariert) und bilden die interne Datenstruktur des Wurms.
//  Der Wurm wird als Ringpuffer aus x- und y-Koordinaten modelliert.
// ============================================================================

// Maximale gültige Index-Position im Ringpuffer (len_max - 1)
int theworm_maxindex;

// Index des Wurm-Kopfes im Ringpuffer
int theworm_headindex;

// Positionsarrays des Wurms:
//  - theworm_wormpos_y: Zeilenkoordinaten
//  - theworm_wormpos_x: Spaltenkoordinaten
int theworm_wormpos_y[WORM_LENGTH];
int theworm_wormpos_x[WORM_LENGTH];

// Bewegungsdelta pro Tick:
//  - theworm_dx: horizontale Bewegung (-1, 0, 1)
//  - theworm_dy: vertikale Bewegung (-1, 0, 1)
int theworm_dx;
int theworm_dy;

// Farbe des Wurms (Farbpaar, z. B. für Körper, Schwanz)
enum ColorPairs theworm_wcolor;

// ============================================================================
//  WURM-INITIALISIERUNG
// ============================================================================
//
//  Initialisiert den Wurm für den Spielstart.
//
//  Parameter:
//   - len_max : maximale Anzahl der Segmente (Ringpuffergröße)
//   - y, x    : Startposition des Wurm-Kopfes
//   - dir     : Start-Bewegungsrichtung des Wurms
//   - color   : zu verwendende Farbe (ColorPairs)
//
//  Ablauf:
//   1. Ringpuffer-Grenzen setzen (maximaler Index)
//   2. Kopfindex auf 0 setzen
//   3. Alle Positionen im Puffer als "unbenutzt" markieren
//   4. Startposition des Kopfes eintragen
//   5. Start-Richtung setzen (dx/dy)
//   6. Wurmfarbe speichern
// ============================================================================

enum ResCodes initializeWorm(int len_max, int y, int x,
                             enum WormHeading dir,
                             enum ColorPairs color)
{
    // Ringpuffer vorbereiten
    theworm_maxindex  = len_max - 1;
    theworm_headindex = 0;

    // Alle Positionen im Ringpuffer auf "unbenutzt" setzen
    for (int i = 0; i <= theworm_maxindex; i++) {
        theworm_wormpos_x[i] = UNUSED_POS_ELEM;
        theworm_wormpos_y[i] = UNUSED_POS_ELEM;
    }

    // Kopfstartposition setzen
    theworm_wormpos_x[0] = x;
    theworm_wormpos_y[0] = y;

    // Startrichtung einstellen (setzt dx/dy)
    setWormHeading(dir);

    // Wurmfarbe speichern
    theworm_wcolor = color;

    return RES_OK;
}

// ============================================================================
//  BEWEGUNGSLOGIK – Richtung
// ============================================================================
//
//  Setzt die Bewegungsrichtung des Wurms.
//  Aus der logischen Richtung (enum WormHeading) werden die
//  konkreten Bewegungsdeltas dx und dy berechnet.
//
//  Beispiel:
//   - WORM_RIGHT  -> dx =  1, dy =  0
//   - WORM_UP     -> dx =  0, dy = -1
//   - WORM_UP_LEFT -> dx = -1, dy = -1
// ============================================================================

void setWormHeading(enum WormHeading dir) {
    switch(dir) {
        case WORM_UP:          theworm_dx =  0; theworm_dy = -1; break;
        case WORM_DOWN:        theworm_dx =  0; theworm_dy =  1; break;
        case WORM_LEFT:        theworm_dx = -1; theworm_dy =  0; break;
        case WORM_RIGHT:       theworm_dx =  1; theworm_dy =  0; break;

        // Diagonale Bewegungsrichtungen
        case WORM_UP_LEFT:     theworm_dx = -1; theworm_dy = -1; break;
        case WORM_UP_RIGHT:    theworm_dx =  1; theworm_dy = -1; break;
        case WORM_DOWN_RIGHT:  theworm_dx =  1; theworm_dy =  1; break;
        case WORM_DOWN_LEFT:   theworm_dx = -1; theworm_dy =  1; break;
    }
}

// ============================================================================
//  ZEICHNEN DES WURMS
// ============================================================================
//
//  Zeichnet den gesamten Wurm (Kopf, Körper, Schwanz) auf das Spielfeld.
//
//  Vorgehen:
//   1. Schwanzindex im Ringpuffer ermitteln
//   2. Vom Kopf rückwärts durch den Ringpuffer iterieren, bis der Schwanz
//      erreicht ist
//   3. Für jedes Segment:
//        - Zeichen und Farbe abhängig von Kopf/Körper/Schwanz auswählen
//        - über placeItem() auf dem Spielfeld ausgeben
// ============================================================================

void showWorm(void) {

    // Zunächst den Index des Schwanzsegments im Ringpuffer bestimmen
    int tailindex = theworm_headindex;
    int prev = (theworm_headindex == 0)
               ? theworm_maxindex
               : theworm_headindex - 1;

    // Rückwärts laufen, bis ein "unbenutztes" Element oder der Kopf erreicht ist
    while (theworm_wormpos_x[prev] != UNUSED_POS_ELEM &&
           prev != theworm_headindex) {

        tailindex = prev;
        prev = (prev == 0) ? theworm_maxindex : prev - 1;
    }

    // Alle Segmente vom Kopf bis zum Schwanz ausgeben
    int idx = theworm_headindex;

    while (1) {

        int y = theworm_wormpos_y[idx];
        int x = theworm_wormpos_x[idx];

        // Nur gültige Einträge zeichnen
        if (y != UNUSED_POS_ELEM) {

            chtype sym;
            enum ColorPairs col;

            // Kopf: eigenes Symbol und eigene Farbe
            if (idx == theworm_headindex) {
                sym = SYMBOL_WORM_HEAD;
                col = COLP_WORM_HEAD;
            }
            // Schwanz: eigenes Symbol, gleiche Farbe wie Körper
            else if (idx == tailindex) {
                sym = SYMBOL_WORM_TAIL;
                col = theworm_wcolor;
            }
            // inneres Segment
            else {
                sym = SYMBOL_WORM_INNER_ELEMENT;
                col = theworm_wcolor;
            }

            // Segment an seiner Position ausgeben
            placeItem(y, x, sym, col);
        }

        // Schleife abbrechen, wenn das Schwanzsegment erreicht wurde
        if (idx == tailindex)
            break;

        // Ringpuffer rückwärts durchlaufen
        idx = (idx == 0) ? theworm_maxindex : idx - 1;
    }
}

// ============================================================================
//  Löscht das ehemalige Schwanzende (setzt freies Feld an diese Stelle)
//
//  Idee:
//   Der Wurm wird als Ringpuffer gespeichert. Wenn sich der Kopf weiter
//   bewegt, wird der Eintrag nach dem Kopfindex zum neuen Schwanz.
//   Diese Position wird wieder als "frei" gezeichnet.
// ============================================================================

void cleanWormTail(void) {

    // Index des Schwanzsegments: Element hinter dem Kopf im Ring
    int tailindex =
        (theworm_headindex + 1) % (theworm_maxindex + 1);

    // Nur löschen, wenn an dieser Stelle tatsächlich ein Segment existiert
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
//
//  In diesem Abschnitt stehen die Funktionen zur Kollisionsprüfung und zur
//  eigentlichen Bewegung des Wurms.
// ============================================================================

// ============================================================================
//  Prüft, ob ein Spielfeldfeld bereits vom Wurm belegt ist
//
//  Parameter:
//   - y, x : zu überprüfende Position
//
//  Rückgabewert:
//   - true  : wenn ein Segment des Wurms an (y,x) liegt
//   - false : wenn das Feld frei ist
//
//  Vorgehen:
//   - Vom Kopf aus durch den Ringpuffer laufen, bis wieder der Kopf erreicht
//     ist oder ein unbenutztes Element gefunden wird.
// ============================================================================

bool isInUseByWorm(int y, int x) {

    int i = theworm_headindex;

    do {
        // im Ring eins weiter gehen
        i = (i + 1) % (theworm_maxindex + 1);

        // Position vergleichen
        if (theworm_wormpos_y[i] == y &&
            theworm_wormpos_x[i] == x)
            return true;

    } while (i != theworm_headindex &&
             theworm_wormpos_x[i] != UNUSED_POS_ELEM);

    return false;
}

// ============================================================================
//  Bewegt den Wurm um einen Schritt in die aktuelle Richtung
//
//  Parameter:
//   - agame_state : Zeiger auf den aktuellen Spielzustand. Dieser wird
//                   modifiziert, wenn eine Kollision erkannt wird.
//
//  Ablauf:
//   1. Neue Kopfposition (newx, newy) aus alter Position + dx/dy berechnen
//   2. Randkollision prüfen:
//        - Falls außerhalb des Spielfelds → WORM_OUT_OF_BOUNDS
//   3. Selbstkollision prüfen:
//        - Falls neue Position bereits vom Wurm belegt → WORM_CROSSING
//   4. Wenn alles frei:
//        - Kopfindex um 1 weiterschieben (Ringpuffer)
//        - neue Kopfposition in die Positionsarrays eintragen
// ============================================================================

void moveWorm(enum GameStates* agame_state) {

    // neue Kopfposition aus aktueller Position plus Bewegungsdelta berechnen
    int newx =
        theworm_wormpos_x[theworm_headindex] + theworm_dx;
    int newy =
        theworm_wormpos_y[theworm_headindex] + theworm_dy;

    // Randkollision prüfen: Bildschirmgrenzen überschritten?
    if (newx < 0 || newx > getLastCol()
     || newy < 0 || newy > getLastRow()) {

        *agame_state = WORM_OUT_OF_BOUNDS;
        return;
    }

    // Selbstkollision prüfen: liegt bereits ein Wurmsegment an der neuen Position?
    if (isInUseByWorm(newy, newx)) {

        *agame_state = WORM_CROSSING;
        return;
    }

    // Wenn keine Kollision vorliegt: Kopf weiterbewegen

    // Kopfindex im Ringpuffer weiterschieben
    theworm_headindex++;
    theworm_headindex %= (theworm_maxindex + 1);

    // neue Kopfposition in den Ringpuffer eintragen
    theworm_wormpos_x[theworm_headindex] = newx;
    theworm_wormpos_y[theworm_headindex] = newy;
}


