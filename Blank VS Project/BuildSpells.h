void buildSpells()
{
	//-----------------------------------------------------------------------------------------Blank Spell
	globalSpells.push_back(new EmptySpell(player, enemies, bullets, texts, "Empty"));
	globalSpells.back()->image.loadFromFile("bin/images/abilities/empty.png", gRenderer);
	globalSpells.back()->buildTexture(gRenderer);

	//-----------------------------------------------------------------------------------------Spell Addition
	globalSpells.push_back(new BurstShot(player, enemies, bullets, texts, "Spectral Shot"));
	globalSpells.back()->image.loadFromFile("bin/images/abilities/burstshot.png", gRenderer);
	globalSpells.back()->bullet = bsbullet;
	globalSpells.back()->buildTexture(gRenderer);

	//-----------------------------------------------------------------------------------------Spell Addition
	globalSpells.push_back(new BurstShield(player, enemies, bullets, texts, "Burst Shield"));
	globalSpells.back()->image.loadFromFile("bin/images/abilities/burstshield.png", gRenderer);
	globalSpells.back()->buildTexture(gRenderer);

	//-----------------------------------------------------------------------------------------Spell Addition
	globalSpells.push_back(new FrostShock(player, enemies, bullets, texts, "Frost Shock"));
	globalSpells.back()->image.loadFromFile("bin/images/abilities/frostshock.png", gRenderer);
	globalSpells.back()->bullet = laser;
	globalSpells.back()->buildTexture(gRenderer);

	//-----------------------------------------------------------------------------------------Spell Addition
	globalSpells.push_back(new GlimmerofLight(player, enemies, bullets, texts, "Glowing Light"));
	globalSpells.back()->image.loadFromFile("bin/images/abilities/glimmeroflight.png", gRenderer);
	globalSpells.back()->buildTexture(gRenderer);

	//-----------------------------------------------------------------------------------------Spell Addition
	globalSpells.push_back(new GuardianShield(player, enemies, bullets, texts, "Guardian Wall"));
	globalSpells.back()->image.loadFromFile("bin/images/abilities/guardianshield.png", gRenderer);
	globalSpells.back()->buildTexture(gRenderer);

	//Set player's spellbook after spells
	player.globalSpells = globalSpells;
	player.spells = &spells;
}
