#pragma once

#include "Grid.h"
#include<chrono>
#include <SDL.h>
//#include <SDL_image.h>
//#include <SDL_mixer.h>
//#include <SDL_ttf.h>
#include <map>
#include <vector>

struct Record {
	std::vector<size_t> parents;
	size_t child = 0;
	bool is_root = false;
	int root_length = 0;
};

class GridRoot {
protected:
	int root_length;
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

	bool includes(size_t member) {
		for (unsigned int i = 0; i < root_members.size(); i++) {
			if (root_members[i] == member) {
				return true;
			}
		}
		return false;
	}

};

class WebWorker {
protected:
	Grid * grid;
	SDL_Thread *workerThread;
	std::map<size_t, Record> personalMap;
	std::map<size_t, Record>* globalMap;
	

	void sendInformation(); //send globalMap your personal map data
	static int StartSearching(); //start thread

	std::vector<std::pair<int, int> > pickRandom(unsigned int pieces, unsigned int distance = 0);
	std::vector<std::pair<int, int> > pickUnion();


public:
	WebWorker();
	WebWorker(unsigned int gridSize, std::map<size_t, Record>* global);

	void start();
	void stop();
};

class GridWeb {
protected:
	std::map<size_t, Record> gridWeb;
	WebWorker* worker;
	size_t shownIndex;

public:
	GridWeb();
	GridWeb(unsigned int gridSize);

	void startSearching();
	void stopSearching();
	std::vector<std::pair<int, int> > nextGrid();
	std::vector<std::pair<int, int> > previousGrid();
};