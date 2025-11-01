#pragma once
#include "BaseNPC.h"
#include "FSM.h"
#include "AStar.h"
#include "NodeAStar.h"

// MedicNPC.h:

enum MedicState {
	MEDIC_IDLE,
	MEDIC_MOVE_TO_WAREHOUSE,
	MEDIC_MOVE_TO_SOLDIER,
	MEDIC_HEALING,
	MEDIC_DEAD
};

const double HEAL_PER_TICK = 0.1;

class MedicNPC : public BaseNPC
{
private:
	FSM<MedicState> fsm;
	AStar* pAStar = nullptr;
	NodeAStar* pGoalNode = nullptr;
	Position heartSoldierPosition;
	BaseNPC* targetSoldier = nullptr; // will be set by CommandNPC
	Position warehousePosition;

public:
	MedicNPC(Position p, TeamID t, Map* m);
	virtual ~MedicNPC();

	void tick() override;
	void handleOrder(Order* pOrder);
	void draw() const override;

	// Setters:
	void setTargetSoldier(BaseNPC* ts) { targetSoldier = ts; }

private:
	Position getMedicineWarehousePosition();
	
};

