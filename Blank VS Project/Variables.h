#ifndef VARIABLES_H
#define VARIABLES_H

//Only include this at bottom of other includes.
//Still need to pass these variables to other headers' functions

//Debug variable
bool Debugging = true;

//SDL Handles
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
gTimer FPS;
gTimer capFPS;

//Global Camera
SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

//Textures
LTexture maptiles;
LTexture tilemap;
LTexture shadowmap;
LTexture objectmap;
LTexture blackmap;

LTexture itemmap;
LTexture gText;

LTexture forestmap;
LTexture townmap;

//Lightmap
vector<Light> lights;
Shade shades[TOTAL_TILES_X*4][TOTAL_TILES_Y*4];

//Sprites
Sprite spacebg;

//Fonts
TTF_Font* CodeNewRoman;
TTF_Font* CodeNewRoman16;
TTF_Font* CodeNewRoman18;
TTF_Font* CodeNewRomanBig;

//Font Colors
SDL_Color BlackText = { 0, 0, 0 };
SDL_Color WhiteText = { 255,255,255 };

//Sprite container
Player player(gRenderer, gWindow);
Sprite mapPlayer(gRenderer, gWindow);
Sprite mapOutline(gRenderer, gWindow);
std::vector<Sprite> bgsprites;
std::vector<Sprite> sprites;
int globalCount = 0; //Unique ID for enemies
std::vector<Enemy> enemies;
std::vector<Item> items;
std::vector<Bullet> bullets;
std::vector<FloatingText> texts;

//For saving/loading items
std::vector<Item> globalItems;

//Map variables
int maxRooms = 15, minRooms = 10;
Tile map[MAP_SIZE][MAP_SIZE];
Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y];
SDL_Rect mapsheetstyle = { 0, 0, 50, 50 };
int roomCount = 0;
bool spawnExitPortal = false;
int portalX = 0, portalY = 0;
std::string mapStyle = STYLE_FOREST;
std::string dungeonStyle = "";
Sprite exitPortal(gRenderer, gWindow);

//Item Structs
Bullet laser(gRenderer, gWindow);
Bullet slimeball(gRenderer, gWindow);
Bullet wispball(gRenderer, gWindow);
Bullet bsbullet(gRenderer, gWindow);
Bullet arrow(gRenderer, gWindow);
Weapon defaultGun;
Weapon speedGun;
Weapon slimeGun;
Weapon wispGun;
Weapon fairyGun;
Weapon longGun;
Weapon ballistaGun;

//Items
Item logs;
Item flower;
Item stick;
Item stone;
Item speedRifle;
Item hpotion;
Item epotion;
Item goldOre;
Item goldBar;
Item crystalOre;
Item blueHelm;
Item blueChest;
Item blueLegs;
Item tealHelm;
Item tealChest;
Item tealLegs;
Item bloomweed;
Item roseleaf;
Item energyCannon;
Item blueKey;
Item redKey;

//Crafting Station
CraftingStation weaponStation;

std::stringstream placeText; //Used for tooltips on longer items

//Spells
std::vector<Spell*> globalSpells; //Storage for holding every spell to be cast
std::vector<Spell*> spells; //Storage for spells that are casted

//Enemies
Enemy slime(gRenderer, gWindow);
Enemy wisp(gRenderer, gWindow);
Enemy fairy(gRenderer, gWindow);
Enemy goldNode(gRenderer, gWindow);
Enemy crystalNode(gRenderer, gWindow);
Enemy bloomweedNode(gRenderer, gWindow);
Enemy roseleafNode(gRenderer, gWindow);
Enemy ballista(gRenderer, gWindow);

//Enemy Gens
std::vector<std::pair<Enemy, int>> mapEnemies;
int minEnemies = 0, maxEnemies = 0;
std::vector<std::pair<Enemy, int>> mapResources;
int minResources = 0, maxResources = 0;
std::vector<Enemy> enemyTiers;
std::vector<Item> mapKeys;


//Main menu items
std::vector<Sprite> screenBG;
int mapScreen = SCREEN_MENUS;
int subScreen = SUBSCREEN_MAIN;
LTexture menuimage;
LTexture gamelogo;
int musicVolume = 50;
int soundVolume = 50;
bool refreshSaves = true;

Button exitGame, playGame, optionsButton, backMain;
std::vector<Button*> menuButtons = { &exitGame, &playGame, &optionsButton };
Button omup, omdown, osup, osdown;
std::vector<Button*> optionButtons = { &backMain, &omup, &omdown, &osup, &osdown };

//Miscellaneous Objects
MessageBox *messageBox = NULL;
MessageBox buyChest(gRenderer, gWindow);
MessageBox enterForestPortal(gRenderer, gWindow);
Sprite greenPortal(gRenderer, gWindow);
Sprite darkgreenPortal(gRenderer, gWindow);
Sprite chest(gRenderer, gWindow);
Sprite reactor(gRenderer, gWindow);
Sprite forge(gRenderer, gWindow);

//Music bytes


class TownObject
{
public:
	TownObject() {}
	TownObject(Sprite s, int rad, int xPos, int yPos, bool runs = true)
	{
		runSprite = runs;
		sprite = s;
		sprite.setPosition(xPos, yPos);
		sCol = { xPos - rad, yPos - rad, (int)sprite.getSize().first + rad * 2, (int)sprite.getSize().second + rad * 2 };
	}
	~TownObject() {}

	void run()
	{
		if (runSprite) {
			sprite.nextFrame();
			if (checkCollision(camera, sprite.getBox())) {
				sprite.draw();
			}
		}
		if (!player.canPick)
		{
			if (checkCollision(sCol, player.getBox()))
			{
				player.canPick = true;
			}
		}

		if (checkCollision(sCol, player.getBox()) && player.ekey)
			interacted = true;
		else
			interacted = false;
	}

	bool interacted = false;
	bool runSprite = true;

	int keySlot = -1;

	Sprite sprite;
	SDL_Rect sCol;
};

Sprite whiteDoor(gRenderer, gWindow);
std::vector<TownObject> mapDoors;
std::vector<std::string> mapDoorKeys;
std::vector<std::string> mapDoorColors;
std::vector<PressurePlate> pressurePlates;
std::vector<PressureDoor> pressureDoors;

#endif // VARIABLES_H
