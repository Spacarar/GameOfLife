#include "GridPlayer.h"


GridPlayer::GridPlayer(int gSize) {
	cout << "Gridplayer " << gSize << endl;
	displayIt = 0;
	deadChecked = oscChecked = liveChecked = 0;
	pickPiecesCount = 20;
	futureDepth = 100;
	keepSearching = true;
	gridSize = gSize;
	if (gSize < 0)gSize = 4;
	workingGrid = new Grid(gSize, 8);
	gd = GridDictionary(gSize);
}
GridPlayer::~GridPlayer() {
	delete workingGrid;
}
void GridPlayer::pickRandom() {
	int tX, tY;
	int dX, dY;
	vector<pair<int, int> > sc;
	for (int i = 0; i < pickPiecesCount; i++) {
		tX = rand() % gridSize;
		tY = rand() % gridSize;
		for (int i = 0; i < pickPiecesCount; i++) {
			if (rand() % 2 == 0) {
				dX = rand() % (i + 1);
			}
			else {
				dX = (-1)*rand() % (i + 1);
			}
			if (rand() % 2 == 0) {
				dY = rand() % (i + 1);
			}
			else {
				dY = (-1)*rand() % (i + 1);
			}
			sc.push_back(make_pair(tX + dX, tY + dY));
		}
	}
	workingGrid->setState(sc);
	//update things so you miss the loads of start pixels
	workingGrid->update();
}
void GridPlayer::start() {
	string origin = "random";
	size_t me;
	long int totalCount = 0;
	while (keepSearching) {
		for (int c = 0; c < pickPiecesCount * 4; c++) {
			if (keepSearching == false) {
				c = pickPiecesCount * 4 + 1;
				break;
			}
			pickRandom();
			me = workingGrid->me();
			totalCount++;
			for (int f = 0; f < futureDepth; f++) {
				if (keepSearching == false) {
					f = futureDepth + 1;
					break;
				}
				gd.update(me, workingGrid, f, origin);
				switch (gd.stateOf(me)) {
				case LIVING:
					workingGrid->update();
					break;
				case OSCILLATING:
					f = futureDepth + 1;
					break;
				default:
					f = futureDepth + 1;
				}
				if (f == futureDepth - 1) {
					liveChecked++;
				}
			}
		}
		cout << "BEFORE  ";
		printf("Final Count: %d  D:%d  O:%d  L:%d", gd.totalCount(), gd.deadCount(), gd.oscCount(), gd.liveCount());
		gd.cleanup(.8);
		cout << "AFTER  ";
		printf("Final Count: %d  D:%d  O:%d  L:%d\n\n", gd.totalCount(), gd.deadCount(), gd.oscCount(), gd.liveCount());

	}
}
void GridPlayer::stop() {
	keepSearching = false;
}
vector<pair<int, int> > GridPlayer::nextFound() {
	int c = 0;
	while (true) {
		c = 0;
		for (map<size_t, GameRecord>::iterator it = gd.gridDict.begin(); it != gd.gridDict.end(); ++it) {
			if (displayIt < c) {
				displayIt = c;
				cout << "Display # " << displayIt << endl;
				printf("{L,D,O,S}:%d %s  %s  ", it->second.deathCycle, gd.stateString(it->second.lastState).c_str(), it->second.origin.c_str());
				printf("min: %d max: %d \n", it->second.minLife, it->second.maxLife);
				return it->second.startCoords;
			}
			c++;
		}
		displayIt = 0;
	}
}
vector<pair<int, int> > GridPlayer::previousFound() {
	size_t c = gd.gridDict.size();
	while (true) {
		c = gd.gridDict.size();
		for (map<size_t, GameRecord>::iterator it = gd.gridDict.end(); it != gd.gridDict.begin(); --it) {
			if (displayIt > c) {
				displayIt = c;
				cout << "Display # " << displayIt << endl;
				printf("{L,D,O,S}: %d %s %s  ", it->second.deathCycle, gd.stateString(it->second.lastState).c_str(), it->second.origin.c_str());
				printf("min: %d max: %d \n", it->second.minLife, it->second.maxLife);
				return it->second.startCoords;
			}
			c--;
		}
		displayIt = gd.gridDict.size();
	}

}