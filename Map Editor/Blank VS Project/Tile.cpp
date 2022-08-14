#include "Tile.h"

Tile::Tile()
{
	tSize = TILE_SIZE;

	sclip = { 0, 0, 50, 50 };
	oclip = { 0, 0, 50, 50 };

	inUse = false;
	type = 0;
}

Tile::Tile(int posX, int posY, SDL_Rect c, bool col, LTexture t, LTexture s, LTexture o, SDL_Rect& cam)
{
	tSize = TILE_SIZE;

	box.x = posX;
	box.y = posY;
	box.w = tSize;
	box.h = tSize;

	inUse = false;
	type = 0;

	camera = &cam;
	collide = col;
	clip = c;

	sclip = { 0, 0, 50, 50 };
	oclip = { 0, 0, 50, 50 };

	texture = t;
	texture.setScale((double)tSize / (double)clip.w, (double)tSize / (double)clip.h);

	shadow = s;
	shadow.setScale((double)tSize / (double)clip.w, (double)tSize / (double)clip.h);

	object = o;
	object.setScale((double)tSize / (double)clip.w, (double)tSize / (double)clip.h);
}

Tile::Tile(int posX, int posY, SDL_Rect c, bool col, LTexture t)
{
	tSize = MAP_TILE_SIZE;

	box.x = posX;
	box.y = posY;
	box.w = tSize;
	box.h = tSize;

	inUse = false;
	type = 0;

	camera = NULL;
	collide = col;
	clip = c;
	texture = t;
	texture.setScale((double)tSize / (double)clip.w, (double)tSize / (double)clip.h);
}

Tile::~Tile()
{
}

Tile & Tile::setCollide(bool col)
{
	collide = col;
	return *this;
}

bool Tile::getCollide()
{
	return collide;
}

bool Tile::hasCamera()
{
	if (camera)
		return true;
	return false;
}

bool Tile::checkCollision(SDL_Rect B)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = box.x;
	rightA = box.x + box.w;
	topA = box.y;
	bottomA = box.y + box.h;

	//Calculate the sides of rect B
	leftB = B.x;
	rightB = B.x + B.w;
	topB = B.y;
	bottomB = B.y + B.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

void Tile::setBox(SDL_Rect newBox)
{
	box = newBox;
}

SDL_Rect Tile::getBox()
{
	return box;
}

void Tile::draw(SDL_Renderer *gRenderer)
{
	if (camera)
	{
		texture.render(gRenderer, box.x - camera->x, box.y - camera->y, &clip);
		if (sclip.x != 0 || sclip.y != 0)
			shadow.render(gRenderer, box.x - camera->x, box.y - camera->y, &sclip);
		if (oclip.x != 0 || oclip.y != 0)
			object.render(gRenderer, box.x - camera->x, box.y - camera->y, &oclip);
	}
	else
		texture.render(gRenderer, box.x, box.y, &clip);
}

void Tile::setTexture(LTexture tm)
{
	texture = tm;
	texture.setScale((double)tSize / (double)clip.w, (double)tSize / (double)clip.h);
}

bool withinRange(int a, int b, int space)
{
	if (b + space >= a && b - space <= a)
		return true;
	return false;
}
