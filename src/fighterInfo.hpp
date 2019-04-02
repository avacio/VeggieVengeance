#pragma once

#include "common.hpp"

// stlib
#include <fstream> // stdout, stderr..
#include <sstream>

#include <map>
#include <vector>

enum FighterCharacter
{
	POTATO = 0,
	BROCCOLI,
	EGGPLANT,
	YAM,
	BLANK		// necessary for main menu
};

enum SpriteType
{
	ORIGINAL = 0,
	IDLE,
	PUNCH,
	POWER_PUNCH,
	CROUCH_PUNCH,
	CROUCH,
	CHARGING,
	DEATH,
	TIRED_1,
	TIRED_2,
	UPPERCUT
};

enum Stage
{
	KITCHEN = 0,
	OVEN,
	VANILLA //  for main menu
};

struct FighterInfo
{
public:

	void setInfo(FighterCharacter fc, int strength, int speed, int health, std::string sciName, std::vector<std::string> names, std::string abilities);
	std::string getFCName();
	void clearTaken();

	FighterCharacter fc;
	std::string sciName;
	int strength; // 1-5
	int speed;    // 1-5
	int health;   // normal HP is 100
	std::string abilities;

protected:
	std::vector<std::string> names;
	std::vector<std::string> takenNames;
};

bool load_all_sprites_from_file();
void load_fighter_templates();

extern std::map<FighterCharacter, FighterInfo> fighterMap;

extern Texture POTATO_TEXTURE;
extern Texture POTATO_IDLE_TEXTURE;
extern Texture POTATO_PUNCH_TEXTURE;
extern Texture POTATO_POWER_PUNCH_TEXTURE;
extern Texture POTATO_CROUCH_PUNCH_TEXTURE;
extern Texture POTATO_CROUCH_TEXTURE;
extern Texture POTATO_CHARGING_TEXTURE;
extern Texture POTATO_DEATH_TEXTURE;
extern Texture POTATO_TIRED_1_TEXTURE;
extern Texture POTATO_TIRED_2_TEXTURE;

extern Texture YAM_TEXTURE;
extern Texture YAM_IDLE_TEXTURE;
extern Texture YAM_PUNCH_TEXTURE;
extern Texture YAM_POWER_PUNCH_TEXTURE;
extern Texture YAM_POWER_PUNCH_TEXTURE;
extern Texture YAM_CROUCH_PUNCH_TEXTURE;
extern Texture YAM_CROUCH_TEXTURE;
extern Texture YAM_CHARGING_TEXTURE;
extern Texture YAM_DEATH_TEXTURE;
extern Texture YAM_TIRED_1_TEXTURE;
extern Texture YAM_TIRED_2_TEXTURE;

extern Texture EGGPLANT_TEXTURE;
extern Texture EGGPLANT_IDLE_TEXTURE;
extern Texture EGGPLANT_CROUCH_TEXTURE;
extern Texture EGGPLANT_PUNCH_TEXTURE;
extern Texture EGGPLANT_CROUCH_PUNCH_TEXTURE;
extern Texture EGGPLANT_CHARGING_TEXTURE;
extern Texture EGGPLANT_POWER_PUNCH_TEXTURE;
extern Texture EGGPLANT_DEATH_TEXTURE;
extern Texture EGGPLANT_TIRED_1_TEXTURE;
extern Texture EGGPLANT_TIRED_2_TEXTURE;

extern Texture BROCCOLI_TEXTURE;
extern Texture BROCCOLI_IDLE_TEXTURE;
extern Texture BROCCOLI_PUNCH_TEXTURE;
extern Texture BROCCOLI_CROUCH_PUNCH_TEXTURE;
extern Texture BROCCOLI_CROUCH_TEXTURE;
extern Texture BROCCOLI_DEATH_TEXTURE;
extern Texture BROCCOLI_UPPERCUT_TEXTURE;
extern Texture BROCCOLI_TIRED_1_TEXTURE;
extern Texture BROCCOLI_TIRED_2_TEXTURE;
extern Texture BROCCOLI_CHARGING_TEXTURE;
extern Texture BROCCOLI_POWER_PUNCH_TEXTURE;

extern Texture MAIN_MENU_TEXTURE;
extern Texture KITCHEN_BACKGROUND_TEXTURE;
extern Texture PLATFORM_TEXTURE;
extern Texture BULLET_TEXTURE;

extern Texture EMOJI_SWEAT_TEXTURE;
extern Texture EMOJI_MOUTH_TEXTURE;
extern Texture EMOJI_100_TEXTURE;
extern Texture EMOJI_OKHAND_TEXTURE;