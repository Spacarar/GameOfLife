#include <SDL.h>
#include<string>
#include <iostream>
#include "Grid.h"

Grid::Grid(int gridSize, int pixelSize) {
	if (gridSize > 1) {
		this->gridSize = gridSize;
	}
	else {
		cout << "malformed gridsize" << gridSize << endl;
		this->gridSize = 2;
	}
	if (pixelSize > 1) {
		this->pixelSize = pixelSize;
	}
	this->pixel = new (Pixel**[this->gridSize]);
	for (int y = 0; y < this->gridSize; y++) {
		this->pixel[y] = new (Pixel*[this->gridSize]);
		for (int x = 0; x < this->gridSize; x++) {
			this->pixel[y][x] = new Pixel();
		}
	}
}

Grid::Grid(const Grid &g) {
	this->freeGrid();
	this->gridSize = g.gridSize;
	this->pixelSize = g.pixelSize;
	this->pixel = new Pixel**[this->gridSize];
	for (int y = 0; y < this->gridSize; y++) {
		this->pixel[y] = new Pixel*[this->gridSize];
		for (int x = 0; x < this->gridSize; x++) {
			pixel[y][x] = new Pixel();
		}
	}
}

size_t Grid::me() {
	std::string representation = "";
	std::hash<string> hashThis;
	representation.clear();
	if (pixel == nullptr) {
		cout << "pixel was null" << endl;
		return 0;
	}
	for (int x = 0; x < gridSize; x++) {
		if (pixel[x] == nullptr) {
			cout << "pixel[x] is null..." << endl;
			return 0;
		}
		for (int y = 0; y < gridSize; y++) {
			if (pixel[x][y] == nullptr) {
				cout << "pixel[x][y] is null..." << endl;
				return 0;
			}
			if (pixel[x][y]->current)representation.append(to_string(x) + "," + to_string(y) + "|");
			//else representation.append("0");
		}
	}
	return hashThis(representation);
}

Grid::~Grid() {
	for (int i = 0; i < this->gridSize; i++) {
		for (int j = 0; j < this->gridSize; j++) {
			delete pixel[i][j];
		}
		delete[] pixel[i];

	}
	delete[] pixel;
}

void Grid::freeGrid() {
	//CAUTION IF GRID IS CALLED AFTER THIS FUNCTION ACCESS VIOLIATION LIKELY
	for (int i = 0; i < this->gridSize; i++) {
		for (int j = 0; j < this->gridSize; j++) {
			delete pixel[i][j];
		}
		delete[] pixel[i];

	}
	delete[] pixel;
}

int Grid::safeN(int n) {
	int rt = 0;

	if (n < 0)rt = (this->gridSize - ((abs(n)) % this->gridSize));
	else rt = n % this->gridSize;
	return rt;
}

int Grid::neighbors(int y, int x) {
	int ee = 0;
	if (pixel == nullptr || pixel[y] == nullptr || pixel[y][x] == nullptr) {
		cout << "something is null when evaluating neighbors\n";
		return 0;
	}
	if (pixel[safeN(y - 1)][safeN(x)]->current)ee++;
	if (pixel[safeN(y - 1)][safeN(x - 1)]->current)ee++;
	if (pixel[safeN(y - 1)][safeN(x + 1)]->current)ee++;
	if (pixel[safeN(y + 1)][safeN(x)]->current)ee++;
	if (pixel[safeN(y + 1)][safeN(x - 1)]->current)ee++;
	if (pixel[safeN(y + 1)][safeN(x + 1)]->current)ee++;
	if (pixel[safeN(y)][safeN(x - 1)]->current)ee++;
	if (pixel[safeN(y)][safeN(x + 1)]->current)ee++;
	return ee;
}

void Grid::planMove() {
	int ee = 0;
	if (pixel == nullptr) {
		cout << "Pixel is null when planning move " << &pixel << "\n";
		return;
	}
	for (int y = 0; y < gridSize; y++) {
		if (pixel[y] == nullptr) {
			cout << "Pixel[y] is null when planning move " << &pixel[y] << "\n";
			return;
		}
		for (int x = 0; x < gridSize; x++) {
			if (pixel[y][x] == nullptr) {
				cout << "Pixel[y][x] is null when planning move " << &pixel[y][x] << "\n";
				return;
			}
			ee = neighbors(y, x);
			//overpopulation or underpopulation
			if (pixel[y][x]->current) {
				if ((ee > 3 || ee < 2)) {
					pixel[y][x]->sync = false;
				}
				else {
					//this wasn't always working
					pixel[y][x]->sync = true;
				}
				//population growth
			}
			else if (ee == 3) {
				pixel[y][x]->sync = true;
			}
		}
	}
}

void Grid::updateGrid() {
	if (pixel == nullptr) {
		cout << "pixel is null\n";
		return;
	}
	for (int y = 0; y < gridSize; y++) {
		if (pixel[y] == nullptr) {
			cout << "pixel[y] is null\n";
			return;
		}
		for (int x = 0; x < gridSize; x++) {
			if (pixel[y][x] == nullptr) {
				cout << "pixel[y][x] is null\n";
				return;
			}
			pixel[y][x]->current = pixel[y][x]->sync;
		}
	}
}

void Grid::update(bool threaded) {
	this->planMove();
	this->updateGrid();
	//if (this->gridSize >= 200) {
	//	this->updateThreaded();
	//}
	//else {
	//	this->planMove();
	//	this->updateGrid();
	//}
}
int Grid::updateThreaded() {
	int riserStatus = 0, fallerStatus = 0;
	this->faller = SDL_CreateThread(Grid::startFallerPlan, "fallerPlan", this);
	this->riser = SDL_CreateThread(Grid::startRiserPlan, "riserPlan", this);
	SDL_WaitThread(this->riser, &riserStatus);
	SDL_WaitThread(this->faller, &fallerStatus);
	if (riserStatus + fallerStatus == 0) {
		this->riser = SDL_CreateThread(Grid::startRiserUpdate, "riserUpdate", this);
		this->faller = SDL_CreateThread(Grid::startFallerUpdate, "fallerUpdate", this);
		SDL_WaitThread(this->riser, &riserStatus);
		SDL_WaitThread(this->faller, &fallerStatus);
	}
	else {
		cout << "An error has occured" << endl;
		cin.get();
		cin.get();
	}
	return riserStatus + fallerStatus;
}
void Grid::draw(SDL_Renderer *ren) {
	SDL_Rect drawRect;
	drawRect.w = pixelSize;
	drawRect.h = pixelSize;
	if (pixel == nullptr) {
		cout << "PIXEL IS NULL" << endl;
		return;
	}
	for (int y = 0; y < gridSize; y++) {
		if (pixel[y] == nullptr) {
			cout << "PIXEL[Y] IS NULL" << endl;
			return;
		}
		for (int x = 0; x < gridSize; x++) {
			if (pixel[y][x] == nullptr) {
				cout << "PIXEL[Y][X] IS NULL" << endl;
				return;
			}
			if (pixel[y][x]->current) {
				SDL_SetRenderDrawColor(ren, 230, 230, 240, 255);
				drawRect.x = x * pixelSize;
				drawRect.y = y * pixelSize;
				SDL_RenderFillRect(ren, &drawRect);
				SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
			}
		}
	}
}
int Grid::count() {
	int c = 0;
	for (int y = 0; y < gridSize; y++) {
		for (int x = 0; x < gridSize; x++) {
			if (pixel[y][x]->current) c++;
		}
	}
	return c;
}
bool Grid::isEmpty() {
	return this->count() == 0;
}

vector<pair<int, int> > Grid::getCoords() {
	vector<pair<int, int> > rt;
	pair<int, int> tmp;
	for (int y = 0; y < gridSize; y++) {
		for (int x = 0; x < gridSize; x++) {
			if (pixel[y][x]->current) {
				tmp.first = x;
				tmp.second = y;
				rt.push_back(tmp);
			}
		}
	}
	return rt;
}
void Grid::printCoords() {
	vector<pair<int, int> > gridC = this->getCoords();
	cout << "Coordinates: ";
	for (int i = 0; i < gridC.size(); i++) {
		cout << "(" << gridC[i].first << ", " << gridC[i].second << "), ";
	}
	cout << "=======================================================\n";
}

bool Grid::isAlive(int x, int y) {
	return pixel[safeN(y)][safeN(x)]->current;
}

bool Grid::operator==(Grid g) {
	if (this->pixel == nullptr || g.pixel == nullptr) {
		return false;
	}
	for (int y = 0; y < gridSize; y++) {
		if (this->pixel[y] == nullptr || g.pixel[y] == nullptr) {
			return false;
		}
		for (int x = 0; x < gridSize; x++) {
			if (this->pixel[y][x] == nullptr || g.pixel[y][x] == nullptr) {
				return false;
			}
			if (this->pixel[y][x]->current != g.pixel[y][x]->current) {
				return false;
			}
		}
	}
	return true;
}

void Grid::setState(int x, int y, bool val) {
	pixel[safeN(y)][safeN(x)]->current = val;
}
void Grid::setState(vector<pair<int, int> > sc) {
	this->clear();
	for (unsigned int i = 0; i < sc.size(); i++) {
		pixel[safeN(sc[i].second)][safeN(sc[i].first)]->current = true;
	}
}
void Grid::clear() {
	if (pixel == nullptr) {
		cout << "pixel is null\n";
		return;
	}
	for (int y = 0; y < gridSize; y++) {
		if (pixel[y] == nullptr) {
			cout << "pixel[y] is null\n";
			return;
		}
		for (int x = 0; x < gridSize; x++) {
			if (pixel[y][x] == nullptr) {
				cout << "pixel[y][x] is null\n";
				return;
			}
			pixel[y][x]->current = false;
			pixel[y][x]->sync = false;
		}
	}
}

int Grid::startRiserPlan(void *self) {
	Grid *g = (Grid*)self;
	int me = 0;
	int ee = 0;
	for (int y = 0; y <= floor(g->gridSize/2); y++) {
		for (int x = 0; x < g->gridSize; x++) {
			me = g->pixel[y][x]->current;
			ee = g->neighbors(y, x);
			if (me) {
				if (ee < 2 || ee>3) {
					g->pixel[y][x]->sync = false;
				}
			}
			else {
				if (ee == 3) {
					g->pixel[y][x]->sync = true;
				}
			}
		}
	}
	return 0;
}

int Grid::startRiserUpdate(void *self) {
	Grid *g = (Grid*)self;
	for (int y = 0; y <= floor(g->gridSize/2); y++) {
		for (int x = 0; x < g->gridSize; x++) {
			g->pixel[y][x]->current = g->pixel[y][x]->sync;
		}
	}
	return 0;
}
int Grid::startFallerPlan(void *self) {
	Grid *g = (Grid*)self;
	int me = 0;
	int ee = 0;
	for (int y = g->gridSize - 1; y > floor(g->gridSize/2) ; y--) {
		for (int x = 0; x < g->gridSize; x++) {
			me = g->pixel[y][x]->current;
			ee = g->neighbors(y, x);
			if (me) {
				if (ee < 2 || ee>3) {
					g->pixel[y][x]->sync = false;
				}
			}
			else {
				if (ee == 3) {
					g->pixel[y][x]->sync = true;
				}
			}
		}
	}
	return 0;
}
int Grid::startFallerUpdate(void *self) {
	Grid *g = (Grid*)self;

	for (int y = g->gridSize - 1; y > floor(g->gridSize/2); y--) {
		for (int x = 0; x < g->gridSize; x++) {
			g->pixel[y][x]->current = g->pixel[y][x]->sync;
		}
	}
	return 0;
}