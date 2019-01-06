#pragma once

#include <vector>
#include "MediaManager.h"
#include "Message.h"
#include "Icon.h"
#include "TextButton.h"
#include "IconButton.h"

enum _messages {g_title};
enum _icons {favicon};
enum t_buttons {b_menu_main, b_exit, b_menu_new, b_clear_web, b_menu_pattern, b_load_pattern, b_ok, b_cancel};
enum i_buttons {b_mode_search, b_mode_select, b_mode_draw, b_mode_pause};

const SDL_Color white = {255, 255, 255, 255};
const SDL_Color grey = {60, 60, 60, 255};

const int font_width = 32;
const int font_height = int(font_width * 1.6);



class HUDManager {
    private: 
        int textLine(int linenum, int scr_height) {
            if (linenum < 0 || scr_height < 0 || linenum > 6) {
                return -1;
            }
            int line[7] = {
                96,
                int(scr_height - 4*scr_height/5),
                int(scr_height - 3*scr_height/5),
                int(scr_height - 2*scr_height/5),
                int(scr_height - 1*scr_height/5),
                int(scr_height - scr_height/10),
                int(scr_height - 96)
            };
            return line[linenum];
        }

    protected:
        MediaManager* media;
        vector<Message*> messages;
        // Message *messages;
        vector<Icon*> icons;
        // Icon *icons;
        vector<TextButton*> text_buttons;
        // TextButton *text_buttons;
        vector<IconButton*> icon_buttons;
        // IconButton *icon_buttons; 

        GameState handleMainMenuEvent(SDL_Event &e, SDL_Point &p) {
            int num = 3;
            int menu_items[num] ={b_menu_new, b_exit, b_menu_pattern};
            GameState defG = NONE;
            for(int i = 0; i < num; i++) {
                defG = text_buttons[menu_items[i]]->handleEvent(e,p);
                if(defG != NONE)
                    return defG;
            }
            return NONE;
        }
        GameState handleGameplayEvent(SDL_Event &e, SDL_Point &p) {
            int i_num = 4;
            int g_icons[i_num] = {b_mode_search, b_mode_select, b_mode_draw, b_mode_pause};
            GameState defG = NONE;
            for (int i = 0; i < i_num; i++ ) {
                defG = icon_buttons[g_icons[i]]->handleEvent(e,p);
                if(defG != NONE)
                    return defG;
            }
            return NONE;
        }
        GameState handleSearchingEvent(SDL_Event &e, SDL_Point &p) {
            // highight proper buttons
            return NONE;
        }
        GameState handleDrawEvent(SDL_Event &e, SDL_Point &p) {
            //highlight proper buttons
            return NONE;
        }
        GameState handleSelectEvent(SDL_Event &e, SDL_Point &p) {
            //highlight proper buttons
            return NONE;
        }
        GameState handlePausedEvent(SDL_Event &e, SDL_Point &p) {
            //highlight proper buttons
            return NONE;
        }

        void drawMainMenu(SDL_Renderer *rend) {
            messages[g_title]->draw(rend);
            text_buttons[b_menu_new]->draw(rend);
            text_buttons[b_menu_pattern]->draw(rend);
            text_buttons[b_exit]->draw(rend);
        }
        void drawGameplay(SDL_Renderer *rend) {
            icon_buttons[b_mode_draw]->draw(rend);
            icon_buttons[b_mode_pause]->draw(rend);
            icon_buttons[b_mode_search]->draw(rend);
            icon_buttons[b_mode_select]->draw(rend);
        }
        void drawSearchHUD(SDL_Renderer *rend) {
            //idk yet
        }
        void drawPaintHUD(SDL_Renderer *rend) {
            //idk yet
        }
        void drawSelectHUD(SDL_Renderer *rend) {
            //idk yet
        }
        void drawPauseMenu(SDL_Renderer *rend) {
            text_buttons[b_menu_main]->draw(rend);
            text_buttons[b_clear_web]->draw(rend);
            text_buttons[b_exit]->draw(rend);
        }

    public:
        HUDManager(SDL_Renderer *rend, int scr_width, int scr_height) {
            cout << "starting hud construction" << endl;
            media = new MediaManager();
            int mx = int(scr_width/2);
            int my = int(scr_height/2);
            // re-use this rect to save line length
            SDL_Rect rect = {int(mx - font_width * 7), textLine(0,scr_height), font_width * 16, font_height}; // init with title size
            // re-use this font for things that need it
            TTF_Font* def_font = media->loadF(rend, "..\\fonts\\default.otf");
            // MESSAGES
            messages.push_back(new Message(rend,rect, def_font, "Conway's Game of Life", white));

            // TEXT BUTTONS
            rect = {50, textLine(2,scr_height), font_width * 9, font_height};
            text_buttons.push_back(new TextButton(rend, rect, def_font, "Main Menu", white, grey, MAINMENU));
            rect = {50, textLine(6, scr_height), font_width * 4, font_height};
            text_buttons.push_back(new TextButton(rend, rect, def_font, "Exit", white, grey, EXITGAME));
            rect = {50, textLine(2, scr_height), font_width * 7, font_height};
            text_buttons.push_back(new TextButton(rend, rect, def_font, "New Game", white, grey, SEARCHMODE));
            rect = {50, textLine(3, scr_height), font_width * 15, font_height};
            text_buttons.push_back(new TextButton(rend,rect,def_font, "Clear Web Data", white, grey, CLEARWEB));
            rect = {50, textLine(3, scr_height), font_width * 12, font_height};
            text_buttons.push_back(new TextButton(rend, rect, def_font, "Load Pattern files", white, grey, PATTERNMODE));
            text_buttons.push_back(new TextButton(rend, rect, def_font, "Load Pattern", white, grey, LOADPATTERN));
            rect = {mx - 3*font_width, my - font_height/2, font_width * 2, font_height};
            text_buttons.push_back(new TextButton(rend,rect,def_font,"Ok", white,grey,G_OK));
            rect = {mx + font_width, my - font_height/2, font_width * 2, font_height};
            text_buttons.push_back(new TextButton(rend, rect, def_font, "Cancel", white, grey, G_CANCEL));

            //ICON BUTTONS
            int i_width = 32, i_height = 32;
            int p = 12;
            SDL_Rect src_rect = {0, 0, 64, 64}; //sprite src rect
            rect = {scr_width - (i_width + p), scr_height - (i_height + p), i_width, i_height };
            // icon_buttons[b_mode_search] = IconButton(media->loadT(rend,"..\\icons\\default_set.png"), src_rect, rect, white, SEARCHMODE);
            // src_rect.x += 64;
            // rect = {rect.x - (i_width + p), rect.y, i_width, i_height };
            // icon_buttons[b_mode_select] = IconButton(media->loadT(rend, "..\\icons\\default_set.png"), src_rect, rect, white, SELECTMODE);
            // src_rect.x += 64;
            // rect = {rect.x - (i_width + p), rect.y, rect.w, rect.h};
            // icon_buttons[b_mode_draw] = IconButton(media->loadT(rend, "..\\icons\\default_set.png"), src_rect, rect, white, DRAWMODE);
            // src_rect.x += 64;
            // rect = {p, p, i_width, i_height};
            // icon_buttons[b_mode_pause] = IconButton(media->loadT(rend, "..\\icons\\default_set.png"), src_rect, rect, white, PAUSED);
            cout << "MADE IT TO CONSTRUCTION" << endl;
        }

        ~HUDManager() {
            delete media;
            for(unsigned int i = 0; i < messages.size(); i++ ){
                delete messages[i];
            }
            for(unsigned int i = 0; i < icons.size(); i++ ){
                delete icons[i];
            }
            for(unsigned int i = 0; i < text_buttons.size(); i++ ){
                delete text_buttons[i];
            }
            for(unsigned int i = 0; i < icon_buttons.size(); i++ ){
                delete icon_buttons[i];
            }
        }

        GameState handleEvent(GameState current, SDL_Event &e, SDL_Point &p) {
            GameState defG = NONE;
            if(current == SEARCHMODE || current == DRAWMODE || current == SELECTMODE) {
                defG = handleGameplayEvent(e,p);
                if (defG != NONE) {
                    return defG;
                }
            }
            switch(current) {
                case MAINMENU:
                    defG = handleMainMenuEvent(e,p);
                    if (defG != NONE) {
                        return defG;
                    }
                    break;
                case SEARCHMODE:
                    defG = handleSearchingEvent(e,p);
                    if (defG != NONE) {
                        return defG;
                    }
                    break;
                case DRAWMODE:
                    defG = handleDrawEvent(e,p);
                    if (defG != NONE) {
                        return defG;
                    }
                    break;
                case SELECTMODE:
                    defG = handleSelectEvent(e,p);
                    if (defG != NONE) {
                        return defG;
                    }
                    break;
                case PAUSED:
                    defG = handlePausedEvent(e,p);
                    if (defG != NONE) {
                        return defG;
                    }
                    break;
                default:
                    return NONE;             
            }
            return NONE;
        }
        void draw(GameState current, SDL_Renderer *rend) {
            // return;
            if(current == SEARCHMODE || current == DRAWMODE || current == SELECTMODE) {
                drawGameplay(rend);
            }
            switch(current) {
                case MAINMENU:
                    drawMainMenu(rend);
                    break;
                case SEARCHMODE:
                    drawSearchHUD(rend);
                    break;
                case DRAWMODE:
                    drawPaintHUD(rend);
                    break;
                case SELECTMODE:
                    drawSelectHUD(rend);
                    break;
                case PAUSED:
                    drawPauseMenu(rend);
                    break;
                default:
                    return;
            }
        }
};
