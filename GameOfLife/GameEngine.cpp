#include "GameEngine.h"

void GameEngine::init() {
	totalFrames = 0;
	FPS = 5;
	grid = new Grid(250, 4);
	gPlayer = new GridPlayer(250);
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
	//Initialize SDL_mixer
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
		printf("SDL2_mixer init error\n CLOSING...");
		SDL_Quit();
	}
	if (TTF_Init() == -1) {
		printf("SDL2_ttf init error\n CLOSING...");
		SDL_Quit();
	}
	cout << "endl init sdl" << endl;
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
	g->gPlayer->start();
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
			grid->setState(gPlayer->previousFound());
		}
		if (e.button.button == SDL_BUTTON_RIGHT) {
			grid->clear();
			grid->setState(gPlayer->nextFound());
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
GameEngine::~GameEngine() {
	delete gPlayer;

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
	if (gPlayer != nullptr) {
		gPlayer->stop();
	}
	printf("FRAMES: %d FPS: %lld", totalFrames, (totalFrames*MILLIS_PER_SECOND) / ((std::chrono::duration_cast<std::chrono::milliseconds>(mClock::now() - sTime).count())));
	SDL_WaitThread(updateThread, &updateResult);
	SDL_WaitThread(drawThread, &updateResult);
	SDL_WaitThread(searchThread, &searchResult);
	quit();
	std::cin.get();

}
void GameEngine::quit() {
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(gWindow);
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}