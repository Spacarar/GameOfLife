#pragma once

#include"GridDictionary.h"

class GridPlayer {
protected:
	Grid *workingGrid;
	GridDictionary gd;
	int gridSize;
	size_t displayIt; //used for display purposes

	bool keepSearching; //control for thread
	int pickPiecesCount;
	int futureDepth; //how for in the future cycles should we be lookign

	long int deadChecked;
	long int oscChecked;
	long int liveChecked;

	//mutation selections
	void pickRandom();
public:
	GridPlayer(int gSize = 50);
	~GridPlayer();
	void start();
	void stop();

	//used for display purposes as to not end up with dangling references
	vector<pair<int, int> > nextFound();
	vector<pair<int, int> > previousFound();

};