void setStyle()
{
	tilemap = townmap; //Fail-safe
	mapsheetstyle = { 0, 0, 50, 50 }; // ^^ same

	if (mapStyle == STYLE_FOREST)
	{
		tilemap = forestmap;
		mapsheetstyle = { 0, 50, 50, 50 };
	}
	else if (mapStyle == STYLE_TOWN)
	{
		tilemap = townmap;
		mapsheetstyle = { 0, 0, 50, 50 };
	}
}

void setEnemies()
{
	minEnemies = 0;
	maxEnemies = 0;
	minResources = 0;
	maxResources = 0;
	if (mapStyle == STYLE_FOREST)
	{
		minEnemies = 5;
		maxEnemies = 7;
		minResources = 1;
		maxResources = 3;
		//Number needs to be ATLEAST 1
		mapEnemies.push_back(std::pair<Enemy, int>{slime, 35});
		mapEnemies.push_back(std::pair<Enemy, int>{fairy, 65});
		//Number needs to be ATLEAST 1
		mapResources.push_back(std::pair<Enemy, int>{crystalNode, 5});
		mapResources.push_back(std::pair<Enemy, int>{goldNode, 15});
		mapResources.push_back(std::pair<Enemy, int>{bloomweedNode, 40});
		mapResources.push_back(std::pair<Enemy, int>{roseleafNode, 40});
	}
	if (dungeonStyle == "forestboss1")
	{
		enemyTiers.push_back(slime);
		enemyTiers.push_back(fairy);
		enemyTiers.push_back(ballista);
		enemyTiers.push_back(wisp);
	}
	if (dungeonStyle != "")
	{
		mapKeys.push_back(blueKey);
		mapDoorKeys.push_back(blueKey.name);
		mapDoorColors.push_back("Blue");

		mapKeys.push_back(redKey);
		mapDoorKeys.push_back(redKey.name);
		mapDoorColors.push_back("Red");
	}
}

void clearMap()
{
	//Fill map with empties
	for (int y = 0; y < MAP_SIZE; y++)
		for (int x = 0; x < MAP_SIZE; x++)
			map[x][y] = Tile(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE, mapsheetstyle, false, maptiles);

	//Fill tiles with blanks
	for (int y = 0; y < TOTAL_TILES_Y; y++)
		for (int x = 0; x < TOTAL_TILES_X; x++)
			tiles[x][y] = Tile(x * TILE_SIZE, y * TILE_SIZE, SDL_Rect{ 50, 50, 50, 50 }, false, tilemap, shadowmap, objectmap, camera);

	//Clear items off map
	if (items.empty() == false)
		items.clear();

	//Clear sprites off map
	if (sprites.empty() == false)
		sprites.clear();

	//Clear background sprites off map
	if (bgsprites.empty() == false)
		bgsprites.clear();

	//Clear enemies off map
	if (enemies.empty() == false)
		enemies.clear();

	//Clear bullets off map
	if (bullets.empty() == false)
		bullets.clear();

	//Clear enemy vectors
	if (mapEnemies.empty() == false)
		mapEnemies.clear();

	//Clear resource vectors
	if (mapResources.empty() == false)
		mapResources.clear();

	//Clear enemy tiers vector
	if (enemyTiers.empty() == false)
		enemyTiers.clear();

	//Clear map keys vector
	if (mapKeys.empty() == false)
		mapKeys.clear();

	//Clear map doors vector
	if (mapDoors.empty() == false)
		mapDoors.clear();

	//Clear map door keys vector
	if (mapDoorKeys.empty() == false)
		mapDoorKeys.clear();

	//Clear non-player lights
	if (lights.size() > 1)
		lights.erase(lights.begin() + 1, lights.end());

	//Clear pressure plates
	if (pressurePlates.empty() == false)
		pressurePlates.clear();

	//Clear pressure doors
	if (pressureDoors.empty() == false)
		pressureDoors.clear();

	//Jank prevention
	player.otherInv = NULL;
	player.targetEnemy = NULL;
	spawnExitPortal = false;

	//Remove keys from player
	player.deleteAllType(ITEM_KEY);
}

void loadChunk(int x, int y, std::string path)
{
	std::fstream RAF;
	RAF.open(path.c_str(), std::ios::in | std::ios::binary);

	if (!RAF.is_open()) // make sure file opens
	{
		std::cout << "Could not open: '" << path << "' for reading!\n";
		return;
	}

	RAF.seekg(0); // read from start
	for (int v = DUNGEON_SIZE * y; v < DUNGEON_SIZE * (y + 1); v++)
	{
		for (int i = DUNGEON_SIZE * x; i < DUNGEON_SIZE * (x + 1); i++)
		{
			TileClip temp;
			RAF.read(reinterpret_cast<char *>(&temp), sizeof(TileClip));
			tiles[i][v].clip.x = temp.clipx;
			tiles[i][v].clip.y = temp.clipy;
			tiles[i][v].setCollide(temp.col);
			RAF.read(reinterpret_cast<char *>(&temp), sizeof(TileClip));
			tiles[i][v].sclip.x = temp.clipx;
			tiles[i][v].sclip.y = temp.clipy;
			RAF.read(reinterpret_cast<char *>(&temp), sizeof(TileClip));
			tiles[i][v].oclip.x = temp.clipx;
			tiles[i][v].oclip.y = temp.clipy;
			if (tiles[i][v].oclip.x == 100) //If the object is a pressure plate
			{
				int mpx = tiles[i][v].oclip.y / 50;
				pressurePlates.push_back( PressurePlate(i * TILE_SIZE, v * TILE_SIZE, "PressurePlate" + to_string(mpx+1)) );
			}
			else if (tiles[i][v].oclip.x == 150) //If the object is a teir of enemy
			{
				int mpx = tiles[i][v].oclip.y / 50;
				if (mpx == 0) //Boss tile
				{

				}
				else
				{
					mpx -= 1;
					enemyTiers[mpx].setPosition(i * TILE_SIZE, v * TILE_SIZE);
					enemyTiers[mpx].uniqueID = globalCount++; //VERY important to do, don't forget!
					enemyTiers[mpx].setCamera();
					enemies.push_back(enemyTiers[mpx]);
				}
			}
			else if (tiles[i][v].oclip.x == 200) //If the object is a light, create light for that tile
			{
				lights.push_back(Light(i * TILE_SIZE + 25, v * TILE_SIZE + 25, 1.7, 2.35));
			}
			else if (tiles[i][v].oclip.x == 250) //If the object is a key
			{
				int mpx = tiles[i][v].oclip.y / 50;
				mapKeys[mpx].setPosition(i * TILE_SIZE, v * TILE_SIZE);
				items.push_back(mapKeys[mpx]);
			}
			else if (tiles[i][v].oclip.x == 300) //If the object is a key door
			{
				int mpx = tiles[i][v].oclip.y / 50;
				mapDoors.push_back(TownObject(whiteDoor, 25, i * TILE_SIZE, v * TILE_SIZE, false));
				mapDoors.back().keySlot = mpx;
				sprites.push_back(mapDoors.back().sprite);
				sprites.back().setAnimation(mapDoorColors[mpx]);
			}
			else if (tiles[i][v].oclip.x == 350) //If the object is a pressure door
			{
				int mpx = tiles[i][v].oclip.y / 50;
				pressureDoors.push_back(PressureDoor(i * TILE_SIZE, v * TILE_SIZE, "PressureDoor" + to_string(mpx + 1)));
			}
		}
	}
	RAF.close();
}

void setWalls()
{
	//Set walls
	for (int y = 0; y < MAP_SIZE; y++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			if (map[x][y].inUse == false)
			{
				if (x + 1 < MAP_SIZE && map[x + 1][y].inUse == true)
				{
					for (int i = 0; i < DUNGEON_SIZE; i++)
					{
						int px = (DUNGEON_SIZE*x + DUNGEON_SIZE - 1);
						int py = ((DUNGEON_SIZE*y) + i);
						tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, SDL_Rect{ 50, 50, 50, 50 }, true, tilemap, shadowmap, objectmap, camera);
					}
				}
				if (x - 1 >= 0 && map[x - 1][y].inUse == true)
				{
					for (int i = 0; i < DUNGEON_SIZE; i++)
					{
						int px = (DUNGEON_SIZE*x);
						int py = ((DUNGEON_SIZE*y) + i);
						tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, SDL_Rect{ 50, 50, 50, 50 }, true, tilemap, shadowmap, objectmap, camera);
					}
				}
				if (y + 1 < MAP_SIZE && map[x][y + 1].inUse == true)
				{
					for (int i = 0; i < DUNGEON_SIZE; i++)
					{
						int px = ((DUNGEON_SIZE*x) + i);
						int py = (DUNGEON_SIZE*y + DUNGEON_SIZE - 1);
						tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, SDL_Rect{ 50, 50, 50, 50 }, true, tilemap, shadowmap, objectmap, camera);
					}
				}
				if (y - 1 >= 0 && map[x][y - 1].inUse == true)
				{
					for (int i = 0; i < DUNGEON_SIZE; i++)
					{
						int px = ((DUNGEON_SIZE*x) + i);
						int py = (DUNGEON_SIZE*y);
						tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, SDL_Rect{ 50, 50, 50, 50 }, true, tilemap, shadowmap, objectmap, camera);
					}
				}
				if (y - 1 >= 0 && x - 1 >= 0 && map[x - 1][y - 1].inUse == true)
				{
					int px = (DUNGEON_SIZE*x);
					int py = (DUNGEON_SIZE*y);
					tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, SDL_Rect{ 50, 50, 50, 50 }, true, tilemap, shadowmap, objectmap, camera);
				}
				if (y + 1 < MAP_SIZE && x + 1 < MAP_SIZE && map[x + 1][y + 1].inUse == true)
				{
					int px = (DUNGEON_SIZE*x) + DUNGEON_SIZE - 1;
					int py = (DUNGEON_SIZE*y) + DUNGEON_SIZE - 1;
					tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, SDL_Rect{ 50, 50, 50, 50 }, true, tilemap, shadowmap, objectmap, camera);
				}
				if (y - 1 >= 0 && x + 1 < MAP_SIZE && map[x + 1][y - 1].inUse == true)
				{
					int px = (DUNGEON_SIZE*x) + DUNGEON_SIZE - 1;
					int py = (DUNGEON_SIZE*y);
					tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, SDL_Rect{ 50, 50, 50, 50 }, true, tilemap, shadowmap, objectmap, camera);
				}
				if (y + 1 < MAP_SIZE && x - 1 >= 0 && map[x - 1][y + 1].inUse == true)
				{
					int px = (DUNGEON_SIZE*x);
					int py = (DUNGEON_SIZE*y) + DUNGEON_SIZE - 1;
					tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, SDL_Rect{ 50, 50, 50, 50 }, true, tilemap, shadowmap, objectmap, camera);
				}
			}
		}
	}
}

void generateMap()
{
	if (mapStyle == STYLE_TOWN) { //Fail-safe
		mapStyle = STYLE_FOREST;
		dungeonStyle = "";
	}

	setStyle();

	clearMap();

	setEnemies();

	//Define home tile to place dungeon around
	int homeX = rand() % ((MAP_SIZE - 2) - 1 + 1) + 1;
	int homeY = rand() % ((MAP_SIZE - 2) - 1 + 1) + 1;
	std::pair<int, int> home = { homeX, homeY }; //Starting point for player, changes at end

	int totalRooms = rand() % (maxRooms - minRooms + 1) + minRooms;
	roomCount = 0;
	//Build initial map
	bool built = false;
	for (int times = 0; times < totalRooms;)
	{
		for (int y = 1; y < MAP_SIZE - 1; y++)
		{
			for (int x = 1; x < MAP_SIZE - 1; x++)
			{
				int xs = rand() % ((MAP_SIZE - 2) - 1 + 1) + 1;
				int ys = rand() % ((MAP_SIZE - 2) - 1 + 1) + 1;
				if (map[xs][ys].inUse == true)
				{
					homeX = xs;
					homeY = ys;
					x--;
					continue;
				}
				if (withinRange(xs, homeX, 1) && withinRange(ys, homeY, 1) && map[xs][ys].inUse == false && !built && (xs == homeX || ys == homeY))
				{
					homeX = xs;
					homeY = ys;
					map[xs][ys].inUse = true;
					map[xs][ys].type = 0;
					built = true;
					roomCount++;
					times++;
				}
			}
		}
		built = false;
	}

	//Set final place
	int finalX = 0, finalY = 0;
	bool finalPlace = false;
	int maxDist = 0;
	do
	{
		for (int y = 0; y < MAP_SIZE; y++)
		{
			for (int x = 0; x < MAP_SIZE; x++)
			{
				if (map[x][y].inUse == true)
				{
					int dist = abs(home.first - x) + abs(home.second - y);
					if (dist > maxDist)
					{
						homeX = x;
						homeY = y;
						maxDist = dist;
					}
				}
			}
		}
		if (maxDist > 0)
		{
			map[homeX][homeY].inUse = true;
			map[homeX][homeY].bossTile = true;
			finalX = homeX;
			finalY = homeY;
			finalPlace = true;
		}
	} while (finalPlace == false);

	//Set home place to be farthest from final place
	finalPlace = false;
	maxDist = 0;
	do
	{
		for (int y = 0; y < MAP_SIZE; y++)
		{
			for (int x = 0; x < MAP_SIZE; x++)
			{
				if (map[x][y].inUse == true)
				{
					int dist = abs(finalX - x) + abs(finalY - y);
					if (dist > maxDist)
					{
						homeX = x;
						homeY = y;
						maxDist = dist;
					}
				}
			}
		}
		if (maxDist > 0)
		{
			map[homeX][homeY].inUse = true;
			home.first = homeX;
			home.second = homeY;
			finalPlace = true;
		}
	} while (finalPlace == false);

	//Move minimap to bottom left
	for (int y = 0; y < MAP_SIZE; y++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			SDL_Rect temp = map[x][y].getBox();
			temp.x += 4;
			temp.y += SCREEN_HEIGHT - (MAP_TILE_SIZE * MAP_SIZE) - 4;
			map[x][y].setBox(temp);
		}
	}

	int totalEnemies = 0;
	int totalResources = 0;

	//Build map
	for (int mapy = 0; mapy < MAP_SIZE; mapy++)
	{
		for (int mapx = 0; mapx < MAP_SIZE; mapx++)
		{
			if (map[mapx][mapy].inUse == true && (mapx != home.first || mapy != home.second) && (mapx != finalX || mapy != finalY))
			{
				if (map[mapx][mapy].type == 0)
				{
					int roll = rand() % (4 - 1 + 1) + 1;
					if (roll == 1)
						loadChunk(mapx, mapy, "bin/maps/" + mapStyle + "/1.dat");
					else if (roll == 2)
						loadChunk(mapx, mapy, "bin/maps/" + mapStyle + "/2.dat");
					else if (roll == 3)
						loadChunk(mapx, mapy, "bin/maps/" + mapStyle + "/3.dat");
					else if (roll == 4)
						loadChunk(mapx, mapy, "bin/maps/" + mapStyle + "/4.dat");
					int spawnedEnemies = 0;
					int spawnedResources = 0;
					//Dungeon Parameters
					int enemiesToMake = rand() % (maxEnemies - minEnemies + 1) + minEnemies;
					int enemyChance = 5; //Spreads them out around map
					int resourcesToMake = rand() % (maxResources - minResources + 1) + minResources;
					int resourceChance = 5; //Spreads them out around map
					for (int i = 0; i < enemiesToMake;)
					{
						for (int y = DUNGEON_SIZE * mapy; y < DUNGEON_SIZE * (mapy + 1) && i < enemiesToMake; y++)
						{
							for (int x = DUNGEON_SIZE * mapx; x < DUNGEON_SIZE * (mapx + 1) && i < enemiesToMake; x++)
							{
								if (tiles[x][y].getCollide() == false)
								{
									int roll = rand() % (1000 - 1 + 1) + 1;
									bool placed = false;
									if (roll <= enemyChance && !placed && mapEnemies.empty() == false)
									{
										int maxRoll = -1;
										std::vector<int> newTable;
										for (unsigned int i = 0; i < mapEnemies.size(); i++)
										{
											maxRoll += mapEnemies[i].second;
											newTable.push_back(mapEnemies[i].second);
											if ((int)(i - 1) >= 0)
												newTable[i] += newTable[(i - 1)];
										}
										int enemyRoll = rand() % (maxRoll - 0 + 1) + 0;
										for (unsigned int i = 0; i < mapEnemies.size(); i++)
										{
											if (enemyRoll >= (newTable[i] - mapEnemies[i].second) && enemyRoll < newTable[i])
											{
												mapEnemies[i].first.setPosition(x * TILE_SIZE, y * TILE_SIZE);
												mapEnemies[i].first.uniqueID = globalCount++; //VERY important to do, don't forget!
												mapEnemies[i].first.setCamera();
												enemies.push_back(mapEnemies[i].first);
												placed = true;
												spawnedEnemies++;
												break;
											}
										}
									}
									if (placed)
									{
										for (unsigned int v = 0; v < enemies.size(); v++)
										{
											if (checkCollision(enemies.back().aiCamera, enemies[v].getBox()) == true && enemies[v].uniqueID != enemies.back().uniqueID && checkCollision(enemies.back().getBox(), enemies[v].getBox()) == true)
											{
												enemies.pop_back();
												i--;
												break;
											}
										}
										i++;
									}
								}
							}
						}
					}
					for (int i = 0; i < resourcesToMake;)
					{
						for (int y = DUNGEON_SIZE * mapy; y < DUNGEON_SIZE * (mapy + 1) && i < resourcesToMake; y++)
						{
							for (int x = DUNGEON_SIZE * mapx; x < DUNGEON_SIZE * (mapx + 1) && i < resourcesToMake; x++)
							{
								if (tiles[x][y].getCollide() == false)
								{
									int roll = rand() % (1000 - 1 + 1) + 1;
									bool placed = false;
									if (roll <= resourceChance && !placed && mapResources.empty() == false)
									{
										int maxRoll = -1;
										std::vector<int> newTable;
										for (unsigned int i = 0; i < mapResources.size(); i++)
										{
											maxRoll += mapResources[i].second;
											newTable.push_back(mapResources[i].second);
											if ((int)(i - 1) >= 0)
												newTable[i] += newTable[(i - 1)];
										}
										int enemyRoll = rand() % (maxRoll - 0 + 1) + 0;
										for (unsigned int i = 0; i < mapResources.size(); i++)
										{
											if (enemyRoll >= (newTable[i] - mapResources[i].second) && enemyRoll < newTable[i])
											{
												mapResources[i].first.setPosition(x * TILE_SIZE + 5, y * TILE_SIZE + 5);
												mapResources[i].first.uniqueID = globalCount++; //VERY important to do, don't forget!
												mapResources[i].first.setCamera();
												enemies.push_back(mapResources[i].first);
												placed = true;
												spawnedResources++;
												break;
											}
										}
									}
									if (placed)
									{
										for (unsigned int v = 0; v < enemies.size(); v++)
										{
											if (checkCollision(enemies.back().aiCamera, enemies[v].getBox()) == true && enemies[v].uniqueID != enemies.back().uniqueID && checkCollision(enemies.back().getBox(), enemies[v].getBox()) == true)
											{
												enemies.pop_back();
												i--;
												break;
											}
										}
										i++;
									}
								}
							}
						}
					}
					std::cout << "Enemies: " << spawnedEnemies << ", Resources: " << spawnedResources << "\n";
					totalEnemies += spawnedEnemies;
					totalResources += spawnedResources;
				}
			}
		}
	}
	std::cout << "Total Enemies: " << totalEnemies << ", Total Resources: " << totalResources << "\n";

	loadChunk(home.first, home.second, "bin/maps/" + mapStyle + "/home.dat");
	loadChunk(finalX, finalY, "bin/maps/" + mapStyle + "/end.dat");

	setWalls();

	//Janky code for portal
	spawnExitPortal = false;
	portalX = (finalX * DUNGEON_SIZE) * TILE_SIZE + ((DUNGEON_SIZE / 2) - 1) * TILE_SIZE;
	portalY = (finalY * DUNGEON_SIZE) * TILE_SIZE + ((DUNGEON_SIZE / 2) - 1) * TILE_SIZE;
	exitPortal.setPosition(portalX, portalY);

	wisp.setPosition(portalX, portalY);
	wisp.uniqueID = globalCount++; //VERY important to do, don't forget!
	wisp.setCamera();
	enemies.push_back(wisp);
	//--------------------------------

	player.setPosition((home.first * DUNGEON_SIZE) * TILE_SIZE + (DUNGEON_SIZE / 2) * TILE_SIZE, (home.second * DUNGEON_SIZE) * TILE_SIZE + (DUNGEON_SIZE / 2) * TILE_SIZE);
}

void generateTown()
{
	if (mapStyle != STYLE_TOWN) { //Fail-safe
		mapStyle = STYLE_TOWN;
		dungeonStyle = "";
	}

	setStyle();

	clearMap();

	setEnemies();

	//Map is located 1 tile over and under on minimap, so multiply it out when adding sprites
	//Basically just multiply dungeon size by tile size and add that to it's position for both x and y
	
	//Make background
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			spacebg.setPosition((x * DUNGEON_SIZE) * TILE_SIZE, (y * DUNGEON_SIZE) * TILE_SIZE);
			bgsprites.push_back(spacebg);
		}
	}

	loadChunk(1, 1, "bin/maps/" + mapStyle + "/1.dat");
	map[1][1].inUse = true;
	map[1][1].discovered = true;

	//Move minimap to bottom left
	for (int y = 0; y < MAP_SIZE; y++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			SDL_Rect temp = map[x][y].getBox();
			temp.x += 4;
			temp.y += SCREEN_HEIGHT - (MAP_TILE_SIZE * MAP_SIZE) - 4;
			map[x][y].setBox(temp);
		}
	}

	player.setPosition((1 * DUNGEON_SIZE) * TILE_SIZE + (DUNGEON_SIZE / 2) * TILE_SIZE, (1 * DUNGEON_SIZE) * TILE_SIZE + (DUNGEON_SIZE / 2) * TILE_SIZE);
	player.checkStats();
	player.rebuildInventoryText();
}

void generateDungeon()
{
	if (mapStyle == STYLE_TOWN) { //Fail-safe
		mapStyle = STYLE_FOREST;
		dungeonStyle = "forestboss1";
	}

	setStyle();

	clearMap();

	setEnemies();

	//Get map layout
	std::ifstream RAFe;
	std::string fpath = "bin/maps/" + dungeonStyle + "/layout.txt";
	RAFe.open(fpath);

	if (!RAFe.is_open()) // make sure file opens
	{
		std::cout << "Could not open: '" << fpath << "' for reading!\n";
		return;
	}
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			int lx;
			RAFe >> lx;
			if (lx > 0) {
				loadChunk(x, y, "bin/maps/" + dungeonStyle + "/" + to_string(lx) + ".dat");
				map[x][y].inUse = true;
			}
			else {
				map[x][y].inUse = false;
			}
		}
	}

	RAFe.close();

	setWalls();

	//Move minimap to bottom left
	for (int y = 0; y < MAP_SIZE; y++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			SDL_Rect temp = map[x][y].getBox();
			temp.x += 4;
			temp.y += SCREEN_HEIGHT - (MAP_TILE_SIZE * MAP_SIZE) - 4;
			map[x][y].setBox(temp);
		}
	}

	player.setPosition((1 * DUNGEON_SIZE) * TILE_SIZE + (DUNGEON_SIZE / 2) * TILE_SIZE, (1 * DUNGEON_SIZE) * TILE_SIZE + (DUNGEON_SIZE / 2) * TILE_SIZE);
}
