#pragma once
#include "WarriorNPC.h"
#include "FSM.h"
#include "AStar.h"
#include <unordered_set>

// SupplierNPC.h

enum SupplierState {
	SUPPLIER_IDLE,
	SUPPLIER_MOVE_TO_WAREHOUSE,
	SUPPLIER_MOVE_TO_SOLDIER,
	SUPPLIER_SUPPLYING,
	SUPPLIER_DEAD
};

const double SUPPLY_PER_TICK = 1;

// Forward declaration:
class WarriorNPC;

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

	queue<WarriorNPC*> supplyRequests;
	unordered_set<WarriorNPC*> supplyRequestsSet;

public:
	SupplierNPC(Position p, TeamID t, Map* m);
	virtual ~SupplierNPC();

	void tick() override;
	void handleOrder(Order* pOrder);
	void draw() const override;
	
	void addSupplyRequest(WarriorNPC* soldier);

	// Setters:
	void setTargetSoldier(WarriorNPC* ts) { targetSoldier = ts; }

private:
	Position getWeaponsWarehousePosition();
	void handleSupplyRequest();
};

