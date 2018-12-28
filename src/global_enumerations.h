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

const int MILLIS_PER_SECOND = 1000;
