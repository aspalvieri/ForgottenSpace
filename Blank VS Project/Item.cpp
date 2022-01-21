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

void Item::buildTooltip(int pS)
{
	if (name != "" && tooltip != "")
	{
		font = TTF_OpenFont("bin/fonts/codenewroman.ttf", 16);
		if (font == NULL)
			std::cout << "Item::buildTooltip() Font was NULL!\n";
		if (itemType == ITEM_CHEST || itemType == ITEM_HEAD || itemType == ITEM_LEGS)
		{
			isarmor = true;
			armorIcon.loadFromFile("bin/images/armoricon.png", gRenderer);
			armorSpace = 9 * 7 + (std::to_string((int)armor.armorRating).length() * 9);
		}
		plasmaIcon.loadFromFile("bin/images/plasmaicon.png", gRenderer);
		for (size_t i = 0; i < tooltip.length(); i++)
			if (tooltip[i] == '\n')
				pYSpace++;
		pYSpace = (pYSpace * 17) + 11;
		plasmaSpace = 9 * (7 + pS) + (std::to_string((int)value).length() * 9);
		tooltipName.loadFromRenderedText(gRenderer, name, rarity, font, 400);
		tooltipText.loadFromRenderedText(gRenderer, tooltip, SDL_Color{ 255,255,255 }, font, 400);
	}
}

double calculateDPS(Weapon weapon)
{
	double dps = (weapon.minDamage + ((weapon.maxDamage - weapon.minDamage) / 2)) * (60.0 / (double)weapon.reload);
	return dps;
}

void Item::buildResource(string type, SDL_Color cRarity)
{
	tooltip = "\n Type: " + type + "\nValue: " + to_string(value);
	rarity = cRarity;
	tooltipSize = 3 * 20 + 10;
	int spacer = 8 + to_string(value).length();
	if (("Type: "+type).length() > spacer) spacer = ("Type: " + type).length();
	if (name.length() > spacer) spacer = name.length();
	tooltipWidth = spacer*9 + 28;
	buildTooltip();
}

void Item::buildWeapon(Weapon wp, int lvreq, SDL_Color cRarity)
{
	weapon = wp; levelreq = lvreq;
	stringstream placeText;
	placeText << "\n  Type: Weapon" << setprecision(1) << fixed << "\n   DPS: " << calculateDPS(weapon);
	placeText << "\nEnergy: " << (weapon.energyCost * (60.0 / (double)weapon.reload)) << "/s\n Range: " << ((double)weapon.range / TILE_SIZE);
	placeText << " Tiles" << setprecision(0) << "\n Level: " << levelreq << "\n Value: " << value;
	tooltip = placeText.str();
	rarity = cRarity;
	tooltipSize = 7 * 20;
	int spacer = 15 + to_string((int)floor((double)weapon.range / TILE_SIZE)).length();
	if (name.length() > spacer) spacer = name.length();
	tooltipWidth = spacer*9 + 28;
	buildTooltip(1);
}

void Item::buildPotion(int ha, int ea, SDL_Color cRarity)
{
	potion.healthAmount = ha;
	potion.energyAmount = ea;
	int tabs = (bool)ha + (bool)ea;
	tooltip = "\n   Type: Potion\n";
	if (ha > 0) tooltip += "Restore: " + to_string(ha) + " Health\n";
	if (ea > 0) tooltip += "Restore: " + to_string(ea) + " Energy\n";
	tooltip += "    Use: Right-Click\n  Value: " + to_string(value);
	rarity = cRarity;
	tooltipSize = (4 + tabs) * 20 + 4;
	int spacer = 19;
	if (name.length() > spacer) spacer = name.length();
	tooltipWidth = spacer * 9 + 28;
	buildTooltip(2);
}

void Item::buildArmor(int ar, int lvreq, string type, SDL_Color aColor, SDL_Color cRarity)
{
	armor.armorRating = ar; levelreq = lvreq;
	armor.color = aColor;
	tooltip = "\n Type: Armor - " + type + "\nArmor: " + to_string(ar) + "\nLevel: " + to_string(lvreq);
	tooltip += "\nValue: " + to_string(value);
	rarity = cRarity;
	tooltipSize = 5 * 20 + 4;
	int spacer = 15 + type.length();
	if (name.length() > spacer) spacer = name.length();
	tooltipWidth = spacer * 9 + 20;
	buildTooltip();
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
	name = "Empty";
	value = 0;

	box.x = 0;
	box.y = 0;
	box.w = ITEM_INV_SIZE;
	box.h = ITEM_INV_SIZE;

	LTexture::createBlank(box.w, box.h, SDL_TEXTUREACCESS_STREAMING, gRenderer);
}
