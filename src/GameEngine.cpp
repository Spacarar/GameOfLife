#include "GameEngine.h"

void GameEngine::init(unsigned int gridSize, unsigned int numWorkers) {
	totalFrames = 0;
	FPS = 5;
	if (gridSize < 4 || gridSize > 1000000) {
		gridSize = 10;
	}
	int pixelSize = 4;
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
	SCR_HEIGHT = current.h - 200;
	SCR_WIDTH = current.w - 200;
	gridCamera = Camera({0, 0, SCR_WIDTH, SCR_HEIGHT});
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
	if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == -1) {
		printf("SDL2_img init error\n CLOSING...");
		SDL_Quit();
	}
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
	static SDL_Point p = {0,0};
	SDL_GetMouseState(&p.x, &p.y);
	//hud cannot directly manipulate game engine, must returns
	static GameState tmpState = NONE;
	tmpState = hud->handleEvent(gameState,e, p);
	//ignore any NONE's
	if (tmpState != NONE) {
		//handle special cases first
		if(tmpState == CLEARWEB) {

		} else if (tmpState == SAVESELECTION) {

		} else {
			//gamestate was returned as not none, and is not a special case
			gameState = tmpState;
			return;
		}
	}
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
			case SDLK_w:
				gridCamera.dY(-CAMERA_SPEED);
				break;
			case SDLK_a:
				gridCamera.dX(-CAMERA_SPEED);
				break;
			case SDLK_s:
				gridCamera.dY(CAMERA_SPEED);
				break;
			case SDLK_d:
				gridCamera.dX(CAMERA_SPEED);
				break;
		}
	}
	else if (e.type == SDL_KEYUP) {
		switch(e.key.keysym.sym) {
			case SDLK_w:
				gridCamera.dY(0);
				break;
			case SDLK_a:
				gridCamera.dX(0);
				break;
			case SDLK_s:
				gridCamera.dY(0);
				break;
			case SDLK_d:
				gridCamera.dX(0);
				break;
		}
	}
	else if (e.type == SDL_MOUSEWHEEL) {
		if (e.wheel.y > 0) {
			grid->increasePixelSize();
		} else if(e.wheel.y < 0) {
			grid->decreasePixelSize();
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
			case SDLK_z:
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
		pair<int, int> pos = grid->convertMouseCoords(mx,my);
		if (e.button.button == SDL_BUTTON_LEFT) {
			grid->setState(pos.first,pos.second, true);
		}
		else if (e.button.button == SDL_BUTTON_RIGHT) {
			grid->setState(pos.first,pos.second, false);
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
	
}

void GameEngine::draw() {
	SDL_RenderClear(ren);
	SDL_SetRenderDrawColor(ren,0,0,0,255);
	if (gameState == SEARCHMODE || gameState == DRAWMODE || gameState == SELECTMODE) {
		grid->draw(ren, gridCamera.rect());
		gridCamera.update();
	}
	hud->draw(gameState,ren);
	SDL_SetRenderDrawColor(ren,0,0,0,255);
	SDL_RenderPresent(ren);
}

std::string GameEngine::stateString(GameState g) const{
    switch(g) {
        case MAINMENU: return "Main Menu";
        case PAUSED: return "Paused";
        case SELECTMODE: return "Select Mode";
        case SEARCHMODE: return "Search Mode";
        case DRAWMODE: return "Draw Mode";
        case SAVESELECTION: return "Save Selection";
        case PATTERNMODE: return "Pattern Mode";
        case LOADPATTERN: return "Load Pattern";
        case CLEARWEB: return "Clear Web";
        case G_OK: return "OK";
        case G_CANCEL: return "Cancel";
        case EXITGAME: return "Exit Game";
        case NONE: return "None";
        default: return "UNKNOWN GAMESTATE";
    }
    return "IMPOSSIBLE GAMESTATE";
}

GameEngine::GameEngine() {
	init();
	initSDL();
}

GameEngine::GameEngine(unsigned int GridSize, unsigned int gridWork) {
	init(GridSize, gridWork);
	initSDL();
	hud = new HUDManager(ren,SCR_WIDTH, SCR_HEIGHT);
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
