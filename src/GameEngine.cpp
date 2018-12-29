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
	// SCR_HEIGHT = SCR_WIDTH = (pixelSize) * gridSize;
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
	SCR_HEIGHT = current.h - 100;
	SCR_WIDTH = current.w - 100;
	gWindow = SDL_CreateWindow("Conway's Game of Life", 50, 50, SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_SHOWN);
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
	SDL_Color white = {255, 255, 255, 255};
	SDL_Color grey = {60, 60, 60, 255};

	int font_width = 32;
	int line[7] = {
		96,
		int(SCR_HEIGHT - 4*SCR_HEIGHT/5),
		int(SCR_HEIGHT - 3*SCR_HEIGHT/5),
		int(SCR_HEIGHT - 2*SCR_HEIGHT/5),
		int(SCR_HEIGHT - 1*SCR_HEIGHT/5),
		int(SCR_HEIGHT - SCR_HEIGHT/10),
		int(SCR_HEIGHT - 96)
	};
	//buttons that show on the main menu
	b_menu_new = GameStateButton(ren, 50, line[1], font_width * 7, font_width * 2, white, grey, "New Game", SEARCHMODE);
	b_exit = GameStateButton(ren, 50, line[6], font_width * 3, font_width * 2, white, grey, "Exit", EXITGAME);
	b_menu_pattern = GameStateButton(ren,50,line[2], font_width * 11, font_width * 2, white, grey, "Load Patterns", PATTERNMODE);
	b_load_pattern = GameStateButton(ren, 50, line[3], font_width * 3, font_width * 2, white, grey, "load", LOADPATTERN);

	//buttons that show while you are playing the game
	b_pause = GameStateButton(ren, 16, 16, 50, 50, grey, white, " || ", PAUSED);
	b_mode_draw = GameStateButton(ren, SCR_WIDTH - 66, SCR_HEIGHT - 66, 50, 50,grey, white,"D", DRAWMODE);
	b_mode_select = GameStateButton(ren, SCR_WIDTH - 122, SCR_HEIGHT - 66, 50, 50, grey, white, "S", SELECTMODE);

	//buttons that show while the game is paused
	b_menu_main = GameStateButton(ren, 50, line[1], font_width * 8, font_width * 2, white, {60, 60, 60,255}, "Main Menu", MAINMENU);
	b_clear_web = GameStateButton(ren, 50, line[2], font_width * 13, font_width * 2, white, grey, "Clear Web Data", MAINMENU);

	//general buttons
	b_ok = GameStateButton(ren, SCR_WIDTH/2 - 80, 0, font_width * 1.5, font_width * 2, white, grey, "ok", G_OK);
	b_cancel = GameStateButton(ren, SCR_WIDTH/2 + 50, 0, font_width * 5, font_width * 2, white, grey, "cancel", G_CANCEL);

	b_ok.verticalCenter(ren,SCR_HEIGHT);
	b_cancel.verticalCenter(ren,SCR_HEIGHT);

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
	if (gameState == SEARCHMODE || gameState == DRAWMODE || gameState == SELECTMODE) {
		handleGameplayEvent(e);
	}
	switch(gameState) {
		case MAINMENU:
			handleMainMenuEvent(e);
			break;
		case SEARCHMODE:
			handleSearchingEvent(e);
			break;
		case DRAWMODE:
			handleDrawEvent(e);
			break;
		case SELECTMODE:
			handleSelectEvent(e);
			break;
		case PAUSED:
			handlePausedEvent(e);
			break;
		default:
			cout << "unhandled gamestate event" << endl;
	}
}

void GameEngine::handleMainMenuEvent(SDL_Event &e) {
	if(b_menu_new.handleGameEvent(e) == SEARCHMODE) {
		gameState = SEARCHMODE;
		return;
	}

	if(b_exit.handleGameEvent(e) == EXITGAME) {
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


void GameEngine::handleGameplayEvent(SDL_Event &e) {
	if (b_pause.handleGameEvent(e) == PAUSED) {
		gameState = PAUSED;
		return;
	}
	if (b_mode_draw.handleGameEvent(e) == DRAWMODE) {
		gameState = DRAWMODE;
		return;
	}
	if (b_mode_select.handleGameEvent(e) == SELECTMODE) {
		gameState = SELECTMODE;
		return;
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
		}
	}
}

void GameEngine::handleSearchingEvent(SDL_Event &e) {
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (g_mode == G_PLAY) {
			if (e.button.button == SDL_BUTTON_LEFT) {
				grid->clear();
				grid->setState(gWeb->previousGrid());
			}
			else if (e.button.button == SDL_BUTTON_RIGHT) {
				grid->clear();
				grid->setState(gWeb->nextGrid());
			}
		}
	}
	if (e.type == SDL_KEYDOWN) {
		switch(e.key.keysym.sym) {
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

void GameEngine::handleDrawEvent(SDL_Event &e) {
	static int mx, my;
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		SDL_GetMouseState(&mx, &my);
		if (e.button.button == SDL_BUTTON_LEFT) {

		}
		else if (e.button.button == SDL_BUTTON_RIGHT) {

		}
	}
}

void GameEngine::handleSelectEvent(SDL_Event &e) {
	static int mx, my;
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		SDL_GetMouseState(&mx, &my);
		if (e.button.button == SDL_BUTTON_LEFT) {

		}
		else if (e.button.button == SDL_BUTTON_RIGHT) {

		}
	}
}

void GameEngine::handlePausedEvent(SDL_Event &e) {
	if (b_menu_main.handleGameEvent(e) == MAINMENU) {
		gameState = MAINMENU;
		return;
	}
	if (b_clear_web.handleGameEvent(e) == CLEARWEB) {
		this->gWeb->clear(); //destroys the web :(
		cout << "WEB CLEARED :( "<<endl;
	}
	if (b_exit.handleGameEvent(e) == EXITGAME) {
		gameState = EXITGAME;
		isRunning = false;
		return;

	}
}

void GameEngine::draw() {
	SDL_RenderClear(ren);
	SDL_SetRenderDrawColor(ren,0,0,0,255);
	if (gameState == SEARCHMODE || gameState == DRAWMODE || gameState == SELECTMODE) {
		drawGameplay();
	}
	switch(gameState) {
		case MAINMENU:
			drawMainMenu();
			break;
		case SEARCHMODE:
			drawSearchHUD();
			break;
		case DRAWMODE:
			drawPaintHUD();
			break;
		case SELECTMODE:
			drawSelectHUD();
			break;
		case PAUSED:
			drawPauseMenu();
			break;
		default:
			cout << "unhandled draw event" <<endl;
	}
	SDL_SetRenderDrawColor(ren,0,0,0,255);
	SDL_RenderPresent(ren);
}

void GameEngine::drawMainMenu() {
	b_menu_new.draw(ren);
	b_menu_pattern.draw(ren);
	b_exit.draw(ren);
}

void GameEngine::drawGameplay() {
	grid->draw(ren);
	b_pause.draw(ren);
	b_mode_draw.draw(ren);
	b_mode_select.draw(ren);
}

void GameEngine::drawSearchHUD() {
	//add text
}
void GameEngine::drawPaintHUD() {
	//add text
}
void GameEngine::drawSelectHUD() {
	//add text
}

void GameEngine::drawPauseMenu() {
	b_menu_main.draw(ren);
	b_clear_web.draw(ren);
	b_exit.draw(ren);
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
