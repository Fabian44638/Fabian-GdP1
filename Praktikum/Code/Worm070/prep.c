// Worm070 - Aufgabenblatt 8
// ============================================================================
//  prep.c – Grundfunktionen zur Initialisierung und Beendigung der Curses-
//           Anwendung
//
//  Dieses Modul kapselt alle Funktionen, die notwendig sind, um die
//  Terminal-Darstellung über die ncurses-Bibliothek korrekt zu starten,
//  einzustellen und am Ende wieder aufzuräumen.
//
//  Typische Aufgaben:
//     - ncurses starten (initscr)
//     - Echo abschalten (keine automatische Zeichenausgabe bei Tastendruck)
//     - Cursor unsichtbar machen
//     - Tastaturmodus steuern (blockierend / nicht blockierend)
//     - ncurses sauber beenden (endwin)
//
//  Die hier definierten Funktionen werden einmal beim Start und einmal beim
//  Verlassen des Programms aufgerufen. Dadurch ist der Umgang mit ncurses an
//  einer zentralen Stelle kapselt und klar strukturiert.
// ============================================================================

#include <curses.h>
#include "prep.h"


// ============================================================================
//  initializeCursesApplication()
//  -----------------------------
//
//  Aufgabe:
//     Initialisiert die ncurses-Bibliothek und setzt die grundlegenden
//     Terminaleinstellungen, damit das Worm-Spiel korrekt ausgeführt werden
//     kann.
//
//  Wichtige Schritte im Überblick:
//
//     initscr()       Initialisiert das ncurses-System.
//                      Erst danach stehen globale Werte wie COLS und LINES
//                      zur Verfügung.
//
//     noecho()        Verhindert, dass Tastendrücke automatisch auf dem
//                      Bildschirm erscheinen.
//
//     cbreak()        Sorgt dafür, dass Eingaben sofort (ohne Enter) an das
//                      Programm weitergereicht werden.
//
//     nonl()          Verhindert eine automatische Umsetzung der Enter-Taste
//                      in ein Newline-Zeichen.
//
//     keypad()        Aktiviert Sondertasten wie Pfeiltasten.
//
//     curs_set(0)     Versteckt den sichtbaren Textcursor.
//
//     nodelay(TRUE)   getch() liefert sofort zurück (nicht blockierend).
//                      Dies ist wichtig für die kontinuierliche Spiellogik.
//
//  Hinweis:
//     Diese Funktion wird zu Beginn des Programms in main() aufgerufen.
// ============================================================================

void initializeCursesApplication() {

    initscr();   // ncurses-System initialisieren
                 // setzt globale Variablen wie stdscr, LINES, COLS

    noecho();    // Tastendrücke nicht automatisch anzeigen
    cbreak();    // Eingaben sofort verarbeiten (nicht puffern)
    nonl();      // Enter-Taste nicht automatisch in '\n' umwandeln

    keypad(stdscr, TRUE);  // Pfeiltasten und Funktionstasten aktivieren
    curs_set(0);           // Cursor unsichtbar machen

    // Standard: Spiel läuft NICHT blockierend.
    // Nur Dialoge verwenden blockierendes getch().
    nodelay(stdscr, TRUE);
}


// ============================================================================
//  cleanupCursesApp()
//  ------------------
//
//  Aufgabe:
//     Stellt das Terminal nach dem Spielende wieder in seinen normalen Zustand
//     zurück und beendet die ncurses-Steuerung korrekt.
//
//  Schritte:
//
//     standend()   Setzt alle Attribute und Farbdarstellungen zurück.
//     refresh()    Aktualisiert die sichtbare Darstellung.
//
//     curs_set(1)  Schaltet den Cursor wieder sichtbar.
//
//     endwin()     Beendet den ncurses-Modus vollständig.
//                   Danach kann das Terminal wieder ganz normal verwendet
//                   werden.
//
//  Wichtig:
//     Diese Funktion MUSS immer aufgerufen werden – sowohl bei normalem Ende
//     als auch bei Programmabbruch, damit keine kaputten Terminalzustände
//     zurückbleiben.
// ============================================================================

void cleanupCursesApp(void)
{
    standend();   // Farb- und Attributzustände zurücksetzen
    refresh();    // Änderungen sichtbar machen

    curs_set(1);  // Cursor wieder aktivieren
    endwin();     // ncurses sauber beenden
}






