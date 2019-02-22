#pragma once
// internal
#include "common.hpp"
#include "fighter.hpp"

class Player1 : public Fighter
{
	// Shared between all fighters, no need to load one for each instance
	static Texture player1_texture;

  public:
	bool get_in_play() const;
	void set_in_play(bool value);

	// Renders the salmon
	// projection is the 2D orthographic projection matrix
	void draw(const mat3 &projection) override;


  private:
	bool m_in_play = false; // denotes if a player object is currently in play, false by default

	int m_idle_counter = 0;

	int m_jump_counter = 0;
	CrouchState m_crouch_state = NOT_CROUCHING;
};