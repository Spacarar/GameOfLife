#include "GameEngine.h"

void GameEngine::init(unsigned int gridSize, unsigned int numWorkers) {
	totalFrames = 0;
	FPS = 5;
	if (gridSize < 4 || gridSize > 1000000) {
		gridSize = 10;
	}
	int pixelSize = 4;
	if (gridSize > 150) {
		pixelSize--;
		if (gridSize > 300) {
			pixelSize--;
		}
	}
	if (gridSize < 75) {
		pixelSize += 2;
		if (gridSize < 50) {
			pixelSize += 2;
		}
	}
	SCR_HEIGHT = SCR_WIDTH = (pixelSize) * gridSize;
	grid = new Grid(gridSize, pixelSize);
	gWeb = new GridWeb(gridSize, numWorkers);
	sTime = lUpdate = lDraw = mClock::now();
	isRunning = true;
	gameState = MAINMENU;
	g_mode = G_PLAY;
}

void GameEngine::initSDL() {
	if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1)) {
		printf("Could not initialize SDL: %s.\n", SDL_GetError());
		exit(-1);
	}
	SDL_DisplayMode current;
	for(int i = 0; i < SDL_GetNumVideoDisplays(); ++i){
		int should_be_zero = SDL_GetCurrentDisplayMode(i, &current);
		if(should_be_zero != 0){
			SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());
		} else {
			SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz.", i, current.w, current.h, current.refresh_rate);
		}
	}

	gWindow = SDL_CreateWindow("Conway's Game of Life", 50, 50, current.w - 160, current.h - 90, SDL_WINDOW_SHOWN);
	if (gWindow == nullptr) {
		printf("Could not create window!");
	}
	ren = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL) {
		SDL_DestroyWindow(gWindow);
		printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_Quit();
	}

	if (TTF_Init() == -1) {
			printf("SDL2_ttf init error\n CLOSING...");
			SDL_Quit();
	}
}

void GameEngine::initButtons() {
	main_menu_button = GameStateButton(ren, 50, 50, 500, 125, {255, 255, 255, 255}, {60, 60, 60,255}, "Main Menu", MAINMENU);
	exit_button = GameStateButton(ren, 50, SCR_HEIGHT - 50, 175, 125, {255, 255, 255, 255}, {60, 60, 60, 255}, "Exit", EXITGAME);
	pause_button = GameStateButton(ren, 16, 16, 50, 50, {0, 0, 0, 255}, {255, 255, 255, 255}, " || ", PAUSED);
	new_game_button = GameStateButton(ren, 50, 300, 475, 125, {255, 255, 255, 255}, {60, 60, 60, 255}, "New Game", SEARCHING);
	clear_web_button = GameStateButton(ren, 50, SCR_HEIGHT - 450, 500, 125, {255,255,255,255}, {60, 60, 60, 255}, "Clear Web Data", MAINMENU);

	// main_menu_button.horizontalCenter(ren,SCR_WIDTH);
	// main_menu_button.verticalCenter(ren,SCR_HEIGHT);
	// clear_web_button.horizontalCenter(ren,SCR_WIDTH);
	// exit_button.horizontalCenter(ren,SCR_WIDTH);
}

void GameEngine::update() {
	if(gameState == EXITGAME){
		isRunning = false;
		return;
	}
	totalFrames++;
	if (g_mode == G_PLAY) {
		grid->update();
	}
	
}

int GameEngine::updateGame(void* self) {
	GameEngine *g = (GameEngine*)self;
	while (g->isRunning) {
		if (g->updateRequired()) {
			g->update();
		}
	}
	return 0;
}

int GameEngine::renderGame(void* self) {
	GameEngine *g = (GameEngine*)self;
	while (g->isRunning) {
		g->draw();
	}
	return 0;
}

int GameEngine::searchPatterns(void* self) {
	GameEngine *g = (GameEngine*)self;
	g->gWeb->startSearching();
	cout << "SEARCH PATTERNS RETURN" << endl;
	return 0;
}

bool GameEngine::updateRequired() {
	long long difference = (long long)(MILLIS_PER_SECOND / (FPS + 1)) - std::chrono::duration_cast<std::chrono::milliseconds>(mClock::now() - lUpdate).count();
	if (difference <= 0) {
		lUpdate = mClock::now();
		return true;
	}
	return false;
}

bool GameEngine::drawRequired() {
	long long difference = (long long)(MILLIS_PER_SECOND / (FPS + 1)) - std::chrono::duration_cast<std::chrono::milliseconds>(mClock::now() - lDraw).count();
	if (difference <= 0) {
		lDraw = mClock::now();
		return true;
	}
	return false;
}

void GameEngine::handleEvent(SDL_Event e) {
	switch(gameState) {
		case MAINMENU:
			handleMainMenuEvent(e);
			break;
		case SEARCHING:
			handleSearchingEvent(e);
			break;
		case PAUSED:
			handlePausedEvent(e);
			break;
		default:
			cout << "unhandled gamestate event" << endl;
	}
}

void GameEngine::handleMainMenuEvent(SDL_Event &e) {
	if(new_game_button.handleGameEvent(e) == SEARCHING) {
		gameState = SEARCHING;
		return;
	}

	if(exit_button.handleGameEvent(e) == EXITGAME) {
		gameState = EXITGAME;
		return;
	}

	static int mouseX;
	static int mouseY;
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		SDL_GetMouseState(&mouseX, &mouseY);
		if (g_mode == G_PLAY) {
			if (e.button.button == SDL_BUTTON_LEFT) {
				grid->clear();
				grid->setState(gWeb->previousGrid());
			}
			if (e.button.button == SDL_BUTTON_RIGHT) {
				grid->clear();
				grid->setState(gWeb->nextGrid());
			}
		}
	}
}

void GameEngine::handleSearchingEvent(SDL_Event &e) {
	static int mouseX;
	static int mouseY;

	if( pause_button.handleGameEvent(e) == PAUSED) {
		gameState = PAUSED;
		return;
	}
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		SDL_GetMouseState(&mouseX, &mouseY);
		if (g_mode == G_PLAY) {
			if (e.button.button == SDL_BUTTON_LEFT) {
				grid->clear();
				grid->setState(gWeb->previousGrid());
			}
			if (e.button.button == SDL_BUTTON_RIGHT) {
				grid->clear();
				grid->setState(gWeb->nextGrid());
			}
		}
	}
	if (e.type == SDL_KEYDOWN) {
		switch(e.key.keysym.sym) {
			case SDLK_0:
				FPS = 0;
				g_mode = G_FREEZE;
				break;

			case SDLK_1:
				FPS = 2;
				g_mode = G_PLAY;
				break;

			case SDLK_2:
				FPS = 5;
				g_mode = G_PLAY;
				break;

			case SDLK_3:
				FPS = 10;
				g_mode = G_PLAY;
				break;

			case SDLK_4:
				FPS = 20;
				g_mode = G_PLAY;
				break;

			case SDLK_5:
				FPS = 40;
				g_mode = G_PLAY;
				break;

			case SDLK_6:
				FPS = 80;
				g_mode = G_PLAY;
				break;

			case SDLK_7:
				FPS = 160;
				g_mode = G_PLAY;
				break;

			case SDLK_8:
				FPS = 320;
				g_mode = G_PLAY;
				break;

			case SDLK_9:
				FPS = 640;
				g_mode = G_PLAY;
				break;

			case SDLK_s:
				if (this->gWeb->isSearching()) {
					this->gWeb->stopSearching();
				} else {
					this->gWeb->startSearching();
				}
				break;
		}
	}
}

void GameEngine::handlePausedEvent(SDL_Event &e) {
	if (main_menu_button.handleGameEvent(e) == MAINMENU) {
		gameState = MAINMENU;
		return;
	}
	if (clear_web_button.handleGameEvent(e) == CLEARWEB) {
		this->gWeb->clear(); //destroys the web :(
		cout << "WEB CLEARED :( "<<endl;
	}
	if (exit_button.handleGameEvent(e) == EXITGAME) {
		gameState = EXITGAME;
		isRunning = false;
		return;

	}
}

void GameEngine::draw() {
	if(gameState == EXITGAME){
		isRunning = false;
		return;
	}
	SDL_RenderClear(ren);
	SDL_SetRenderDrawColor(ren,0,0,0,255);
	switch(gameState) {
		case MAINMENU:
			drawMainMenu();
			break;
		case SEARCHING:
			drawGameplay();
			break;
		case PAUSED:
			drawPauseMenu();
			break;
		case DRAWMODE:
			drawGameplay();
		default:
			cout << "unhandled draw event" <<endl;
	}
	SDL_SetRenderDrawColor(ren,0,0,0,255);
	SDL_RenderPresent(ren);
}

void GameEngine::drawMainMenu() {
	new_game_button.draw(ren);
	exit_button.draw(ren);
}

void GameEngine::drawGameplay() {
	grid->draw(ren);
	pause_button.draw(ren);
}

void GameEngine::drawPauseMenu() {
	main_menu_button.draw(ren);
	clear_web_button.draw(ren);
	exit_button.draw(ren);
}

GameEngine::GameEngine() {
	init();
	initSDL();
	initButtons();
}

GameEngine::GameEngine(unsigned int GridSize, unsigned int gridWork) {
	init(GridSize, gridWork);
	initSDL();
	initButtons();
}

GameEngine::~GameEngine() {
	delete gWeb;
}


void GameEngine::run() {
	int updateResult, drawResult;

	updateThread = SDL_CreateThread(updateGame, "Update", this);
	drawThread = SDL_CreateThread(renderGame, "Render", this);
	while (isRunning) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_WINDOWEVENT) {
				if (event.window.event == SDL_WINDOWEVENT_CLOSE)
					isRunning = false;
			}
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_ESCAPE)
					isRunning = false;
			}
			if (isRunning) handleEvent(event);
		}
	}
	if (gWeb != nullptr) {
		gWeb->stopSearching();
	}
	printf("FRAMES: %d FPS: %lld", totalFrames, (totalFrames*MILLIS_PER_SECOND) / ((std::chrono::duration_cast<std::chrono::milliseconds>(mClock::now() - sTime).count())));
	SDL_WaitThread(updateThread, &updateResult);
	SDL_WaitThread(drawThread, &drawResult);
	quit();
	std::cin.get();

}

void GameEngine::quit() {
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(gWindow);
	TTF_Quit();
	// Mix_Quit();
	// IMG_Quit();
	SDL_Quit();
}
