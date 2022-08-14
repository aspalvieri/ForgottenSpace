#include "Item.h"

Item::Item()
{
	zeroZero = { 0, 0, 40, 40 };

	alive = false;
	hovered = false;

	itemType = ITEM_NONE;
	name = "Empty";
	value = 0;

	box.x = 0;
	box.y = 0;
	box.w = ITEM_INV_SIZE;
	box.h = ITEM_INV_SIZE;
}

Item::Item(SDL_Renderer *gR, SDL_Window *gW, SDL_Rect c, int type, LTexture t, SDL_Rect& cam, int val, std::string n)
{
	gRenderer = gR;
	gWindow = gW;

	zeroZero = { 0, 0, 40, 40 };

	alive = true;
	hovered = false;
	name = n;
	value = val;

	itemType = type;

	box.x = 0;
	box.y = 0;
	box.w = ITEM_MAP_SIZE;
	box.h = ITEM_MAP_SIZE;

	camera = &cam;
	clip = c;
	LTexture::setTexture(t);
	LTexture::setScale((double)ITEM_MAP_SIZE / (double)clip.w, (double)ITEM_MAP_SIZE / (double)clip.h);

	weapon.sprite.setRender(gR, gW);
	weapon.bullet.setRender(gR, gW);
}

Item::~Item()
{
}

void Item::draw()
{
	if (camera)
	{
		if (alive)
			LTexture::render(gRenderer, box.x - camera->x, box.y - camera->y, &zeroZero);
		LTexture::render(gRenderer, box.x - camera->x, box.y - camera->y, &clip);
	}
	else
	{
		if (alive)
			LTexture::render(gRenderer, box.x, box.y, &zeroZero);
		LTexture::render(gRenderer, box.x, box.y, &clip);
	}
}

void Item::delCamera()
{
	camera = NULL;
}

void Item::setScale(double w, double h)
{
	LTexture::setScale(w, h);
	box.w = (int)((double)box.w * w);
	box.h = (int)((double)box.h * h);
}

void Item::setPosition(int x, int y)
{
	box.x = x;
	box.y = y;
}

LTexture Item::getTexture()
{
	return LTexture::getTexture();
}

SDL_Rect Item::getClip()
{
	return clip;
}

SDL_Rect Item::getBox()
{
	return box;
}

void Item::setBox(int w, int h)
{
	box.w = w;
	box.h = h;
}

void Item::clearSlot()
{
	alive = false;
	hovered = false;

	itemType = ITEM_NONE;
	name = "";
	value = 0;

	box.x = 0;
	box.y = 0;
	box.w = ITEM_INV_SIZE;
	box.h = ITEM_INV_SIZE;

	LTexture::createBlank(box.w, box.h, SDL_TEXTUREACCESS_STREAMING, gRenderer);
}
