#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>

using namespace std;

class Button {
    protected:
        SDL_Rect button_rect;
        SDL_Rect text_rect;
        TTF_Font* font;
        string text;
        SDL_Color text_color;
        SDL_Color button_color;
        SDL_Texture* texture;
        bool highlight;
        void updateTexture(SDL_Renderer *rend) {
            SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), text_color);
            texture = SDL_CreateTextureFromSurface(rend, surface);
            SDL_FreeSurface(surface);
        }

    public:
        Button() {
           
        }

        Button(SDL_Renderer *rend, int x, int y, int w, int h, SDL_Color textColor, SDL_Color buttonColor) {
            button_rect = {x,y,w,h};
            highlight = false;
            text = "default button";
            text_rect = {x + 10, y + 10, w - 20, h - 20};
            text_color = textColor;
            button_color = buttonColor;
            font = TTF_OpenFont("..\\fonts\\default.otf", 128);
            if(font == nullptr) {
                cout << "Could not open fonts/default.otf" << endl;
            }
            updateTexture(rend);
        }

        virtual void update() {
            //do nothing
        }

        virtual void draw(SDL_Renderer *rend) {
            if (texture != nullptr) {
                SDL_SetRenderDrawColor(rend, button_color.r, button_color.g, button_color.b, button_color.a);
                SDL_RenderFillRect(rend, &button_rect);
                SDL_SetRenderDrawColor(rend, text_color.r, text_color.g, text_color.b, text_color.a);
			    SDL_RenderCopy(rend, texture, NULL, &text_rect);
            }
            else {
                cout << "texture invalid!!!" << endl;
            }
        }

        virtual void handleEvent(SDL_Event &e) {
            //do nothing
        }

        void changeTextSize(SDL_Renderer* rend, int x, int y, int w, int h) {
            this->button_rect = {x - 10, y - 10, w + 20, h + 20};
            this->text_rect = {x, y, w, h};
            updateTexture(rend);
        }

        void changeTextSize(SDL_Renderer *rend, SDL_Rect rect) {
            this->button_rect = {rect.x - 10, rect.y - 10, rect.w + 20, rect.h + 20};
            this->text_rect = {rect.x, rect.y, rect.w, rect.h};
            updateTexture(rend);
        }
        void changeText(SDL_Renderer *rend, string new_text) {
            this->text = text;
            updateTexture(rend);
        }

        void changeTextColor(SDL_Renderer *rend, SDL_Color c) {
            text_color = {c.r, c.g, c.b, c.a};
            updateTexture(rend);
        }

        void changeRectColor(SDL_Color c) {
            button_color = {c.r, c.g, c.b, c.a};
        }

        void horizontalCenter(SDL_Renderer *rend, int scr_width) {
            button_rect.x = (scr_width/2 -  button_rect.w/2);
            text_rect.x = (scr_width/2 - text_rect.w/2);
            updateTexture(rend);
        }

        void verticalCenter(SDL_Renderer *rend, int scr_height) {
            button_rect.y = (scr_height/2 -  button_rect.h/2);
            text_rect.y = (scr_height/2 - text_rect.h/2);
            updateTexture(rend);
        }

        string message() {
            return text;
        }

};