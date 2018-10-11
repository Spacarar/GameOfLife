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
	unsigned int gridSize = 0, pickPieces = 0, futureDepth = 0;
	cout << "gridSize: ";
	cin >> gridSize;
	cin.get();
	cout << "\n pickPieces: ";
	cin >> pickPieces;
	cin.get();
	cout << "\n futureDepth: ";
	cin >> futureDepth;
	cin.get();
	GameEngine *g = new GameEngine(gridSize, pickPieces, futureDepth);
	cout << "Welcome to conways game " << endl;
	g->run();
	g->quit();
	cout << "Program has finished" << endl;
	cin.get();
	delete g;
	return 0;
}