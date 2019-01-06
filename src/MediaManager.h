#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
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
        map<string, Mix_Music *> music;
        map<string, Mix_Chunk *> sounds;


    public:
        MediaManager() {
            textures.clear();
            fonts.clear();
            music.clear();
            sounds.clear();
        }
        ~MediaManager() {
            for (map<string, SDL_Texture *>::iterator tit = textures.begin(); tit != textures.end(); tit++) {
                SDL_DestroyTexture(tit->second);
            }
            for (map<string, TTF_Font *>::iterator fit = fonts.begin(); fit != fonts.end(); fit++) {
                TTF_CloseFont(fit->second);
            }
        }

        SDL_Texture * loadT(SDL_Renderer *ren, string path) {
            if (textures.count(path) == 0) {
                SDL_Surface *surface = IMG_Load(path.c_str());
                if(surface != NULL) {
                    textures[path] = SDL_CreateTextureFromSurface(ren,surface);
                    SDL_FreeSurface(surface);
                }
                else {
                    cout << "Media manager could not load: " << path << endl << IMG_GetError() <<endl;
                }
            }
            return textures[path];
        }

        TTF_Font * loadF(SDL_Renderer *ren, string path) {
            if (fonts.count(path) == 0) {
                TTF_Font * font = TTF_OpenFont(path.c_str(), 256);
                if (font != NULL) {
                    fonts[path] = font;
                }
            }
            return fonts[path];
        }

        Mix_Music * loadM(SDL_Renderer *ren, string path) {
            if (music.count(path) == 0) {
                Mix_Music *m = Mix_LoadMUS(path.c_str());
                if (m != NULL) {
                    music[path] = m;
                } else {
                    cout << "media manager could not load: "<< path <<endl;
                }
            }
            return music[path];
        }

        Mix_Chunk * loadS(SDL_Renderer *ren, string path) {
            if (sounds.count(path) == 0) {
                Mix_Chunk * s = Mix_LoadWAV(path.c_str());
                if(s != NULL) {
                    sounds[path] = s;
                } else {
                    cout << "Media manager could not load: "<<path<<endl;
                }
            }
            return sounds[path];
        }

};
