#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>

using namespace std;

class Message {
    protected:
        SDL_Rect text_rect;
        TTF_Font* font;
        string text;
        SDL_Color text_color;
        SDL_Texture* texture;

        void updateTexture(SDL_Renderer *rend) {
            if (font == NULL) {
                cout << "message font null" <<endl;
                return;
            }
            SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), text_color);
            if(surface == NULL) {
                cout << "message could not create surface" <<endl;
                return;
            }
            texture = SDL_CreateTextureFromSurface(rend, surface);
            if (texture == NULL) {
                cout << "message could not create texture from surface" << endl;
            }
            SDL_FreeSurface(surface);
        }
    public:
        Message() {
            text_rect = {50, 50, 50, 50};
            text = "ERROR";
        }

        Message(SDL_Renderer *rend, int x, int y, int w, int h, string message, SDL_Color textColor) {
            text_rect = {x, y, w, h};
            text = message;
            text_color = textColor;
            font = TTF_OpenFont("..\\fonts\\default.otf", 256);
            if(font == nullptr) {
                cout << "Could not open font/default.otf" << endl;
                return;
            }
            updateTexture(rend);
        }

        Message(SDL_Renderer *rend, SDL_Rect rect, string message, SDL_Color textColor) {
            text_rect = rect;
            text_color = textColor;
            text = message;
            font = TTF_OpenFont("..\\fonts\\default.otf", 256);
            if(font == nullptr) {
                cout << "Could not open font/default.otf" << endl;
                return;
            }
            updateTexture(rend);
        }

        //constructor to be used with media manager
        Message(SDL_Renderer *rend, SDL_Rect rect, TTF_Font *f, string message, SDL_Color textColor) {
            text_rect = rect;
            text_color = textColor;
            text = message;
            font = f;
            updateTexture(rend);
        }
        virtual ~Message() {
            
        }

        void changeTextSize(SDL_Renderer *rend, SDL_Rect rect) {
                text_rect = rect;
                updateTexture(rend);
        }
        
        void changeTextColor(SDL_Renderer *rend, SDL_Color c) {
            text_color = c;
            updateTexture(rend);
        }
        void changeText(SDL_Renderer *rend, string message) {
            text = message;
            updateTexture(rend);
        }
        string message() {
            return text;
        }

        virtual void draw(SDL_Renderer *rend) {
            if(texture != nullptr) {
                SDL_SetRenderDrawColor(rend, text_color.r, text_color.g, text_color.b, text_color.a);
                SDL_RenderCopy(rend, texture, NULL, &text_rect);
            }
            else {
                cout << "Texture invalid on message!"<<endl;
            }
        }

};
