#pragma once
#include "BaseNPC.h"
#include "FSM.h"
#include "AStar.h"
#include "NodeBFS.h"
#include "Bullet.h"
#include "Grenade.h"

// WarriorNPC.h:

enum WarriorState {
	IDLE, // Warrior is idle - not performing any action
	MOVE_TO_GIVEN_POSITION, // Warrior is moving to a specified position via A*
	MOVE_TO_ATTACK_POSITION, // Warrior is moving to an attack position via A*
	ATTACKING, // Warrior is attacking an enemy within range - shooting / throwing grenade
	MOVE_TO_COVER_POSITION, // Warrior is moving to the selected cover position via A*
	WAIT_FOR_SUPPLY, // Warrior is waiting for supplies from Provider
	IN_SUPPLY_PROCESS, // Warrior is being supplied by Provider
	WAIT_FOR_HEALING, // Warrior is waiting for healing from Medic
	IN_HEALING_PROCESS, // Warrior is being healed by Medic
	DEAD // Warrior is dead
};

static const int MAX_BULLETS = 30;
static const int MAX_GRENADES = 5;
static const int VIEW_RADIUS = 10;

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
	vector<Bullet*> activeBullets;
	vector<Grenade*> activeGrenades;
	vector<BaseNPC*> allEnemies;

public:
	WarriorNPC(Position p, TeamID t, Map* m);
	virtual ~WarriorNPC();
	void tick() override;
	void handleOrder(Order* pOrder);
	void draw() const override;
	void setEnemies(const vector<BaseNPC*>& enemies) { allEnemies = enemies; }
	const bool (*getVisibilityMap() const)[MAP_SIZE] { return visibilityMap; }


private:
	bool hasLineOfSight(Map* pMap, int row1, int col1, int row2, int col2);
	void updateVisibilityMap();
	BaseNPC* findBestVisibleEnemy();
	Position findBestAttackPosition(const BaseNPC* targetEnemy);
	double calculateDistanceToEnemy(const BaseNPC* enemy);
	void attackBehavior();
	NodeBFS* findSafestPosition();
	NodeBFS* getNextStepToSafestPosition(NodeBFS* pSafestNode);
};


