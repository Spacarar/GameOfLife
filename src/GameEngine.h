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

	GameStateButton b_menu_main, b_exit, b_menu_new;
	GameStateButton b_pause, b_mode_select, b_mode_draw;
	GameStateButton b_clear_web, b_menu_pattern, b_load_pattern;
	GameStateButton b_ok, b_cancel;

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
	void handleMainMenuEvent(SDL_Event &e);
	void handleSearchingEvent(SDL_Event &e);
	void handlePausedEvent(SDL_Event &e);

	void draw();
	void drawMainMenu();
	void drawGameplay();
	void drawPauseMenu();

public:
	GameEngine();
	GameEngine(unsigned int GridSize, unsigned int numWorkers);
	~GameEngine();

	void run();
	void quit();
};