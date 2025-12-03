#include "SupplierNPC.h"
#include <iostream>

// SupplierNPC.cpp

SupplierNPC::SupplierNPC(Position p, TeamID t, Map* m) : BaseNPC(p, t, m), fsm(SUPPLIER_IDLE)
{
	pAStar = new AStar(m);
	warehousePosition = getWeaponsWarehousePosition();
}

SupplierNPC::~SupplierNPC()
{
	delete pAStar;
}

void SupplierNPC::tick()
{
	std::cout << "DEBUG: SupplierNPC="
		<< this
		<< ", state="
		<< supplierStateToString(this->fsm.getCurrentState()) << std::endl;

	// Check if alive
	if (!isAlive) {
		fsm.setCurrentState(SUPPLIER_DEAD);
		return;
	}

	// Check for death
	if (health <= 0) {
		isAlive = false;
		fsm.setCurrentState(SUPPLIER_DEAD);
		return;
	}

	switch (fsm.getCurrentState())
	{
	case SUPPLIER_IDLE:
		handleSupplyRequest();
		break;

	case SUPPLIER_MOVE_TO_WAREHOUSE: {
		NodeAStar* pNext = pAStar->getNextStepTowardsTarget(
			pGoalNode,
			position.row,
			position.col);

		if (pNext) {
			position.row = pNext->getRow();
			position.col = pNext->getCol();

			if (position.row == warehousePosition.row &&
				position.col == warehousePosition.col) {

				// Supplier reached to medicine warehouse, calculate the path to the heart soldier
				pGoalNode = pAStar->findPath(
					position.row,
					position.col,
					supplySoldierPosition.row,
					supplySoldierPosition.col);
				fsm.setCurrentState(SUPPLIER_MOVE_TO_SOLDIER);
			}
		}
		break;
	}

	case SUPPLIER_MOVE_TO_SOLDIER: {
		NodeAStar* pNext = pAStar->getNextStepTowardsTarget(
			pGoalNode,
			position.row,
			position.col);

		if (pNext) {
			position.row = pNext->getRow();
			position.col = pNext->getCol();

			if (position.row == supplySoldierPosition.row &&
				position.col == supplySoldierPosition.col) {

				// Medic reached to heart soldier
				pGoalNode = pAStar->findPath(
					position.row,
					position.col,
					supplySoldierPosition.row,
					supplySoldierPosition.col);
				fsm.setCurrentState(SUPPLIER_SUPPLYING);
			}
		}
		break;
	}

	case SUPPLIER_SUPPLYING:
		if (targetSoldier == nullptr) {
			cout << "SupplierNPC.cpp: targetSoldier has not been initialize" << endl;
			fsm.setCurrentState(SUPPLIER_IDLE);
			return;
		}

		targetSoldier->addSupply(SUPPLY_PER_TICK);

		if (targetSoldier->getBullets() == MAX_BULLETS 
			&& targetSoldier->getGrenades() == MAX_GRENADES)
			fsm.setCurrentState(SUPPLIER_IDLE);

		break;

	case SUPPLIER_DEAD:
		break;
	}
}

void SupplierNPC::draw() const
{
	drawNPC('p');
}

void SupplierNPC::addSupplyRequest(WarriorNPC* soldier)
{
	// Exit if the soldier is dead or nullptr
	if (soldier == nullptr || !soldier->getIsAlive())
		return;

	// avoid duplicate requests:
	if (supplyRequestsSet.find(soldier) != supplyRequestsSet.end())
		return; // this soldier already has an active request 

	// add soldier
	supplyRequests.push(soldier);
	supplyRequestsSet.insert(soldier);
}

Position SupplierNPC::getWeaponsWarehousePosition()
{
	// const Warehouse& w - relate to each warehouse as a const object and get the reference of it to prevent coping
	for (const Warehouse& w : pMap->getWarehouses()) {
		if (w.getType() == WEAPONS && w.getTeamID() == teamID) {
			return { w.getRow(), w.getCol() };
		}
	}
	return { 0,0 }; // shouldn't happen
}

void SupplierNPC::handleSupplyRequest()
{
	// Exit if there is no heal requests 
	if (supplyRequests.empty())
		return;

	// get soldier
	WarriorNPC* suppliedSoldier = supplyRequests.front();
	supplyRequests.pop();
	supplyRequestsSet.erase(suppliedSoldier);

	if (suppliedSoldier == nullptr || !suppliedSoldier->getIsAlive())
		return;

	targetSoldier = suppliedSoldier;
	supplySoldierPosition = suppliedSoldier->getPosition();

	// Go to medicine warehouse
	pGoalNode = pAStar->findPath(
		position.row,
		position.col,
		warehousePosition.row,
		warehousePosition.col);

	fsm.setCurrentState(SUPPLIER_MOVE_TO_WAREHOUSE);
}

const char* SupplierNPC::supplierStateToString(SupplierState state)
{
	switch (state) {
	case SUPPLIER_IDLE:    return "SUPPLIER_IDLE";
	case SUPPLIER_MOVE_TO_WAREHOUSE:  return "SUPPLIER_MOVE_TO_WAREHOUSE";
	case SUPPLIER_MOVE_TO_SOLDIER:  return "SUPPLIER_MOVE_TO_SOLDIER";
	case SUPPLIER_SUPPLYING:  return "SUPPLIER_SUPPLYING";
	case SUPPLIER_DEAD:  return "SUPPLIER_DEAD";
	default:return "UNKNOWN";
	}
}
