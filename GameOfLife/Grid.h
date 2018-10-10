#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

/* one pixel on the grid
current represents what the value of the pixel is currently
sync represents what the pixel will be after a grid evaluation
in this rework we want the pixel to be as light as possible
the grid will handle these changes and drawing to reduce weight */
struct Pixel {
	bool current = 0;
	bool sync = 0;
	Pixel() {
		current = sync = 0;
	}
};

class Grid {
protected:
	int gridSize; // grid will be square 
	int pixelSize; //size of a displayed pixel of the grid
	Pixel ***pixel; // 2d array of pointers
	void freeGrid(); //deallocate pixel array

	int safeN(int n); //modulo math to ensure n is in the array
	void planMove();//sets the sync bit
	void updateGrid();//cp sync bit to current bit
	int neighbors(int x, int y);
public:
	Grid(int gridSize = 4, int pixelSize = 2);
	Grid(const Grid &g);
	~Grid();
	Pixel*** pixelValue() {
		return pixel;
	}
	void draw(SDL_Renderer *ren);
	void update();

	size_t me();
	bool isEmpty();
	int count();
	void setState(int x, int y, bool val);
	void setState(vector<pair<int, int> > sc);
	void clear();
	vector<pair<int, int> > getCoords();
	bool isAlive(int x, int y);

	bool operator==(Grid g);


};