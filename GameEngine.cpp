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
	grid->update();
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
	else if (e.type == SDL_KEYDOWN) {
		switch (e.key.keysym.sym) {
		case SDLK_0:
			FPS = 0;
			break;
		case SDLK_1:
			FPS = 2;
			break;
		case SDLK_2:
			FPS = 5;
			break;
		case SDLK_3:
			FPS = 10;
			break;
		case SDLK_4:
			FPS = 20;
			break;
		case SDLK_5:
			FPS = 40;
			break;
		case SDLK_6:
			FPS = 80;
			break;
		case SDLK_7:
			FPS = 160;
			break;
		case SDLK_8:
			FPS = 320;
			break;
		case SDLK_9:
			FPS = 640;
			break;
		case SDLK_s:
			if(this->gWeb->isSearching()){
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