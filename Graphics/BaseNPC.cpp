#include "BaseNPC.h"

// BaseNPC.cpp:

BaseNPC::BaseNPC(Position p, TeamID t, Map* m)
{
	position = p;
	teamID = t;
	pMap = m;
}

void BaseNPC::addHealth(double val)
{
	health += val;
	if (health >= MAX_HEALTH)
		health = MAX_HEALTH;
}

void BaseNPC::subHealth(double val)
{
	health -= val;
	if (health <= 0)
		health = 0;
}
