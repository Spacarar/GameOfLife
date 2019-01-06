#pragma once
#include<chrono>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include "GridWeb.h"
#include "HUDManager.h"

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

	HUDManager *hud;

	void init(unsigned int gridSize = 50, unsigned int numWorkers = 2);
	void initSDL();
	void update();
	
	static int updateGame(void* self);
	static int renderGame(void* self);
	static int searchPatterns(void* self);

	bool updateRequired();
	bool drawRequired();

	void handleEvent(SDL_Event e);
	void handleMainMenuEvent(SDL_Event &e);
	void handleGameplayEvent(SDL_Event &e); //used for all gameplay types
	void handleSearchingEvent(SDL_Event &e);
	void handleDrawEvent(SDL_Event &e);
	void handleSelectEvent(SDL_Event &e);
	void handlePausedEvent(SDL_Event &e);

	void draw();
	string stateString(GameState g) const;

public:
	GameEngine();
	GameEngine(unsigned int GridSize, unsigned int numWorkers);
	~GameEngine();

	void run();
	void quit();
};