// Header
#include "player.hpp"
#include "fighter.hpp"

//implement player specific functions here

bool Player::get_in_play() const {
	return in_play;
}

void Player::set_in_play(bool value) {
	in_play = value;
}