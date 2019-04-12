#pragma once
// internal
#include "common.hpp"
#include "fighter.hpp"
#include "attack.hpp"

class AI : public Fighter
{
  public:
    AI(unsigned int id, AIType type);

    bool get_in_play() const;
    void set_in_play(bool value);
    Attack * update(float ms, QuadTree* platform_tree, vec2 player1Position, bool player1FacingFront, int player1HP, bool player1isBlocking);
    float distance(vec2 pos1, vec2 pos2);

  private:
    bool in_play = false; // denotes if a player object is currently in play, false by default
	bool is_in_Front(int p1Xpos);
	void move_Randomly();

    AIType aitype;
};