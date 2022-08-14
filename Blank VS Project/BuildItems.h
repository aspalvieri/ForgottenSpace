Item findItem(string name)
{
	//Blank or empty item is called "Empty"
	for (Item i : globalItems)
	{
		if (i.name == name)
		{
			return i;
		}
	}
	//If it was unable to find an item, exit game
	exit(2);
}

void buildItems()
{
	//***********************************************************************************************************************
	//																									*		Items		*
	//***********************************************************************************************************************

	Item itemsArray[] = {
	{gRenderer, gWindow, SDL_Rect{ 40,  0, 40, 40}, ITEM_JUNK, itemmap, camera, 10, "Log"},
	{},
	};

	logs = Item(gRenderer, gWindow, SDL_Rect{ 40, 0, 40, 40 }, ITEM_JUNK, itemmap, camera, /*Value*/10, /*Name*/"Log");
	logs.buildResource("Junk", { COLOR_POOR });
	globalItems.push_back(logs);

	flower = Item(gRenderer, gWindow, SDL_Rect{ 0, 40, 40, 40 }, ITEM_JUNK, itemmap, camera, /*Value*/10, /*Name*/"Flower");
	flower.buildResource("Junk", { COLOR_POOR });
	globalItems.push_back(flower);

	stick = Item(gRenderer, gWindow, SDL_Rect{ 40, 40, 40, 40 }, ITEM_JUNK, itemmap, camera, /*Value*/10, /*Name*/"Stick");
	stick.buildResource("Junk", { COLOR_POOR });
	globalItems.push_back(stick);

	stone = Item(gRenderer, gWindow, SDL_Rect{ 80, 40, 40, 40 }, ITEM_JUNK, itemmap, camera, /*Value*/10, /*Name*/"Stone");
	stone.buildResource("Junk", { COLOR_POOR });
	globalItems.push_back(stone);

	speedRifle = Item(gRenderer, gWindow, SDL_Rect{ 80, 0, 40, 40 }, ITEM_WEAPON, itemmap, camera, /*Value*/60, /*Name*/speedGun.name);
	speedRifle.buildWeapon(/*Weapon*/speedGun, /*Level*/1, { COLOR_UNCOMMON });
	globalItems.push_back(speedRifle);

	hpotion = Item(gRenderer, gWindow, SDL_Rect{ 120, 0, 40, 40 }, ITEM_POTION, itemmap, camera, /*Value*/15, /*Name*/"Small Health Potion");
	hpotion.buildPotion(/*Health*/50, /*Energy*/0, { COLOR_COMMON });
	globalItems.push_back(hpotion);

	epotion = Item(gRenderer, gWindow, SDL_Rect{ 80, 160, 40, 40 }, ITEM_POTION, itemmap, camera, /*Value*/35, /*Name*/"Small Energy Potion");
	epotion.buildPotion(/*Health*/0, /*Energy*/50, { COLOR_COMMON });
	globalItems.push_back(epotion);

	goldOre = Item(gRenderer, gWindow, SDL_Rect{ 120, 40, 40, 40 }, ITEM_RESOURCE, itemmap, camera, /*Value*/25, /*Name*/"Gold Ore");
	goldOre.buildResource("Resource", { COLOR_COMMON });
	globalItems.push_back(goldOre);

	goldBar = Item(gRenderer, gWindow, SDL_Rect{ 120, 120, 40, 40 }, ITEM_RESOURCE, itemmap, camera, /*Value*/60, /*Name*/"Gold Bar");
	goldBar.buildResource("Resource", { COLOR_COMMON });
	globalItems.push_back(goldBar);

	crystalOre = Item(gRenderer, gWindow, SDL_Rect{ 120, 80, 40, 40 }, ITEM_RESOURCE, itemmap, camera, /*Value*/40, /*Name*/"Crystal Ore");
	crystalOre.buildResource("Resource", { COLOR_COMMON });
	globalItems.push_back(crystalOre);

	blueHelm = Item(gRenderer, gWindow, SDL_Rect{ 0, 80, 40, 40 }, ITEM_HEAD, itemmap, camera, /*Value*/25, /*Name*/"Blue Helmet");
	blueHelm.buildArmor(/*Armor*/9, /*Level*/1, "Head", /*Color*/{ 0, 178, 255 }, { COLOR_COMMON });
	globalItems.push_back(blueHelm);

	blueChest = Item(gRenderer, gWindow, SDL_Rect{ 40, 80, 40, 40 }, ITEM_CHEST, itemmap, camera, /*Value*/45, /*Name*/"Blue Chestplate");
	blueChest.buildArmor(/*Armor*/15, /*Level*/1, "Chest", /*Color*/{ 0, 178, 255 }, { COLOR_COMMON });
	globalItems.push_back(blueChest);

	blueLegs = Item(gRenderer, gWindow, SDL_Rect{ 80, 80, 40, 40 }, ITEM_LEGS, itemmap, camera, /*Value*/35, /*Name*/"Blue Platelegs");
	blueLegs.buildArmor(/*Armor*/11, /*Level*/1, "Legs", /*Color*/{ 0, 178, 255 }, { COLOR_COMMON });
	globalItems.push_back(blueLegs);

	tealHelm = Item(gRenderer, gWindow, SDL_Rect{ 0, 120, 40, 40 }, ITEM_HEAD, itemmap, camera, /*Value*/55, /*Name*/"Green Helmet");
	tealHelm.buildArmor(/*Armor*/30, /*Level*/2, "Head", /*Color*/{ 32, 255, 64 }, { COLOR_UNCOMMON });
	globalItems.push_back(tealHelm);

	tealChest = Item(gRenderer, gWindow, SDL_Rect{ 40, 120, 40, 40 }, ITEM_CHEST, itemmap, camera, /*Value*/75, /*Name*/"Green Chestplate");
	tealChest.buildArmor(/*Armor*/70, /*Level*/2, "Chest", /*Color*/{ 32, 255, 64 }, { COLOR_UNCOMMON });
	globalItems.push_back(tealChest);

	tealLegs = Item(gRenderer, gWindow, SDL_Rect{ 80, 120, 40, 40 }, ITEM_LEGS, itemmap, camera, /*Value*/65, /*Name*/"Green Platelegs");
	tealLegs.buildArmor(/*Armor*/40, /*Level*/2, "Legs", /*Color*/{ 32, 255, 64 }, { COLOR_UNCOMMON });
	globalItems.push_back(tealLegs);

	bloomweed = Item(gRenderer, gWindow, SDL_Rect{ 0, 160, 40, 40 }, ITEM_RESOURCE, itemmap, camera, /*Value*/20, /*Name*/"Bloomweed");
	bloomweed.buildResource("Resource", { COLOR_COMMON });
	globalItems.push_back(bloomweed);

	roseleaf = Item(gRenderer, gWindow, SDL_Rect{ 40, 160, 40, 40 }, ITEM_RESOURCE, itemmap, camera, /*Value*/25, /*Name*/"Roseleaf");
	roseleaf.buildResource("Resource", { COLOR_COMMON });
	globalItems.push_back(roseleaf);

	energyCannon = Item(gRenderer, gWindow, SDL_Rect{ 120, 160, 40, 40 }, ITEM_WEAPON, itemmap, camera, /*Value*/160, /*Name*/longGun.name);
	energyCannon.buildWeapon(/*Weapon*/longGun, /*Level*/3, { COLOR_UNCOMMON });
	globalItems.push_back(energyCannon);

	blueKey = Item(gRenderer, gWindow, SDL_Rect{ 160, 0, 40, 40 }, ITEM_KEY, itemmap, camera, /*Value*/0, /*Name*/"Blue Key");
	blueKey.buildResource("Key", { COLOR_RARE });
	globalItems.push_back(blueKey);

	redKey = Item(gRenderer, gWindow, SDL_Rect{ 160, 40, 40, 40 }, ITEM_KEY, itemmap, camera, /*Value*/0, /*Name*/"Red Key");
	redKey.buildResource("Key", { COLOR_RARE });
	globalItems.push_back(redKey);

	Item none = Item();
	globalItems.push_back(none);

	//***********************************************************************************************************************
	//																									*	Crafting Items	*
	//***********************************************************************************************************************

	CraftingItem speedRifleCI;
	speedRifleCI.item = speedRifle;
	speedRifleCI.ingredients.push_back(crystalOre);
	speedRifleCI.ingredients.push_back(goldBar);
	speedRifleCI.ingredients.push_back(goldBar);
	speedRifleCI.levelReq = 1;
	speedRifleCI.plasmaCost = 140;
	speedRifleCI.expReward = 40;

	CraftingItem goldB;
	goldB.item = goldBar;
	goldB.ingredients.push_back(goldOre);
	goldB.ingredients.push_back(goldOre);
	goldB.levelReq = 1;
	goldB.plasmaCost = 20;
	goldB.expReward = 8;

	CraftingItem blueC;
	blueC.item = blueChest;
	blueC.ingredients.push_back(goldBar);
	blueC.ingredients.push_back(goldBar);
	blueC.ingredients.push_back(goldBar);
	blueC.levelReq = 1;
	blueC.plasmaCost = 100;
	blueC.expReward = 40;

	CraftingItem blueL;
	blueL.item = blueLegs;
	blueL.ingredients.push_back(goldBar);
	blueL.ingredients.push_back(goldBar);
	blueL.levelReq = 1;
	blueL.plasmaCost = 60;
	blueL.expReward = 24;

	CraftingItem blueH;
	blueH.item = blueHelm;
	blueH.ingredients.push_back(goldBar);
	blueH.ingredients.push_back(goldBar);
	blueH.levelReq = 1;
	blueH.plasmaCost = 40;
	blueH.expReward = 16;

	CraftingItem shpPot;
	shpPot.item = hpotion;
	shpPot.ingredients.push_back(bloomweed);
	shpPot.ingredients.push_back(bloomweed);
	shpPot.ingredients.push_back(roseleaf);
	shpPot.ingredients.push_back(roseleaf);
	shpPot.levelReq = 1;
	shpPot.plasmaCost = 20;
	shpPot.expReward = 12;

	CraftingItem ehpPot;
	ehpPot.item = epotion;
	ehpPot.ingredients.push_back(bloomweed);
	ehpPot.ingredients.push_back(bloomweed);
	ehpPot.ingredients.push_back(roseleaf);
	ehpPot.ingredients.push_back(roseleaf);
	ehpPot.ingredients.push_back(crystalOre);
	ehpPot.ingredients.push_back(goldBar);
	ehpPot.levelReq = 2;
	ehpPot.plasmaCost = 50;
	ehpPot.expReward = 20;

	CraftingItem energyCannonCI;
	energyCannonCI.item = energyCannon;
	energyCannonCI.ingredients.push_back(speedRifle);
	energyCannonCI.ingredients.push_back(crystalOre);
	energyCannonCI.ingredients.push_back(crystalOre);
	energyCannonCI.levelReq = 2;
	energyCannonCI.plasmaCost = 225;
	energyCannonCI.expReward = 70;

	//***********************************************************************************************************************
	//																								*	Crafting Stations	*
	//***********************************************************************************************************************
	//Push items into station first then run setRenders and buildLocalImages in that order

	weaponStation.items.push_back(goldB);
	weaponStation.items.push_back(speedRifleCI);
	weaponStation.items.push_back(blueH);
	weaponStation.items.push_back(blueC);
	weaponStation.items.push_back(blueL);
	weaponStation.items.push_back(shpPot);
	weaponStation.items.push_back(ehpPot);
	weaponStation.items.push_back(energyCannonCI);
	weaponStation.setRenders(gRenderer, gWindow);
	weaponStation.buildLocalImages(player, STATION_WEAPON);
}