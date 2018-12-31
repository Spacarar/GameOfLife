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
            changePadding(B_NORMAL);
            button_color = {0, 0, 0, 255};
            highlight = false;
        }

        IconButton(SDL_Texture *tex, SDL_Rect src_rect, SDL_Rect dst_rect, SDL_Color c={0,0,0,255}, GameState active=NONE):Icon(tex,src_rect,dst_rect) {
            activate = active;
            changePadding(B_NORMAL);
            button_color = c;
            highlight = false;
        }
        void changePadding(ButtonPadding type) {
            switch (type) {
                case B_TIGHT:
                    button_rect = display_rect;
                    break;
                case B_NORMAL:
                    button_rect = {display_rect.x - 10, display_rect.y - 10, display_rect.w + 20, display_rect.h + 20};
                    break;
                case B_WIDE:
                    button_rect = {display_rect.x - 20, display_rect.y - 20, display_rect.w + 40, display_rect.h + 40};
                    break;
                case B_NONE:
                    button_rect = {0, 0, 0, 0};
                    break;
            }
        }

        void draw(SDL_Renderer *rend) override {
            SDL_SetRenderDrawColor(rend, button_color.r, button_color.g, button_color.b, button_color.a);
            SDL_RenderFillRect(rend, &button_rect);
            Icon::draw(rend);
        }

        GameState handleEvent(SDL_Event &e, SDL_Point &mouse) {
            if(SDL_PointInRect(&mouse, &button_rect)) {
                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                    return activate;
                }
                highlight = true;
            } else {
                highlight = false;
            }
            return NONE;
        }
}