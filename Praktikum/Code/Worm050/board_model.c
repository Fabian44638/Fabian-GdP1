// ============================================================================
//  Modul: board_model.c - Fabian Ausmann
//  Zuständig für die Darstellung auf dem Spielfeld
//
//  Dieses Modul kapselt alle Funktionen, die direkt mit der Ausgabe
//  auf dem Bildschirm zu tun haben. Andere Module verwenden diese
//  Funktionen, um Spielfiguren oder Symbole zu zeichnen, ohne selbst
//  ncurses-Befehle aufrufen zu müssen.
//
//  Hinweis ab Aufgabenblatt 7:
//   - Am unteren Rand des Fensters werden einige Zeilen für eine
//     "Message Area" (Status-/Fehlermeldungen) reserviert.
//   - Diese Zeilen gehören nicht mehr zum eigentlichen Spielfeld,
//     d.h. der Wurm darf sie nicht betreten.
//   - getLastRow() berücksichtigt deshalb ROWS_RESERVED.
// ============================================================================

#include <curses.h>        // ncurses-Bibliothek für Terminalausgabe
#include "worm.h"          // Farbdefinitionen, ROWS_RESERVED, Konstanten
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
//
//  Diese Funktion stellt die zentrale Zeichenoperation für das Spielfeld dar.
//  Alle Module, die etwas auf dem „Board“ anzeigen wollen, sollten diese
//  Funktion verwenden und nicht direkt ncurses-Funktionen aufrufen.
// ============================================================================

void placeItem(int y, int x, chtype symbol, enum ColorPairs color_pair) {
    move(y, x);                          // Cursor zur Zielposition bewegen
    attron(COLOR_PAIR(color_pair));      // Gewünschte Farbe einschalten
    addch(symbol);                       // Zeichen ausgeben
    attroff(COLOR_PAIR(color_pair));     // Farbe wieder ausschalten
}

// ============================================================================
//  Liefert die letzte sichtbare Bildschirmzeile für das SPIELFELD
//
//  Die Konstante LINES wird von ncurses bereitgestellt und enthält die
//  aktuelle Anzahl der Terminal-Zeilen. Da die Zählung bei 0 beginnt,
//  wäre die letzte gültige Zeile eigentlich LINES - 1.
//
//  Ab Aufgabenblatt 7 werden jedoch die unteren ROWS_RESERVED Zeilen
//  für die Message Area reserviert. Diese Zeilen gehören nicht mehr
//  zum Spielfeld, sondern dienen z.B. zur Ausgabe von Statusmeldungen.
//
//  Daher:
//    letzte Spielfeldzeile = (LINES - 1) - ROWS_RESERVED
//
//  Beispiel:
//    LINES = 24, ROWS_RESERVED = 4
//      -> Zeilen 0..19  : Spielfeld (getLastRow() = 19)
//      -> Zeilen 20..23 : Message Area
// ============================================================================

int getLastRow(void) {
    return (LINES - 1) - ROWS_RESERVED;
}

// ============================================================================
//  Liefert die letzte sichtbare Bildschirmspalte
//
//  Analog zu getLastRow() liefert diese Funktion die größte gültige
//  Spalten-Nummer für das Spielfeld. COLS ist die von ncurses
//  bereitgestellte Anzahl der Spalten, gezählt ab 1.
//
//  Hier werden keine Spalten für eine Message Area reserviert, daher
//  bleibt die Definition wie bisher:
//
//    letzte Spalte = COLS - 1
//
//  Durch die Verwendung dieser Getter-Funktion bleibt das restliche
//  Programm unabhängig von direkten ncurses-Konstanten.
// ============================================================================

int getLastCol(void) {
    return COLS - 1;
}



