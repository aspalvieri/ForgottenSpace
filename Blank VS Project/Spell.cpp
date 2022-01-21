#include "Spell.h"

Spell::Spell()
{
}

Spell::Spell(Player &p, std::vector<Enemy>&e, std::vector<Bullet> &b, std::vector<FloatingText> &f, std::string n)
{
	playerptr = &p;
	enemyptr = &e;
	bulletptr = &b;
	textptr = &f;
	name = n;
	id = globalID++;
}

Spell::~Spell()
{
}

void Spell::buildTexture(SDL_Renderer *gR)
{
	gRenderer = gR;
	font = TTF_OpenFont("bin/fonts/codenewroman.ttf", 15);
	if (font == NULL)
		std::cout << "Item::buildTooltip() Font was NULL!\n";
	nameTexture.loadFromRenderedText(gR, name, nameColor, font, 400);
	cdTexture.loadFromRenderedText(gR, "Cooldown: " + std::to_string((int)((double)cooldown / 60)) + "s", SDL_Color{ 255,255,255 }, font, 400);
	energyTexture.loadFromRenderedText(gR, "Energy: " + std::to_string((int)cost), SDL_Color{ 255,255,255 }, font, 400);
	levelText.loadFromRenderedText(gR, std::to_string((int)levelreq), SDL_Color{ 255,255,255 }, font, 400);
}

bool EmptySpell::canCast()
{
	return false;
}

void EmptySpell::activate()
{
}

bool EmptySpell::isActive()
{
	return false;
}

bool BurstShield::canCast()
{
	calculateValues();
	if (playerptr->energy >= cost) {
		for (unsigned int i = 0; i < playerptr->effects.size(); i++)
		{
			if (playerptr->effects[i]->type == "shield")
			{
				playerptr->shields = 0;
				playerptr->shieldsmax = playerptr->shields;
				playerptr->effects[i]->texture.free();
				playerptr->effects.erase(playerptr->effects.begin() + i);
				i--;
			}
		}
		return true;
	}
	else
		textptr->push_back(FloatingText("Not Enough Energy!", playerptr->getBox().x + PLAYER_SIZE / 2, playerptr->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, playerptr->CodeNewRoman20, SDL_Color{ 255,0,0 }));
	return false;
}

void BurstShield::activate()
{
	calculateValues();
	playerptr->energy -= cost;
	/*
	int potentialheal = healAmount;
	playerptr->health += healAmount;
	if (playerptr->health > playerptr->maxHealth)
	{
		potentialheal -= (playerptr->health - playerptr->maxHealth);
		playerptr->health = playerptr->maxHealth;
	}
	textptr->push_back(FloatingText("+" + std::to_string(potentialheal), playerptr->getBox().x + PLAYER_SIZE / 2, playerptr->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, playerptr->CodeNewRoman20, SDL_Color{ 0,255,0 }));
	*/
	textptr->push_back(FloatingText("+" + std::to_string(shieldAmount), playerptr->getBox().x + PLAYER_SIZE / 2, playerptr->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, playerptr->CodeNewRoman20, SDL_Color{ 171,155,206 }));
	playerptr->effects.push_back(new EffectShield(shieldDuration, 1, shieldAmount));
}

void BurstShield::calculateValues()
{
	shieldAmount = baseShieldAmount + (playerptr->vitality * (VIT_BONUS * 0.33)) + ((double)playerptr->saveFile.defenseLevel * ((VIT_BONUS * 0.66)));
	cost = baseCost + (playerptr->saveFile.defenseLevel * COST_INC);
}

void BurstShield::rebuildText()
{
	calculateValues();
	energyTexture.loadFromRenderedText(gRenderer, "Energy: " + std::to_string((int)cost), SDL_Color{ 255,255,255 }, font, 400);
	descriptionText.loadFromRenderedText(gRenderer, "Creates a shield that absorbs " + std::to_string((int)std::round(shieldAmount)) + " health. Lasts " + to_string((int)((double)(shieldDuration) / SCREEN_FPS)) + " seconds.", SDL_Color{ 255,255,255 }, font, 190);
}

bool BurstShield::isActive()
{
	for (unsigned int i = 0; i < playerptr->effects.size(); i++)
	{
		if (playerptr->effects[i]->type == "shield")
		{
			textptr->push_back(FloatingText("This spell is active!", playerptr->getBox().x + PLAYER_SIZE / 2, playerptr->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, playerptr->CodeNewRoman16, SDL_Color{ 255,0,0 }));
			return true;
		}
	}
	return false;
}

bool BurstShot::canCast()
{
	calculateValues();
	if (playerptr->energy >= cost)
		return true;
	else
		textptr->push_back(FloatingText("Not Enough Energy!", playerptr->getBox().x + PLAYER_SIZE / 2, playerptr->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, playerptr->CodeNewRoman20, SDL_Color{ 255,0,0 }));
	return false;
}

void BurstShot::activate()
{
	calculateValues();
	playerptr->energy -= cost;

	SDL_Rect mouseClip = playerptr->mouseClip;
	double deltaX = (mouseClip.x + playerptr->camera->x) - (playerptr->getPosition().first + (playerptr->getBox().w / 2));
	double deltaY = (mouseClip.y + playerptr->camera->y) - (playerptr->getPosition().second + (playerptr->getBox().h / 2));
	double xAng = (atan2(deltaY, deltaX)*180.0000) / 3.1416 + 90;

	double magnitude = sqrt(deltaX*deltaX + deltaY*deltaY);
	double xVelB = bulletSpeed * (deltaX / magnitude);
	double yVelB = bulletSpeed * (deltaY / magnitude);

	double dmg = (minDamage) + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / ((maxDamage) - (minDamage))));

	bullet.type = BULLET_PLAYER;
	bullet.effects.clear();
	bullet.lifesteal = lifeSteal;
	bullet.setPosition(playerptr->getBox().x + (playerptr->getBox().w / 2) - (bullet.getBox().w / 2), playerptr->getBox().y + (playerptr->getBox().h / 2) - (bullet.getBox().h / 2));
	bullet.setBullet(xVelB, yVelB, dmg, xAng, range);
	bullet.spellBullet = true;

	bulletptr->push_back(bullet);
}

void BurstShot::calculateValues()
{
	double strmod = (playerptr->strength * (STR_BONUS)) + ((double)playerptr->saveFile.damageLevel * (SPELL_BONUS));
	minDamage = baseMinDamage + (strmod * 0.9);
	maxDamage = baseMaxDamage + (strmod * 1.1);
	cost = baseCost + (playerptr->saveFile.damageLevel * COST_INC);
}

void BurstShot::rebuildText()
{
	calculateValues();
	energyTexture.loadFromRenderedText(gRenderer, "Energy: " + std::to_string((int)cost), SDL_Color{ 255,255,255 }, font, 400);
	descriptionText.loadFromRenderedText(gRenderer, "Shoots a bullet dealing " + std::to_string((int)std::round(minDamage)) + " - " + std::to_string((int)std::round(maxDamage)) + " damage, healing you for "+to_string((int)(lifeSteal*100))+"% of the damage.", SDL_Color{ 255,255,255 }, font, 190);
}

bool BurstShot::isActive()
{
	return false;
}

bool FrostShock::canCast()
{
	calculateValues();
	if (playerptr->energy >= cost)
		return true;
	else
		textptr->push_back(FloatingText("Not Enough Energy!", playerptr->getBox().x + PLAYER_SIZE / 2, playerptr->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, playerptr->CodeNewRoman20, SDL_Color{ 255,0,0 }));
	return false;
}

void FrostShock::activate()
{
	calculateValues();
	playerptr->energy -= cost;

	SDL_Rect mouseClip = playerptr->mouseClip;
	double deltaX = (mouseClip.x + playerptr->camera->x) - (playerptr->getPosition().first + (playerptr->getBox().w / 2));
	double deltaY = (mouseClip.y + playerptr->camera->y) - (playerptr->getPosition().second + (playerptr->getBox().h / 2));
	double xAng = (atan2(deltaY, deltaX)*180.0000) / 3.1416 + 90;

	double magnitude = sqrt(deltaX*deltaX + deltaY * deltaY);
	double xVelB = bulletSpeed * (deltaX / magnitude);
	double yVelB = bulletSpeed * (deltaY / magnitude);

	double dmg = (minDamage)+static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / ((maxDamage)-(minDamage))));

	bullet.type = BULLET_PLAYER;
	bullet.effects.clear();
	bullet.effects.push_back(new EffectSlow(slowTime, 1, slowPercent));
	bullet.setPosition(playerptr->getBox().x + (playerptr->getBox().w / 2) - (bullet.getBox().w / 2), playerptr->getBox().y + (playerptr->getBox().h / 2) - (bullet.getBox().h / 2));
	bullet.setBullet(xVelB, yVelB, dmg, xAng, range);
	bullet.spellBullet = true;

	bulletptr->push_back(bullet);
}

void FrostShock::calculateValues()
{
	double strmod = (playerptr->strength * (STR_BONUS - 0.2)) + ((double)playerptr->saveFile.damageLevel * (SPELL_BONUS - 0.6));
	minDamage = baseMinDamage + (strmod * 0.9);
	maxDamage = baseMaxDamage + (strmod * 1.1);
	cost = baseCost + (playerptr->saveFile.damageLevel * COST_INC);
}

void FrostShock::rebuildText()
{
	calculateValues();
	energyTexture.loadFromRenderedText(gRenderer, "Energy: " + std::to_string((int)cost), SDL_Color{ 255,255,255 }, font, 400);
	descriptionText.loadFromRenderedText(gRenderer, "Shoots a bullet dealing " + std::to_string((int)std::round(minDamage)) + " - " + std::to_string((int)std::round(maxDamage)) + " damage and slowing the target by " + to_string((int)(slowPercent*100)) + "% for " + to_string((int)(slowTime / 60.0)) + " seconds.", SDL_Color{ 255,255,255 }, font, 190);
}

bool FrostShock::isActive()
{
	return false;
}

void BurstShotBoss::activate()
{
	int id = -1;
	for (unsigned int i = 0; i < enemyptr->size(); i++)
	{
		if ((*enemyptr)[i].uniqueID == uid)
		{
			id = i;
			break;
		}
	}

	double divvy = (rand() % (40 - 30 + 1) + 30) / 10; //""Accuracy""
	double deltaX = (playerptr->getBox().x + (playerptr->getBox().w / 2)) - ((*enemyptr)[id].getBox().x + ((*enemyptr)[id].getBox().w / 2)) + (playerptr->xVel * ((double)playerptr->getBox().w / divvy));
	double deltaY = (playerptr->getBox().y + (playerptr->getBox().h / 2)) - ((*enemyptr)[id].getBox().y + ((*enemyptr)[id].getBox().h / 2)) + (playerptr->yVel * ((double)playerptr->getBox().h / divvy));
	double magnitude = sqrt(deltaX*deltaX + deltaY*deltaY);
	double xAng = (atan2(deltaY, deltaX)*180.0000) / 3.1416 - 90;
	double xVelB = bulletSpeed * (deltaX / magnitude);
	double yVelB = bulletSpeed * (deltaY / magnitude);
	double dmg = minDamage + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (maxDamage - minDamage)));
	bullet.type = BULLET_ENEMY;
	bullet.setPosition((*enemyptr)[id].getBox().x + ((*enemyptr)[id].getBox().w / 2) - (bullet.getBox().w / 2), (*enemyptr)[id].getBox().y + ((*enemyptr)[id].getBox().h / 2) - (bullet.getBox().h / 2));
	bullet.setBullet(xVelB, yVelB, dmg, xAng, range, (*enemyptr)[id].level);
	bullet.shooterID = id;
	bulletptr->push_back(bullet);
}

bool BurstShotBoss::isActive()
{
	return false;
}

bool GlimmerofLight::canCast()
{
	calculateValues();
	if (playerptr->energy >= cost && playerptr->health != playerptr->maxHealth) {
		for (unsigned int i = 0; i < playerptr->effects.size(); i++)
		{
			if (playerptr->effects[i]->type == "heal")
			{
				playerptr->effects[i]->texture.free();
				playerptr->effects.erase(playerptr->effects.begin() + i);
				i--;
			}
		}
		return true;
	}
	else if (playerptr->health == playerptr->maxHealth)
		textptr->push_back(FloatingText("Health Is Full!", playerptr->getBox().x + PLAYER_SIZE / 2, playerptr->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, playerptr->CodeNewRoman16, SDL_Color{ 255,0,0 }));
	else
		textptr->push_back(FloatingText("Not Enough Energy!", playerptr->getBox().x + PLAYER_SIZE / 2, playerptr->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, playerptr->CodeNewRoman20, SDL_Color{ 255,0,0 }));
	return false;
}

void GlimmerofLight::activate()
{
	calculateValues();
	playerptr->energy -= cost;
	playerptr->effects.push_back(new EffectHeal(timeBetweenTicks, ticks, healAmount));
}

void GlimmerofLight::calculateValues()
{
	healAmount = baseHealAmount + ((double)playerptr->vitality / ticks) + (((double)playerptr->saveFile.defenseLevel * (SPELL_BONUS)) / ticks);
	cost = baseCost + (playerptr->saveFile.defenseLevel * COST_INC);
}

void GlimmerofLight::rebuildText()
{
	calculateValues();
	energyTexture.loadFromRenderedText(gRenderer, "Energy: " + std::to_string((int)cost), SDL_Color{ 255,255,255 }, font, 400);
	descriptionText.loadFromRenderedText(gRenderer, "Heals the player for up to " + std::to_string((int)std::round(healAmount*ticks)) + " health over " + std::to_string((int)((double)(timeBetweenTicks*ticks)/SCREEN_FPS)) + " seconds.", SDL_Color{ 255,255,255 }, font, 190);
}

bool GlimmerofLight::isActive()
{
	for (unsigned int i = 0; i < playerptr->effects.size(); i++)
	{
		if (playerptr->effects[i]->type == "heal")
		{
			textptr->push_back(FloatingText("This spell is active!", playerptr->getBox().x + PLAYER_SIZE / 2, playerptr->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, playerptr->CodeNewRoman16, SDL_Color{ 255,0,0 }));
			return true;
		}
	}
	return false;
}

bool GuardianShield::canCast()
{
	calculateValues();
	/*
	for (unsigned int i = 0; i < playerptr->effects.size(); i++)
	{
		if (playerptr->effects[i]->type == "armor")
		{
			textptr->push_back(FloatingText("Armor Bonus Is Already Active!", playerptr->getBox().x + PLAYER_SIZE / 2, playerptr->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, playerptr->CodeNewRoman16, SDL_Color{ 255,0,0 }));
			return false;
		}
	}
	*/
	if (playerptr->energy >= cost) {
		for (unsigned int i = 0; i < playerptr->effects.size(); i++)
		{
			if (playerptr->effects[i]->type == "armor")
			{
				playerptr->armorRatingBonus -= shieldAmount;
				playerptr->rebuildInventoryText();
				playerptr->effects[i]->texture.free();
				playerptr->effects.erase(playerptr->effects.begin() + i);
				i--;
			}
		}
		return true;
	}
	else
		textptr->push_back(FloatingText("Not Enough Energy!", playerptr->getBox().x + PLAYER_SIZE / 2, playerptr->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, playerptr->CodeNewRoman20, SDL_Color{ 255,0,0 }));
	return false;
}

void GuardianShield::activate()
{
	calculateValues();
	playerptr->energy -= cost;
	textptr->push_back(FloatingText("+" + std::to_string(shieldAmount), playerptr->getBox().x + PLAYER_SIZE / 2, playerptr->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, playerptr->CodeNewRoman20, SDL_Color{ 150,150,150 }));
	playerptr->effects.push_back(new EffectArmor(shieldDuration, 1, shieldAmount));
}

void GuardianShield::calculateValues()
{
	shieldAmount = round((double)baseShieldAmount + ((double)playerptr->level * 2.5) + ((double)playerptr->saveFile.defenseLevel * (SPELL_BONUS)));
	cost = baseCost + (playerptr->saveFile.defenseLevel * COST_INC);
}

void GuardianShield::rebuildText()
{
	calculateValues();
	energyTexture.loadFromRenderedText(gRenderer, "Energy: " + std::to_string((int)cost), SDL_Color{ 255,255,255 }, font, 400);
	descriptionText.loadFromRenderedText(gRenderer, "Increases armor rating by " + std::to_string((int)std::round(shieldAmount)) + ". Lasts " + to_string((int)((double)(shieldDuration) / SCREEN_FPS)) + " seconds.", SDL_Color{ 255,255,255 }, font, 190);
}

bool GuardianShield::isActive()
{
	for (unsigned int i = 0; i < playerptr->effects.size(); i++)
	{
		if (playerptr->effects[i]->type == "armor")
		{
			textptr->push_back(FloatingText("This spell is active!", playerptr->getBox().x + PLAYER_SIZE / 2, playerptr->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, playerptr->CodeNewRoman16, SDL_Color{ 255,0,0 }));
			return true;
		}
	}
	return false;
}

void EffectFire::runEffect(Entity * entity, vector<FloatingText> &texts, TTF_Font *font)
{
	if (runtime)
	{
		texture.loadFromFile("bin/images/effects/fire.png", entity->gRenderer);
		runtime = false;
	}
	if (currentDuration <= totalDuration)
	{
		currentDuration++;
		if (currentDuration % timeBetweenTick == 0)
		{
			//Effect itself
			entity->takeDamage(damage);
			texts.push_back(FloatingText(std::to_string((int)round(damage)), entity->getBox().x + PLAYER_SIZE / 2, entity->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, font, SDL_Color{ 255,0,0 }));
		}
	}
	else
	{
		alive = false;
		texture.free();
	}
}

void EffectHeal::runEffect(Entity * entity, vector<FloatingText>&texts, TTF_Font *font)
{
	if (runtime)
	{
		texture.loadFromFile("bin/images/effects/heal.png", entity->gRenderer);
		runtime = false;
	}
	if (currentDuration <= totalDuration)
	{
		currentDuration++;
		if (currentDuration % timeBetweenTick == 0)
		{
			//Effect itself
			int potentialheal = healamount;
			entity->health += healamount;
			if (entity->health > entity->maxHealth)
			{
				potentialheal -= (entity->health - entity->maxHealth);
				entity->health = entity->maxHealth;
			}
			texts.push_back(FloatingText("+" + std::to_string((int)round(potentialheal)), entity->getBox().x + PLAYER_SIZE / 2, entity->getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, font, SDL_Color{ 0,255,0 }));
		}
	}
	else
	{
		alive = false;
		texture.free();
	}
}

void EffectShield::runEffect(Entity * entity, vector<FloatingText>&texts, TTF_Font *font)
{
	if (runtime)
	{
		texture.loadFromFile("bin/images/effects/shield.png", entity->gRenderer);
		entity->shields += shieldamount;
		entity->shieldsmax = entity->shields;
		runtime = false;
	}
	if (currentDuration <= totalDuration)
	{
		currentDuration++;
	}
	else
	{
		entity->shields = 0;
		entity->shieldsmax = entity->shields;
		alive = false;
		texture.free();
	}
}

void EffectArmor::runEffect(Entity * entity, vector<FloatingText>&texts, TTF_Font *font)
{
	if (runtime)
	{
		texture.loadFromFile("bin/images/effects/armor.png", entity->gRenderer);
		entity->armorRatingBonus += shieldamount;
		if (entity->isPlayer)
			((Player*)entity)->rebuildInventoryText();
		runtime = false;
	}
	if (currentDuration <= totalDuration)
	{
		currentDuration++;
	}
	else
	{
		entity->armorRatingBonus -= shieldamount;
		if (entity->isPlayer)
			((Player*)entity)->rebuildInventoryText();
		alive = false;
		texture.free();
	}
}

void EffectSlow::runEffect(Entity * entity, vector<FloatingText>& texts, TTF_Font * font)
{
	if (runtime)
	{
		texture.loadFromFile("bin/images/effects/slow.png", entity->gRenderer);
		slowvel = (entity->velocity + entity->velocityBonus) * slowamount;
		entity->velocityBonus -= slowvel;
		runtime = false;
	}
	if (currentDuration <= totalDuration)
	{
		currentDuration++;
	}
	else
	{
		entity->velocityBonus += slowvel;
		alive = false;
		texture.free();
	}
}


/* //Old frost shock
int shots = 5;
int spX = -shots, spY = 0;
bool xDown = false, yDown = false;

for (int i = 0; i < shots*4; i++)
{
double deltaX = (playerptr->getPosition().first + spX + (playerptr->getBox().w / 2)) - (playerptr->getPosition().first + (playerptr->getBox().w / 2));
double deltaY = (playerptr->getPosition().second + spY + (playerptr->getBox().h / 2)) - (playerptr->getPosition().second + (playerptr->getBox().h / 2));
double xAng = (atan2(deltaY, deltaX)*180.0000) / 3.1416 + 90;

double magnitude = sqrt(deltaX*deltaX + deltaY*deltaY);
double xVelB = bulletSpeed * (deltaX / magnitude);
double yVelB = bulletSpeed * (deltaY / magnitude);

double dmg = (minDamage) + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / ((maxDamage) - (minDamage))));

bullet.type = BULLET_PLAYER;
bullet.setPosition(playerptr->getBox().x + (playerptr->getBox().w / 2) - (bullet.getBox().w / 2) + (xVelB * 3),
playerptr->getBox().y + (playerptr->getBox().h / 2) - (bullet.getBox().h / 2) + (yVelB * 3));
bullet.setBullet(xVelB, yVelB, dmg, xAng, range);
bullet.spellBullet = true;

bulletptr->push_back(bullet);

if (spX == shots)
xDown = true;
else if (spX == -shots)
xDown = false;
if (spY == shots)
yDown = true;
else if (spY == -shots)
yDown = false;

if (xDown)
spX--;
else if (!xDown)
spX++;

if (yDown)
spY--;
else if (!yDown)
spY++;
}
*/