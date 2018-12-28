#include "Button.h"
#include "global_enumerations.h"

class GameStateButton:public Button {
    protected:
        GameState activate;

    public:
        GameStateButton():Button() {

        }

        GameStateButton(SDL_Renderer *rend, int x, int y, int w, int h, SDL_Color textColor, SDL_Color buttonColor, string button_text = "default gs button", GameState default_state = NONE):Button(rend,x,y,w,h,textColor,buttonColor) {
            text = button_text;
            activate = default_state;
            this->updateTexture(rend);
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
