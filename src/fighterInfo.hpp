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
	TIRED_2
};

struct FighterInfo
{
public:
	/*void setInfo(FighterCharacter fc, int strength, int speed, int health, std::string sciName, std::vector<std::string> names, std::string abilities,
		Texture original, Texture idle, Texture punch, Texture power_punch, Texture crouch_punch, Texture crouch,
		Texture charging, Texture death, Texture tired_1, Texture tired_2
	);*/

	void setInfo(FighterCharacter fc, int strength, int speed, int health, std::string sciName, std::vector<std::string> names, std::string abilities,
		Texture original, Texture idle, Texture punch, Texture power_punch, Texture crouch_punch, Texture crouch,
		Texture charging, Texture death, Texture tired_1, Texture tired_2
	);
	std::string getFCName();
	void clearTaken();

	FighterCharacter fc;
	std::string sciName;
	int strength; // 1-5
	int speed;    // 1-5
	int health;   // normal HP is 100
	std::string abilities;

	// SPRITES
	//Texture original;
	//Texture idle;
	//Texture punch;
	//Texture power_punch;
	//Texture crouch_punch;
	//Texture crouch;
	//Texture charging;
	//Texture death;
	//Texture tired_1;
	//Texture tired_2;

	Texture* original;
	Texture* idle;
	Texture* punch;
	Texture* power_punch;
	Texture* crouch_punch;
	Texture* crouch;
	Texture* charging;
	Texture* death;
	Texture* tired_1;
	Texture* tired_2;

protected:
	std::vector<std::string> names;
	std::vector<std::string> takenNames;
};

bool load_all_sprites_from_file();
void load_fighter_templates();
//Texture get_sprite(FighterCharacter fc, SpriteType st);

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

extern Texture BROCCOLI_TEXTURE;
extern Texture BROCCOLI_IDLE_TEXTURE;
extern Texture BROCCOLI_PUNCH_TEXTURE;
extern Texture BROCCOLI_CROUCH_PUNCH_TEXTURE;
extern Texture BROCCOLI_CROUCH_TEXTURE;
extern Texture BROCCOLI_DEATH_TEXTURE;

extern Texture MAIN_MENU_TEXTURE;
extern Texture BACKGROUND_TEXTURE;

extern std::vector<Texture> originalTextures;
extern std::vector<Texture> idleTextures;
extern std::vector<Texture> punchTextures;
extern std::vector<Texture> powerPunchTextures;
extern std::vector<Texture> crouchPunchTextures;
extern std::vector<Texture> crouchTextures;
extern std::vector<Texture> chargingTextures;
extern std::vector<Texture> deathTextures;
extern std::vector<Texture> tired1Textures;
extern std::vector<Texture> tired2Textures;
