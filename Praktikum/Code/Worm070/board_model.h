// Worm070 - Aufgabenblatt 8
// ============================================================================

#ifndef _BOARD_MODEL_H
#define _BOARD_MODEL_H

#include <curses.h>
#include "worm.h"

// ============================================================================
//  BoardCodes – logische Inhalte einer Spielfeldzelle
//
//  Idee:
//    Jede Zelle des Spielfeldes (struct board) bekommt genau einen dieser
//    Kennwerte zugeordnet. Die Spiellogik kann dadurch leicht erkennen,
//    was sich an einer bestimmten Position befindet, ohne das tatsächlich
//    gezeichnete Ausgabesymbol auswerten zu müssen.
//
//  Typische Verwendung:
//    - Beim Aufbau des Spielfelds wird für jede Zelle ein BoardCode gesetzt.
//    - Beim Bewegen des Wurms wird geprüft, welchen BoardCode die nächste
//      Zielzelle hat (frei, Futter, Barriere usw.).
// ============================================================================

enum BoardCodes {
    BC_FREE_CELL,    // Zelle ist frei (kein Wurm, kein Futter, keine Barriere)
    BC_USED_BY_WORM, // Zelle wird momentan vom Wurm belegt
    BC_FOOD_1,       // Futter Typ 1; beim Einsammeln gibt es einen Bonus vom Typ 1
    BC_FOOD_2,       // Futter Typ 2; beim Einsammeln gibt es einen Bonus vom Typ 2
    BC_FOOD_3,       // Futter Typ 3; beim Einsammeln gibt es einen Bonus vom Typ 3
    BC_BARRIER       // Barriere; eine Kollision entspricht einem Crash oder Spielende
};

// ============================================================================
//  struct pos – Position auf dem Spielfeld
//
//  Beschreibung:
//    Eine einfache Struktur zur Speicherung von Koordinaten innerhalb des
//    Spielfelds. y steht für die Zeile (vertikale Position), x für die Spalte
//    (horizontale Position).
//
//  Verwendung:
//    Diese Struktur wird sowohl vom Wurm-Modell als auch vom Board-Modell
//    benutzt, um Positionen auf dem Spielfeld eindeutig zu beschreiben.
// ============================================================================

struct pos {
    int y;   // y-Koordinate (Zeilennummer auf dem Spielfeld, von oben nach unten)
    int x;   // x-Koordinate (Spaltennummer auf dem Spielfeld, von links nach rechts)
};

// ============================================================================
//  struct board – Repräsentation des Spielfeldes
//
//  Felder:
//
//  last_row / last_col:
//    - geben den jeweils größten gültigen Index an, der benutzt werden darf
//      (0 bis last_row beziehungsweise 0 bis last_col)
//    - orientieren sich an MIN_NUMBER_OF_ROWS und MIN_NUMBER_OF_COLS aus worm.h
//    - stellen also nicht die Anzahl, sondern den maximalen Index dar
//
//  cells:
//    - zweidimensionales Array mit BoardCodes
//    - für jede Zelle wird gespeichert, ob sie frei ist, Futter enthält,
//      vom Wurm belegt ist oder eine Barriere darstellt
//    - dient als logische Repräsentation des Spielfeldzustands unabhängig
//      von der tatsächlichen Ausgabe mit curses
//
//  food_items:
//    - Anzahl der verbliebenen Futterobjekte im aktuellen Level
//    - wird zum Beispiel reduziert, wenn der Wurm ein Futterfeld betritt
//      und dieses Futter „gefressen“ hat
// ============================================================================

struct board {
    int last_row; // letzte nutzbare Zeile auf dem Board (gültiger Indexbereich 0 bis last_row)
    int last_col; // letzte nutzbare Spalte auf dem Board (gültiger Indexbereich 0 bis last_col)

    enum BoardCodes cells[MIN_NUMBER_OF_ROWS][MIN_NUMBER_OF_COLS];
    // Logische Inhalte aller Spielfeldzellen in einem festen zweidimensionalen Array

    int food_items; // Anzahl der noch vorhandenen Futterstücke im aktuellen Level
};

// ============================================================================
//  Initialisierung des Boards
// ============================================================================

// Prüft die Fenstergröße, setzt last_row und last_col und bereitet das Board vor.
// Es wird überprüft, ob das Terminalfenster groß genug ist, um das Spielfeld
// und die Message Area darzustellen.
// Liefert RES_OK bei Erfolg, sonst RES_FAILED.
extern enum ResCodes initializeBoard(struct board* aboard);

// Initialisiert ein komplettes Level:
//  - ruft initializeBoard auf, um Größe und Grenzen des Spielfelds zu prüfen
//  - füllt das Board mit freien Zellen, Barrieren und Futter an festen Positionen
//  - setzt die Variable food_items passend zur Anzahl der Futterstellen
extern enum ResCodes initializeLevel(struct board* aboard);

// ============================================================================
//  placeItem – logischen Inhalt setzen und Zeichen ausgeben
//
//  Parameter:
//    aboard      : Zeiger auf das Spielfeld (Board-Struktur)
//    y, x        : Zielposition auf dem Spielfeld (Zeile, Spalte)
//    board_code  : logischer Inhalt (BoardCodes), der im Array cells
//                  abgelegt wird, zum Beispiel BC_FREE_CELL oder BC_BARRIER
//    symbol      : sichtbares Zeichen, das mit ncurses an dieser Stelle
//                  gezeichnet wird
//    color_pair  : Farbnummer für die Darstellung (ncurses-Farbpaar)
//
//  Wirkung:
//    - schreibt board_code an die betreffende Position in die Datenstruktur
//    - zeichnet symbol in der gewünschten Farbe an die entsprechende Stelle
//      im Terminalfenster
//    Dadurch bleiben interne Repräsentation und grafische Ausgabe im
//    Terminal immer konsistent.
// ============================================================================

extern void placeItem(struct board* aboard,
                      int y, int x,
                      enum BoardCodes board_code,
                      chtype symbol,
                      enum ColorPairs color_pair);

// ============================================================================
//  Getter-Funktionen für Board-Informationen
// ============================================================================

// Liefert die aktuelle Anzahl der Futterstücke auf dem Board.
// Diese Information kann zum Beispiel für Statusanzeigen oder zur
// Entscheidung verwendet werden, ob das Level bereits „leer gefressen“ ist.
extern int getNumberOfFoodItems(struct board* aboard);

// Liefert den BoardCode an einer bestimmten Position.
//
// Besonderheit bei ungültigen Positionen:
//  - Ist die angegebene Position außerhalb des gültigen Spielfeldbereichs,
//    wird BC_BARRIER zurückgegeben.
//  - Dadurch wird der Bereich außerhalb des Boards von der Spiellogik so
//    behandelt, als wäre dort eine undurchdringliche Wand.
extern enum BoardCodes getContentAt(struct board* aboard, struct pos position);

// Liefert die letzte nutzbare Zeile des Boards (Index, nicht Anzahl).
// Damit lässt sich zum Beispiel der gültige Bereich für Schleifen bestimmen.
extern int getLastRowOnBoard(struct board* aboard);

// Liefert die letzte nutzbare Spalte des Boards (Index, nicht Anzahl).
// Auch hier handelt es sich um den maximalen gültigen Spaltenindex.
extern int getLastColOnBoard(struct board* aboard);

// ============================================================================
//  Setter-Funktionen für Board-Informationen
// ============================================================================

// Verringert die Anzahl der Futterstücke um 1, sofern der Wert größer 0 ist.
// Wird typischerweise aufgerufen, wenn der Wurm ein Futterfeld betritt.
// Eine Unterlaufprüfung verhindert, dass der Zähler einen negativen Wert annimmt.
extern void decrementNumberOfFoodItems(struct board* aboard);

// Setzt die Anzahl der Futterstücke explizit auf n.
// Diese Funktion eignet sich zum Beispiel für die Initialisierung oder
// für Tests, bei denen bestimmte Futterkonstellationen nachgestellt werden.
extern void setNumberOfFoodItems(struct board* aboard, int n);

#endif  // #define _BOARD_MODEL_H






