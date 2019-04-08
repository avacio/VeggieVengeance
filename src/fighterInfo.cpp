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
Texture BROCCOLI_UPPERCUT_TEXTURE;
Texture BROCCOLI_TIRED_1_TEXTURE;
Texture BROCCOLI_TIRED_2_TEXTURE;
Texture BROCCOLI_CHARGING_TEXTURE;
Texture BROCCOLI_POWER_PUNCH_TEXTURE;

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

Texture EGGPLANT_TEXTURE;
Texture EGGPLANT_IDLE_TEXTURE;
Texture EGGPLANT_CROUCH_TEXTURE;
Texture EGGPLANT_PUNCH_TEXTURE;
Texture EGGPLANT_CROUCH_PUNCH_TEXTURE;
Texture EGGPLANT_CHARGING_TEXTURE;
Texture EGGPLANT_POWER_PUNCH_TEXTURE;
Texture EGGPLANT_DEATH_TEXTURE;
Texture EGGPLANT_TIRED_1_TEXTURE;
Texture EGGPLANT_TIRED_2_TEXTURE;

Texture MAIN_MENU_TEXTURE;
Texture KITCHEN_BACKGROUND_TEXTURE;
Texture OVEN_BACKGROUND_TEXTURE;
Texture PLATFORM_TEXTURE;
Texture BULLET_TEXTURE;

Texture EMOJI_SWEAT_TEXTURE;
Texture EMOJI_MOUTH_TEXTURE;
Texture EMOJI_100_TEXTURE;
Texture EMOJI_OKHAND_TEXTURE;

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
		BROCCOLI_UPPERCUT_TEXTURE.load_from_file(textures_path("broccoli_uppercut.png")) &&
		BROCCOLI_TIRED_1_TEXTURE.load_from_file(textures_path("broccoli_tired_1.png")) &&
		BROCCOLI_TIRED_2_TEXTURE.load_from_file(textures_path("broccoli_tired_2.png")) &&
		BROCCOLI_CHARGING_TEXTURE.load_from_file(textures_path("broccoli_charging.png")) &&
		BROCCOLI_POWER_PUNCH_TEXTURE.load_from_file(textures_path("broccoli_power_punch.png")) &&

		YAM_TEXTURE.load_from_file(textures_path("yam.png")) &&
		YAM_IDLE_TEXTURE.load_from_file(textures_path("yam_idle.png")) &&
		YAM_PUNCH_TEXTURE.load_from_file(textures_path("yam_punch.png")) &&
		YAM_POWER_PUNCH_TEXTURE.load_from_file(textures_path("yam_power_punch.png")) &&
		YAM_CROUCH_PUNCH_TEXTURE.load_from_file(textures_path("yam_crouch_punch.png")) &&
		YAM_CROUCH_TEXTURE.load_from_file(textures_path("yam_crouch.png")) &&
		YAM_CHARGING_TEXTURE.load_from_file(textures_path("yam_charging.png")) &&
		YAM_DEATH_TEXTURE.load_from_file(textures_path("yam_death.png")) &&
		YAM_TIRED_1_TEXTURE.load_from_file(textures_path("yam_tired_1.png")) &&
		YAM_TIRED_2_TEXTURE.load_from_file(textures_path("yam_tired_2.png")) &&

		EGGPLANT_TEXTURE.load_from_file(textures_path("eggplant.png")) &&
		EGGPLANT_IDLE_TEXTURE.load_from_file(textures_path("eggplant_idle.png")) &&
		EGGPLANT_CROUCH_TEXTURE.load_from_file(textures_path("eggplant_crouch.png")) &&
		EGGPLANT_PUNCH_TEXTURE.load_from_file(textures_path("eggplant_punch.png")) &&
		EGGPLANT_CROUCH_PUNCH_TEXTURE.load_from_file(textures_path("eggplant_crouch_punch.png")) &&
		EGGPLANT_CHARGING_TEXTURE.load_from_file(textures_path("eggplant_charging.png")) &&
		EGGPLANT_POWER_PUNCH_TEXTURE.load_from_file(textures_path("eggplant_power_punch.png")) &&
		EGGPLANT_DEATH_TEXTURE.load_from_file(textures_path("eggplant_death.png")) &&
		EGGPLANT_TIRED_1_TEXTURE.load_from_file(textures_path("eggplant_tired_1.png")) &&
		EGGPLANT_TIRED_2_TEXTURE.load_from_file(textures_path("eggplant_tired_2.png")) &&

		MAIN_MENU_TEXTURE.load_from_file(textures_path("mainMenu.jpg")) &&
		KITCHEN_BACKGROUND_TEXTURE.load_from_file(textures_path("kitchen_background.png")) &&
		OVEN_BACKGROUND_TEXTURE.load_from_file(textures_path("oven_background.png")) && // TODO
		PLATFORM_TEXTURE.load_from_file(textures_path("grey.png")) &&
		BULLET_TEXTURE.load_from_file(textures_path("french_fry.png")) &&

		EMOJI_SWEAT_TEXTURE.load_from_file(textures_path("sweat_wind.png")) &&
		EMOJI_MOUTH_TEXTURE.load_from_file(textures_path("mouth_wind.png")) &&
		EMOJI_100_TEXTURE.load_from_file(textures_path("100_wind.png")) &&
		EMOJI_OKHAND_TEXTURE.load_from_file(textures_path("okhand_wind.png"));

	if (!initSuccess) {
		fprintf(stderr, "Failed to load sprites from file!\n");
	}
	return initSuccess;
}

void FighterInfo::setInfo(FighterCharacter fc, int strength, int speed, int health, std::string sciName, std::vector<std::string> names, std::string abilities)
{
	this->fc = fc;
	this->strength = strength;
	this->speed = speed;
	this->health = health;
	this->sciName = sciName;
	this->names = names;
	this->abilities = abilities;
}

void load_fighter_templates() {
	FighterInfo potato, broccoli, eggplant, yam;
	potato.setInfo(POTATO, 5, 2, 100, "solanum tuberosum", { "Spud", "PoeTatum", "BabyTater", "TaterHater" }, "Fry bullets, Tater tot bombs");
	fighterMap[POTATO] = potato;
	broccoli.setInfo(BROCCOLI, 3, 3, 100, "brassica oleracea", { "BrockLee", "Sprout", "BrockNRoll", "BroccOn" }, "Bouncy cauliflower, uppercut, double-jump");
	fighterMap[BROCCOLI] = broccoli;
	eggplant.setInfo(EGGPLANT, 4, 2, 100, "solanum melongena", { "Eggsy", "Aubergine", "PurpleCuke", "Thirsty" }, "Circling emojis, emoji bullets");
	fighterMap[EGGPLANT] = eggplant;
	yam.setInfo(YAM, 3, 5, 80, "ipomoea batatas", { "YamaJama", "MamaYama", "SweetzSpud", "SkinnyPotato" }, "Heal, dash");
	fighterMap[YAM] = yam;
	fprintf(stderr, "Loaded fighter templates\n");
}

std::string FighterInfo::getFCName() {
	int r = get_random_number(names.size() - 1);
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

vec3 get_particle_color_for_fc(FighterCharacter fc) {
	switch (fc) {
	default:
		return { 0.8f,0.8f,0.8f };
		break;
	case POTATO:
		return { 1.f,0.7f,0.2f };
		break;
	case BROCCOLI:
		return { 0.f,1.f,0.f };
		break;
	case EGGPLANT:
		return { 0.5f,0.2f,1.f };
		break;
	case YAM:
		return { 1.f,0.2f,0.f };
		break;
	}
}