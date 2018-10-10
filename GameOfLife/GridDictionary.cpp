#include "GridDictionary.h"


GridDictionary::GridDictionary() {
	gridDict.clear();
	lowAverage = 0.0;
	Grid* emptyG = new Grid();
	update(emptyG->me(), emptyG, 0, "empty");
}
GridDictionary::GridDictionary(int gridSize) {
	gridDict.clear();
	lowAverage = 0.0;
	Grid* emptyG =new Grid(gridSize);
	update(emptyG->me(), emptyG, 0, "empty");
}

GridDictionary::GridDictionary(const GridDictionary &g) {
	lowAverage = g.lowAverage;
	cout << "no copy constructor currently \n";
}
GridDictionary::~GridDictionary() {

}
UpdateState GridDictionary::stateOf(size_t me) {
	if (gridDict.count(me) > 0) {
		return gridDict[me].lastState;
	}
	else {
		printf("I DONT EXIST YET!\n");
		return FAILED;
	}
}
UpdateState GridDictionary::update(size_t me, Grid *g, int cycle, string origin) {
	static int numAdded = 1;
	if (gridDict.count(me) > 0) {
		gridDict[me].origin = origin;
		return gridDict[me].update(g, cycle);
	}
	else {
		gridDict.insert(make_pair(me, GameRecord(g->getCoords(), origin)));
		return update(me, g, cycle, origin);
	}
}

//Dictionary information things
int GridDictionary::deadCount() {
	int c = 0;
	map<size_t, GameRecord>::const_iterator it;
	for (it = gridDict.begin(); it != gridDict.end(); ++it) {
		if (it->second.lastState == DEAD)c++;
	}
	return c;
}
int GridDictionary::liveCount() {
	int c = 0;
	map<size_t, GameRecord>::const_iterator it;
	for (it = gridDict.begin(); it != gridDict.end(); ++it) {
		if (it->second.lastState == LIVING)c++;
	}
	return c;
}
int GridDictionary::totalCount() {
	return int(gridDict.size());
}
int GridDictionary::oscCount() {
	int c = 0;
	map<size_t, GameRecord>::const_iterator it;
	for (it = gridDict.begin(); it != gridDict.end(); ++it) {
		if (it->second.lastState == OSCILLATING)c++;
	}
	return c;
}
string GridDictionary::stateString(int st) {
	switch (st) {
	case LIVING:
		return "Living";
	case DEAD:
		return "Dead";
	case OSCILLATING:
		return "Oscillating";
	default:
		return "FAILED";
	}
}

int GridDictionary::cleanup(double cutoffModifier) {
	map<size_t, GameRecord>::iterator it;
	int count = 0;
	double avgSum = 0;
	for (it = gridDict.begin(); it != gridDict.end(); ++it) {
		if (it->second.lastAverage != 0.0)avgSum += it->second.lastAverage;
	}
	lowAverage = (avgSum / gridDict.size() - 1)*cutoffModifier;
	cout << "\n Low Average: " << lowAverage << endl;
	for (it = gridDict.begin(); it != gridDict.end() && gridDict.size() > 1; ++it) {
		if ((it->second.lastAverage < lowAverage)) {
			gridDict.erase(it);
			count++;
			it = gridDict.begin();
		}
	}
	return count;
}