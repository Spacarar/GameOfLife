#pragma once

#include <map>
#include <SDL.h>
#include "Grid.h"
#include "Record.h"
#include "GridRoot.h"

typedef enum WorkCycle {WORKLIVING, REPEATED, DEADROOT, GLOBALREPEATED, ROOTCREATED, DONE, WORKFAILED} WorkCycle;
class WebWorker {
	private:
		SDL_mutex *writeMutex;

	protected:
		Grid * grid;
		unsigned int worker_id;
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
		int countParents(size_t node, int count=0, map<size_t, bool> *seen = new map<size_t, bool>());

		std::vector<pair<int, int> > selectStartingGrid();
		void clearGrid();
		static int StartSearching(void* self); //start thread

		std::vector<std::pair<int, int> > pickRandom(unsigned int pieces, unsigned int distance = 0);
		std::vector<std::pair<int, int> > pickUnion();


	public:
		WebWorker();
		WebWorker(unsigned int gridSize, std::map<size_t, Record>* global, std::vector<GridRoot>* gRoots,SDL_mutex *mute, unsigned int id=0);
		~WebWorker();
		void start();
		void stop();
};