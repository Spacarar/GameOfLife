#pragma once

#include "MediaManager.h"
#include "global_enumerations.h"
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
        Message *messages;
        Icon *icons;
        TextButton *text_buttons;
        IconButton *icon_buttons; 

        GameState handleMainMenuEvent(SDL_Event &e, SDL_Point &p);
        GameState handleGameplayEvent(SDL_Event &e, SDL_Point &p); 
        GameState handleSearchingEvent(SDL_Event &e, SDL_Point &p);
        GameState handleDrawEvent(SDL_Event &e, SDL_Point &p);
        GameState handleSelectEvent(SDL_Event &e, SDL_Point &p);
        GameState handlePausedEvent(SDL_Event &e, SDL_Point &p);

        void drawMainMenu(SDL_Renderer *rend);
        void drawGameplay(SDL_Renderer *rend);
        void drawSearchHUD(SDL_Renderer *rend);
        void drawPaintHUD(SDL_Renderer *rend);
        void drawSelectHUD(SDL_Renderer *rend);
        void drawPauseMenu(SDL_Renderer *rend);

    public:
        HUDManager(SDL_Renderer *rend, int scr_width, int scr_height) {
            media = new MediaManager();
            int mx = int(scr_width/2);
            int my = int(scr_height/2);
            // re-use this rect to save line length
            SDL_Rect rect = {int(mx - font_width * 7), textLine(0,scr_height), font_width * 16, font_height}; // init with title size
            // re-use this font for things that need it
            TTF_Font* def_font = media->loadF(rend, "..\\fonts\\default.otf");
            
            // MESSAGES
            messages = new Message[g_title + 1];
            messages[g_title] = Message(rend,rect, def_font, "Conway's Game of Life", white);

            // TEXT BUTTONS
            text_buttons = new TextButton[b_cancel + 1];
            rect = {50, textLine(2,scr_height), font_width * 9, font_height};
            text_buttons[b_menu_main] = TextButton(rend, rect, def_font, "Main Menu", white, grey, MAINMENU);
            rect = {50, textLine(6, scr_height), font_width * 4, font_height};
            text_buttons[b_exit] = TextButton(rend, rect, def_font, "Exit", white, grey, EXITGAME);
            rect = {50, textLine(2, scr_height), font_width * 7, font_height};
            text_buttons[b_menu_new] = TextButton(rend, rect, def_font, "New Game", white, grey, SEARCHMODE);
            rect = {50, textLine(3, scr_height), font_width * 15, font_height};
            text_buttons[b_clear_web] = TextButton(rend,rect,def_font, "Clear Web Data", white, grey, CLEARWEB);
            rect = {50, textLine(3, scr_height), font_width * 12, font_height};
            text_buttons[b_load_pattern] = TextButton(rend, rect, def_font, "Load Pattern", white, grey, LOADPATTERN);
            rect = {mx - 3*font_width, my - font_height/2, font_width * 2, font_height};
            text_buttons[b_ok] = TextButton(rend,rect,def_font,"Ok", white,grey,G_OK);
            rect = {mx + font_width, my - font_height/2, font_width * 2, font_height};
            text_buttons[b_cancel] = TextButton(rend, rect, def_font, "Cancel", white, grey, G_CANCEL);

            //ICON BUTTONS
            int i_width, i_height = 32;
            int p = 12;
            icon_buttons = new IconButton[b_mode_pause + 1];
            SDL_Rect src_rect = {0, 0, 64, 64}; //sprite src rect
            rect = {scr_width - (i_width + p), scr_height - (i_height + p), i_width, i_height };
            icon_buttons[b_mode_search] = IconButton(media->loadT(rend,"..\\icons\\default_set.png"), src_rect, rect, white, SEARCHMODE);
            src_rect.x += 64;
            rect = {rect.x - (i_width + p), rect.y, i_width, i_height };
            icon_buttons[b_mode_select] = IconButton(media->loadT(rend, "..\\icons\\default_set.png"), src_rect, rect, white, SELECTMODE);
            src_rect.x += 64;
            rect = {rect.x - (i_width + p), rect.y, rect.w, rect.h};
            icon_buttons[b_mode_draw] = IconButton(media->loadT(rend, "..\\icons\\default_set.png"), src_rect, rect, white, DRAWMODE);
            src_rect.x += 64;
            rect = {p, p, i_width, i_height};
            icon_buttons[b_mode_pause] = IconButton(media->loadT(rend, "..\\icons\\default_set.png"), src_rect, rect, white, PAUSED);

        }

        ~HUDManager() {
            delete media;
            delete messages;
            delete icons;
            delete text_buttons;
            delete icon_buttons;
        }

        GameState handleEvent(GameState current, SDL_Event &e, SDL_Point &p) {
            if(current == SEARCHMODE || current == DRAWMODE || current == SELECTMODE) {
                handleGameplayEvent(e,p);
            }
            switch(current) {
                case MAINMENU:
                    handleMainMenuEvent(e,p);
                    break;
                case SEARCHMODE:
                    handleSearchingEvent(e,p);
                    break;
                case DRAWMODE:
                    handleDrawEvent(e,p);
                    break;
                case SELECTMODE:
                    handleSelectEvent(e,p);
                    break;
                case PAUSED:
                    handlePausedEvent(e,p);
                    break;
                default:
                
            }
        }
        void draw(GameState current, SDL_Renderer *rend) {
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
                
            }
        }
};