// Header

#include "fighter.hpp"
#include "ai.hpp"
#include <time.h>
#include <random>

int d;
int engagingDistance = 75;
int randNum;
long t = time(0); //initialize time
float stageLowerXBound = 200;
float stageUpperXBound = 800;

AI::AI(unsigned int id, AIType type) : Fighter(id)
{
    this->aitype = type;
}

bool AI::get_in_play() const
{
    return in_play;
}

void AI::set_in_play(bool value)
{
    in_play = value;
}

Attack * AI::update(float ms, std::vector<Platform> platforms, vec2 player1Position)
{
	Attack * attack = NULL;
    if (aitype == CHASE)
    {

        if (player1Position.x > this->get_position().x)
        {
            this->set_movement(MOVING_FORWARD);
        }
        else if (player1Position.x < this->get_position().x)
        {
            this->set_movement(MOVING_BACKWARD);
        }

        attack = Fighter::update(ms, platforms);
        this->set_movement(STOP_MOVING_FORWARD);
        this->set_movement(STOP_MOVING_BACKWARD);
    }
    else if (aitype == AVOID)
    {
        std::random_device rd; // non-deterministic generator
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 2); //ADD FOR MORE ACTIONS
		d = abs(player1Position.x - this->get_position().x);
		//==================================
		//If the distance from AI to player is > engagingDistance, do random movements
		//==================================
		if (d > engagingDistance) {
			if (time(0) > t)
			{
				if (randNum == 2)
					std::uniform_int_distribution<> dist(0, 1); //check that it didn't just jump
				randNum = dist(gen);                            //REPLACE FOR BOUNDED RANDOM

				t = time(0);
			}

			if (this->get_position().x < stageLowerXBound)
			{ //Left boundary detector
				randNum = 0;
			}
			if (this->get_position().x > stageUpperXBound)
			{ //Right boundary detector
				randNum = 1;
			}
			if (randNum == 2)
			{
				this->set_movement(randNum);
				randNum = dist(gen);
				this->set_movement(randNum);
			}
			else
				this->set_movement(randNum);
		}
		else {
			if (time(0) > t) {
				//=========================== PUNCH BEHAVIOR IMPLEMENTED HERE!!!
				this->set_movement(PUNCHING);
				t = time(0);
				
			}
		}

        attack = Fighter::update(ms, platforms);
        this->set_movement(STOP_MOVING_FORWARD);
        this->set_movement(STOP_MOVING_BACKWARD);
    }
	else if (aitype == RANDOM) {
		std::random_device rd; // non-deterministic generator
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 2); //ADD FOR MORE ACTIONS

		if (time(0) > t)
		{
			if (randNum == 2)
				std::uniform_int_distribution<> dist(0, 1); //check that it didn't just jump
			randNum = dist(gen);                            //REPLACE FOR BOUNDED RANDOM

			t = time(0);
		}

		if (this->get_position().x < stageLowerXBound)
		{ //Left boundary detector
			randNum = 0;
		}
		if (this->get_position().x > stageUpperXBound)
		{ //Right boundary detector
			randNum = 1;
		}
		if (randNum == 2)
		{
			this->set_movement(randNum);
			randNum = dist(gen);
			this->set_movement(randNum);
		}
		else
			this->set_movement(randNum);

		attack = Fighter::update(ms, platforms);
		this->set_movement(STOP_MOVING_FORWARD);
		this->set_movement(STOP_MOVING_BACKWARD);
	}


	//may return either NULL or the punch collision object depending if the character was punching
	return attack;
}

float distance(vec2 pos1, vec2 pos2)
{
    return std::sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y));
}