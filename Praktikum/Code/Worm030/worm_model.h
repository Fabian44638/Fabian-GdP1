// ============================================================================
//  Modul-Header: worm_model.h
//
//  Dieser Header beschreibt die öffentliche Schnittstelle des Moduls
//  „worm_model“. Alle hier deklarierten Funktionen und Datentypen können
//  von anderen Modulen des Programms verwendet werden.
//
//  Das eigentliche Innenleben des Wurms (Ringpuffer, Positionsarrays,
//  Bewegungslogik) ist in der Implementierungsdatei worm_model.c verborgen.
//  Auf diese internen Details hat kein anderes Modul direkten Zugriff.
//
//  Ziele dieses Moduls:
//   - Bereitstellung einer klaren und sauberen API zur Steuerung des Wurms
//   - Kapselung aller internen Datenstrukturen
//   - Ermöglichung von Richtungswechsel, Bewegung, Zeichnung und Kollisionsprüfung
// ============================================================================

#ifndef _WORM_MODEL_H
#define _WORM_MODEL_H

#include <stdbool.h>  // Für den bool-Typ (true / false)
#include "worm.h"     // Enthält gemeinsame Datentypen wie GameStates, ResCodes, ColorPairs

// ============================================================================
//  Bewegungsrichtungen des Wurms
//
//  Dieser Aufzählungstyp beschreibt alle möglichen Bewegungsrichtungen
//  des Wurms. Neben den klassischen vier Hauptachsen (oben, unten, links,
//  rechts) werden hier auch diagonale Richtungen unterstützt.
// ============================================================================

enum WormHeading {
    WORM_UP,          // Bewegung nach oben
    WORM_DOWN,        // Bewegung nach unten
    WORM_LEFT,        // Bewegung nach links
    WORM_RIGHT,       // Bewegung nach rechts

    // Diagonale Bewegungsrichtungen
    WORM_UP_LEFT,
    WORM_UP_RIGHT,
    WORM_DOWN_RIGHT,
    WORM_DOWN_LEFT
};

// ============================================================================
//  Initialisiert den Wurm für ein neues Spiel
//
//  Parameter:
//   - len_max : Maximale Anzahl von Segmenten (Ringpuffergröße)
//   - y, x    : Startposition des Wurmkopfes
//   - dir     : Startbewegungsrichtung
//   - color   : Farbe des Wurms
//
//  Rückgabewert:
//   - RES_OK      : Initialisierung erfolgreich
//   - RES_FAILED : Fehler bei der Initialisierung
// ============================================================================

extern enum ResCodes initializeWorm(int len_max, int y, int x,
                                    enum WormHeading dir,
                                    enum ColorPairs color);

// ============================================================================
//  Zeichnet den kompletten Wurm auf das Spielfeld
//
//  Dabei werden Kopf, Körper und Schwanz korrekt unterschieden und mit
//  ihren jeweiligen Symbolen und Farben dargestellt.
// ============================================================================

extern void showWorm(void);

// ============================================================================
//  Entfernt das hinterste Wurmsegment vom Spielfeld
//
//  Diese Funktion wird vor der Bewegung des Wurms aufgerufen, um an der
//  alten Schwanzposition wieder ein leeres Feld zu zeichnen.
// ============================================================================

extern void cleanWormTail(void);

// ============================================================================
//  Bewegt den Wurm um einen einzelnen Schritt
//
//  Parameter:
//   - agame_state : Zeiger auf den aktuellen Spielstatus. Dieser wird
//                   angepasst, falls eine Rand- oder Selbstkollision
//                   erkannt wird.
// ============================================================================

extern void moveWorm(enum GameStates* agame_state);

// ============================================================================
//  Prüft, ob ein bestimmtes Feld bereits vom Wurm belegt ist
//
//  Parameter:
//   - y, x : Koordinaten des zu prüfenden Spielfeldes
//
//  Rückgabewert:
//   - true  : Position ist durch ein Wurmsegment belegt
//   - false : Feld ist frei
// ============================================================================

extern bool isInUseByWorm(int y, int x);

// ============================================================================
//  Setzt die aktuelle Bewegungsrichtung des Wurms
//
//  Parameter:
//   - dir : Neue Bewegungsrichtung aus dem Enum WormHeading
//
//  Diese Funktion berechnet intern die Bewegungsdeltas dx und dy, die beim
//  nächsten Bewegungsschritt verwendet werden.
// ============================================================================

extern void setWormHeading(enum WormHeading dir);

#endif  // _WORM_MODEL_H

