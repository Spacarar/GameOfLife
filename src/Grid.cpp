#include <SDL.h>
#include<string>
#include <iostream>
#include "Grid.h"
#include "global_enumerations.h"

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

	for (int y = 0; y < gridSize; y++) {
		for (int x = 0; x < gridSize; x++) {
			if (!pixelCheck(x, y)) return 0;
			if (pixel[y][x]->current) representation.append(to_string(x) + "," + to_string(y) + "|");
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
	for (int y = 0; y < gridSize; y++) {
		for (int x = 0; x < gridSize; x++) {
			if (!pixelCheck(x, y))return;
			ee = neighbors(y, x);
			//overpopulation or underpopulation
			if (pixel[y][x]->current) {
				if ((ee > 3 || ee < 2)) {
					pixel[y][x]->sync = false;
				}
				else {
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
	for (int y = 0; y < gridSize; y++) {
		for (int x = 0; x < gridSize; x++) {
			if (!pixelCheck(x, y))return;
			pixel[y][x]->current = pixel[y][x]->sync;
		}
	}
}

void Grid::update(bool threaded) {
	this->planMove();
	this->updateGrid();
}

void Grid::draw(SDL_Renderer *ren) {
	static SDL_Rect drawRect;
	drawRect.w = pixelSize;
	drawRect.h = pixelSize;
	for (int y = 0; y < gridSize; y++) {
		for (int x = 0; x < gridSize; x++) {
			if (!pixelCheck(x, y)) return;
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

void Grid::draw(SDL_Renderer *ren, SDL_Rect camera) {
	static SDL_Rect drawRect;
	static SDL_Rect renderRect;
	static SDL_Rect edgeRect;
	edgeRect = {0 - camera.x, 0 - camera.y, pixelSize * gridSize, pixelSize * gridSize};
	SDL_SetRenderDrawColor(ren, 230,30,30,255);
	SDL_RenderDrawRect(ren,&edgeRect);
	SDL_SetRenderDrawColor(ren, 0,0,0,255);

	drawRect = {0,0,pixelSize,pixelSize};
	for(int y=0; y < gridSize; y++) {
		for( int x = 0; x < gridSize; x++) {
			if(!pixelCheck(x, y))return;
			drawRect.x = x*pixelSize;
			drawRect.y = y*pixelSize;
			if(pixel[y][x]->current &&  hasIntersection(drawRect, camera)){
				renderRect = {drawRect.x - camera.x, drawRect.y - camera.y, drawRect.w, drawRect.h};
				SDL_SetRenderDrawColor(ren, 230, 230, 240, 255);
				SDL_RenderFillRect(ren, &renderRect);
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
	for (unsigned int i = 0; i < gridC.size(); i++) {
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
	for (int y = 0; y < gridSize; y++) {
		for (int x = 0; x < gridSize; x++) {
			if (!pixelCheck(x, y))return;
			pixel[y][x]->current = false;
			pixel[y][x]->sync = false;
		}
	}
}

bool Grid::pixelCheck(int x, int y) {
	if (this->pixel == nullptr) {
		cout << "pixel was null" <<endl;
		return false;
	}
	if (this->pixel[y] == nullptr) {
		cout << "pixel["<<y<<"] was null"<<endl;
		return false;
	}
	if (this->pixel[y][x] == nullptr) {
		cout << "pixel["<<y<<"]["<<x<<"] was null"<<endl;
		return false;
	}
	return true;
}

pair<int, int> Grid::convertMouseCoords(int mouseX, int mouseY) {
	if ( gridSize < 1 || pixelSize < 1 ) {
		return make_pair(int(-1), int(-1));
	}
	int x = int(mouseX/pixelSize);
	int y = int(mouseY/pixelSize);
	if( x < 0 || y < 0 || x >= gridSize || y >= gridSize) {
		return make_pair(int(-1), int(-1));
	}
	return make_pair(x,y);
}