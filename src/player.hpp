#pragma once
// internal
#include "common.hpp"
#include "fighter.hpp"

class Player : public Fighter
{
  public:
	Player(unsigned int id);
	bool get_in_play() const;
	void set_in_play(bool value);

  private:
	bool m_in_play = false; // denotes if a player object is currently in play, false by default
};