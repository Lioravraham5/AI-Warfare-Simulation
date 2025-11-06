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
		// do nothing
		break;

	case SUPPLIER_MOVE_TO_WAREHOUSE:
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

	case SUPPLIER_MOVE_TO_SOLDIER:
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

void SupplierNPC::handleOrder(Order* pOrder)
{
	if (!pOrder)
		return;

	switch (pOrder->getType())
	{
	case RESUPPLY:
		// Go to weapons warehouse
		pGoalNode = pAStar->findPath(
			position.row,
			position.col,
			warehousePosition.row,
			warehousePosition.col);

		// Set soldier position
		if (pOrder->getTarget()) {
			supplySoldierPosition.row = pOrder->getTarget()->row;
			supplySoldierPosition.col = pOrder->getTarget()->col;
		}

		fsm.setCurrentState(SUPPLIER_MOVE_TO_WAREHOUSE);
		break;

	default:
		fsm.setCurrentState(SUPPLIER_IDLE);
		break;
	}

}

void SupplierNPC::draw() const
{
	switch (teamID)
	{
	case TEAM_1:
		glColor3d(0, 0, 1); // Blue for Team 1
		break;
	case TEAM_2:
		glColor3d(1, 0, 0); // Red for Team 2
		break;
	}

	// draw medic as a square
	glBegin(GL_POLYGON);
	glVertex2d(position.col, position.row);
	glVertex2d(position.col, position.row + 1);
	glVertex2d(position.col + 1, position.row + 1);
	glVertex2d(position.col + 1, position.row);
	glEnd();

	// Draw letter 'P' above the square
	glColor3d(0, 0, 0); // black text
	glRasterPos2d(position.col + 0.35, position.row + 1.2); // position slightly above square
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'P');
}

Position SupplierNPC::getWeaponsWarehousePosition()
{
	// const Warehouse& w - relate to each warehouse as a const object and get the reference of it to prevent coping
	for (const Warehouse& w : pMap->getWarehouses()) {
		if (w.getType() == WEAPONS && w.getTeamID() == teamID) {
			return { w.getRow(), w.getCol() };
		}
		return { 0,0 }; // shouldn't happen
	}
}
