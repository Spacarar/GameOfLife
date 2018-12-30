#pragma once

#include <chrono>

typedef std::chrono::steady_clock mClock;

typedef enum GameState {
    MAINMENU,
    PAUSED,
    SEARCHMODE,
    SELECTMODE,
    DRAWMODE,
    SAVESELECTION,
    PATTERNMODE,
    LOADPATTERN,
    CLEARWEB,
    G_OK,
    G_CANCEL,
    EXITGAME,
    NONE} GameState;

typedef enum GridMode {G_PLAY, G_FREEZE} GridMode;

typedef enum ButtonPadding {B_TIGHT, B_NORMAL, B_WIDE, B_NONE} ButtonPadding;

const int MILLIS_PER_SECOND = 1000;
