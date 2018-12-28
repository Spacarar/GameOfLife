#pragma once

#include <map>
#include <vector>
#include <SDL.h>
#include "Grid.h"
#include "Record.h"
#include "GridRoot.h"
#include "WebWorker.h"

class GridWeb {
protected:
	std::map<size_t, Record> gridWeb;
	std::vector<GridRoot> gridRoots;
	bool is_searching;
	unsigned int numWorkers;
	WebWorker** worker; //array of pointers
	unsigned int shownIndex;
	SDL_mutex *workerMutex;

public:
	GridWeb();
	GridWeb(unsigned int gridSize, unsigned int numWork);
	~GridWeb();

	bool isSearching();
	void startSearching();
	void stopSearching();

	void clear();
	
	std::vector<std::pair<int, int> > nextGrid();
	std::vector<std::pair<int, int> > previousGrid();

};
