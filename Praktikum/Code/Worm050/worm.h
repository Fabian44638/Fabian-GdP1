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
//   - Konstanten für die Message Area (reservierte Zeilen, Barriere)
//
//  Ziel:
//   worm.h stellt sicher, dass alle Module dieselben Datentypen und
//   Konstanten verwenden und konsistent auf die gleichen Werte zugreifen.
// ============================================================================

#ifndef _WORM_H
#define _WORM_H

// ============================================================================
//  Rückgabewerte von Funktionen im Spiel
//
//  Dieser Aufzählungstyp wird verwendet, um anzuzeigen, ob eine Funktion
//  erfolgreich ausgeführt werden konnte oder fehlgeschlagen ist.
//
//  Neu in Blatt 7:
//   - RES_INTERNAL_ERROR : wird verwendet, wenn ein interner Fehler
//     festgestellt wird, der eigentlich „nie passieren sollte“.
// ============================================================================

enum ResCodes {
    RES_OK,             // Funktion wurde erfolgreich ausgeführt
    RES_FAILED,         // Fehler bei der Ausführung
    RES_INTERNAL_ERROR  // interner Fehler im Programmablauf
};

// ============================================================================
//  Zentrale Spieleinstellungen
//
//  Diese Konstanten definieren grundlegende Parameter des Spiels.
//
//  Neu in Blatt 7:
//   - ROWS_RESERVED      : Anzahl der Zeilen, die unten für die Message Area
//                          reserviert werden (Status-/Fehlermeldungen)
//   - MIN_NUMBER_OF_ROWS : Mindestanzahl an Zeilen, die für den Wurm-Bereich
//                          zur Verfügung stehen müssen (ohne Message Area)
//   - MIN_NUMBER_OF_COLS : Mindestanzahl an Spalten für das Spielfeld
// ============================================================================

// Wartezeit zwischen zwei Spiel-Frames in Millisekunden
#define NAP_TIME 100

// Anzahl der Zeilen, die am unteren Rand für die Message Area reserviert sind
#define ROWS_RESERVED 4

// Minimale Anzahl an Zeilen für den eigentlichen Wurm-Spielbereich
// (die Fensterhöhe muss also mindestens ROWS_RESERVED + MIN_NUMBER_OF_ROWS sein)
#define MIN_NUMBER_OF_ROWS 16

// Minimale Anzahl an Spalten für den Spielbereich (Spielfeldbreite)
#define MIN_NUMBER_OF_COLS 40

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
//
//  Erweiterungen für Blatt 7:
//   - COLP_BARRIER : Farbe für die Trennlinie bzw. Barriere zwischen
//                    Spielfeld und Message Area.
//
//  Zusätzliche Extra-Farben (aus deinen Erweiterungen):
//   - COLP_WORM_HEAD : Sonderfarbe für den Kopf des Wurms.
// ============================================================================

enum ColorPairs {
    COLP_USER_WORM = 1,   // Farbe für den Körper des Wurms
    COLP_FREE_CELL,       // Farbe für freie Spielfeldzellen
    COLP_BARRIER,         // Farbe für Barriere / Trennlinie (Message Area)
    COLP_WORM_HEAD        // Sonderfarbe für den Wurm-Kopf (Extra)
};

// ============================================================================
//  Zeichendefinitionen für die grafische Darstellung
//
//  Diese Konstanten legen fest, welche Zeichen zur Anzeige der einzelnen
//  Bestandteile des Spiels verwendet werden.
//
//  Neu in Blatt 7:
//   - SYMBOL_BARRIER : Zeichen für die horizontale Trennlinie zwischen
//                      Spielfeld und Message Area.
//
//  Aus deinen Extras bleibt erhalten:
//   - Kopf und Schwanz des Wurms werden beide mit 'X' dargestellt.
// ============================================================================

#define SYMBOL_FREE_CELL           ' '   // Darstellung eines leeren Feldes
#define SYMBOL_BARRIER             '#'   // Darstellung der Barriere / Trennlinie
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



