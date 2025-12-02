// ============================================================================
//  Modul-Header: board_model.h - Fabian Ausmann
//
//  Dieses Header-File definiert die öffentliche Schnittstelle des Moduls
//  „board_model“. Es stellt Funktionen zur Verfügung, mit denen andere
//  Programmteile Ausgaben auf dem Spielfeld vornehmen können, ohne direkt
//  auf die ncurses-Bibliothek zugreifen zu müssen.
//
//  In dieser Datei befinden sich ausschließlich:
//
//   - Funktions-Deklarationen (keine Implementierungen)
//   - benötigte Typ-Definitionen über Includes
//
//  Die eigentliche Implementierung der Funktionen befindet sich in:
//      board_model.c
// ============================================================================

#ifndef _BOARD_MODEL_H
#define _BOARD_MODEL_H

#include <curses.h>   // Notwendig für den Datentyp "chtype"
#include "worm.h"    // Enthält Farbdefinitionen und globale Datentypen

// ============================================================================
//  Zeichnet ein einzelnes Zeichen auf das Spielfeld
//
//  Parameter:
//   - y          : Zeilenposition (vertikale Koordinate)
//   - x          : Spaltenposition (horizontale Koordinate)
//   - symbol     : Das darzustellende Zeichen
//   - color_pair : Zu verwendendes Farb-Paar (ColorPairs aus worm.h)
//
//  Zweck:
//   Diese Funktion kapselt sämtliche ncurses-Zeichenoperationen und stellt
//   eine einfache Schnittstelle zum Zeichnen einzelner Spielfeldobjekte
//   bereit.
// ============================================================================

extern void placeItem(int y, int x, chtype symbol, enum ColorPairs color_pair);

// ============================================================================
//  Abfragefunktionen für Spielfeld-Größe
//
//  Diese Funktionen liefern die maximal gültigen Koordinaten des aktuell
//  verfügbaren Terminalfensters. Sie werden verwendet, um z. B. Kollisionen
//  mit dem Rand des Spielfelds zu erkennen.
// ============================================================================

// Liefert die letzte verwendbare Bildschirmzeile
extern int getLastRow(void);

// Liefert die letzte verwendbare Bildschirmspalte
extern int getLastCol(void);

#endif  // _BOARD_MODEL_H

