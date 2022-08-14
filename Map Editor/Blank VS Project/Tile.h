#ifndef TILE_H
#define TILE_H

#include "Includes.h"
#include "GlobalVariables.h"
#include "LTexture.h"

class Tile
{
public:
	Tile();
	Tile(int, int, SDL_Rect, bool, LTexture, LTexture, LTexture, SDL_Rect&);
	Tile(int, int, SDL_Rect, bool, LTexture);
	~Tile();

	Tile& setCollide(bool);

	bool getCollide();

	bool hasCamera();

	bool checkCollision(SDL_Rect);

	void setBox(SDL_Rect);

	SDL_Rect getBox();

	void draw(SDL_Renderer*);

	void setTexture(LTexture tm);

	//For map
	bool inUse;
	int type;
	SDL_Rect* camera;
	SDL_Rect clip;
	SDL_Rect sclip;
	SDL_Rect oclip;
	SDL_Rect box;

	LTexture texture;
	LTexture shadow;
	LTexture object;

private:
	//Tile specific variables
	int tSize;
	bool collide;
};

class TileClip
{
public:
	TileClip() {}
	TileClip(int w, int h, bool c)
	{
		clipx = w;
		clipy = h;
		col = c;
	}
	~TileClip() {}
	void setClip(int w, int h)
	{
		clipx = w;
		clipy = h;
	}
	void setAll(int w, int h, bool c)
	{
		clipx = w;
		clipy = h;
		col = c;
	}
	int clipx = 0, clipy = 0;
	bool col = false;
};

bool withinRange(int, int, int);

#endif//TILE_H