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

void Entity::setRenderCamera(SDL_Renderer *gR, SDL_Window *gW, SDL_Rect *cM)
{
	gRenderer = gR;
	gWindow = gW;
	Sprite::setRender(gR, gW);
	camera = cM;
}

bool effectToDie(const Effect *e)
{
	return !e->alive;
}

void Entity::effectManager(vector<FloatingText> &texts, TTF_Font*font)
{
	for (unsigned int i = 0; i < effects.size(); i++)
	{
		effects[i]->runEffect(this, texts, font);
		if (effects[i]->alive == false)
		{
			effects.erase(remove_if(effects.begin(), effects.end(), effectToDie), effects.end());
			i--;
		}
	}
}

void Entity::takeDamage(double dmg, bool toShields)
{
	if (toShields == false)
	{
		health -= dmg;
	}
	else
	{
		double pdmg = dmg;
		if (shields > 0)
		{
			if (shields < pdmg)
			{
				pdmg -= shields;
				shields = 0;
				health -= pdmg;
			}
			else if (shields >= pdmg)
			{
				shields -= pdmg;
			}
		}
		else
		{
			health -= pdmg;
		}
	}
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
	isPlayer = true;
	displayInv = false;
	itemSelected = false;
	slotSelected = -1;
	mdiff = ITEM_INV_SIZE / 2;
	mouseClip = { 0, 0, 0, 0 };
	statText.str(std::string());
	statText.precision(0);

	for (int i = 0; i < 5; i++)
		buttons.push_back(&clearSlots[i]);

	for (int i = 0; i < 5; i++)
		for (int v = 0; v < 2; v++)
			spellCooldown[i][v] = 0;

	for (int i = 0; i < 5; i++)
		spellBook[i] = "Empty";
}

Player::~Player()
{
}

double calculateDPS(Weapon weapon, double strmod)
{
	double dps = ((weapon.minDamage + strmod) + (((weapon.maxDamage + strmod) - (weapon.minDamage + strmod)) / 2)) * (60.0 / (double)weapon.reload);
	return dps;
}

double calculateEPS(Weapon weapon)
{
	return (weapon.energyCost * (60.0 / (double)weapon.reload));
}

int calculateArmor(int level, int armor)
{
	double reduc = (armor * pow(1.0 / 2.0, (double)level / 7.5)) / 100.0;
	if (reduc > BULLET_REDUC)
		reduc = BULLET_REDUC;
	return std::round(reduc*100);
}

void Player::handleEvents(SDL_Event * e, std::vector<Item> &items, std::vector<FloatingText>&texts)
{
	//Button events
	for (unsigned int i = 0; i < buttons.size(); i++)
	{
		buttons[i]->handleEvents(e);
	}

	//Clear spellbook slot
	for (int i = 0; i < 5; i++)
	{
		if (clearSlots[i].released && offCooldown(texts, i))
		{
			bool deb = false;
			for (unsigned int sp = 1; sp < globalSpells.size(); sp++)
			{
				if (globalSpells[sp]->name == spellBook[i] && globalSpells[sp]->isActive())
				{
					deb = true;
					break;
				}
			}
			if (!deb)
			{
				spellBook[i] = "Empty";
				clickedSpell = false;
				selectedSpell = -1;
				clickingSpell = false;
				for (int i = 0; i < 6; i++)
					aicc[i] = 0;
				rebuildAttackText();
			}
			clearSlots[i].released = false;
			clearSlots[i].setAnimation("Idle");
		}
	}

	//Increase Destruction
	if (incDES.released)
	{
		if (saveFile.spellPoints > 0)
		{
			saveFile.damageLevel++;
			saveFile.spellPoints--;
			rebuildAttackText();
		}
		incDES.released = false;
	}

	//Increase Restoration
	if (incRES.released)
	{
		if (saveFile.spellPoints > 0)
		{
			saveFile.defenseLevel++;
			saveFile.spellPoints--;
			rebuildAttackText();
		}
		incRES.released = false;
	}
	
	//Increase Strength
	if (incSTR.released)
	{
		if (skillPoints > 0)
		{
			strength++;
			skillPoints--;
			rebuildInventoryText();
		}
		incSTR.released = false;
	}

	//Increase Vitality
	if (incVIT.released)
	{
		if (skillPoints > 0)
		{
			vitality++;
			health += VIT_BONUS;
			skillPoints--;
			rebuildInventoryText();
		}
		incVIT.released = false;
	}

	//Increase Dexterity
	if (incDEX.released)
	{
		if (skillPoints > 0)
		{
			dexterity++;
			energy += DEX_BONUS;
			skillPoints--;
			rebuildInventoryText();
		}
		incDEX.released = false;
	}

	//Convert Plasma from Plasma Reactor
	if (reactorConvert.released)
	{
		pressedReactor = true;
		reactorConvert.released = false;
		reactorConvert.setAnimation("Idle");
	}

	//Exit Game
	if (exitGame.released)
	{
		killSwitch = true;
		exitGame.released = false;
		exitGame.setAnimation("Idle");
	}

	//Resume Game
	if (resumeGame.released)
	{
		closeEscapeMenu();
		resumeGame.released = false;
		resumeGame.setAnimation("Idle");
	}

	//Armor Info
	if (armorInfo.released)
	{
		infoPage = "Armor";
		rebuildInventoryText();
		armorInfo.released = false;
		armorInfo.setAnimation("Idle");
		weaponInfo.setAnimation("Hover");
	}

	//Weapon Info
	if (weaponInfo.released)
	{
		infoPage = "Weapon";
		rebuildInventoryText();
		weaponInfo.released = false;
		weaponInfo.setAnimation("Idle");
		armorInfo.setAnimation("Hover");
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

		case SDLK_z:
			if (!ckey)
				quickHeal(texts);
			ckey = true;
			break;

		case SDLK_x:
			if (!vkey)
				quickEnergy(texts);
			vkey = true;
			break;

		case SDLK_RCTRL:
		case SDLK_LCTRL:
			ctrl = true;
			break;

		case SDLK_1:
			castSpell(texts, 1); //0 - 4, 0 is Right-Click, 1-4 is 1-4 numbers
			break;

		case SDLK_2:
			castSpell(texts, 2); //0 - 4, 0 is Right-Click, 1-4 is 1-4 numbers
			break;

		case SDLK_3:
			castSpell(texts, 3); //0 - 4, 0 is Right-Click, 1-4 is 1-4 numbers
			break;

		case SDLK_4:
			castSpell(texts, 4); //0 - 4, 0 is Right-Click, 1-4 is 1-4 numbers
			break;

		case SDLK_g:
			if (!hkey && !msgbox)
			{
				if (displayEscape)
					closeEscapeMenu();
				if (displayInv)
					closeInventory();
				if (displaySpells)
					closeSpells();
				else
				{
					displaySpells = true;
					rebuildAttackText();
				}
			}
			hkey = true;
			break;

		case SDLK_i:
		case SDLK_TAB:
			if (!tab && !msgbox)
			{
				if (displayEscape)
					closeEscapeMenu();
				if (displaySpells)
					closeSpells();
				if (displayInv)
					closeInventory();
				else
				{
					displayInv = true;
					rebuildInventoryText();
				}
			}
			tab = true;
			break;

		case SDLK_ESCAPE:
			if (!esckey)
			{
				if (displayInv || displaySpells || msgbox)
				{
					if (displayInv)
						closeInventory();
					if (displaySpells)
						closeSpells();
				}
				else
				{
					if (displayEscape)
						closeEscapeMenu();
					else if (!displayEscape)
						displayEscape = true;
				}
			}
			esckey = true;
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

		case SDLK_z:
			ckey = false;
			break;

		case SDLK_x:
			vkey = false;
			break;

		case SDLK_RCTRL:
		case SDLK_LCTRL:
			ctrl = false;
			break;

		case SDLK_g:
			hkey = false;
			break;

		case SDLK_i:
		case SDLK_TAB:
			tab = false;
			break;

		case SDLK_ESCAPE:
			esckey = false;
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
				if (otherInv != NULL)
				{
					for (int i = 0; i < MAX_STORAGE; i++)
					{
						if (checkCollision(mouseClip, otherInv->items[i].getBox()) == true)
							otherInv->items[i].hovered = true;
						else
							otherInv->items[i].hovered = false;
					}
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
				else if (otherInv != NULL && slotType == SLOT_STORAGE)
				{
					if (checkCollision(mouseClip, otherInv->items[slotSelected].getBox()) == false)
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
			mouseLeft = true;

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
				if (otherInv != NULL)
				{
					for (int i = 0; i < MAX_STORAGE; i++)
					{
						if (itemSelected == false && slotSelected == -1 && checkCollision(mouseClip, otherInv->items[i].getBox()) == true && otherInv->items[i].itemType != ITEM_NONE)
						{
							slotSelected = i;
							selectedTexture = otherInv->items[i].getTexture();
							slotType = SLOT_STORAGE;
						}
					}
				}
			}
		}

		//Right click
		if (e->button.button == SDL_BUTTON_RIGHT)
		{
			mouseRight = true;
			castSpell(texts, 0);
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
							else if (slotType == SLOT_EQUIPMENT && (inventory[i].itemType == equipment[slotSelected].itemType || inventory[i].itemType == ITEM_NONE) && level < inventory[i].levelreq)
							{
								texts.push_back(FloatingText("Level Not High Enough!", Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman16, SDL_Color{ 255,0,0 }));
								break;
							}
							//Swap from otherInv to inventory
							if (slotType == SLOT_STORAGE)
							{
								inventory[i].hovered = false;
								otherInv->items[slotSelected].hovered = true;
								Item holder = otherInv->items[slotSelected];
								otherInv->items[slotSelected] = inventory[i];
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
							//Swap from inventory to equipment
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
								else if (inventory[slotSelected].itemType == i + 1 && level < inventory[slotSelected].levelreq)
								{
									texts.push_back(FloatingText("Level Not High Enough!", Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman16, SDL_Color{ 255,0,0 }));
									break;
								}
							}
							//Swap from otherInv to equipment
							else if (otherInv != NULL && slotType == SLOT_STORAGE && checkCollision(mouseClip, equipment[i].getBox()) == true)
							{
								if (otherInv->items[slotSelected].itemType == i + 1 && level >= otherInv->items[slotSelected].levelreq)
								{
									equipment[i].hovered = false;
									otherInv->items[slotSelected].hovered = true;
									Item holder = otherInv->items[slotSelected];
									otherInv->items[slotSelected] = equipment[i];
									equipment[i] = holder;
									itemSelected = false;
									slotSelected = -1;
									slotType = SLOT_NONE;
									debounce = true;
									hoverRefresh = true;
									break;
								}
								else if (otherInv->items[slotSelected].itemType == i + 1 && level < otherInv->items[slotSelected].levelreq)
								{
									texts.push_back(FloatingText("Level Not High Enough!", Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman16, SDL_Color{ 255,0,0 }));
									break;
								}
							}
						}
					}
					if (!debounce && otherInv != NULL)
					{
						//Swap from inventory to otherInv
						for (int i = 0; i < MAX_STORAGE; i++)
						{
							if (checkCollision(mouseClip, otherInv->items[i].getBox()) == true)
							{
								//Swap from inventory to otherInv
								if (slotType == SLOT_INVENTORY)
								{
									otherInv->items[i].hovered = false;
									inventory[slotSelected].hovered = true;
									Item holder = inventory[slotSelected];
									inventory[slotSelected] = otherInv->items[i];
									otherInv->items[i] = holder;
									itemSelected = false;
									slotSelected = -1;
									slotType = SLOT_NONE;
									debounce = true;
									hoverRefresh = true;
									break;
								}
								//Swap from otherInv to otherInv
								else if (slotType == SLOT_STORAGE)
								{
									otherInv->items[i].hovered = false;
									otherInv->items[slotSelected].hovered = true;
									Item holder = otherInv->items[slotSelected];
									otherInv->items[slotSelected] = otherInv->items[i];
									otherInv->items[i] = holder;
									itemSelected = false;
									slotSelected = -1;
									slotType = SLOT_NONE;
									debounce = true;
									hoverRefresh = true;
									break;
								}
								//Swap from equipment to otherInv
								else if (slotType == SLOT_EQUIPMENT && (otherInv->items[i].itemType == equipment[slotSelected].itemType || otherInv->items[i].itemType == ITEM_NONE) && level >= otherInv->items[i].levelreq)
								{
									otherInv->items[i].hovered = false;
									equipment[slotSelected].hovered = true;
									Item holder = equipment[slotSelected];
									equipment[slotSelected] = otherInv->items[i];
									otherInv->items[i] = holder;
									itemSelected = false;
									slotSelected = -1;
									slotType = SLOT_NONE;
									debounce = true;
									hoverRefresh = true;
									break;
								}
								else if (slotType == SLOT_EQUIPMENT && (otherInv->items[i].itemType == equipment[slotSelected].itemType || otherInv->items[i].itemType == ITEM_NONE) && level < otherInv->items[i].levelreq)
								{
									texts.push_back(FloatingText("Level Not High Enough!", Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman16, SDL_Color{ 255,0,0 }));
									break;
								}
							}
						}
						
					}
					//Drop item on floor
					if (!debounce && !checkCollision(mouseClip, invSheetClip) && (otherInv == NULL || !checkCollision(mouseClip, oinvSheetClip)))
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
						else if (otherInv != NULL && slotType == SLOT_STORAGE)
						{
							otherInv->items[slotSelected].setPosition(Sprite::getBox().x, Sprite::getBox().y);
							otherInv->items[slotSelected].alive = true;
							otherInv->items[slotSelected].camera = camera;
							otherInv->items[slotSelected].setScale((double)ITEM_MAP_SIZE / (double)otherInv->items[slotSelected].getClip().w, (double)ITEM_MAP_SIZE / (double)otherInv->items[slotSelected].getClip().h);
							otherInv->items[slotSelected].setBox(ITEM_MAP_SIZE, ITEM_MAP_SIZE);
							items.push_back(otherInv->items[slotSelected]);
							otherInv->items[slotSelected].clearSlot();
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
						else if (otherInv != NULL && slotType == SLOT_STORAGE)
							otherInv->items[slotSelected].hovered = false;
						itemSelected = false;
						slotSelected = -1;
						slotType = SLOT_NONE;
						hoverRefresh = true;
					}
				}
				//Picking item up
				else if (itemSelected == false && slotSelected > -1)
				{
					if (reactor) //Set item on plasma Reactor
					{
						if (slotType == SLOT_INVENTORY  && checkCollision(mouseClip, inventory[slotSelected].getBox()) && ctrl)
						{
							reactorSlot = slotSelected;
							plasma += inventory[reactorSlot].value;
							texts.push_back(FloatingText("+" + std::to_string(inventory[reactorSlot].value), getBox().x + PLAYER_SIZE / 2, getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman, SDL_Color{ 0,255,255 }));
							inventory[reactorSlot].clearSlot();
							reactorSlot = -1;
							itemSelected = false;
							slotSelected = -1;
							slotType = SLOT_NONE;
							hoverRefresh = true;
							rebuildInventoryText();
						}
						else if (reactorSlot != -1 && reactorSlot == slotSelected)
						{
							reactorSlot = -1;
							itemSelected = false;
							slotSelected = -1;
							slotType = SLOT_NONE;
							hoverRefresh = true;
						}
						else if (slotType == SLOT_INVENTORY  && checkCollision(mouseClip, inventory[slotSelected].getBox()) && !ctrl)
						{
							reactorSlot = slotSelected;
							itemSelected = false;
							slotSelected = -1;
							slotType = SLOT_NONE;
							hoverRefresh = true;
						}
						else
						{
							itemSelected = false;
							slotSelected = -1;
							slotType = SLOT_NONE;
							hoverRefresh = true;
						}
					}
					else //If plasma Reactor isn't open
					{
						if (slotType == SLOT_INVENTORY && checkCollision(mouseClip, inventory[slotSelected].getBox()))
						{
							itemSelected = true;
						}
						else if (slotType == SLOT_EQUIPMENT && checkCollision(mouseClip, equipment[slotSelected].getBox()))
						{
							itemSelected = true;
						}
						else if (otherInv != NULL && slotType == SLOT_STORAGE && checkCollision(mouseClip, otherInv->items[slotSelected].getBox()))
						{
							itemSelected = true;
						}
						else
						{
							itemSelected = false;
							slotSelected = -1;
							slotType = SLOT_NONE;
							hoverRefresh = true;
						}
					}
				}
				setEquipment();
			}
		}

		//Right click
		if (e->button.button == SDL_BUTTON_RIGHT)
		{
			mouseRight = false;

			//Inventory events
			if (displayInv == true && !reactor)
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
						//Equip item from inventory
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
									else if (!debounce && inventory[i].itemType == v + 1 && level < inventory[i].levelreq)
									{
										texts.push_back(FloatingText("Level Not High Enough!", Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman16, SDL_Color{ 255,0,0 }));
										break;
									}
								}

								if (!debounce && inventory[i].itemType == ITEM_POTION)
								{
									if ((health != maxHealth && inventory[i].potion.healthAmount > 0) || (energy != maxEnergy && inventory[i].potion.energyAmount > 0))
									{
										int potentialheal = inventory[i].potion.healthAmount;
										health += inventory[i].potion.healthAmount;
										if (health > maxHealth)
										{
											potentialheal -= health - maxHealth;
											health = maxHealth;
										}
										if (potentialheal > 0)
											texts.push_back(FloatingText("+" + std::to_string(potentialheal), Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman20, SDL_Color{ 0,255,0 }));
										int potentialenergy = inventory[i].potion.energyAmount;
										energy += inventory[i].potion.energyAmount;
										if (energy > maxEnergy)
										{
											potentialenergy -= energy - maxEnergy;
											energy = maxEnergy;
										}
										if (potentialenergy > 0)
											texts.push_back(FloatingText("+" + std::to_string(potentialenergy), Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman20, SDL_Color{ 255,205,0 }));
										inventory[i].clearSlot();
										inventory[i].hovered = true;
										hoverRefresh = true;
										debounce = true;
									}
									else
									{
										texts.push_back(FloatingText("I'm already healed!", Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman16, SDL_Color{ 255,0,0 }));
									}
								}
							}
						}
						//Equip item from otherInv
						if (otherInv != NULL)
						{
							for (int i = 0; i < MAX_STORAGE; i++)
							{
								if (checkCollision(mouseClip, otherInv->items[i].getBox()) == true)
								{
									for (int v = 0; v < MAX_EQUIPMENT; v++)
									{
										if (!debounce && otherInv->items[i].itemType == v + 1 && level >= otherInv->items[i].levelreq)
										{
											Item holder = equipment[v];
											equipment[v] = otherInv->items[i];
											otherInv->items[i] = holder;
											otherInv->items[i].hovered = true;
											equipment[v].hovered = false;
											debounce = true;
											hoverRefresh = true;
											break;
										}
										else if (!debounce && otherInv->items[i].itemType == v + 1 && level < otherInv->items[i].levelreq)
										{
											texts.push_back(FloatingText("Level Not High Enough!", Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman16, SDL_Color{ 255,0,0 }));
											break;
										}
									}

									if (!debounce && otherInv->items[i].itemType == ITEM_POTION)
									{
										if ((health != maxHealth && otherInv->items[i].potion.healthAmount > 0) || (energy != maxEnergy && otherInv->items[i].potion.energyAmount > 0))
										{
											int potentialheal = otherInv->items[i].potion.healthAmount;
											health += otherInv->items[i].potion.healthAmount;
											if (health > maxHealth)
											{
												potentialheal -= health - maxHealth;
												health = maxHealth;
											}
											if (potentialheal > 0)
												texts.push_back(FloatingText("+" + std::to_string(potentialheal), Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman20, SDL_Color{ 0,255,0 }));
											int potentialenergy = otherInv->items[i].potion.energyAmount;
											energy += otherInv->items[i].potion.energyAmount;
											if (energy > maxEnergy)
											{
												potentialenergy -= energy - maxEnergy;
												energy = maxEnergy;
											}
											if (potentialenergy > 0)
												texts.push_back(FloatingText("+" + std::to_string(potentialenergy), Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman20, SDL_Color{ 255,205,0 }));
											otherInv->items[i].clearSlot();
											otherInv->items[i].hovered = true;
											hoverRefresh = true;
											debounce = true;
										}
										else
										{
											texts.push_back(FloatingText("I'm already healed!", Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman16, SDL_Color{ 255,0,0 }));
										}
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

void Player::handleMove(Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y], std::vector<Sprite> &sprites, std::vector<Item> &items, std::vector<Enemy> &enemies, std::vector<Bullet> &bullets, std::vector<PressurePlate> &pressurePlates) //This code is ugly :(
{
	if (msgbox)
		msgbox = false;

	maxCamY = (int)ceil(camera->y / TILE_SIZE);
	maxCamH = (int)(((ceil(camera->y / TILE_SIZE) + ceil(camera->h / TILE_SIZE)) < TOTAL_TILES_Y - 1) ? ceil(camera->y / TILE_SIZE) + ceil(camera->h / TILE_SIZE) + 1 : ceil(camera->y / TILE_SIZE) + ceil(camera->h / TILE_SIZE));
	maxCamX = (int)ceil(camera->x / TILE_SIZE);
	maxCamW = (int)(((ceil(camera->x / TILE_SIZE) + ceil(camera->w / TILE_SIZE)) < TOTAL_TILES_X - 1) ? ceil(camera->x / TILE_SIZE) + ceil(camera->w / TILE_SIZE) + 1 : ceil(camera->x / TILE_SIZE) + ceil(camera->w / TILE_SIZE));

	SDL_GetMouseState(&mouseClip.x, &mouseClip.y);
	double deltaX = (mouseClip.x + camera->x) - (Sprite::getPosition().first + (Sprite::getBox().w / 2));
	double deltaY = (mouseClip.y + camera->y) - (Sprite::getPosition().second + (Sprite::getBox().h / 2));
	double xAng = (atan2(deltaY, deltaX)*180.0000) / 3.1416 + 90;
	Sprite::setAngle(xAng);
	weapon.sprite.setAngle(xAng);
	bubble.setAngle(xAng);

	if (weapon.sprite.getCurrentAnimation() == "Shoot")
	{
		weapon.sprite.nextFrame();
		if (weapon.sprite.finishedAnimation())
			weapon.sprite.setAnimation("Idle");
	}

	//Shoot weapon
	if (mouseLeft && energy >= weapon.energyCost && displayInv == false && displaySpells == false && displayEscape == false && reloading == false)
	{
		reloading = true;
		double strmod = (strength * STR_BONUS) * ((double)weapon.reload / 60);
		energy -= weapon.energyCost;
		double magnitude = sqrt(deltaX*deltaX + deltaY*deltaY);
		double xVelB = weapon.speed * (deltaX / magnitude);
		double yVelB = weapon.speed * (deltaY / magnitude);
		double dmg = (weapon.minDamage+strmod) + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / ((weapon.maxDamage+strmod) - (weapon.minDamage+strmod))));
		weapon.bullet.type = BULLET_PLAYER;
		weapon.bullet.setPosition(Sprite::getBox().x + (Sprite::getBox().w / 2) - (weapon.bullet.getBox().w / 2), Sprite::getBox().y + (Sprite::getBox().h / 2) - (weapon.bullet.getBox().h / 2));
		weapon.bullet.setBullet(xVelB, yVelB, dmg, xAng, weapon.range);
		weapon.sprite.setAnimation("Shoot");
		bullets.push_back(weapon.bullet);
	}

	//Set speed variables
	xVel = (Entity::velocity + Entity::velocityBonus + (dexterity * 0.005)) * (right - left);
	yVel = (Entity::velocity + Entity::velocityBonus + (dexterity * 0.005)) * (down - up);
	if (xVel != 0 && yVel != 0)
	{
		xVel /= sqrt(2.0);
		yVel /= sqrt(2.0);
	}

	//Declare positions and check for x Position
	xPos = Sprite::getPosition().first + xVel;
	yPos = Sprite::getPosition().second;
	
	if (std::isnan(xPos) == false && std::isnan(yPos) == false)
		Sprite::setPosition(xPos, yPos);

	bool success = true;

	//Check collision between every tile
	for (unsigned int y = maxCamY; y <= maxCamH; y++)
	{
		for (unsigned int x = maxCamX; x <= maxCamW; x++)
		{
			if (tiles[x][y].getCollide() == true && checkCollision(Sprite::getBox(), tiles[x][y].getBox()) == true)
				success = false;
		}
	}

	//Check collision between every sprite
	for (unsigned int i = 0; i < sprites.size(); i++)
	{
		if (checkCollision(*camera, sprites[i].getBox()) == true && sprites[i].getCollide() == true && checkCollision(Sprite::getBox(), sprites[i].getBox()) == true)
			success = false;
	}

	//Check collision between every pressurePlate
	for (unsigned int i = 0; i < pressurePlates.size(); i++)
	{
		if (checkCollision(*camera, pressurePlates[i].box) == true && checkCollision(Sprite::getBox(), pressurePlates[i].box) == true)
		{
			pressurePlates[i].active = true;
		}
	}

	//Check collision between every enemy
	for (unsigned int i = 0; i < enemies.size(); i++)
	{
		if (checkCollision(*camera, enemies[i].getBox()) == true && enemies[i].getCollide() == true && checkCollision(Sprite::getBox(), enemies[i].getBox()) == true)
			success = false;
	}

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
	for (unsigned int y = maxCamY; y <= maxCamH; y++)
	{
		for (unsigned int x = maxCamX; x <= maxCamW; x++)
		{
			if (tiles[x][y].getCollide() == true && checkCollision(Sprite::getBox(), tiles[x][y].getBox()) == true)
				success = false;
		}
	}

	//Check collision between every sprite
	for (unsigned int i = 0; i < sprites.size(); i++)
	{
		if (checkCollision(*camera, sprites[i].getBox()) == true && sprites[i].getCollide() == true && checkCollision(Sprite::getBox(), sprites[i].getBox()) == true)
			success = false;
	}

	//Check collision between every pressurePlate
	for (unsigned int i = 0; i < pressurePlates.size(); i++)
	{
		if (checkCollision(*camera, pressurePlates[i].box) == true && checkCollision(Sprite::getBox(), pressurePlates[i].box) == true)
		{
			pressurePlates[i].active = true;
		}
	}

	//Check collision between every enemy
	for (unsigned int i = 0; i < enemies.size(); i++)
	{
		if (checkCollision(*camera, enemies[i].getBox()) == true && enemies[i].getCollide() == true && checkCollision(Sprite::getBox(), enemies[i].getBox()) == true)
			success = false;
	}

	//If player collided with something, reverse movement
	if (success == false)
	{
		//xPos -= xVel;
		yPos -= yVel;
		if (std::isnan(xPos) == false && std::isnan(yPos) == false)
			Sprite::setPosition(xPos, yPos);
		yVel = 0;
	}

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
						rebuildInventoryText();
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

bool textToDie(const FloatingText &t)
{
	return !t.alive;
}

void Player::draw(SDL_Rect& camera, std::vector<FloatingText> &texts)
{
	checkStats();

	if (!inCombat)
	{
		healthRegen = ((maxHealth * BASE_HEALTH_MULTIPLYER) + BASE_HEALTH_REGEN) / 60;
		health += healthRegen;
	}
	if (health > maxHealth)
		health = maxHealth;

	if (inCombat)
	{
		combatTimer++;
		if (combatTimer >= TARGET_TIMEOUT)
		{
			combatTimer = 0;
			inCombat = false;
		}
	}

	if (shields <= 0)
	{
		for (unsigned int i = 0; i < effects.size(); i++)
		{
			if (!effects[i]->runtime && effects[i]->type == "shield" && effects[i]->alive)
			{
				effects[i]->alive = false;
				effects[i]->texture.free();
			}
		}
	}

	energyRegen = ((maxEnergy * 0.02) + BASE_ENERGY_REGEN) / 60;
	energy += energyRegen;
	if (energy >= maxEnergy)
		energy = maxEnergy;

	effectManager(texts, codeNewRoman18);

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
	if (xVel != 0 || yVel != 0)
	{
		legFrame++;
		Sprite::setFrame(legFrame);
		Sprite::setAnimationRaw("Legs");
		SDL_SetTextureColorMod(Sprite::getTexture(), LegsColor.r, LegsColor.g, LegsColor.b);
		Sprite::draw();
		if (legFrame > Sprite::getCurrentAnimationSize())
			legFrame = 0;
	}
	Sprite::setAnimationRaw("Chest");
	SDL_SetTextureColorMod(Sprite::getTexture(), ChestColor.r, ChestColor.g, ChestColor.b);
	Sprite::draw();
	SDL_SetTextureColorMod(Sprite::getTexture(), HelmetColor.r, HelmetColor.g, HelmetColor.b);
	Sprite::setAnimationRaw("Helmet");
	Sprite::draw();

	if (shields > 0)
	{
		bubble.setPosition(Sprite::getBox().x + 1, Sprite::getBox().y + 9);
		bubble.nextFrame();
		bubble.draw();
	}

	weapon.sprite.setPosition(Sprite::getBox().x + 10, Sprite::getBox().y - 13);
	weapon.sprite.draw();

	if (levelRefresh)
	{
		levelRefresh = false;
		texts.push_back(FloatingText("Level Up!", getBox().x + PLAYER_SIZE / 2, getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRomanBig, SDL_Color{ 0,255,0 }));
	}

	for (unsigned int i = 0; i < texts.size(); i++)
	{
		texts[i].run(gRenderer, camera);
		if (texts[i].alive == false)
		{
			texts.erase(std::remove_if(texts.begin(), texts.end(), textToDie), texts.end());
			i--;
		}
	}

	if (canPick)
	{
		canPickSprite.setPosition(Sprite::getBox().x + (abs(canPickSprite.getSize().first - Sprite::getSize().first) / 2), Sprite::getBox().y - 40);
		canPickSprite.nextFrame();
		canPickSprite.draw();
	}

	if (displayInv)
	{
		displayInventory();
		if (itemSelected == true)
		{
			if (slotType == SLOT_INVENTORY)
				selectedTexture.render(gRenderer, mouseClip.x - mdiff, mouseClip.y - mdiff, &inventory[slotSelected].getClip());
			else if (slotType == SLOT_EQUIPMENT)
				selectedTexture.render(gRenderer, mouseClip.x - mdiff, mouseClip.y - mdiff, &equipment[slotSelected].getClip());
			else if (otherInv != NULL && slotType == SLOT_STORAGE)
				selectedTexture.render(gRenderer, mouseClip.x - mdiff, mouseClip.y - mdiff, &otherInv->items[slotSelected].getClip());
		}
	}

	if (displaySpells)
	{
		displaySpellBook(texts);
	}

	if (displayEscape)
	{
		displayEscapeMenu();
	}
	else
	{
		exitGame.visible = false;
		resumeGame.visible = false;
	}

	//Icon sheet display
	iconBar.render(gRenderer, 168, SCREEN_HEIGHT - 60);

	//Health/Energy/EXP display
	statClip = { 0, 50, 400, 25 };
	statBar.render(gRenderer, (SCREEN_WIDTH / 2) - 200, SCREEN_HEIGHT - 50, &statClip);
	statBar.render(gRenderer, (SCREEN_WIDTH / 2) - 200, SCREEN_HEIGHT - 25, &statClip);

	statClip = { 0, 0, (int)((health/maxHealth) * 400), 25 };
	statBar.render(gRenderer, (SCREEN_WIDTH / 2) - 200, SCREEN_HEIGHT - 50, &statClip);
	
	statText.clear();
	statText.str(std::string());
	statText << std::fixed << std::setprecision(0) << "" << round(health);
	statText << "/" << maxHealth;
	statBarText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman20, 400);
	statBarText.render(gRenderer, (SCREEN_WIDTH / 2) - statBarText.getWidth() / 2, SCREEN_HEIGHT - statBarText.getHeight() - 25);
	healthword.render(gRenderer, (SCREEN_WIDTH / 2) - 190, SCREEN_HEIGHT - healthword.getHeight() - 30);

	statClip = { 0, 25, (int)((energy / maxEnergy) * 400), 25 };
	statBar.render(gRenderer, (SCREEN_WIDTH / 2) - 200, SCREEN_HEIGHT - 25, &statClip);
	
	statText.clear();
	statText.str(std::string());
	statText << std::fixed << std::setprecision(0) << "" << round(energy) << "/" << maxEnergy;
	statBarText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman20, 400);
	statBarText.render(gRenderer, (SCREEN_WIDTH / 2) - statBarText.getWidth() / 2, SCREEN_HEIGHT - statBarText.getHeight());
	energyword.render(gRenderer, (SCREEN_WIDTH / 2) - 190, SCREEN_HEIGHT - energyword.getHeight() - 5);

	int spacerY = 0;

	if (shields > 0)
	{
		spacerY = 25;
		statClip = { 0, 25, 400, 25 };
		shieldsbar.render(gRenderer, (SCREEN_WIDTH / 2) - 200, SCREEN_HEIGHT - 75, &statClip);
		statClip = { 0, 0, (int)((shields / shieldsmax) * 400), 25 };
		shieldsbar.render(gRenderer, (SCREEN_WIDTH / 2) - 200, SCREEN_HEIGHT - 75, &statClip);
		statText.clear();
		statText.str(std::string());
		statText << std::fixed << std::setprecision(0) << "" << round(shields) << "/" << round(shieldsmax);
		statBarText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman20, 400);
		statBarText.render(gRenderer, (SCREEN_WIDTH / 2) - statBarText.getWidth() / 2, SCREEN_HEIGHT - statBarText.getHeight() - 50);
		shieldword.render(gRenderer, (SCREEN_WIDTH / 2) - 190, SCREEN_HEIGHT - shieldword.getHeight() - 55);
	}

	statClip = { 0, 25, 400, 25 };
	expBar.render(gRenderer, 0, 0, &statClip);
	statClip = { 0, 0, (int)((exp / maxExp) * 400), 25 };
	expBar.render(gRenderer, 0, 0, &statClip);

	if (inCombat)
		combatText.render(gRenderer, (SCREEN_WIDTH / 2) + 165 - combatText.getWidth() / 2, SCREEN_HEIGHT - combatText.getHeight() - 30);
	else
		hpsText.render(gRenderer, (SCREEN_WIDTH / 2) + 165 - hpsText.getWidth() / 2, SCREEN_HEIGHT - hpsText.getHeight() - 30);
	epsText.render(gRenderer, (SCREEN_WIDTH / 2) + 165 - epsText.getWidth() / 2, SCREEN_HEIGHT - epsText.getHeight() - 5);

	if (skillPoints > 0)
		unspentBox.render(gRenderer, 490, SCREEN_HEIGHT - 125 - spacerY);
	else if (saveFile.spellPoints > 0)
		unspentSpells.render(gRenderer, 490, SCREEN_HEIGHT - 125 - spacerY);

	for (unsigned int i = 0; i < effects.size(); i++)
	{
		effectBoxCord = { (int)((SCREEN_WIDTH / 2) - 200 + (30 * i)), (int)(SCREEN_HEIGHT - 80 - spacerY), 30, 30 };
		if (checkCollision(mouseClip, effectBoxCord)) 
		{
			effectBoxHover.render(gRenderer, effectBoxCord.x, effectBoxCord.y);
			tooltip.setPosition(effectBoxCord.x, effectBoxCord.y - tooltip.getBox().h);
			tooltip.draw();
			statText.clear();
			statText.str(std::string());
			statText << std::fixed << std::setprecision(1) << effects[i]->getInfo();
			statBarText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, codeNewRoman18, tooltip.getBox().w - 20);
			statBarText.render(gRenderer, tooltip.getBox().x + 10, tooltip.getBox().y + 10);
		}
		else
			effectBox.render(gRenderer, effectBoxCord.x, effectBoxCord.y);
		if (!effects[i]->runtime)
			effects[i]->texture.render(gRenderer, effectBoxCord.x + 5, effectBoxCord.y + 5);
		abRe = { 0, 0, 20, (int)(((double)effects[i]->currentDuration / (double)effects[i]->totalDuration)*20.0) };
		abilityRefresh.render(gRenderer, (SCREEN_WIDTH / 2) - 195 + (30 * i), SCREEN_HEIGHT - 75 - spacerY, &abRe);
	}

	//Abilities
	for (int i = 0; i < 6; i++)
	{
		abilityIcon.render(gRenderer, 880 + (50 * i), SCREEN_HEIGHT - 50, &abRect[i]);
		if (i != 0 && findSpell(i-1))
		{
			globalSpells[spellIndex]->image.render(gRenderer, 880 + (50 * i) + 8, SCREEN_HEIGHT - 45);
		}
		else if (i == 0)
		{
			basicAttackIcon.render(gRenderer, 880 + (50 * i) + 8, SCREEN_HEIGHT - 45);
		}
	}
	abilityIcon.render(gRenderer, 1180, SCREEN_HEIGHT - 50, &abRect[6]);
	bool debounce = false;
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		if (inventory[i].itemType == ITEM_POTION && inventory[i].potion.healthAmount > 0)
		{
			itemSheet.render(gRenderer, 1185, SCREEN_HEIGHT - 45, &inventory[i].getClip());
			debounce = true;
			break;
		}
	}
	if (!debounce)
		emptyhp.render(gRenderer, 1185, SCREEN_HEIGHT - 45);
	abilityIconRaw.render(gRenderer, 1180, SCREEN_HEIGHT - 50, &abRect[6]);

	abilityIcon.render(gRenderer, 1230, SCREEN_HEIGHT - 50, &abRect[7]);
	debounce = false;
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		if (inventory[i].itemType == ITEM_POTION && inventory[i].potion.energyAmount > 0)
		{
			itemSheet.render(gRenderer, 1235, SCREEN_HEIGHT - 45, &inventory[i].getClip());
			debounce = true;
			break;
		}
	}
	if (!debounce)
		emptyep.render(gRenderer, 1235, SCREEN_HEIGHT - 45);
	abilityIconRaw.render(gRenderer, 1230, SCREEN_HEIGHT - 50, &abRect[7]);

	//Cooldowns
	for (int i = 0; i < 5; i++)
	{
		if (spellCooldown[i][0] > 0)
		{
			abRe = { 0, 0, 40, (int)(((double)spellCooldown[i][0] / (double)spellCooldown[i][1])*40.0) };
			abilityRefresh.render(gRenderer, 880 + (50 * i) + 55, SCREEN_HEIGHT - 45, &abRe);
			spellCooldown[i][0]--;
		}
	}

	if (reloadTime > 0)
	{
		abRe = { 0, 0, 40, 40-((int)(((double)reloadTime / (double)weapon.reload)*40.0)) };
		abilityRefresh.render(gRenderer, 885, SCREEN_HEIGHT - 45, &abRe);
	}

	//Target display
	if (targetEnemy != NULL)
	{
		if (targetEnemy->alive && round(targetEnemy->health) > 0)
		{
			statClip = { 0, 50, 400, 25 };
			enemyStatBar.render(gRenderer, (SCREEN_WIDTH / 2) - 100, 55, &statClip);
			statClip = { 0, 0, (int)((targetEnemy->health / targetEnemy->maxHealth) * 400), 25 };
			enemyStatBar.render(gRenderer, (SCREEN_WIDTH / 2) - 100, 55, &statClip);

			statText.clear();
			statText.str(std::string());
			statText << targetEnemy->name;
			SDL_Color ncolor = { 255, 255, 255 };
			if (targetEnemy->isBoss)
				ncolor = { 255, 143, 47 };
			statBarText.loadFromRenderedText(gRenderer, statText.str(), ncolor, CodeNewRoman20, 400);
			statBarText.render(gRenderer, (SCREEN_WIDTH / 2) - statBarText.getWidth() / 2, 16);

			statText.clear();
			statText.str(std::string());
			if (targetEnemy->aiType == ENEMY_TYPE_NODE)
				statText << "Resource";
			else
				statText << "Level: " << targetEnemy->level;
			statBarText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman16, 400);
			statBarText.render(gRenderer, (SCREEN_WIDTH / 2) - statBarText.getWidth() / 2, 36);

			statText.clear();
			statText.str(std::string());
			statText << std::fixed << std::setprecision(0) << round(targetEnemy->health) << "/" << targetEnemy->maxHealth;
			statBarText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman20, 400);
			statBarText.render(gRenderer, (SCREEN_WIDTH / 2) - statBarText.getWidth() / 2, 59);
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
	canPick = false;
	if (ekey)
		ekey = false;
}

void Player::displayInventory()
{
	if (displaySpells)
		closeSpells();

	if (displayEscape)
		closeEscapeMenu();

	if (reactor)
	{
		reactorSheet.render(gRenderer, oinvSheetClip.x - 50, oinvSheetClip.y);
		statBarText.loadFromRenderedText(gRenderer, "       Plasma Converter\nSelect an item from your inventory to turn into Plasma.\nCtrl-Click to insta-convert.", SDL_Color{ 255,255,255 }, CodeNewRoman16, 270);
		statBarText.render(gRenderer, oinvSheetClip.x + 18 - 45, oinvSheetClip.y + 15);
		inventoryBox.render(gRenderer, space * 5 + oxoffset - ((5 / oxslots) * space * oxslots), space * (5 / oxslots) + oyoffset + 10);
		if (reactorSlot != -1)
		{
			inventory[reactorSlot].setPosition(5 + space * 5 + oxoffset - ((5 / oxslots) * space * oxslots), 5 + space * (5 / oxslots) + oyoffset + 10);
			inventory[reactorSlot].draw();
			statBarText.loadFromRenderedText(gRenderer, "Name: \nPlasma Value: ", SDL_Color{ 255,255,255 }, CodeNewRoman16, 230);
			statBarText.render(gRenderer, oinvSheetClip.x + 18 - 45, oinvSheetClip.y + 140);
			int psy = (std::to_string((int)inventory[reactorSlot].value).length() * 9) - 9;
			plasmaIcon.render(gRenderer, 1096 + psy, 188);
			inventory[reactorSlot].tooltipName.render(gRenderer, oinvSheetClip.x + 68 - 45, oinvSheetClip.y + 140);
			statBarText.loadFromRenderedText(gRenderer, std::to_string(inventory[reactorSlot].value), SDL_Color{ 255,255,255 }, CodeNewRoman16, 230);
			statBarText.render(gRenderer, oinvSheetClip.x + 140 - 45, oinvSheetClip.y + 157);
			reactorConvert.draw();
		}
		else
			reactorConvert.visible = false;
	}
	else
		reactorConvert.visible = false;

	inventorypure.render(gRenderer, invSheetClip.x, invSheetClip.y);
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		if ((slotSelected == i && slotType == SLOT_INVENTORY) || (reactor && reactorSlot == i))
			inventoryBoxClicked.render(gRenderer, space * i + xoffset - ((i / xslots) * space * xslots), space * (i / xslots) + yoffset);
		else if (inventory[i].hovered == true)
			inventoryBoxHovered.render(gRenderer, space * i + xoffset - ((i / xslots) * space * xslots), space * (i / xslots) + yoffset);
		else
			inventoryBox.render(gRenderer, space * i + xoffset - ((i / xslots) * space * xslots), space * (i / xslots) + yoffset);

		inventory[i].setPosition(5 + space * i + xoffset - ((i / xslots) * space * xslots), 5 + space * (i / xslots) + yoffset);
		if ((slotType != SLOT_INVENTORY || (slotSelected != i || (itemSelected == false && slotSelected == i))) && inventory[i].itemType != ITEM_NONE)
			inventory[i].draw();
	}

	equipment[0].setPosition(385, 50);
	equipment[1].setPosition(440, 50);
	equipment[2].setPosition(440, 105);
	equipment[3].setPosition(440, 160);

	for (int i = 0; i < MAX_EQUIPMENT; i++)
	{
		if (slotSelected == i && slotType == SLOT_EQUIPMENT)
			inventoryBoxClicked.render(gRenderer, equipment[i].getBox().x - 5, equipment[i].getBox().y - 5);
		else if (equipment[i].hovered == true && equipment[i].itemType != ITEM_NONE)
			inventoryBoxHovered.render(gRenderer, equipment[i].getBox().x - 5, equipment[i].getBox().y - 5);
		else if (equipment[i].hovered == true && equipment[i].itemType == ITEM_NONE)
			equipmentEmptyHover.render(gRenderer, equipment[i].getBox().x - 5, equipment[i].getBox().y - 5, &eqRect[i]);
		else if (equipment[i].hovered == false && equipment[i].itemType == ITEM_NONE)
			equipmentEmpty.render(gRenderer, equipment[i].getBox().x - 5, equipment[i].getBox().y - 5, &eqRect[i]);
		else
			inventoryBox.render(gRenderer, equipment[i].getBox().x - 5, equipment[i].getBox().y - 5);

		if ((slotType != SLOT_EQUIPMENT || (slotSelected != i || (itemSelected == false && slotSelected == i))) && equipment[i].itemType != ITEM_NONE)
			equipment[i].draw();
	}
	infoText.render(gRenderer, 260, 97);
	levelText.render(gRenderer, 50 + (185.0 / 2.0) - (double)levelText.getWidth() / 2.0, 45);
	expText.render(gRenderer, 50 + (185.0 / 2.0) - (double)expText.getWidth() / 2.0, 60);
	sexpbarcord = { 0, 0, 150, 8 };
	sexpbar.render(gRenderer, 66, 74, &sexpbarcord);
	sexpbarcord = { 0, 8, (int)((exp / maxExp) * 150), 8 };
	sexpbar.render(gRenderer, 66, 74, &sexpbarcord);

	if (skillPoints > 0)
	{
		incSTR.draw();
		incVIT.draw();
		incDEX.draw();
	}
	else
	{
		incSTR.visible = false;
		incVIT.visible = false;
		incDEX.visible = false;
	}

	statsText.render(gRenderer, 52, 105);
	armorIcon.render(gRenderer, 162 + armorSpace, 174);
	plasmaIcon.render(gRenderer, 162 + plasmaSpace, 191);
	strengthIcon.render(gRenderer, 162 + strengthSpace, 123);
	vitalityIcon.render(gRenderer, 162 + vitalitySpace, 140);
	dexterityIcon.render(gRenderer, 162 + dexteritySpace, 157);

	if (infoPage == "Weapon")
	{
		armorInfo.draw();
		weaponInfo.visible = false;
	}
	else if (infoPage == "Armor")
	{
		weaponInfo.draw();
		armorInfo.visible = false;
	}

	if (otherInv != NULL)
	{
		oinventorySheet.render(gRenderer, oinvSheetClip.x, oinvSheetClip.y);
		for (int i = 0; i < MAX_STORAGE; i++)
		{
			if (slotSelected == i && slotType == SLOT_STORAGE)
				inventoryBoxClicked.render(gRenderer, space * i + oxoffset - ((i / oxslots) * space * oxslots), space * (i / oxslots) + oyoffset);
			else if (otherInv->items[i].hovered == true)
				inventoryBoxHovered.render(gRenderer, space * i + oxoffset - ((i / oxslots) * space * oxslots), space * (i / oxslots) + oyoffset);
			else
				inventoryBox.render(gRenderer, space * i + oxoffset - ((i / oxslots) * space * oxslots), space * (i / oxslots) + oyoffset);

			otherInv->items[i].setPosition(5 + space * i + oxoffset - ((i / oxslots) * space * oxslots), 5 + space * (i / oxslots) + oyoffset);
			if ((slotType != SLOT_STORAGE || (slotSelected != i || (itemSelected == false && slotSelected == i))) && otherInv->items[i].itemType != ITEM_NONE)
				otherInv->items[i].draw();
		}
	}

	//Set tooltip
	int spx = 5;
	if (itemSelected == false)
	{
		for (int i = 0; i < MAX_INVENTORY; i++)
		{
			if (inventory[i].itemType != ITEM_NONE && checkCollision(mouseClip, inventory[i].getBox()))
			{
				tooltip.setSize(inventory[i].tooltipWidth, inventory[i].tooltipSize);
				tooltip.setPosition(spx + inventory[i].getBox().x + inventory[i].getBox().w,-spx + inventory[i].getBox().y);
				tooltip.draw();
				inventory[i].tooltipName.render(gRenderer, tooltip.getBox().x + ((double)tooltip.getBox().w / 2.0) - (double)inventory[i].tooltipName.getWidth() / 2.0, tooltip.getBox().y + 10);
				inventory[i].tooltipText.render(gRenderer, tooltip.getBox().x + 10, tooltip.getBox().y + 10);
				inventory[i].plasmaIcon.render(gRenderer, tooltip.getBox().x + 10 + inventory[i].plasmaSpace + 2, tooltip.getBox().y + inventory[i].pYSpace);
				if (inventory[i].isarmor)
					inventory[i].armorIcon.render(gRenderer, tooltip.getBox().x + 10 + inventory[i].armorSpace + 2, tooltip.getBox().y + 45);
				return;
			}
		}
		for (int i = 0; i < MAX_EQUIPMENT; i++)
		{
			if (equipment[i].itemType != ITEM_NONE && checkCollision(mouseClip, equipment[i].getBox()))
			{
				tooltip.setSize(equipment[i].tooltipWidth, equipment[i].tooltipSize);
				tooltip.setPosition(spx + equipment[i].getBox().x + equipment[i].getBox().w,-spx + equipment[i].getBox().y);
				tooltip.draw();
				equipment[i].tooltipName.render(gRenderer, tooltip.getBox().x + ((double)tooltip.getBox().w / 2.0) - (double)equipment[i].tooltipName.getWidth() / 2.0, tooltip.getBox().y + 10);
				equipment[i].tooltipText.render(gRenderer, tooltip.getBox().x + 10, tooltip.getBox().y + 10);
				equipment[i].plasmaIcon.render(gRenderer, tooltip.getBox().x + 10 + equipment[i].plasmaSpace + 2, tooltip.getBox().y + equipment[i].pYSpace);
				if (equipment[i].isarmor)
					equipment[i].armorIcon.render(gRenderer, tooltip.getBox().x + 10 + equipment[i].armorSpace + 2, tooltip.getBox().y + 45);
				return;
			}
		}
		if (otherInv != NULL)
		{
			for (int i = 0; i < MAX_STORAGE; i++)
			{
				if (otherInv->items[i].itemType != ITEM_NONE && checkCollision(mouseClip, otherInv->items[i].getBox()))
				{
					tooltip.setSize(otherInv->items[i].tooltipWidth, otherInv->items[i].tooltipSize);
					tooltip.setPosition(-spx + otherInv->items[i].getBox().x - tooltip.getSize().first,-spx + otherInv->items[i].getBox().y);
					tooltip.draw();
					otherInv->items[i].tooltipName.render(gRenderer, tooltip.getBox().x + ((double)tooltip.getBox().w / 2.0) - (double)otherInv->items[i].tooltipName.getWidth() / 2.0, tooltip.getBox().y + 10);
					otherInv->items[i].tooltipText.render(gRenderer, tooltip.getBox().x + 10, tooltip.getBox().y + 10);
					otherInv->items[i].plasmaIcon.render(gRenderer, tooltip.getBox().x + 10 + otherInv->items[i].plasmaSpace + 2, tooltip.getBox().y + otherInv->items[i].pYSpace);
					if (otherInv->items[i].isarmor)
						otherInv->items[i].armorIcon.render(gRenderer, tooltip.getBox().x + 10 + otherInv->items[i].armorSpace + 2, tooltip.getBox().y + 45);
					return;
				}
			}
		}
	}
}

void Player::checkStats()
{
	maxHealth = BASE_HEALTH + (vitality * VIT_BONUS);
	maxEnergy = BASE_ENERGY + (dexterity * DEX_BONUS);
}

void Player::checkLevel()
{
	//Level ups
	if (exp >= maxExp)
	{
		levelRefresh = true;
		skillPoints += 5;
		saveFile.spellPoints += 1;
		level += 1;
		checkStats();
		health = maxHealth;
		energy = maxEnergy;
		exp -= maxExp;
		maxExp = floor(level + (135 * pow(2, (double)level / 7)));
		rebuildInventoryText();
		rebuildAttackText();
	}
}

void Player::buildLocalImages()
{
	inventoryBox.loadFromFile("bin/images/iconbox.png", Entity::gRenderer);
	inventoryBox.setScale((double)ITEM_BOX_SIZE / 120.0, (double)ITEM_BOX_SIZE / 120.0);

	inventoryBoxHovered.loadFromFile("bin/images/iconboxhover.png", Entity::gRenderer);
	inventoryBoxHovered.setScale((double)ITEM_BOX_SIZE / 120.0, (double)ITEM_BOX_SIZE / 120.0);

	inventoryBoxClicked.loadFromFile("bin/images/iconboxclick.png", Entity::gRenderer);
	inventoryBoxClicked.setScale((double)ITEM_BOX_SIZE / 120.0, (double)ITEM_BOX_SIZE / 120.0);

	effectBox.loadFromFile("bin/images/iconbox.png", Entity::gRenderer);
	effectBox.setScale(30.0 / 120.0, 30.0 / 120.0);
	effectBoxHover.loadFromFile("bin/images/iconboxhover.png", Entity::gRenderer);
	effectBoxHover.setScale(30.0 / 120.0, 30.0 / 120.0);

	equipmentEmpty.loadFromFile("bin/images/eqempty.png", Entity::gRenderer);
	equipmentEmpty.setScale((double)ITEM_BOX_SIZE / 120.0, (double)ITEM_BOX_SIZE / 120.0);

	equipmentEmptyHover.loadFromFile("bin/images/eqemptyhover.png", Entity::gRenderer);
	equipmentEmptyHover.setScale((double)ITEM_BOX_SIZE / 120.0, (double)ITEM_BOX_SIZE / 120.0);

	eqRect[EQUIPMENT_WEAPON] = { 0, 0, 120, 120 };
	eqRect[EQUIPMENT_HEAD] = { 120, 0, 120, 120 };
	eqRect[EQUIPMENT_CHEST] = { 0, 120, 120, 120 };
	eqRect[EQUIPMENT_LEGS] = { 120, 120, 120, 120 };

	abilityIcon.loadFromFile("bin/images/abilityicon.png", Entity::gRenderer);
	abilityIconRaw.loadFromFile("bin/images/abilityiconraw.png", Entity::gRenderer);

	unspentBox.loadFromFile("bin/images/unspent.png", Entity::gRenderer);
	unspentSpells.loadFromFile("bin/images/unspentspell.png", Entity::gRenderer);

	bubble.setRender(gRenderer, gWindow);
	bubble.camera = camera;
	bubble.loadSpriteImage("bin/images/bubble.png")
		.setFrameSize(38, 30)
		.setSize(38, 30)
		.setDelay(3)
		.setCenter(PLAYER_SIZE / 2.0 - 1, PLAYER_SIZE / 2.0 - 9)
		.pushFrameRow("Bubble", 0, 0, 38, 0, 4)
		.setAnimation("Bubble");

	vitalityIcon.loadFromFile("bin/images/vitality.png", Entity::gRenderer);
	dexterityIcon.loadFromFile("bin/images/dexterity.png", Entity::gRenderer);
	strengthIcon.loadFromFile("bin/images/strength.png", Entity::gRenderer);

	emptyhp.loadFromFile("bin/images/emptyhpotion.png", Entity::gRenderer);
	emptyep.loadFromFile("bin/images/emptyepotion.png", Entity::gRenderer);
	itemSheet.loadFromFile("bin/images/itemsheet.png", Entity::gRenderer);

	abRect[0] = { 0, 0, 50, 50 };
	abRect[1] = { 50, 0, 50, 50 };
	abRect[2] = { 0, 50, 50, 50 };
	abRect[3] = { 50, 50, 50, 50 };
	abRect[4] = { 0, 100, 50, 50 };
	abRect[5] = { 50, 100, 50, 50 };
	abRect[6] = { 0, 150, 50, 50 };
	abRect[7] = { 50, 150, 50, 50 };

	abilityRefresh.loadFromFile("bin/images/abilityRefresh.png", Entity::gRenderer);

	inventorySheet.loadFromFile("bin/images/inventory.png", Entity::gRenderer);
	inventorySheet.setScale(495.0 / 570.0, 495.0 / 795.0);
	invSheetClip = { 20, 20, 495, 495 };

	inventorypure.loadFromFile("bin/images/inventorypure.png", Entity::gRenderer);

	escapeMenu.loadFromFile("bin/images/inventory.png", Entity::gRenderer);
	escapeMenu.setScale(200 / 570.0, 300 / 795.0);

	abilityBox.loadFromFile("bin/images/abilitybox.png", Entity::gRenderer);

	spellBG.loadFromFile("bin/images/inventory.png", Entity::gRenderer);
	spellBG.setScale(325.0 / 570.0, 495.0 / 795.0);

	iconBar.loadFromFile("bin/images/iconbar.png", Entity::gRenderer);

	sbSheet.loadFromFile("bin/images/inventory.png", Entity::gRenderer);
	sbSheet.setScale(289.0 / 570.0, 400.0 / 795.0);

	sbIn.loadFromFile("bin/images/tabin.png", Entity::gRenderer);
	sbOut.loadFromFile("bin/images/tabout.png", Entity::gRenderer);

	sbIn2.loadFromFile("bin/images/tabin2.png", Entity::gRenderer);
	sbOut2.loadFromFile("bin/images/tabout2.png", Entity::gRenderer);

	sbInH.loadFromFile("bin/images/tabinhover.png", Entity::gRenderer);
	sbIn2H.loadFromFile("bin/images/tabin2hover.png", Entity::gRenderer);

	oinventorySheet.loadFromFile("bin/images/inventory.png", Entity::gRenderer);
	oinventorySheet.setScale(255 / 570.0, 310.0 / 795.0);
	oinvSheetClip = { 990, 30, 255, 310 };

	reactorSheet.loadFromFile("bin/images/inventory.png", Entity::gRenderer);
	reactorSheet.setScale(310 / 570.0, 235 / 795.0);

	statBar.loadFromFile("bin/images/health.png", Entity::gRenderer);
	shieldsbar.loadFromFile("bin/images/shields.png", Entity::gRenderer);

	expBar.loadFromFile("bin/images/exp.png", Entity::gRenderer);
	expBar.setScale(SCREEN_WIDTH / 400.0, 15.0 / 50.0);

	basicAttackIcon.loadFromFile("bin/images/abilities/basicattack.png", gRenderer);
	
	sexpbar.loadFromFile("bin/images/smallexpbar.png", gRenderer);

	armorIcon.loadFromFile("bin/images/armoricon.png", gRenderer);
	plasmaIcon.loadFromFile("bin/images/plasmaicon.png", gRenderer);

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
	incSTR.buildButton(BUTTON_PLUS, SDL_Rect{ 210, 121, 15, 15 });

	incVIT.setRender(gRenderer, gWindow);
	incVIT.buildButton(BUTTON_PLUS, SDL_Rect{ 210, 138, 15, 15 });

	incDEX.setRender(gRenderer, gWindow);
	incDEX.buildButton(BUTTON_PLUS, SDL_Rect{ 210, 155, 15, 15 });

	incDES.setRender(gRenderer, gWindow);
	incDES.buildButton(BUTTON_PLUS, SDL_Rect{ 1207, 56, 15, 15 });

	incRES.setRender(gRenderer, gWindow);
	incRES.buildButton(BUTTON_PLUS, SDL_Rect{ 1207, 74, 15, 15 });

	desIcon.loadFromFile("bin/images/destruction.png", Entity::gRenderer);
	resIcon.loadFromFile("bin/images/restoration.png", Entity::gRenderer);

	reactorConvert.setRender(gRenderer, gWindow);
	reactorConvert.buildButton(BUTTON_WIDE, SDL_Rect{ 1040, 215, 110, 30 }, "Convert");

	exitGame.setRender(gRenderer, gWindow);
	exitGame.buildButton(BUTTON_WIDE, SDL_Rect{ 40, 260, 160, 35 }, "Exit Game");

	resumeGame.setRender(gRenderer, gWindow);
	resumeGame.buildButton(BUTTON_WIDE, SDL_Rect{ 40, 40, 160, 35 }, "Return");

	armorInfo.setRender(gRenderer, gWindow);
	armorInfo.buildButton(BUTTON_WIDE, SDL_Rect{ 260, 53, 115, 35 }, "Armor Info", 16);

	weaponInfo.setRender(gRenderer, gWindow);
	weaponInfo.buildButton(BUTTON_WIDE, SDL_Rect{ 260, 53, 115, 35 }, "Weapon Info", 16);

	int psize = 12, bsize = 15;
	clearSlots[0].setRender(gRenderer, gWindow);
	clearSlots[0].buildButton(BUTTON_WIDE, SDL_Rect{ 460, 51, bsize, bsize }, "X", psize);
	clearSlots[1].setRender(gRenderer, gWindow);
	clearSlots[1].buildButton(BUTTON_WIDE, SDL_Rect{ 240, 201, bsize, bsize }, "X", psize);
	clearSlots[2].setRender(gRenderer, gWindow);
	clearSlots[2].buildButton(BUTTON_WIDE, SDL_Rect{ 460, 201, bsize, bsize }, "X", psize);
	clearSlots[3].setRender(gRenderer, gWindow);
	clearSlots[3].buildButton(BUTTON_WIDE, SDL_Rect{ 240, 351, bsize, bsize }, "X", psize);
	clearSlots[4].setRender(gRenderer, gWindow);
	clearSlots[4].buildButton(BUTTON_WIDE, SDL_Rect{ 460, 351, bsize, bsize }, "X", psize);

	CodeNewRomanBig = TTF_OpenFont("bin/fonts/codenewroman.ttf", 32);
	CodeNewRoman = TTF_OpenFont("bin/fonts/codenewroman.ttf", 24);
	CodeNewRoman20 = TTF_OpenFont("bin/fonts/codenewroman.ttf", 20);
	CodeNewRoman16 = TTF_OpenFont("bin/fonts/codenewroman.ttf", 16);
	codeNewRoman18 = TTF_OpenFont("bin/fonts/codenewroman.ttf", 18);
	CodeNewRoman15 = TTF_OpenFont("bin/fonts/codenewroman.ttf", 15);
	CodeNewRoman12 = TTF_OpenFont("bin/fonts/codenewroman.ttf", 12);

	attackText.loadFromRenderedText(gRenderer, "Basic Attack\nCooldown: 0s\nEnergy: 0", SDL_Color{ 255,255,255 }, CodeNewRoman15, 200);
	desText.loadFromRenderedText(gRenderer, "Destruction\n  Spells", SDL_Color{ 255,255,255 }, CodeNewRoman15, 200);
	defText.loadFromRenderedText(gRenderer, "Protection", SDL_Color{ 255,255,255 }, CodeNewRoman12, 200);
	healthword.loadFromRenderedText(gRenderer, "Health:", SDL_Color{ 255,255,255 }, CodeNewRoman12, 200);
	energyword.loadFromRenderedText(gRenderer, "Energy:", SDL_Color{ 255,255,255 }, CodeNewRoman12, 200);
	shieldword.loadFromRenderedText(gRenderer, "Shield:", SDL_Color{ 255,255,255 }, CodeNewRoman12, 200);
	combatText.loadFromRenderedText(gRenderer, "In Combat", SDL_Color{ 255,255,255 }, CodeNewRoman12, 200);
}

void Player::setCamera(SDL_Rect & camera)
{
	camera.x = (Sprite::getBox().x + (Sprite::getBox().w / 2) - (SCREEN_WIDTH / 2));
	camera.y = (Sprite::getBox().y + (Sprite::getBox().h / 2) - (SCREEN_HEIGHT / 2));

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
	{
		armorRating += equipment[EQUIPMENT_HEAD].armor.armorRating;
		HelmetColor = equipment[EQUIPMENT_HEAD].armor.color;
	}
	else
		HelmetColor = { 255, 255, 255 };


	if (equipment[EQUIPMENT_CHEST].itemType == ITEM_CHEST)
	{
		armorRating += equipment[EQUIPMENT_CHEST].armor.armorRating;
		ChestColor = equipment[EQUIPMENT_CHEST].armor.color;
	}
	else
		ChestColor = { 255, 255, 255 };

	if (equipment[EQUIPMENT_LEGS].itemType == ITEM_LEGS)
	{
		armorRating += equipment[EQUIPMENT_LEGS].armor.armorRating;
		LegsColor = equipment[EQUIPMENT_LEGS].armor.color;
	}
	else
		LegsColor = { 255, 255, 255 };

	rebuildInventoryText();
}

void Player::quickHeal(std::vector<FloatingText>&texts)
{
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		if (inventory[i].itemType == ITEM_POTION && inventory[i].potion.healthAmount > 0)
		{
			if (health != maxHealth)
			{
				int potentialheal = inventory[i].potion.healthAmount;
				health += inventory[i].potion.healthAmount;
				if (health > maxHealth)
				{
					potentialheal -= health - maxHealth;
					health = maxHealth;
				}
				if (potentialheal > 0)
					texts.push_back(FloatingText("+" + std::to_string(potentialheal), Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman20, SDL_Color{ 0,255,0 }));
				inventory[i].clearSlot();
				hoverRefresh = true;
			}
			else
			{
				texts.push_back(FloatingText("I'm already healed!", Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman16, SDL_Color{ 255,0,0 }));
			}
			return;
		}
	}
}

void Player::quickEnergy(std::vector<FloatingText>&texts)
{
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		if (inventory[i].itemType == ITEM_POTION && inventory[i].potion.energyAmount > 0)
		{
			if (energy != maxEnergy)
			{
				int potentialenergy = inventory[i].potion.energyAmount;
				energy += inventory[i].potion.energyAmount;
				if (energy > maxEnergy)
				{
					potentialenergy -= energy - maxEnergy;
					energy = maxEnergy;
				}
				if (potentialenergy > 0)
					texts.push_back(FloatingText("+" + std::to_string(potentialenergy), Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman20, SDL_Color{ 255,205,0 }));
				inventory[i].clearSlot();
				hoverRefresh = true;
			}
			else
			{
				texts.push_back(FloatingText("I'm already healed!", Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman16, SDL_Color{ 255,0,0 }));
			}
			return;
		}
	}
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

void Player::closeInventory()
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
	if (otherInv != NULL)
	{
		for (int i = 0; i < MAX_STORAGE; i++)
			otherInv->items[i].hovered = false;
		otherInv = NULL;
	}
	displayInv = false;
	reactor = false;
	reactorSlot = -1;
	rebuildInventoryText();
}

void Player::resetPlayer()
{
	health = maxHealth;
	energy = maxEnergy;
	shields = 0;
	armorRatingBonus = 0;
	inCombat = false;
	velocityBonus = 0;
	effects.clear();
	resetCooldowns();
	rebuildInventoryText();
	deleteAllType(ITEM_KEY);
}

void Player::resetCooldowns()
{
	for (int i = 0; i < 5; i++)
	{
		spellCooldown[i][0] = 0;
	}
}

bool Player::findSpell(int x)
{
	for (unsigned int i = 0; i < globalSpells.size(); i++)
	{
		if (spellBook[x] == globalSpells[i]->name)
		{
			spellIndex = i;
			return true;
		}
	}
	return false;
}

void Player::castSpell(std::vector<FloatingText> &texts, int ind)
{
	if (!msgbox && !displayInv && !displaySpells && !displayEscape && findSpell(ind))
	{
		if (offCooldown(texts, ind))
		{
			if (globalSpells[spellIndex]->canCast())
			{
				spells->push_back(globalSpells[spellIndex]);
				spellCooldown[ind][0] = globalSpells[spellIndex]->cooldown;
				spellCooldown[ind][1] = globalSpells[spellIndex]->cooldown;
				//Enable for Global Cooldown (GCD)
				/*
				for (int i = 0; i < 5; i++)
				{
					if (i != ind && spellBook[i] != "Empty" && spellCooldown[i][0] < 1 * SCREEN_FPS)
					{
						//30 = 0.5 seconds
						spellCooldown[i][0] = 30;
						spellCooldown[i][1] = 30;
					}
				}
				*/
			}
		}
	}
}

bool Player::offCooldown(std::vector<FloatingText> &texts, int x)
{
	if (spellCooldown[x][0] == 0)
		return true;
	else
	{
		texts.push_back(FloatingText("I Can't Do That!", Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman16, SDL_Color{ 255,0,0 }));
		if (clickedSpell)
		{
			clickedSpell = false;
			selectedSpell = -1;
			clickingSpell = false;
			rebuildAttackText();
		}
	}
	return false;
}

void encryptDecrypt(char x[24])
{
	char key[3] = { 'K', 'Z', 'J' };
	std::string enc = x;
	for (int i = 0; i < 24; i++)
		x[i] = enc[i] ^ key[i % (sizeof(key) / sizeof(char))];
}

void Player::saveGame(int file)
{
	saveFile.str = strength;
	saveFile.dex = dexterity;
	saveFile.vit = vitality;
	saveFile.energy = energy;
	saveFile.maxEnergy = maxEnergy;
	saveFile.exp = exp;
	saveFile.maxExp = maxExp;
	saveFile.health = health;
	saveFile.maxHealth = maxHealth;
	saveFile.level = level;
	saveFile.plasma = plasma;
	saveFile.skillPoints = skillPoints;

	for (int i = 0; i < 5; i++)
	{
		memset(saveFile.spellBook[i], 0, sizeof saveFile.spellBook[i]);
		strcpy_s(saveFile.spellBook[i], spellBook[i].c_str());
		encryptDecrypt(saveFile.spellBook[i]);
	}

	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		memset(saveFile.inv[i], 0, sizeof saveFile.inv[i]);
		strcpy_s(saveFile.inv[i], inventory[i].name.c_str());
		encryptDecrypt(saveFile.inv[i]);
	}

	for (int i = 0; i < MAX_EQUIPMENT; i++)
	{
		memset(saveFile.equip[i], 0, sizeof saveFile.equip[i]);
		strcpy_s(saveFile.equip[i], equipment[i].name.c_str());
		encryptDecrypt(saveFile.equip[i]);
	}

	for (int v = 0; v < 6; v++)
	{
		saveFile.storageopen[v] = storage[v].open;
		for (int i = 0; i < MAX_STORAGE; i++)
		{
			memset(saveFile.storage[v][i], 0, sizeof saveFile.storage[v][i]);
			strcpy_s(saveFile.storage[v][i], storage[v].items[i].name.c_str());
			encryptDecrypt(saveFile.storage[v][i]);
		}
	}
	
	std::fstream RAF;
	std::string path = "bin/saves/save" + std::to_string(file) + ".dat";
	RAF.open(path.c_str(), std::ios::out | std::ios::binary);
	RAF.write(reinterpret_cast<const char*>(&saveFile), sizeof(PlayerStorage));
	RAF.close();
	std::cout << "Saved game!\n";
	rebuildInventoryText();
}

void Player::loadGame(int file, std::vector<Item> global)
{
	std::fstream RAF;
	std::string path = "bin/saves/save" + std::to_string(file) + ".dat";
	RAF.open(path.c_str(), std::ios::in | std::ios::binary);
	if (!RAF.is_open())
		return;
	RAF.seekg(0); // read from start
	RAF.read(reinterpret_cast<char *>(&saveFile), sizeof(PlayerStorage));
	RAF.close();

	strength = saveFile.str;
	dexterity = saveFile.dex;
	vitality = saveFile.vit;
	energy = saveFile.energy;
	maxEnergy = saveFile.maxEnergy;
	exp = saveFile.exp;
	maxExp = saveFile.maxExp;
	health = saveFile.health;
	maxHealth = saveFile.maxHealth;
	level = saveFile.level;
	plasma = saveFile.plasma;
	skillPoints = saveFile.skillPoints;

	for (int i = 0; i < 5; i++)
	{
		encryptDecrypt(saveFile.spellBook[i]);
		spellBook[i] = saveFile.spellBook[i];
	}

	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		encryptDecrypt(saveFile.inv[i]);
		std::string want = saveFile.inv[i];
		for (unsigned int v = 0; v < global.size(); v++)
		{
			if (want == global[v].name)
			{
				inventory[i] = global[v];
				inventory[i].delCamera();
				inventory[i].setScale((double)ITEM_INV_SIZE / (double)inventory[i].getClip().w, (double)ITEM_INV_SIZE / (double)inventory[i].getClip().h);
				inventory[i].setBox(ITEM_INV_SIZE, ITEM_INV_SIZE);
				inventory[i].alive = false;
				break;
			}
		}
	}

	for (int i = 0; i < MAX_EQUIPMENT; i++)
	{
		encryptDecrypt(saveFile.equip[i]);
		std::string want = saveFile.equip[i];
		for (unsigned int v = 0; v < global.size(); v++)
		{
			if (want == global[v].name)
			{
				equipment[i] = global[v];
				equipment[i].delCamera();
				equipment[i].setScale((double)ITEM_INV_SIZE / (double)equipment[i].getClip().w, (double)ITEM_INV_SIZE / (double)equipment[i].getClip().h);
				equipment[i].setBox(ITEM_INV_SIZE, ITEM_INV_SIZE);
				equipment[i].alive = false;
				break;
			}
		}
	}

	for (int z = 0; z < 6; z++)
	{
		storage[z].open = saveFile.storageopen[z];
		for (int i = 0; i < MAX_STORAGE; i++)
		{
			encryptDecrypt(saveFile.storage[z][i]);
			std::string want = saveFile.storage[z][i];
			for (unsigned int v = 0; v < global.size(); v++)
			{
				if (want == global[v].name)
				{
					storage[z].items[i] = global[v];
					storage[z].items[i].delCamera();
					storage[z].items[i].setScale((double)ITEM_INV_SIZE / (double)storage[z].items[i].getClip().w, (double)ITEM_INV_SIZE / (double)storage[z].items[i].getClip().h);
					storage[z].items[i].setBox(ITEM_INV_SIZE, ITEM_INV_SIZE);
					storage[z].items[i].alive = false;
					break;
				}
			}
		}
	}

	setEquipment();
	std::cout << "Loaded game!\n";
	rebuildInventoryText();
}

bool Player::checkForSpell(std::string val)
{
	if (val == "Empty")
		return false;
	for (int i = 0; i < 5; i++)
	{
		if (spellBook[i] == val)
			return true;
	}
	return false;
}

void Player::displaySpellBook(std::vector<FloatingText> &texts)
{
	if (displayInv)
		closeInventory();

	if (displayEscape)
		closeEscapeMenu();

	yspace = 0;
	xspace = 0;
	inventorySheet.render(gRenderer, invSheetClip.x, invSheetClip.y);

	for (int i = 0; i < 6; i++)
	{
		//Display holding box
		if (i % 2 == 0 && i != 0)
		{
			yspace += 150;
			xspace = 0;
		}
		abilityBox.render(gRenderer, invSheetClip.x + xspace + 28, invSheetClip.y + yspace + 23);

		//Display clear buttons
		if (i > 0)
		{
			if (spellBook[i - 1] != "Empty")
				clearSlots[i - 1].draw();
			else
				clearSlots[i - 1].visible = false;
		}

		//Display the rest
		SDL_Rect ais{ invSheetClip.x + xspace + 50, invSheetClip.y + yspace + 40, 50, 50 };
		if (!mouseLeft && aicc[i] && i != 0 && offCooldown(texts, i-1))
		{
			if (clickedSpell)
			{
				bool deb = false;
				for (int v = 0; v < 5 && !deb; v++)
				{
					for (unsigned int sp = 1; sp < globalSpells.size(); sp++)
					{
						if (aicc[i] && globalSpells[sp]->name == spellBook[i-1] && globalSpells[sp]->isActive())
						{
							deb = true;
							clickedSpell = false;
							selectedSpell = -1;
							clickingSpell = false;
							rebuildAttackText();
							break;
						}
					}
					if (!deb && spellBook[v] == globalSpells[selectedSpell]->name)
					{
						if (spellCooldown[v][0] != 0)
						{
							deb = true;
							clickedSpell = false;
							selectedSpell = -1;
							clickingSpell = false;
							rebuildAttackText();
							texts.push_back(FloatingText("I Can't Do That!", Sprite::getBox().x + PLAYER_SIZE / 2, Sprite::getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman16, SDL_Color{ 255,0,0 }));
							break;
						}
						spellBook[v] = spellBook[i - 1];
					}
				}
				if (!deb)
				{
					spellBook[i - 1] = globalSpells[selectedSpell]->name;
					clickedSpell = false;
					selectedSpell = -1;
					clickingSpell = false;
					rebuildAttackText();
				}
			}
			else if (!clickedSpell)
			{
				if (spellBook[i - 1] != "Empty")
				{
					for (int v = 0; v < 5; v++)
					{
						if (spellBook[v] == spellBook[i - 1] && findSpell(v))
						{
							selectedSpell = spellIndex;
							clickedSpell = true;
							clickingSpell = false;
							break;
						}
					}
					//spellBook[i - 1] = "Empty";
					rebuildAttackText();
				}
			}
		}

		if ((checkCollision(mouseClip, ais) && mouseLeft))
		{
			aicc[i] = 1;
			if (i > 0 && (clickedSpell || spellBook[i - 1] != "Empty"))
				inventoryBoxClicked.render(gRenderer, invSheetClip.x + xspace + 50, invSheetClip.y + yspace + 40);
			else
				inventoryBox.render(gRenderer, invSheetClip.x + xspace + 50, invSheetClip.y + yspace + 40);
		}
		else if (checkCollision(mouseClip, ais) || (i > 0 && spellCooldown[i-1][0] > 0))
		{
			aicc[i] = 0;
			if (i > 0 && (clickedSpell || spellBook[i-1] != "Empty"))
				inventoryBoxHovered.render(gRenderer, invSheetClip.x + xspace + 50, invSheetClip.y + yspace + 40);
			else
				inventoryBox.render(gRenderer, invSheetClip.x + xspace + 50, invSheetClip.y + yspace + 40);
		}
		else
		{
			aicc[i] = 0;
			inventoryBox.render(gRenderer, invSheetClip.x + xspace + 50, invSheetClip.y + yspace + 40);
		}

		abilityIconRaw.render(gRenderer, invSheetClip.x + xspace + 50, invSheetClip.y + yspace + 40, &abRect[i]);
		if (i != 0 && findSpell(i-1))
		{
			if (globalSpells[spellIndex]->name != "Empty")
			{
				globalSpells[spellIndex]->image.render(gRenderer, invSheetClip.x + xspace + 58, invSheetClip.y + yspace + 45);
				globalSpells[spellIndex]->nameTexture.render(gRenderer, invSheetClip.x + xspace + 105, invSheetClip.y + yspace + 42);
				globalSpells[spellIndex]->cdTexture.render(gRenderer, invSheetClip.x + xspace + 105, invSheetClip.y + yspace + 57);
				globalSpells[spellIndex]->energyTexture.render(gRenderer, invSheetClip.x + xspace + 105, invSheetClip.y + yspace + 72);
				globalSpells[spellIndex]->descriptionText.render(gRenderer, invSheetClip.x + xspace + 45, invSheetClip.y + yspace + 92);
			}
		}
		else if (i == 0)
		{
			basicAttackIcon.render(gRenderer, invSheetClip.x + xspace + 58, invSheetClip.y + yspace + 45);
			attackText.render(gRenderer, invSheetClip.x + xspace + 105, invSheetClip.y + yspace + 42);
			attackDesc.render(gRenderer, invSheetClip.x + xspace + 45, invSheetClip.y + yspace + 92);
		}

		//Up the xspace
		xspace += 220;
	}

	//Display spells
	yspace = 77;
	xspace = 7;
	spellBG.render(gRenderer, SCREEN_WIDTH - invSheetClip.x - 325, invSheetClip.y);
	int minX = SCREEN_WIDTH - invSheetClip.x - 325;

	sbSheet.render(gRenderer, minX + 18, 96);
	if (spellTab == SPELL_TYPE_DAMAGE)
	{
		sbOut.render(gRenderer, minX + 18, 96);
		SDL_Rect sBox = { minX + 18 + 144, 96 + 15, 145, 30 };
		if (checkCollision(mouseClip, sBox))
		{
			sbInH.render(gRenderer, sBox.x, sBox.y - 15);
		}
		else
			sbIn.render(gRenderer, sBox.x, sBox.y - 15);
		desText.render(gRenderer, minX + 47, 118);
		defText.render(gRenderer, minX + 144 + 56, 96 + 33);
		if (checkCollision(mouseClip, sBox) && mouseLeft)
		{
			spellTab = SPELL_TYPE_DEFENSE;
			mouseLeft = false;
			clickedSpell = false;
			selectedSpell = -1;
			clickingSpell = false;
			rebuildAttackText();
		}
	}
	else if (spellTab == SPELL_TYPE_DEFENSE)
	{
		sbOut2.render(gRenderer, minX + 18 + 144, 96);
		SDL_Rect sBox = { minX + 18, 96 + 15, 144, 30 };
		if (checkCollision(mouseClip, sBox))
		{
			sbIn2H.render(gRenderer, sBox.x, sBox.y - 15);
		}
		else
			sbIn2.render(gRenderer, sBox.x, sBox.y - 15);
		defText.render(gRenderer, minX + 144 + 49, 118);
		desText.render(gRenderer, minX + 54, 96 + 33);
		if (checkCollision(mouseClip, sBox) && mouseLeft)
		{
			spellTab = SPELL_TYPE_DAMAGE;
			mouseLeft = false;
			clickedSpell = false;
			selectedSpell = -1;
			clickingSpell = false;
			rebuildAttackText();
		}
	}

	spellPointsText.render(gRenderer, minX + 85, 41);
	offensiveText.render(gRenderer, minX + 40, 57);
	defensiveText.render(gRenderer, minX + 40, 74);
	desIcon.render(gRenderer, minX + 42 + (9 * 20) + desSpace, 58);
	resIcon.render(gRenderer, minX + 42 + (9 * 20) + resSpace, 75);
	if (saveFile.spellPoints > 0)
	{
		incDES.draw();
		incRES.draw();
	}
	else
	{
		incDES.visible = false;
		incRES.visible = false;
	}

	int slevel = 0;
	if (spellTab == SPELL_TYPE_DAMAGE)
		slevel = saveFile.damageLevel;
	else if (spellTab == SPELL_TYPE_DEFENSE)
		slevel = saveFile.defenseLevel;

	if (selectedSpell != -1 && mouseLeft)
	{
		findSpell(selectedSpell);
		if (globalSpells[selectedSpell]->spellType != spellTab && mouseLeft && !checkCollision(mouseClip, invSheetClip))
		{
			clickedSpell = false;
			selectedSpell = -1;
			clickingSpell = false;
		}
	}

	for (unsigned int i = 1; i < globalSpells.size(); i++)
	{
		if (globalSpells[i]->spellType != spellTab)
			continue;

		if (i + 1 % 4 == 0)
		{
			yspace += 60;
			xspace = 0;
		}
		SDL_Rect abox{ minX + xspace + 40, invSheetClip.y + yspace + 70, 50, 50 };
		if (checkCollision(mouseClip, abox) && (!mouseLeft || (slevel < globalSpells[i]->levelreq && spellTab == globalSpells[i]->spellType)))
		{
			if (!mouseLeft && clickingSpell && clickedSpell && selectedSpell == i)
			{
				selectedSpell = -1;
				clickedSpell = false;
			}
			else if (selectedSpell == i && !mouseLeft && clickingSpell)
				clickedSpell = true;
			int em = abox.x - abilityBox.getWidth();
			inventoryBoxHovered.render(gRenderer, minX + xspace + 40, invSheetClip.y + yspace + 70);
			if (!clickedSpell)
			{
				abilityBox.render(gRenderer, em, abox.y);
				inventoryBox.render(gRenderer, em + 22, abox.y + 17);
				globalSpells[i]->image.render(gRenderer, em + 30, abox.y + 25);
				globalSpells[i]->levelText.render(gRenderer, em + 27, abox.y + 49);
				globalSpells[i]->nameTexture.render(gRenderer, em + 77, abox.y + 19);
				globalSpells[i]->cdTexture.render(gRenderer, em + 77, abox.y + 34);
				globalSpells[i]->energyTexture.render(gRenderer, em + 77, abox.y + 49);
				globalSpells[i]->descriptionText.render(gRenderer, em + 17, abox.y + 69);
			}
			clickingSpell = false;
		}
		else if (checkCollision(mouseClip, abox) && mouseLeft && slevel >= globalSpells[i]->levelreq && spellTab == globalSpells[i]->spellType)
		{
			inventoryBoxClicked.render(gRenderer, minX + xspace + 40, invSheetClip.y + yspace + 70);
			if (selectedSpell == -1)
			{
				selectedSpell = i;
			}
			clickingSpell = true;
		}
		else if (slevel >= globalSpells[i]->levelreq && spellTab == globalSpells[i]->spellType)
		{
			inventoryBox.render(gRenderer, minX + xspace + 40, invSheetClip.y + yspace + 70);
		}
		else
		{
			inventoryBoxHovered.render(gRenderer, minX + xspace + 40, invSheetClip.y + yspace + 70);
		}
		if (clickedSpell && selectedSpell == i && mouseLeft && !checkCollision(mouseClip, abox) && !checkCollision(mouseClip, invSheetClip))
		{
			clickedSpell = false;
			selectedSpell = -1;
			clickingSpell = false;
		}
		if (clickingSpell && selectedSpell == i && !checkCollision(mouseClip, abox))
		{
			clickingSpell = false;
			clickedSpell = false;
			selectedSpell = -1;
		}
		if (selectedSpell != i)
			globalSpells[i]->image.render(gRenderer, minX + xspace + 48, invSheetClip.y + yspace + 78);
		else if (selectedSpell == i && !clickedSpell)
			globalSpells[i]->image.render(gRenderer, minX + xspace + 48, invSheetClip.y + yspace + 78);
		else if (selectedSpell == i && clickedSpell)
			inventoryBoxClicked.render(gRenderer, minX + xspace + 40, invSheetClip.y + yspace + 70);
		globalSpells[i]->levelText.render(gRenderer, minX + xspace + 45, invSheetClip.y + yspace + 102);

		xspace += 60;
	}

	if (clickedSpell)
	{
		globalSpells[selectedSpell]->image.render(gRenderer, mouseClip.x - 18, mouseClip.y - 18);
	}
}

void Player::closeSpells()
{
	displaySpells = false;

	for (int i = 0; i < 6; i++)
		aicc[i] = 0;

	selectedSpell = -1;
	clickedSpell = false;
	clickingSpell = false;
}

void Player::displayEscapeMenu()
{
	escapeMenu.render(gRenderer, invSheetClip.x, invSheetClip.y);
	exitGame.draw();
	resumeGame.draw();
}

void Player::closeEscapeMenu()
{
	displayEscape = false;
}

bool Player::findItem(std::string iname)
{
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		if (inventory[i].name == iname)
		{
			return true;
		}
	}
	return false;
}

bool Player::findAndDeleteItem(std::string iname)
{
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		if (inventory[i].name == iname)
		{
			inventory[i].clearSlot();
			return true;
		}
	}
	return false;
}

void Player::deleteAllType(int itype)
{
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		if (inventory[i].itemType == itype)
		{
			inventory[i].clearSlot();
		}
	}
}

void Player::rebuildAttackText()
{
	for (unsigned int i = 0; i < globalSpells.size(); i++)
		globalSpells[i]->rebuildText();

	std::stringstream dmgTxt;
	dmgTxt.precision(2);
	dmgTxt << std::fixed << (double)weapon.reload / 60.0;
	std::stringstream nrgTxt;
	nrgTxt.precision(2);
	nrgTxt << std::fixed << weapon.energyCost;
	double strmod = (strength * STR_BONUS) * ((double)weapon.reload / 60);
	attackText.loadFromRenderedText(gRenderer, "Basic Attack\nCooldown: "+dmgTxt.str()+"s\nEnergy: "+nrgTxt.str(), SDL_Color{ 255,255,255 }, CodeNewRoman15, 200);
	attackDesc.loadFromRenderedText(gRenderer, "Shoots a bullet in the direction of your cursor, dealing " + std::to_string((int)std::round(weapon.minDamage + strmod)) + " - " + std::to_string((int)std::round(weapon.maxDamage + strmod))+" damage.", SDL_Color{ 255,255,255 }, CodeNewRoman15, 190);

	offensiveText.loadFromRenderedText(gRenderer, "Destruction Level: " + std::to_string(saveFile.damageLevel), SDL_Color{ 255,255,255 }, CodeNewRoman16, 400);
	defensiveText.loadFromRenderedText(gRenderer, " Protection Level: " + std::to_string(saveFile.defenseLevel), SDL_Color{ 255,255,255 }, CodeNewRoman16, 400);
	spellPointsText.loadFromRenderedText(gRenderer, "Spell Points: " + std::to_string(saveFile.spellPoints), SDL_Color{ 255,255,255 }, CodeNewRoman16, 400);

	if (spellTab == SPELL_TYPE_DAMAGE)
	{
		desText.loadFromRenderedText(gRenderer, "Destruction\n  Spells", SDL_Color{ 255,255,255 }, CodeNewRoman15, 200);
		defText.loadFromRenderedText(gRenderer, "Protection", SDL_Color{ 255,255,255 }, CodeNewRoman12, 200);
	}
	else if (spellTab == SPELL_TYPE_DEFENSE)
	{
		desText.loadFromRenderedText(gRenderer, "Destruction", SDL_Color{ 255,255,255 }, CodeNewRoman12, 200);
		defText.loadFromRenderedText(gRenderer, "Protection\n  Spells", SDL_Color{ 255,255,255 }, CodeNewRoman15, 200);
	}
	desSpace = (std::to_string((int)saveFile.damageLevel).length() * 9) - 9;
	resSpace = (std::to_string((int)saveFile.defenseLevel).length() * 9) - 9;
}

void Player::rebuildInventoryText()
{
	statText.clear();
	statText.str(std::string());
	statText.fixed;
	statText.precision(0);
	statText << "Level: " << level;
	levelText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman16, 185);

	statText.clear();
	statText.str(std::string());
	statText.fixed;
	statText.precision(0);
	statText << "EXP: " << exp << "/" << maxExp;
	expText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman16, 185);

	statText.clear();
	statText.str(std::string());
	statText.fixed;
	statText.precision(0);
	statText << "Attribute Points: " << skillPoints;
	statText << "\n Strength: " << strength;
	statText << "\n Vitality: " << vitality;
	statText << "\nDexterity: " << dexterity;
	statText << "\n    Armor: " << armorRating + armorRatingBonus;
	//if (armorRatingBonus > 0)
	//	statText << "+" << armorRatingBonus;
	statText << "\n   Plasma: " << plasma;
	statsText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman16, 200);
	armorSpace = (std::to_string((int)(armorRating + armorRatingBonus)).length() * 9) - 9;
	//if (armorRatingBonus > 0)
	//	armorSpace += (std::to_string((int)armorRatingBonus*10).length() * 9);
	plasmaSpace = (std::to_string(plasma).length() * 9) - 9;
	vitalitySpace = (std::to_string(vitality).length() * 9) - 9;
	dexteritySpace = (std::to_string(dexterity).length() * 9) - 9;
	strengthSpace = (std::to_string(strength).length() * 9) - 9;

	statText.clear();
	statText.str(std::string());
	statText.fixed;
	statText.precision(2);
	double strmod = (strength * STR_BONUS) * ((double)weapon.reload / 60);
	if (infoPage == "Weapon")
	{
		statText << "  Weapon Information\n";
		statText << "Damage: " << weapon.minDamage + strmod << " - " << weapon.maxDamage + strmod;
		statText << "\n   DPS: " << calculateDPS(weapon, strmod);
		statText.precision(1);
		statText << "\nEnergy: " << calculateEPS(weapon) << "/s";
		statText << "\n Range: " << ((double)weapon.range / TILE_SIZE) << " Tiles";
		statText.precision(2);
		statText << "\nReload: " << ((double)weapon.reload) / 60.0 << "s";
	}
	if (infoPage == "Armor")
	{
		statText << "   Armor Information\n";
		statText << "  Enemy      Damage\n";
		statText << "  Level     Absorbed\n";
		int levelx = level;
		if (level < 2)
			levelx = 2;
		for (int i = levelx - 1; i < levelx + 3; i++)
		{
			statText << "    ";
			if (i < 10)
				statText << " ";
			statText << i << "         ";
			int reduc = calculateArmor(i, armorRating + armorRatingBonus);
			if (reduc < 10)
				statText << " ";
			statText << reduc << "%\n";
		}
	}
	infoText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman15, 190);

	checkStats();

	statText.clear();
	statText.str(std::string());
	statText.fixed;
	statText.precision(1);
	statText << std::fixed << "+" << ((maxHealth * BASE_HEALTH_MULTIPLYER) + BASE_HEALTH_REGEN) << "/s";
	hpsText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman12, 60);

	statText.clear();
	statText.str(std::string());
	statText.fixed;
	statText.precision(1);
	statText << std::fixed << "+" << ((maxEnergy * 0.02) + BASE_ENERGY_REGEN) << "/s";
	epsText.loadFromRenderedText(gRenderer, statText.str(), SDL_Color{ 255,255,255 }, CodeNewRoman12, 60);
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
	if (round(health) <= 0)
		alive = false;

	if (alive == false) //Enemy dies
	{
		//Reward exp
		player.exp += expReward;
		if (player.displayInv)
			player.rebuildInventoryText();
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

void Enemy::handleMove(Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y], std::vector<Sprite>&sprites, std::vector<Enemy> &enemies, Player &player, std::vector<Bullet> &bullets) //This code is ugly :(
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

	double xAng = (atan2(deltaY, deltaX)*180.0000) / 3.1416 - 90;
	Sprite::setAngle(xAng);

	//Set speed variables
	xVel = (Entity::velocity + Entity::velocityBonus) * (deltaX / magnitude);
	yVel = (Entity::velocity + Entity::velocityBonus) * (deltaY / magnitude);

	if (Sprite::getCurrentAnimation() == "Shoot" && Sprite::finishedAnimation())
		Sprite::setAnimation("Idle");

	if (rotationcd != 0)
	{
		//Spell handle for bosses
		if (rotationtime == rotationcd)
		{
			rotationtime = 0;
			Sprite::setAnimation("Shoot");
			rotation[currentspell]->uid = uniqueID;
			spells->push_back(rotation[currentspell]);
			currentspell++;
			if (currentspell >= rotation.size())
				currentspell = 0;
		}
		else
		{
			rotationtime++;
		}
	}

	if (sqrt(diffX*diffX + diffY*diffY) < (weapon.range - (weapon.range * (0.10 * minRange))))
	{
		if (reloading == false)
		{
			Sprite::setAnimation("Shoot");
			double xVelB = weapon.speed * (deltaX / magnitude);
			double yVelB = weapon.speed * (deltaY / magnitude);
			double dmg = weapon.minDamage + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (weapon.maxDamage - weapon.minDamage)));
			weapon.bullet.type = BULLET_ENEMY;
			weapon.bullet.setPosition(Sprite::getBox().x + (Sprite::getBox().w / 2) - (weapon.bullet.getBox().w / 2), Sprite::getBox().y + (Sprite::getBox().h / 2) - (weapon.bullet.getBox().h / 2));
			weapon.bullet.setBullet(xVelB, yVelB, dmg, xAng, weapon.range, level);
			weapon.bullet.shooterID = uniqueID;
			bullets.push_back(weapon.bullet);
			reloading = true;
		}
		if (!wallWasHit)
			return;
	}

	if (Entity::velocity == 0)
		return;

	if ((xVel != 0 || yVel != 0) && Sprite::getCurrentAnimation() != "Shoot")
		Sprite::setAnimationRaw("Move");

	maxCamY = (int)ceil(aiCamera.y / TILE_SIZE);
	maxCamH = (int)(((ceil(aiCamera.y / TILE_SIZE) + ceil(aiCamera.h / TILE_SIZE)) < TOTAL_TILES_Y - 1) ? ceil(aiCamera.y / TILE_SIZE) + ceil(aiCamera.h / TILE_SIZE) + 1 : ceil(aiCamera.y / TILE_SIZE) + ceil(aiCamera.h / TILE_SIZE));
	maxCamX = (int)ceil(aiCamera.x / TILE_SIZE);
	maxCamW = (int)(((ceil(aiCamera.x / TILE_SIZE) + ceil(aiCamera.w / TILE_SIZE)) < TOTAL_TILES_X - 1) ? ceil(aiCamera.x / TILE_SIZE) + ceil(aiCamera.w / TILE_SIZE) + 1 : ceil(aiCamera.x / TILE_SIZE) + ceil(aiCamera.w / TILE_SIZE));
	
	bool Xsuccess = true;
	bool Ysuccess = true;

	if (xMod != 0)
		xVel = ((Entity::velocity + Entity::velocityBonus) * xMod);// +(deltaX / magnitude) + (divvy / 20);
	if (yMod != 0)
		yVel = ((Entity::velocity + Entity::velocityBonus) * yMod);// +(deltaY / magnitude) + (divvy / 20);

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

	//Check collision between every sprite
	for (unsigned int i = 0; i < sprites.size(); i++)
	{
		if (checkCollision(aiCamera, sprites[i].getBox()) == true && sprites[i].getCollide() == true && checkCollision(Sprite::getBox(), sprites[i].getBox()) == true)
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

	//Check collision between every sprite
	for (unsigned int i = 0; i < sprites.size(); i++)
	{
		if (checkCollision(aiCamera, sprites[i].getBox()) == true && sprites[i].getCollide() == true && checkCollision(Sprite::getBox(), sprites[i].getBox()) == true)
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

void Enemy::buildEnemy(int ai, int hp, int ar, double vel, int vis, int xpr, int lvl, string nm, Weapon wp)
{
	aiType = ai;
	maxHealth = hp;
	health = hp;
	armorRating = ar;
	setVelocity(vel);
	setVision(vis);
	expReward = xpr;
	level = lvl;
	name = nm;
	weapon = wp;
	buildLocalImages();
}

void Enemy::buildResource(int ai, int hp, int ar, double vel, int vis, int xpr, int lvl, string nm)
{
	aiType = ai;
	maxHealth = hp;
	health = hp;
	armorRating = ar;
	setVelocity(vel);
	setVision(vis);
	expReward = xpr;
	level = lvl;
	name = nm;
	buildLocalImages();
}

void Enemy::setSpellcaster(bool ib, int rcd, std::vector<Spell*> *sps)
{
	isBoss = ib;
	rotationcd = rcd;
	spells = sps;
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

void Enemy::draw(vector<FloatingText> &texts)
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
	codeNewRoman18 = TTF_OpenFont("bin/fonts/codenewroman.ttf", 18);

	if (Entity::velocity == 0)
		minRange = false;
}

PressurePlate::PressurePlate()
{
}

PressurePlate::PressurePlate(int xpos, int ypos, std::string nm)
{
	name = nm;
	box = { xpos, ypos, 50, 50 };
}

PressurePlate::~PressurePlate()
{
}

PressureDoor::PressureDoor()
{
}

PressureDoor::PressureDoor(int xpos, int ypos, std::string nm)
{
	box = { xpos, ypos, 50, 50 };
	name = nm;
}

PressureDoor::~PressureDoor()
{
}
