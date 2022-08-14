#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

//Screen Variables
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define SCREEN_FPS 60
#define SCREEN_TICK_PER_FRAME (1000 / SCREEN_FPS)

//Field Variables
#define DUNGEON_SIZE 30 //Number of tiles for x/y
#define TILE_SIZE 50 //Size of tile
#define MAP_TILE_SIZE 20 //Size of minimap tile
#define MAP_SIZE 8 //Map size for x/y
#define FIELD_WIDTH (DUNGEON_SIZE * TILE_SIZE)
#define FIELD_HEIGHT (DUNGEON_SIZE * TILE_SIZE)

#define SCALE 1.7857142857142857142857142857143
#define TILES 1
#define SHADOWS 2
#define OBJECTS 3

//Bullet Types
#define BULLET_ENEMY	0
#define BULLET_PLAYER	1
#define BULLET_REDUC	0.9

//Tile Variables
#define TOTAL_TILES_X (FIELD_WIDTH / TILE_SIZE)  //used for x/y of
#define TOTAL_TILES_Y (FIELD_HEIGHT / TILE_SIZE) //tiles map
#define TOTAL_TILES 100 //Used in tilesheet array

//Tile Numbers
#define TILE_BLACK		0
#define TILE_ROCK		1
#define TILE_GRASS		2
#define TILE_GRASS_ROCK	3
#define TILE_START		98
#define TILE_END		99

//Item Variables
#define ITEM_MAP_SIZE	40
#define ITEM_INV_SIZE	50
#define ITEM_NONE		0
#define ITEM_WEAPON		1
#define ITEM_HEAD		2
#define ITEM_CHEST		3
#define ITEM_LEGS		4
#define ITEM_POTION		5
#define ITEM_RESOURCE	6

//#define COLOR_POOR		150, 150, 150
#define COLOR_COMMON	255, 255, 255
#define COLOR_UNCOMMON	0, 255, 0
#define COLOR_RARE		0, 255, 255
#define COLOR_EPIC		255, 0, 255

//Make sure item and equipment items line up in numbers with difference of 1 between each

//Player Variables
#define MAX_INVENTORY		30
#define MAX_EQUIPMENT		4 //Total amount of equipments
#define EQUIPMENT_WEAPON	0
#define EQUIPMENT_HEAD		1
#define EQUIPMENT_CHEST		2
#define EQUIPMENT_LEGS		3

#define SLOT_NONE			0
#define SLOT_INVENTORY		1
#define SLOT_EQUIPMENT		2

#define PLAYER_SIZE			40

#define TARGET_TIMEOUT		300 //60 = 1 second

//Enemy Variables
#define ENEMY_TYPE_NORMAL	0
#define ENEMY_TYPE_NODE		1

//Button Variables
#define BUTTON_WIDE		0
#define BUTTON_PLUS		1

#endif//GLOBAL_VARIABLES_H
