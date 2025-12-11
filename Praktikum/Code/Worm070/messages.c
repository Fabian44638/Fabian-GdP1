// Worm070 - Aufgabenblatt 8
// =======================================================================
#include <curses.h>

#include "worm.h"
#include "board_model.h"
#include "worm_model.h"
#include "messages.h"

// ---------------------------------------------------------------------------
// Eine komplette Zeile im Nachrichtenbereich löschen
// ---------------------------------------------------------------------------
void clearLineInMessageArea(int row) {
    move(row, 0);
    for (int i = 0; i < COLS; i++) {
        addch(' ');
    }
}

// ---------------------------------------------------------------------------
// Trennlinie am unteren Rand des Spielfelds zeichnen
// ---------------------------------------------------------------------------
void showBorderLine(void) {
    int pos = LINES - ROWS_RESERVED;

    for (int i = 0; i < COLS; i++) {
        move(pos, i);
        attron(COLOR_PAIR(COLP_BARRIER));
        addch(SYMBOL_BARRIER);
        attroff(COLOR_PAIR(COLP_BARRIER));
    }
}

// ---------------------------------------------------------------------------
// Statusanzeige im Message-Bereich
//
// Problem vorher:
//  - Wenn sich die Anzahl der Futterbrocken oder die Länge des Wurms
//    von zweistellig auf einstellig ändert, bleiben alte Zeichen stehen.
// Lösung:
//  - Zeile vor der Ausgabe komplett löschen.
// ---------------------------------------------------------------------------
void showStatus(struct board* aboard, struct worm* aworm) {

    int line = LINES - ROWS_RESERVED + 2;

    struct pos head = getWormHeadPos(aworm);
    int len        = getWormLength(aworm);
    int food_left  = getNumberOfFoodItems(aboard);

    // Zeile zuerst vollständig löschen, damit keine alten Zeichen übrig bleiben
    clearLineInMessageArea(line);

    // Status neu ausgeben
    mvprintw(
        line,
        1,
        "Position des Wurms: y = %3d   x = %3d   Segmente: %3d   Futterbrocken: %2d   ",
        head.y,
        head.x,
        len,
        food_left
    );
}

// ---------------------------------------------------------------------------
// Dialog im Messagebereich anzeigen
// ---------------------------------------------------------------------------
int showDialog(char* prompt1, char* prompt2) {

    int l1 = LINES - ROWS_RESERVED + 1;
    int l2 = LINES - ROWS_RESERVED + 2;
    int l3 = LINES - ROWS_RESERVED + 3;

    // alte Meldungen löschen
    clearLineInMessageArea(l1);
    clearLineInMessageArea(l2);
    clearLineInMessageArea(l3);

    mvprintw(l2, 1, "%s", prompt1);
    if (prompt2)
        mvprintw(l3, 1, "%s", prompt2);

    refresh();

    nodelay(stdscr, FALSE);
    int ch = getch();     // Taste abwarten
    nodelay(stdscr, TRUE);

    clearLineInMessageArea(l1);
    clearLineInMessageArea(l2);
    clearLineInMessageArea(l3);
    refresh();

    return ch;
}







