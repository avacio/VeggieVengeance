// Header

#include "fighter.hpp"
#include "ai.hpp"

bool AI::get_in_play() const
{
    return in_play;
}

void AI::set_in_play(bool value)
{
    in_play = value;
}

void AI::update(float ms, vec2 player1Position)
{
    if (player1Position.x > this->get_position().x)
    {
        this->set_movement(0);
    }
    else if(player1Position.x < this->get_position().x)
    {
        this->set_movement(1);
    }

    Fighter::update(ms);
    this->set_movement(5);
    this->set_movement(6);
}

float distance(vec2 pos1, vec2 pos2)
{
    return std::sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y));
}