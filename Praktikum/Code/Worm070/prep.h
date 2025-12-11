// Worm070 - Aufgabenblatt 8
// ============================================================================
//  prep.h – Headerdatei für die Initialisierung und das Beenden der
//           ncurses-Anwendung
//
//  Zweck dieser Datei:
//     Diese Headerdatei stellt die Funktionsprototypen bereit, die benötigt
//     werden, um die Terminalsteuerung über die ncurses-Bibliothek korrekt zu
//     starten und am Ende des Programms wieder zu beenden.
//
//  Hintergrund:
//     Das Worm-Spiel verwendet die Bibliothek ncurses für die Darstellung.
//     Damit Eingaben und Ausgaben korrekt verarbeitet werden, müssen bestimmte
//     Terminaleinstellungen gesetzt werden (nicht blockierende Eingabe,
//     unsichtbarer Cursor, Sondertasten aktivieren usw.).
//     Diese Konfigurationen werden in initializeCursesApplication() ausgeführt.
//     cleanupCursesApp() macht diese Änderungen rückgängig.
//
//  Typische Anwendung in main():
//
//         initializeCursesApplication();
//         ... Spielablauf ...
//         cleanupCursesApp();
//
//  Dadurch ist sichergestellt, dass das Terminal nach Programmende wieder in
//  seinem ursprünglichen Zustand ist.
// ============================================================================

#ifndef _PREP_H
#define _PREP_H

// ---------------------------------------------------------------------------
// initializeCursesApplication()
// ---------------------------------------------------------------------------
// Aufgabe:
//     Startet ncurses und richtet alle benötigten Terminaleinstellungen ein,
//     damit das Spielfeld korrekt dargestellt wird und Tastatureingaben
//     zuverlässig funktionieren.
//
// Wichtige Effekte:
//     - ncurses initialisieren
//     - Eingabe puffert nicht (cbreak)
//     - Tastendrücke werden nicht angezeigt (noecho)
//     - Cursor wird unsichtbar
//     - Pfeiltasten werden freigeschaltet
//     - getch() standardmäßig nicht blockierend
//
// Diese Funktion wird zu Beginn des Programms in main() aufgerufen.
// ---------------------------------------------------------------------------
extern void initializeCursesApplication();


// ---------------------------------------------------------------------------
// cleanupCursesApp()
// ---------------------------------------------------------------------------
// Aufgabe:
//     Stellt das Terminal nach Spielende wieder vollständig her,
//     hebt alle ncurses-Konfigurationen auf und beendet den ncurses-Modus.
//
// Wichtige Effekte:
//     - Cursor wieder sichtbar schalten
//     - Textattribute zurücksetzen
//     - ncurses vollständig beenden (endwin)
//
// Diese Funktion muss immer aufgerufen werden – auch im Fehlerfall.
// ---------------------------------------------------------------------------
extern void cleanupCursesApp(void);

#endif  // _PREP_H






