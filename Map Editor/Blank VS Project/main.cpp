#include "GlobalVariables.h"
#include "Tile.h"
#include "Sprite.h"
#include "Entity.h"
#include "gTimer.h"

//Only local declarations, still need to pass variables to other header files
#include "Variables.h"

//Requires Variables
#include "MapGen.h"
#include "InitClose.h"
#include "HandleBulletHit.h"
#include "BuildItems.h"
#include "BuildStructs.h"

#define MAX_UNDO 100

class TileHold
{
public:
	TileClip l[3][TOTAL_TILES_X][TOTAL_TILES_Y];
};

std::vector<TileHold> tileLog;

void saveChunk(std::string path)
{
	std::fstream RAF;
	RAF.open(path.c_str(), std::ios::out | std::ios::binary);

	for (int y = 0; y < DUNGEON_SIZE; y++)
	{
		for (int i = 0; i < DUNGEON_SIZE; i++)
		{
			TileClip temp(tiles[i][y].clip.x, tiles[i][y].clip.y, tiles[i][y].getCollide());
			RAF.write(reinterpret_cast<const char*>(&temp), sizeof(TileClip));
			temp = TileClip(tiles[i][y].sclip.x, tiles[i][y].sclip.y, tiles[i][y].getCollide());
			RAF.write(reinterpret_cast<const char*>(&temp), sizeof(TileClip));
			temp = TileClip(tiles[i][y].oclip.x, tiles[i][y].oclip.y, tiles[i][y].getCollide());
			RAF.write(reinterpret_cast<const char*>(&temp), sizeof(TileClip));
		}
	}
	RAF.close();
	std::cout << "Saved chunk to: " << path << "\n";
}

int main(int argc, char* args[]) 
{
	if (!init()) { printf("Failed to initialize!\n"); }

	//Make this a splash image?
	SDL_SetRenderDrawColor(gRenderer, 0x1F, 0x1F, 0x1F, 0xFF);
	SDL_RenderClear(gRenderer);
	SDL_RenderPresent(gRenderer);

	//Load Textures
	tilemap.loadFromFile("bin/images/tiles.png", gRenderer);
	shadowmap.loadFromFile("bin/images/shadows.png", gRenderer);
	objectmap.loadFromFile("bin/images/objects.png", gRenderer);

	itemmap.loadFromFile("bin/images/itemsheet.png", gRenderer);

	//Load Font
	CodeNewRoman = TTF_OpenFont("bin/fonts/codenewroman.ttf", 24);
	CodeNewRoman16 = TTF_OpenFont("bin/fonts/codenewroman.ttf", 16);

	bool quit = false;
	SDL_Event e;

	buildStructs();
	buildItems();

	//Sets tilesheet variables
	tilesheet[TILE_BLACK] =		SDL_Rect{ 200, 0, 50, 50 };
	tilesheet[TILE_ROCK] =		SDL_Rect{ 50, 50, 50, 50 };
	tilesheet[TILE_GRASS] =		SDL_Rect{ 0, 0, 50, 50 };
	tilesheet[TILE_GRASS_ROCK]= SDL_Rect{ 0, 50, 50, 50 };

	tilesheet[TILE_START] =		SDL_Rect{ 150, 0, 50, 50 };
	tilesheet[TILE_END] =		SDL_Rect{ 150, 50, 50, 50 };

	//Player is declared in Variables.h
	player.setRenderer(gRenderer, gWindow);
	player.camera = &camera;
	player.loadSpriteImage("bin/images/example.png")
		.setFrameSize(50, 50)
		.setPosition(300, 300)
		.setDelay(5)
		//.setAlpha(32)
		.setSize(PLAYER_SIZE, PLAYER_SIZE)
		.setCollide(true)
		.pushFrameRow("Idle", 0, 0, 50, 0, 4)
		.pushFrameRow("Idle", 0, 50, 50, 0, 4)
		.setAnimation("Idle");
	player.setVelocity(10);
	player.health = 200;
	player.maxHealth = 200;
	player.energy = 100;
	player.maxEnergy = 100;
	player.buildLocalImages();

	Sprite example(gRenderer, gWindow);
	//example.camera = &camera;
	example.loadSpriteImage("bin/images/example.png")
		.setFrameSize(50, 50)
		.setPosition(550, 100)
		.setDelay(10)
		.setSize(MAP_TILE_SIZE, MAP_TILE_SIZE)
		.pushFrameRow("Start", 0, 0, 50, 0, 4)
		.pushFrameRow("Start", 0, 50, 50, 0, 4)
		.setAnimation("Start");

	player.weapon = defaultGun;
	player.defaultWeapon = defaultGun;


	FPS.start();
	double avgFPS = 0;
	int countedFrames = 0;
	
	//Try to proc gen?
	//generateMap();

	clearMap();
	/*
	for (int v = 0; v < DUNGEON_SIZE; v++)
	{
		for (int i = 0; i < DUNGEON_SIZE; i++)
		{
			tiles[i][v] = tiles[i][v] = Tile(i * TILE_SIZE, v * TILE_SIZE, tilesheet[TILE_GRASS], false, tilemap, camera);
		}
	}
	*/
	loadChunk(0, 0, "bin/temp.dat");

	int maxCamY, maxCamH, maxCamX, maxCamW;
	SDL_Rect mouseClip = { 0, 0, 0, 0 };

	Sprite tilepic(gRenderer, gWindow);
	tilepic.loadSpriteImage("bin/images/tiles.png")
		.setFrameSize(500, 500)
		.setPosition(0, 0)
		.setSize(280, 280)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");

	Sprite shadowpic(gRenderer, gWindow);
	shadowpic.loadSpriteImage("bin/images/shadows.png")
		.setFrameSize(250, 250)
		.setPosition(0, 0)
		.setSize(140, 140)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");

	Sprite objectpic(gRenderer, gWindow);
	objectpic.loadSpriteImage("bin/images/objects.png")
		.setFrameSize(400, 250)
		.setPosition(0, 0)
		.setSize(224, 140)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");

	Sprite tilebox(gRenderer, gWindow);
	tilebox.loadSpriteImage("bin/images/iconbox.png")
		.setFrameSize(120, 120)
		.setPosition(0, 0)
		.setSize(28, 28)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");

	Sprite tileboxright(gRenderer, gWindow);
	tileboxright.loadSpriteImage("bin/images/iconboxright.png")
		.setFrameSize(120, 120)
		.setPosition(0, 0)
		.setSize(28, 28)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");

	Sprite tileboxboth(gRenderer, gWindow);
	tileboxboth.loadSpriteImage("bin/images/iconboxboth.png")
		.setFrameSize(120, 120)
		.setPosition(0, 0)
		.setSize(28, 28)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");

	bool cancol = false, disp = true, cancolr = false;
	bool leftm = false, rightm = false;

	bool ctrl = false, keyz = false, keyy = false;

	LTexture red, green;

	red.loadFromFile("bin/images/red.png", gRenderer);
	red.setAlpha(16);
	green.loadFromFile("bin/images/green.png", gRenderer);
	green.setAlpha(16);

	int sheet = TILES;

	int tindex = 0;

	bool placedTile = false;

	TileHold t;
	for (int y = 0; y < TOTAL_TILES_Y; y++)
	{
		for (int x = 0; x < TOTAL_TILES_X; x++)
		{
			t.l[0][x][y].setAll(tiles[x][y].clip.x, tiles[x][y].clip.y, tiles[x][y].getCollide());
			t.l[1][x][y].setClip(tiles[x][y].sclip.x, tiles[x][y].sclip.y);
			t.l[2][x][y].setClip(tiles[x][y].oclip.x, tiles[x][y].oclip.y);
		}
	}
	tileLog.push_back(t);
	
	while (!quit)
	{
		//Start capFPS
		capFPS.start();

		//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		//Handle Events
		while (SDL_PollEvent(&e) != 0)
		{
			//Handle events for player
			player.handleEvents(&e, items);

			//User presses a key down
			if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = true;
					break;
				case SDLK_LCTRL:
					ctrl = true;
					break;
				case SDLK_z:
				{
					keyz = true;
					if (ctrl && keyz && tileLog.size() > 1 && tindex > 0)
					{
						tindex--;
						for (int y = 0; y < TOTAL_TILES_Y; y++)
						{
							for (int x = 0; x < TOTAL_TILES_X; x++)
							{
								tiles[x][y].clip.x = tileLog[tindex].l[0][x][y].clipx;
								tiles[x][y].clip.y = tileLog[tindex].l[0][x][y].clipy;
								tiles[x][y].setCollide(tileLog[tindex].l[0][x][y].col);
								tiles[x][y].sclip.x = tileLog[tindex].l[1][x][y].clipx;
								tiles[x][y].sclip.y = tileLog[tindex].l[1][x][y].clipy;
								tiles[x][y].oclip.x = tileLog[tindex].l[2][x][y].clipx;
								tiles[x][y].oclip.y = tileLog[tindex].l[2][x][y].clipy;
							}
						}
					}
					break;
				}
				case SDLK_y:
				{
					keyy = true;
					if (ctrl && keyy && tindex < (int)tileLog.size() - 1)
					{
						tindex++;
						for (int y = 0; y < TOTAL_TILES_Y; y++)
						{
							for (int x = 0; x < TOTAL_TILES_X; x++)
							{
								tiles[x][y].clip.x = tileLog[tindex].l[0][x][y].clipx;
								tiles[x][y].clip.y = tileLog[tindex].l[0][x][y].clipy;
								tiles[x][y].setCollide(tileLog[tindex].l[0][x][y].col);
								tiles[x][y].sclip.x = tileLog[tindex].l[1][x][y].clipx;
								tiles[x][y].sclip.y = tileLog[tindex].l[1][x][y].clipy;
								tiles[x][y].oclip.x = tileLog[tindex].l[2][x][y].clipx;
								tiles[x][y].oclip.y = tileLog[tindex].l[2][x][y].clipy;
							}
						}
					}
					break;
				}
				case SDLK_0:
				{
#define NEWSCALE 0.54347826086956521739130434782609
					for (int y = 0; y < TOTAL_TILES_Y; y++)
					{
						for (int x = 0; x < TOTAL_TILES_X; x++)
						{
							tiles[x][y].clip = SDL_Rect{ (int)((double)tiles[x][y].clip.x * NEWSCALE), (int)((double)tiles[x][y].clip.y * NEWSCALE), 50, 50 };
							tiles[x][y].sclip = SDL_Rect{ (int)((double)tiles[x][y].sclip.x * NEWSCALE), (int)((double)tiles[x][y].sclip.y * NEWSCALE), 50, 50 };
							tiles[x][y].oclip = SDL_Rect{ (int)((double)tiles[x][y].oclip.x * NEWSCALE), (int)((double)tiles[x][y].oclip.y * NEWSCALE), 50, 50 };
						}
					}
					break;
				}
				case SDLK_t:
					player.enableCol = !player.enableCol;
					break;
				case SDLK_h:
					showCol = !showCol;
					break;
				case SDLK_TAB:
				case SDLK_e:
					disp = !disp;
					break;
				case SDLK_f:
					cancol = !cancol;
					break;
				case SDLK_g:
					cancolr = !cancolr;
					break;
				case SDLK_1:
					sheet = TILES;
					tilebox.setPosition(0, 0);
					tileboxright.setPosition(0, 0);
					break;
				case SDLK_2:
					sheet = SHADOWS;
					tilebox.setPosition(0, 0);
					tileboxright.setPosition(0, 0);
					break;
				case SDLK_3:
					sheet = OBJECTS;
					tilebox.setPosition(0, 0);
					tileboxright.setPosition(0, 0);
					break;
				case SDLK_k:
				{
					std::cout << "Loading Tilemap (x to cancel)... File name: ";
					std::string cs;
					std::stringstream cx;
					cx << "bin/images/";
					std::cin >> cs;
					if (cs == "x")
					{
						std::cout << "Loading cancelled...\n";
						break;
					}
					cx << cs << ".png";
					tilemap.loadFromFile(cx.str(), gRenderer);
					for (int y = 0; y < TOTAL_TILES_Y; y++)
					{
						for (int x = 0; x < TOTAL_TILES_X; x++)
						{
							tiles[x][y].texture = tilemap;
						}
					}
					tilepic.loadSpriteImage(cx.str())
						.setFrameSize(500, 500)
						.setPosition(0, 0)
						.setSize(280, 280)
						.pushFrame("Idle", 0, 0)
						.setAnimation("Idle");
					break;
				}
				case SDLK_o:
				{
					std::cout << "Saving (x to cancel)... File name: ";
					std::string cs;
					std::stringstream cx;
					cx << "bin/";
					std::cin >> cs;
					if (cs == "x")
					{
						std::cout << "Saving cancelled...\n";
						break;
					}
					cx << cs << ".dat";
					saveChunk(cx.str());
					break;
				}
				case SDLK_m:
				{
					std::cout << "Loading (x to cancel)... File name: ";
					std::string cs;
					std::stringstream cx;
					cx << "bin/";
					std::cin >> cs;
					if (cs == "x")
					{
						std::cout << "Loading cancelled...\n";
						break;
					}
					cx << cs << ".dat";
					loadChunk(0, 0, cx.str());
					tindex++;
					if (tindex > MAX_UNDO)
					{
						tileLog.erase(tileLog.begin());
						tindex--;
					}
					TileHold t;
					for (int y = 0; y < TOTAL_TILES_Y; y++)
					{
						for (int x = 0; x < TOTAL_TILES_X; x++)
						{
							t.l[0][x][y].setAll(tiles[x][y].clip.x, tiles[x][y].clip.y, tiles[x][y].getCollide());
							t.l[1][x][y].setClip(tiles[x][y].sclip.x, tiles[x][y].sclip.y);
							t.l[2][x][y].setClip(tiles[x][y].oclip.x, tiles[x][y].oclip.y);
						}
					}
					if (tindex < (int)tileLog.size())
					{
						tileLog[tindex] = t;
						tileLog.erase(tileLog.begin() + tindex + 1, tileLog.end());
					}
					else
						tileLog.push_back(t);
					break;
				}
				default:
					break;
				}
			}

			if (e.type == SDL_KEYUP)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_LCTRL:
					ctrl = false;
					break;
				case SDLK_z:
					keyz = false;
					break;
				case SDLK_y:
					keyz = false;
					break;
				default:
					break;
				}
			}

			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (e.button.button == SDL_BUTTON_LEFT)
				{
					leftm = true;
				}
				if (e.button.button == SDL_BUTTON_RIGHT)
				{
					rightm = true;
				}
			}

			if (e.type == SDL_MOUSEBUTTONUP)
			{
				if (e.button.button == SDL_BUTTON_LEFT)
				{
					leftm = false;
				}
				if (e.button.button == SDL_BUTTON_RIGHT)
				{
					rightm = false;
				}
				if (placedTile)
				{
					placedTile = false;
					tindex++;
					if (tindex > MAX_UNDO)
					{
						tileLog.erase(tileLog.begin());
						tindex--;
					}
					TileHold t;
					for (int y = 0; y < TOTAL_TILES_Y; y++)
					{
						for (int x = 0; x < TOTAL_TILES_X; x++)
						{
							t.l[0][x][y].setAll(tiles[x][y].clip.x, tiles[x][y].clip.y, tiles[x][y].getCollide());
							t.l[1][x][y].setClip(tiles[x][y].sclip.x, tiles[x][y].sclip.y);
							t.l[2][x][y].setClip(tiles[x][y].oclip.x, tiles[x][y].oclip.y);
						}
					}
					if (tindex < (int)tileLog.size())
					{
						tileLog[tindex] = t;
						tileLog.erase(tileLog.begin() + tindex + 1, tileLog.end());
					}
					else
						tileLog.push_back(t);
				}
			}
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		if (leftm)
		{
			SDL_GetMouseState(&mouseClip.x, &mouseClip.y);
			bool debounce = false;
			if (disp)
			{
				if ((checkCollision(mouseClip, tilepic.getBox()) && sheet == TILES) || (checkCollision(mouseClip, shadowpic.getBox()) && sheet == SHADOWS) || (checkCollision(mouseClip, objectpic.getBox()) && sheet == OBJECTS))
				{
					int x = (double)mouseClip.x / 28;
					int y = (double)mouseClip.y / 28;
					tilebox.setPosition(28 * x, 28 * y);
					debounce = true;
				}
			}
			if (!debounce)
			{
				placedTile = true;
				mouseClip = { mouseClip.x + camera.x, mouseClip.y + camera.y, 0, 0 };
				for (int v = 0; v < DUNGEON_SIZE; v++)
				{
					for (int i = 0; i < DUNGEON_SIZE; i++)
					{
						if (checkCollision(mouseClip, tiles[i][v].getBox()))
						{
							if (sheet == TILES)
							{
								SDL_Rect g = { (int)(tilebox.getBox().x * SCALE), (int)(tilebox.getBox().y * SCALE), 50, 50 };
								tiles[i][v] = Tile(i * TILE_SIZE, v * TILE_SIZE, g, cancol, tilemap, shadowmap, objectmap, camera);
							}
							if (sheet == SHADOWS)
							{
								SDL_Rect g = { (int)(tilebox.getBox().x * SCALE), (int)(tilebox.getBox().y * SCALE), 50, 50 };
								tiles[i][v].sclip = g;
							}
							if (sheet == OBJECTS)
							{
								SDL_Rect g = { (int)(tilebox.getBox().x * SCALE), (int)(tilebox.getBox().y * SCALE), 50, 50 };
								tiles[i][v].oclip = g;
							}
						}
					}
				}
			}
		}
		if (rightm)
		{
			SDL_GetMouseState(&mouseClip.x, &mouseClip.y);
			bool debounce = false;
			if (disp)
			{
				if ((checkCollision(mouseClip, tilepic.getBox()) && sheet == TILES) || (checkCollision(mouseClip, shadowpic.getBox()) && sheet == SHADOWS) || (checkCollision(mouseClip, objectpic.getBox()) && sheet == OBJECTS))
				{
					int x = (double)mouseClip.x / 28;
					int y = (double)mouseClip.y / 28;
					tileboxright.setPosition(28 * x, 28 * y);
					debounce = true;
				}
			}
			if (!debounce)
			{
				placedTile = true;
				mouseClip = { mouseClip.x + camera.x, mouseClip.y + camera.y, 0, 0 };
				for (int v = 0; v < DUNGEON_SIZE; v++)
				{
					for (int i = 0; i < DUNGEON_SIZE; i++)
					{
						if (checkCollision(mouseClip, tiles[i][v].getBox()))
						{
							if (sheet == TILES)
							{
								SDL_Rect g = { (int)(tileboxright.getBox().x * SCALE), (int)(tileboxright.getBox().y * SCALE), 50, 50 };
								tiles[i][v] = Tile(i * TILE_SIZE, v * TILE_SIZE, g, cancolr, tilemap, shadowmap, objectmap, camera);
							}
							if (sheet == SHADOWS)
							{
								SDL_Rect g = { (int)(tileboxright.getBox().x * SCALE), (int)(tileboxright.getBox().y * SCALE), 50, 50 };
								tiles[i][v].sclip = g;
							}
							if (sheet == OBJECTS)
							{
								SDL_Rect g = { (int)(tileboxright.getBox().x * SCALE), (int)(tileboxright.getBox().y * SCALE), 50, 50 };
								tiles[i][v].oclip = g;
							}
						}
					}
				}
			}
		}

		//Calculate and fix FPS issues
		avgFPS = countedFrames / (FPS.getTicks() / 1000.f);
		if (avgFPS > 2000000 || std::isnan(avgFPS))
			avgFPS = 0;

		//Draw below here

		//Sprite Header Handler

		maxCamY = (int)ceil(camera.y / TILE_SIZE);
		maxCamH = (int)(((ceil(camera.y / TILE_SIZE) + ceil(camera.h / TILE_SIZE)) < TOTAL_TILES_Y - 1) ? ceil(camera.y / TILE_SIZE) + ceil(camera.h / TILE_SIZE) + 1 : ceil(camera.y / TILE_SIZE) + ceil(camera.h / TILE_SIZE));
		maxCamX = (int)ceil(camera.x / TILE_SIZE);
		maxCamW = (int)(((ceil(camera.x / TILE_SIZE) + ceil(camera.w / TILE_SIZE)) < TOTAL_TILES_X - 1) ? ceil(camera.x / TILE_SIZE) + ceil(camera.w / TILE_SIZE) + 1 : ceil(camera.x / TILE_SIZE) + ceil(camera.w / TILE_SIZE));

		//Draw tiles
		for (int y = maxCamY; y < TOTAL_TILES_Y; y++)
		{
			for (int x = 0; x < TOTAL_TILES_X; x++)
			{
				tiles[x][y].draw(gRenderer);
				if (showCol) {
					if (tiles[x][y].getCollide() == true)
						red.render(gRenderer, tiles[x][y].box.x - camera.x, tiles[x][y].box.y - camera.y);
					else if (tiles[x][y].getCollide() == false)
						green.render(gRenderer, tiles[x][y].box.x - camera.x, tiles[x][y].box.y - camera.y);
				}
			}
		}

		//Handles for the Player
		player.nextFrame();
		player.handleMove(tiles, sprites, items, enemies, bullets);
		player.draw(camera);

		if (disp)
		{
			if (sheet == TILES)
				tilepic.draw();
			if (sheet == SHADOWS)
				shadowpic.draw();
			if (sheet == OBJECTS)
				objectpic.draw();
			if (tilebox.getBox().x == tileboxright.getBox().x && tilebox.getBox().y == tileboxright.getBox().y)
			{
				tileboxboth.setPosition(tilebox.getBox().x, tilebox.getBox().y);
				tileboxboth.draw();
			}
			else
			{
				tilebox.draw();
				tileboxright.draw();
			}
		}


		//Code After Sprites
		/*
		if (example.finishedAnimation() == true)
		{
			//Setting the animation will set the frame and ticks to 0
			if (example.getCurrentAnimation() == "Start")
				example.setAnimation("Stop");
			else
				example.setAnimation("Start");
		}
		*/
		//std::cout << example.getFrame() << "\t";

		int yposf = 0;
		std::stringstream text;
		if (disp)
		{
			yposf = tilepic.getBox().h;
			text << "(f) Collide Left: ";
			if (cancol)
				text << "True";
			else
				text << "False";
			text << "\n(g) Collide Right: ";
			if (cancolr)
				text << "True";
			else
				text << "False";
			text << "\n(1) Tiles\n(2) Shadows\n(3) Objects";
			text << "\n(k) Select Tilemap";
			text << "\n(o) Save Map\n(m) Load Map\n(t) Toggle Collision\n(h) Toggle Tile Color";
			text << "\n(e) Hide Menu";
		}
		else
		{
			text << "(e) Show Menu";
		}
		gText.loadFromRenderedText(gRenderer, text.str(), WhiteText, CodeNewRoman16, 1000);
		gText.render(gRenderer, 0, yposf, NULL, 0.0, NULL, SDL_FLIP_NONE);
		
		countedFrames++;

		//Cap FPS
		if (avgFPS > (SCREEN_FPS + 1))
		{
			int frameTicks = capFPS.getTicks();
			if (frameTicks < SCREEN_TICK_PER_FRAME)
			{
				SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
			}
		}

		//Update screen
		SDL_RenderPresent(gRenderer);
	}

	saveChunk("bin/temp.dat");

	close();

	return 0;
}
