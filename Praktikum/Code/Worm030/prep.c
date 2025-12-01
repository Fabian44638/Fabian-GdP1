// ============================================================================
//  Modul: prep.c - Fabian Ausmann
//
//  Dieses Modul ist für die Initialisierung und das ordnungsgemäße
//  Beenden der ncurses-Umgebung zuständig. Ohne diese Funktionen wäre
//  keine sichere Terminalausgabe im Curses-Modus möglich.
//
//  Alle notwendigen Grundeinstellungen für das Spiel werden hier
//  zentral vorgenommen:
//
//   - Aktivieren des Curses-Modus
//   - Abschalten der Zeichenecho-Ausgabe
//   - Aktivieren direkter Tasteneingabe
//   - Konfiguration der Tastatursteuerung
//   - Verbergen des Cursors
//   - Steuerung, ob getch() blockierend arbeitet
//
//  Die zugehörigen Funktionsdeklarationen befinden sich in:
//      prep.h
// ============================================================================

#include <curses.h>   // ncurses-Bibliothek für Terminalsteuerung
#include "prep.h"    // Öffentliche Schnittstelle dieses Moduls

// ============================================================================
//  Initialisiert die ncurses-Umgebung und setzt alle benötigten Einstellungen
//
//  Aufgabe dieser Funktion:
//   - Startet den Curses-Modus für die Terminalausgabe
//   - Konfiguriert die Ein- und Ausgabeeigenschaften des Terminals
//   - Bereitet das System auf die interaktive Steuerung des Spiels vor
//
//  Diese Funktion wird einmal zu Beginn des Programms aufgerufen.
// ============================================================================

void initializeCursesApplication(void) {

    initscr();               // Initialisiert den ncurses-Modus und das Hauptfenster

    noecho();                // Unterdrückt die automatische Ausgabe
                              // von gedrückten Tasten im Terminal

    cbreak();                // Tasteneingaben werden sofort verarbeitet,
                              // ohne dass die Eingabetaste benötigt wird

    nonl();                  // Unterdrückt den automatischen Zeilenumbruch
                              // bei Eingabe eines Newline-Zeichens

    keypad(stdscr, TRUE);   // Aktiviert die Erkennung spezieller Tasten
                              // wie Pfeiltasten oder Funktionstasten

    curs_set(0);            // Versteckt den blinkenden Textcursor

    nodelay(stdscr, TRUE);  // Setzt getch() auf nicht-blockierenden Modus:
                              // Das Spiel läuft weiter, auch wenn keine
                              // Taste gedrückt wurde
}

// ============================================================================
//  Beendet die ncurses-Umgebung und stellt das Terminal wieder her
//
//  Aufgabe dieser Funktion:
//   - Entfernt Curses-Attribute
//   - Stellt Anzeige und Cursor-Einstellungen zurück
//   - Beendet den ncurses-Modus sauber
//
//  Diese Funktion wird beim Spielende aufgerufen, damit das Terminal
//  wieder normal benutzbar ist.
// ============================================================================

void cleanupCursesApp(void) {

    standend();   // Setzt alle Zeicheneigenschaften (Farben, Attribute) zurück
    refresh();    // Erzwingt letzte Bildschirmausgabe

    curs_set(1);  // Aktiviert den Textcursor wieder

    endwin();     // Beendet den ncurses-Modus und kehrt zum normalen
                  // Terminalbetrieb zurück
}

