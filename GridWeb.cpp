#include "GridWeb.h"
#include <vector>

GridWeb::GridWeb() {
	this->gridWeb.clear();
	this->shownIndex = 0;
	this->is_searching = false;
	this->worker = new WebWorker*;
}
GridWeb::GridWeb(unsigned int gridSize, unsigned int numWork) {
	this->gridWeb.clear();
	/*if (numWork < 2 || numWork > 12) {
		numWork = 2;
	}*/
	numWork = 1;
	this->shownIndex = 0;
	this->numWorkers = numWork;
	this->worker = new WebWorker*[numWork];
	this->is_searching = false;
	for (unsigned int i = 0; i < numWork; i++) {
		this->worker[i] = new WebWorker(gridSize, &(this->gridWeb), &(this->gridRoots), i);
	}
}
GridWeb::~GridWeb() {
	for (unsigned int i = 0; i < this->numWorkers; i++) {
		delete this->worker[i];
	}
	delete[] this->worker;
}

bool GridWeb::isSearching() {
	return this->is_searching;
}
void GridWeb::startSearching() {
	this->is_searching = true;
	for (unsigned int i = 0; i < this->numWorkers; i++) {
		this->worker[i]->start();
		for(int j = 0; j < (rand()%1000000 + 100000) && i < this->numWorkers - 1; j++){
			cout << '-';
		}
		cout << "waited"<<endl;
	}
}
void GridWeb::stopSearching() {
	for (unsigned int i = 0; i < this->numWorkers; i++) {
		this->worker[i]->stop();
	}
	this->is_searching = false;
}

//==============================================================================================
WebWorker::WebWorker() {
	this->grid = new Grid();
	this->gridSize = 0;
	this->deadCode = this->grid->me();
	this->workerThread = nullptr;
	this->isSearching = false;
	this->worker_id = 0;
	if (personalMap != nullptr) {
		delete personalMap;
		personalMap = nullptr;
	}
	this->globalMap = nullptr;
	this->globalRoots = nullptr;
}
WebWorker::WebWorker(unsigned int gridSize, std::map<size_t, Record>* global, std::vector<GridRoot>* gRoots, unsigned int id) {
	this->grid = new Grid(gridSize);
	this->deadCode = this->grid->me();
	this->gridSize = gridSize;
	this->globalMap = global;
	this->globalRoots = gRoots;
	this->worker_id = id;
	this->isSearching = false;
	if (personalMap != nullptr) {
		delete personalMap;
		personalMap = nullptr;
	}
	this->personalMap = new map<size_t, Record>;
	this->workerThread = nullptr;
}
WebWorker::~WebWorker() {
	if (this->personalMap != nullptr) {
		(*this->personalMap).clear();
		delete this->personalMap;
	}
}

void WebWorker::start() {
	this->isSearching = true;
	this->workerThread = SDL_CreateThread(this->StartSearching, "worker searching", this);
}

int WebWorker::StartSearching(void *self) {
	WebWorker* worker = (WebWorker*)self;
	if (worker->globalMap == nullptr || worker->globalRoots == nullptr) {
		cout << "global is null on worker... force quitting this thread" << endl;
		return -1;
	}
	WorkCycle cycle = WORKLIVING;
	int count = 0;
	while (worker->isSearching) {
		worker->clearGrid();
		worker->grid->setState(worker->selectStartingGrid());
		worker->updateGrid(); //update once to help prevent erroneous results
		cycle = WORKLIVING;
		count++;
		while (cycle == WORKLIVING && worker->isSearching) {
			cycle = worker->updateGrid();
		}
		while (cycle == REPEATED) {
			cycle = worker->createRoot();//grid should be pointing to the right place
		}
		while (cycle == ROOTCREATED || cycle == GLOBALREPEATED) {
			cycle = worker->dumpData(cycle); //dumps data in parent dictionary
		}
		if(count%10 == 0)cout << "#" << worker->worker_id<< " - count: " <<count <<endl;
	}
	return 0;
}
std::vector<pair<int, int> > WebWorker::selectStartingGrid() {
	int choice = rand() % 2;
	if (choice == 0) {
		return pickRandom(rand() % this->gridSize, rand() % this->gridSize);
	}
	else if (choice == 1) {
		return pickUnion();
	}
	else {
		return pickRandom(rand() % this->gridSize, rand() % this->gridSize);
	}
}
std::vector<pair<int, int> > WebWorker::pickRandom(unsigned int pieces, unsigned int distance) {
	if (pieces < 3) pieces = 3;
	if (distance < 2) distance = 2;
	int baseX = rand() % this->gridSize; 
	int baseY = rand() % this->gridSize;
	int dX, dY;
	vector<pair<int, int> > chosenCoords;
	for (unsigned int i = 0; i < pieces; i++) {
		dX = rand() % distance;
		if (rand() % 2 == 0) dX *= -1;
		dY = rand() % distance;
		if (rand() % 2 == 0) dY *= -1;
		chosenCoords.push_back(make_pair((baseX + dX), (baseY + dY)));
	}
	return chosenCoords;
}
std::vector<pair<int, int> > WebWorker::pickUnion() {
	vector<pair<int, int> > chosenCoords;
	if (this->globalMap == nullptr || (*this->globalMap).size() < 5) {
		return this->pickRandom(rand() % this->gridSize, rand() % this->gridSize);
	}
	int chosen1 = rand() % (*this->globalMap).size();
	int chosen2 = rand() % (*this->globalMap).size();
	map<size_t, Record>::iterator it1 = (*this->globalMap).begin(), it2 = (*this->globalMap).begin();
	int tmp = chosen1;
	if (chosen1 < chosen2) tmp = chosen2;
	for (int i = 0; i < tmp; i++) {
		if (i <= chosen1) ++it1;
		if (i <= chosen2) ++it2;
	}
	for (unsigned int i = 0; i < it1->second.startCoords.size(); i++) {
		chosenCoords.push_back(make_pair(it1->second.startCoords[i].first, it1->second.startCoords[i].second));
	}
	for (unsigned int i = 0; i < it2->second.startCoords.size(); i++) {
		chosenCoords.push_back(make_pair(it2->second.startCoords[i].first, it2->second.startCoords[i].second));
	}
	return chosenCoords;

}

WorkCycle WebWorker::updateGrid() {
	size_t prev = this->grid->me();
	size_t next = 0;
	if ((*this->globalMap).count(prev) > 0) {
		//this grid exists on global map we just found a branch to get to it
		// cout << "global repeated" << endl;
		if((*this->personalMap).count(prev) != 0){
			this->countParents(prev, (*this->globalMap)[prev].node_height);
		}
		return GLOBALREPEATED;
	}
	if ((*this->personalMap)[prev].child != 0) {
		//this grid was already found
		this->personalRoot.clear();
		this->countParents(prev);
		// cout << "personal repeated" << endl;
		return REPEATED;
	}
	else {
		this->grid->update(false);
		next = this->grid->me();
		(*this->personalMap)[prev].child = next;
		(*this->personalMap)[next].parents.push_back(prev);
		(*this->personalMap)[next].startCoords = this->grid->getCoords();
		if (prev == this->deadCode || next == this->deadCode) {
			return DEADROOT;
		}
		if (prev == next) {
			return REPEATED;
		}
		else {
			return WORKLIVING;
		}
	}

}
WorkCycle WebWorker::createRoot() {
	size_t origin = this->grid->me();
	if (!(*this->personalMap).count(origin)) {
		cout << "this hasn't been found yet?" << endl;
		return WORKFAILED;
	}
	if (!(*this->personalMap)[origin].child) {
		cout << "origin has no child" << endl;
		return WORKFAILED;
	}
	this->personalRoot.push(origin);
	size_t temp = (*this->personalMap)[origin].child;
	//FIXME this code looks off mostly child=0?
	while (temp != 0 && !this->personalRoot.includes(temp)) {
		this->personalRoot.push(temp);
		temp = (*this->personalMap)[temp].child;
		if (temp == origin) {
			// cout << "root found!" << endl;
			return ROOTCREATED;
		}
	}
	// cout << "root probably found ?" << endl;
	return ROOTCREATED;
}

WorkCycle WebWorker::dumpData(WorkCycle cy) {
	if (this->globalMap == nullptr || this->globalRoots == nullptr) {
		cout << "global objects missing cannot dump data" << endl;
		return WORKFAILED;
	}
	if (this->grid->count() < 2) {
		cout << "count too small to be worth writing" << endl;
		return DONE;
	}

	//static sdl mutex
	//lock
	if (cy != GLOBALREPEATED) {
		bool exists = false;
		for (unsigned int i = 0; i < this->globalRoots->size(); i++) {
			if ((*this->globalRoots)[i].isMatching(this->personalRoot)) {
				exists = true;
				break;
			}
		}
		if (!exists) {
			this->globalRoots->push_back(this->personalRoot);
			// cout << "wrote global root" << endl;
		}
	}
	std::map<size_t, Record>::iterator myRecords;//records from the worker
	for (myRecords = (*this->personalMap).begin(); myRecords != (*this->personalMap).end(); myRecords++) {
		if (this->globalMap->count(myRecords->first) <= 0) {
			(*this->globalMap)[myRecords->first] = Record(myRecords->second);
			//cout << "Created new record" << endl;
		}
		else {
			for (unsigned int i = 0; i < myRecords->second.parents.size(); i++) {
				//if this one already exists extend its parent's to whatever we found too
				if (!(*this->globalMap)[myRecords->first].parentsIncludes(myRecords->second.parents[i])) {
					(*this->globalMap)[myRecords->first].parents.push_back(myRecords->second.parents[i]);
				}
			}
			//cout << "Updated existing record" << endl;
		}
	}
	//unlock

	return DONE;
}
void WebWorker::clearGrid() {
	this->grid->clear();
	this->personalRoot.clear();
	map<size_t, Record>::iterator mapIt;
	for (mapIt = (*this->personalMap).begin(); mapIt != (*this->personalMap).end(); mapIt++) {
		mapIt->second.parents.clear();
		mapIt->second.startCoords.clear();
	}
	(*this->personalMap).clear();
}

void WebWorker::stop() {
	this->isSearching = false;
	int result = 0;
	SDL_WaitThread(this->workerThread, &result);
	if (result != 0) {
		cout << "Worker thread error!" << endl;
	}
}

std::vector<pair<int, int> > GridWeb::nextGrid() {
	shownIndex++;
	if (!this->gridRoots.size() || !this->gridWeb.size()) {
		std::vector<pair<int, int> > temp;
		return temp;
	}
	shownIndex = shownIndex % this->gridWeb.size();
	map<size_t, Record>::iterator member = this->gridWeb.begin();
	advance(member,rand() % shownIndex);
	cout <<"#"<<member->first<< "  root: "<< (member->second.is_root ? "true" : "false") << "  height"<< member->second.node_height<<endl;
	return member->second.startCoords;
	//return this->gridWeb[this->gridRoots[this->shownIndex % this->gridRoots.size()].member(0)].startCoords;
}
std::vector<pair<int, int> > GridWeb::previousGrid() {
	shownIndex--;
	if (!this->gridRoots.size()) {
		std::vector<pair<int, int> > temp;
		return temp;
	}
	shownIndex = shownIndex % this->gridWeb.size();
	map<size_t, Record>::iterator member = this->gridWeb.begin();
	advance(member, rand() % shownIndex);
	cout <<"#"<<member->first<< "  root: "<< (member->second.is_root ? "true" : "false") << "  height"<< member->second.node_height<<endl;
	return member->second.startCoords;
	//return this->gridWeb[this->gridRoots[this->shownIndex % this->gridRoots.size()].member(0)].startCoords;
}

int WebWorker::countParents(size_t node, int count, map<size_t, bool> *seen) {
	if(seen == nullptr){
		cout << "countparents null seen"<<endl;
		return count;
	}
	if(personalMap == nullptr || globalMap == nullptr){
		cout << "somebody's map doesn't look correct"<<endl;
		return count;
	}
	if((*personalMap).count(node) < 1){
		cout << "node passed does not exist" <<endl;
		return count;
	}
	if((*seen)[node] == true){
		cout << "node parents circular reference" << endl;
		return count;
	}
	(*personalMap)[node].node_height = count;
	(*seen)[node] = true;
	for(unsigned int i = 0; i < (*personalMap)[node].parents.size(); i++){
		return countParents((*personalMap)[node].parents[i], count+1, seen);
	}
	return count;
}