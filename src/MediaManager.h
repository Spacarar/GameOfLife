#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <map>
#include <string>
#include <iostream>
#include "global_enumerations.h"

using namespace std;

class MediaManager {
    protected:
        map<string, SDL_Texture *> textures;
        map<string, TTF_Font *> fonts;

    public:
        MediaManager() {
            textures.clear();
            fonts.clear();
        }
        ~MediaManager() {
            map<string, SDL_Texture *>::iterator tit = textures.begin();
            map<string, TTF_Font *>::iterator fit = fonts.begin();
            for (tit; tit != textures.end(); tit++) {
                SDL_DestroyTexture(tit->second);
            }
            for (fit; fit != fonts.end(); fit++) {
                TTF_CloseFont(fit->second);
            }
        }

        SDL_Texture * loadT(SDL_Renderer *ren, string path) {
            if (textures.count(imagePath) == 0) {
                SDL_Surface surface = IMG_Load(path.c_str());
                if(surface != nullptr) {
                    images[path] = SDL_CreateTextureFromSurface(ren,surface);
                    SDL_FreeSurface(surface)
                }
                else {
                    cout << "Media manager could not load: " << path << endl << IMG_GetError() <<endl;
                }
            }
            return textures[path];
        }

        TTF_Font * loadF(SDL_Renderer *ren, string path) {
            if (fonts.count(path) == 0) {
                TTF_Font * font = TTF_OpenFont(path, 256);
                if font( != nullptr) {
                    fonts[path] = font;
                }
            }
        }

};
