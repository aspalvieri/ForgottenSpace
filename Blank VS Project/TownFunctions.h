#define POSMOD (DUNGEON_SIZE * TILE_SIZE)

TownObject forestPortal;
TownObject forestboss1Portal;
TownObject plasmaReactor;
TownObject weaponForge;
TownObject chests[6];
MessageBox enterForestBoss1Portal(gRenderer, gWindow);

void buildTownObjects()
{
	forestPortal = TownObject(greenPortal, 5, POSMOD + 25 * TILE_SIZE + 15, POSMOD + 21 * TILE_SIZE + 15);
	forestboss1Portal = TownObject(darkgreenPortal, 5, POSMOD + 25 * TILE_SIZE + 15, POSMOD + 25 * TILE_SIZE + 15);
	plasmaReactor = TownObject(reactor, 25, POSMOD + 5 * TILE_SIZE, POSMOD + 11 * TILE_SIZE);
	weaponForge = TownObject(forge, 25, POSMOD + 10 * TILE_SIZE, POSMOD + 11 * TILE_SIZE);
	chests[0] = TownObject(chest, 25, POSMOD + 11 * TILE_SIZE, POSMOD + 22 * TILE_SIZE);
	chests[1] = TownObject(chest, 25, POSMOD + 14 * TILE_SIZE, POSMOD + 22 * TILE_SIZE);
	chests[2] = TownObject(chest, 25, POSMOD + 17 * TILE_SIZE, POSMOD + 22 * TILE_SIZE);
	chests[3] = TownObject(chest, 25, POSMOD + 11 * TILE_SIZE, POSMOD + 25 * TILE_SIZE);
	chests[4] = TownObject(chest, 25, POSMOD + 14 * TILE_SIZE, POSMOD + 25 * TILE_SIZE);
	chests[5] = TownObject(chest, 25, POSMOD + 17 * TILE_SIZE, POSMOD + 25 * TILE_SIZE);
	buyChest.setRenders(gRenderer, gWindow);
	buyChest.buildBox(MESSAGE_YESNO, "Storage chest\nHolds 20 items\nCost: 1000 Plasma\n\n              Purchase?");
	enterForestPortal.setRenders(gRenderer, gWindow);
	enterForestPortal.buildBigBox(MESSAGE_YESNO, "Forestlands", "\nPuzzles: No. Randomized: Yes. Boss: Mini-boss. Important Drops: None. Resources: Gold Ore, Crytal Ore, Bloomweed, Roseleaf.\nLevel Recommended: 1","\n\n\n\n\n                Enter?");
	enterForestBoss1Portal.setRenders(gRenderer, gWindow);
	enterForestBoss1Portal.buildBigBox(MESSAGE_YESNO, "Deep Forest", "\nPuzzles: Yes. Randomized: No. Boss: Standard. Important Drops: None. Resources: None.\nLevel Recommended: 3", "\n\n\n\n\n                Enter?");

}

int chestnum = -1;

void handleTown()
{
	forestPortal.run();
	if (forestPortal.interacted) { if (messageBox == NULL) { 
		messageBox = &enterForestPortal;
		messageBox->tag = "forestPortal"; } 
	} if (messageBox != NULL && messageBox->tag == "forestPortal") {
		if (messageBox->noClicked) { messageBox->resetAll(); messageBox = NULL; } else if (messageBox->yesClicked) { messageBox->resetAll(); messageBox = NULL;
			forestPortal.interacted = false;
			player.resetPlayer();
			mapStyle = STYLE_FOREST;
			dungeonStyle = "";
			generateMap();
		}
	}

	forestboss1Portal.run();
	if (forestboss1Portal.interacted) { if (messageBox == NULL) { 
		messageBox = &enterForestBoss1Portal;
		messageBox->tag = "forestboss1Portal"; } 
	} if (messageBox != NULL && messageBox->tag == "forestboss1Portal") {
		if (messageBox->noClicked) { messageBox->resetAll(); messageBox = NULL; } else if (messageBox->yesClicked) { messageBox->resetAll(); messageBox = NULL;
			forestboss1Portal.interacted = false;
			player.resetPlayer();
			mapStyle = STYLE_FOREST;
			dungeonStyle = "forestboss1";
			generateDungeon();
		}
	}

	weaponForge.run();
	if (weaponForge.interacted)
	{
		if (!weaponStation.visible)
		{
			weaponStation.page = 1;
			weaponStation.hasItems = false;
			weaponStation.visible = true;
			weaponStation.screen = 0;
			weaponStation.buildDisplay();
			player.displayInv = true;
			player.closeEscapeMenu();
			player.closeSpells();
			player.rebuildInventoryText();
		}
		else if (weaponStation.visible)
		{
			weaponStation.visible = false;
			player.displayInv = false;
		}
	}
	if (weaponStation.visible)
	{
		if (!checkCollision(player.getBox(), weaponForge.sCol))
		{
			weaponStation.visible = false;
			player.closeInventory();
		}
		else if (!player.displayInv)
		{
			weaponStation.visible = false;
		}
	}

	plasmaReactor.run();
	if (plasmaReactor.interacted)
	{
		if (!player.reactor)
		{
			plasmaReactor.interacted = false;
			player.reactor = true;
			player.displayInv = true;
			player.closeEscapeMenu();
			player.closeSpells();
			player.rebuildInventoryText();
		}
		else
		{
			plasmaReactor.interacted = false;
			player.closeInventory();
		}
	}
	if (player.reactor)
	{
		if (!checkCollision(player.getBox(), plasmaReactor.sCol))
		{
			player.closeInventory();
		}
		if (player.pressedReactor)
		{
			player.plasma += player.inventory[player.reactorSlot].value;
			texts.push_back(FloatingText("+"+std::to_string(player.inventory[player.reactorSlot].value), player.getBox().x + PLAYER_SIZE / 2, player.getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman, SDL_Color{ 0,255,255 }));
			player.inventory[player.reactorSlot].clearSlot();
			player.reactorSlot = -1;
			player.pressedReactor = false;
			player.rebuildInventoryText();
		}
	}

	for (int i = 0; i < 6; i++)
	{
		if (player.storage[i].open && chests[i].sprite.getCurrentAnimation() != "Open")
			chests[i].sprite.setAnimation("Open");
		else if (!player.storage[i].open && chests[i].sprite.getCurrentAnimation() != "Closed")
			chests[i].sprite.setAnimation("Closed");
			chests[i].run();
		if (chests[i].interacted)
		{
			chests[i].interacted = false;
			if (player.storage[i].open == true && player.otherInv == NULL)
			{
				player.otherInv = &player.storage[i];
				player.displayInv = true;
				player.rebuildInventoryText();
				chestnum = i;
			}
			else if (player.storage[i].open == false)
			{
				if (messageBox == NULL)
				{
					messageBox = &buyChest;
					messageBox->tag = "buyChest";
					chestnum = i;
				}
			}
			else
			{
				player.closeInventory();
				chestnum = -1;
			}
		}
		if (chestnum == i && !checkCollision(player.getBox(), chests[i].sCol))
		{
			player.closeInventory();
			chestnum = -1;
		}
	}
	//Message box interaction with chest
	if (messageBox != NULL && messageBox->tag == "buyChest")
	{
		if (messageBox->noClicked)
		{
			chestnum = -1;
			messageBox->resetAll();
			messageBox = NULL;
		}
		else if (messageBox->yesClicked)
		{
			if (player.plasma > 1000)
			{
				player.plasma -= 1000;
				player.storage[chestnum].open = true;
				texts.push_back(FloatingText("-1000", player.getBox().x + PLAYER_SIZE / 2, player.getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRomanBig, SDL_Color{ 0,0,255 }));
			}
			else
			{
				texts.push_back(FloatingText("Not Enough Plasma!", player.getBox().x + PLAYER_SIZE / 2, player.getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRomanBig, SDL_Color{ 255,0,0 }));
			}
			chestnum = -1;
			messageBox->resetAll();
			messageBox = NULL;
		}
	}
}

void handlePressurePlate()
{
	if (dungeonStyle == "forestboss1")
	{
		for (unsigned int i = 0; i < pressurePlates.size(); i++)
		{
			if (pressurePlates[i].active)
			{
				pressurePlates[i].active = false;
				bool deb = false;
				if (pressurePlates[i].name == "PressurePlate1")
				{
					for (unsigned int v = 0; v < pressureDoors.size(); v++)
					{
						if (pressureDoors[v].name == "PressureDoor1" || pressureDoors[v].name == "PressureDoor2")
						{
							whiteDoor.setPosition(pressureDoors[v].box.x, pressureDoors[v].box.y);
							sprites.push_back(whiteDoor);
							sprites.back().spriteName = pressureDoors[v].name;
						}
						else if (pressureDoors[v].name == "PressureDoor3")
						{
							enemyTiers[3].setPosition(pressureDoors[v].box.x, pressureDoors[v].box.y);
							enemyTiers[3].uniqueID = globalCount++; //VERY important to do, don't forget!
							enemyTiers[3].setCamera();
							enemies.push_back(enemyTiers[3]);
							enemies.back().doorLink = "PressureDoor2";
						}
					}
					deb = true;
				}
				if (pressurePlates[i].name == "PressurePlate2")
				{
					for (unsigned int v = 0; v < sprites.size(); v++)
					{
						if (sprites[v].spriteName == "PressureDoor1")
						{
							deb = true;
							sprites.erase(sprites.begin() + v);
							v--;
						}
					}
				}

				//Delete pressure plates
				if (deb == true)
				{
					std::string pname = pressurePlates[i].name;
					for (unsigned int v = 0; v < pressurePlates.size(); v++)
					{
						if (pressurePlates[v].name == pname)
						{
							pressurePlates.erase(pressurePlates.begin() + v);
							v--;
						}
					}
					break;
				}
			}
		}
	}


}

void handleDoorLink(Enemy &enemy)
{
	if (enemy.doorLink != "")
	{
		for (unsigned int v = 0; v < sprites.size(); v++)
		{
			if (sprites[v].spriteName == enemy.doorLink)
			{
				sprites.erase(sprites.begin() + v);
				v--;
			}
		}
	}
}
