#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

//Screen Variables
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 768

#define SCREEN_MENUS	0
#define SCREEN_INGAME	1

#define SUBSCREEN_MAIN		0
#define SUBSCREEN_OPTIONS	1

#define SCREEN_FPS 60
#define SCREEN_TICK_PER_FRAME (1000 / SCREEN_FPS)

//Field Variables
#define DUNGEON_SIZE 30 //Number of tiles for x/y
#define TILE_SIZE 50 //Size of tile
#define TILE_HALF 25
#define TILE_QUART 12.5
#define MAP_TILE_SIZE 20 //Size of minimap tile
#define MAP_SIZE 8 //Map size for x/y
#define FIELD_WIDTH (DUNGEON_SIZE * TILE_SIZE * MAP_SIZE)
#define FIELD_HEIGHT (DUNGEON_SIZE * TILE_SIZE * MAP_SIZE)

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

//Item Variables
#define ITEM_BOX_SIZE	50
#define ITEM_MAP_SIZE	40
#define ITEM_INV_SIZE	40
#define ITEM_NONE		0
#define ITEM_WEAPON		1
#define ITEM_HEAD		2
#define ITEM_CHEST		3
#define ITEM_LEGS		4
#define ITEM_POTION		5
#define ITEM_RESOURCE	6
#define ITEM_JUNK		7
#define ITEM_KEY		8

#define COLOR_POOR		175, 175, 175
#define COLOR_COMMON	255, 255, 255
#define COLOR_UNCOMMON	0, 255, 0
#define COLOR_RARE		0, 255, 255
#define COLOR_EPIC		255, 0, 255

#define COLOR_ARMORBUFF 0,255,64

//Map Style Variables
#define STYLE_TOWN		"town"
#define STYLE_FOREST	"forest"

//Make sure item and equipment items line up in numbers with difference of 1 between each

//Player Variables
#define MAX_INVENTORY		40
#define MAX_STORAGE			20
#define MAX_EQUIPMENT		4 //Total amount of equipments
#define EQUIPMENT_WEAPON	0
#define EQUIPMENT_HEAD		1
#define EQUIPMENT_CHEST		2
#define EQUIPMENT_LEGS		3

#define MAX_SPELLS			10

#define SLOT_NONE			0
#define SLOT_INVENTORY		1
#define SLOT_EQUIPMENT		2
#define SLOT_STORAGE		3

#define STATION_WEAPON		1

#define PLAYER_SIZE			40

#define TARGET_TIMEOUT		300 //60 = 1 second

#define SPELL_TYPE_NONE		0
#define SPELL_TYPE_DAMAGE	1
#define SPELL_TYPE_DEFENSE	2

#define VIT_BONUS		7.5
#define DEX_BONUS		4.5
#define STR_BONUS		0.5
#define SPELL_BONUS		STR_BONUS * 5
#define COST_INC		SPELL_BONUS * 0.55

#define BASE_ENERGY			50
#define BASE_ENERGY_REGEN	0.8
#define BASE_HEALTH			100
#define BASE_HEALTH_REGEN	0.3
#define BASE_HEALTH_MULTIPLYER	0.01

//Enemy Variables
#define ENEMY_TYPE_NORMAL	0
#define ENEMY_TYPE_NODE		1

#define ADVENTURE_TYPE_NONE		0
#define ADVENTURE_TYPE_KILL		1

//Button Variables
#define BUTTON_WIDE				0
#define BUTTON_PLUS				1
#define BUTTON_MINUS			4
#define BUTTON_ARROW_LEFT		2
#define BUTTON_ARROW_RIGHT		3

#define MESSAGE_YESNO	0
#define MESSAGE_OK		1

#endif//GLOBAL_VARIABLES_H
