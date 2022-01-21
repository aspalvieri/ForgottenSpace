void buildSprites()
{
	//spacebg is declared in Variables.h
	spacebg.setRender(gRenderer, gWindow);
	spacebg.camera = &camera;
	spacebg.loadSpriteImage("bin/images/spacebg.png")
		.setFrameSize(500, 500)
		.setSize(1500, 1500)
		.setPosition(0, 0)
		.setCollide(false)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");

	//greenPortal is declared in Variables.h
	greenPortal.setRender(gRenderer, gWindow);
	greenPortal.camera = &camera;
	greenPortal.loadSpriteImage("bin/images/greenportal.png")
		.setFrameSize(50, 50)
		.setSize(75, 75)
		.setPosition(0, 0)
		.setDelay(3)
		.setCollide(false)
		.pushFrameRow("Idle", 0, 0, 50, 0, 4)
		.setAnimation("Idle");

	darkgreenPortal.setRender(gRenderer, gWindow);
	darkgreenPortal.camera = &camera;
	darkgreenPortal.loadSpriteImage("bin/images/darkgreenportal.png")
		.setFrameSize(50, 50)
		.setSize(75, 75)
		.setPosition(0, 0)
		.setDelay(3)
		.setCollide(false)
		.pushFrameRow("Idle", 0, 0, 50, 0, 4)
		.setAnimation("Idle");

	whiteDoor.setRender(gRenderer, gWindow);
	whiteDoor.camera = &camera;
	whiteDoor.loadSpriteImage("bin/images/whitedoor.png")
		.setFrameSize(50, 50)
		.setSize(50, 50)
		.setPosition(0, 0)
		.setCollide(true)
		.setDelay(3)
		.pushFrameRow("White", 0, 0, 50, 0, 8)
		.pushFrameRow("Blue", 0, 50, 50, 0, 8)
		.pushFrameRow("Red", 0, 100, 50, 0, 8)
		.setAnimation("White");

	//exitPortal is declared in Variables.h
	exitPortal.setRender(gRenderer, gWindow);
	exitPortal.camera = &camera;
	exitPortal.loadSpriteImage("bin/images/exitportal.png")
		.setFrameSize(50, 50)
		.setSize(100, 100)
		.setPosition(0, 0)
		.setDelay(3)
		.setCollide(false)
		.pushFrameRow("Idle", 0, 0, 50, 0, 4)
		.setAnimation("Idle");

	//chest is declared in Variables.h
	chest.setRender(gRenderer, gWindow);
	chest.camera = &camera;
	chest.loadSpriteImage("bin/images/chest.png")
		.setFrameSize(50, 50)
		.setSize(TILE_SIZE, TILE_SIZE)
		.setPosition(0, 0)
		.setCollide(true)
		.pushFrame("Closed", 0, 0)
		.pushFrame("Open", 50, 0)
		.setAnimation("Closed");

	//reactor is declared in Variables.h
	reactor.setRender(gRenderer, gWindow);
	reactor.camera = &camera;
	reactor.loadSpriteImage("bin/images/reactor.png")
		.setFrameSize(100, 100)
		.setSize(100, 100)
		.setPosition(0, 0)
		.setCollide(true)
		.setDelay(2)
		.pushFrameRow("Idle", 0, 0, 100, 0, 6)
		.setAnimation("Idle");

	//forge is declared in Variables.h
	forge.setRender(gRenderer, gWindow);
	forge.camera = &camera;
	forge.loadSpriteImage("bin/images/weaponforge.png")
		.setFrameSize(100, 100)
		.setSize(100, 100)
		.setPosition(0, 0)
		.setCollide(true)
		.setDelay(3)
		.pushFrameRow("Idle", 0, 0, 100, 0, 7)
		.pushFrameRow("Idle", 0, 100, 100, 0, 7)
		.setAnimation("Idle");


	//-------------------------------------------------------------------PLAYER SPRITES
	//Player is declared in Variables.h
	player.setRenderer(gRenderer, gWindow);
	player.camera = &camera;
	player.loadSpriteImage("bin/images/player.png")
		.setFrameSize(40, 40)
		.setPosition(300, 300)
		.setDelay(5)
		.setSize(PLAYER_SIZE, PLAYER_SIZE)
		.setCollide(true)
		.pushFrame("Chest", 0, 0)
		.pushFrame("Helmet", 40, 0)
		.pushFrameRow("Legs", 0, 40, 40, 0, 2)
		.pushFrameRow("Legs", 0, 80, 40, 0, 2)
		.pushFrameRow("Legs", 0, 120, 40, 0, 2)
		.pushFrameRow("Legs", 0, 160, 40, 0, 2)
		.pushFrameRow("Legs", 0, 200, 40, 0, 2)
		.pushFrameRow("Legs", 0, 240, 40, 0, 2)
		.pushFrameRow("Legs", 0, 280, 40, 0, 2)
		.pushFrameRow("Legs", 0, 320, 40, 0, 2)
		.pushFrameRow("Legs", 0, 360, 40, 0, 2)
		.pushFrameRow("Legs", 0, 400, 40, 0, 2);
	player.setVelocity(4.88);
	player.health = 500;
	player.maxHealth = 500;
	player.energy = 500;
	player.maxEnergy = 500;
	player.buildLocalImages();

	player.weapon = defaultGun;
	player.defaultWeapon = defaultGun;

	//mapPlayer is declared in Variables.h
	mapPlayer.setRender(gRenderer, gWindow);
	mapPlayer.loadSpriteImage("bin/images/mmsprite.png")
		.setFrameSize(40, 40)
		.setPosition(550, 100)
		.setDelay(6)
		.setSize(MAP_TILE_SIZE, MAP_TILE_SIZE)
		.pushFrameRow("Start", 0, 0, 40, 0, 6)
		.setAnimation("Start");

	//mapOutline is declared in Variables.h
	mapOutline.setRender(gRenderer, gWindow);
	mapOutline.loadSpriteImage("bin/images/mmoutline.png")
		.setFrameSize(168, 168)
		.setPosition(0, SCREEN_HEIGHT - 168)
		.setSize(168, 168)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");
	//-------------------------------------------------------------------PLAYER SPRITES


	//-------------------------------------------------------------------ENEMY SPRITES
	//Enemy is declared in Variables
	slime.setRenderCamera(gRenderer, gWindow, &camera);
	slime.loadSpriteImage("bin/images/slime.png")
		.setFrameSize(50, 50)
		.setPosition(100, 100)
		.setDelay(5)
		.setSize(35, 35)
		.setCollide(true)
		.pushFrame("Idle", 0, 0)
		.pushFrameRow("Move", 50, 0, 50, 0, 2)
		.pushFrameRow("Shoot", 0, 50, 50, 0, 2)
		.setAnimation("Idle");
	slime.buildEnemy(ENEMY_TYPE_NORMAL, /*Health*/60, /*Armor*/10, /*Velocity*/3.22, /*Vision*/1300, /*Exp Reward*/11, /*Level*/1, "Green Slime", slimeGun);
	slime.dropChance = 30;
	slime.dropTable.push_back(std::pair<Item, int>{logs, 20});
	slime.dropTable.push_back(std::pair<Item, int>{flower, 20});
	slime.dropTable.push_back(std::pair<Item, int>{stick, 20});
	slime.dropTable.push_back(std::pair<Item, int>{stone, 10});
	slime.dropTable.push_back(std::pair<Item, int>{speedRifle, 15});
	slime.dropTable.push_back(std::pair<Item, int>{hpotion, 15});
	

	//fairy is declared in Variables
	fairy.setRenderCamera(gRenderer, gWindow, &camera);
	fairy.loadSpriteImage("bin/images/fairy.png")
		.setFrameSize(43, 31)
		.setPosition(100, 100)
		.setDelay(2)
		.setSize(30, 25)
		.setCollide(true)
		.pushFrame("Idle", 0, 1)
		.pushFrameRow("Move", 0, 1, 43, 0, 3)
		.pushFrame("Move", 0, 32)
		.pushFrameRow("Shoot", 0, 1, 43, 0, 3)
		.pushFrame("Shoot", 0, 32)
		.setAnimation("Idle");
	fairy.buildEnemy(ENEMY_TYPE_NORMAL, /*Health*/40, /*Armor*/5, /*Velocity*/3.86, /*Vision*/1300, /*Exp Reward*/8, /*Level*/1, "Forest Fairy", fairyGun);
	fairy.dropChance = 20;
	fairy.dropTable.push_back(std::pair<Item, int>{logs, 20});
	fairy.dropTable.push_back(std::pair<Item, int>{flower, 20});
	fairy.dropTable.push_back(std::pair<Item, int>{stick, 20});
	fairy.dropTable.push_back(std::pair<Item, int>{stone, 10});
	fairy.dropTable.push_back(std::pair<Item, int>{speedRifle, 15});
	fairy.dropTable.push_back(std::pair<Item, int>{hpotion, 15});

	//wisp is declared in Variables
	wisp.setRenderCamera(gRenderer, gWindow, &camera);
	wisp.loadSpriteImage("bin/images/wisp.png")
		.setFrameSize(50, 50)
		.setPosition(100, 100)
		.setDelay(4)
		.setSize(50, 50)
		.setCollide(true)
		.pushFrame("Idle", 0, 0)
		.pushFrameRow("Move", 0, 0, 50, 0, 3)
		.pushFrame("Move", 0, 50)
		.pushFrameRow("Shoot", 0, 50, 50, 0, 2)
		.setAnimation("Idle");
	wisp.buildEnemy(ENEMY_TYPE_NORMAL, /*Health*/175, /*Armor*/50, /*Velocity*/4.18, /*Vision*/1300, /*Exp Reward*/25, /*Level*/3, "Portal Guardian", wispGun);
	wisp.setSpellcaster(true, 5 * SCREEN_FPS, &spells);
	wisp.rotation.push_back(new BurstShotBoss(player, enemies, bullets, texts, "Burst Shot Boss"));
	wisp.rotation.back()->bullet = bsbullet;
	wisp.dropChance = 40;
	wisp.dropTable.push_back(std::pair<Item, int>{logs, 20});
	wisp.dropTable.push_back(std::pair<Item, int>{flower, 20});
	wisp.dropTable.push_back(std::pair<Item, int>{stick, 20});
	wisp.dropTable.push_back(std::pair<Item, int>{stone, 10});
	wisp.dropTable.push_back(std::pair<Item, int>{speedRifle, 15});
	wisp.dropTable.push_back(std::pair<Item, int>{hpotion, 15});

	ballista.setRenderCamera(gRenderer, gWindow, &camera);
	ballista.loadSpriteImage("bin/images/ballista.png")
		.setFrameSize(50, 50)
		.setPosition(0, 0)
		.setDelay(2)
		.setSize(50, 50)
		.setCollide(true)
		.pushFrame("Idle", 0, 0)
		.pushFrameRow("Shoot", 0, 0, 50, 0, 4)
		.setAnimation("Idle");
	ballista.buildEnemy(ENEMY_TYPE_NORMAL, /*Health*/95, /*Armor*/40, /*Velocity*/0, /*Vision*/1300, /*Exp Reward*/15, /*Level*/2, "Wall Tower", ballistaGun);

	//-------------------------------------------------------------------ENEMY RESOURCE SPRITES

	//goldNode is declared in Variables
	goldNode.setRenderCamera(gRenderer, gWindow, &camera);
	goldNode.loadSpriteImage("bin/images/goldexample.png")
		.setFrameSize(30, 30)
		.setPosition(0, 0)
		.setCollide(true)
		.setSize(40, 40)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");
	goldNode.buildResource(ENEMY_TYPE_NODE, /*Health*/35, /*Armor*/0, /*Velocity*/0, /*Vision*/0, /*Exp Reward*/4, /*Level*/1, "Gold Node");
	goldNode.dropChance = 100;
	goldNode.dropTable.push_back(std::pair<Item, int>{goldOre, 1});

	//crystalNode is declared in Variables
	crystalNode.setRenderCamera(gRenderer, gWindow, &camera);
	crystalNode.loadSpriteImage("bin/images/crystalexample.png")
		.setFrameSize(27, 29)
		.setPosition(0, 0)
		.setCollide(true)
		.setSize(40, 40)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");
	crystalNode.buildResource(ENEMY_TYPE_NODE, /*Health*/40, /*Armor*/0, /*Velocity*/0, /*Vision*/0, /*Exp Reward*/5, /*Level*/1, "Crystal Node");
	crystalNode.dropChance = 100;
	crystalNode.dropTable.push_back(std::pair<Item, int>{crystalOre, 1});

	//bloomweedNode is declared in Variables
	bloomweedNode.setRenderCamera(gRenderer, gWindow, &camera);
	bloomweedNode.loadSpriteImage("bin/images/bloomweed.png")
		.setFrameSize(40, 40)
		.setPosition(0, 0)
		.setCollide(true)
		.setSize(40, 40)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");
	bloomweedNode.buildResource(ENEMY_TYPE_NODE, /*Health*/35, /*Armor*/0, /*Velocity*/0, /*Vision*/0, /*Exp Reward*/3, /*Level*/1, "Bloomweed");
	bloomweedNode.dropChance = 100;
	bloomweedNode.dropTable.push_back(std::pair<Item, int>{bloomweed, 1});

	//roseleafNode is declared in Variables
	roseleafNode.setRenderCamera(gRenderer, gWindow, &camera);
	roseleafNode.loadSpriteImage("bin/images/roseleaf.png")
		.setFrameSize(40, 40)
		.setPosition(0, 0)
		.setCollide(true)
		.setSize(40, 40)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");
	roseleafNode.buildResource(ENEMY_TYPE_NODE, /*Health*/30, /*Armor*/0, /*Velocity*/0, /*Vision*/0, /*Exp Reward*/3, /*Level*/1, "Roseleaf");
	roseleafNode.dropChance = 100;
	roseleafNode.dropTable.push_back(std::pair<Item, int>{roseleaf, 1});
	//-------------------------------------------------------------------ENEMY SPRITES

	screenBG.push_back(Sprite(gRenderer, gWindow));
	screenBG.back().setRender(gRenderer, gWindow);
	screenBG.back().loadSpriteImage("bin/images/bg01.png")
		.setFrameSize(SCREEN_WIDTH, SCREEN_HEIGHT)
		.setPosition(0, 0)
		.setSize(SCREEN_WIDTH, SCREEN_HEIGHT)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");

	screenBG.push_back(Sprite(gRenderer, gWindow));
	screenBG.back().setRender(gRenderer, gWindow);
	screenBG.back().loadSpriteImage("bin/images/bg02.png")
		.setFrameSize(SCREEN_WIDTH, SCREEN_HEIGHT)
		.setPosition(SCREEN_WIDTH, 0)
		.setSize(SCREEN_WIDTH, SCREEN_HEIGHT)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");

	menuimage.loadFromFile("bin/images/inventory.png", gRenderer);
	menuimage.setScale(300.0 / 570.0, 400.0 / 795.0);

	gamelogo.loadFromFile("bin/images/gamelogo.png", gRenderer);
}
