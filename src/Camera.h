#pragma once
#include"SDL.h"

class Camera {
    protected:
        SDL_Rect camera_rect;
        int dx, dy;
        int ax, ay;
    public:
        Camera() {
            camera_rect = {0,0,0,0};
            dx = dy = ax = ay = 0;
        }
        Camera(SDL_Rect r) {
            camera_rect = r;
            dx = dy = ax = ay = 0;
        }
        void update() {
            camera_rect.x += dx;
            dx += ax;
            camera_rect.y += dy;
            dy += ay;
        }
        SDL_Rect rect() {
            return camera_rect;
        }
        void stop() {
            dx = dy = ax = ay = 0;
        }
        void setPosition(int x, int y) {
            camera_rect.x = x;
            camera_rect.y = y;
        }
        int dX() {
            return dx;
        }
        void dX(int new_dx) {
            dx = new_dx;
        }
        int dY() {
            return dy;
        }
        void dY(int new_dy) {
            dy = new_dy;
        }
};