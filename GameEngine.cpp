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
}
void GameEngine::update() {
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
	//g->gPlayer->start();
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
	static int mouseX;
	static int mouseY;
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		SDL_GetMouseState(&mouseX, &mouseY);
		if (g_mode == G_PLAY) {
			if (e.button.button == SDL_BUTTON_LEFT) {
				grid->clear();
				//grid->setState(gPlayer->previousFound());
				grid->setState(gWeb->previousGrid());
			}
			if (e.button.button == SDL_BUTTON_RIGHT) {
				grid->clear();
				//grid->setState(gPlayer->nextFound());
				grid->setState(gWeb->nextGrid());
				//grid.drawGlider(mouseX, mouseY);
			}
		}
	}
	else if (e.type == SDL_KEYDOWN) {
		int FPS_Map[10] = {0, 2, 5, 10, 20, 40,80, 160,320, 640};
		SDL_Keycode KEY_Map[10] = {SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9};
		for (int i = 0; i < 9; i++) {
			if (e.key.keysym.sym == KEY_Map[i]) {
				FPS = FPS_Map[i];
				i == 0 ? g_mode = G_FREEZE : g_mode = G_PLAY;
			}
		}
		switch (e.key.keysym.sym) {
		case SDLK_s:
			if (this->gWeb->isSearching()) {
				this->gWeb->stopSearching();
			} else {
				this->gWeb->startSearching();
			}
		}
	}
}

void GameEngine::draw() {
	SDL_RenderClear(ren);
	grid->draw(ren);
	SDL_RenderPresent(ren);
}

GameEngine::GameEngine() {
	init();
	initSDL();

}
GameEngine::GameEngine(unsigned int GridSize, unsigned int gridWork) {
	init(GridSize, gridWork);
	initSDL();
}
GameEngine::~GameEngine() {
	//delete gPlayer;
	delete gWeb;
}

void GameEngine::run() {
	int updateResult, drawResult, searchResult;

	updateThread = SDL_CreateThread(updateGame, "Update", this);
	drawThread = SDL_CreateThread(renderGame, "Render", this);
	searchThread = SDL_CreateThread(searchPatterns, "Search", this);
	this->gWeb->stopSearching();
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
	//if (gPlayer != nullptr) {
	//	gPlayer->stop();
	//}
	if (gWeb != nullptr) {
		gWeb->stopSearching();
	}
	printf("FRAMES: %d FPS: %lld", totalFrames, (totalFrames*MILLIS_PER_SECOND) / ((std::chrono::duration_cast<std::chrono::milliseconds>(mClock::now() - sTime).count())));
	SDL_WaitThread(updateThread, &updateResult);
	SDL_WaitThread(drawThread, &drawResult);
	SDL_WaitThread(searchThread, &searchResult);
	quit();
	std::cin.get();

}
void GameEngine::quit() {
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(gWindow);
	// Mix_Quit();
	// TTF_Quit();
	// IMG_Quit();
	SDL_Quit();
}