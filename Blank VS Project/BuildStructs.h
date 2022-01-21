void buildStructs()
{
	//Bullet
	laser.setRender(gRenderer, gWindow);
	laser.camera = &camera;
	laser.loadSpriteImage("bin/images/bulletsheet.png")
		.setFrameSize(40, 40)
		.setPosition(0, 0)
		.setSize(10, 20)
		.setDelay(1)
		.pushFrameRow("Idle", 0, 0, 40, 0, 4) //Standard for when moving
		.pushFrameRow("Time", 0, 40, 40, 0, 4) //If bullet timed out (reached travel distance)
		.pushFrameRow("Die", 0, 80, 40, 0, 5) //If bullet hit something
		.setAnimation("Idle");

	//Bullet
	slimeball.setRender(gRenderer, gWindow);
	slimeball.camera = &camera;
	slimeball.loadSpriteImage("bin/images/slimeballsheet.png")
		.setFrameSize(40,40)
		.setPosition(0,0)
		.setSize(15,15)
		.setDelay(1)
		.pushFrameRow("Idle", 0, 0, 40, 0, 4) //Standard for when moving
		.pushFrameRow("Time", 0, 40, 40, 0, 4) //If bullet timed out (reached travel distance)
		.pushFrameRow("Die", 0, 80, 40, 0, 5) //If bullet hit something
		.setAnimation("Idle");

	//Bullet
	wispball.setRender(gRenderer, gWindow);
	wispball.camera = &camera;
	wispball.loadSpriteImage("bin/images/wispballsheet.png")
		.setFrameSize(40, 40)
		.setPosition(0, 0)
		.setSize(15, 15)
		.setDelay(1)
		.pushFrameRow("Idle", 0, 0, 40, 0, 4) //Standard for when moving
		.pushFrameRow("Time", 0, 40, 40, 0, 4) //If bullet timed out (reached travel distance)
		.pushFrameRow("Die", 0, 80, 40, 0, 5) //If bullet hit something
		.setAnimation("Idle");

	//Bullet
	bsbullet.setRender(gRenderer, gWindow);
	bsbullet.camera = &camera;
	bsbullet.loadSpriteImage("bin/images/abilities/burstshotbullet.png")
		.setFrameSize(40, 40)
		.setPosition(0, 0)
		.setSize(20, 15)
		.setDelay(1)
		.pushFrameRow("Idle", 0, 0, 40, 0, 4) //Standard for when moving
		.pushFrameRow("Time", 0, 40, 40, 0, 4) //If bullet timed out (reached travel distance)
		.pushFrameRow("Die", 0, 80, 40, 0, 5) //If bullet hit something
		.setAnimation("Idle");

	//Bullet
	arrow.setRender(gRenderer, gWindow);
	arrow.camera = &camera;
	arrow.loadSpriteImage("bin/images/arrowsheet.png")
		.setFrameSize(13, 26)
		.setPosition(0, 0)
		.setSize(13, 26)
		.setDelay(1)
		.pushFrame("Idle", 0, 0)
		.pushFrameRow("Time", 0, 26, 13, 0, 4)
		.pushFrameRow("Die", 0, 52, 13, 0, 4)
		.setAnimation("Idle");

	//Weapon
	defaultGun.bullet = laser;
	defaultGun.sprite.setRender(gRenderer, gWindow);
	defaultGun.sprite.camera = &camera;
	defaultGun.sprite.loadSpriteImage("bin/images/defaultGun.png")
		.setFrameSize(20, 25)
		.setPosition(0, 0)
		.setSize(20, 25)
		.setDelay(3)
		.setCenter(PLAYER_SIZE / 2.0 - 10, PLAYER_SIZE / 2.0 + 13)
		.pushFrame("Idle", 0, 0)
		.pushFrameRow("Shoot", 20, 0, 20, 0, 3)
		.setAnimation("Idle");
	defaultGun.energyCost = 0.9;
	defaultGun.minDamage = 3.5;
	defaultGun.maxDamage = 7.5;
	defaultGun.range = (int)(7.0 * TILE_SIZE); //Max pixels to travel
	defaultGun.reload = 20; //60 = 1 second
	defaultGun.speed = 15; //Pixels per frame
	defaultGun.name = "Default Laser";

	//Weapon
	speedGun.bullet = laser;
	speedGun.sprite.setRender(gRenderer, gWindow);
	speedGun.sprite.camera = &camera;
	speedGun.sprite.loadSpriteImage("bin/images/speedGun.png")
		.setFrameSize(20, 25)
		.setPosition(0, 0)
		.setSize(20, 25)
		.setCenter(PLAYER_SIZE / 2.0 - 10, PLAYER_SIZE / 2.0 + 13)
		.pushFrame("Idle", 0, 0)
		.pushFrameRow("Shoot", 20, 0, 20, 0, 3)
		.setAnimation("Idle");
	speedGun.energyCost = 0.56;
	speedGun.minDamage = 1.8;
	speedGun.maxDamage = 3.6;
	speedGun.range = (int)(5.5 * TILE_SIZE); //Max pixels to travel
	speedGun.reload = 7; //60 = 1 second
	speedGun.speed = 18; //Pixels per frame
	speedGun.name = "Speed Rifle";

	//Weapon
	longGun.bullet = laser;
	longGun.sprite.setRender(gRenderer, gWindow);
	longGun.sprite.camera = &camera;
	longGun.sprite.loadSpriteImage("bin/images/longGun.png")
		.setFrameSize(20, 25)
		.setPosition(0, 0)
		.setSize(20, 25)
		.setCenter(PLAYER_SIZE / 2.0 - 10, PLAYER_SIZE / 2.0 + 13)
		.pushFrame("Idle", 1, 1)
		.pushFrameRow("Shoot", 21, 1, 20, 0, 3)
		.setAnimation("Idle");
	longGun.energyCost = 3.75;
	longGun.minDamage = 14.5;
	longGun.maxDamage = 19.5;
	longGun.range = (int)(11.0 * TILE_SIZE); //Max pixels to travel
	longGun.reload = 34; //60 = 1 second
	longGun.speed = 22; //Pixels per frame
	longGun.name = "Energy Cannon";

	//Enemy Weapon
	slimeGun.bullet = slimeball;
	slimeGun.minDamage = 1.8;
	slimeGun.maxDamage = 3.4;
	slimeGun.range = (int)(7.0 * TILE_SIZE); //Max pixels to travel
	slimeGun.reload = 20; //60 = 1 second
	slimeGun.speed = 6; //Pixels per frame

	//Enemy Weapon
	fairyGun.bullet = wispball;
	fairyGun.minDamage = 1.6;
	fairyGun.maxDamage = 3.2;
	fairyGun.range = (int)(6.0 * TILE_SIZE); //Max pixels to travel
	fairyGun.reload = 20; //60 = 1 second
	fairyGun.speed = 6; //Pixels per frame

	//Enemy Weapon
	wispGun.bullet = wispball;
	wispGun.minDamage = 4.2;
	wispGun.maxDamage = 7.5;
	wispGun.range = (int)(7.0 * TILE_SIZE); //Max pixels to travel
	wispGun.reload = 20; //60 = 1 second
	wispGun.speed = 8; //Pixels per frame

	//Enemy Weapon
	ballistaGun.bullet = arrow;
	ballistaGun.minDamage = 2.6;
	ballistaGun.maxDamage = 4.3;
	ballistaGun.range = (int)(11.0 * TILE_SIZE);
	ballistaGun.reload = 40;
	ballistaGun.speed = 8;
}