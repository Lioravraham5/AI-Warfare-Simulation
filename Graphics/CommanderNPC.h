#pragma once
#include "BaseNPC.h"
#include "AStar.h"

// CommanderNPC.h:

class CommanderNPC : public BaseNPC
{
private:
	vector<BaseNPC*> teamMembers; // ADD: initialize of it
	AStar* pAStar = nullptr;
	vector<BaseNPC*> supplyRequests;
	vector<BaseNPC*> healRequests;
	NodeAStar* pGoalNode = nullptr; // store the target position found by A-Star 	
	bool combinedVisibilityMap[MAP_SIZE][MAP_SIZE];

public:
	CommanderNPC(Position p, TeamID t, Map* m);
	virtual ~CommanderNPC();
	void tick() override;
	void draw() const override;
	void requestSupply(BaseNPC* soldier);
	void requestHeal(BaseNPC* soldier);

private:
	void generateCombinedVisibilityMap();
	
};

