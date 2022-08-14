#include "Entity.h"

//***********************************************************************************************************************
//																									*		Entity		*
//***********************************************************************************************************************

Entity::Entity(SDL_Renderer *gR, SDL_Window *gW) : Sprite(gR, gW)
{
	gRenderer = gR;
	gWindow = gW;
	velocity = 0;
	health = 0;
	maxHealth = 0;
	healthRegen = 0;
	energy = 0;
	maxEnergy = 0;
	energyRegen = 0;
}

Entity::~Entity()
{
}

Entity & Entity::setVelocity(double vel)
{
	velocity = vel;
	return *this;
}

double Entity::getVelocity()
{
	return velocity;
}

void Entity::setRenderer(SDL_Renderer *gR, SDL_Window *gW)
{
	gRenderer = gR;
	gWindow = gW;
	Sprite::setRender(gR, gW);
}

void Entity::handleEvents()
{
}

void Entity::handleEvents(SDL_Event* e)
{
}

void Entity::draw()
{
	Sprite::draw();
}


//***********************************************************************************************************************
//																									*		Player		*
//***********************************************************************************************************************

Player::Player(SDL_Renderer *gR, SDL_Window *gW) : Entity(gR, gW)
{
	timeTicks = 0;
	up = false;
	down = false;
	left = false;
	right = false;
	tab = false;
	ekey = false;
	maxCamH = 0;
	maxCamW = 0;
	maxCamX = 0;
	maxCamY = 0;
	displayInv = false;
	itemSelected = false;
	slotSelected = -1;
	mdiff = ITEM_INV_SIZE / 2;
	mouseClip = { 0, 0, 0, 0 };
	statText.str(std::string());
	statText.precision(0);
}

Player::~Player()
{
}

void Player::handleEvents(SDL_Event * e, std::vector<Item> &items)
{
	//Button events
	for (unsigned int i = 0; i < buttons.size(); i++)
	{
		buttons[i]->handleEvents(e);
	}
	
	//Increase Strength
	if (incSTR.released)
	{
		if (skillPoints > 0)
		{
			strength++;
			skillPoints--;
		}
		incSTR.released = false;
	}

	//Increase Dexterity
	if (incDEX.released)
	{
		if (skillPoints > 0)
		{
			dexterity++;
			skillPoints--;
			maxEnergy += 3;
			energy += 3;
		}
		incDEX.released = false;
	}

	//Increase Vitality
	if (incVIT.released)
	{
		if (skillPoints > 0)
		{
			vitality++;
			skillPoints--;
			maxHealth += 6;
			health += 6;
		}
		incVIT.released = false;
	}

	//User pressed a key down
	if (e->type == SDL_KEYDOWN)
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_UP:
		case SDLK_w: 
			up = true;
			break;

		case SDLK_DOWN:
		case SDLK_s:
			down = true; 
			break;

		case SDLK_LEFT:
		case SDLK_a: 
			left = true; 
			break;

		case SDLK_RIGHT:
		case SDLK_d: 
			right = true; 
			break;

		case SDLK_e:
			ekey = true;
			break;

		case SDLK_i:
			if (!tab)
			{
				slotType = SLOT_NONE;
				slotSelected = -1;
				itemSelected = false;
				for (int i = 0; i < MAX_INVENTORY; i++)
					inventory[i].hovered = false;
				for (int i = 0; i < MAX_EQUIPMENT; i++)
					equipment[i].hovered = false;
				for (unsigned i = 0; i < buttons.size(); i++)
				{
					buttons[i]->visible = false;
					buttons[i]->setAnimation("Idle");
				}
				if (displayInv)
					displayInv = false;
				else
					displayInv = true;
			}
			tab = true;
			break;

		default: break;
		}
	}

	//User released a key
	if (e->type == SDL_KEYUP)
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_UP:
		case SDLK_w: 
			up = false; 
			break;

		case SDLK_DOWN:
		case SDLK_s: 
			down = false;
			break;

		case SDLK_LEFT:
		case SDLK_a: 
			left = false; 
			break;

		case SDLK_RIGHT:
		case SDLK_d: 
			right = false; 
			break;

		case SDLK_e:
			ekey = false;
			break;

		case SDLK_i:
			tab = false;
			break;

		default: break;
		}
	}
	
	//Mouse Motion
	if (e->type == SDL_MOUSEMOTION)
	{
		SDL_GetMouseState(&mouseClip.x, &mouseClip.y);
		//Inventory events
		if (displayInv == true)
		{
			if (!hoverRefresh)
			{
				for (int i = 0; i < MAX_INVENTORY; i++)
				{
					if (checkCollision(mouseClip, inventory[i].getBox()) == true)
						inventory[i].hovered = true;
					else
						inventory[i].hovered = false;
				}
				for (int i = 0; i < MAX_EQUIPMENT; i++)
				{
					if (checkCollision(mouseClip, equipment[i].getBox()) == true)
						equipment[i].hovered = true;
					else
						equipment[i].hovered = false;
				}
			}
			if (slotSelected != -1 && slotType != SLOT_NONE && itemSelected == false)
			{
				if (slotType == SLOT_INVENTORY)
				{
					if (checkCollision(mouseClip, inventory[slotSelected].getBox()) == false)
					{
						slotSelected = -1;
						slotType = SLOT_NONE;
					}
				}
				else if (slotType == SLOT_EQUIPMENT)
				{
					if (checkCollision(mouseClip, equipment[slotSelected].getBox()) == false)
					{
						slotSelected = -1;
						slotType = SLOT_NONE;
					}
				}
			}
		}
	}
	if (hoverRefresh)
	{
		hoverRefresh = false;
	}
	if (e->type == SDL_MOUSEBUTTONDOWN)
	{
		//Left click
		if (e->button.button == SDL_BUTTON_LEFT)
		{
			//mouseLeft = true;

			//Inventory events
			if (displayInv == true)
			{
				for (int i = 0; i < MAX_INVENTORY; i++)
				{
					if (itemSelected == false && slotSelected == -1 && checkCollision(mouseClip, inventory[i].getBox()) == true && inventory[i].itemType != ITEM_NONE)
					{
						slotSelected = i;
						selectedTexture = inventory[i].getTexture();
						slotType = SLOT_INVENTORY;
					}
				}
				for (int i = 0; i < MAX_EQUIPMENT; i++)
				{
					if (itemSelected == false && slotSelected == -1 && checkCollision(mouseClip, equipment[i].getBox()) == true && equipment[i].itemType != ITEM_NONE)
					{
						slotSelected = i;
						selectedTexture = equipment[i].getTexture();
						slotType = SLOT_EQUIPMENT;
					}
				}
			}
		}

	}
	if (e->type == SDL_MOUSEBUTTONUP)
	{
		//Left Click
		if (e->button.button == SDL_BUTTON_LEFT)
		{
			mouseLeft = false;

			//Inventory events
			if (displayInv == true)
			{
				//Dropping item off
				if (itemSelected == true && slotSelected > -1 && slotType != SLOT_NONE)
				{
					bool debounce = false;
					//Swap two item's places with inventory/equipment
					for (int i = 0; i < MAX_INVENTORY; i++)
					{
						if (checkCollision(mouseClip, inventory[i].getBox()) == true)
						{
							//Swap from inventory to inventory
							if (slotType == SLOT_INVENTORY)
							{
								inventory[i].hovered = false;
								inventory[slotSelected].hovered = true;
								Item holder = inventory[slotSelected];
								inventory[slotSelected] = inventory[i];
								inventory[i] = holder;
								itemSelected = false;
								slotSelected = -1;
								slotType = SLOT_NONE;
								debounce = true;
								hoverRefresh = true;
								break;
							}
							//Swap from equipment to inventory
							else if (slotType == SLOT_EQUIPMENT && (inventory[i].itemType == equipment[slotSelected].itemType || inventory[i].itemType == ITEM_NONE) && level >= inventory[i].levelreq)
							{
								inventory[i].hovered = false;
								equipment[slotSelected].hovered = true;
								Item holder = equipment[slotSelected];
								equipment[slotSelected] = inventory[i];
								inventory[i] = holder;
								itemSelected = false;
								slotSelected = -1;
								slotType = SLOT_NONE;
								debounce = true;
								hoverRefresh = true;
								break;
							}
						}
					}
					if (!debounce)
					{
						//Swap from inventory to equipment
						for (int i = 0; i < MAX_EQUIPMENT; i++)
						{
							if (slotType == SLOT_INVENTORY && checkCollision(mouseClip, equipment[i].getBox()) == true)
							{
								if (inventory[slotSelected].itemType == i + 1 && level >= inventory[slotSelected].levelreq)
								{
									equipment[i].hovered = false;
									inventory[slotSelected].hovered = true;
									Item holder = inventory[slotSelected];
									inventory[slotSelected] = equipment[i];
									equipment[i] = holder;
									itemSelected = false;
									slotSelected = -1;
									slotType = SLOT_NONE;
									debounce = true;
									hoverRefresh = true;
									break;
								}
							}
						}
					}
					//Drop item on floor
					if (!debounce && !checkCollision(mouseClip, invSheetClip))
					{
						if (slotType == SLOT_INVENTORY)
						{
							inventory[slotSelected].setPosition(Sprite::getBox().x, Sprite::getBox().y);
							inventory[slotSelected].alive = true;
							inventory[slotSelected].camera = camera;
							inventory[slotSelected].setScale((double)ITEM_MAP_SIZE / (double)inventory[slotSelected].getClip().w, (double)ITEM_MAP_SIZE / (double)inventory[slotSelected].getClip().h);
							inventory[slotSelected].setBox(ITEM_MAP_SIZE, ITEM_MAP_SIZE);
							items.push_back(inventory[slotSelected]);
							inventory[slotSelected].clearSlot();
							itemSelected = false;
							slotSelected = -1;
							slotType = SLOT_NONE;
							hoverRefresh = true;
						}
						else if (slotType == SLOT_EQUIPMENT)
						{
							equipment[slotSelected].setPosition(Sprite::getBox().x, Sprite::getBox().y);
							equipment[slotSelected].alive = true;
							equipment[slotSelected].camera = camera;
							equipment[slotSelected].setScale((double)ITEM_MAP_SIZE / (double)equipment[slotSelected].getClip().w, (double)ITEM_MAP_SIZE / (double)equipment[slotSelected].getClip().h);
							equipment[slotSelected].setBox(ITEM_MAP_SIZE, ITEM_MAP_SIZE);
							items.push_back(equipment[slotSelected]);
							equipment[slotSelected].clearSlot();
							itemSelected = false;
							slotSelected = -1;
							slotType = SLOT_NONE;
							hoverRefresh = true;
						}
					}
					//De-select slot
					else if (!debounce)
					{
						if (slotType == SLOT_INVENTORY)
							inventory[slotSelected].hovered = false;
						else if (slotType == SLOT_EQUIPMENT)
							equipment[slotSelected].hovered = false;
						itemSelected = false;
						slotSelected = -1;
						slotType = SLOT_NONE;
						hoverRefresh = true;
					}
				}
				//Picking item up
				else if (itemSelected == false && slotSelected > -1)
				{
					if (slotType == SLOT_INVENTORY && checkCollision(mouseClip, inventory[slotSelected].getBox()))
					{
						itemSelected = true;
					}
					else if (slotType == SLOT_EQUIPMENT && checkCollision(mouseClip, equipment[slotSelected].getBox()))
					{
						itemSelected = true;
					}
					else
					{
						slotSelected = -1;
						slotType = SLOT_NONE;
						hoverRefresh = true;
					}
				}
				setEquipment();
			}
		}

		//Right click
		if (e->button.button == SDL_BUTTON_RIGHT)
		{
			//Inventory events
			if (displayInv == true)
			{
				if (itemSelected == false && slotSelected == -1 && slotType == SLOT_NONE)
				{
					bool debounce = false;
					if (inventoryFreeSlot() == true && debounce == false)
					{
						//Unequip item
						for (int i = 0; i < MAX_EQUIPMENT; i++)
						{
							if (equipment[i].itemType != ITEM_NONE && checkCollision(mouseClip, equipment[i].getBox()) == true)
							{
								inventoryAddItem(i);
								equipment[i].clearSlot();
								equipment[i].hovered = true;
								debounce = true;
								hoverRefresh = true;
								break;
							}
						}
					}
					if (debounce == false)
					{
						//Equip item
						for (int i = 0; i < MAX_INVENTORY; i++)
						{
							if (checkCollision(mouseClip, inventory[i].getBox()) == true)
							{
								for (int v = 0; v < MAX_EQUIPMENT; v++)
								{
									if (!debounce && inventory[i].itemType == v + 1 && level >= inventory[i].levelreq)
									{
										Item holder = equipment[v];
										equipment[v] = inventory[i];
										inventory[i] = holder;
										inventory[i].hovered = true;
										equipment[v].hovered = false;
										debounce = true;
										hoverRefresh = true;
										break;
									}
								}

								if (!debounce && inventory[i].itemType == ITEM_POTION)
								{
									if ((health != maxHealth && inventory[i].potion.healthAmount > 0) || (energy != maxEnergy && inventory[i].potion.energyAmount > 0))
									{
										health += inventory[i].potion.healthAmount;
										if (health > maxHealth)
											health = maxHealth;
										energy += inventory[i].potion.energyAmount;
										if (energy > maxEnergy)
											energy = maxEnergy;
										inventory[i].clearSlot();
										inventory[i].hovered = true;
										hoverRefresh = true;
										debounce = true;
									}
								}
							}
						}
					}
					setEquipment();
				}
			}
		}

	}

}

void Player::handleMove(Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y], std::vector<Sprite> &sprites, std::vector<Item> &items, std::vector<Enemy> &enemies, std::vector<Bullet> &bullets) //This code is ugly :(
{
	maxCamY = (int)ceil(camera->y / TILE_SIZE);
	maxCamH = (int)(((ceil(camera->y / TILE_SIZE) + ceil(camera->h / TILE_SIZE)) < TOTAL_TILES_Y - 1) ? ceil(camera->y / TILE_SIZE) + ceil(camera->h / TILE_SIZE) + 1 : ceil(camera->y / TILE_SIZE) + ceil(camera->h / TILE_SIZE));
	maxCamX = (int)ceil(camera->x / TILE_SIZE);
	maxCamW = 30;

	//Shoot weapon
	if (mouseLeft && energy >= weapon.energyCost && displayInv == false && reloading == false)
	{
		reloading = true;
		double strmod = (strength * 0.7) * ((double)weapon.reload / 60);
		energy -= weapon.energyCost;
		SDL_GetMouseState(&mouseClip.x, &mouseClip.y);
		double deltaX = (mouseClip.x + camera->x) - (Sprite::getPosition().first + (Sprite::getBox().w / 2));
		double deltaY = (mouseClip.y + camera->y) - (Sprite::getPosition().second + (Sprite::getBox().h / 2));
		double magnitude = sqrt(deltaX*deltaX + deltaY*deltaY);
		double xVelB = weapon.speed * (deltaX / magnitude);
		double yVelB = weapon.speed * (deltaY / magnitude);
		double xAng = (atan2(deltaY, deltaX)*180.0000) / 3.1416 + 90;
		double dmg = (weapon.minDamage+strmod) + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / ((weapon.maxDamage+strmod) - (weapon.minDamage+strmod))));
		weapon.bullet.type = BULLET_PLAYER;
		weapon.bullet.setPosition(Sprite::getBox().x + (Sprite::getBox().w / 2) - (weapon.bullet.getBox().w / 2), Sprite::getBox().y + (Sprite::getBox().h / 2) - (weapon.bullet.getBox().h / 2));
		weapon.bullet.setBullet(xVelB, yVelB, dmg, xAng, weapon.range);
		bullets.push_back(weapon.bullet);
	}

	//Set speed variables
	xVel = (Entity::velocity + (dexterity * 0.005)) * (right - left);
	yVel = (Entity::velocity + (dexterity * 0.005)) * (down - up);

	//Declare positions and check for x Position
	xPos = Sprite::getPosition().first + xVel;
	yPos = Sprite::getPosition().second;
	
	if (std::isnan(xPos) == false && std::isnan(yPos) == false)
		Sprite::setPosition(xPos, yPos);

	bool success = true;

	//Check collision between every tile
	if (enableCol) {
		for (unsigned int y = maxCamY; y <= maxCamH; y++)
		{
			for (unsigned int x = maxCamX; x <= maxCamW; x++)
			{
				if (tiles[x][y].getCollide() == true && checkCollision(Sprite::getBox(), tiles[x][y].getBox()) == true)
					success = false;
			}
		}
	}

	//Check collision between every sprite
	for (unsigned int i = 0; i < sprites.size(); i++)
	{
		if (checkCollision(*camera, sprites[i].getBox()) == true && sprites[i].getCollide() == true && checkCollision(Sprite::getBox(), sprites[i].getBox()) == true)
			success = false;
	}

	//Check collision between every enemy
	for (unsigned int i = 0; i < enemies.size(); i++)
	{
		if (checkCollision(*camera, enemies[i].getBox()) == true && enemies[i].getCollide() == true && checkCollision(Sprite::getBox(), enemies[i].getBox()) == true)
			success = false;
	}

	if (xPos < 0 || yPos < 0 || xPos+PLAYER_SIZE > DUNGEON_SIZE*TILE_SIZE || yPos+PLAYER_SIZE > DUNGEON_SIZE*TILE_SIZE)
		success = false;

	//If player collided with something, reverse movement
	if (success == false)
	{
		xPos -= xVel;
		//yPos -= yVel;
		if (std::isnan(xPos) == false && std::isnan(yPos) == false)
			Sprite::setPosition(xPos, yPos);
		xVel = 0;
	}

	//Check for y Position
	xPos = Sprite::getPosition().first;
	yPos = Sprite::getPosition().second + yVel;

	if (std::isnan(xPos) == false && std::isnan(yPos) == false)
		Sprite::setPosition(xPos, yPos);

	success = true;

	//Check collision between every tile
	if (enableCol) {
		for (unsigned int y = maxCamY; y <= maxCamH; y++)
		{
			for (unsigned int x = maxCamX; x <= maxCamW; x++)
			{
				if (tiles[x][y].getCollide() == true && checkCollision(Sprite::getBox(), tiles[x][y].getBox()) == true)
					success = false;
			}
		}
	}

	//Check collision between every sprite
	for (unsigned int i = 0; i < sprites.size(); i++)
	{
		if (checkCollision(*camera, sprites[i].getBox()) == true && sprites[i].getCollide() == true && checkCollision(Sprite::getBox(), sprites[i].getBox()) == true)
			success = false;
	}

	//Check collision between every enemy
	for (unsigned int i = 0; i < enemies.size(); i++)
	{
		if (checkCollision(*camera, enemies[i].getBox()) == true && enemies[i].getCollide() == true && checkCollision(Sprite::getBox(), enemies[i].getBox()) == true)
			success = false;
	}

	if (xPos < 0 || yPos < 0 || xPos + PLAYER_SIZE > DUNGEON_SIZE*TILE_SIZE || yPos + PLAYER_SIZE > DUNGEON_SIZE*TILE_SIZE)
		success = false;

	//If player collided with something, reverse movement
	if (success == false)
	{
		//xPos -= xVel;
		yPos -= yVel;
		if (std::isnan(xPos) == false && std::isnan(yPos) == false)
			Sprite::setPosition(xPos, yPos);
		yVel = 0;
	}

	canPick = false;
	//Check collision between every item and add collided ones into inventory
	for (unsigned int i = 0; i < items.size(); i++)
	{
		if (checkCollision(*camera, items[i].getBox()) == true && items[i].alive == true && checkCollision(Sprite::getBox(), items[i].getBox()) == true)
		{
			canPick = true;
			if (ekey)
			{
				for (int v = 0; v < MAX_INVENTORY; v++)
				{
					if (inventory[v].itemType == ITEM_NONE)
					{
						ekey = false;
						items[i].alive = false;
						inventory[v] = items[i];
						inventory[v].delCamera();
						inventory[v].setScale((double)ITEM_INV_SIZE / (double)inventory[v].getClip().w, (double)ITEM_INV_SIZE / (double)inventory[v].getClip().h);
						inventory[v].setBox(ITEM_INV_SIZE, ITEM_INV_SIZE);
						break;
					}
				}
			}
		}
	}

}

void Player::draw(SDL_Rect& camera)
{
	//Reloading
	if (reloading)
	{
		if (++reloadTime >= weapon.reload)
		{
			reloadTime = 0;
			reloading = false;
		}
	}

	setCamera(camera);
	Sprite::draw();
	if (enableCol) Sprite::setAlpha(255);
	else Sprite::setAlpha(96);
	if (displayInv)
	{
		displayInventory();
		if (itemSelected == true)
		{
			if (slotType == SLOT_INVENTORY)
				selectedTexture.render(gRenderer, mouseClip.x - mdiff, mouseClip.y - mdiff, &inventory[slotSelected].getClip());
			else if (slotType == SLOT_EQUIPMENT)
				selectedTexture.render(gRenderer, mouseClip.x - mdiff, mouseClip.y - mdiff, &equipment[slotSelected].getClip());
		}
	}
	if (canPick)
	{
		canPickSprite.setPosition(Sprite::getBox().x + (abs(canPickSprite.getSize().first - Sprite::getSize().first) / 2), Sprite::getBox().y - 40);
		canPickSprite.nextFrame();
		canPickSprite.draw();
	}

	//Target display
	if (targetEnemy != NULL)
	{
		if (targetEnemy->alive && targetEnemy->health > 0)
		{
			statClip = { 0, 50, 400, 25 };
			enemyStatBar.render(gRenderer, (SCREEN_WIDTH / 2) - 100, 50, &statClip);
			statClip = { 0, 0, (int)((targetEnemy->health / targetEnemy->maxHealth) * 400), 25 };
			enemyStatBar.render(gRenderer, (SCREEN_WIDTH / 2) - 100, 50, &statClip);

			statText.clear();
			statText.str(std::string());
			statText << targetEnemy->name;
			statBarText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman20, 400);
			statBarText.render(gRenderer, (SCREEN_WIDTH / 2) - statBarText.getWidth() / 2, 10);

			statText.clear();
			statText.str(std::string());
			if (targetEnemy->aiType == ENEMY_TYPE_NODE)
				statText << "Resource";
			else
				statText << "Level: " << targetEnemy->level;
			statBarText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman16, 400);
			statBarText.render(gRenderer, (SCREEN_WIDTH / 2) - statBarText.getWidth() / 2, 31);

			statText.clear();
			statText.str(std::string());
			statText << std::fixed << std::setprecision(0) << targetEnemy->health << "/" << targetEnemy->maxHealth;
			statBarText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman20, 400);
			statBarText.render(gRenderer, (SCREEN_WIDTH / 2) - statBarText.getWidth() / 2, 53);
		}
		else
		{
			delete targetEnemy;
			targetEnemy = NULL;
		}
		if (++targetTimer >= TARGET_TIMEOUT)
		{
			delete targetEnemy;
			targetEnemy = NULL;
		}
	}
	
}

void Player::displayInventory()
{
	inventorySheet.render(gRenderer, invSheetClip.x, invSheetClip.y);
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		if (slotSelected == i && slotType == SLOT_INVENTORY)
			inventoryBoxClicked.render(gRenderer, space * i + xoffset - ((i / xslots) * space * xslots), space * (i / xslots) + yoffset);
		else if (inventory[i].hovered == true)
			inventoryBoxHovered.render(gRenderer, space * i + xoffset - ((i / xslots) * space * xslots), space * (i / xslots) + yoffset);
		else
			inventoryBox.render(gRenderer, space * i + xoffset - ((i / xslots) * space * xslots), space * (i / xslots) + yoffset);

		inventory[i].setPosition(space * i + xoffset - ((i / xslots) * space * xslots), space * (i / xslots) + yoffset);
		if ((slotType != SLOT_INVENTORY || (slotSelected != i || (itemSelected == false && slotSelected == i))) && inventory[i].itemType != ITEM_NONE)
			inventory[i].draw();
	}

	equipment[0].setPosition(243, 115);
	equipment[1].setPosition(298, 60);
	equipment[2].setPosition(298, 115);
	equipment[3].setPosition(298, 170);

	for (int i = 0; i < MAX_EQUIPMENT; i++)
	{
		if (slotSelected == i && slotType == SLOT_EQUIPMENT)
			inventoryBoxClicked.render(gRenderer, equipment[i].getBox().x, equipment[i].getBox().y);
		else if (equipment[i].hovered == true && equipment[i].itemType != ITEM_NONE)
			inventoryBoxHovered.render(gRenderer, equipment[i].getBox().x, equipment[i].getBox().y);
		else if (equipment[i].hovered == true && equipment[i].itemType == ITEM_NONE)
			equipmentEmptyHover.render(gRenderer, equipment[i].getBox().x, equipment[i].getBox().y, &eqRect[i]);
		else if (equipment[i].hovered == false && equipment[i].itemType == ITEM_NONE)
			equipmentEmpty.render(gRenderer, equipment[i].getBox().x, equipment[i].getBox().y, &eqRect[i]);
		else
			inventoryBox.render(gRenderer, equipment[i].getBox().x, equipment[i].getBox().y);

		if ((slotType != SLOT_EQUIPMENT || (slotSelected != i || (itemSelected == false && slotSelected == i))) && equipment[i].itemType != ITEM_NONE)
			equipment[i].draw();
	}

	statText.clear();
	statText.str(std::string());
	statText.fixed;
	statText.precision(0);
	statText << "Level: " << level;
	statText << "\nEXP: " << exp << "/" << maxExp;
	if (skillPoints > 0)
	{
		statText << "\n\nSkill Points: " << skillPoints;
		incSTR.draw();
		incDEX.draw();
		incVIT.draw();
	}
	else
	{
		statText << "\n\n";
		incSTR.visible = false;
		incDEX.visible = false;
		incVIT.visible = false;
	}
	statText << "\n Strength: " << strength;
	statText << "\nDexterity: " << dexterity;
	statText << "\n Vitality: " << vitality;
	statText << "\n    Armor: " << armorRating;
	statText << "\n   Plasma: " << plasma;
	statBarText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman16, 400);
	statBarText.render(gRenderer, 50, 55);


	//Set tooltip
	if (itemSelected == false)
	{
		for (int i = 0; i < MAX_INVENTORY; i++)
		{
			if (inventory[i].itemType != ITEM_NONE && checkCollision(mouseClip, inventory[i].getBox()))
			{
				tooltip.setSize(inventory[i].tooltipWidth, inventory[i].tooltipSize);
				tooltip.setPosition(inventory[i].getBox().x + inventory[i].getBox().w, inventory[i].getBox().y);
				tooltip.draw();
				statBarText.loadFromRenderedText(gRenderer, inventory[i].name, inventory[i].rarity, CodeNewRoman16, 400);
				statBarText.render(gRenderer, tooltip.getBox().x + ((double)tooltip.getBox().w / 2) - statBarText.getWidth() / 2, tooltip.getBox().y+10);
				statBarText.loadFromRenderedText(gRenderer, inventory[i].tooltip, SDL_Color{ 255,255,255 }, CodeNewRoman16, 400);
				statBarText.render(gRenderer, tooltip.getBox().x + 10, tooltip.getBox().y + 10);
				return;
			}
		}
		for (int i = 0; i < MAX_EQUIPMENT; i++)
		{
			if (equipment[i].itemType != ITEM_NONE && checkCollision(mouseClip, equipment[i].getBox()))
			{
				tooltip.setSize(equipment[i].tooltipWidth, equipment[i].tooltipSize);
				tooltip.setPosition(equipment[i].getBox().x + equipment[i].getBox().w, equipment[i].getBox().y);
				tooltip.draw();
				statBarText.loadFromRenderedText(gRenderer, equipment[i].name, equipment[i].rarity, CodeNewRoman16, 400);
				statBarText.render(gRenderer, tooltip.getBox().x + ((double)tooltip.getBox().w / 2) - statBarText.getWidth() / 2, tooltip.getBox().y + 10);
				statBarText.loadFromRenderedText(gRenderer, equipment[i].tooltip, SDL_Color{ 255,255,255 }, CodeNewRoman16, 400);
				statBarText.render(gRenderer, tooltip.getBox().x + 10, tooltip.getBox().y + 10);
				return;
			}
		}
	}
}

void Player::checkLevel()
{
	//Level ups
	if (exp >= maxExp)
	{
		exp = exp - maxExp;
		maxExp *= 1.488;
		skillPoints += 5;
		level += 1;
		maxHealth += 20;
		health = maxHealth;
		maxEnergy += 10;
		energy = maxEnergy;
	}
}

void Player::buildLocalImages()
{
	inventoryBox.loadFromFile("bin/images/iconbox.png", Entity::gRenderer);
	inventoryBox.setScale((double)ITEM_INV_SIZE / 120.0, (double)ITEM_INV_SIZE / 120.0);

	inventoryBoxHovered.loadFromFile("bin/images/iconboxhover.png", Entity::gRenderer);
	inventoryBoxHovered.setScale((double)ITEM_INV_SIZE / 120.0, (double)ITEM_INV_SIZE / 120.0);

	inventoryBoxClicked.loadFromFile("bin/images/iconboxclick.png", Entity::gRenderer);
	inventoryBoxClicked.setScale((double)ITEM_INV_SIZE / 120.0, (double)ITEM_INV_SIZE / 120.0);

	equipmentEmpty.loadFromFile("bin/images/eqempty.png", Entity::gRenderer);
	equipmentEmpty.setScale((double)ITEM_INV_SIZE / 120.0, (double)ITEM_INV_SIZE / 120.0);

	equipmentEmptyHover.loadFromFile("bin/images/eqemptyhover.png", Entity::gRenderer);
	equipmentEmptyHover.setScale((double)ITEM_INV_SIZE / 120.0, (double)ITEM_INV_SIZE / 120.0);

	eqRect[EQUIPMENT_WEAPON] = { 0, 0, 120, 120 };
	eqRect[EQUIPMENT_HEAD] = { 120, 0, 120, 120 };
	eqRect[EQUIPMENT_CHEST] = { 0, 120, 120, 120 };
	eqRect[EQUIPMENT_LEGS] = { 120, 120, 120, 120 };

	inventorySheet.loadFromFile("bin/images/inventory.png", Entity::gRenderer);
	inventorySheet.setScale(385.0 / 570.0, 500.0 / 795.0);
	invSheetClip = { 20, 30, 385, 500 };

	statBar.loadFromFile("bin/images/health.png", Entity::gRenderer);

	expBar.loadFromFile("bin/images/exp.png", Entity::gRenderer);
	expBar.setScale(SCREEN_WIDTH / 400.0, 15.0 / 50.0);

	canPickSprite.setRender(gRenderer, gWindow);
	canPickSprite.camera = camera;
	canPickSprite.loadSpriteImage("bin/images/ekey.png")
		.setFrameSize(50, 50)
		.setPosition(0, 0)
		.setSize(30, 30)
		.setDelay(4)
		.setCollide(false)
		.pushFrameRow("Idle", 0, 0, 50, 0, 8)
		.setAnimation("Idle");

	enemyStatBar.loadFromFile("bin/images/health.png", Entity::gRenderer);
	enemyStatBar.setScale(0.5, 1);

	tooltip.setRender(gRenderer, gWindow);
	tooltip.loadSpriteImage("bin/images/tooltip.png")
		.setFrameSize(192, 92)
		.setPosition(0, 0)
		.setSize(200, 100)
		.setCollide(false)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");

	//Buttons
	incSTR.setRender(gRenderer, gWindow);
	incSTR.buildButton(BUTTON_PLUS, SDL_Rect{ 200, 123, 15, 15 });

	incDEX.setRender(gRenderer, gWindow);
	incDEX.buildButton(BUTTON_PLUS, SDL_Rect{ 200, 140, 15, 15 });

	incVIT.setRender(gRenderer, gWindow);
	incVIT.buildButton(BUTTON_PLUS, SDL_Rect{ 200, 157, 15, 15 });

	CodeNewRoman = TTF_OpenFont("bin/fonts/codenewroman.ttf", 24);
	CodeNewRoman20 = TTF_OpenFont("bin/fonts/codenewroman.ttf", 20);
	CodeNewRoman16 = TTF_OpenFont("bin/fonts/codenewroman.ttf", 16);
}

void Player::setCamera(SDL_Rect & camera)
{
	//camera.x = (Sprite::getBox().x + (Sprite::getBox().w / 2) - (SCREEN_WIDTH / 2));
	camera.y = (Sprite::getBox().y + (Sprite::getBox().h / 2) - (900 / 2));

	if (camera.x < 0)
		camera.x = 0;

	if (camera.y < 0)
		camera.y = 0;

	if (camera.x > FIELD_WIDTH - camera.w)
		camera.x = FIELD_WIDTH - camera.w;

	if (camera.y > FIELD_HEIGHT - camera.h)
		camera.y = FIELD_HEIGHT - camera.h;
}

void Player::setEquipment()
{
	//Set weapon
	if (equipment[EQUIPMENT_WEAPON].itemType == ITEM_WEAPON)
		weapon = equipment[EQUIPMENT_WEAPON].weapon;
	else
		weapon = defaultWeapon;

	//Calculate armor rating
	armorRating = 0;

	if (equipment[EQUIPMENT_HEAD].itemType == ITEM_HEAD)
		armorRating += equipment[EQUIPMENT_HEAD].armor.armorRating;

	if (equipment[EQUIPMENT_CHEST].itemType == ITEM_CHEST)
		armorRating += equipment[EQUIPMENT_CHEST].armor.armorRating;

	if (equipment[EQUIPMENT_LEGS].itemType == ITEM_LEGS)
		armorRating += equipment[EQUIPMENT_LEGS].armor.armorRating;
}

bool Player::inventoryFreeSlot()
{
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		if (inventory[i].itemType == ITEM_NONE)
			return true;
	}
	return false;
}

void Player::inventoryAddItem(int x)
{
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		if (inventory[i].itemType == ITEM_NONE)
		{
			inventory[i] = equipment[x];
			inventory[i].hovered = false;
			return;
		}
	}
}

//***********************************************************************************************************************
//																									*		Enemy		*
//***********************************************************************************************************************

Enemy::Enemy(SDL_Renderer *gR, SDL_Window *gW) : Entity(gR, gW)
{
	maxCamH = 0;
	maxCamW = 0;
	maxCamX = 0;
	maxCamY = 0;
	visionRange = 0;
	aiCamera = { 0, 0, 0, 0 };
	uniqueID = -1;
	dropChance = 0;
	minRange = true;
	alive = true;
}

Enemy::~Enemy()
{
}

void Enemy::handleDeath(std::vector<Item> &items, Player &player)
{
	if (health <= 0)
		alive = false;

	if (alive == false) //Enemy dies
	{
		//Reward exp
		player.exp += expReward;
		player.checkLevel();
		//Drop item
		int maxRoll = -1;
		std::vector<int> newTable;
		for (unsigned int i = 0; i < dropTable.size(); i++)
		{
			maxRoll += dropTable[i].second;
			newTable.push_back(dropTable[i].second);
			if ((int)(i - 1) >= 0)
				newTable[i] += newTable[(i - 1)];
		}
		int dropRoll = rand() % (100 - 1 + 1) + 1;
		if (dropRoll <= dropChance) //We got an item!
		{
			int itemRoll = rand() % (maxRoll - 0 + 1) + 0;
			for (unsigned int i = 0; i < dropTable.size(); i++)
			{
				if (itemRoll >= (newTable[i] - dropTable[i].second) && itemRoll < newTable[i])
				{
					dropTable[i].first.setPosition(Sprite::getPosition().first, Sprite::getPosition().second);
					items.push_back(dropTable[i].first);
					break;
				}
			}
		}
	}
}

void Enemy::handleMove(Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y], std::vector<Enemy> &enemies, Player &player, std::vector<Bullet> &bullets) //This code is ugly :(
{
	setCamera();

	if (weapon.range == 0)
		return;

	if (reloading)
	{
		if (++reloadTime >= weapon.reload)
		{
			reloadTime = 0;
			reloading = false;
		}
	}

	//IF YOU WANT TO MODIFY THE ENEMY AI, CHANGE THE END OF deltaX AND deltaY

	//Set deltas
	double divvy = (rand() % (40 - 30 + 1) + 30) / 10; //""Accuracy""
	deltaX = (player.getBox().x + (player.getBox().w / 2)) - (Sprite::getBox().x + (Sprite::getBox().w / 2)) + (player.xVel * ((double)player.getBox().w / divvy));
	deltaY = (player.getBox().y + (player.getBox().h / 2)) - (Sprite::getBox().y + (Sprite::getBox().h / 2)) + (player.yVel * ((double)player.getBox().h / divvy));
	magnitude = sqrt(deltaX*deltaX + deltaY*deltaY);

	diffX = abs(player.getBox().x + (player.getBox().w / 2)) - abs(Sprite::getBox().x + (Sprite::getBox().w / 2));
	diffY = abs(player.getBox().y + (player.getBox().h / 2)) - abs(Sprite::getBox().y + (Sprite::getBox().h / 2));

	if (sqrt(diffX*diffX + diffY*diffY) < (weapon.range - (weapon.range * (0.10 * minRange))))
	{
		if (reloading == false)
		{
			double xVelB = weapon.speed * (deltaX / magnitude);
			double yVelB = weapon.speed * (deltaY / magnitude);
			double xAng = (atan2(deltaY, deltaX)*180.0000) / 3.1416 + 90;
			double dmg = weapon.minDamage + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (weapon.maxDamage - weapon.minDamage)));
			weapon.bullet.type = BULLET_ENEMY;
			weapon.bullet.setPosition(Sprite::getBox().x + (Sprite::getBox().w / 2) - (weapon.bullet.getBox().w / 2), Sprite::getBox().y + (Sprite::getBox().h / 2) - (weapon.bullet.getBox().h / 2));
			weapon.bullet.setBullet(xVelB, yVelB, dmg, xAng, weapon.range);
			weapon.bullet.shooterID = uniqueID;
			bullets.push_back(weapon.bullet);
			reloading = true;
		}
		if (!wallWasHit)
			return;
	}

	if (Entity::velocity == 0)
		return;

	maxCamY = (int)ceil(aiCamera.y / TILE_SIZE);
	maxCamH = (int)(((ceil(aiCamera.y / TILE_SIZE) + ceil(aiCamera.h / TILE_SIZE)) < TOTAL_TILES_Y - 1) ? ceil(aiCamera.y / TILE_SIZE) + ceil(aiCamera.h / TILE_SIZE) + 1 : ceil(aiCamera.y / TILE_SIZE) + ceil(aiCamera.h / TILE_SIZE));
	maxCamX = (int)ceil(aiCamera.x / TILE_SIZE);
	maxCamW = (int)(((ceil(aiCamera.x / TILE_SIZE) + ceil(aiCamera.w / TILE_SIZE)) < TOTAL_TILES_X - 1) ? ceil(aiCamera.x / TILE_SIZE) + ceil(aiCamera.w / TILE_SIZE) + 1 : ceil(aiCamera.x / TILE_SIZE) + ceil(aiCamera.w / TILE_SIZE));
	
	bool Xsuccess = true;
	bool Ysuccess = true;

	//Set speed variables
	xVel = Entity::velocity * (deltaX / magnitude);
	if (xMod != 0)
		xVel = (Entity::velocity * xMod);// +(deltaX / magnitude) + (divvy / 20);
	yVel = Entity::velocity * (deltaY / magnitude);
	if (yMod != 0)
		yVel = (Entity::velocity * yMod);// +(deltaY / magnitude) + (divvy / 20);

	//Declare positions and check for x Position
	xPos = Sprite::getPosition().first + xVel;
	yPos = Sprite::getPosition().second;

	if (std::isnan(xPos) == false && std::isnan(yPos) == false)
		Sprite::setPosition(xPos, yPos);

	//Check collision between every tile
	for (unsigned int y = maxCamY; y <= maxCamH; y++)
	{
		for (unsigned int x = maxCamX; x <= maxCamW; x++)
		{
			if (tiles[x][y].getCollide() == true && checkCollision(Sprite::getBox(), tiles[x][y].getBox()) == true)
				Xsuccess = false;
		}
	}

	//Check collision between every enemy
	for (unsigned int i = 0; i < enemies.size(); i++)
	{
		if (checkCollision(aiCamera, enemies[i].getBox()) == true && enemies[i].uniqueID != uniqueID &&  enemies[i].getCollide() == true && checkCollision(Sprite::getBox(), enemies[i].getBox()) == true)
			Xsuccess = false;
	}

	//Check collision between player
	if (checkCollision(Sprite::getBox(), player.getBox()) == true)
		Xsuccess = false;

	//If player collided with something, reverse movement
	if (Xsuccess == false)
	{
		xPos -= xVel;
		//yPos -= yVel;
		if (std::isnan(xPos) == false && std::isnan(yPos) == false)
			Sprite::setPosition(xPos, yPos);
	}

	//Check for y Position
	xPos = Sprite::getPosition().first;
	yPos = Sprite::getPosition().second + yVel;

	if (std::isnan(xPos) == false && std::isnan(yPos) == false)
		Sprite::setPosition(xPos, yPos);

	//Check collision between every tile
	for (unsigned int y = maxCamY; y <= maxCamH; y++)
	{
		for (unsigned int x = maxCamX; x <= maxCamW; x++)
		{
			if (tiles[x][y].getCollide() == true && checkCollision(Sprite::getBox(), tiles[x][y].getBox()) == true)
				Ysuccess = false;
		}
	}

	//Check collision between every enemy
	for (unsigned int i = 0; i < enemies.size(); i++)
	{
		if (checkCollision(aiCamera, enemies[i].getBox()) == true && enemies[i].uniqueID != uniqueID &&  enemies[i].getCollide() == true && checkCollision(Sprite::getBox(), enemies[i].getBox()) == true)
			Ysuccess = false;
	}

	//Check collision between player
	if (checkCollision(Sprite::getBox(), player.getBox()) == true)
		Ysuccess = false;

	//If player collided with something, reverse movement
	if (Ysuccess == false)
	{
		//xPos -= xVel;
		yPos -= yVel;
		if (std::isnan(xPos) == false && std::isnan(yPos) == false)
			Sprite::setPosition(xPos, yPos);
	}

	//If you try to move on X, but was unable to
	if (xVel != 0 && Xsuccess == false && yMod == 0)
	{
		int posneg = rand() % (1 - 0 + 1) + 0;//(Sprite::getPosition().second + (Sprite::getBox().h / 2)) - (player.getPosition().second + (player.getBox().h / 2));
		if (posneg > 0)
			yMod = -1;
		else
			yMod = 1;
	}

	//If you try to move on Y, but was unable to
	if (yVel != 0 && Ysuccess == false && xMod == 0)
	{
		int posneg = rand() % (1 - 0 + 1) + 0;//(Sprite::getPosition().first + (Sprite::getBox().w / 2)) - (player.getPosition().first + (player.getBox().w / 2));
		if (posneg > 0)
			xMod = -1;
		else
			xMod = 1;
	}

	//Reset values if enemy is able to move
	if ((Xsuccess && Ysuccess) || resetMod++ > 30)
	{
		resetMod = 0;
		xMod = 0;
		yMod = 0;
	}
	//Else if he's jammed, try to move just 1 x or y
	else if (!Xsuccess && !Ysuccess)
	{
		int xdist = Sprite::getPosition().first - player.getPosition().first;
		int ydist = Sprite::getPosition().second - player.getPosition().second;
		if (xdist < ydist)
			xMod = -xMod;
		else
			yMod = -yMod;
	}

	//I do it in player.
	xVel = 0;
	yVel = 0;
}

void Enemy::setVision(int x)
{
	visionRange = x;
	aiCamera.x = (Sprite::getBox().x + (Sprite::getBox().w / 2) - (visionRange / 2));
	aiCamera.y = (Sprite::getBox().y + (Sprite::getBox().h / 2) - (visionRange / 2));

	aiCamera.w = visionRange;
	if (aiCamera.w > FIELD_WIDTH)
		aiCamera.w = FIELD_WIDTH;

	aiCamera.h = visionRange;
	if (aiCamera.h > FIELD_HEIGHT)
		aiCamera.h = FIELD_HEIGHT;

	if (aiCamera.x < 0)
		aiCamera.x = 0;

	if (aiCamera.y < 0)
		aiCamera.y = 0;

	if (aiCamera.x > FIELD_WIDTH - aiCamera.w)
		aiCamera.x = FIELD_WIDTH - aiCamera.w;

	if (aiCamera.y > FIELD_HEIGHT - aiCamera.h)
		aiCamera.y = FIELD_HEIGHT - aiCamera.h;
}

void Enemy::setCamera()
{
	aiCamera.x = (Sprite::getBox().x + (Sprite::getBox().w / 2) - (visionRange / 2));
	aiCamera.y = (Sprite::getBox().y + (Sprite::getBox().h / 2) - (visionRange / 2));

	if (aiCamera.x < 0)
		aiCamera.x = 0;

	if (aiCamera.y < 0)
		aiCamera.y = 0;

	if (aiCamera.x > FIELD_WIDTH - aiCamera.w)
		aiCamera.x = FIELD_WIDTH - aiCamera.w;

	if (aiCamera.y > FIELD_HEIGHT - aiCamera.h)
		aiCamera.y = FIELD_HEIGHT - aiCamera.h;
}

void Enemy::draw()
{
	Sprite::draw();
}

void Enemy::drawHealth()
{
	statClip = { 0, 50, 400, 25 };
	statBar.render(Entity::gRenderer, getPosition().first - camera->x, getPosition().second - 10 - camera->y, &statClip);

	statClip = { 0, 0, (int)((health / maxHealth) * 400), 25 };
	statBar.render(Entity::gRenderer, getPosition().first - camera->x, getPosition().second - 10 - camera->y, &statClip);
}

void Enemy::buildLocalImages()
{
	statBar.loadFromFile("bin/images/health.png", Entity::gRenderer);
	statBar.setScale((getSize().first) / 400.0, 15.0 / 75.0);

	if (Entity::velocity == 0)
		minRange = false;
}
