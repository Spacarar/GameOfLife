#pragma once

#include <chrono>
#include <SDL.h>

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

const int CAMERA_SPEED= 24;

namespace {
    const bool hasIntersection(const SDL_Rect& r1, const SDL_Rect &r2) {
        int top1 = r1.y;
        int top2 = r2.y;
        int bot1 = r1.y + r1.h;
        int bot2 = r2.y + r2.h;
        int left1 = r1.x;
        int left2 = r2.x;
        int right1 = r1.x + r1.w;
        int right2 = r2.x + r2.w;
        
        bool vertically_within = false;
        if (top1 <= bot2 && top1 >= top2) {
            vertically_within = true;
        }
        if (bot1 <= bot2 && bot1 >= top2) {
            vertically_within = true;
        }
        if (top2 <= bot1 && top2 >= top1) {
            vertically_within = true;
        }
        if (bot2 <= bot1 && bot2 >= top2) {
            vertically_within = true;
        }

        bool horizontally_within = false;
        if (left1  <= right2 && left1 >= left2) {
            horizontally_within = true;
        }
        if (right1 <= right2 && right1 >= left2) {
            horizontally_within = true;
        }
        if (left2 <= right1 && left2 >= left1) {
            horizontally_within = true;
        }
        if (right2 <= right1 && right2 >= left1) {
            horizontally_within = true;
        }
        return bool(horizontally_within && vertically_within);
    }
}