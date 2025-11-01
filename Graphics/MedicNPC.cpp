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
	case MEDIC_IDLE:
		// do nothing
		break;

	case MEDIC_MOVE_TO_WAREHOUSE:
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

	case MEDIC_MOVE_TO_SOLDIER:
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

	case MEDIC_HEALING:
		if (targetSoldier == nullptr) {
			cout << "MedicNPC.cpp: targetSoldier has not been initialize" << endl;
			fsm.setCurrentState(MEDIC_IDLE);
			return;
		}

		targetSoldier->addHealth(HEAL_PER_TICK);

		if (targetSoldier->getHealth() == MAX_HEALTH)
			fsm.setCurrentState(MEDIC_IDLE);

		break;

	case MEDIC_DEAD:
		break;
	}
}

void MedicNPC::handleOrder(Order* pOrder)
{
	if (!pOrder)
		return;

	switch (pOrder->getType())
	{
	case HEAL:
		// Go to medicine warehouse
		pGoalNode = pAStar->findPath(
			position.row,
			position.col,
			warehousePosition.row,
			warehousePosition.col);

		// Set heart soldier position
		if (pOrder->getTarget()) {
			heartSoldierPosition.row = pOrder->getTarget()->row;
			heartSoldierPosition.col = pOrder->getTarget()->col;
		}

		fsm.setCurrentState(MEDIC_MOVE_TO_WAREHOUSE);
		break;

	default:
		fsm.setCurrentState(MEDIC_IDLE);
		break;
	}

}

void MedicNPC::draw() const
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

	// Draw letter 'M' above the square
	glColor3d(0, 0, 0); // black text
	glRasterPos2d(position.col + 0.35, position.row + 1.2); // position slightly above square
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'M');
}

Position MedicNPC::getMedicineWarehousePosition()
{
	// const Warehouse& w - relate to each warehouse as a const object and get the reference of it to prevent coping
	for (const Warehouse& w : pMap->getWarehouses()) {
		if (w.getType() == MEDICINES && w.getTeamID() == teamID) {
			return { w.getRow(), w.getCol() };
		}
		return { 0,0 }; // shouldn't happen
	}
}
