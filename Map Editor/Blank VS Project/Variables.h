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
SDL_Rect camera = { 0, 0, SCREEN_WIDTH, 900 };

bool showCol = true;

//Textures
LTexture tilemap;
LTexture shadowmap;
LTexture objectmap;

LTexture itemmap;
LTexture gText;

//Fonts
TTF_Font* CodeNewRoman;
TTF_Font* CodeNewRoman16;

//Font Colors
SDL_Color BlackText = { 0, 0, 0 };
SDL_Color WhiteText = { 255,255,255 };

//Sprite container
Player player(gRenderer, gWindow);
std::vector<Sprite> sprites;
int globalCount = 0; //Unique ID for enemies
std::vector<Enemy> enemies;
std::vector<Item> items;
std::vector<Bullet> bullets;

//For saving/loading items
std::vector<Item> globalItems;

//Map variables
int maxRooms = 15, minRooms = 8;
Tile map[MAP_SIZE][MAP_SIZE];
Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y];
SDL_Rect tilesheet[TOTAL_TILES];
int roomCount = 0;

//Item Structs
Bullet laser(gRenderer, gWindow);
Weapon defaultGun;
Weapon speedGun;
Weapon longGun;

//Items
Item logs;
Item flower;
Item stick;
Item stone;
Item speedRifle;
Item hpotion;
Item goldOre;
Item blueHelm;
Item blueChest;
Item blueLegs;
Item tealHelm;
Item tealChest;
Item tealLegs;

std::stringstream placeText; //Used for tooltips on longer items

//Enemies
Enemy enemy(gRenderer, gWindow);
Enemy goldNode(gRenderer, gWindow);

#endif // VARIABLES_H
