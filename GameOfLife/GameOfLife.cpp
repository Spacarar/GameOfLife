#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include "GameEngine.h"

using namespace std;

int main(int argc, char** argv) {
	cout << "Begin Main" << endl;
	GameEngine *g = new GameEngine();
	cout << "Welcome to conways game " << endl;
	g->run();
	g->quit();
	cout << "Program has finished" << endl;
	cin.get();
	delete g;
	return 0;
}