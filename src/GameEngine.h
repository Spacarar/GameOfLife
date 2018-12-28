#pragma once
#include<chrono>
#include <SDL.h>
#include <SDL_ttf.h>
#include "GridWeb.h"
#include "GameStateButton.h"
#include "global_enumerations.h"

class GameEngine {
	int SCR_WIDTH;
	int SCR_HEIGHT;
	int FPS;
	int totalFrames;
	mClock::time_point sTime;
	bool isRunning;
	GridMode g_mode;
	GameState gameState;

	SDL_Thread *updateThread, *drawThread;

	SDL_Window *gWindow;
	SDL_Renderer *ren;
	mClock::time_point lUpdate, lDraw;

	Grid *grid; //for drawing
	GridWeb *gWeb; //for searching patterns

	GameStateButton main_menu_button, exit_button, pause_button, new_game_button, clear_web_button;

	void init(unsigned int gridSize = 50, unsigned int numWorkers = 2);
	void initSDL();
	void initButtons();
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
	GameEngine(unsigned int GridSize, unsigned int numWorkers);
	~GameEngine();

	void run();
	void quit();
};