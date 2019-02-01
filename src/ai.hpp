#pragma once
// internal
#include "common.hpp"
#include "fighter.hpp"

enum aiType {CHASE, AVOID};

class AI : public Fighter
{
  public:
	 AI(aiType type);

    bool get_in_play() const;
    void set_in_play(bool value);
    void update(float ms, vec2 player1Position);
    float distance(vec2 pos1, vec2 pos2);

  private:
    bool in_play = false; // denotes if a player object is currently in play, false by default
	aiType aitype;
	
};