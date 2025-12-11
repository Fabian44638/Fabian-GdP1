// Worm070 - Aufgabenblatt 8
// ============================================================================
//  Modul: board_model.c  –  Spielfeldmodell für Worm
//
//  Aufgaben dieses Moduls:
//
//   - Prüfen, ob das Terminalfenster groß genug ist            (initializeBoard)
//   - Spielfeld mit freien Zellen, Barrieren und Futter füllen (initializeLevel)
//   - Inhalt einzelner Zellen setzen und abfragen              (placeItem, getContentAt)
//   - Verwalten der Anzahl an verbliebenen Futterstellen
//
//  Wichtige Begriffe:
//
//   * Board / Spielfeld:
//       Ein rechteckiger Bereich im Terminal, in dem sich der Wurm bewegt.
//       Dieser Bereich wird in der Struktur struct board als 2D-Array
//       (cells[y][x]) gespeichert.
//
//   * Message Area:
//       Unten im Fenster sind einige Zeilen als Ausgabe-Bereich für
//       Status- und Game-Over-Meldungen reserviert.
//       Diese Zeilen gehören NICHT zum Board und werden hier nicht
//       in der Board-Struktur verwaltet.
// ============================================================================

#include <curses.h>
#include <stdio.h>

#include "worm.h"
#include "board_model.h"
#include "messages.h"

// ============================================================================
//  initializeBoard
// ============================================================================

enum ResCodes initializeBoard(struct board* aboard) {
    if (COLS < MIN_NUMBER_OF_COLS ||
        LINES < MIN_NUMBER_OF_ROWS + ROWS_RESERVED) {

        char buf[100];
        sprintf(
            buf,
            "Das Fenster ist zu klein: wir brauchen %dx%d",
            MIN_NUMBER_OF_COLS,
            MIN_NUMBER_OF_ROWS + ROWS_RESERVED
        );

        showDialog(buf, "Bitte eine Taste druecken");
        return RES_FAILED;
    }

    aboard->last_row = MIN_NUMBER_OF_ROWS - 1;
    aboard->last_col = MIN_NUMBER_OF_COLS - 1;

    return RES_OK;
}

// ============================================================================
//  placeItem – logischen Inhalt setzen UND Symbol zeichnen
// ============================================================================

void placeItem(struct board* aboard,
               int y, int x,
               enum BoardCodes board_code,
               chtype symbol,
               enum ColorPairs color_pair)
{
    if (y >= 0 && y <= aboard->last_row &&
        x >= 0 && x <= aboard->last_col) {
        aboard->cells[y][x] = board_code;
    }

    move(y, x);
    attron(COLOR_PAIR(color_pair));
    addch(symbol);
    attroff(COLOR_PAIR(color_pair));
}

// ============================================================================
//  initializeLevel – komplettes Level aufbauen
// ============================================================================

enum ResCodes initializeLevel(struct board* aboard) {
    int x, y;

    if (initializeBoard(aboard) != RES_OK) {
        return RES_FAILED;
    }

    // ------------------------------------------------------------------------
    // 1. Alle Zellen als freie Felder initialisieren
    // ------------------------------------------------------------------------
    for (y = 0; y <= aboard->last_row; y++) {
        for (x = 0; x <= aboard->last_col; x++) {
            placeItem(
                aboard,
                y, x,
                BC_FREE_CELL,
                SYMBOL_FREE_CELL,
                COLP_FREE_CELL
            );
        }
    }

    // ------------------------------------------------------------------------
    // 2. Untere Barriere (Trennlinie zur Message Area)
    // ------------------------------------------------------------------------
    y = aboard->last_row + 1;
    for (x = 0; x < MIN_NUMBER_OF_COLS; x++) {
        move(y, x);
        attron(COLOR_PAIR(COLP_BARRIER));
        addch(SYMBOL_BARRIER);
        attroff(COLOR_PAIR(COLP_BARRIER));
    }

    // ------------------------------------------------------------------------
    // 3. Rechte Begrenzungslinie des Boards
    // ------------------------------------------------------------------------
    for (y = 0; y <= aboard->last_row; y++) {
        placeItem(
            aboard,
            y, aboard->last_col,
            BC_BARRIER,
            SYMBOL_BARRIER,
            COLP_BARRIER
        );
    }

    // ------------------------------------------------------------------------
    // 4. Vertikale Barriere im linken Drittel
    // ------------------------------------------------------------------------
    int left_x = aboard->last_col / 4;
    for (y = 5; y <= 15; y++) {
        placeItem(
            aboard,
            y, left_x,
            BC_BARRIER,
            SYMBOL_BARRIER,
            COLP_BARRIER
        );
    }

    // ------------------------------------------------------------------------
    // 5. Vertikale Barriere im rechten Drittel
    // ------------------------------------------------------------------------
    int right_x = (aboard->last_col * 3) / 4;
    for (y = 8; y <= 20; y++) {
        placeItem(
            aboard,
            y, right_x,
            BC_BARRIER,
            SYMBOL_BARRIER,
            COLP_BARRIER
        );
    }

    // ------------------------------------------------------------------------
    // 6. Futter verteilen (insgesamt 10 Stück)
    // ------------------------------------------------------------------------

    // Typ 1
    placeItem(aboard,  3,  3, BC_FOOD_1, SYMBOL_FOOD_1, COLP_FOOD_1);
    placeItem(aboard, 10, 10, BC_FOOD_1, SYMBOL_FOOD_1, COLP_FOOD_1);

    // Typ 2
    placeItem(aboard,  5, 20, BC_FOOD_2, SYMBOL_FOOD_2, COLP_FOOD_2);
    placeItem(aboard, 15, 25, BC_FOOD_2, SYMBOL_FOOD_2, COLP_FOOD_2);

    // Hier war dein Fehler: kaputte Zeile mit "BC_F_"
    placeItem(aboard,  8, 40, BC_FOOD_2, SYMBOL_FOOD_2, COLP_FOOD_2);

    placeItem(aboard, 18, 35, BC_FOOD_2, SYMBOL_FOOD_2, COLP_FOOD_2);

    // Typ 3
    placeItem(aboard,  4, 50, BC_FOOD_3, SYMBOL_FOOD_3, COLP_FOOD_3);
    placeItem(aboard, 12, 55, BC_FOOD_3, SYMBOL_FOOD_3, COLP_FOOD_3);
    placeItem(aboard, 20, 45, BC_FOOD_3, SYMBOL_FOOD_3, COLP_FOOD_3);
    placeItem(aboard,  7, 30, BC_FOOD_3, SYMBOL_FOOD_3, COLP_FOOD_3);

    aboard->food_items = 10;

    return RES_OK;
}

// ============================================================================
//  Getter / Setter
// ============================================================================

int getNumberOfFoodItems(struct board* aboard) {
    return aboard->food_items;
}

enum BoardCodes getContentAt(struct board* aboard, struct pos position) {
    if (position.y < 0 || position.y > aboard->last_row ||
        position.x < 0 || position.x > aboard->last_col) {
        return BC_BARRIER;
    }
    return aboard->cells[position.y][position.x];
}

int getLastRowOnBoard(struct board* aboard) {
    return aboard->last_row;
}

int getLastColOnBoard(struct board* aboard) {
    return aboard->last_col;
}

void decrementNumberOfFoodItems(struct board* aboard) {
    if (aboard->food_items > 0) {
        aboard->food_items--;
    }
}

void setNumberOfFoodItems(struct board* aboard, int n) {
    aboard->food_items = n;
}






