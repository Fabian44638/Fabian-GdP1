// Worm070 - Aufgabenblatt 8
// ============================================================================
//
//  In dieser Headerdatei wird alles definiert, was zur Verwaltung und Bewegung
//  des Wurms notwendig ist. Dazu gehören:
//
//    - die zentrale Datenstruktur struct worm
//    - Bewegungsrichtungen
//    - Ringpuffer-Größe und Markerwerte
//    - Bonus-Werte für Futter und manuelles Wachstum
//    - Funktionsprototypen für Initialisierung, Bewegung und Darstellung
//
//  Besonderheit der aktuellen Version:
//    BONUS_MANUAL = 3
//    Dieser Bonus wird verwendet, wenn der Spieler die Taste g drückt.
//    Dadurch wächst der Wurm um exakt drei Segmente.
// -----------------------------------------------------------------------------

#ifndef _WORM_MODEL_H
#define _WORM_MODEL_H

#include <stdbool.h>
#include "worm.h"
#include "board_model.h"

// ============================================================================
//  Marker für unbenutzte Positionen
// ============================================================================
//
//  UNUSED_POS_ELEM:
//     Wird im Ringpuffer verwendet, um Positionen zu kennzeichnen,
//     die momentan nicht belegt sind. Dies dient zur Initialisierung.
// ============================================================================
#define UNUSED_POS_ELEM -1


// ============================================================================
//  Maximale Wurmlänge
// ============================================================================
//
//  WORM_LENGTH:
//     Der Wurm verwendet einen Ringpuffer, dessen maximale Größe dem gesamten
//     Spielfeld entspricht.
//
//     Hintergrund:
//       Es könnte theoretisch jede Zelle des Spielfelds belegt werden.
//       Deshalb ist die maximale Länge gleich der Anzahl aller Felder.
//
//  WORM_INITIAL_LENGTH:
//     Die Startlänge des Wurms beim Beginn eines Levels.
// ============================================================================
#define WORM_LENGTH         (MIN_NUMBER_OF_ROWS * MIN_NUMBER_OF_COLS)
#define WORM_INITIAL_LENGTH 4


// ============================================================================
//  Boni für Futter und manuelles Wachstum
// ============================================================================
//
//  Die drei Futtersorten liefern unterschiedliche Wachstumswerte:
//
//      BONUS_1 = 2   // kleines Futter
//      BONUS_2 = 4   // mittleres Futter
//      BONUS_3 = 6   // großes Futter
//
//      BONUS_MANUAL = 3
//      Dieser Wert wird verwendet, wenn der Spieler die Taste g drückt.
//      Der Wurm wächst dann um drei Segmente.
// ============================================================================
enum Boni {
    BONUS_1 = 2,
    BONUS_2 = 4,
    BONUS_3 = 6,
    BONUS_MANUAL = 3
};


// ============================================================================
//  Datenstruktur eines Wurms
// ============================================================================
//
//  Die Struktur stellt einen Wurm als Ringpuffer dar.
//
//  Felder:
//
//    cur_lastindex:
//         Der letzte gültige Index im Ringpuffer. Dies bestimmt die aktuelle
//         Länge des Wurms. Die tatsächliche Länge ist cur_lastindex + 1.
//
//    maxindex:
//         Größter zulässiger Index im Ringpuffer. Entspricht der maximal
//         möglichen Wurmlänge.
//
//    headindex:
//         Der Index, an dem sich der Kopf befindet.
//         Alle anderen Segmente folgen rückwärts im Ringpuffer.
//
//    wormpos[]:
//         Array aller Positionsdaten. Jeder Eintrag enthält eine x- und
//         y-Koordinate. Unbenutzte Einträge stehen auf UNUSED_POS_ELEM.
//
//    dx, dy:
//         Die Richtung, in die sich der Wurm beim nächsten Bewegungsschritt
//         bewegen wird.
//
//    wcolor:
//         Farbdefinition für den kompletten Wurm (außer Kopf).
// ============================================================================
struct worm {
    int cur_lastindex;               // letzter gültiger Index (aktuelle Wurmlänge - 1)
    int maxindex;                    // maximal zulässiger Index

    int headindex;                   // Position des Kopfes im Ringpuffer

    struct pos wormpos[WORM_LENGTH]; // Ringpuffer für Wurmsegmente

    int dx;                          // Bewegungsrichtung in x-Richtung
    int dy;                          // Bewegungsrichtung in y-Richtung

    enum ColorPairs wcolor;          // Farbe des Wurms
};


// ============================================================================
//  Bewegungsrichtungen des Wurms
// ============================================================================
//
//  Jede Richtung bestimmt die Werte für dx und dy, die später von
//  moveWorm() eingesetzt werden.
//
//     WORM_UP:        y - 1
//     WORM_DOWN:      y + 1
//     WORM_LEFT:      x - 1
//     WORM_RIGHT:     x + 1
//
//  Dazu vier diagonale Richtungen.
// ============================================================================
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
//  API-Funktionen für den Wurm
// ============================================================================
//
//  initializeWorm:
//      Setzt die Startlänge, Position, Richtung und Farbe des Wurmes.
//
//  growWorm:
//      Erhöht die Länge durch Futter oder manuellen Bonus.
//
//  showWorm:
//      Zeichnet den kompletten Wurm auf das Spielfeld.
//
//  cleanWormTail:
//      Entfernt das Schwanzsegment vom Bildschirm.
//
//  moveWorm:
//      Bewegt den Wurm und verarbeitet Futter, Kollisionen und Wachstum.
//
// Getter:
//      getWormHeadPos  → liefert die aktuelle Kopfposition
//      getWormLength   → liefert die Anzahl der Segmente
//
// Setter:
//      setWormHeading  → neue Bewegungsrichtung setzen
// ============================================================================
extern enum ResCodes initializeWorm(struct worm* aworm,
                                    int len_max,
                                    int len_cur,
                                    struct pos headpos,
                                    enum WormHeading dir,
                                    enum ColorPairs color);

extern void growWorm(struct worm* aworm, enum Boni growth);
extern void showWorm(struct board* aboard, struct worm* aworm);
extern void cleanWormTail(struct board* aboard, struct worm* aworm);

extern void moveWorm(struct board* aboard,
                     struct worm* aworm,
                     enum GameStates* agame_state);

// Getter-Funktionen
extern struct pos getWormHeadPos(struct worm* aworm);
extern int getWormLength(struct worm* aworm);

// Setter-Funktion
extern void setWormHeading(struct worm* aworm, enum WormHeading dir);

#endif






