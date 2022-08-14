#include "Crafting.h"

//***********************************************************************************************************************
//																									*	Crafting Item	*
//***********************************************************************************************************************

bool CraftingItem::hasIngredients(Player &p)
{
	bool checked[MAX_INVENTORY] = { 0 };
	int tries = ingredients.size();
	for (unsigned int i = 0; i < ingredients.size(); i++)
	{
		for (unsigned int v = 0; v < MAX_INVENTORY; v++)
		{
			if (checked[v] == false && p.inventory[v].name == ingredients[i].name)
			{
				checked[v] = true;
				tries -= 1;
				break;
			}
		}
	}
	if (tries == 0)
		return true;
	else
		return false;
}

//***********************************************************************************************************************
//																								*	Crafting Station	*
//***********************************************************************************************************************

void CraftingStation::setRenders(SDL_Renderer *gR, SDL_Window *gW)
{
	gRenderer = gR;
	gWindow = gW;
}

void CraftingStation::buildLocalImages(Player &p, int flag)
{
	player = &p;

	//Set station type and local stats
	if (flag == STATION_WEAPON)
	{
		level = &p.saveFile.wsLevel;
		exp = &p.saveFile.wsExp;
		expMax = &p.saveFile.wsMaxExp;
		name = "Plasma Foundry";
	}

	//Clear item and make it into an "inventory" item
	for (unsigned int i = 0; i < items.size(); i++)
	{
		items[i].item.delCamera();
		items[i].item.setScale((double)ITEM_INV_SIZE / (double)items[i].item.getClip().w, (double)ITEM_INV_SIZE / (double)items[i].item.getClip().h);
		items[i].item.setBox(ITEM_INV_SIZE, ITEM_INV_SIZE);
		items[i].item.alive = false;
		for (unsigned int v = 0; v < items[i].ingredients.size(); v++)
		{
			items[i].ingredients[v].delCamera();
			items[i].ingredients[v].setScale((double)ITEM_INV_SIZE / (double)items[i].ingredients[v].getClip().w, (double)ITEM_INV_SIZE / (double)items[i].ingredients[v].getClip().h);
			items[i].ingredients[v].setBox(ITEM_INV_SIZE, ITEM_INV_SIZE);
			items[i].ingredients[v].alive = false;
		}
	}

	font = TTF_OpenFont("bin/fonts/codenewroman.ttf", 20);
	fontSmall = TTF_OpenFont("bin/fonts/codenewroman.ttf", 16);

	nameTexture.loadFromRenderedText(gRenderer, name, SDL_Color{ 255,255,255 }, font, 325);
	levelTexture.loadFromRenderedText(gRenderer, "Level: " + std::to_string(*level), SDL_Color{ 255,255,255 }, fontSmall, 325);
	expTexture.loadFromRenderedText(gRenderer, "EXP: " + std::to_string(*exp) + "/" + std::to_string(*expMax), SDL_Color{ 255,255,255 }, fontSmall, 325);

	text1.loadFromRenderedText(gRenderer, "Plasma Foundry Level: ", SDL_Color{ 255,255,255 }, fontSmall, 325);
	text2.loadFromRenderedText(gRenderer, "Ingredients", SDL_Color{ 255,255,255 }, fontSmall, 325);

	inventoryBox.loadFromFile("bin/images/iconbox.png", gRenderer);
	inventoryBox.setScale((double)ITEM_BOX_SIZE / 120.0, (double)ITEM_BOX_SIZE / 120.0);

	inventoryBoxHovered.loadFromFile("bin/images/iconboxhover.png", gRenderer);
	inventoryBoxHovered.setScale((double)ITEM_BOX_SIZE / 120.0, (double)ITEM_BOX_SIZE / 120.0);

	inventoryBoxClicked.loadFromFile("bin/images/iconboxclick.png", gRenderer);
	inventoryBoxClicked.setScale((double)ITEM_BOX_SIZE / 120.0, (double)ITEM_BOX_SIZE / 120.0);

	background.loadFromFile("bin/images/inventory.png", gRenderer);
	background.setScale(325.0 / 570.0, 495.0 / 795.0);
	backgroundClip = { 935, 20, 325, 495 };

	tooltip.setRender(gRenderer, gWindow);
	tooltip.loadSpriteImage("bin/images/tooltip.png")
		.setFrameSize(192, 92)
		.setPosition(0, 0)
		.setSize(200, 100)
		.setCollide(false)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");

	plasmaIcon.loadFromFile("bin/images/plasmaicon.png", gRenderer);

	boxBar.loadFromFile("bin/images/bar.png", gRenderer);
	boxBar.setScale(299.0 / 506.0, 15.0 / 33.0);

	back.setRender(gRenderer, gWindow);
	back.buildButton(BUTTON_WIDE, SDL_Rect{ backgroundClip.x + 25, backgroundClip.y + 435, 100, 30 }, "Back");

	craft.setRender(gRenderer, gWindow);
	craft.buildButton(BUTTON_WIDE, SDL_Rect{ backgroundClip.x + 200, backgroundClip.y + 435, 100, 30 }, "Craft");

	pageLeft.setRender(gRenderer, gWindow);
	pageLeft.buildButton(BUTTON_ARROW_LEFT, SDL_Rect{ backgroundClip.x + 20, backgroundClip.y + 440, 30, 30 });

	pageRight.setRender(gRenderer, gWindow);
	pageRight.buildButton(BUTTON_ARROW_RIGHT, SDL_Rect{ backgroundClip.x + 275, backgroundClip.y + 440, 30, 30 });

	sexpbar.loadFromFile("bin/images/smallexpbar.png", gRenderer);

	text5.loadFromRenderedText(gRenderer, "Have Materials", SDL_Color{ 255,255,255 }, fontSmall, 325);

	text6.loadFromRenderedText(gRenderer, "Have Crafting Level", SDL_Color{ 255,255,255 }, fontSmall, 325);

	matsOn.setRender(gRenderer, gWindow);
	matsOn.buildButton(BUTTON_WIDE, SDL_Rect{ backgroundClip.x + 65, backgroundClip.y + 405, 20, 20 });

	matsOff.setRender(gRenderer, gWindow);
	matsOff.buildButton(BUTTON_WIDE, SDL_Rect{ backgroundClip.x + 65, backgroundClip.y + 405, 20, 20 }, "X");

	levelOn.setRender(gRenderer, gWindow);
	levelOn.buildButton(BUTTON_WIDE, SDL_Rect{ backgroundClip.x + 65, backgroundClip.y + 428, 20, 20 });

	levelOff.setRender(gRenderer, gWindow);
	levelOff.buildButton(BUTTON_WIDE, SDL_Rect{ backgroundClip.x + 65, backgroundClip.y + 428, 20, 20 }, "X");

	//Fail-safe, literally just kills program on runtime
	if (level == NULL || exp == NULL || expMax == NULL || player == NULL)
		p.killSwitch = true;
}

void CraftingStation::buildDisplay()
{
	//Build display vector
	if (display.empty() == false)
		display.clear();

	if (hasItems == true)
	{
		for (unsigned int i = 0; i < items.size(); i++)
		{
			if (items[i].hasIngredients(*player) && player->plasma >= items[i].plasmaCost && hasLevel && *level >= items[i].levelReq)
			{
				display.push_back(items[i]);
			}
			else if (items[i].hasIngredients(*player) && player->plasma >= items[i].plasmaCost && !hasLevel)
			{
				display.push_back(items[i]);
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < items.size(); i++)
		{
			if (hasLevel && *level >= items[i].levelReq)
			{
				display.push_back(items[i]);
			}
			else if (!hasLevel)
			{
				display.push_back(items[i]);
			}
		}
	}

	//Set level text
	levelTexture.loadFromRenderedText(gRenderer, "Level: " + std::to_string(*level), SDL_Color{ 255,255,255 }, fontSmall, 325);

	//Set exp text
	expTexture.loadFromRenderedText(gRenderer, "EXP: " + std::to_string(*exp) + "/" + std::to_string(*expMax), SDL_Color{ 255,255,255 }, fontSmall, 325);

	//Reset pages and slot
	slotSelected = -1;
	maxPage = ceil((double)display.size() / 25.0);

	if (maxPage == 0)
		maxPage = 1;

	//Set page text
	pageText.loadFromRenderedText(gRenderer, "Page: " + std::to_string(page) + "/" + std::to_string(maxPage), SDL_Color{ 255,255,255 }, font, 325);
}

void CraftingStation::handleEvents(SDL_Event * e, std::vector<FloatingText> &texts)
{
	if (screen == 1)
	{
		back.handleEvents(e);
		craft.handleEvents(e);

		pageLeft.visible = false;
		pageRight.visible = false;
		matsOn.visible = false;
		matsOff.visible = false;
		levelOn.visible = false;
		levelOff.visible = false;

		//Handle Back
		if (back.released)
		{
			screen = 0;
			buildDisplay();
			back.released = false;
			back.setAnimation("Idle");
		}

		//Handle Craft
		if (craft.released)
		{
			if (chosen.hasIngredients(*player) && player->plasma >= chosen.plasmaCost && *level >= chosen.levelReq)
			{
				if (player->inventoryFreeSlot() == true || chosen.ingredients.size() > 0)
				{
					player->plasma -= chosen.plasmaCost;
					texts.push_back(FloatingText("-" + std::to_string(chosen.plasmaCost), player->getBox().x + PLAYER_SIZE / 2, player->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, fontSmall, SDL_Color{ 0,0,255 }));
					*exp += chosen.expReward;
					if (*exp >= *expMax)
					{
						*exp -= *expMax;
						*expMax *= 1.66;
						*level += 1;
						texts.push_back(FloatingText("Level Up!", player->getBox().x + PLAYER_SIZE / 2, player->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, font, SDL_Color{ 0,255,0 }));
					}
					for (unsigned int i = 0; i < chosen.ingredients.size(); i++)
					{
						for (int v = 0; v < MAX_INVENTORY; v++)
						{
							if (player->inventory[v].name == chosen.ingredients[i].name)
							{
								player->inventory[v].clearSlot();
								break;
							}
						}
					}
					for (int i = 0; i < MAX_INVENTORY; i++)
					{
						if (player->inventory[i].name == "Empty")
						{
							player->inventory[i] = chosen.item;
							break;
						}
					}
					texts.push_back(FloatingText("+" + std::to_string(chosen.expReward), player->getBox().x + PLAYER_SIZE / 2, player->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, font, SDL_Color{ 255,0,255 }));
					buildDisplay();
				}
				else
				{
					texts.push_back(FloatingText("Not Enough Room!", player->getBox().x + PLAYER_SIZE / 2, player->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, fontSmall, SDL_Color{ 255,0,0 }));
				}
			}
			else if (*level < chosen.levelReq)
			{
				texts.push_back(FloatingText("Plasma Foundry Too Low!", player->getBox().x + PLAYER_SIZE / 2, player->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, fontSmall, SDL_Color{ 255,0,0 }));
			}
			else if (!chosen.hasIngredients(*player))
			{
				texts.push_back(FloatingText("I Can't Make That!", player->getBox().x + PLAYER_SIZE / 2, player->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, fontSmall, SDL_Color{ 255,0,0 }));
			}
			else if (player->plasma < chosen.plasmaCost)
			{
				texts.push_back(FloatingText("Not Enough Plasma!", player->getBox().x + PLAYER_SIZE / 2, player->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, fontSmall, SDL_Color{ 255,0,0 }));
			}
			craft.released = false;
			player->rebuildInventoryText();
		}
	}

	if (screen == 0)
	{
		pageLeft.handleEvents(e);
		pageRight.handleEvents(e);
		matsOn.handleEvents(e);
		matsOff.handleEvents(e);
		levelOn.handleEvents(e);
		levelOff.handleEvents(e);

		back.visible = false;
		craft.visible = false;

		if (pageRight.released)
		{
			if (page < maxPage)
				page++;
			pageRight.released = false;
			buildDisplay();
		}

		if (pageLeft.released)
		{
			if (page > 1)
				page--;
			pageLeft.released = false;
			buildDisplay();
		}

		if (matsOn.released)
		{
			if (!hasItems)
			{
				hasItems = true;
				page = 1;
				buildDisplay();
				matsOff.released = false;
			}
			matsOn.released = false;
		}

		if (matsOff.released)
		{
			if (hasItems)
			{
				hasItems = false;
				page = 1;
				buildDisplay();
				matsOn.released = false;
			}
			matsOff.released = false;
		}

		if (levelOn.released)
		{
			if (!hasLevel)
			{
				hasLevel = true;
				page = 1;
				buildDisplay();
				levelOff.released = false;
			}
			levelOn.released = false;
		}

		if (levelOff.released)
		{
			if (hasLevel)
			{
				hasLevel = false;
				page = 1;
				buildDisplay();
				levelOn.released = false;
			}
			levelOff.released = false;
		}
	}

	if (e->type == SDL_MOUSEMOTION)
	{
		SDL_GetMouseState(&mouseClip.x, &mouseClip.y);
		if (screen == 0)
		{
			for (unsigned int i = 0; i < display.size(); i++)
			{
				if (checkCollision(mouseClip, display[i].item.getBox()) == true)
					display[i].item.hovered = true;
				else
					display[i].item.hovered = false;
			}
			if (slotSelected != -1 && !checkCollision(mouseClip, display[slotSelected].item.getBox()))
			{
				slotSelected = -1;
			}
		}
		else if (screen == 1)
		{
			if (checkCollision(mouseClip, chosen.item.getBox()))
				chosen.item.hovered = true;
			else
				chosen.item.hovered = false;

			for (unsigned int i = 0; i < chosen.ingredients.size(); i++)
			{
				if (checkCollision(mouseClip, chosen.ingredients[i].getBox()))
					chosen.ingredients[i].hovered = true;
				else
					chosen.ingredients[i].hovered = false;
			}
		}
	}

	if (e->type == SDL_MOUSEBUTTONDOWN)
	{
		//Left click
		if (e->button.button == SDL_BUTTON_LEFT)
		{
			if (screen == 0)
			{
				for (unsigned int i = 0; i < display.size(); i++)
				{
					if (slotSelected == -1 && checkCollision(mouseClip, display[i].item.getBox()) == true)
					{
						slotSelected = (int)i;
					}
				}
			}
		}
	}

	if (e->type == SDL_MOUSEBUTTONUP)
	{
		//Left click
		if (e->button.button == SDL_BUTTON_LEFT)
		{
			if (screen == 0)
			{
				if (slotSelected != -1 && checkCollision(mouseClip, display[slotSelected].item.getBox()) == true)
				{
					display[slotSelected].item.hovered = false;
					chosen = display[slotSelected];
					text1.loadFromRenderedText(gRenderer, name + " Level: " + std::to_string(chosen.levelReq), SDL_Color{ 255,255,255 }, fontSmall, 325);
					text3.loadFromRenderedText(gRenderer, "Plasma Cost: " + std::to_string(chosen.plasmaCost), SDL_Color{ 255,255,255 }, fontSmall, 325);
					plasmaSpace = ("Plasma Cost: " + std::to_string(chosen.plasmaCost)).length() * 9 + 2;
					text4.loadFromRenderedText(gRenderer, "Crafting EXP: +" + std::to_string(chosen.expReward), SDL_Color{ 255,255,255 }, fontSmall, 325);
					slotSelected = -1;
					screen = 1;
				}
			}
		}
	}
}

void CraftingStation::draw()
{
	if (screen == 0)
	{
		background.render(gRenderer, backgroundClip.x, backgroundClip.y);
		boxBar.render(gRenderer, backgroundClip.x + 14, backgroundClip.y + 85);
		boxBar.render(gRenderer, backgroundClip.x + 14, backgroundClip.y + 390);

		nameTexture.render(gRenderer, backgroundClip.x + ((double)backgroundClip.w / 2.0) - (double)nameTexture.getWidth() / 2.0, backgroundClip.y + 25);
		levelTexture.render(gRenderer, backgroundClip.x + ((double)backgroundClip.w / 2.0) - (double)levelTexture.getWidth() / 2.0, backgroundClip.y + 45);
		expTexture.render(gRenderer, backgroundClip.x + ((double)backgroundClip.w / 2.0) - (double)expTexture.getWidth() / 2.0, backgroundClip.y + 61);
		pageText.render(gRenderer, backgroundClip.x + ((double)backgroundClip.w / 2.0) - (double)pageText.getWidth() / 2.0, backgroundClip.y + 450);
		sexpbarcord = { 0, 0, 150, 8 };
		sexpbar.render(gRenderer, backgroundClip.x + 87, 96, &sexpbarcord);
		sexpbarcord = { 0, 8, (int)(((double)*exp / (double)*expMax) * 150), 8 };
		sexpbar.render(gRenderer, backgroundClip.x + 87, 96, &sexpbarcord);

		if (page > 1)
			pageLeft.draw();

		if (page < maxPage)
			pageRight.draw();

		text5.render(gRenderer, backgroundClip.x + 0 + ((double)backgroundClip.w / 2.0) - (double)text5.getWidth() / 2.0, backgroundClip.y + 408);
		text6.render(gRenderer, backgroundClip.x + 0 + ((double)backgroundClip.w / 2.0) - (double)text5.getWidth() / 2.0, backgroundClip.y + 430);

		if (!hasItems)
			matsOn.draw();
		else if (hasItems)
			matsOff.draw();

		if (!hasLevel)
			levelOn.draw();
		else if (hasLevel)
			levelOff.draw();

		int ys = 110;
		int xs = 0;
		int displayed = 0;
		for (unsigned int i = (page - 1) * 25; i < display.size(); i++)
		{
			if (slotSelected == i)
				inventoryBoxClicked.render(gRenderer, backgroundClip.x + (xs * 55) + 25, backgroundClip.y + ys);
			else if (display[i].item.hovered == true)
				inventoryBoxHovered.render(gRenderer, backgroundClip.x + (xs * 55) + 25, backgroundClip.y + ys);
			else
				inventoryBox.render(gRenderer, backgroundClip.x + (xs * 55) + 25, backgroundClip.y + ys);
			display[i].item.setPosition(5 + backgroundClip.x + (xs * 55) + 25, 5 + backgroundClip.y + ys);
			display[i].item.draw();
			xs++;
			if ((i + 1) % 5 == 0)
			{
				ys += 55;
				xs = 0;
			}
			displayed++;
			if (displayed >= 25)
				break;
		}
	}
	else if (screen == 1)
	{
		background.render(gRenderer, backgroundClip.x, backgroundClip.y);
		boxBar.render(gRenderer, backgroundClip.x + 14, backgroundClip.y + 85);
		boxBar.render(gRenderer, backgroundClip.x + 14, backgroundClip.y + 210);
		boxBar.render(gRenderer, backgroundClip.x + 14, backgroundClip.y + 390);

		nameTexture.render(gRenderer, backgroundClip.x + ((double)backgroundClip.w / 2.0) - (double)nameTexture.getWidth() / 2.0, backgroundClip.y + 25);
		levelTexture.render(gRenderer, backgroundClip.x + ((double)backgroundClip.w / 2.0) - (double)levelTexture.getWidth() / 2.0, backgroundClip.y + 45);
		expTexture.render(gRenderer, backgroundClip.x + ((double)backgroundClip.w / 2.0) - (double)expTexture.getWidth() / 2.0, backgroundClip.y + 61);
		sexpbarcord = { 0, 0, 150, 8 };
		sexpbar.render(gRenderer, backgroundClip.x + 87, 96, &sexpbarcord);
		sexpbarcord = { 0, 8, (int)(((double)*exp / (double)*expMax) * 150), 8 };
		sexpbar.render(gRenderer, backgroundClip.x + 87, 96, &sexpbarcord);

		chosen.item.tooltipName.render(gRenderer, backgroundClip.x + ((double)backgroundClip.w / 2.0) - (double)chosen.item.tooltipName.getWidth() / 2.0, backgroundClip.y + 110);
		text1.render(gRenderer, backgroundClip.x + ((double)backgroundClip.w / 2.0) - (double)text1.getWidth() / 2.0, backgroundClip.y + 130);
		text2.render(gRenderer, backgroundClip.x + ((double)backgroundClip.w / 2.0) - (double)text2.getWidth() / 2.0, backgroundClip.y + 230);
		text3.render(gRenderer, backgroundClip.x + ((double)backgroundClip.w / 2.0) - (double)text3.getWidth() / 2.0, backgroundClip.y + 250);
		plasmaIcon.render(gRenderer, backgroundClip.x + plasmaSpace + ((double)backgroundClip.w / 2.0) - (double)text3.getWidth() / 2.0, backgroundClip.y + 251);
		text4.render(gRenderer, backgroundClip.x + ((double)backgroundClip.w / 2.0) - (double)text4.getWidth() / 2.0, backgroundClip.y + 412);

		int ys = 275;
		int xs = 0;
		for (unsigned int i = 0; i < chosen.ingredients.size(); i++)
		{
			if (chosen.ingredients[i].hovered == true)
				inventoryBoxHovered.render(gRenderer, backgroundClip.x + (xs * 55) + 25, backgroundClip.y + ys);
			else
				inventoryBox.render(gRenderer, backgroundClip.x + (xs * 55) + 25, backgroundClip.y + ys);
			chosen.ingredients[i].setPosition(5 + backgroundClip.x + (xs * 55) + 25, 5 + backgroundClip.y + ys);
			chosen.ingredients[i].draw();
			xs++;
			if ((i + 1) % 5 == 0)
			{
				ys += 55;
				xs = 0;
			}
		}
		int spx = 5;
		for (unsigned int i = 0; i < chosen.ingredients.size(); i++)
		{
			if (chosen.ingredients[i].hovered == true)
			{
				tooltip.setSize(chosen.ingredients[i].tooltipWidth, chosen.ingredients[i].tooltipSize);
				tooltip.setPosition(-spx + chosen.ingredients[i].getBox().x - tooltip.getSize().first, -spx + chosen.ingredients[i].getBox().y);
				tooltip.draw();
				chosen.ingredients[i].tooltipName.render(gRenderer, tooltip.getBox().x + ((double)tooltip.getBox().w / 2.0) - (double)chosen.ingredients[i].tooltipName.getWidth() / 2.0, tooltip.getBox().y + 10);
				chosen.ingredients[i].tooltipText.render(gRenderer, tooltip.getBox().x + 10, tooltip.getBox().y + 10);
				chosen.ingredients[i].plasmaIcon.render(gRenderer, tooltip.getBox().x + 10 + chosen.ingredients[i].plasmaSpace + 2, tooltip.getBox().y + chosen.ingredients[i].pYSpace);
				if (chosen.ingredients[i].isarmor)
					chosen.ingredients[i].armorIcon.render(gRenderer, tooltip.getBox().x + 10 + chosen.ingredients[i].armorSpace + 2, tooltip.getBox().y + 45);
			}
		}

		if (chosen.item.hovered == true)
		{
			inventoryBoxHovered.render(gRenderer, backgroundClip.x + 138, backgroundClip.y + 150);
			tooltip.setSize(chosen.item.tooltipWidth, chosen.item.tooltipSize);
			tooltip.setPosition(-spx + chosen.item.getBox().x - tooltip.getSize().first, -spx + chosen.item.getBox().y);
			tooltip.draw();
			chosen.item.tooltipName.render(gRenderer, tooltip.getBox().x + ((double)tooltip.getBox().w / 2.0) - (double)chosen.item.tooltipName.getWidth() / 2.0, tooltip.getBox().y + 10);
			chosen.item.tooltipText.render(gRenderer, tooltip.getBox().x + 10, tooltip.getBox().y + 10);
			chosen.item.plasmaIcon.render(gRenderer, tooltip.getBox().x + 10 + chosen.item.plasmaSpace + 2, tooltip.getBox().y + chosen.item.pYSpace);
			if (chosen.item.isarmor)
				chosen.item.armorIcon.render(gRenderer, tooltip.getBox().x + 10 + chosen.item.armorSpace + 2, tooltip.getBox().y + 45);
		}
		else
			inventoryBox.render(gRenderer, backgroundClip.x + 138, backgroundClip.y + 150);
		chosen.item.setPosition(5 + backgroundClip.x + 138, 5 + backgroundClip.y + 150);
		chosen.item.draw();

		back.draw();
		craft.draw();
	}
}
