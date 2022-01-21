#include "Tile.h"

Light::Light()
{
}

Light::Light(int v1, int v2, double dis, double str)
{
	x = v1;
	y = v2;
	distance = dis;
	strength = str;
	buildBounds();
}

void Light::buildBounds()
{
	int holdv = (distance * 10.0);
	holdv = ((holdv % 2 == 0) ? holdv + 1 : holdv) + 1;
	bbox = { (int)x - (holdv / 2) * TILE_SIZE, (int)y - (holdv / 2) * TILE_SIZE, (holdv)* TILE_SIZE, (holdv)* TILE_SIZE };
	bbound = holdv;
}

Tile::Tile()
{
	tSize = TILE_SIZE;

	sclip = { 0, 0, 50, 50 };
	oclip = { 0, 0, 50, 50 };

	inUse = false;
	type = 0;
}

Tile::Tile(int posX, int posY, SDL_Rect c, bool col, LTexture t, LTexture s, LTexture o, SDL_Rect& cam)
{
	tSize = TILE_SIZE;

	box.x = posX;
	box.y = posY;
	box.w = tSize;
	box.h = tSize;

	inUse = false;
	type = 0;

	camera = &cam;
	mapTile = true;
	collide = col;
	clip = c;

	sclip = { 0, 0, 50, 50 };
	oclip = { 0, 0, 50, 50 };

	texture = t;
	texture.setScale((double)tSize / (double)clip.w, (double)tSize / (double)clip.h);

	shadow = s;
	shadow.setScale((double)tSize / (double)clip.w, (double)tSize / (double)clip.h);

	object = o;
	object.setScale((double)tSize / (double)clip.w, (double)tSize / (double)clip.h);
}

Tile::Tile(int posX, int posY, SDL_Rect c, bool col, LTexture t)
{
	tSize = MAP_TILE_SIZE;

	box.x = posX;
	box.y = posY;
	box.w = tSize;
	box.h = tSize;

	inUse = false;
	type = 0;

	camera = NULL;
	collide = col;
	clip = c;
	texture = t;
	texture.setScale((double)tSize / (double)clip.w, (double)tSize / (double)clip.h);

	dbclip = { clip.x + 200, clip.y, clip.w, clip.h };
	dclip = { clip.x + 150, clip.y, clip.w, clip.h };
	ruclip = { clip.x + 100, clip.y, clip.w, clip.h };
	rclip = { clip.x + 50, clip.y, clip.w, clip.h };
}

Tile::~Tile()
{
}

Tile & Tile::setCollide(bool col)
{
	collide = col;
	return *this;
}

bool Tile::getCollide()
{
	return collide;
}

bool Tile::hasCamera()
{
	if (camera)
		return true;
	return false;
}

bool Tile::checkCollision(SDL_Rect &B)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = box.x;
	rightA = box.x + box.w;
	topA = box.y;
	bottomA = box.y + box.h;

	//Calculate the sides of rect B
	leftB = B.x;
	rightB = B.x + B.w;
	topB = B.y;
	bottomB = B.y + B.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

void Tile::setBox(SDL_Rect newBox)
{
	box = newBox;
}

SDL_Rect Tile::getBox()
{
	return box;
}

void Tile::draw(SDL_Renderer *gRenderer, Uint8 &alpha)
{
	if (mapTile)
	{
		texture.render(gRenderer, box.x - camera->x, box.y - camera->y, &clip);
		if (sclip.x != 0 || sclip.y != 0)
		{
			if (!collide)
			{
				shadow.setAlpha(255 - alpha);
			}
			else
			{
				shadow.setAlpha(255);
			}
			shadow.render(gRenderer, box.x - camera->x, box.y - camera->y, &sclip);
		}
		if (oclip.x != 0 || oclip.y != 0)
		{
			object.render(gRenderer, box.x - camera->x, box.y - camera->y, &oclip);
		}
	}
	else
	{
		if (inUse)
		{
			if (discovered && bossTile == false)
			{
				texture.render(gRenderer, box.x, box.y, &dclip);
			}
			else if (discovered && bossTile)
			{
				texture.render(gRenderer, box.x, box.y, &dbclip);
			}
			else if (inrange && inUse)
			{
				texture.render(gRenderer, box.x, box.y, &ruclip);
			}
			else
				texture.render(gRenderer, box.x, box.y, &clip);
		}
		else if (inrange && inUse == false)
		{
			texture.render(gRenderer, box.x, box.y, &rclip);
		}
		else
			texture.render(gRenderer, box.x, box.y, &clip);
	}
}

void Tile::setTexture(LTexture tm)
{
	texture = tm;
	texture.setScale((double)tSize / (double)clip.w, (double)tSize / (double)clip.h);
}

bool withinRange(int a, int b, int space)
{
	if (b + space >= a && b - space <= a)
		return true;
	return false;
}

Shade::Shade()
{
}

Shade::Shade(int posX, int posY, LTexture b, SDL_Rect &cam)
{
	box.x = posX;
	box.y = posY;
	box.w = TILE_HALF;
	box.h = TILE_HALF;

	camera = &cam;

	black = b;
	black.setScale((double)TILE_HALF / (double)1, (double)TILE_HALF / (double)1);
}

Shade::~Shade()
{
}


bool Shade::checkCollision(SDL_Rect &B)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = box.x;
	rightA = box.x + box.w;
	topA = box.y;
	bottomA = box.y + box.h;

	//Calculate the sides of rect B
	leftB = B.x;
	rightB = B.x + B.w;
	topB = B.y;
	bottomB = B.y + B.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

void Shade::renderLight(SDL_Renderer *gRenderer)
{
	if (alpha > 0)
	{
		black.setAlpha(alpha);
		black.render(gRenderer, box.x - camera->x, box.y - camera->y);
	}
}

void Shade::calculateLight(vector<Light>& lights/*, Tile tiles[TOTAL_TILES_X][TOTAL_TILES_Y], int &maxCamY, int &maxCamH, int &maxCamX, int &maxCamW*/, Uint8 &maxAlpha)
{
	int currMax = maxAlpha;
	int x = box.x;
	int y = box.y;
	for (unsigned int i = 0; i < lights.size(); i++)
	{
		if (checkCollision(lights[i].bbox))
		{
			double diffx = abs((x + 12.5) - lights[i].x);
			double diffy = abs((y + 12.5) - lights[i].y);
			double diff = sqrt((diffx*diffx) + (diffy*diffy));
			double amt = min(diff / lights[i].strength, (double)maxAlpha);
			if (amt < maxAlpha)
			{
				amt = amt / (lights[i].strength - min(max((diff / 250), 0.0), lights[i].strength));
				if (amt > maxAlpha)
					amt = maxAlpha;
			}
			//Old
			currMax = min((int)amt, currMax);

			//New
			/*
			int reduc = maxAlpha - amt;
			currMax -= reduc;
			if (currMax < 0)
				currMax = 0;
			*/

			//Enable for Wall Blocking (but not really since it needs re-working)
			/*
			if (amt < maxAlpha)
			{
				collidedy = false, collidedx = false;
				for (int yy = maxCamY; yy <= maxCamH; yy++)
				{
					for (int xx = maxCamX; xx <= maxCamW; xx++)
					{
						if (tiles[xx][yy].id != id && tiles[xx][yy].collide && (tiles[xx][yy].sclip.x != 0 || tiles[xx][yy].sclip.y != 0))
						{
							int x2 = lights[i].x, y2 = lights[i].y;
							int closex = x + TILE_HALF, closey = y + TILE_HALF;
							if (y - 1 > lights[i].y)
							{
								closey = y - 1;
								closex = x + TILE_HALF;
							}
							else if (y + TILE_SIZE <= lights[i].y)
							{
								closey = y + TILE_SIZE;
								closex = x + TILE_HALF;
							}
							if (SDL_IntersectRectAndLine(&tiles[xx][yy].box, &closex, &closey, &x2, &y2))
							{
								collidedy = true;
							}

							closex = x + TILE_HALF, closey = y + TILE_HALF;
							x2 = lights[i].x, y2 = lights[i].y;
							if (x - 1 > lights[i].x)
							{
								closex = x - 1;
								closey = y + TILE_HALF;
							}
							else if (x + TILE_SIZE <= lights[i].x)
							{
								closex = x + TILE_SIZE;
								closey = y + TILE_HALF;
							}
							if (SDL_IntersectRectAndLine(&tiles[xx][yy].box, &closex, &closey, &x2, &y2))
							{
								collidedx = true;
							}
							if (collidedx && collidedy)
								amt = maxAlpha;
						}
					}
				}
			}
			*/
		}
	}

	alpha = currMax;
}
