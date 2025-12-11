// Worm070 - Aufgabenblatt 8
// ============================================================================

#include <curses.h>

#include "worm.h"
#include "board_model.h"
#include "worm_model.h"

// ============================================================================
//  initializeWorm
// ============================================================================
// Aufgabe:
//    Initialisiert einen Wurm und setzt ihn an eine Startposition auf dem
//    Spielfeld. Außerdem werden interne Werte wie maximale Länge, aktuelle
//    Länge, Kopfindex und Bewegungsrichtung gesetzt.
//
// Parameter:
//    aworm     : Zeiger auf die Wurmstruktur
//    len_max   : maximal mögliche Länge des Wurms
//    len_cur   : tatsächliche Startlänge
//    headpos   : Startposition des Kopfes
//    dir       : Anfangsrichtung des Wurms
//    color     : Farbattribut des Wurmkörpers
//
// Vorgehen:
//    - Falls len_cur > len_max ist, wird korrigiert.
//    - Alle Positionen des Ringspeichers werden in UNUSED_POS_ELEM gesetzt.
//    - Die Kopfposition wird eingetragen.
//    - Richtung und Farbe werden gesetzt.
//
// Rückgabewert:
//    RES_OK bei Erfolg.
// ============================================================================
enum ResCodes initializeWorm(struct worm* aworm,
                             int len_max,
                             int len_cur,
                             struct pos headpos,
                             enum WormHeading dir,
                             enum ColorPairs color)
{
    if (len_cur > len_max)
        len_cur = len_max;

    aworm->maxindex      = len_max - 1;   // maximal genutzter Index im Ringspeicher
    aworm->cur_lastindex = len_cur - 1;   // letzter Index der aktuellen Länge
    aworm->headindex     = 0;             // Kopf befindet sich initial bei Index 0

    // Alle gespeicherten Positionen ungültig machen
    for (int i = 0; i <= aworm->maxindex; i++) {
        aworm->wormpos[i].x = UNUSED_POS_ELEM;
        aworm->wormpos[i].y = UNUSED_POS_ELEM;
    }

    // Kopf auf Startposition setzen
    aworm->wormpos[0] = headpos;

    // Anfangsrichtung übernehmen
    setWormHeading(aworm, dir);

    // Farbinformation speichern
    aworm->wcolor = color;

    return RES_OK;
}


// ============================================================================
//  setWormHeading
// ============================================================================
// Aufgabe:
//    Setzt die Bewegungsrichtung des Wurms.
//
// Funktionsweise:
//    Die Bewegungsrichtung wird durch zwei Werte beschrieben:
//       dx = Änderung der x-Koordinate
//       dy = Änderung der y-Koordinate
//
//    Beispiel:
//       WORM_UP bedeutet dy = -1 (eine Zeile nach oben), dx = 0.
//
// Hinweis:
//    Die Richtung wirkt sich erst beim nächsten moveWorm()-Aufruf aus.
// ============================================================================
void setWormHeading(struct worm* aworm, enum WormHeading dir)
{
    switch (dir) {
        case WORM_UP:         aworm->dx =  0; aworm->dy = -1; break;
        case WORM_DOWN:       aworm->dx =  0; aworm->dy =  1; break;
        case WORM_LEFT:       aworm->dx = -1; aworm->dy =  0; break;
        case WORM_RIGHT:      aworm->dx =  1; aworm->dy =  0; break;
        case WORM_UP_LEFT:    aworm->dx = -1; aworm->dy = -1; break;
        case WORM_UP_RIGHT:   aworm->dx =  1; aworm->dy = -1; break;
        case WORM_DOWN_RIGHT: aworm->dx =  1; aworm->dy =  1; break;
        case WORM_DOWN_LEFT:  aworm->dx = -1; aworm->dy =  1; break;
    }
}


// ============================================================================
//  growWorm
// ============================================================================
// Aufgabe:
//    Erhöht die Länge des Wurms um den angegebenen Bonuswert.
//
// Funktionsweise:
//    Die Wurmlänge ergibt sich aus cur_lastindex + 1.
//    Durch Wachstum wird cur_lastindex entsprechend vergrößert.
//    Der maximale Wert bleibt aber durch maxindex begrenzt.
//
// Wichtig:
//    Es findet kein Doppelwachstum statt. Wachstum ist linear additiv.
// ============================================================================
void growWorm(struct worm* aworm, enum Boni growth)
{
    int new_lastindex = aworm->cur_lastindex + (int)growth;

    if (new_lastindex > aworm->maxindex)
        new_lastindex = aworm->maxindex;

    aworm->cur_lastindex = new_lastindex;
}


// ============================================================================
//  showWorm
// ============================================================================
// Aufgabe:
//    Zeichnet den gesamten Wurm auf das Spielfeld. Der Wurm liegt als
//    Ringspeicher vor. Der Kopf ist headindex, der Schwanz ist headindex+1.
//
// Darstellung:
//    Kopf   X (Symbol_WORM_HEAD)
//    Schwanz X (Symbol_WORM_TAIL)
//    Körper 0 (Symbol_WORM_INNER)
//
// Vorgehen:
//    - Start bei headindex
//    - rückwärts durch den Ringspeicher laufen
//    - an jeder gültigen Position das passende Symbol zeichnen
//
// Hinweis:
//    Das Löschen des Schwanzsegments erfolgt separat durch cleanWormTail().
// ============================================================================
void showWorm(struct board* aboard, struct worm* aworm)
{
    int ring_size = aworm->cur_lastindex + 1;
    int idx = aworm->headindex;

    // Schwanzindex ist der nächste Eintrag nach dem Kopf im Ringspeicher
    int tailindex = (aworm->headindex + 1) % ring_size;

    while (1) {
        int y = aworm->wormpos[idx].y;
        int x = aworm->wormpos[idx].x;

        if (y != UNUSED_POS_ELEM) {

            chtype sym;
            enum ColorPairs col;

            if (idx == aworm->headindex) {
                sym = SYMBOL_WORM_HEAD;
                col = COLP_WORM_HEAD;
            }
            else if (idx == tailindex) {
                sym = SYMBOL_WORM_TAIL;
                col = aworm->wcolor;
            }
            else {
                sym = SYMBOL_WORM_INNER;
                col = aworm->wcolor;
            }

            placeItem(aboard, y, x, BC_USED_BY_WORM, sym, col);
        }

        if (idx == tailindex)
            break;

        // Im Ringspeicher rückwärts gehen
        idx = (idx == 0) ? aworm->cur_lastindex : idx - 1;
    }
}


// ============================================================================
//  cleanWormTail
// ============================================================================
// Aufgabe:
//    Löscht genau das Schwanzsegment des Wurms vom Spielfeld.
//
// Funktionsweise:
//    tailindex = headindex + 1 im Ringspeicher.
//    Diese Position wird wieder als freies Feld eingezeichnet.
//
// Hinweis:
//    Diese Funktion wird vor jeder Bewegung aufgerufen, bevor der neue
//    Kopf eingezeichnet wird.
// ============================================================================
void cleanWormTail(struct board* aboard, struct worm* aworm)
{
    int ring_size = aworm->cur_lastindex + 1;
    int tailindex = (aworm->headindex + 1) % ring_size;

    if (aworm->wormpos[tailindex].y != UNUSED_POS_ELEM) {
        placeItem(aboard,
                  aworm->wormpos[tailindex].y,
                  aworm->wormpos[tailindex].x,
                  BC_FREE_CELL,
                  SYMBOL_FREE_CELL,
                  COLP_FREE_CELL);
    }
}


// ============================================================================
//  moveWorm
// ============================================================================
// Aufgabe:
//    Bewegt den Wurm um genau ein Segment in die aktuell gesetzte Richtung.
//    Dabei wird geprüft:
//
//       - Ist der neue Kopf außerhalb des Spielfelds?
//       - Kollision mit Barriere?
//       - Kollision mit eigenem Körper?
//       - Futter aufgenommen?
//
// Funktionsweise:
//    - neue Kopfposition berechnen
//    - Inhalt der Zielzelle auswerten
//    - GameStates entsprechend setzen
//    - bei Futter: Wurm wachsen lassen + Futterzähler reduzieren
//    - Kopfindex im Ringspeicher weiterschalten
// ============================================================================
void moveWorm(struct board* aboard,
              struct worm* aworm,
              enum GameStates* agame_state)
{
    struct pos headpos;

    // Neue Kopfposition bestimmen
    headpos.x = aworm->wormpos[aworm->headindex].x + aworm->dx;
    headpos.y = aworm->wormpos[aworm->headindex].y + aworm->dy;

    // Spielfeldgrenzen prüfen
    if (headpos.x < 0 || headpos.x > aboard->last_col ||
        headpos.y < 0 || headpos.y > aboard->last_row) {

        *agame_state = WORM_OUT_OF_BOUNDS;
        return;
    }

    // Inhalt der Zielzelle bestimmen
    enum BoardCodes content = getContentAt(aboard, headpos);

    switch (content) {

        case BC_BARRIER:
            *agame_state = WORM_CRASH;
            return;

        case BC_USED_BY_WORM:
            *agame_state = WORM_CROSSING;
            return;

        case BC_FOOD_1:
            growWorm(aworm, BONUS_1);
            decrementNumberOfFoodItems(aboard);
            break;

        case BC_FOOD_2:
            growWorm(aworm, BONUS_2);
            decrementNumberOfFoodItems(aboard);
            break;

        case BC_FOOD_3:
            growWorm(aworm, BONUS_3);
            decrementNumberOfFoodItems(aboard);
            break;

        default:
            break;
    }

    // Kopfindex im Ringspeicher weiterschalten
    aworm->headindex++;

    if (aworm->headindex > aworm->cur_lastindex)
        aworm->headindex = 0;

    // Neue Kopfposition schreiben
    aworm->wormpos[aworm->headindex] = headpos;
}


// ============================================================================
//  Getter-Funktionen
// ============================================================================
//  getWormHeadPos:
//      Liefert die aktuelle Kopfposition aus dem Ringspeicher.
//
//  getWormLength:
//      Berechnet die aktuelle Länge des Wurms aus cur_lastindex + 1.
// ============================================================================
struct pos getWormHeadPos(struct worm* aworm)
{
    return aworm->wormpos[aworm->headindex];
}

int getWormLength(struct worm* aworm)
{
    return aworm->cur_lastindex + 1;
}






