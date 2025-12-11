// ============================================================================
//  Modul-Header: worm_model.h - Fabian Ausmann
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
//
//  Erweiterungen ab Aufgabenblatt 7:
//   - Einführung von struct pos für Koordinaten
//   - Einführung von struct worm zur vollständigen Kapselung aller
//     Wurm-Daten (ersetzt ehemals globale Variablen)
// ============================================================================

#ifndef _WORM_MODEL_H
#define _WORM_MODEL_H

#include <stdbool.h>       // Für den bool-Typ (true / false)
#include "worm.h"          // Allgemeine Datentypen und Konstanten
#include "board_model.h"  // Enthält struct pos

// ============================================================================
//  Bewegungsrichtungen des Wurms
//
//  Beschreibt alle möglichen Bewegungsrichtungen des Wurms.
//  Neben den vier Hauptachsen werden auch diagonale Richtungen unterstützt.
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
//  Struktur: struct worm
//
//  Diese Struktur enthält alle Daten, die den Zustand eines Wurms
//  vollständig beschreiben.
//
//  Sie ersetzt die ehemaligen globalen Variablen aus Blatt 6 wie:
//
//    theworm_headindex
//    theworm_maxindex
//    theworm_wormpos_x[]
//    theworm_wormpos_y[]
//    theworm_dx
//    theworm_dy
//    theworm_wcolor
//
//  Vorteile:
//   - komplette Datenkapselung
//   - mehrere Würmer wären prinzipiell möglich
//   - saubere, modulare Programmstruktur
// ============================================================================

struct worm {

    // letzter erlaubter Index im Ringpuffer
    int maxindex;

    // aktueller Index des Wurmkopfes
    int headindex;

    // Ringpuffer mit allen Segmentpositionen
    // Jeder Eintrag ist ein struct pos mit y/x-Koordinate
    struct pos wormpos[WORM_LENGTH];

    // Bewegungsdelta pro Schritt
    int dx;   // x-Änderung (-1, 0, +1)
    int dy;   // y-Änderung (-1, 0, +1)

    // Anzeigefarbe des Wurms
    enum ColorPairs wcolor;
};

// ============================================================================
//  Funktions-Prototypen
//
//  WICHTIG:
//  Jede Funktion bekommt nun einen Zeiger auf den zu bearbeitenden Wurm
//  übergeben:
//
//      struct worm* aworm
//
//  Dadurch arbeiten alle Funktionen nicht mehr mit globalen Daten,
//  sondern ausschließlich mit dem konkret übergebenen Wurm-Objekt.
// ============================================================================

// Initialisiert den Wurm für ein neues Spiel
extern enum ResCodes initializeWorm(struct worm* aworm,
                                    int len_max,
                                    struct pos headpos,
                                    enum WormHeading dir,
                                    enum ColorPairs color);

// Zeichnet den kompletten Wurm auf das Spielfeld
extern void showWorm(struct worm* aworm);

// Löscht das letzte Schwanzsegment
extern void cleanWormTail(struct worm* aworm);

// Bewegt den Wurm um einen einzelnen Schritt
extern void moveWorm(struct worm* aworm,
                     enum GameStates* agame_state);

// Prüft, ob eine Position bereits durch den Wurm belegt ist
extern bool isInUseByWorm(struct worm* aworm,
                          struct pos new_headpos);

// Setzt die Bewegungsrichtung des Wurms
extern void setWormHeading(struct worm* aworm,
                           enum WormHeading dir);

// Gibt die aktuelle Position des Wurm-Kopfes zurück
// (wird für die Message-Area benötigt)
extern struct pos getWormHeadPos(struct worm* aworm);

#endif  // _WORM_MODEL_H

