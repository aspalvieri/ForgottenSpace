#ifndef ENTITY_H
#define ENTITY_H

#include "Sprite.h"
#include "Tile.h"
#include "GlobalVariables.h"
#include "Item.h"
#include "Bullet.h"
#include "Button.h"
#include "Spell.h"

class PressurePlate {
public:
	PressurePlate();
	PressurePlate(int xpos, int ypos, std::string nm);
	~PressurePlate();

	bool active = false;
	SDL_Rect box = { 0, 0, 50, 50 };
	std::string name = "";
};

class PressureDoor {
public:
	PressureDoor();
	PressureDoor(int xpos, int ypos, std::string nm);
	~PressureDoor();

	SDL_Rect box = { 0, 0, 50, 50 };
	std::string name = "";
};

class Effect;

class Spell;

//=====================================================Entity
class Entity : public Sprite
{
public:
	Entity(SDL_Renderer*, SDL_Window*);
	~Entity();
	
	Entity& setVelocity(double vel);

	double getVelocity();

	void setRenderer(SDL_Renderer*, SDL_Window*);

	void setRenderCamera(SDL_Renderer*, SDL_Window*, SDL_Rect*);

	void effectManager(vector<FloatingText> &, TTF_Font*);

	void takeDamage(double dmg, bool toShields = true);

	virtual void handleEvents();

	virtual void handleEvents(SDL_Event* e);

	virtual void draw();

	//Entity variables
	vector<Effect*> effects;
	double health = 0, maxHealth = 0, healthRegen = 0;
	double energy = 0, maxEnergy = 0, energyRegen = 0;
	double shields = 0, shieldsmax = 0;
	double armorRating = 0;
	double armorRatingBonus = 0, velocityBonus = 0;
	int level = 1;
	bool isPlayer = false;

	TTF_Font *codeNewRoman18 = NULL;

	//Entities variables
	double velocity = 0;

	//SDL Handlers
	SDL_Window* gWindow = NULL;
	SDL_Renderer* gRenderer = NULL;
};

class Enemy;

//=====================================================Player Storage

struct ItemStorage
{
	Item items[MAX_STORAGE];
	bool open = false;
};

struct PlayerStorage
{
	char inv[MAX_INVENTORY][24];
	char equip[MAX_EQUIPMENT][24];
	char storage[6][MAX_STORAGE][24];
	char spellBook[5][24];
	bool storageopen[6];
	double health = 0, maxHealth = 0;
	double energy = 0, maxEnergy = 0;
	double exp = 0, maxExp = floor(level + (135 * pow(2, (double)level / 7)));
	int level = 1, skillPoints = 0, plasma = 0;
	int str = 0, dex = 0, vit = 0;
	int spellPoints = 1, damageLevel = 0, defenseLevel = 0;
	int wsLevel = 1, wsExp = 0, wsMaxExp = 100;
};

//=====================================================Player
class Player : public Entity
{
public:
	Player(SDL_Renderer*, SDL_Window*);
	~Player();

	void handleEvents(SDL_Event* e, std::vector<Item> &items, std::vector<FloatingText>&);

	void handleMove(Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y], std::vector<Sprite>&, std::vector<Item>&, std::vector<Enemy>&, std::vector<Bullet> &bullets, std::vector<PressurePlate> &pressurePlates);

	void draw(SDL_Rect& camera, std::vector<FloatingText>&);

	void displayInventory();

	void checkStats();

	void checkLevel();

	void buildLocalImages();

	void setCamera(SDL_Rect& camera);

	void setEquipment();

	void quickHeal(std::vector<FloatingText>&);

	void quickEnergy(std::vector<FloatingText>&);

	bool inventoryFreeSlot();

	void inventoryAddItem(int); //Adds item from Equipment, takes in Equipment index

	void closeInventory();

	void resetPlayer();

	void resetCooldowns();

	bool findSpell(int);

	void castSpell(std::vector<FloatingText> &, int);

	bool offCooldown(std::vector<FloatingText> &, int);

	void saveGame(int);

	void loadGame(int, std::vector<Item>);

	bool checkForSpell(std::string);

	void displaySpellBook(std::vector<FloatingText> &);

	void closeSpells();

	void displayEscapeMenu();

	void closeEscapeMenu();

	bool findItem(std::string iname);

	bool findAndDeleteItem(std::string iname);

	void deleteAllType(int itype);

	void rebuildAttackText();
	LTexture attackText;
	LTexture attackDesc;

	void rebuildInventoryText();
	LTexture infoText;
	std::string infoPage = "Weapon";

	ItemStorage *otherInv = NULL;
	ItemStorage storage[6];

	int skillPoints = 0;
	int plasma = 0;
	double exp = 0, maxExp = floor(level + (135 * pow(2, (double)level / 7)));
	int strength = 5; //Increases attack dps by 0.7 per strength
	int dexterity = 5; //Affects total energy, minor velocity (vel = vel + (dex * 0.005))
	int vitality = 5; //Affects total health
	int legFrame = 0;

	Weapon weapon;
	Weapon defaultWeapon;

	Enemy *targetEnemy = NULL;
	std::vector<Spell*> globalSpells;
	std::vector<Spell*> *spells = NULL;
	std::string spellBook[5];
	int spellCooldown[5][2];

	int spellIndex = 0;
	int targetTimer = 0;

	double xVel = 0, yVel = 0, xPos = 0, yPos = 0;
	bool displaySpells = false;
	bool hoverRefresh = false, levelRefresh = false, reactor = false, pressedReactor = false;
	int reactorSlot = -1;

	//bool selectSpell = false;
	//int spellSlot = -1, clickSlot = -1;

	SDL_Color HelmetColor = { 255, 255, 255 };
	SDL_Color ChestColor = { 255, 255, 255 };
	SDL_Color LegsColor = { 255, 255, 255 };

	PlayerStorage saveFile;

	bool displayInv, killSwitch = false, esckey = false, msgbox = false, displayEscape = false;
	bool up, down, left, right, tab, reloading, mouseLeft = false, mouseRight = false, ekey, canPick = false, hkey = false, ctrl = false;
	bool ckey = false, vkey = false;
	bool inCombat = false;
	int combatTimer = 0;

	Item inventory[MAX_INVENTORY];
	TTF_Font *CodeNewRoman, *CodeNewRomanBig, *CodeNewRoman20, *CodeNewRoman16, *CodeNewRoman15, *CodeNewRoman12;
	SDL_Rect mouseClip;

private:
	//Movement handles
	unsigned int maxCamY, maxCamH, maxCamX, maxCamW;
	int mdiff;
	int timeTicks, reloadTime, yspace, xspace;

	//Inventory
	bool itemSelected; //Is an item selected
	int slotSelected; //Slot number selected
	LTexture selectedTexture;
	Item equipment[MAX_EQUIPMENT];
	int slotType = SLOT_NONE; //Selected slot's type
	LTexture inventoryBox;
	LTexture inventorypure;
	SDL_Rect invSheetClip;
	LTexture inventorySheet;
	LTexture shieldsbar;
	Sprite bubble;
	LTexture healthword;
	LTexture energyword;
	LTexture shieldword;
	LTexture ssText;
	SDL_Rect oinvSheetClip;
	LTexture oinventorySheet;
	LTexture reactorSheet;
	LTexture inventoryBoxHovered;
	LTexture inventoryBoxClicked;
	LTexture unspentBox;
	LTexture unspentSpells;
	LTexture abilityBox;
	LTexture abilityIconRaw;
	LTexture combatText;
	LTexture escapeMenu;
	int aicc[6];
	LTexture equipmentEmpty;
	LTexture equipmentEmptyHover;
	SDL_Rect eqRect[MAX_EQUIPMENT];
	LTexture abilityIcon;
	LTexture abilityRefresh;
	SDL_Rect abRect[8];
	SDL_Rect abRe;
	LTexture basicAttackIcon;
	LTexture iconBar;
	LTexture spellBG;
	LTexture emptyhp;
	LTexture emptyep;
	LTexture itemSheet;
	Sprite tooltip;
	Sprite canPickSprite;
	LTexture armorIcon;
	int armorSpace = 0;
	LTexture plasmaIcon;
	int plasmaSpace = 0;
	LTexture vitalityIcon;
	int vitalitySpace = 0;
	LTexture dexterityIcon;
	int dexteritySpace = 0;
	LTexture strengthIcon;
	int strengthSpace = 0;
	LTexture hpsText;
	LTexture epsText;

	LTexture sbSheet;
	LTexture sbIn, sbIn2;
	LTexture sbOut, sbOut2;
	LTexture sbInH, sbIn2H;
	LTexture desText;
	LTexture defText;
	LTexture desIcon;
	int desSpace = 0;
	LTexture resIcon;
	int resSpace = 0;

	SDL_Rect sexpbarcord;
	LTexture sexpbar;
	SDL_Rect effectBoxCord;
	LTexture effectBox, effectBoxHover;

	LTexture offensiveText;
	LTexture defensiveText;
	LTexture spellPointsText;
	int selectedSpell = -1;
	bool clickedSpell = false;
	bool clickingSpell = false;
	int spellTab = SPELL_TYPE_DAMAGE;

	LTexture levelText;
	LTexture expText;
	LTexture statsText;

	//Buttons
	Button incSTR, incDEX, incVIT, reactorConvert, exitGame, resumeGame, weaponInfo, armorInfo, incDES, incRES;
	std::vector<Button*> buttons = {&incSTR, &incDEX, &incVIT, &reactorConvert, &exitGame, &resumeGame, &weaponInfo, &armorInfo, &incDES, &incRES};

	Button clearSlots[5];

	//Inventory specific variables
	int xoffset = 50;
	int yoffset = 220;
	int space = 55;
	int xslots = 8;
	int oxoffset = 1010;
	int oyoffset = 50;
	int oxslots = 4;

	//Health bar
	LTexture statBar, statBarText, enemyStatBar, expBar;
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

	virtual void handleMove(Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y], std::vector<Sprite>&, std::vector<Enemy>&, Player &player, std::vector<Bullet>&);

	void buildEnemy(int ai, int hp, int ar, double vel, int vis, int xpr, int lvl, string nm, Weapon wp);

	void buildResource(int ai, int hp, int ar, double vel, int vis, int xpr, int lvl, string nm);

	void setSpellcaster(bool ib, int rcd, std::vector<Spell*> *sps);

	void setVision(int);

	void setCamera();

	void draw(vector<FloatingText> &texts);

	void drawHealth();

	void buildLocalImages();

	SDL_Rect aiCamera;

	int uniqueID, reloadTime = 0, aiType = ENEMY_TYPE_NORMAL;

	int dropChance = 0; // Integer out of 100 (0-100)

	int xMod = 0, yMod = 0, resetMod = 0;

	int rotationtime = 0;
	int rotationcd = 0;
	unsigned int currentspell = 0;

	std::vector<Spell*> rotation;
	std::vector<Spell*> *spells = NULL;

	bool alive, reloading = false, wallWasHit = false, playerHitMe = false;

	bool minRange = true, isBoss = false, bigBoss = false;

	Weapon weapon;

	std::vector<std::pair<Item, int>> dropTable;

	std::string name = "";

	std::string doorLink = "";

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
