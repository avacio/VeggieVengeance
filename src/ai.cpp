// Header

#include "fighter.hpp"
#include "ai.hpp"
#include <time.h>
#include <random>

#define ABILITY_RATE 10
#define RESET_RATE 12
#define DISENGAGE_RATE 4

vec2 d;
int engagingDistance = 75;
int verticalEngagingDistance = 30;
int randNum;
long t = time(0); //initialize time
float stageLowerXBound = 200;
float stageUpperXBound = 1000;

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

Attack * AI::update(float ms, QuadTree* platform_tree, vec2 player1Position, bool player1FacingFront, int player1HP, bool player1isBlocking)
{
	Attack * attack = NULL;
	switch (aitype) {
	case CHASE:
	{

		if (player1Position.x > this->get_position().x)
		{
			this->set_movement(MOVING_FORWARD);
		}
		else if (player1Position.x < this->get_position().x)
		{
			this->set_movement(MOVING_BACKWARD);
		}

		attack = Fighter::update(ms, platform_tree);
		this->set_movement(STOP_MOVING_FORWARD);
		this->set_movement(STOP_MOVING_BACKWARD);
	}
		break;
	
	case AVOID:
	{	
		if (time(0) % RESET_RATE == 0) {
			this->aitype = INIT;
		}
		d.x = abs(player1Position.x - this->get_position().x);
		d.y = player1Position.y - this->get_position().y;
		bool facingP1 = is_in_Front(player1Position.x);

		if (d.x > engagingDistance  || !facingP1 || abs(d.y) > 100) {
			this->set_movement(STOP_PUNCHING);
			this->set_movement(STOP_ABILITIES);
			move_Randomly();
		}
		else if (d.x <= engagingDistance ) {
			this->set_movement(BLOCKING);
			
			if (time(0) % DISENGAGE_RATE == 0) {
				this->set_movement(STOP_BLOCKING);
				
				if (d.x <= engagingDistance && facingP1) {
					if (abs(d.y) <= verticalEngagingDistance)
						this->set_movement(PUNCHING);
					else if (d.y < -70 && d.y > -100) this->set_movement(ABILITY_1);
				}
				break;
			}
			
			
		}

		attack = Fighter::update(ms, platform_tree);
		this->set_movement(STOP_MOVING_FORWARD);
		this->set_movement(STOP_MOVING_BACKWARD);

	}
		break;
	
	case INIT:
	{
		if (player1HP - this->get_health() >= 50) this->aitype = AVOID;
		else {
			d.x = abs(player1Position.x - this->get_position().x);
			d.y = player1Position.y - this->get_position().y;
			bool facingP1 = is_in_Front(player1Position.x);

			if (d.x > engagingDistance || !facingP1 || abs(d.y) > 100) {
				this->set_movement(STOP_PUNCHING);
				this->set_movement(STOP_ABILITIES);
				move_Randomly();
			}
			else if (d.x <= engagingDistance && facingP1) {
				if (time(0) % DISENGAGE_RATE) move_Randomly();

				else if (abs(d.y) <= verticalEngagingDistance)
					this->set_movement(PUNCHING);
				else if (d.y < -70 && d.y > -100) this->set_movement(ABILITY_1);
				
			}


			if (time(0) % ABILITY_RATE == 0 && facingP1) {
				this->set_movement(ABILITY_2);
			}

			attack = Fighter::update(ms, platform_tree);
			this->set_movement(STOP_MOVING_FORWARD);
			this->set_movement(STOP_MOVING_BACKWARD);
		}

	}
		break;
	case RANDOM:
	{
		move_Randomly();

		attack = Fighter::update(ms, platform_tree);
		this->set_movement(STOP_MOVING_FORWARD);
		this->set_movement(STOP_MOVING_BACKWARD);
	}
		break;
	}
	
	//may return either NULL or the punch collision object depending if the character was punching
	return attack;
}

void AI::move_Randomly(){
	std::random_device rd; // non-deterministic generator
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, 2); //ADD FOR MORE ACTIONS
	if (time(0) > t) {
		//if (randNum == 2)
		//	std::uniform_int_distribution<> dist(0, 1); //add if AI can not double jump
		randNum = dist(gen);                            

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

bool AI::is_in_Front(int p1Xpos) {
	
	int d = p1Xpos - this->get_position().x;
	if (d > 0 && this->get_facing_front()) return true;
	if (d < 0 && !this->get_facing_front()) return true;
	else return false;
}

float distance(vec2 pos1, vec2 pos2)
{
    return std::sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y));
}