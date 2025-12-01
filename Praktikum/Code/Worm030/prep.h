// ============================================================================
//  Modul-Header: prep.h - Fabian Ausmann
//
//  Dieser Header definiert die öffentliche Schnittstelle des Moduls „prep“.
//  Das Modul ist für die Initialisierung und das ordnungsgemäße Beenden
//  der ncurses-Umgebung zuständig.
//
//  In dieser Datei stehen ausschließlich die Funktionsdeklarationen,
//  die von anderen Modulen benötigt werden, um:
//
//   - den Curses-Modus zu starten
//   - das Terminal nach Spielende korrekt zurückzusetzen
//
//  Die eigentliche Implementierung der Funktionen befindet sich in:
//      prep.c
// ============================================================================

#ifndef _PREP_H
#define _PREP_H

// ============================================================================
//  Initialisiert die ncurses-Bibliothek und konfiguriert alle notwendigen
//  Terminal-Einstellungen für den Start des Spiels.
// ============================================================================

extern void initializeCursesApplication(void);

// ============================================================================
//  Beendet die ncurses-Umgebung und stellt den normalen Terminalzustand
//  nach dem Spielende wieder her.
// ============================================================================

extern void cleanupCursesApp(void);

#endif  // _PREP_H

