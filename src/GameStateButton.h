#include "Button.h"
#include "global_enumerations.h"

class GameStateButton:public Button {
    protected:
        GameState activate;
    public:
        GameStateButton():Button() {

        }
        GameStateButton(SDL_Renderer *rend, int x, int y, int w, int h, SDL_Color textColor, SDL_Color buttonColor):Button(rend,x,y,w,h,textColor,buttonColor) {
            //nothing extra I guess
        }

        void handleEvent(SDL_Event &e) {
            //use handleGameEvent
        }
        GameState handleGameEvent(SDL_Event &e) {
            static SDL_Point point;
            SDL_GetMouseState(&point.x, &point.y);
            if(SDL_PointInRect(&point, &button_rect)) {
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