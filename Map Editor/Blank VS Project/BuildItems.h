double calculateDPS(Weapon weapon)
{
	double dps = (weapon.minDamage + ((weapon.maxDamage - weapon.minDamage) / 2)) * (60.0 / (double)weapon.reload);
	return dps;
}

void buildItems()
{
	logs = Item(gRenderer, gWindow, SDL_Rect{ 40, 0, 40, 40 }, ITEM_RESOURCE, itemmap, camera, /*Value*/10, /*Name*/"Log");
	logs.tooltip = "\n Type: Resource\nValue: " + std::to_string(logs.value) + " Plasma";
	logs.rarity = { COLOR_COMMON }; logs.tooltipSize = 3 * 20 + 10; logs.tooltipWidth = 170;

	flower = Item(gRenderer, gWindow, SDL_Rect{ 0, 40, 40, 40 }, ITEM_RESOURCE, itemmap, camera, /*Value*/10, /*Name*/"Flower");
	flower.tooltip = "\n Type: Resource\nValue: " + std::to_string(flower.value) + " Plasma";
	flower.rarity = { COLOR_COMMON }; flower.tooltipSize = 3 * 20 + 10; flower.tooltipWidth = 170;

	stick = Item(gRenderer, gWindow, SDL_Rect{ 40, 40, 40, 40 }, ITEM_RESOURCE, itemmap, camera, /*Value*/10, /*Name*/"Stick");
	stick.tooltip = "\n Type: Resource\nValue: " + std::to_string(stick.value) + " Plasma";
	stick.rarity = { COLOR_COMMON }; stick.tooltipSize = 3 * 20 + 10; stick.tooltipWidth = 170;

	stone = Item(gRenderer, gWindow, SDL_Rect{ 80, 40, 40, 40 }, ITEM_RESOURCE, itemmap, camera, /*Value*/10, /*Name*/"Stone");
	stone.tooltip = "\n Type: Resource\nValue: " + std::to_string(stone.value) + " Plasma";
	stone.rarity = { COLOR_COMMON }; stone.tooltipSize = 3 * 20 + 10; stone.tooltipWidth = 170;

	speedRifle = Item(gRenderer, gWindow, SDL_Rect{ 80, 0, 40, 40 }, ITEM_WEAPON, itemmap, camera, /*Value*/60, /*Name*/speedGun.name);
	speedRifle.weapon = speedGun; speedRifle.levelreq = 1;
	placeText.clear(); placeText.str(std::string());
	placeText << std::fixed << std::setprecision(0) << "\n  Type: Weapon" << std::setprecision(1);
	placeText << "\n   DPS: " << calculateDPS(speedRifle.weapon) << "\nEnergy: " << (speedRifle.weapon.energyCost * (60.0 / (double)speedRifle.weapon.reload));
	placeText << "/s\n Range: " << ((double)speedRifle.weapon.range / TILE_SIZE) << " Tiles" << std::setprecision(0) << "\n Level: " << speedRifle.levelreq;
	placeText << "\n Value: " << speedRifle.value << " Plasma";
	speedRifle.tooltip = placeText.str();
	speedRifle.rarity = { COLOR_UNCOMMON }; speedRifle.tooltipSize = 7 * 20; speedRifle.tooltipWidth = 180;

	hpotion = Item(gRenderer, gWindow, SDL_Rect{ 120, 0, 40, 40 }, ITEM_POTION, itemmap, camera, /*Value*/15, /*Name*/"Small Health Potion");
	hpotion.potion.healthAmount = 100;
	placeText.clear(); placeText.str(std::string());
	placeText << std::fixed << std::setprecision(0) << "\n   Type: Potion\nRestore: " << hpotion.potion.healthAmount << " Health" << "\n    Use: Right-Click";
	placeText << "\n  Value: " <<  hpotion.value << " Plasma";
	hpotion.tooltip = placeText.str();
	hpotion.rarity = { COLOR_COMMON }; hpotion.tooltipSize = 5 * 20 + 4; hpotion.tooltipWidth = 205;

	goldOre = Item(gRenderer, gWindow, SDL_Rect{ 120, 40, 40, 40 }, ITEM_RESOURCE, itemmap, camera, /*Value*/25, /*Name*/"Gold Ore");
	goldOre.tooltip = "\n Type: Resource\nValue: " + std::to_string(goldOre.value) + " Plasma";
	goldOre.rarity = { COLOR_COMMON }; goldOre.tooltipSize = 3 * 20 + 10; goldOre.tooltipWidth = 165;

	blueHelm = Item(gRenderer, gWindow, SDL_Rect{ 0, 80, 40, 40 }, ITEM_HEAD, itemmap, camera, /*Value*/25, /*Name*/"Blue Helmet");
	blueHelm.armor.armorRating = 13; blueHelm.levelreq = 1;
	placeText.clear(); placeText.str(std::string());
	placeText << std::fixed << std::setprecision(0) << "\n Type: Armor - Head\nArmor: " << blueHelm.armor.armorRating;
	placeText << "\nLevel: " << blueHelm.levelreq << "\nValue: " << blueHelm.value << " Plasma";
	blueHelm.tooltip = placeText.str();
	blueHelm.rarity = { COLOR_COMMON }; blueHelm.tooltipSize = 5 * 20 + 4; blueHelm.tooltipWidth = 200;

	blueChest = Item(gRenderer, gWindow, SDL_Rect{ 40, 80, 40, 40 }, ITEM_CHEST, itemmap, camera, /*Value*/45, /*Name*/"Blue Chestplate");
	blueChest.armor.armorRating = 22; blueChest.levelreq = 1;
	placeText.clear(); placeText.str(std::string());
	placeText << std::fixed << std::setprecision(0) << "\n Type: Armor - Chest\nArmor: " << blueChest.armor.armorRating;
	placeText << "\nLevel: " << blueChest.levelreq << "\nValue: " << blueChest.value << " Plasma";
	blueChest.tooltip = placeText.str();
	blueChest.rarity = { COLOR_COMMON }; blueChest.tooltipSize = 5 * 20 + 4; blueChest.tooltipWidth = 205;

	blueLegs = Item(gRenderer, gWindow, SDL_Rect{ 80, 80, 40, 40 }, ITEM_LEGS, itemmap, camera, /*Value*/35, /*Name*/"Blue Platelegs");
	blueLegs.armor.armorRating = 15; blueLegs.levelreq = 1;
	placeText.clear(); placeText.str(std::string());
	placeText << std::fixed << std::setprecision(0) << "\n Type: Armor - Legs\nArmor: " << blueLegs.armor.armorRating;
	placeText << "\nLevel: " << blueLegs.levelreq << "\nValue: " << blueLegs.value << " Plasma";
	blueLegs.tooltip = placeText.str();
	blueLegs.rarity = { COLOR_COMMON }; blueLegs.tooltipSize = 5 * 20 + 4; blueLegs.tooltipWidth = 200;

	tealHelm = Item(gRenderer, gWindow, SDL_Rect{ 0, 120, 40, 40 }, ITEM_HEAD, itemmap, camera, /*Value*/55, /*Name*/"Green Helmet");
	tealHelm.armor.armorRating = 30; tealHelm.levelreq = 2;
	placeText.clear(); placeText.str(std::string());
	placeText << std::fixed << std::setprecision(0) << "\n Type: Armor - Head\nArmor: " << tealHelm.armor.armorRating;
	placeText << "\nLevel: " << tealHelm.levelreq << "\nValue: " << tealHelm.value << " Plasma";
	tealHelm.tooltip = placeText.str();
	tealHelm.rarity = { COLOR_UNCOMMON }; tealHelm.tooltipSize = 5 * 20 + 4; tealHelm.tooltipWidth = 200;

	tealChest = Item(gRenderer, gWindow, SDL_Rect{ 40, 120, 40, 40 }, ITEM_CHEST, itemmap, camera, /*Value*/75, /*Name*/"Green Chestplate");
	tealChest.armor.armorRating = 70; tealChest.levelreq = 2;
	placeText.clear(); placeText.str(std::string());
	placeText << std::fixed << std::setprecision(0) << "\n Type: Armor - Chest\nArmor: " << tealChest.armor.armorRating;
	placeText << "\nLevel: " << tealChest.levelreq << "\nValue: " << tealChest.value << " Plasma";
	tealChest.tooltip = placeText.str();
	tealChest.rarity = { COLOR_UNCOMMON }; tealChest.tooltipSize = 5 * 20 + 4; tealChest.tooltipWidth = 205;

	tealLegs = Item(gRenderer, gWindow, SDL_Rect{ 80, 120, 40, 40 }, ITEM_LEGS, itemmap, camera, /*Value*/65, /*Name*/"Green Platelegs");
	tealLegs.armor.armorRating = 40; tealLegs.levelreq = 2;
	placeText.clear(); placeText.str(std::string());
	placeText << std::fixed << std::setprecision(0) << "\n Type: Armor - Legs\nArmor: " << tealLegs.armor.armorRating;
	placeText << "\nLevel: " << tealLegs.levelreq << "\nValue: " << tealLegs.value << " Plasma";
	tealLegs.tooltip = placeText.str();
	tealLegs.rarity = { COLOR_UNCOMMON }; tealLegs.tooltipSize = 5 * 20 + 4; tealLegs.tooltipWidth = 200;

	/*
	//This will be used for storing items, using them when loading from save file
	globalItems.push_back(logs);
	globalItems.push_back(flower);
	globalItems.push_back(stick);
	globalItems.push_back(stone);
	globalItems.push_back(speedRifle);
	globalItems.push_back(hpotion);
	globalItems.push_back(goldOre);
	globalItems.push_back(blueHelm);
	globalItems.push_back(blueChest);
	globalItems.push_back(blueLegs);
	globalItems.push_back(tealHelm);
	globalItems.push_back(tealChest);
	globalItems.push_back(tealLegs);
	*/
}