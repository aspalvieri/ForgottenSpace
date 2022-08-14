#ifndef ENTITY_H
#define ENTITY_H

#include "Sprite.h"
#include "Tile.h"
#include "GlobalVariables.h"
#include "Item.h"
#include "Bullet.h"
#include "Button.h"

//=====================================================Entity
class Entity : public Sprite
{
public:
	Entity(SDL_Renderer*, SDL_Window*);
	~Entity();
	
	Entity& setVelocity(double vel);

	double getVelocity();

	void setRenderer(SDL_Renderer*, SDL_Window*);

	virtual void handleEvents();

	virtual void handleEvents(SDL_Event* e);

	virtual void draw();

	//Entity variables
	double health = 0, maxHealth = 0, healthRegen = 0;
	double energy = 0, maxEnergy = 0, energyRegen = 0;
	double armorRating = 0;
	int level = 1;
	bool enableCol = false;

protected:
	//Entities variables
	double velocity = 0;

	//SDL Handlers
	SDL_Window* gWindow = NULL;
	SDL_Renderer* gRenderer = NULL;
};

class Enemy;

//=====================================================Player
class Player : public Entity
{
public:
	Player(SDL_Renderer*, SDL_Window*);
	~Player();

	void handleEvents(SDL_Event* e, std::vector<Item> &items);

	void handleMove(Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y], std::vector<Sprite>&, std::vector<Item>&, std::vector<Enemy>&, std::vector<Bullet> &bullets);

	void draw(SDL_Rect& camera);

	void displayInventory();

	void checkLevel();

	void buildLocalImages();

	void setCamera(SDL_Rect& camera);

	void setEquipment();

	bool inventoryFreeSlot();

	void inventoryAddItem(int); //Adds item from Equipment, takes in Equipment index

	std::vector<Item> *otherInv = NULL;

	int skillPoints = 0;
	int plasma = 0;
	double exp = 0, maxExp = 100;
	int strength = 10; //Increases basic attack dps by 0.7 per strength
	int dexterity = 10; //Affects total energy, energy regen, minor velocity (vel = vel + (dex * 0.005))
	int vitality = 10; //Affects total health and health regen
	//dex/vit rates = stat * 0.0001

	Weapon weapon;
	Weapon defaultWeapon;

	Enemy *targetEnemy = NULL;
	int targetTimer = 0;

	double xVel = 0, yVel = 0, xPos = 0, yPos = 0;
	bool hoverRefresh = false;

private:
	//Movement handles
	bool up, down, left, right, tab, reloading, mouseLeft = false, ekey, canPick = false;
	unsigned int maxCamY, maxCamH, maxCamX, maxCamW;
	int mdiff;
	SDL_Rect mouseClip;
	int timeTicks, reloadTime;

	//Inventory
	bool displayInv;
	bool itemSelected; //Is an item selected
	int slotSelected; //Slot number selected
	LTexture selectedTexture;
	Item inventory[MAX_INVENTORY];
	Item equipment[MAX_EQUIPMENT];
	int slotType = SLOT_INVENTORY; //Selected slot's type
	LTexture inventoryBox;
	SDL_Rect invSheetClip;
	LTexture inventorySheet;
	LTexture inventoryBoxHovered;
	LTexture inventoryBoxClicked;
	LTexture equipmentEmpty;
	LTexture equipmentEmptyHover;
	SDL_Rect eqRect[MAX_EQUIPMENT];
	Sprite tooltip;
	Sprite canPickSprite;

	//Buttons
	Button incSTR, incDEX, incVIT;
	std::vector<Button*> buttons = {&incSTR, &incDEX, &incVIT};

	//Inventory specific variables
	int xoffset = 50;
	int yoffset = 230;
	int space = 55;
	int xslots = 6;

	//Health bar
	LTexture statBar, statBarText, enemyStatBar, expBar;
	TTF_Font *CodeNewRoman, *CodeNewRoman20, *CodeNewRoman16;
	SDL_Rect statClip;
	std::stringstream statText;
};

//=====================================================Enemy
class Enemy : public Entity
{
public:
	Enemy(SDL_Renderer*, SDL_Window*);
	~Enemy();

	void handleDeath(std::vector<Item>&, Player &player);

	virtual void handleMove(Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y], std::vector<Enemy>&, Player &player, std::vector<Bullet>&);

	void setVision(int);

	void setCamera();

	void draw();

	void drawHealth();

	void buildLocalImages();

	SDL_Rect aiCamera;

	int uniqueID, reloadTime = 0, aiType = ENEMY_TYPE_NORMAL;

	int dropChance = 0; // Integer out of 100 (0-100)

	int xMod = 0, yMod = 0, resetMod = 0;

	bool alive, reloading = false, wallWasHit = false;

	bool minRange = true;

	Weapon weapon;

	std::vector<std::pair<Item, int>> dropTable;

	std::string name = "";

	double expReward = 0;

private:
	//Camera handles
	int visionRange;

	double deltaX, deltaY;
	double magnitude;
	double xVel, yVel, xPos, yPos;
	unsigned int maxCamY, maxCamH, maxCamX, maxCamW;
	double diffX, diffY;

	LTexture statBar;
	SDL_Rect statClip;
};

#endif//ENTITY_H
