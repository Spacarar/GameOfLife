#include <SDL.h>
#include <iostream>
#include <string>
#include "GameEngine.h"

using namespace std;

int main(int argc, char** argv) {
	cout << "Begin Main" << endl;
	unsigned int gridSize = 0;
	//unsigned int numWorkers = 0;
	cout << "gridSize: ";
	cin >> gridSize;
	cin.get();
	//cout << "\n numWorkers: ";
	//cin >> numWorkers;
	//cin.get();
	GameEngine *g = new GameEngine(gridSize, 1);
	cout << "Welcome to conways game " << endl;
	g->run();
	g->quit();
	cout << "Program has finished" << endl;
	cin.get();
	delete g;
	return 0;
}