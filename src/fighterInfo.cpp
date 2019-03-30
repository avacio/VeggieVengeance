#include "fighterInfo.hpp"
#include <iostream>
#include <sstream>

std::map<FighterCharacter, FighterInfo> fighterMap;
Texture POTATO_TEXTURE;
Texture POTATO_IDLE_TEXTURE;
Texture POTATO_PUNCH_TEXTURE;
Texture POTATO_POWER_PUNCH_TEXTURE;
Texture POTATO_CROUCH_PUNCH_TEXTURE;
Texture POTATO_CROUCH_TEXTURE;
Texture POTATO_CHARGING_TEXTURE;
Texture POTATO_DEATH_TEXTURE;
Texture POTATO_TIRED_1_TEXTURE;
Texture POTATO_TIRED_2_TEXTURE;

Texture BROCCOLI_TEXTURE;
Texture BROCCOLI_IDLE_TEXTURE;
Texture BROCCOLI_PUNCH_TEXTURE;
Texture BROCCOLI_CROUCH_PUNCH_TEXTURE;
Texture BROCCOLI_CROUCH_TEXTURE;
Texture BROCCOLI_DEATH_TEXTURE;

Texture YAM_TEXTURE;
Texture YAM_IDLE_TEXTURE;
Texture YAM_PUNCH_TEXTURE;
Texture YAM_POWER_PUNCH_TEXTURE;
Texture YAM_CROUCH_PUNCH_TEXTURE;
Texture YAM_CROUCH_TEXTURE;
Texture YAM_CHARGING_TEXTURE;
Texture YAM_DEATH_TEXTURE;
Texture YAM_TIRED_1_TEXTURE;
Texture YAM_TIRED_2_TEXTURE;

Texture MAIN_MENU_TEXTURE;
Texture BACKGROUND_TEXTURE;

std::vector<Texture> originalTextures;
std::vector<Texture> idleTextures;
std::vector<Texture> punchTextures;
std::vector<Texture> powerPunchTextures;
std::vector<Texture> crouchPunchTextures;
std::vector<Texture> crouchTextures;
std::vector<Texture> chargingTextures;
std::vector<Texture> deathTextures;
std::vector<Texture> tired1Textures;
std::vector<Texture> tired2Textures;

bool load_all_sprites_from_file() {
	bool initSuccess = POTATO_TEXTURE.load_from_file(textures_path("potato.png")) &&
		POTATO_IDLE_TEXTURE.load_from_file(textures_path("potato_idle.png")) &&
		POTATO_PUNCH_TEXTURE.load_from_file(textures_path("potato_punch.png")) &&
		POTATO_POWER_PUNCH_TEXTURE.load_from_file(textures_path("potato_power_punch.png")) &&
		POTATO_CROUCH_PUNCH_TEXTURE.load_from_file(textures_path("potato_crouch_punch.png")) &&
		POTATO_CROUCH_TEXTURE.load_from_file(textures_path("potato_crouch.png")) &&
		POTATO_CHARGING_TEXTURE.load_from_file(textures_path("potato_charging.png")) &&
		POTATO_DEATH_TEXTURE.load_from_file(textures_path("potato_death.png")) &&
		POTATO_TIRED_1_TEXTURE.load_from_file(textures_path("potato_tired_1.png")) &&
		POTATO_TIRED_2_TEXTURE.load_from_file(textures_path("potato_tired_2.png")) &&

		BROCCOLI_TEXTURE.load_from_file(textures_path("broccoli.png")) &&
		BROCCOLI_IDLE_TEXTURE.load_from_file(textures_path("broccoli_idle.png")) &&
		BROCCOLI_PUNCH_TEXTURE.load_from_file(textures_path("broccoli_punch.png")) &&
		BROCCOLI_CROUCH_PUNCH_TEXTURE.load_from_file(textures_path("broccoli_crouch_punch.png")) &&
		BROCCOLI_CROUCH_TEXTURE.load_from_file(textures_path("broccoli_crouch.png")) &&
		BROCCOLI_DEATH_TEXTURE.load_from_file(textures_path("broccoli_death.png")) &&

		YAM_TEXTURE.load_from_file(textures_path("yam.png")) &&
		YAM_IDLE_TEXTURE.load_from_file(textures_path("yam.png")) &&
		YAM_PUNCH_TEXTURE.load_from_file(textures_path("yam_punch.png")) &&
		YAM_POWER_PUNCH_TEXTURE.load_from_file(textures_path("yam_power_punch.png")) &&
		YAM_CROUCH_PUNCH_TEXTURE.load_from_file(textures_path("yam_crouch_punch.png")) &&
		YAM_CROUCH_TEXTURE.load_from_file(textures_path("yam_crouch.png")) &&
		YAM_CHARGING_TEXTURE.load_from_file(textures_path("yam_charging.png")) &&
		YAM_DEATH_TEXTURE.load_from_file(textures_path("yam_death.png")) &&
		YAM_TIRED_1_TEXTURE.load_from_file(textures_path("yam_tired_1.png")) &&
		YAM_TIRED_2_TEXTURE.load_from_file(textures_path("yam_tired_2.png")) &&

		MAIN_MENU_TEXTURE.load_from_file(textures_path("mainMenu.jpg")) &&
		BACKGROUND_TEXTURE.load_from_file(textures_path("background.png"));

	//originalTextures.emplace_back(POTATO_TEXTURE);
	////originalTextures.emplace_back(BROCCOLI_TEXTURE);
	////originalTextures.emplace_back(YAM_TEXTURE); originalTextures.emplace_back(YAM_TEXTURE);
	//idleTextures.emplace_back(POTATO_IDLE_TEXTURE);

	//originalTextures.emplace_back(BROCCOLI_TEXTURE);
	////originalTextures.emplace_back(YAM_TEXTURE); originalTextures.emplace_back(YAM_TEXTURE);
	//idleTextures.emplace_back(BROCCOLI_IDLE_TEXTURE);
	//idleTextures.emplace_back(YAM_IDLE_TEXTURE); idleTextures.emplace_back(YAM_IDLE_TEXTURE);

	//punchTextures.emplace_back(POTATO_PUNCH_TEXTURE); punchTextures.emplace_back(BROCCOLI_PUNCH_TEXTURE);
	//punchTextures.emplace_back(YAM_PUNCH_TEXTURE); punchTextures.emplace_back(YAM_PUNCH_TEXTURE);
	//powerPunchTextures.emplace_back(POTATO_POWER_PUNCH_TEXTURE); powerPunchTextures.emplace_back(BROCCOLI_PUNCH_TEXTURE);
	//powerPunchTextures.emplace_back(YAM_POWER_PUNCH_TEXTURE); powerPunchTextures.emplace_back(YAM_POWER_PUNCH_TEXTURE);

	//crouchPunchTextures.emplace_back(POTATO_CROUCH_PUNCH_TEXTURE); crouchPunchTextures.emplace_back(BROCCOLI_CROUCH_PUNCH_TEXTURE);
	//crouchPunchTextures.emplace_back(YAM_CROUCH_PUNCH_TEXTURE); crouchPunchTextures.emplace_back(YAM_CROUCH_PUNCH_TEXTURE);
	//crouchTextures.emplace_back(POTATO_CROUCH_TEXTURE); crouchTextures.emplace_back(BROCCOLI_CROUCH_TEXTURE);
	//crouchTextures.emplace_back(YAM_CROUCH_TEXTURE); crouchTextures.emplace_back(YAM_CROUCH_TEXTURE);

	//chargingTextures.emplace_back(POTATO_CHARGING_TEXTURE); chargingTextures.emplace_back(BROCCOLI_TEXTURE); //STUB FOR BROC
	//chargingTextures.emplace_back(YAM_CHARGING_TEXTURE); chargingTextures.emplace_back(YAM_CHARGING_TEXTURE);
	//deathTextures.emplace_back(POTATO_DEATH_TEXTURE); deathTextures.emplace_back(BROCCOLI_DEATH_TEXTURE);
	//deathTextures.emplace_back(YAM_DEATH_TEXTURE); deathTextures.emplace_back(YAM_DEATH_TEXTURE);

	//tired1Textures.emplace_back(POTATO_TIRED_1_TEXTURE); tired1Textures.emplace_back(BROCCOLI_TEXTURE); //STUB FOR BROC
	//tired1Textures.emplace_back(YAM_TIRED_1_TEXTURE); tired1Textures.emplace_back(YAM_TIRED_1_TEXTURE);
	//tired2Textures.emplace_back(POTATO_TIRED_2_TEXTURE); tired1Textures.emplace_back(BROCCOLI_TEXTURE); //STUB FOR BROC
	//tired2Textures.emplace_back(YAM_TIRED_2_TEXTURE); tired1Textures.emplace_back(YAM_TIRED_2_TEXTURE);

	if (!initSuccess) {
		fprintf(stderr, "Failed to load sprites from file!\n");
	}
	return initSuccess;
}

//FighterCharacter fc, int strength, int speed, int health, std::string sciName, std::vector<std::string> names, std::string abilities,
//Texture* original, Texture* idle, Texture* punch, Texture* power_punch, Texture* crouch_punch, Texture* crouch,
//Texture* charging, Texture* death, Texture* tired_1, Texture* tired_2
void FighterInfo::setInfo(FighterCharacter fc, int strength, int speed, int health, std::string sciName, std::vector<std::string> names, std::string abilities,
	Texture original, Texture idle, Texture punch, Texture power_punch, Texture crouch_punch, Texture crouch,
	Texture charging, Texture death, Texture tired_1, Texture tired_2)
{
	this->fc = fc;
	this->strength = strength;
	this->speed = speed;
	this->health = health;
	this->sciName = sciName;
	this->names = names;
	this->abilities = abilities;
	//this->original = original;
	//this->idle = idle;
	//this->punch = punch;
	//this->power_punch = power_punch;
	//this->crouch_punch = crouch_punch;
	//this->crouch = crouch;
	//this->charging = charging;
	//this->death = death;
	//this->tired_1 = tired_1;
	//this->tired_2 = tired_2;

	this->original = &original;
	this->idle = &idle;
	this->punch = &punch;
	this->power_punch = &power_punch;
	this->crouch_punch = &crouch_punch;
	this->crouch = &crouch;
	this->charging = &charging;
	this->death = &death;
	this->tired_1 = &tired_1;
	this->tired_2 = &tired_2;
}

void load_fighter_templates() {
	FighterInfo potato, broccoli, eggplant, yam;
	potato.setInfo(POTATO, 5, 1, 100, "solanum tuberosum", { "Spud", "PoeTatum", "BabyTater", "TaterHater" }, "Fry bullets, wedge bombs",
		POTATO_TEXTURE, POTATO_IDLE_TEXTURE, POTATO_PUNCH_TEXTURE, POTATO_POWER_PUNCH_TEXTURE, POTATO_CROUCH_PUNCH_TEXTURE,
		POTATO_CROUCH_TEXTURE, POTATO_CHARGING_TEXTURE, POTATO_DEATH_TEXTURE, POTATO_TIRED_1_TEXTURE, POTATO_TIRED_2_TEXTURE);
	fighterMap[POTATO] = potato;
	// TODO: not all broccoli sprites done
	broccoli.setInfo(BROCCOLI, 3, 3, 100, "brassica oleracea", { "BrockLee", "Sprout", "BrockNRoll", "BroccOn" }, "Bouncy sprouts, uppercut, double-jump",
		BROCCOLI_TEXTURE, BROCCOLI_IDLE_TEXTURE, BROCCOLI_PUNCH_TEXTURE, BROCCOLI_PUNCH_TEXTURE, BROCCOLI_CROUCH_PUNCH_TEXTURE, BROCCOLI_CROUCH_TEXTURE,
		BROCCOLI_PUNCH_TEXTURE, BROCCOLI_DEATH_TEXTURE, BROCCOLI_DEATH_TEXTURE, BROCCOLI_DEATH_TEXTURE);
	fighterMap[BROCCOLI] = broccoli;
	// TODO: eggplant stubs
	eggplant.setInfo(EGGPLANT, 4, 2, 100, "solanum melongena", { "Eggsy", "Aubergine", "PurpleCuke", "Thirsty" }, "Emoji bullets",
		YAM_TEXTURE, YAM_IDLE_TEXTURE, YAM_PUNCH_TEXTURE, YAM_POWER_PUNCH_TEXTURE, YAM_CROUCH_PUNCH_TEXTURE, YAM_CROUCH_TEXTURE, YAM_CHARGING_TEXTURE,
		YAM_DEATH_TEXTURE, YAM_TIRED_1_TEXTURE, YAM_TIRED_2_TEXTURE);
	fighterMap[EGGPLANT] = eggplant;
	yam.setInfo(YAM, 3, 5, 80, "ipomoea batatas", { "YamaJama", "MamaYama", "SweeterThanSpud", "SkinnyPotato" }, "Heal, Gap closure",
		YAM_TEXTURE, YAM_IDLE_TEXTURE, YAM_PUNCH_TEXTURE, YAM_POWER_PUNCH_TEXTURE, YAM_CROUCH_PUNCH_TEXTURE, YAM_CROUCH_TEXTURE, YAM_CHARGING_TEXTURE,
		YAM_DEATH_TEXTURE, YAM_TIRED_1_TEXTURE, YAM_TIRED_2_TEXTURE);
	fighterMap[YAM] = yam;
	fprintf(stderr, "Loaded fighter templates\n");
}

//Texture get_sprite(FighterCharacter fc, SpriteType st) {
//	std::cout << "fc: " << fc << ", st: " << st << std::endl;
//	Texture t;
//
//	return BROCCOLI_DEATH_TEXTURE;
//	//if (fc == POTATO) {
//	//	switch (st) {
//	//	case ORIGINAL: { t = POTATO_TEXTURE; break; }
//	//	case IDLE: return POTATO_IDLE_TEXTURE;
//	//	case PUNCH: return POTATO_PUNCH_TEXTURE;
//	//	case POWER_PUNCH: return POTATO_POWER_PUNCH_TEXTURE;
//	//	}
//	//} else if (fc == BROCCOLI) {
//	//	switch (st) {
//	//	case ORIGINAL: return BROCCOLI_TEXTURE;
//	//	case IDLE: return BROCCOLI_IDLE_TEXTURE;
//	//	}
//	//}
//	//return t;
//}

std::string FighterInfo::getFCName() {
	int r = get_random_number(names.size() - 1);
	//std::cout << "NAMES SIZE: " << names.size() << std::endl;
	//std::cout << "rand: " << r << std::endl;
	std::string p = names.at(r);

	if (std::find(takenNames.begin(), takenNames.end(), p) != takenNames.end()) {
		std::string newP = p + "Jr";
		names.emplace_back(newP);
		takenNames.emplace_back(newP);
		return newP;
	}
	else {
		takenNames.emplace_back(p);
		return p;
	}
}

void FighterInfo::clearTaken() {
	takenNames.clear();
}

