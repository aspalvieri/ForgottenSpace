#ifndef BULLET_H
#define BULLET_H

#include "Includes.h"
#include "GlobalVariables.h"
#include "Sprite.h"
#include "Tile.h"

class Bullet : public Sprite
{
public:
	Bullet();
	Bullet(SDL_Renderer*gR, SDL_Window*gW);
	~Bullet();

	void setBullet(double x, double y, double d, double a, double md);

	void handleMove(Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y]);

	void draw();

	void setCamera();

	bool alive = true, hitWall = false;

	int shooterID = -1;

	int type;

	double damage;

	SDL_Rect aiCamera;

private:
	//Direction
	double dirX, dirY, angle, distance, maxDistance;
	unsigned int maxCamY, maxCamH, maxCamX, maxCamW;

	//SDL_Handles
	SDL_Renderer *gRenderer;
	SDL_Window *gWindow;
};

#endif // BULLET_H
