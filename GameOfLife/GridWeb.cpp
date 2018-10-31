#include "GridWeb.h"

GridWeb::GridWeb() {
	this->gridWeb.clear();
	this->shownIndex = 0;
	this->worker = new WebWorker();
}
GridWeb::GridWeb(unsigned int gridSize) {
	this->gridWeb.clear();
	this->shownIndex = 0;
	this->worker = new WebWorker(gridSize, &this->gridWeb);
}
void GridWeb::startSearching() {
	this->worker->start();
}
void GridWeb::stopSearching() {
	this->worker->stop();
}

WebWorker::WebWorker() {
	this->grid = new Grid();
	this->workerThread = nullptr;
	this->isSearching = false;
	this->personalMap.clear();
	this->globalMap = nullptr;
}
WebWorker::WebWorker(unsigned int gridSize, std::map<size_t, Record>* global) {
	this->grid = new Grid(gridSize);
	this->globalMap = global;
	this->isSearching = false;
	this->personalMap.clear();
	this->workerThread = nullptr;
}

void WebWorker::start() {
	this->isSearching = true;
	this->workerThread = SDL_CreateThread(this->StartSearching, "worker searching", this);
}
void WebWorker::stop() {
	this->isSearching = false;
	int result = 0;
	SDL_WaitThread(this->workerThread, &result);
	if (result != 0) {
		cout << "Worker thread error!" << endl;
	}
}