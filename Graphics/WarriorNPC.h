#pragma once
#include "BaseNPC.h"
#include "FSM.h"

// WarriorNPC.h:

enum WarriorState {
	IDLE, // Warrior is idle - not performing any action
	MOVE_TO_GIVEN_POSITION, // Warrior is moving to a specified position via A*
	MOVE_TO_ATTACK_POSITION, // Warrior is moving to an attack position via A*
	ATTACKING, // Warrior is attacking an enemy within range - shooting / throwing grenade
	SEARCH_FOR_NEAREST_COVER_POSITION, // Warrior is searching for the nearest cover position via BFS
	MOVE_TO_COVER_POSITION, // Warrior is moving to the selected cover position via A*
	WAIT_FOR_SUPPLY, // Warrior is waiting for supplies from Provider
	IN_SUPPLY_PROCESS, // Warrior is being supplied by Provider
	WAIT_FOR_HEALING, // Warrior is waiting for healing from Medic
	IN_HEALING_PROCESS, // Warrior is being healed by Medic
	DEAD // Warrior is dead
};

static const int MAX_BULLETS = 30;
static const int MAX_GRENADES = 5;

class WarriorNPC : public BaseNPC
{
private:
	int bullets;
	int grenades;
	FSM<WarriorState> fsm;

public:
	WarriorNPC(Position p, TeamID t, Map* m);
	virtual ~WarriorNPC() = default;
	void tick() override;
	void draw() const override;
	void handleOrder(Order* pOrder);

private:


};


