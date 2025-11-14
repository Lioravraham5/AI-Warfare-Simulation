#pragma once
#include "BaseNPC.h"
#include "AStar.h"
#include "MedicNPC.h"
#include "SupplierNPC.h"
#include <unordered_set>
#include <queue>

// CommanderNPC.h:

enum CommanderState {
	COMMANDER_IDLE,             
	COMMANDER_DEAD             
};

// Forward declaration:
class MedicNPC;
class SupplierNPC;

class CommanderNPC : public BaseNPC
{
private:
	FSM<CommanderState> fsm;
	vector<WarriorNPC*> warriors; // ADD: initialize of it
	MedicNPC* medic = nullptr;
	SupplierNPC* supplier = nullptr;
	AStar* pAStar = nullptr;
	
	NodeAStar* pGoalNode = nullptr; // store the target position found by A-Star 	
	bool combinedVisibilityMap[MAP_SIZE][MAP_SIZE];

public:
	CommanderNPC(Position p, TeamID t, Map* m, MedicNPC* med, SupplierNPC* s, vector<WarriorNPC*> warriors);
	virtual ~CommanderNPC();
	void tick() override;
	void draw() const override;
	void requestSupply(WarriorNPC* soldier);
	void requestHeal(WarriorNPC* soldier);

private:
	void generateCombinedVisibilityMap();
	bool isEnemyDetectedByWarrior() const;
	NodeBFS* findSafestPosition();
	void moveOneStepToward(NodeBFS* pSafestNode);
};

