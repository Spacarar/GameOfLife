#pragma once
#include "Message.h"
#include "global_enumerations.h"

class TextButton:public Message {
    protected:
        GameState activate; //return value when clicked
        SDL_Rect button_rect;
        SDL_Color button_color;
        bool highlight;

    public:
        TextButton():Message() {

        }

        TextButton(SDL_Renderer *rend, int x, int y, int w, int h, string message, SDL_Color textColor, SDL_Color buttonColor = {0, 0, 0, 255}, GameState gState=NONE):Message(rend,x,y,w,h,message,textColor) {
            button_color = buttonColor;
            activate = gState;
            changePadding(B_NORMAL);
        }

        TextButton(SDL_Renderer *rend, SDL_Rect rect, string message, SDL_Color textColor, SDL_Color buttonColor = {0, 0, 0, 255}, GameState gState=NONE):Message(rend,rect,message,textColor) {
            button_color = buttonColor;
            activate = gState;
            changePadding(B_NORMAL);
        }

        TextButton(SDL_Renderer *rend, SDL_Rect rect, TTF_Font *f, string message, SDL_Color textColor, SDL_Color buttonColor = {0, 0, 0, 255}, GameState gState=NONE):Message(rend,rect,f,message,textColor) {
            button_color = buttonColor;
            activate = gState;
            changePadding(B_NORMAL);
        }
        ~TextButton() override {
            
        }

        void changePadding(ButtonPadding type) {
            switch (type) {
                case B_TIGHT:
                    button_rect = text_rect;
                    break;
                case B_NORMAL:
                    button_rect = {text_rect.x - 10, text_rect.y - 10, text_rect.w + 20, text_rect.h + 20};
                    break;
                case B_WIDE:
                    button_rect = {text_rect.x - 20, text_rect.y - 20, text_rect.w + 40, text_rect.h + 40};
                    break;
                case B_NONE:
                    button_rect = {0, 0, 0, 0};
                    break;
            }
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

        void draw(SDL_Renderer *rend) override {
            SDL_SetRenderDrawColor(rend, button_color.r, button_color.g, button_color.b, button_color.a);
            SDL_RenderFillRect(rend, &button_rect);
            Message::draw(rend);
        }
};
