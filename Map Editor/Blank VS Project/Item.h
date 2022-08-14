#ifndef ITEM_H
#define ITEM_H

#include "Includes.h"
#include "GlobalVariables.h"
#include "Sprite.h"
#include "Bullet.h"

//Item structures
struct Weapon
{
	int range = 0;
	double minDamage = 0, maxDamage = 0;
	double energyCost = 0;
	int reload = 0; //reload is number of frames between each shot, use 1 as minimum
	double speed = 0; //speed is the velocity of the bullet
	Sprite sprite;
	Bullet bullet;
	std::string name = "";
};

struct Armor
{
	double armorRating = 0;
};

struct Potion
{
	double healthAmount = 0;
	double energyAmount = 0;
};

struct Resource
{
};

//Item class
class Item : public LTexture
{
public:
	Item();
	Item(SDL_Renderer*, SDL_Window*, SDL_Rect, int, LTexture, SDL_Rect&, int, std::string);
	~Item();

	void draw();

	void delCamera();

	void setScale(double w, double h);

	void setPosition(int x, int y);

	LTexture getTexture();

	SDL_Rect getClip();

	SDL_Rect getBox();

	void setBox(int, int);

	void clearSlot();

	int itemType, value;
	bool alive;
	SDL_Color rarity = { 255,255,255 };
	Weapon weapon;
	Armor armor;
	Potion potion;
	Resource resource;

	bool hovered;

	std::string name = "";
	int levelreq = 0;
	std::string tooltip = "";
	int tooltipSize = 0;
	int tooltipWidth = 200;
	SDL_Rect *camera;

private:
	//Map Texture
	SDL_Rect box;
	SDL_Rect clip;
	
	SDL_Rect zeroZero;

	//SDL Handlers
	SDL_Window* gWindow = NULL;
	SDL_Renderer* gRenderer = NULL;
};

#endif//ITEM_H