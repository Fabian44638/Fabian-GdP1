// ============================================================================
//  Modul: board_model - Fabian Ausmann
//  Zuständig für die Darstellung auf dem Spielfeld
//
//  Dieses Modul kapselt alle Funktionen, die direkt mit der Ausgabe
//  auf dem Bildschirm zu tun haben. Andere Module verwenden diese
//  Funktionen, um Spielfiguren oder Symbole zu zeichnen, ohne selbst
//  ncurses-Befehle aufrufen zu müssen.
// ============================================================================

#include <curses.h>   // ncurses-Bibliothek für Terminalausgabe
#include "worm.h"    // Farbdefinitionen und gemeinsame Typen
#include "board_model.h"   // Öffentliche Schnittstelle dieses Moduls

// ============================================================================
//  Zeichnet ein einzelnes Symbol an die Position (y, x)
//
//  Parameter:
//   - y          : Zeilenposition auf dem Bildschirm
//   - x          : Spaltenposition auf dem Bildschirm
//   - symbol     : Zeichen, das ausgegeben werden soll
//   - color_pair : Farbnummer (Color-Pair) für die Ausgabe
//
//  Ablauf:
//   1. Cursor an gewünschte Position bewegen
//   2. Farb-Attribut aktivieren
//   3. Zeichen ausgeben
//   4. Farb-Attribut wieder deaktivieren
// ============================================================================

void placeItem(int y, int x, chtype symbol, enum ColorPairs color_pair) {
    move(y, x);                         // Cursor zur Zielposition bewegen
    attron(COLOR_PAIR(color_pair));    // Gewünschte Farbe einschalten
    addch(symbol);                     // Zeichen ausgeben
    attroff(COLOR_PAIR(color_pair));   // Farbe wieder ausschalten
}

// ============================================================================
//  Liefert die letzte sichtbare Bildschirmzeile
//
//  Die Konstante LINES wird von ncurses bereitgestellt und enthält die
//  aktuelle Anzahl der Terminal-Zeilen. Da die Zählung bei 0 beginnt,
//  ist die letzte gültige Zeile LINES - 1.
// ============================================================================

int getLastRow(void) {
    return LINES - 1;
}

// ============================================================================
//  Liefert die letzte sichtbare Bildschirmspalte
//
//  Analog zu getLastRow(). Die ncurses-Konstante COLS gibt die Breite
//  des Terminals an.
// ============================================================================

int getLastCol(void) {
    return COLS - 1;
}


