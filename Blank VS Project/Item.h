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
	SDL_Color color = { 255, 255, 255 };
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

	void buildTooltip(int pS = 0);

	void buildResource(string type, SDL_Color cRarity);

	void buildWeapon(Weapon wp, int lvreq, SDL_Color cRarity);

	void buildPotion(int ha, int ea, SDL_Color cRarity);

	void buildArmor(int ar, int lvreq, string type, SDL_Color aColor, SDL_Color cRarity);

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

	std::string name = "Empty";
	int levelreq = 0;
	std::string tooltip = "";
	int tooltipSize = 0;
	int tooltipWidth = 200;
	SDL_Rect *camera;

	//Tooltip Texture
	LTexture tooltipName;
	LTexture tooltipText;
	LTexture armorIcon;
	int armorSpace = 0;
	LTexture plasmaIcon;
	int plasmaSpace = 0;
	int pYSpace = 0;
	bool isarmor = false;
	TTF_Font *font = NULL;

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