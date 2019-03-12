#pragma once

#include "common.hpp"
#include "textRenderer.hpp"

class Water : public Renderable
{
	TextRenderer* title;

  public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Renders the water
	void draw(const mat3 &projection) override;

	void set_is_wavy(bool is_wavy);
	void set_game_over(std::string winnerName);
	void reset_game_over();

  private:
	bool m_is_wavy;
	bool m_is_game_over = false;
	float m_game_over_time = -1;
	std::string m_winner_name = "";
};
