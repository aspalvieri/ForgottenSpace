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

void handleHit(Bullet &bullet, std::vector<Sprite> &sprites, std::vector<Enemy> &enemies, Player &player)
{
	//Check collision between every sprite
	for (unsigned int i = 0; i < sprites.size(); i++)
	{
		if (bullet.hitWall)
			break;

		if (checkCollision(bullet.aiCamera, sprites[i].getBox()) == true && sprites[i].getCollide() == true && checkCollision(bullet.Sprite::getBox(), sprites[i].getBox()) == true)
		{
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
				double reduc = (enemies[i].armorRating * pow(1.0 / 2.0, (double)player.level / 2.0)) / 100.0;
				if (reduc > BULLET_REDUC)
					reduc = BULLET_REDUC;
				double bDamage = bullet.damage - (bullet.damage * reduc);
				if (bDamage <= 0)
					bDamage = 0;
				enemies[i].wallWasHit = false;
				enemies[i].health -= bDamage;
				if (player.targetEnemy != NULL)
				{
					delete player.targetEnemy;
					player.targetEnemy = NULL;
				}
				player.targetEnemy = new Enemy(enemies[i]);
				player.targetTimer = 0;
				bullet.alive = false;
				bullet.setAnimation("Die");
				return;
			}
		}
	}
	if (bullet.hitWall)
		return;
	
	//Check collision between player
	if (bullet.type == BULLET_ENEMY)
	{
		if (checkCollision(bullet.Sprite::getBox(), player.getBox()) == true)
		{
			int ei = -1;
			for (unsigned int i = 0; i < enemies.size(); i++)
			{
				if (enemies[i].uniqueID == bullet.shooterID)
				{
					ei = i;
					enemies[i].wallWasHit = false;
					break;
				}
			}
			if (ei == -1)
				return;
			double reduc = (player.armorRating * pow(1.0 / 2.0, (double)enemies[ei].level / 2.0)) / 100.0;
			if (reduc > BULLET_REDUC)
				reduc = BULLET_REDUC;
			double bDamage = bullet.damage - (bullet.damage * reduc);
			if (bDamage <= 0)
				bDamage = 0;
			player.health -= bDamage;
			bullet.alive = false;
			bullet.setAnimation("Die");
			return;
		}
	}
}
