#pragma once
#include "BaseNPC.h"
#include "FSM.h"
#include "AStar.h"
#include "NodeBFS.h"
#include "Bullet.h"
#include "Grenade.h"
#include "CommanderNPC.h"
#include "BulletsManager.h"

// WarriorNPC.h:

enum WarriorState {
	WARRIOR_IDLE, // Warrior is idle - not performing any action
	WARRIO_MOVE_TO_GIVEN_POSITION, // Warrior is moving to a specified position via A*
	WARRIOR_MOVE_TO_ATTACK_POSITION, // Warrior is moving to an attack position via A*
	WARRIOR_ATTACKING, // Warrior is attacking an enemy within range - shooting / throwing grenade
	WARRIOR_MOVE_TO_COVER_POSITION, // Warrior is moving to the selected cover position via A*
	WARRIOR_WAIT_FOR_SUPPLY, // Warrior is waiting for supplies from Provider
	WARRIOR_IN_SUPPLY_PROCESS, // Warrior is being supplied by Provider
	WARRIOR_WAIT_FOR_HEALING, // Warrior is waiting for healing from Medic
	WARRIOR_IN_HEALING_PROCESS, // Warrior is being healed by Medic
	WARRIOR_DEAD // Warrior is dead
};

static const int MAX_BULLETS = 50;
static const int MAX_GRENADES = 10;

// forward declaration:
class CommanderNPC;

class WarriorNPC : public BaseNPC
{
private:
	int bullets;
	int grenades;
	FSM<WarriorState> fsm;
	AStar* pAStar = nullptr;
	NodeAStar* pGoalNode = nullptr; // store the target position found by A-Star 
	NodeBFS* pSafestNode = nullptr; // store the safest point found by BFS
	bool visibilityMap[MAP_SIZE][MAP_SIZE];
	vector<BaseNPC*> allEnemies;
	CommanderNPC* pCommander = nullptr;
	BulletsManager* bulletsManager = nullptr;

public:
	WarriorNPC(Position p, TeamID t, Map* m, CommanderNPC* c, BulletsManager* bm);
	virtual ~WarriorNPC();
	void tick() override;
	void handleOrder(Order* pOrder);
	void draw() const override;
	
	//Getters:
	const bool (*getVisibilityMap() const)[MAP_SIZE] { return visibilityMap; }
	int getBullets() const { return bullets; }
	int getGrenades() const { return grenades; }

	//Setters:
	void setEnemies(const vector<BaseNPC*>& enemies) { allEnemies = enemies; }
	void setCommander(CommanderNPC* c) { pCommander = c; }

	void addSupply(int val);
	void subBullets(int val);
	void subGrenades(int val);
private:
	bool hasLineOfSight(Map* pMap, int row1, int col1, int row2, int col2);
	void updateVisibilityMap();
	BaseNPC* findBestVisibleEnemy();
	Position findBestAttackPosition(const BaseNPC* targetEnemy);
	double calculateDistanceToEnemy(const BaseNPC* enemy);
	void attackBehavior();
	NodeBFS* findSafestPosition();
	NodeBFS* getNextStepToSafestPosition(NodeBFS* pSafestNode);
	const char* warriorStateToString(WarriorState state);
};


