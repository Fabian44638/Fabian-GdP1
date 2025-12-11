// Worm070 - Aufgabenblatt 8
// ============================================================================
//  worm.h – Zentrale Definitionen für das Worm-Spiel
//
//  Diese Header-Datei enthält alle global relevanten Konstanten, Enumerationen
//  und Symbole, die das gesamte Spiel benötigt. Sie wird in fast jedem Modul
//  eingebunden und bildet damit die gemeinsame Basis für:
//
//    - Rückgabecodes der Funktionen
//    - Spielfeldgrößen und Timing-Werte
//    - Farbcodes für die ncurses-Ausgabe
//    - Symbole für Wurm, Futter und Barrieren
//    - die verschiedenen möglichen Spielzustände
//
//  Hinweis:
//    Änderungen an dieser Datei wirken sich sofort auf das gesamte Spiel aus.
//    Daher sollte sehr bewusst entschieden werden, wenn Werte oder Symbole
//    angepasst werden sollen.
// ============================================================================

#ifndef _WORM_H
#define _WORM_H


// ============================================================================
//  Rückgabecodes für Funktionen
// ============================================================================
//
//  Diese Codes werden von vielen Modulen verwendet, um den Erfolg oder
//  Misserfolg von Operationen eindeutig auszudrücken. Beispiele:
//
//    - initializeLevel()
//    - initializeWorm()
//    - doLevel()
//
//  RES_OK             Die Funktion wurde erfolgreich ausgeführt.
//  RES_FAILED         Ein allgemeiner Fehler ist aufgetreten.
//  RES_INTERNAL_ERROR Unerwarteter Fehler innerhalb der Programmlogik.
// ============================================================================

enum ResCodes {
    RES_OK,
    RES_FAILED,
    RES_INTERNAL_ERROR,
};


// ============================================================================
//  Spielfeldgrößen und Zeitsteuerung
// ============================================================================
//
//  Diese Konstanten legen das grundlegende Verhalten des Spiels fest.
//
//  NAP_TIME:
//     Zeitverzögerung zwischen zwei Spielschritten in Millisekunden.
//     Kleinere Werte = schnelleres Spiel.
//
//  ROWS_RESERVED:
//     Anzahl der Bildschirmzeilen, die **nicht** Teil des Spielfelds sind.
//     Diese werden ausschließlich für Status- und Dialogmeldungen genutzt.
//
//  MIN_NUMBER_OF_ROWS / MIN_NUMBER_OF_COLS:
//     Mindestgröße des Spielfelds. Das Programm prüft beim Start,
//     ob das Terminal groß genug ist. Falls nicht, wird abgebrochen.
// ============================================================================

#define NAP_TIME            100
#define ROWS_RESERVED       4
#define MIN_NUMBER_OF_ROWS  26
#define MIN_NUMBER_OF_COLS  70


// ============================================================================
//  Farbcodes für ncurses (COLOR_PAIR)
// ============================================================================
//
//  Die Farbcodes definieren Kombinationen aus Vordergrund- und Hintergrundfarben.
//  Sie werden in initializeColors() im Hauptprogramm gesetzt und dann von
//  Board, Wurm und Meldungsbereich verwendet.
//
//  Die Werte beginnen ab 1, da COLOR_PAIR(0) eine besondere Bedeutung besitzt.
// ============================================================================

enum ColorPairs {
    COLP_USER_WORM = 1,   // Grün auf Weiß: Körpersegmente und Schwanz
    COLP_FREE_CELL,       // Schwarz auf Weiß: freie Spielfeldzellen
    COLP_FOOD_1,          // Farbe für Futtertyp 1
    COLP_FOOD_2,          // Farbe für Futtertyp 2
    COLP_FOOD_3,          // Farbe für Futtertyp 3
    COLP_BARRIER,         // Rot auf Weiß: Barrieren und Spielfeldrahmen
    COLP_WORM_HEAD        // Rot auf Weiß: Kopf des Wurms
};


// ============================================================================
//  Symbole für das Spielfeld
// ============================================================================
//
//  Diese Zeichen werden auf dem Terminal ausgegeben.
//  Die Farbinformationen stammen aus den ColorPairs.
//
//  SYMBOL_FREE_CELL       Darstellung einer freien Spielfeldzelle
//  SYMBOL_BARRIER         Darstellung einer Barriere
//  SYMBOL_FOOD_*          Zahlenzeichen zur Identifikation der Futtersorten
//
//  SYMBOL_WORM_*:
//     Kopf, Innensegmente und Schwanz des Wurms.
//     Die ältere Template-Version verwendete andere Makronamen – diese bleiben
//     hier aus Kompatibilitätsgründen erhalten.
// ============================================================================

// Freie Zelle / Hintergrund
#define SYMBOL_FREE_CELL        ' '

// Barriere / Hindernis
#define SYMBOL_BARRIER          '#'

// Futter unterschiedlicher Typen
#define SYMBOL_FOOD_1           '2'
#define SYMBOL_FOOD_2           '4'
#define SYMBOL_FOOD_3           '6'

// Wurmteile (neue Namen)
#define SYMBOL_WORM_HEAD        'X'   // Kopf des Wurms
#define SYMBOL_WORM_INNER       '0'   // Innere Segmente
#define SYMBOL_WORM_TAIL        'X'   // Schwanzende

// Alte Namen, zur Kompatibilität erhalten
#define SYMBOL_WORM_HEAD_ELEMENT   SYMBOL_WORM_HEAD
#define SYMBOL_WORM_INNER_ELEMENT  SYMBOL_WORM_INNER
#define SYMBOL_WORM_TAIL_ELEMENT   SYMBOL_WORM_TAIL


// ============================================================================
//  Spielzustände
// ============================================================================
//
//  Diese Werte beschreiben den aktuellen Zustand des Spiels. Sie werden von
//  der Spiellogik, der Eingabeverarbeitung und der Kollisionsüberprüfung
//  verwendet.
//
//  WORM_GAME_ONGOING   Spiel läuft normal weiter.
//  WORM_CRASH          Der Wurm hat eine Barriere getroffen.
//  WORM_OUT_OF_BOUNDS  Der Wurm hat das Spielfeld verlassen.
//  WORM_CROSSING       Der Wurm ist in sein eigenes Körpersegment gelaufen.
//  WORM_GAME_QUIT      Der Spieler hat freiwillig beendet (Taste q).
// ============================================================================

enum GameStates {
    WORM_GAME_ONGOING,
    WORM_CRASH,
    WORM_OUT_OF_BOUNDS,
    WORM_CROSSING,
    WORM_GAME_QUIT
};


#endif  // _WORM_H






