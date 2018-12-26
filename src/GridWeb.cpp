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
	if (numWork < 1) numWork = 1;
	if (numWork > 3) numWork = 3;
	cout << "Initializing web with " << numWork << " worker threads.\nPress 's' to toggle search" << endl;
	this->shownIndex = 0;
	this->numWorkers = numWork;
	this->worker = new WebWorker*[numWork];
	this->is_searching = false;
	this->workerMutex = SDL_CreateMutex();
	for (unsigned int i = 0; i < numWork; i++) {
		this->worker[i] = new WebWorker(gridSize, &(this->gridWeb), &(this->gridRoots), this->workerMutex, i);
	}
}
GridWeb::~GridWeb() {
	for (unsigned int i = 0; i < this->numWorkers; i++) {
		delete this->worker[i];
	}
	delete[] this->worker;
	SDL_DestroyMutex(this->workerMutex);
}

bool GridWeb::isSearching() {
	return this->is_searching;
}
void GridWeb::startSearching() {
	cout << "search started" << endl;
	this->is_searching = true;
	for (unsigned int i = 0; i < this->numWorkers; i++) {
		this->worker[i]->start();
	}
}
void GridWeb::stopSearching() {
	cout << "search stopped" << endl;
	for (unsigned int i = 0; i < this->numWorkers; i++) {
		this->worker[i]->stop();
	}
	this->is_searching = false;
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