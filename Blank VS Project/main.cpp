#include "GlobalVariables.h"
#include "Tile.h"
#include "Sprite.h"
#include "Entity.h"
#include "Crafting.h"
#include "gTimer.h"

//Only local declarations, still need to pass variables to other header files
#include "Variables.h"

int Spell::globalID = 0;
int TimeClock::hour = 8; //default 8
double TimeClock::minute = 0;
double TimeClock::speed = 0.1; //default 0.1
Uint8 TimeClock::maxAlpha = 0;
double TimeClock::alpha = 0;
int TimeClock::maxDay = 0;
int TimeClock::maxNight = 255;

//Requires Variables
#include "BuildItems.h"
#include "SpriteDecs.h"
#include "MapGen.h"
#include "InitClose.h"
#include "HandleBulletHit.h"
#include "BuildStructs.h"
#include "TownFunctions.h"
#include "BuildSpells.h"

void saveVolume() {
	ofstream vcon("bin/saves/volume.txt");
	vcon << musicVolume << " " << soundVolume;
	vcon.close();
	int mvolume = 32.0 * (musicVolume / 100.0);
	Mix_VolumeMusic(mvolume);
}

void buildSaves() {
	if (refreshSaves) {
		refreshSaves = false;
	}
}

int main(int argc, char* argv[])
{
	if (!init()) { printf("Failed to initialize!\n"); }

	LTexture loadingscreen;
	loadingscreen.loadFromFile("bin/images/loadingscreen.png", gRenderer);
	//Make this a splash image?
	SDL_SetRenderDrawColor(gRenderer, 0x1F, 0x1F, 0x1F, 0xFF);
	SDL_RenderClear(gRenderer);
	loadingscreen.render(gRenderer, 0, 0);
	SDL_RenderPresent(gRenderer);
	
	//Create cursor
	std::string currCursor = "Menu";
	SDL_Surface* cursorIdle;
	cursorIdle = IMG_Load("bin/images/cursor.png");
	SDL_Surface* cursorShoot;
	cursorShoot = IMG_Load("bin/images/cursorshoot.png");
	SDL_Cursor* cursor = SDL_CreateColorCursor(cursorIdle, 0, 0);
	SDL_SetCursor(cursor);

	//Load Textures
	forestmap.loadFromFile("bin/images/tilesforest.png", gRenderer);
	townmap.loadFromFile("bin/images/tilestown.png", gRenderer);

	maptiles.loadFromFile("bin/images/maptiles.png", gRenderer);
	shadowmap.loadFromFile("bin/images/shadows.png", gRenderer);
	objectmap.loadFromFile("bin/images/objects.png", gRenderer);
	itemmap.loadFromFile("bin/images/itemsheet.png", gRenderer);
	blackmap.loadFromFile("bin/images/blackmap.png", gRenderer);

	tilemap = townmap; //Fail-safe
	mapsheetstyle = { 0, 0, 50, 50 }; // ^^ lmao same

	//Load Font
	CodeNewRoman16 = TTF_OpenFont("bin/fonts/codenewroman.ttf", 16);
	CodeNewRoman18 = TTF_OpenFont("bin/fonts/codenewroman.ttf", 18);
	CodeNewRoman = TTF_OpenFont("bin/fonts/codenewroman.ttf", 24);
	CodeNewRomanBig = TTF_OpenFont("bin/fonts/codenewroman.ttf", 32);

	bool quit = false;
	SDL_Event e;

	buildStructs();
	buildItems();
	buildSprites();
	buildTownObjects();
	buildSpells();

	exitGame.setRender(gRenderer, gWindow);
	exitGame.buildButton(BUTTON_WIDE, SDL_Rect{ 521, 550, 235, 45 }, "Exit Game");

	backMain.setRender(gRenderer, gWindow);
	backMain.buildButton(BUTTON_WIDE, SDL_Rect{ 521, 550, 235, 45 }, "Back");

	playGame.setRender(gRenderer, gWindow);
	playGame.buildButton(BUTTON_WIDE, SDL_Rect{ 521, 254, 235, 45 }, "Play Game");

	optionsButton.setRender(gRenderer, gWindow);
	optionsButton.buildButton(BUTTON_WIDE, SDL_Rect{ 521, 304, 235, 45 }, "Options");

	omup.setRender(gRenderer, gWindow);
	omup.buildButton(BUTTON_PLUS, SDL_Rect{ 731, 295, 30, 30 });

	omdown.setRender(gRenderer, gWindow);
	omdown.buildButton(BUTTON_MINUS, SDL_Rect{ 521, 295, 30, 30 });

	osup.setRender(gRenderer, gWindow);
	osup.buildButton(BUTTON_PLUS, SDL_Rect{ 731, 335, 30, 30 });

	osdown.setRender(gRenderer, gWindow);
	osdown.buildButton(BUTTON_MINUS, SDL_Rect{ 521, 335, 30, 30 });

	ifstream vcon("bin/saves/volume.txt");
	if (vcon.is_open()) {
		vcon >> musicVolume >> soundVolume;
		vcon.close();
	}

	//The player's light
	lights.push_back(Light(2250, 2250, 1.7, 2.35));
	
	if (Debugging)
	{
		//Screen resolution
		std::cout << "Screen Size: " << SCREEN_WIDTH << "x" << SCREEN_HEIGHT << "\n";

		//Base SDL Version
		SDL_version v;
		SDL_GetVersion(&v);
		std::stringstream sdlv;
		sdlv << "SDL Version: " << (int)v.major << "." << (int)v.minor << "." << (int)v.patch << "\n";
		std::cout << sdlv.str();

		//SDL_image Version
		std::stringstream sdliv;
		sdliv << "SDL_image Version: " << SDL_IMAGE_MAJOR_VERSION << "." << SDL_IMAGE_MINOR_VERSION << "." << SDL_IMAGE_PATCHLEVEL << "\n";
		std::cout << sdliv.str();

		//SDL_ttf Version
		std::stringstream sdltv;
		sdltv << "SDL_ttf Version: " << SDL_TTF_MAJOR_VERSION << "." << SDL_TTF_MINOR_VERSION << "." << SDL_TTF_PATCHLEVEL << "\n";
		std::cout << sdltv.str();

		//SDL_mixer Version
		std::stringstream sdlmv;
		sdlmv << "SDL_mixer Version: " << SDL_MIXER_MAJOR_VERSION << "." << SDL_MIXER_MINOR_VERSION << "." << SDL_MIXER_PATCHLEVEL << "\n";
		std::cout << sdlmv.str();

		//Map Size
		std::cout << "Map size: (" << TOTAL_TILES_X << ", " << TOTAL_TILES_Y << ") ... Room size: (" << DUNGEON_SIZE << ", " << DUNGEON_SIZE <<
			") ... Total tiles: " << TOTAL_TILES_X * TOTAL_TILES_Y << "\n";
	}

	if (player.killSwitch)
	{
		return 1;
	}

	generateTown();

	int maxCamY, maxCamH, maxCamX, maxCamW, playX, playY;
	int maxShadeY, maxShadeH, maxShadeX, maxShadeW;

	FPS.start();
	double avgFPS = 0, minFPS = 999;
	int countedFrames = 0;

	for (int y = 0; y < TOTAL_TILES_Y * 4; y++)
		for (int x = 0; x < TOTAL_TILES_X * 4; x++)
			shades[x][y] = Shade(x * TILE_HALF, y * TILE_HALF, blackmap, camera);

	bool displayItems = false;

	Mix_Music *titlemusic;
	titlemusic = Mix_LoadMUS("bin/music/titlemusic.ogg");
	saveVolume();
	Mix_PlayMusic(titlemusic, -1);

	while (!quit)
	{
		//Start capFPS
		capFPS.start();

		//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		//Calculate and fix FPS issues
		avgFPS = countedFrames / (FPS.getTicks() / 1000.f);
		if (avgFPS > 2000000 || std::isnan(avgFPS))
			avgFPS = 0;
		if (countedFrames > 300)
			minFPS = (minFPS < avgFPS) ? minFPS : avgFPS;

		if (mapScreen == SCREEN_INGAME)
		{

			//Handle Events
			while (SDL_PollEvent(&e) != 0)
			{
				//Handle events for player
				player.handleEvents(&e, items, texts);

				//Crafting Stations Events
				if (weaponStation.visible)
					weaponStation.handleEvents(&e, texts);

				//Events for message box
				if (messageBox != NULL)
					messageBox->handleEvents(&e);

				//User presses a key down
				if (e.type == SDL_KEYDOWN)
				{
					switch (e.key.keysym.sym)
					{
					case SDLK_v:
						player.setPosition(exitPortal.getBox().x, exitPortal.getBox().y);
						break;
					case SDLK_l:
						mapStyle = STYLE_FOREST;
						dungeonStyle = "";
						generateMap();
						player.resetPlayer();
						std::cout << "New map generated\n";
						break;
					case SDLK_m:
						if (enemies.empty() == false)
							enemies.clear();
						std::cout << "Enemies cleared\n";
						break;
					case SDLK_0:
						player.effects.push_back(new EffectSlow(5 * SCREEN_FPS, 1, 0.50));
						break;
					case SDLK_p:
						countedFrames = 0;
						avgFPS = 0;
						minFPS = 999;
						FPS.reset();
						std::cout << "Reset FPS\n";
						break;
					case SDLK_j:
						player.saveGame(1);
						break;
					case SDLK_k:
						player.loadGame(1, globalItems);
						break;
					case SDLK_t:
						player.exp = player.maxExp;
						player.checkLevel();
						break;
					case SDLK_BACKQUOTE:
						displayItems = !displayItems;
						break;
					case SDLK_o:
						slime.setPosition(player.getPosition().first - 100, player.getPosition().second - 100);
						slime.uniqueID = globalCount++; //VERY important to do, don't forget!
						slime.setCamera();
						enemies.push_back(slime);
						std::cout << "Spawned enemy\n";
						break;
					default:
						break;
					}
				}

				//User releases a key
				/*
				if (e.type == SDL_KEYUP)
				{
					switch (e.key.keysym.sym)
					{
					default: break;
					}
				}
				*/

				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
			}

			//Draw below here

			//Sprite Header Handler

			maxCamY = (int)ceil(camera.y / TILE_SIZE);
			maxCamH = (int)(((ceil(camera.y / TILE_SIZE) + ceil(camera.h / TILE_SIZE)) < TOTAL_TILES_Y - 1) ? ceil(camera.y / TILE_SIZE) + ceil(camera.h / TILE_SIZE) + 1 : ceil(camera.y / TILE_SIZE) + ceil(camera.h / TILE_SIZE));
			maxCamX = (int)ceil(camera.x / TILE_SIZE);
			maxCamW = (int)(((ceil(camera.x / TILE_SIZE) + ceil(camera.w / TILE_SIZE)) < TOTAL_TILES_X - 1) ? ceil(camera.x / TILE_SIZE) + ceil(camera.w / TILE_SIZE) + 1 : ceil(camera.x / TILE_SIZE) + ceil(camera.w / TILE_SIZE));

			//Handle Spells
			for (unsigned int i = 0; i < spells.size(); i++)
			{
				spells[i]->activate();
				spells.erase(spells.begin() + i);
			}

			//Draw background sprites
			for (unsigned int i = 0; i < bgsprites.size(); i++)
			{
				bgsprites[i].draw();
			}

			//Draw tiles
			for (int y = maxCamY; y <= maxCamH; y++)
			{
				for (int x = maxCamX; x <= maxCamW; x++)
				{
					tiles[x][y].draw(gRenderer, TimeClock::maxAlpha);
				}
			}

			if (spawnExitPortal)
			{
				exitPortal.nextFrame();
				exitPortal.draw();
				if (checkCollision(player.getBox(), exitPortal.getBox()))
				{
					player.canPick = true;
					if (player.ekey)
					{
						bool debounce = false;
						for (unsigned int i = 0; i < items.size(); i++)
						{
							if (checkCollision(camera, items[i].getBox()) && checkCollision(exitPortal.getBox(), items[i].getBox()))
							{
								debounce = true;
								break;
							}
						}
						if (!debounce)
						{
							player.ekey = false;
							spawnExitPortal = false;
							player.resetPlayer();
							mapStyle = STYLE_TOWN;
							dungeonStyle = "";
							generateTown();
						}
					}
				}
			}

			if (mapStyle == STYLE_TOWN) //If player is at town
				handleTown();

			if (dungeonStyle != "") //If player is in a dungeon
			{
				handlePressurePlate();
			}

			//Draw sprites
			for (unsigned int i = 0; i < sprites.size(); i++)
			{
				sprites[i].nextFrame();
				if (checkCollision(sprites[i].getBox(), camera) == true)
					sprites[i].draw();
				else if (sprites[i].camera == NULL)
					sprites[i].draw();
			}

			//Draw map doors
			for (unsigned int i = 0; i < mapDoors.size(); i++)
			{
				mapDoors[i].run();
				if (mapDoors[i].interacted) {
					if (player.findAndDeleteItem(mapDoorKeys[mapDoors[i].keySlot])) {
						for (unsigned int v = 0; v < sprites.size(); v++) {
							if (sprites[v].getPosition() == mapDoors[i].sprite.getPosition()) {
								sprites.erase(sprites.begin() + v);
								v--;
								break;
							}
						}
						mapDoors.erase(mapDoors.begin() + i);
						i--;
					}
					else {
						texts.push_back(FloatingText("Missing " + mapDoorKeys[mapDoors[i].keySlot] + "!", player.getBox().x + player.getBox().w / 2, player.getBox().y + player.getBox().h / 4, player.getBox().w, CodeNewRoman16, SDL_Color{ 255,0,0 }));
					}
				}
			}

			//Draw items
			for (unsigned int i = 0; i < items.size(); i++)
			{
				if (checkCollision(items[i].getBox(), camera) == true)
					items[i].draw();
				if (items[i].alive == false)
				{
					items.erase(std::remove_if(items.begin(), items.end(), itemToDie), items.end());
					i--;
				}
			}

			//Draw enemies
			for (unsigned int i = 0; i < enemies.size(); i++)
			{
				if (enemies[i].alive)
				{
					enemies[i].effectManager(texts, enemies[i].codeNewRoman18);
				}
				if (enemies[i].aiType != ENEMY_TYPE_NODE && (checkCollision(enemies[i].aiCamera, player.getBox()) == true || enemies[i].playerHitMe))
				{
					enemies[i].handleMove(tiles, sprites, enemies, player, bullets);
				}
				if (round(enemies[i].health) <= 0)
				{
					if (enemies[i].isBoss)
					{
						spawnExitPortal = true;
						lights.push_back(Light(exitPortal.getPosition().first + TILE_SIZE, exitPortal.getPosition().second + TILE_SIZE, 1.5, 1.5));
					}
					enemies[i].handleDeath(items, player);
				}
				if (checkCollision(camera, enemies[i].getBox()))
				{
					enemies[i].nextFrame();
					enemies[i].draw(texts);
				}
				if (enemies[i].alive == false)
				{
					cout << &enemies[i] << "\n";
					handleDoorLink(enemies[i]);
					texts.push_back(FloatingText("+" + std::to_string((int)round(enemies[i].expReward)), enemies[i].getBox().x + enemies[i].getBox().w / 2, enemies[i].getBox().y + enemies[i].getBox().h / 4, enemies[i].getBox().w, CodeNewRoman, SDL_Color{ 255,0,255 }));
					enemies.erase(std::remove_if(enemies.begin(), enemies.end(), enemyToDie), enemies.end());
					i--;
				}
			}
			for (unsigned int i = 0; i < enemies.size(); i++) //Health bars overtop all enemiess
				if (checkCollision(camera, enemies[i].getBox()) && enemies[i].alive)
					enemies[i].drawHealth();

			//Draw bullets
			for (unsigned int i = 0; i < bullets.size(); i++)
			{
				if (bullets[i].alive)
				{
					bullets[i].handleMove(tiles);
				}
				if (bullets[i].alive || bullets[i].hitWall)
				{
					handleHit(bullets[i], sprites, enemies, player);
				}
				bullets[i].nextFrame();
				if (checkCollision(camera, bullets[i].getBox()))
				{
					bullets[i].draw();
				}
				if (bullets[i].alive == false && bullets[i].getSprite().finishedAnimation())
				{
					bullets.erase(std::remove_if(bullets.begin(), bullets.end(), bulletToDie), bullets.end());
					i--;
				}
			}

			//Pass time
			TimeClock::tick();
			double ppx = (player.xPos + PLAYER_SIZE / 2);
			double ppy = (player.yPos + PLAYER_SIZE / 2);

			//Move light to player
			lights[0].x = ppx;
			lights[0].y = ppy;
			lights[0].buildBounds();

			maxShadeY = (int)ceil(camera.y / TILE_HALF);
			maxShadeH = (int)(((ceil(camera.y / TILE_HALF) + ceil(camera.h / TILE_HALF)) < (TOTAL_TILES_Y * 4) - 1) ? ceil(camera.y / TILE_HALF) + ceil(camera.h / TILE_HALF) + 1 : ceil(camera.y / TILE_HALF) + ceil(camera.h / TILE_HALF));
			maxShadeX = (int)ceil(camera.x / TILE_HALF);
			maxShadeW = (int)(((ceil(camera.x / TILE_HALF) + ceil(camera.w / TILE_HALF)) < (TOTAL_TILES_X * 4) - 1) ? ceil(camera.x / TILE_HALF) + ceil(camera.w / TILE_HALF) + 1 : ceil(camera.x / TILE_HALF) + ceil(camera.w / TILE_HALF));

			//Draw lighting
			for (int y = maxShadeY; y <= maxShadeH; y++)
			{
				for (int x = maxShadeX; x <= maxShadeW; x++)
				{
					shades[x][y].calculateLight(lights, TimeClock::maxAlpha);
					shades[x][y].renderLight(gRenderer);
				}
			}

			mapOutline.draw();
			//Move player on minimap
			playX = (int)floor((player.getPosition().first / TILE_SIZE) / DUNGEON_SIZE) * MAP_TILE_SIZE;
			playY = (int)floor((player.getPosition().second / TILE_SIZE) / DUNGEON_SIZE) * MAP_TILE_SIZE + SCREEN_HEIGHT - (MAP_TILE_SIZE * MAP_SIZE);
			mapPlayer.setPosition(playX + 4, playY - 4);
			playX = (int)floor((player.getPosition().first / TILE_SIZE) / DUNGEON_SIZE);
			playY = (int)floor((player.getPosition().second / TILE_SIZE) / DUNGEON_SIZE);
			if (map[playX][playY].discovered == false)
			{
				map[playX][playY].discovered = true;
				if (playX > 0 && playY > 0 && playX < MAP_SIZE && playY < MAP_SIZE)
				{
					map[playX - 1][playY].inrange = true;
					map[playX + 1][playY].inrange = true;
					map[playX][playY - 1].inrange = true;
					map[playX][playY + 1].inrange = true;
					map[playX - 1][playY - 1].inrange = true;
					map[playX - 1][playY + 1].inrange = true;
					map[playX + 1][playY - 1].inrange = true;
					map[playX + 1][playY + 1].inrange = true;
				}
			}

			//Draw minimap
			for (int y = 0; y < MAP_SIZE; y++)
			{
				for (int x = 0; x < MAP_SIZE; x++)
				{
					map[x][y].draw(gRenderer, TimeClock::maxAlpha);
				}
			}
			mapPlayer.nextFrame();
			mapPlayer.draw();

			//Display time
			gText.loadFromRenderedText(gRenderer, TimeClock::display(), WhiteText, CodeNewRoman18, 1000);
			gText.render(gRenderer, 49, 580);

			//Crafting Stations
			if (weaponStation.visible)
				weaponStation.draw();

			//Handles for the Player
			if (messageBox == NULL || !player.msgbox)
				player.handleMove(tiles, sprites, items, enemies, bullets, pressurePlates); //Handles movement and shooting
			player.draw(camera, texts);
			if (messageBox != NULL)
			{
				if (!player.msgbox)
					player.msgbox = true;
				if (player.displayInv)
					player.closeInventory();
				if (player.displaySpells)
					player.closeSpells();
				if (player.displayEscape)
					player.closeEscapeMenu();
				messageBox->draw();
			}

			//Make-shift death
			if (player.health <= 0)
			{
				mapStyle = STYLE_TOWN;
				dungeonStyle = "";
				generateTown();
				player.resetPlayer();
			}
			//Make-shift exit game
			if (player.killSwitch)
			{
				return 1;
			}
			//Make-shift cursor changer
			if ((player.displayInv || player.displayEscape || player.displaySpells || messageBox != NULL) && currCursor != "Menu")
			{
				cursor = SDL_CreateColorCursor(cursorIdle, 0, 0);
				SDL_SetCursor(cursor);
				currCursor = "Menu";
			}
			else if ((!player.displayInv && !player.displayEscape && !player.displaySpells && messageBox == NULL) && currCursor != "Game")
			{
				cursor = SDL_CreateColorCursor(cursorShoot, 7, 7);
				SDL_SetCursor(cursor);
				currCursor = "Game";
			}

			std::stringstream text;
			text << "FPS: " << std::setprecision(2) << std::fixed << avgFPS;// << " / " << minFPS;
			gText.loadFromRenderedText(gRenderer, text.str(), WhiteText, CodeNewRoman, 1000);
			gText.render(gRenderer, 0, 0, NULL, 0.0, NULL, SDL_FLIP_NONE);
			if (displayItems)
			{
				for (unsigned int i = 0; i < globalItems.size(); i++)
				{
					globalItems[i].setPosition(i * 50, 0);
					globalItems[i].render(gRenderer, i * 50, 0, &globalItems[i].getClip());
					if (checkCollision(globalItems[i].getBox(), player.mouseClip) && player.mouseLeft)
					{
						globalItems[i].setPosition(player.getBox().x + 50, player.getBox().y);
						items.push_back(globalItems[i]);
						std::cout << "Spawned " << globalItems[i].name << "\n";
						player.mouseLeft = false;
					}
				}
			}

		}
		if (mapScreen == SCREEN_MENUS)
		{
			buildSaves();
			//Handle Events
			while (SDL_PollEvent(&e) != 0)
			{
				for (unsigned int i = 0; i < menuButtons.size(); i++) {
					menuButtons[i]->handleEvents(&e);
				}
				for (unsigned int i = 0; i < optionButtons.size(); i++) {
					optionButtons[i]->handleEvents(&e);
				}

				if (exitGame.released) {
					exitGame.released = false;
					return 1;
				}
				else if (playGame.released) {
					playGame.released = false;
					mapScreen = SCREEN_INGAME;
					Mix_HaltMusic();
				}
				else if (optionsButton.released) {
					optionsButton.released = false;
					optionsButton.setAnimation("Idle");
					subScreen = SUBSCREEN_OPTIONS;
					for (unsigned int i = 0; i < menuButtons.size(); i++) {
						menuButtons[i]->visible = false;
					}
				}
				else if (backMain.released) {
					backMain.released = false;
					backMain.setAnimation("Idle");
					subScreen = SUBSCREEN_MAIN;
					for (unsigned int i = 0; i < optionButtons.size(); i++) {
						optionButtons[i]->visible = false;
					}
				}
				else if (omup.released) {
					omup.released = false;
					musicVolume += (musicVolume + 5 <= 100 ? 5 : 0);
					saveVolume();
				}
				else if (omdown.released) {
					omdown.released = false;
					musicVolume -= (musicVolume - 5 >= 0 ? 5 : 0);
					saveVolume();
				}
				else if (osup.released) {
					osup.released = false;
					soundVolume += (soundVolume + 5 <= 100 ? 5 : 0);
					saveVolume();
				}
				else if (osdown.released) {
					osdown.released = false;
					soundVolume -= (soundVolume - 5 >= 0 ? 5 : 0);
					saveVolume();
				}

				//User presses a key down
				if (e.type == SDL_KEYDOWN)
				{
					switch (e.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						if (subScreen != SUBSCREEN_MAIN) {
							subScreen = SUBSCREEN_MAIN;
							for (unsigned int i = 0; i < optionButtons.size(); i++) {
								optionButtons[i]->visible = false;
							}
						}
						break;
					default:
						break;
					}
				}

				//User releases a key
				/*
				if (e.type == SDL_KEYUP)
				{
				switch (e.key.keysym.sym)
				{
				default: break;
				}
				}
				*/

				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
			}

			for (unsigned int i = 0; i < screenBG.size(); i++)
			{
				screenBG[i].draw();
				screenBG[i].setPosition(screenBG[i].getPosition().first - 1, 0);
				if (screenBG[i].getPosition().first < -SCREEN_WIDTH) {
					screenBG[i].setPosition((SCREEN_WIDTH * (screenBG.size() - 1)) - 1, 0);
				}
			}

			gamelogo.render(gRenderer, 0, 0);
			menuimage.render(gRenderer, 490, 225);

			if (subScreen == SUBSCREEN_MAIN)
			{
				for (unsigned int i = 0; i < menuButtons.size(); i++) {
					menuButtons[i]->draw();
				}
			}
			if (subScreen == SUBSCREEN_OPTIONS)
			{
				for (unsigned int i = 0; i < optionButtons.size(); i++) {
					optionButtons[i]->draw();
				}
				gText.loadFromRenderedText(gRenderer, "Volume Control", WhiteText, CodeNewRoman, 230);
				gText.render(gRenderer, 525 + 115 - gText.getWidth() / 2.0, 260);
				gText.loadFromRenderedText(gRenderer, "Music: "+to_string(musicVolume)+"%", WhiteText, CodeNewRoman, 230);
				gText.render(gRenderer, 525 + 115 - gText.getWidth() / 2.0, 300);
				gText.loadFromRenderedText(gRenderer, "Sound: " + to_string(soundVolume) + "%", WhiteText, CodeNewRoman, 230);
				gText.render(gRenderer, 525 + 115 - gText.getWidth() / 2.0, 340);
			}

		}

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

	close();

	return 0;
}
