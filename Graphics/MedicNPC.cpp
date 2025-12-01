#include "MedicNPC.h"
#include <iostream>

// MedicNPC.cpp:

MedicNPC::MedicNPC(Position p, TeamID t, Map* m) : BaseNPC(p, t, m), fsm(MEDIC_IDLE)
{
	pAStar = new AStar(m);
	warehousePosition = getMedicineWarehousePosition();
}

MedicNPC::~MedicNPC()
{
	delete pAStar;
}

void MedicNPC::tick()
{
	// Check if alive
	if (!isAlive) {
		fsm.setCurrentState(MEDIC_DEAD);
		return;
	}

	// Check for death
	if (health <= 0) {
		isAlive = false;
		fsm.setCurrentState(MEDIC_DEAD);
		return;
	}

	switch (fsm.getCurrentState())
	{
		case MEDIC_IDLE: {
			handleHealRequest();
			break;
		}

		case MEDIC_MOVE_TO_WAREHOUSE: {
			NodeAStar* pNext = pAStar->getNextStepTowardsTarget(
				pGoalNode,
				position.row,
				position.col);

			if (pNext) {
				position.row = pNext->getRow();
				position.col = pNext->getCol();

				if (position.row == warehousePosition.row &&
					position.col == warehousePosition.col) {

					// Medic reached to medicine warehouse, calculate the path to the heart soldier
					pGoalNode = pAStar->findPath(
						position.row,
						position.col,
						heartSoldierPosition.row,
						heartSoldierPosition.col);
					fsm.setCurrentState(MEDIC_MOVE_TO_SOLDIER);
				}
			}
			break;
		}

		case MEDIC_MOVE_TO_SOLDIER: {
			NodeAStar* pNext = pAStar->getNextStepTowardsTarget(
				pGoalNode,
				position.row,
				position.col);

			if (pNext) {
				position.row = pNext->getRow();
				position.col = pNext->getCol();

				if (position.row == heartSoldierPosition.row &&
					position.col == heartSoldierPosition.col) {

					// Medic reached to heart soldier
					pGoalNode = pAStar->findPath(
						position.row,
						position.col,
						heartSoldierPosition.row,
						heartSoldierPosition.col);
					fsm.setCurrentState(MEDIC_HEALING);
				}
			}
			break;
		}


		case MEDIC_HEALING: {
			if (targetSoldier == nullptr || !targetSoldier->getIsAlive()) {
				cout << "MedicNPC.cpp: targetSoldier has not been initialize" << endl;
				fsm.setCurrentState(MEDIC_IDLE);
				return;
			}

			targetSoldier->addHealth(HEAL_PER_TICK);

			if (targetSoldier->getHealth() == MAX_HEALTH)
				fsm.setCurrentState(MEDIC_IDLE);

			break;
		}

		case MEDIC_DEAD:
			break;
	}
}

void MedicNPC::draw() const
{
	drawNPC('m');
}

void MedicNPC::addHealRequests(WarriorNPC* soldier)
{
	// Exit if the soldier is dead or nullptr
	if (soldier == nullptr || !soldier->getIsAlive())
		return;
	
	// avoid duplicate requests:
	if (healRequestsSet.find(soldier) != healRequestsSet.end())
		return; // this soldier already has an active request 

	healRequest.push(soldier);
	healRequestsSet.insert(soldier);
}

Position MedicNPC::getMedicineWarehousePosition()
{
	// const Warehouse& w - relate to each warehouse as a const object and get the reference of it to prevent coping
	for (const Warehouse& w : pMap->getWarehouses()) {
		if (w.getType() == MEDICINES && w.getTeamID() == teamID) {
			return { w.getRow(), w.getCol() };
		}
	}
	return { 0,0 }; // shouldn't happen
}

void MedicNPC::handleHealRequest()
{
	// Exit if there is no heal requests 
	if (healRequest.empty())
		return;
	
	// get soldier
	WarriorNPC* injuredSoldier = healRequest.front();
	healRequest.pop();
	healRequestsSet.erase(injuredSoldier);
	
	if (injuredSoldier == nullptr || !injuredSoldier->getIsAlive())
		return;

	targetSoldier = injuredSoldier;
	heartSoldierPosition = injuredSoldier->getPosition();

	// Go to medicine warehouse
	pGoalNode = pAStar->findPath(
		position.row,
		position.col,
		warehousePosition.row,
		warehousePosition.col);

	fsm.setCurrentState(MEDIC_MOVE_TO_WAREHOUSE);
}
