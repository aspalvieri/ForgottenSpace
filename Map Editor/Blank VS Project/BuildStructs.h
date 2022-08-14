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

	//Weapon
	defaultGun.bullet = laser;
	defaultGun.energyCost = 2.5;
	defaultGun.minDamage = 7.5;
	defaultGun.maxDamage = 15.0;
	defaultGun.range = 350; //Max pixels to travel
	defaultGun.reload = 20; //60 = 1 second
	defaultGun.speed = 14.88; //Pixels per frame
	defaultGun.name = "Default Laser";

	//Weapon
	speedGun.bullet = laser;
	speedGun.energyCost = 0.88;
	speedGun.minDamage = 4.0;
	speedGun.maxDamage = 8.0;
	speedGun.range = 200; //Max pixels to travel
	speedGun.reload = 5; //60 = 1 second
	speedGun.speed = 18; //Pixels per frame
	speedGun.name = "Speed Rifle";

	//Weapon
	longGun.bullet = laser;
	longGun.energyCost = 3.75;
	longGun.minDamage = 14.5;
	longGun.maxDamage = 19.5;
	longGun.range = 700; //Max pixels to travel
	longGun.reload = 45; //60 = 1 second
	longGun.speed = 22; //Pixels per frame
	longGun.name = "Long Gun";
}