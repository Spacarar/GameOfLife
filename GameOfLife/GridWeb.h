#pragma once

#include "Grid.h"
#include<chrono>
#include <SDL.h>
//#include <SDL_image.h>
//#include <SDL_mixer.h>
//#include <SDL_ttf.h>
#include <map>
#include <vector>

class Record {
public:
	size_t child = 0;
	std::vector<pair<int, int> > startCoords;
	bool is_root = false;
	std::vector<size_t> parents;
	int root_length = 0;
	bool parentsIncludes(size_t o) {
		for (int i = 0; i < parents.size(); i++) {
			if (parents[i] == o)return true;
		}
		return false;
	}
	Record() {
		child = 0;
		is_root = false;
		parents.clear();
		startCoords.clear();
		root_length = 0;
	}
	Record(const Record & r) {
		this->child = r.child;
		this->is_root = r.is_root;
		this->parents.clear();
		this->startCoords.clear();
		for (int i = 0; i < r.parents.size(); i++) {
			this->parents.push_back(r.parents[i]);
		}
		for (int i = 0; i < r.startCoords.size(); i++) {
			this->startCoords.push_back(r.startCoords[i]);
		}
		this->root_length = this->parents.size();
	}
};

class GridRoot {
protected:
	size_t root_length;
	std::vector<size_t> root_members;

public:
	GridRoot() {
		this->root_length = 0;
		this->root_members.clear();
	}
	GridRoot(std::vector<size_t> members) {
		this->root_length = members.size();
		this->root_members.clear();
		for (unsigned int i = 0; i < members.size(); i++) {
			this->root_members.push_back(members[i]);
		}
	}
	void clear() {
		root_length = 0;
		root_members.clear();
	}
	int size() {
		return root_length;
	}
	size_t member(unsigned int n) {
		return root_members[n % root_members.size()];
	}
	void set(std::vector<size_t> members) {
		this->root_length = members.size();
	}
	void push(size_t member) {
		this->root_length += 1;
		this->root_members.push_back(member);
	}
	bool includes(size_t member) {
		for (unsigned int i = 0; i < root_members.size(); i++) {
			if (root_members[i] == member) {
				return true;
			}
		}
		return false;
	}
	bool isMatching(GridRoot &g) {
		if (g.root_length != this->root_length) {
			return false;
		}
		for (int i = 0; i < g.root_members.size(); i++) {
			if (!this->includes(g.root_members[i])) {
				return false;
			}
		}
		return true;
	}

};

typedef enum WorkCycle {WORKLIVING, REPEATED, DEADROOT, GLOBALREPEATED, ROOTCREATED, DONE, WORKFAILED} WorkCycle;
class WebWorker {
protected:
	Grid * grid;
	unsigned int gridSize;
	SDL_Thread *workerThread;
	std::map<size_t, Record>* personalMap;
	GridRoot personalRoot;
	size_t deadCode;
	std::map<size_t, Record>* globalMap;
	std::vector<GridRoot>* globalRoots;

	bool isSearching;

	WorkCycle dumpData(WorkCycle cy); //send globalMap your personal map data
	WorkCycle updateGrid();
	WorkCycle createRoot();

	std::vector<pair<int, int> > selectStartingGrid();
	void clearGrid();
	static int StartSearching(void* self); //start thread

	std::vector<std::pair<int, int> > pickRandom(unsigned int pieces, unsigned int distance = 0);
	std::vector<std::pair<int, int> > pickUnion();


public:
	WebWorker();
	WebWorker(unsigned int gridSize, std::map<size_t, Record>* global, std::vector<GridRoot>* gRoots);
	~WebWorker();
	void start();
	void stop();
};

class GridWeb {
protected:
	std::map<size_t, Record> gridWeb;
	std::vector<GridRoot> gridRoots;
	unsigned int numWorkers;
	WebWorker** worker; //array of pointers
	unsigned int shownIndex;

public:
	GridWeb();
	GridWeb(unsigned int gridSize, unsigned int numWork);
	~GridWeb();

	void startSearching();
	void stopSearching();
	std::vector<std::pair<int, int> > nextGrid();
	std::vector<std::pair<int, int> > previousGrid();
};