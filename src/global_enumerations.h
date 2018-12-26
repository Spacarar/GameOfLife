#pragma once
#include <chrono>
typedef std::chrono::steady_clock mClock;
typedef enum GameState {MAINMENU, SEARCHING, FREEPLAY, EXIT, NONE} GameState;
typedef enum GridMode {G_PLAY, G_FREEZE} GridMode;
const int MILLIS_PER_SECOND = 1000;