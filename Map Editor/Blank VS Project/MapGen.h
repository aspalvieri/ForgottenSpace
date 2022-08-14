void clearMap()
{
	//Fill map with empties
	for (int y = 0; y < MAP_SIZE; y++)
		for (int x = 0; x < MAP_SIZE; x++)
			map[x][y] = Tile(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE, tilesheet[TILE_ROCK], false, tilemap);

	//Fill tiles with blanks
	for (int y = 0; y < TOTAL_TILES_Y; y++)
		for (int x = 0; x < TOTAL_TILES_X; x++)
			tiles[x][y] = Tile(x * TILE_SIZE, y * TILE_SIZE, tilesheet[TILE_BLACK], false, tilemap, shadowmap, objectmap, camera);

	//Clear items off map
	if (items.empty() == false)
		items.clear();

	//Clear sprites off map
	if (sprites.empty() == false)
		sprites.clear();

	//Clear enemies off map
	if (enemies.empty() == false)
		enemies.clear();

	//Clear bullets off map
	if (bullets.empty() == false)
		bullets.clear();

	player.targetEnemy = NULL;
}

void loadChunk(int x, int y, std::string path)
{
	std::fstream RAF;
	RAF.open(path.c_str(), std::ios::in | std::ios::binary);
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
		}
	}
	RAF.close();
	std::cout << "Loaded chunk from: " << path << "\n";
}

void generateMap()
{
	clearMap();

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
					map[xs][ys] = Tile(homeX * MAP_TILE_SIZE, homeY * MAP_TILE_SIZE, tilesheet[TILE_GRASS], false, tilemap);
					map[xs][ys].inUse = true;
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
			map[homeX][homeY] = Tile(homeX * MAP_TILE_SIZE, homeY * MAP_TILE_SIZE, tilesheet[TILE_END], false, tilemap);
			map[homeX][homeY].inUse = true;
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
			map[homeX][homeY] = Tile(homeX * MAP_TILE_SIZE, homeY * MAP_TILE_SIZE, tilesheet[TILE_START], false, tilemap);
			map[homeX][homeY].inUse = true;
			home.first = homeX;
			home.second = homeY;
			finalPlace = true;
		}
	} while (finalPlace == false);

	//---------------------------------------------------------------------------CAN BE DELETED vvvvv
	//Fill squares with resources
	int oreChance = 20;
	int crystalChance = 10;
	int orbChance = 1;
	for (int y = 0; y < MAP_SIZE; y++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			if (x != home.first && x != finalX && y != home.second && y != finalY && map[x][y].inUse == true && map[x][y].type == 0)
			{
				int roll = rand() % (100 - 1 + 1) + 1;
				if (roll <= oreChance)
				{
					map[x][y] = Tile(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE, SDL_Rect{ 184, 276, 92, 92 }, false, tilemap);
					map[x][y].inUse = true;
					map[x][y].type = 1;
				}
				roll = rand() % (100 - 1 + 1) + 1;
				if (roll <= crystalChance)
				{
					map[x][y] = Tile(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE, SDL_Rect{ 184, 368, 92, 92 }, false, tilemap);
					map[x][y].inUse = true;
					map[x][y].type = 2;
				}
				roll = rand() % (100 - 1 + 1) + 1;
				if (roll <= orbChance)
				{
					map[x][y] = Tile(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE, SDL_Rect{ 0, 368, 92, 92 }, false, tilemap);
					map[x][y].inUse = true;
					map[x][y].type = 3;
				}
			}
		}
	}
	//---------------------------------------------------------------------------CAN BE DELETED ^^^^^

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

	int rockChance = 5;
	//Build map
	for (int mapy = 0; mapy < MAP_SIZE; mapy++)
	{
		for (int mapx = 0; mapx < MAP_SIZE; mapx++)
		{
			for (int y = DUNGEON_SIZE * mapy; y < DUNGEON_SIZE * (mapy + 1); y++)
			{
				for (int x = DUNGEON_SIZE * mapx; x < DUNGEON_SIZE * (mapx + 1); x++)
				{
					int roll = rand() % (100 - 1 + 1) + 1;

					if (map[mapx][mapy].inUse == true)
					{
						if (roll <= rockChance && (mapx != home.first || mapy != home.second))
						{
							//-------------------------------------------------CAN BE DELETED vvvvv
							if (map[mapx][mapy].type > 0)
							{
								if (map[mapx][mapy].type == 1)
								{
									//tiles[x][y] = Tile(x * TILE_SIZE, y * TILE_SIZE, SDL_Rect{ 184, 276, 92, 92 }, true, tilemap, shadowmap, objectmap, shadowmap, objectmap, camera);
									goldNode.setPosition(x * TILE_SIZE + 5, y * TILE_SIZE + 5);
									goldNode.uniqueID = globalCount++; //VERY important to do, don't forget!
									goldNode.setCamera();
									enemies.push_back(goldNode);
									tiles[x][y] = Tile(x * TILE_SIZE, y * TILE_SIZE, tilesheet[TILE_GRASS], false, tilemap, shadowmap, objectmap, camera);
								}
								else if (map[mapx][mapy].type == 2)
								{
									tiles[x][y] = Tile(x * TILE_SIZE, y * TILE_SIZE, SDL_Rect{ 184, 368, 92, 92 }, true, tilemap, shadowmap, objectmap, camera);
								}
								else if (map[mapx][mapy].type == 3)
								{
									tiles[x][y] = Tile(x * TILE_SIZE, y * TILE_SIZE, SDL_Rect{ 0, 368, 92, 92 }, true, tilemap, shadowmap, objectmap, camera);
								}
							}
							//-------------------------------------------------CAN BE DELETED ^^^^^
							else //Place a rock tile
							{
								tiles[x][y] = Tile(x * TILE_SIZE, y * TILE_SIZE, tilesheet[TILE_GRASS_ROCK], true, tilemap, shadowmap, objectmap, camera);
							}
						}
						else //If roll wasn't a "rock"
						{
							tiles[x][y] = Tile(x * TILE_SIZE, y * TILE_SIZE, tilesheet[TILE_GRASS], false, tilemap, shadowmap, objectmap, camera);

							//Add random enemies to the map
							if (mapx != home.first || mapy != home.second)
							{
								int roll = rand() % (500 - 1 + 1) + 1;
								if (roll <= 1)
								{
									enemy.setPosition(x * TILE_SIZE, y * TILE_SIZE);
									enemy.uniqueID = globalCount++; //VERY important to do, don't forget!
									enemy.setCamera();
									enemies.push_back(enemy);
								}
							}

						}
					}
					else //If section of map isn't in use
					{
						tiles[x][y] = Tile(x * TILE_SIZE, y * TILE_SIZE, tilesheet[TILE_BLACK], false, tilemap, shadowmap, objectmap, camera);
					}
				}
			}
		}
	}

	loadChunk(home.first, home.second, "bin/file.dat");

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
						tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, tilesheet[TILE_ROCK], true, tilemap, shadowmap, objectmap, camera);
					}
				}
				if (x - 1 >= 0 && map[x - 1][y].inUse == true)
				{
					for (int i = 0; i < DUNGEON_SIZE; i++)
					{
						int px = (DUNGEON_SIZE*x);
						int py = ((DUNGEON_SIZE*y) + i);
						tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, tilesheet[TILE_ROCK], true, tilemap, shadowmap, objectmap, camera);
					}
				}
				if (y + 1 < MAP_SIZE && map[x][y + 1].inUse == true)
				{
					for (int i = 0; i < DUNGEON_SIZE; i++)
					{
						int px = ((DUNGEON_SIZE*x) + i);
						int py = (DUNGEON_SIZE*y + DUNGEON_SIZE - 1);
						tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, tilesheet[TILE_ROCK], true, tilemap, shadowmap, objectmap, camera);
					}
				}
				if (y - 1 >= 0 && map[x][y - 1].inUse == true)
				{
					for (int i = 0; i < DUNGEON_SIZE; i++)
					{
						int px = ((DUNGEON_SIZE*x) + i);
						int py = (DUNGEON_SIZE*y);
						tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, tilesheet[TILE_ROCK], true, tilemap, shadowmap, objectmap, camera);
					}
				}
				if (y - 1 >= 0 && x - 1 >= 0 && map[x - 1][y - 1].inUse == true)
				{
					int px = (DUNGEON_SIZE*x);
					int py = (DUNGEON_SIZE*y);
					tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, tilesheet[TILE_ROCK], true, tilemap, shadowmap, objectmap, camera);
				}
				if (y + 1 < MAP_SIZE && x + 1 < MAP_SIZE && map[x + 1][y + 1].inUse == true)
				{
					int px = (DUNGEON_SIZE*x) + DUNGEON_SIZE - 1;
					int py = (DUNGEON_SIZE*y) + DUNGEON_SIZE - 1;
					tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, tilesheet[TILE_ROCK], true, tilemap, shadowmap, objectmap, camera);
				}
				if (y - 1 >= 0 && x + 1 < MAP_SIZE && map[x + 1][y - 1].inUse == true)
				{
					int px = (DUNGEON_SIZE*x) + DUNGEON_SIZE - 1;
					int py = (DUNGEON_SIZE*y);
					tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, tilesheet[TILE_ROCK], true, tilemap, shadowmap, objectmap, camera);
				}
				if (y + 1 < MAP_SIZE && x - 1 >= 0 && map[x - 1][y + 1].inUse == true)
				{
					int px = (DUNGEON_SIZE*x);
					int py = (DUNGEON_SIZE*y) + DUNGEON_SIZE - 1;
					tiles[px][py] = Tile(px * TILE_SIZE, py * TILE_SIZE, tilesheet[TILE_ROCK], true, tilemap, shadowmap, objectmap, camera);
				}
			}
		}
	}

	player.setPosition((home.first * DUNGEON_SIZE) * TILE_SIZE + (DUNGEON_SIZE / 2) * TILE_SIZE, (home.second * DUNGEON_SIZE) * TILE_SIZE + (DUNGEON_SIZE / 2) * TILE_SIZE);
}
