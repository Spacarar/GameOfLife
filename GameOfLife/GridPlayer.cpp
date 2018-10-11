#include "GridPlayer.h"


GridPlayer::GridPlayer(unsigned int gSize, unsigned int gPieces, unsigned int fDepth) {
	displayIt = 0;
	deadChecked = oscChecked = liveChecked = 0;
	pickPiecesCount = gPieces;
	futureDepth = fDepth;
	keepSearching = true;
	gridSize = gSize;
	if (gSize < 0)gSize = 4;
	workingGrid = new Grid(gSize, 1);
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

void GridPlayer::pickUnion() {
	int chosen1, chosen2;
	map<size_t, GameRecord>::iterator it1, it2;
	vector<pair<int, int> > chosenCoords;
	chosen1 = rand() % (gd.size());
	chosen2 = rand() % (gd.size());
	it1 = gd.gridDict.begin();
	it2 = gd.gridDict.begin();
	for (int i = 0; i < chosen1 && it1 != gd.gridDict.end(); i++) ++it1;
	for (int i = 0; i < chosen2 && it2 != gd.gridDict.end(); i++) ++it2;
	if (it1 == gd.gridDict.end()) {
		cout << "it1 invalid for union" << endl;
		return;
	}
	if (it2 == gd.gridDict.end()) {
		cout << "it2 invalid for union" << endl;
		return;
	}
	for (int i = 0; i < it1->second.startCoords.size(); i++) {
		chosenCoords.push_back(make_pair(it1->second.startCoords[i].first, it1->second.startCoords[i].second));
	}
	for (int j = 0; j < it2->second.startCoords.size(); j++) {
		chosenCoords.push_back(make_pair(it2->second.startCoords[j].first, it2->second.startCoords[j].second));
	}
	workingGrid->setState(chosenCoords);
	workingGrid->update();
	
}
void GridPlayer::start() {
	string origin = "random";
	size_t me;
	long int totalCount = 0;
	while (keepSearching) {
		std::cout << ++totalCount << endl;
		for (int c = 0; c < pickPiecesCount * 2; c++) {
			std::cout << "c:" << c << endl;
			if (keepSearching == false) {
				c = pickPiecesCount * 2 + 1;
				break;
			}
			if (c < 3 || rand() % 2 == 0) {
				origin = "random";
				pickRandom();
			}
			else {
				origin = "union";
				pickUnion();
			}
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
		std::cout << "BEFORE  ";
		std::printf("Final Count: %d  D:%d  O:%d  L:%d", gd.totalCount(), gd.deadCount(), gd.oscCount(), gd.liveCount());
		gd.cleanup(.8);
		std::cout << "AFTER  ";
		std::printf("Final Count: %d  D:%d  O:%d  L:%d\n\n", gd.totalCount(), gd.deadCount(), gd.oscCount(), gd.liveCount());

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