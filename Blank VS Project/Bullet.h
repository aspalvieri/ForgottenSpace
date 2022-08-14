#ifndef BULLET_H
#define BULLET_H

#include "Includes.h"
#include "GlobalVariables.h"
#include "Sprite.h"
#include "Tile.h"

class Effect;

class Bullet : public Sprite
{
public:
	Bullet();
	Bullet(SDL_Renderer*gR, SDL_Window*gW);
	~Bullet();

	void setBullet(double x, double y, double d, double a, double md, int lv = 0);

	void handleMove(Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y]);

	void draw();

	void setCamera();

	bool alive = true, hitWall = false, spellBullet = false, madeParticles = false;

	int shooterID = -1;

	int type;

	int level = 0;

	double damage, lifesteal = 0.00;

	SDL_Rect aiCamera;

	vector<Effect*> effects;

private:
	//Direction
	double dirX, dirY, angle, distance, maxDistance;
	unsigned int maxCamY, maxCamH, maxCamX, maxCamW;

	//SDL_Handles
	SDL_Renderer *gRenderer;
	SDL_Window *gWindow;
};

class FloatingText
{
public:
	FloatingText(std::string, int x, int y, int w, TTF_Font*, SDL_Color);

	~FloatingText();

	void run(SDL_Renderer *gRenderer, SDL_Rect &);

	bool alive = true;

private:
	bool runtime = false;
	LTexture text, text2;
	SDL_Color color;
	TTF_Font* font = NULL;
	std::string textc;
	int timer = 0, box = 0, alpha = 255;
	double dirx = 0, diry = 0;
	double posx = 0, posy = 0;
};

#endif // BULLET_H
