#pragma once
#include "WarriorNPC.h"
#include "FSM.h"
#include "AStar.h"

// SupplierNPC.h

enum SupplierState {
	SUPPLIER_IDLE,
	SUPPLIER_MOVE_TO_WAREHOUSE,
	SUPPLIER_MOVE_TO_SOLDIER,
	SUPPLIER_SUPPLYING,
	SUPPLIER_DEAD
};

const double SUPPLY_PER_TICK = 1;

class SupplierNPC :
    public BaseNPC
{
private:
	FSM<SupplierState> fsm;
	AStar* pAStar = nullptr;
	NodeAStar* pGoalNode = nullptr;
	Position supplySoldierPosition;
	WarriorNPC* targetSoldier = nullptr; // will be set by CommandNPC
	Position warehousePosition;

public:
	SupplierNPC(Position p, TeamID t, Map* m);
	virtual ~SupplierNPC();

	void tick() override;
	void handleOrder(Order* pOrder);
	void draw() const override;

	// Setters:
	void setTargetSoldier(WarriorNPC* ts) { targetSoldier = ts; }

private:
	Position getWeaponsWarehousePosition();
};

