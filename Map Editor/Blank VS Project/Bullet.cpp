#include "Bullet.h"

Bullet::Bullet()
{
	aiCamera = { 0, 0, 150, 150 };
	distance = 0;
	maxDistance = 0;
}

Bullet::Bullet(SDL_Renderer * gR, SDL_Window * gW) : Sprite(gR, gW)
{
	gRenderer = gR;
	gWindow = gW;

	aiCamera = { 0, 0, 150, 150 };
	distance = 0;
	maxDistance = 0;
}

Bullet::~Bullet()
{
}

void Bullet::setBullet(double x, double y, double d, double a, double md)
{
	dirX = x;
	dirY = y;
	damage = d;
	angle = a;
	maxDistance = md;
	Sprite::setAngle(angle);
}

void Bullet::handleMove(Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y])
{
	setCamera();
	if (distance >= maxDistance)
	{
		alive = false;
		Sprite::setAnimation("Time");
		return;
	}
	double xPos = Sprite::getPosition().first + dirX;
	double yPos = Sprite::getPosition().second + dirY;

	if (std::isnan(xPos) == false && std::isnan(yPos) == false)
		Sprite::setPosition(xPos, yPos);

	if (std::isnan(xPos) == false && std::isnan(yPos) == false)
	{
		distance += sqrt((abs(dirX)*abs(dirX)) + (abs(dirY)*abs(dirY)));
	}
	else
	{
		alive = false;
		Sprite::setAnimation("Time");
		return;
	}
	

	maxCamY = (int)ceil(aiCamera.y / TILE_SIZE);
	maxCamH = (int)(((ceil(aiCamera.y / TILE_SIZE) + ceil(aiCamera.h / TILE_SIZE)) < TOTAL_TILES_Y - 1) ? ceil(aiCamera.y / TILE_SIZE) + ceil(aiCamera.h / TILE_SIZE) + 1 : ceil(aiCamera.y / TILE_SIZE) + ceil(aiCamera.h / TILE_SIZE));
	maxCamX = (int)ceil(aiCamera.x / TILE_SIZE);
	maxCamW = (int)(((ceil(aiCamera.x / TILE_SIZE) + ceil(aiCamera.w / TILE_SIZE)) < TOTAL_TILES_X - 1) ? ceil(aiCamera.x / TILE_SIZE) + ceil(aiCamera.w / TILE_SIZE) + 1 : ceil(aiCamera.x / TILE_SIZE) + ceil(aiCamera.w / TILE_SIZE));

	//Check collision between every tile
	for (unsigned int y = maxCamY; y <= maxCamH; y++)
	{
		for (unsigned int x = maxCamX; x <= maxCamW; x++)
		{
			if (tiles[x][y].getCollide() == true && checkCollision(Sprite::getBox(), tiles[x][y].getBox()) == true)
			{
				if (shooterID != -1)
					hitWall = true;
				alive = false;
				Sprite::setAnimation("Die");
				return;
			}
		}
	}
}

void Bullet::draw()
{
	if (Sprite::getCurrentAnimation() == "Die")
	{
		if (Sprite::getSize().first < Sprite::getSize().second)
			Sprite::setSize(Sprite::getSize().second, Sprite::getSize().second);
		else if (Sprite::getSize().first > Sprite::getSize().second)
			Sprite::setSize(Sprite::getSize().first, Sprite::getSize().first);
	}
	Sprite::draw();
}

void Bullet::setCamera()
{
	aiCamera.x = (Sprite::getBox().x + (Sprite::getBox().w / 2) - (aiCamera.w / 2));
	aiCamera.y = (Sprite::getBox().y + (Sprite::getBox().h / 2) - (aiCamera.h / 2));

	if (aiCamera.x < 0)
		aiCamera.x = 0;

	if (aiCamera.y < 0)
		aiCamera.y = 0;

	if (aiCamera.x > FIELD_WIDTH - aiCamera.w)
		aiCamera.x = FIELD_WIDTH - aiCamera.w;

	if (aiCamera.y > FIELD_HEIGHT - aiCamera.h)
		aiCamera.y = FIELD_HEIGHT - aiCamera.h;
}
