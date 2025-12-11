// Worm070 - Aufgabenblatt 8
// ============================================================================

#ifndef _MESSAGES_H
#define _MESSAGES_H

#include "worm.h"
#include "worm_model.h"
#include "board_model.h"

// ============================================================================
//
//  Dieses Header-Modul stellt alle Funktionen bereit, die für die Ausgabe im
//  unteren Nachrichtenbereich des Spiels notwendig sind.
//
//  Der Nachrichtenbereich („Message Area“):
//     - befindet sich am unteren Rand des Terminalfensters
//     - besteht aus mehreren reservierten Zeilen (ROWS_RESERVED)
//     - wird NICHT als Teil des Spielfeldes betrachtet
//
//  Typische Anwendungsfälle:
//     - Statusmeldungen während des Spiels (Position des Wurmkopfes,
//       Anzahl verbleibender Futterobjekte, Wurmlänge usw.)
//     - Dialoge wie Fehlermeldungen, Spielstart, Game Over
//     - temporäre Ausgaben, die nach Tastendruck wieder verschwinden
// ============================================================================


// ---------------------------------------------------------------------------
//  clearLineInMessageArea(row)
//  ---------------------------
//
//  Zweck:
//     Löscht den Inhalt einer gesamten Zeile im Nachrichtenbereich.
//     Dies dient dazu, veraltete Meldungen zu entfernen.
//
//  Parameter:
//     row – absolute Zeilennummer im Terminalfenster, die bereinigt werden soll
//
//  Einsatz:
//     - vor dem Zeichnen eines Dialogs
//     - nach dem Schließen eines Dialogs
//     - beim Aktualisieren von Statusmeldungen
// ---------------------------------------------------------------------------
extern void clearLineInMessageArea(int row);


// ---------------------------------------------------------------------------
//  showBorderLine()
//  ----------------
//
//  Zweck:
//     Zeichnet eine horizontale Barriere unterhalb des Spielfelds.
//     Diese Linie markiert den Beginn des Nachrichtenbereichs.
//
//  Verwendung:
//     - beim Start eines Levels
//     - beim Neuzeichnen der Oberfläche nach einem Refresh
// ---------------------------------------------------------------------------
extern void showBorderLine(void);


// ---------------------------------------------------------------------------
//  showStatus(aboard, aworm)
//  -------------------------
//
//  Zweck:
//     Gibt eine Statuszeile im Nachrichtenbereich aus.
//     Diese zeigt dem Spieler während des Spiels wichtige Werte:
//
//        - aktuelle Kopfposition des Wurms
//        - verbleibende Futterstellen
//        - aktuelle Länge des Wurms (in Segmenten)
//
//  Parameter:
//     aboard – Zeiger auf das Spielfeldmodell
//     aworm  – Zeiger auf das Wurmmodell
//
//  Hinweis:
//     Die Funktion wird regelmäßig während des Spiels aufgerufen.
// ---------------------------------------------------------------------------
extern void showStatus(struct board* aboard, struct worm* aworm);


// ---------------------------------------------------------------------------
//  showDialog(prompt1, prompt2)
//  ----------------------------
//
//  Zweck:
//     Zeigt einen Dialogtext im Nachrichtenbereich an und wartet auf eine
//     Taste des Spielers.
//
//  Parameter:
//     prompt1 – erste Dialogzeile (Pflichttext)
//     prompt2 – zweite Dialogzeile (optional)
//
//  Rückgabewert:
//     Der eingelesene Tastencode des Benutzers.
//
//  Typische Situationen:
//     - Fehlermeldungen (z. B. Fenster zu klein)
//     - Startdialog eines Levels
//     - Spiel-Ende-Meldungen
// ---------------------------------------------------------------------------
extern int showDialog(char* prompt1, char* prompt2);


#endif  // #define _MESSAGES_H






