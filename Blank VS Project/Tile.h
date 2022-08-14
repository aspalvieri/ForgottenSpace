#ifndef TILE_H
#define TILE_H

#include "Includes.h"
#include "GlobalVariables.h"
#include "LTexture.h"

//=====================================================Light
class Light
{
public:
	Light();
	Light(int v1, int v2, double dis = 1, double str = 1);
	void buildBounds();

	SDL_Rect bbox;
	double strength = 1;
	double distance = 1;
	double x = 0, y = 0;
	int bbound = 0;
};

class Shade
{
public:
	Shade();
	Shade(int, int, LTexture, SDL_Rect&);
	~Shade();

	bool checkCollision(SDL_Rect&);

	void renderLight(SDL_Renderer*);

	void calculateLight(vector<Light> &lights/*, Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y], int &maxCamY, int &maxCamH, int &maxCamX, int &maxCamW*/, Uint8 &maxAlpha);

private:
	LTexture black;
	bool collidedx = false;
	bool collidedy = false;
	bool setMax = false;
	SDL_Rect* camera;
	SDL_Rect box;
	Uint8 alpha = 255;
};

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

	bool checkCollision(SDL_Rect&);

	void setBox(SDL_Rect);

	SDL_Rect getBox();

	void draw(SDL_Renderer*, Uint8 &);

	void setTexture(LTexture tm);

	//For map
	bool inUse;
	bool discovered = false;
	bool inrange = false;
	bool bossTile = false;
	bool mapTile = false;
	int type;
	SDL_Rect* camera;
	SDL_Rect clip;
	SDL_Rect sclip;
	SDL_Rect oclip;
	SDL_Rect box;

	SDL_Rect dbclip;
	SDL_Rect dclip;
	SDL_Rect ruclip;
	SDL_Rect rclip;

private:
	//Tile specific variables
	int tSize;
	bool collide;
	LTexture texture;
	LTexture shadow;
	LTexture object;
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
	int clipx = 0, clipy = 0;
	bool col = false;
};

bool withinRange(int, int, int);

#endif//TILE_H