bool bulletToDie(const Bullet &b) 
{
	return !b.alive;
}

bool enemyToDie(const Enemy &e)
{
	return !e.alive;
}

bool itemToDie(const Item &i)
{
	return !i.alive;
}

void addEffect(vector<Effect*> &effects, Effect* effect)
{
	if (effect->type == "fire")
		effects.push_back(new EffectFire(effect->timeBetweenTick, effect->totalTicks, ((EffectFire*)effect)->damage));
	if (effect->type == "heal")
		effects.push_back(new EffectHeal(effect->timeBetweenTick, effect->totalTicks, ((EffectHeal*)effect)->healamount));
	if (effect->type == "shield")
		effects.push_back(new EffectShield(effect->timeBetweenTick, 1, ((EffectShield*)effect)->shieldamount));
	if (effect->type == "armor")
		effects.push_back(new EffectArmor(effect->timeBetweenTick, 1, ((EffectArmor*)effect)->shieldamount));
	if (effect->type == "slow")
		effects.push_back(new EffectSlow(effect->timeBetweenTick, 1, ((EffectSlow*)effect)->slowamount));
}

void handleHit(Bullet &bullet, std::vector<Sprite> &sprites, std::vector<Enemy> &enemies, Player &player)
{
	//Check collision between every sprite
	for (unsigned int i = 0; i < sprites.size(); i++)
	{
		if (bullet.hitWall)
			break;

		if (checkCollision(bullet.aiCamera, sprites[i].getBox()) == true && sprites[i].getCollide() == true && checkCollision(bullet.Sprite::getBox(), sprites[i].getBox()) == true)
		{
			if (bullet.type == BULLET_ENEMY) bullet.hitWall = true;
			bullet.alive = false;
			bullet.setAnimation("Die");
			return;
		}
	}

	//Check collision between every enemy
	for (unsigned int i = 0; i < enemies.size(); i++)
	{
		if (!bullet.hitWall && bullet.type == BULLET_ENEMY)
		{
			if (checkCollision(bullet.aiCamera, enemies[i].getBox()) == true && enemies[i].aiType == ENEMY_TYPE_NODE && enemies[i].getCollide() == true && checkCollision(bullet.Sprite::getBox(), enemies[i].getBox()) == true)
			{
				bullet.hitWall = true;
				bullet.alive = false;
				bullet.setAnimation("Die");
				return;
			}
		}
		if (bullet.hitWall && bullet.shooterID != -1)
		{
			if (enemies[i].uniqueID == bullet.shooterID)
			{
				enemies[i].wallWasHit = true;
				return;
			}
			continue;
		}
		else if (bullet.type == BULLET_PLAYER)
		{
			if (checkCollision(bullet.aiCamera, enemies[i].getBox()) == true && enemies[i].getCollide() == true && checkCollision(bullet.Sprite::getBox(), enemies[i].getBox()) == true)
			{
				double reduc = (enemies[i].armorRating * pow(1.0 / 2.0, (double)player.level / 7.5)) / 100.0;
				if (reduc > BULLET_REDUC)
					reduc = BULLET_REDUC;
				double bDamage = bullet.damage - (bullet.damage * reduc);
				if (bDamage <= 0)
					bDamage = 0;
				double lifebonus = 0;
				if (bullet.lifesteal > 0)
				{
					lifebonus = bDamage * bullet.lifesteal;
					double potentialheal = lifebonus;
					player.health += lifebonus;
					if (player.health > player.maxHealth)
					{
						potentialheal -= (player.health - player.maxHealth);
						player.health = player.maxHealth;
					}
					texts.push_back(FloatingText("+"+std::to_string((int)round(potentialheal)), player.getBox().x + PLAYER_SIZE / 2, player.getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman18, SDL_Color{ 0,255,0 }));
				}
				enemies[i].wallWasHit = false;
				enemies[i].playerHitMe = true;
				enemies[i].health -= bDamage;
				for (unsigned int v = 0; v < bullet.effects.size(); v++)
					addEffect(enemies[i].effects, bullet.effects[v]);
				if (player.targetEnemy != NULL)
				{
					delete player.targetEnemy;
					player.targetEnemy = NULL;
				}
				player.targetEnemy = new Enemy(enemies[i]);
				player.inCombat = true;
				player.combatTimer = 0;
				player.targetTimer = 0;
				bullet.alive = false;
				bullet.setAnimation("Die");

				if (!bullet.spellBullet)
					texts.push_back(FloatingText(std::to_string((int)round(bDamage)), enemies[i].getBox().x + enemies[i].getBox().w / 2, enemies[i].getBox().y + enemies[i].getBox().h / 4, enemies[i].getBox().w, CodeNewRoman18, SDL_Color{ 255,255,255 }));
				else
					texts.push_back(FloatingText(std::to_string((int)round(bDamage)), enemies[i].getBox().x + enemies[i].getBox().w / 2, enemies[i].getBox().y + enemies[i].getBox().h / 4, enemies[i].getBox().w, CodeNewRoman18, SDL_Color{ 255,217,0 }));

				return;
			}
		}
	}
	if (bullet.hitWall)
		return;
	
	bool enemyAlive = false;
	int eID = -1;
	//Check collision between player
	if (bullet.type == BULLET_ENEMY)
	{
		if (checkCollision(bullet.Sprite::getBox(), player.getBox()) == true)
		{
			for (unsigned int i = 0; i < enemies.size(); i++)
			{
				if (enemies[i].uniqueID == bullet.shooterID)
				{
					enemyAlive = true;
					eID = i;
					enemies[i].wallWasHit = false;
					break;
				}
			}
			double reduc = ((player.armorRating + player.armorRatingBonus) * pow(1.0 / 2.0, (double)bullet.level / 7.5)) / 100.0;
			if (reduc > BULLET_REDUC)
				reduc = BULLET_REDUC;
			double bDamage = bullet.damage - (bullet.damage * reduc);
			if (bDamage <= 0)
				bDamage = 0;
			double lifebonus = 0;
			if (enemyAlive && bullet.lifesteal > 0)
			{
				lifebonus = bDamage * bullet.lifesteal;
				double potentialheal = lifebonus;
				enemies[eID].health += lifebonus;
				if (enemies[eID].health > enemies[eID].maxHealth)
				{
					potentialheal -= (enemies[eID].health - enemies[eID].maxHealth);
					enemies[eID].health = enemies[eID].maxHealth;
				}
				if (player.targetEnemy != NULL)
				{
					if (player.targetEnemy->uniqueID == enemies[eID].uniqueID)
					{
						delete player.targetEnemy;
						player.targetEnemy = new Enemy(enemies[eID]);
					}
				}
				texts.push_back(FloatingText(std::to_string((int)round(potentialheal)), enemies[eID].getBox().x + enemies[eID].getBox().w / 2, enemies[eID].getBox().y + enemies[eID].getBox().h / 4, enemies[eID].getBox().w, CodeNewRoman18, SDL_Color{ 0,255,0 }));
			}
			player.takeDamage(bDamage);
			player.inCombat = true;
			player.combatTimer = 0;
			for (unsigned int v = 0; v < bullet.effects.size(); v++)
				addEffect(player.effects, bullet.effects[v]);
			bullet.alive = false;
			bullet.setAnimation("Die");

			texts.push_back(FloatingText(std::to_string((int)round(bDamage)), player.getBox().x + PLAYER_SIZE / 2, player.getBox().y + PLAYER_SIZE / 4, PLAYER_SIZE, CodeNewRoman18, SDL_Color{ 255,0,0 }));

			return;
		}
	}
}
