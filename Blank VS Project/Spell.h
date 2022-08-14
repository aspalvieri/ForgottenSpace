#ifndef SPELL_H
#define SPELL_H

#include "Bullet.h"
#include "Entity.h"

class Entity;

class Player;

class Enemy;

class Effect
{
public:
	Effect() {};
	Effect(int timeBetweenTick, int totalTicks) {
		(*this).timeBetweenTick = timeBetweenTick;
		(*this).totalDuration = timeBetweenTick * totalTicks;
		(*this).totalTicks = totalTicks;
	}
	~Effect() {};

	virtual void runEffect(Entity *, vector<FloatingText> &, TTF_Font*) = 0;

	virtual string getInfo() = 0;

	string description = "";
	string type = "";

	bool alive = true;
	bool runtime = true;

	int timeBetweenTick = 0, totalTicks = 0;
	int totalDuration = 0, currentDuration = 0;

	LTexture texture;
};

class EffectFire : public Effect
{
public:
	EffectFire(int timeBetweenTick, int totalTicks, double damage) : Effect(timeBetweenTick, totalTicks) {
		(*this).damage = damage;
		(*this).type = "fire";
	}

	string getInfo() {
		return "Effect: Fire\nDeals " + to_string((int)damage) + " damage every " + to_string((int)(timeBetweenTick / 60.0)) + " seconds for " + to_string((int)((timeBetweenTick * totalTicks) / 60.0)) + " seconds.";
	}

	void runEffect(Entity *entity, vector<FloatingText> &, TTF_Font*);

	double damage = 0;
};

class EffectHeal : public Effect
{
public:
	EffectHeal(int timeBetweenTick, int totalTicks, double healamount) : Effect(timeBetweenTick, totalTicks) {
		(*this).healamount = healamount;
		(*this).type = "heal";
	}

	string getInfo() {
		return "Effect: Heal\nHeals " + to_string((int)healamount) + " health every " + to_string((int)(timeBetweenTick / 60.0)) + " seconds for " + to_string((int)((timeBetweenTick * totalTicks) / 60.0)) + " seconds.";
	}

	void runEffect(Entity *entity, vector<FloatingText> &, TTF_Font*);

	double healamount = 0;
};

class EffectShield : public Effect
{
public:
	EffectShield(int timeBetweenTick, int totalTicks, double shieldamount) : Effect(timeBetweenTick, totalTicks) {
		(*this).shieldamount = shieldamount;
		(*this).type = "shield";
	}

	string getInfo() {
		return "Effect: Shield\nAbsorbs " + to_string((int)shieldamount) + " damage for " + to_string((int)((timeBetweenTick * totalTicks) / 60.0)) + " seconds.";
	}

	void runEffect(Entity *entity, vector<FloatingText> &, TTF_Font*);

	double shieldamount = 0;
};

class EffectArmor : public Effect
{
public:
	EffectArmor(int timeBetweenTick, int totalTicks, double shieldamount) : Effect(timeBetweenTick, totalTicks) {
		(*this).shieldamount = shieldamount;
		(*this).type = "armor";
	}

	string getInfo() {
		return "Effect: Armor\nIncreases armor by " + to_string((int)shieldamount) + " for " + to_string((int)((timeBetweenTick * totalTicks) / 60.0)) + " seconds.";
	}

	void runEffect(Entity *entity, vector<FloatingText> &, TTF_Font*);

	double shieldamount = 0;
};

class EffectSlow : public Effect
{
public:
	EffectSlow(int timeBetweenTick, int totalTicks, double slowamount) : Effect(timeBetweenTick, totalTicks) {
		(*this).slowamount = slowamount;
		(*this).type = "slow";
	}

	string getInfo() {
		return "Effect: Slow\nSlows movement speed by " + to_string((int)slowamount) + "% for " + to_string((int)((timeBetweenTick * totalTicks) / 60.0)) + " seconds.";
	}

	void runEffect(Entity *entity, vector<FloatingText> &texts, TTF_Font*font);

	double slowamount = 0;
	double slowvel = 0;
};

class Spell
{
public:
	Spell();
	Spell(Player &, std::vector<Enemy> &, std::vector<Bullet> &, std::vector<FloatingText> &, std::string n);
	~Spell();

	int cooldown = 0;

	int levelreq = 1;

	int cost = 0;

	int baseCost = 0;

	static int globalID;

	int id;

	int uid = -1;

	int spellType = SPELL_TYPE_NONE;

	virtual bool canCast() = 0;

	virtual void activate() = 0;

	void buildTexture(SDL_Renderer *);

	virtual void rebuildText() = 0;

	virtual void calculateValues() = 0;

	virtual bool isActive() = 0;

	std::string name = "Empty";
	Player *playerptr = NULL;
	std::vector<Enemy> *enemyptr = NULL;
	std::vector<Bullet> *bulletptr = NULL;
	std::vector<FloatingText> *textptr = NULL;

	LTexture image;
	Bullet bullet;

	SDL_Color nameColor = { 255,255,255 };

	LTexture nameTexture;
	LTexture cdTexture;
	LTexture energyTexture;
	LTexture levelText;
	LTexture descriptionText;
	TTF_Font *font = NULL;
	SDL_Renderer* gRenderer = NULL;

private:
	
};

class EmptySpell : public Spell
{
public:
	EmptySpell(Player &p, std::vector<Enemy> &e, std::vector<Bullet> &b, std::vector<FloatingText> &f, std::string n) : Spell(p, e, b, f, n) {
		cooldown = 0;
		levelreq = 1;
		baseCost = 0;
		cost = baseCost;
	}
	bool canCast();
	void rebuildText() {};
	void calculateValues() {};
	void activate();
	bool isActive();
};

class BurstShield : public Spell
{
public:
	BurstShield(Player &p, std::vector<Enemy> &e, std::vector<Bullet> &b, std::vector<FloatingText> &f, std::string n) : Spell(p, e, b, f, n) {
		cooldown = 10 * SCREEN_FPS; //60 = 1 second;
		levelreq = 1;
		baseCost = 12;
		cost = baseCost;
		nameColor = { 255,233,209 };
		spellType = SPELL_TYPE_DEFENSE;
	}
	int shieldDuration = 30 * SCREEN_FPS;
	int baseShieldAmount = 40;
	int shieldAmount = baseShieldAmount;
	bool canCast();
	void activate();
	void calculateValues();
	void rebuildText();
	bool isActive();
};

class BurstShot : public Spell
{
public:
	BurstShot(Player &p, std::vector<Enemy> &e, std::vector<Bullet> &b, std::vector<FloatingText> &f, std::string n) : Spell(p, e, b, f, n) {
		cooldown = 5 * SCREEN_FPS; //60 = 1 second;
		levelreq = 1;
		baseCost = 7;
		cost = baseCost;
		nameColor = { 128,128,255 };
		spellType = SPELL_TYPE_DAMAGE;
	}
	double bulletSpeed = 14;
	double baseMinDamage = 11;
	double baseMaxDamage = 23;
	double minDamage = baseMinDamage;
	double maxDamage = baseMaxDamage;
	double lifeSteal = 0.35;
	double range = (int)(8.0 * TILE_SIZE);
	bool canCast();
	void activate();
	void calculateValues();
	void rebuildText();
	bool isActive();
};

class FrostShock : public Spell
{
public:
	FrostShock(Player &p, std::vector<Enemy> &e, std::vector<Bullet> &b, std::vector<FloatingText> &f, std::string n) : Spell(p, e, b, f, n) {
		cooldown = 7 * SCREEN_FPS; //60 = 1 second;
		levelreq = 3;
		baseCost = 7;
		cost = baseCost;
		nameColor = { 128,175,255 };
		spellType = SPELL_TYPE_DAMAGE;
	}
	double slowPercent = 0.40;
	double slowTime = 5 * SCREEN_FPS;
	double bulletSpeed = 13;
	double baseMinDamage = 8;
	double baseMaxDamage = 15;
	double minDamage = baseMinDamage;
	double maxDamage = baseMaxDamage;
	double range = (int)(7.0 * TILE_SIZE);
	bool canCast();
	void activate();
	void calculateValues();
	void rebuildText();
	bool isActive();
};

class GlimmerofLight : public Spell
{
public:
	GlimmerofLight(Player &p, std::vector<Enemy> &e, std::vector<Bullet> &b, std::vector<FloatingText> &f, std::string n) : Spell(p, e, b, f, n) {
		cooldown = 1 * SCREEN_FPS; //60 = 1 second;
		levelreq = 3;
		baseCost = 23;
		cost = baseCost;
		nameColor = { 186,255,130 };
		spellType = SPELL_TYPE_DEFENSE;
	}
	double baseHealAmount = 7;
	double healAmount = baseHealAmount; //Heal per tick
	int timeBetweenTicks = 1 * SCREEN_FPS;
	int ticks = 10;
	bool canCast();
	void activate();
	void calculateValues();
	void rebuildText();
	bool isActive();
};

class GuardianShield : public Spell
{
public:
	GuardianShield(Player &p, std::vector<Enemy> &e, std::vector<Bullet> &b, std::vector<FloatingText> &f, std::string n) : Spell(p, e, b, f, n) {
		cooldown = 1 * SCREEN_FPS; //60 = 1 second;
		levelreq = 3;
		baseCost = 23;
		cost = baseCost;
		nameColor = { 150,150,150 };
		spellType = SPELL_TYPE_DEFENSE;
	}
	int shieldDuration = 30 * SCREEN_FPS;
	int baseShieldAmount = 15;
	int shieldAmount = baseShieldAmount;
	bool canCast();
	void activate();
	void calculateValues();
	void rebuildText();
	bool isActive();
};

class BurstShotBoss : public Spell
{
public:
	BurstShotBoss(Player &p, std::vector<Enemy> &e, std::vector<Bullet> &b, std::vector<FloatingText> &f, std::string n) : Spell(p, e, b, f, n) {}
	double bulletSpeed = 8;
	double minDamage = 15;
	double maxDamage = 29;
	double range = (int)(7.0 * TILE_SIZE);
	bool canCast() { return false; }
	void rebuildText() {};
	void calculateValues() {};
	void activate();
	bool isActive();
};

#endif//SPELL_H