#pragma once
#include <vector>
#include<map>
#include "Grid.h"
using namespace std;
typedef enum UpdateState {
	LIVING, DEAD, OSCILLATING, FAILED
} UpdateState;

class GameRecord {
protected:
	int oscDepth;
	unsigned int currentOscillationIndex;
	size_t *oscRecord;
	int repeatedView;

	void countOsc(size_t me) {
		repeatedView = 0;
		for (int i = 0; i < oscDepth; i++) {
			if (me == oscRecord[i]) {
				repeatedView++;
			}
		}
	}
public:
	vector<pair<int, int> > startCoords;
	string origin;
	UpdateState lastState;
	int startLife;
	int minLife;
	int maxLife;
	int lastLife;
	int deathCycle;
	double lastAverage;

	GameRecord(int oscillationDepth = 5) {
		lastState = LIVING;
		startCoords.clear();
		startLife = 0;
		minLife = 10000000000;
		maxLife = lastLife = 0;
		repeatedView = 0;
		if (oscillationDepth < 5) {
			oscillationDepth = 5;
		}
		origin = "Random";
		oscDepth = oscillationDepth;
		currentOscillationIndex = 0;
		deathCycle = -1;
		lastAverage = 0.0;
		oscRecord = new size_t[oscDepth];
	}
	GameRecord(vector<pair<int, int> > startC, string originString, int oscillationDepth = 5) {
		lastState = LIVING;
		startCoords.clear();
		startLife = startC.size();
		for (int i = 0; i < startLife; i++) {
			startCoords.push_back(make_pair(startC[i].first, startC[i].second));
		}
		repeatedView = 0;
		minLife = 10000000; //too big to be real
		maxLife = lastLife = 0;
		if (oscillationDepth < 5) {
			oscillationDepth = 5;
		}
		origin = originString;
		oscDepth = oscillationDepth;
		currentOscillationIndex = 0;
		deathCycle = -1;
		lastAverage = 0.0;
		oscRecord = new size_t[oscDepth];
		
	}
	~GameRecord() {

	}
	UpdateState update(Grid *g, int cycle) {
		int lifeValue = g->count();
		if (cycle == 0) {
			lastAverage = 0.0;
		}
		else if (cycle == 1) {
			lastAverage = abs(lastLife - lifeValue);
		}
		else {
			lastAverage = ((lastAverage*(cycle - 1)) + abs(lastLife - lifeValue) + (cycle*.1)) / cycle;
		}
		lastLife = lifeValue;
		//min/max update
		minLife > lifeValue ? minLife = lifeValue : minLife;
		maxLife < lifeValue ? maxLife = lifeValue : maxLife;
		countOsc(g->me());
		oscRecord[currentOscillationIndex] = g->me();
		currentOscillationIndex = (currentOscillationIndex + 1) % oscDepth;
		
		if (lifeValue == 0) {
			lastState = DEAD;
			deathCycle = cycle;
		}
		else if (repeatedView > 1) {
			lastState = OSCILLATING;
			deathCycle = cycle;
		}
		else {
			lastState = LIVING;
		}
		return lastState;
	}
}; 

class GridDictionary {
protected:
	double lowAverage; //the cleanup average of the dictionary (change in life per cycle averaged over the whole dictionary * a cutoff modifier)

public:
	map<size_t, GameRecord> gridDict;
	GridDictionary();
	GridDictionary(int gridSize);
	GridDictionary(const GridDictionary &g);
	~GridDictionary();

	UpdateState stateOf(size_t me);
	UpdateState update(size_t me, Grid *g, int cycle, string origin);

	//GameRecords hold the average change in life per cycle as a running average
	//Cleanup will take the average of the entire dictionary then delete anything that is lower than that average * cutoffModifier
	//thus cutoffModifier = 1 means anything lower than the average will be thrown out
	int cleanup(double cutoffModifier = 1);
	int size(); 
	//Dictionary information things
	int deadCount();
	int liveCount();
	int totalCount();
	int oscCount();
	string stateString(int st);
};