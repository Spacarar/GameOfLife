#pragma once

#include "Icon.h"
#include "global_enumerations.h"

class IconButton:public Icon {
    protected:
        GameState activate;
        SDL_Rect button_rect;
        SDL_Color button_color;
        bool highlight;

    public:
        IconButton():Icon() {
            activate = NONE;
            button_rect = {display_rect.x - 10, display_rect.y - 10, display_rect.w + 20, display_rect.h + 20};
            button_color = {0, 0, 0, 255};
            highlight = false;
        }

        IconButton(SDL_Texture *tex, SDL_Rect src_rect, SDL_Rect dst_rect, SDL_Color c={0,0,0,255}, GameState active=NONE):Icon(tex,src_rect,dst_rect) {
            activate = active;
            button_rect = {display_rect.x - 10, display_rect.y - 10, display_rect.w + 20, display_rect.h + 20};
            button_color = c;
            highlight = false;
        }

        void draw(SDL_Renderer *rend) override {
            SDL_SetRenderDrawColor(rend, button_color.r, button_color.g, button_color.b, button_color.a);
            SDL_RenderFillRect(rend, &button_rect);
            Icon::draw(rend);
        }
}