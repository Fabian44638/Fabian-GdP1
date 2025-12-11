// ============================================================================
//  Modul: worm_model.c - Fabian Ausmann
//
//  Dieses Modul kapselt die komplette Datenstruktur und Bewegungslogik
//  des Wurms. Andere Teile des Programms greifen nur über die hier
//  definierten Funktionen auf den Wurm zu und kennen die internen
//  Implementierungsdetails (Ringpuffer, Arrays etc.) nicht.
//
//  Ab Aufgabenblatt 7:
//   - Es gibt keinen globalen "theworm" mehr.
//   - Stattdessen werden alle Wurm-Daten in einer Struktur
//       struct worm
//     gesammelt.
//   - Jede Funktion arbeitet mit einem Zeiger auf einen konkreten Wurm:
//       struct worm* aworm
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
#include "board_model.h"   // placeItem(), getLastRow(), getLastCol(), struct pos
#include "worm_model.h"    // Öffentliche Schnittstelle des Wurmmodells

// ============================================================================
//  WURM-INITIALISIERUNG
// ============================================================================
//
//  Initialisiert den Wurm für den Spielstart.
//
//  Parameter:
//   - aworm   : Zeiger auf die Wurmstruktur, die initialisiert werden soll
//   - len_max : maximale Anzahl der Segmente (Ringpuffergröße)
//   - headpos : Startposition des Wurm-Kopfes (struct pos mit y/x)
//   - dir     : Start-Bewegungsrichtung des Wurms
//   - color   : zu verwendende Farbe (ColorPairs)
//
//  Ablauf:
//   1. Ringpuffer-Grenzen im struct worm setzen (maximaler Index)
//   2. Kopfindex auf 0 setzen
//   3. Alle Positionen im Puffer als "unbenutzt" markieren
//   4. Startposition des Kopfes eintragen
//   5. Start-Richtung setzen (dx/dy)
//   6. Wurmfarbe speichern
// ============================================================================

enum ResCodes initializeWorm(struct worm* aworm,
                             int len_max,
                             struct pos headpos,
                             enum WormHeading dir,
                             enum ColorPairs color)
{
    // Ringpuffer vorbereiten
    aworm->maxindex  = len_max - 1;
    aworm->headindex = 0;

    // Alle Positionen im Ringpuffer auf "unbenutzt" setzen
    for (int i = 0; i <= aworm->maxindex; i++) {
        aworm->wormpos[i].x = UNUSED_POS_ELEM;
        aworm->wormpos[i].y = UNUSED_POS_ELEM;
    }

    // Kopfstartposition setzen (Index 0)
    aworm->wormpos[0] = headpos;

    // Startrichtung einstellen (setzt dx/dy im Wurmobjekt)
    setWormHeading(aworm, dir);

    // Wurmfarbe speichern
    aworm->wcolor = color;

    return RES_OK;
}

// ============================================================================
//  BEWEGUNGSLOGIK – Richtung
// ============================================================================
//
//  Setzt die Bewegungsrichtung des Wurms.
//  Aus der logischen Richtung (enum WormHeading) werden die
//  konkreten Bewegungsdeltas dx und dy im struct worm berechnet.
//
//  Beispiel:
//   - WORM_RIGHT   -> dx =  1, dy =  0
//   - WORM_UP      -> dx =  0, dy = -1
//   - WORM_UP_LEFT -> dx = -1, dy = -1
// ============================================================================

void setWormHeading(struct worm* aworm, enum WormHeading dir) {
    switch(dir) {
        case WORM_UP:          aworm->dx =  0; aworm->dy = -1; break;
        case WORM_DOWN:        aworm->dx =  0; aworm->dy =  1; break;
        case WORM_LEFT:        aworm->dx = -1; aworm->dy =  0; break;
        case WORM_RIGHT:       aworm->dx =  1; aworm->dy =  0; break;

        // Diagonale Bewegungsrichtungen
        case WORM_UP_LEFT:     aworm->dx = -1; aworm->dy = -1; break;
        case WORM_UP_RIGHT:    aworm->dx =  1; aworm->dy = -1; break;
        case WORM_DOWN_RIGHT:  aworm->dx =  1; aworm->dy =  1; break;
        case WORM_DOWN_LEFT:   aworm->dx = -1; aworm->dy =  1; break;
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
//
//  Kopf und Schwanz werden (wie gefordert) mit 'X' dargestellt,
//  der Kopf zusätzlich in der Kopf-Farbe (COLP_WORM_HEAD).
// ============================================================================

void showWorm(struct worm* aworm) {

    // Zunächst den Index des Schwanzsegments im Ringpuffer bestimmen
    int tailindex = aworm->headindex;
    int prev = (aworm->headindex == 0)
               ? aworm->maxindex
               : aworm->headindex - 1;

    // Rückwärts laufen, bis ein "unbenutztes" Element oder der Kopf erreicht ist
    while (aworm->wormpos[prev].x != UNUSED_POS_ELEM &&
           prev != aworm->headindex) {

        tailindex = prev;
        prev = (prev == 0) ? aworm->maxindex : prev - 1;
    }

    // Alle Segmente vom Kopf bis zum Schwanz ausgeben
    int idx = aworm->headindex;

    while (1) {

        int y = aworm->wormpos[idx].y;
        int x = aworm->wormpos[idx].x;

        // Nur gültige Einträge zeichnen
        if (y != UNUSED_POS_ELEM) {

            chtype sym;
            enum ColorPairs col;

            // Kopf: eigenes Symbol und eigene Farbe (rot, X)
            if (idx == aworm->headindex) {
                sym = SYMBOL_WORM_HEAD;
                col = COLP_WORM_HEAD;
            }
            // Schwanz: eigenes Symbol, gleiche Farbe wie Körper
            else if (idx == tailindex) {
                sym = SYMBOL_WORM_TAIL;
                col = aworm->wcolor;
            }
            // inneres Segment
            else {
                sym = SYMBOL_WORM_INNER_ELEMENT;
                col = aworm->wcolor;
            }

            // Segment an seiner Position ausgeben
            placeItem(y, x, sym, col);
        }

        // Schleife abbrechen, wenn das Schwanzsegment erreicht wurde
        if (idx == tailindex)
            break;

        // Ringpuffer rückwärts durchlaufen
        idx = (idx == 0) ? aworm->maxindex : idx - 1;
    }
}

// ============================================================================
//  Löscht das ehemalige Schwanzende (setzt freies Feld an diese Stelle)
//
//  Idee:
//   Der Wurm wird als Ringpuffer gespeichert. Wenn sich der Kopf weiter
//   bewegt, wird das Element hinter dem Kopfindex zum neuen Schwanz.
//   Diese Position wird wieder als "frei" gezeichnet.
// ============================================================================

void cleanWormTail(struct worm* aworm) {

    // Index des Schwanzsegments: Element hinter dem Kopf im Ring
    int tailindex =
        (aworm->headindex + 1) % (aworm->maxindex + 1);

    // Nur löschen, wenn an dieser Stelle tatsächlich ein Segment existiert
    if (aworm->wormpos[tailindex].y != UNUSED_POS_ELEM) {

        placeItem(
            aworm->wormpos[tailindex].y,
            aworm->wormpos[tailindex].x,
            SYMBOL_FREE_CELL,
            COLP_FREE_CELL
        );
    }
}

// ============================================================================
//  Prüft, ob ein Spielfeldfeld bereits vom Wurm belegt ist
//
//  Parameter:
//   - aworm        : Zeiger auf den zu prüfenden Wurm
//   - new_headpos  : zu überprüfende Position (struct pos mit y/x)
//
//  Rückgabewert:
//   - true  : wenn ein Segment des Wurms an new_headpos liegt
//   - false : wenn das Feld frei ist
//
//  Vorgehen:
//   - Vom Kopf aus durch den Ringpuffer laufen, bis wieder der Kopf erreicht
//     ist oder ein unbenutztes Element gefunden wird.
// ============================================================================

bool isInUseByWorm(struct worm* aworm, struct pos new_headpos) {

    int i = aworm->headindex;

    do {
        // im Ring eins weiter gehen
        i = (i + 1) % (aworm->maxindex + 1);

        // Position vergleichen
        if (aworm->wormpos[i].y == new_headpos.y &&
            aworm->wormpos[i].x == new_headpos.x)
            return true;

    } while (i != aworm->headindex &&
             aworm->wormpos[i].x != UNUSED_POS_ELEM);

    return false;
}

// ============================================================================
//  Bewegt den Wurm um einen Schritt in die aktuelle Richtung
//
//  Parameter:
//   - aworm       : Zeiger auf den zu bewegenden Wurm
//   - agame_state : Zeiger auf den aktuellen Spielzustand. Dieser wird
//                   modifiziert, wenn eine Kollision erkannt wird.
//
//  Ablauf:
//   1. Neue Kopfposition (new_headpos) aus alter Position + dx/dy berechnen
//   2. Randkollision prüfen:
//        - Falls außerhalb des Spielfelds → WORM_OUT_OF_BOUNDS
//   3. Selbstkollision prüfen:
//        - Falls neue Position bereits vom Wurm belegt → WORM_CROSSING
//   4. Wenn alles frei:
//        - Kopfindex um 1 weiterschieben (Ringpuffer)
//        - neue Kopfposition in den Positionspuffer eintragen
// ============================================================================

void moveWorm(struct worm* aworm, enum GameStates* agame_state) {

    // neue Kopfposition aus aktueller Position plus Bewegungsdelta berechnen
    struct pos new_headpos;
    new_headpos.x = aworm->wormpos[aworm->headindex].x + aworm->dx;
    new_headpos.y = aworm->wormpos[aworm->headindex].y + aworm->dy;

    // Randkollision prüfen: Bildschirmgrenzen überschritten?
    if (new_headpos.x < 0 || new_headpos.x > getLastCol()
     || new_headpos.y < 0 || new_headpos.y > getLastRow()) {

        *agame_state = WORM_OUT_OF_BOUNDS;
        return;
    }

    // Selbstkollision prüfen: liegt bereits ein Wurmsegment an der neuen Position?
    if (isInUseByWorm(aworm, new_headpos)) {

        *agame_state = WORM_CROSSING;
        return;
    }

    // Wenn keine Kollision vorliegt: Kopf weiterbewegen

    // Kopfindex im Ringpuffer weiterschieben
    aworm->headindex++;
    aworm->headindex %= (aworm->maxindex + 1);

    // neue Kopfposition in den Ringpuffer eintragen
    aworm->wormpos[aworm->headindex] = new_headpos;
}

// ============================================================================
//  Liefert die aktuelle Kopfposition des Wurms
//
//  Rückgabewert:
//   - struct pos mit der (y,x)-Position des Kopfes
//
//  Nutzung:
//   - Wird z.B. von der Message-Area verwendet, um dem Spieler die aktuelle
//     Kopfposition anzuzeigen (Aufgabenblatt 7).
// ============================================================================

struct pos getWormHeadPos(struct worm* aworm) {
    return aworm->wormpos[aworm->headindex];
}



