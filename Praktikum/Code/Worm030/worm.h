// ============================================================================
//  Modul-Header: worm.h - Fabian Ausmann
//
//  Diese Header-Datei enthält alle gemeinsamen Definitionen, die von
//  mehreren Modulen des Spiels benötigt werden.
//
//  Dazu gehören:
//
//   - Aufzählungstypen (enum) für Rückgabewerte, Spielzustände und Farben
//   - Zentrale Spielkonstanten (Spielfeldgröße, Pausenzeit, Wurmlänge)
//   - Zeichendefinitionen für die Darstellung
//
//  Ziel:
//   worm.h stellt sicher, dass alle Module dieselben Datentypen und
//   Konstanten verwenden und konsistent auf die gleichen Werte zugreifen.
//
// ============================================================================

#ifndef _WORM_H
#define _WORM_H

// ============================================================================
//  Rückgabewerte von Funktionen im Spiel
//
//  Dieser Aufzählungstyp wird verwendet, um anzuzeigen, ob eine Funktion
//  erfolgreich ausgeführt werden konnte oder fehlgeschlagen ist.
// ============================================================================

enum ResCodes {
    RES_OK,        // Funktion wurde erfolgreich ausgeführt
    RES_FAILED     // Fehler bei der Ausführung
};

// ============================================================================
//  Zentrale Spieleinstellungen
//
//  Diese Konstanten definieren grundlegende Parameter des Spiels.
// ============================================================================

// Wartezeit zwischen zwei Spiel-Frames in Millisekunden
#define NAP_TIME 100

// Minimale erlaubte Fenstergröße für ein spielbares Terminal
#define MIN_NUMBER_OF_ROWS 15
#define MIN_NUMBER_OF_COLS 20

// Maximale Länge des Wurms (Größe des Ringpuffers)
#define WORM_LENGTH 16

// ============================================================================
//  Marker für unbenutzte Felder im Positions-Ringpuffer
//
//  Mit diesem Wert werden freie Positionen in den Positionsarrays des
//  Wurms gekennzeichnet.
// ============================================================================

#define UNUSED_POS_ELEM -1

// ============================================================================
//  Farbdefinitionen für ncurses
//
//  Diese Werte werden gemeinsam mit der ncurses-Funktion COLOR_PAIR()
//  verwendet, um die darzustellenden Farben auszuwählen.
//
//  Hinweis:
//   Der erste Wert beginnt bewusst bei 1, da ncurses Farbpaare ab 1
//   nummeriert.
// ============================================================================

enum ColorPairs {
    COLP_USER_WORM = 1,   // Farbe für den Körper des Wurms
    COLP_FREE_CELL,      // Farbe für freie Spielfeldzellen
    COLP_WORM_HEAD       // Sonderfarbe für den Wurm-Kopf
};

// ============================================================================
//  Zeichendefinitionen für die grafische Darstellung
//
//  Diese Konstanten legen fest, welche Zeichen zur Anzeige der einzelnen
//  Bestandteile des Spiels verwendet werden.
// ============================================================================

#define SYMBOL_FREE_CELL           ' '   // Darstellung eines leeren Feldes
#define SYMBOL_WORM_INNER_ELEMENT  '0'   // Darstellung eines Körper-Segments
#define SYMBOL_WORM_HEAD           'X'   // Darstellung des Wurm-Kopfes
#define SYMBOL_WORM_TAIL           'X'   // Darstellung des Wurm-Schwanzes

// ============================================================================
//  Spielzustände
//
//  Dieser Aufzählungstyp beschreibt mögliche Zustände des Spiels während
//  der Ausführung.
// ============================================================================

enum GameStates {
    WORM_GAME_ONGOING,  // Das Spiel läuft noch
    WORM_OUT_OF_BOUNDS, // Der Wurm hat den Spielfeldrand überschritten
    WORM_CROSSING,      // Der Wurm ist mit sich selbst kollidiert
    WORM_GAME_QUIT      // Das Spiel wurde vom Benutzer beendet
};

#endif  // _WORM_H


