#ifndef CRAFTING_H
#define CRAFTING_H

#include "Entity.h"

class CraftingItem
{
public:

	bool hasIngredients(Player&);

	std::vector<Item> ingredients; //Items used to make 'item'

	int plasmaCost = 0; //Plasma cost to create item

	int levelReq = 1; //Required Crafting Station level to display item

	int expReward = 0; //Experience gained from crafting

	Item item; //Final product

private:

};

class CraftingStation
{
public:

	void setRenders(SDL_Renderer*, SDL_Window*);

	void buildLocalImages(Player &, int);

	void buildDisplay();

	void handleEvents(SDL_Event *e, std::vector<FloatingText>&);

	void draw();

	std::vector<CraftingItem> items; //List of craftable items

	std::vector<CraftingItem> display; //Items to display on crafting tree (so variables can be placed)

	bool hasItems = false; //If true, will only show items player can craft

	bool hasLevel = true; //If true, will only show items player has level for

	bool visible = false; //For town objects to interact with

	std::string name = ""; //Name of crafting station displayed at top

	//EXP and Level is pulled from player.saveFile.xxx.exp, expMax, level
	int *exp = NULL, *expMax = NULL, *level = NULL;

	int page = 1, maxPage = 1, screen = 0;

	int slotSelected = -1;

	SDL_Rect mouseClip; //Storage for mouse

private:

	CraftingItem chosen;

	Button back, craft, pageLeft, pageRight, matsOn, matsOff, levelOn, levelOff;

	LTexture background, boxBar;
	Sprite tooltip;
	SDL_Rect backgroundClip;

	SDL_Rect sexpbarcord;
	LTexture inventoryBox, inventoryBoxHovered, inventoryBoxClicked, plasmaIcon, sexpbar;
	int plasmaSpace = 0;

	//Text Textures
	LTexture nameTexture, levelTexture, expTexture, text1, text2, text3, text4, pageText, text5, text6;

	TTF_Font *font, *fontSmall;

	Player *player = NULL;
	
	//SDL Handlers
	SDL_Window* gWindow = NULL;
	SDL_Renderer* gRenderer = NULL;
};

#endif//CRAFTING_H