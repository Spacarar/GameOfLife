#pragma once
#include<chrono>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "GridPlayer.h"


const int SCR_WIDTH = 1080;
const int SCR_HEIGHT = 720;
typedef std::chrono::steady_clock mClock;
typedef enum GameState {MAINMENU, SEARCHING, FREEPLAY, EXIT} GameState;

const int MILLIS_PER_SECOND = 1000;
class GameEngine {
	int FPS;
	int totalFrames;
	mClock::time_point sTime;
	bool isRunning;
	GameState gameState;

	SDL_Thread *updateThread, *drawThread, *searchThread;

	SDL_Window *gWindow;
	SDL_Renderer *ren;
	mClock::time_point lUpdate, lDraw;

	Grid *grid; //for drawing
	GridPlayer *gPlayer;

	void init();
	void initSDL();
	void update();
	
	static int updateGame(void* self);
	static int renderGame(void* self);
	static int searchPatterns(void* self);

	bool updateRequired();
	bool drawRequired();

	void handleEvent(SDL_Event e);
	void draw();

public:
	GameEngine();
	~GameEngine();

	void run();
	void quit();
};