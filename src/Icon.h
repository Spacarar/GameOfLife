#pragma once

#include <SDL.h>
// #include <SDL_image.h>
#include <iostream>
#include <string>

using namespace std;

class Icon {
    protected:
        SDL_Texture *texture;
        SDL_Rect source_rect, display_rect;
    public:
        Icon() {
            source_rect =  display_rect = {0, 0, 0, 0};
        }
        Icon(SDL_Texture *tex, SDL_Rect src_rect, SDL_Rect dst_rect) {
            texture = tex;
            source_rect = src_rect;
            display_rect = dst_rect;
        }
        virtual ~Icon() {
            SDL_DestroyTexture(texture);
        }

        void changeSize(SDL_Rect rect) {
            display_rect = rect;
        }

        virtual void draw(SDL_Renderer *rend) {
            if(texture != NULL) {
                SDL_RenderCopy(rend, texture, &source_rect, &display_rect);
            } else {
                cout << "icon texture is null!" << endl;
            }
        }
};