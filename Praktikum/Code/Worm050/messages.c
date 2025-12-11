// ============================================================================
//  Modul: messages.c - Fabian Ausmann
//
//  Anzeige von Meldungen und einfachen Dialogen in der Message Area
//
//  Aufgabe der Message Area:
//   - Am unteren Rand des Terminals (ROWS_RESERVED Zeilen) werden
//     Status- und Hinweistexte angezeigt.
//   - Diese Zeilen gehören NICHT mehr zum eigentlichen Spielfeld und
//     werden nur für Ausgaben verwendet (z.B. Kopfposition, Dialoge).
// ============================================================================

#include <curses.h>

#include "worm.h"
#include "board_model.h"
#include "worm_model.h"
#include "messages.h"

// ============================================================================
//  clearLineInMessageArea
//  Löscht eine komplette Zeile innerhalb der Message Area.
//
//  Parameter:
//    row : absolute Zeilennummer im Terminalfenster, die gelöscht
//          werden soll.
//
//  Vorgehen:
//    - Cursor an den Anfang der Zeile setzen
//    - über die ganze Breite (COLS) Leerzeichen ausgeben
// ============================================================================

void clearLineInMessageArea(int row) {
    int i;

    move(row, 0);                 // Cursor an den Zeilenanfang setzen
    for (i = 1; i <= COLS; i++) { // gesamte Zeile mit Leerzeichen überschreiben
        addch(' ');
    }
}

// ============================================================================
//  showBorderLine
//  Zeichnet die Trennlinie zwischen Spielfeld und Message Area.
//
//  Die Trennlinie liegt genau an der ersten Zeile der Message Area:
//    pos_line0 = LINES - ROWS_RESERVED
//
//  Diese Zeile wird komplett mit SYMBOL_BARRIER (z.B. '#') in der
//  Farbe COLP_BARRIER gezeichnet.
// ============================================================================

void showBorderLine() {
    int pos_line0 = LINES - ROWS_RESERVED;
    int i;

    for (i = 0; i < COLS; i++) {
        move(pos_line0, i);
        attron(COLOR_PAIR(COLP_BARRIER));
        addch(SYMBOL_BARRIER);
        attroff(COLOR_PAIR(COLP_BARRIER));
    }
}

// ============================================================================
//  showStatus
//  Zeigt Statusinformationen über den Wurm in der Message Area an.
//
//  Parameter:
//    aworm : Zeiger auf den Wurm, dessen Status angezeigt werden soll.
//
//  Aktuell wird hier die Kopfposition des Wurms ausgegeben:
//    "Wurm ist an Position: y=  .. x=  .."
//
//  Die Statuszeile wird zwei Zeilen unterhalb der Trennlinie gezeichnet:
//
//    Zeile LINES - ROWS_RESERVED + 0 : (Trennlinie)
//    Zeile LINES - ROWS_RESERVED + 1 : (evtl. Dialogüberschrift)
//    Zeile LINES - ROWS_RESERVED + 2 : Statuszeile (hier)
// ============================================================================

void showStatus(struct worm* aworm) {
    int pos_line2 = LINES - ROWS_RESERVED + 2;

    // Kopfposition über das Wurm-Modul erfragen
    struct pos headpos = getWormHeadPos(aworm);

    mvprintw(pos_line2, 1,
             "Der Wurm ist an der Position: y =%3d   x =%3d",
             headpos.y, headpos.x);
}

// ============================================================================
//  showDialog
//  Zeigt einen einfachen Textdialog in der Message Area an und wartet
//  auf eine Tasteneingabe.
//
//  Parameter:
//    prompt1 : Text für die zweite Zeile der Message Area
//    prompt2 : Text für die dritte Zeile der Message Area (optional,
//              darf NULL sein)
//
//  Zeilenaufteilung der Message Area:
//
//    pos_line0 = LINES - ROWS_RESERVED
//      -> Trennlinie (wird in showBorderLine() gezeichnet)
//
//    pos_line1 = LINES - ROWS_RESERVED + 1
//    pos_line2 = LINES - ROWS_RESERVED + 2
//    pos_line3 = LINES - ROWS_RESERVED + 3
//
//  Ablauf:
//    1. Prüfen, ob prompt1 != NULL ist (ohne Text kein Dialog)
//    2. Alle drei Textzeilen löschen
//    3. prompt1 (und ggf. prompt2) ausgeben
//    4. getch() blockierend auf Taste warten
//    5. Message-Zeilen wieder löschen
//    6. gedrücktes Zeichen als Rückgabewert liefern
//
//  Rückgabewert:
//    - das Zeichen, das der Benutzer gedrückt hat
//    - RES_FAILED, falls prompt1 == NULL
// ============================================================================

int showDialog(char* prompt1, char* prompt2) {
    int pos_line1 = LINES - ROWS_RESERVED + 1;
    int pos_line2 = LINES - ROWS_RESERVED + 2;
    int pos_line3 = LINES - ROWS_RESERVED + 3;

    int ch;

    // Ohne Text in prompt1 ergibt ein Dialog keinen Sinn
    if (prompt1 == NULL) {
        return RES_FAILED;
    }

    // Zeilen in der Message Area löschen
    clearLineInMessageArea(pos_line1);
    clearLineInMessageArea(pos_line2);
    clearLineInMessageArea(pos_line3);

    // Meldung ausgeben
    mvprintw(pos_line2, 1, "%s", prompt1);
    if (prompt2 != NULL) {
        mvprintw(pos_line3, 1, "%s", prompt2);
    }
    refresh();

    // Eingabe blockierend abwarten
    nodelay(stdscr, FALSE);
    ch = getch();   // Warten, bis der Benutzer eine Taste drückt
    nodelay(stdscr, TRUE);

    // Zeilen in der Message Area wieder löschen
    clearLineInMessageArea(pos_line1);
    clearLineInMessageArea(pos_line2);
    clearLineInMessageArea(pos_line3);

    // Änderungen anzeigen
    refresh();

    // Geklickte Taste zurückgeben
    return ch;
}

