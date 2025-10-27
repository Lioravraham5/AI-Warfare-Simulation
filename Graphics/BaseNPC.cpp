#include "BaseNPC.h"

// BaseNPC.cpp:

BaseNPC::BaseNPC(Position p, TeamID t, Map* m)
{
	position = p;
	teamID = t;
	pMap = m;
}
