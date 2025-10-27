#include "WarriorNPC.h"
#include "glut.h"

// WarriorNPC.cpp:

WarriorNPC::WarriorNPC(Position p, TeamID t, Map* m) : BaseNPC(p, t, m), fsm(IDLE)
{
	bullets = MAX_BULLETS;
	grenades = MAX_GRENADES;

}

void WarriorNPC::tick()
{
	// Check if alive
	if(!isAlive) {
		fsm.setCurrentState(DEAD);
		return;
	}

	// Check for death
	if(health <= 0) {
		isAlive = false;
		fsm.setCurrentState(DEAD);
		return;
	}

	// Check health status
	if (health < 0.25 * MAX_HEALTH) {
		fsm.setCurrentState(WAIT_FOR_HEALING);
		// ADD: notify commander about low health
	}

	if(bullets < 0.25 * MAX_BULLETS && grenades < 0.25 * MAX_GRENADES) {
		fsm.setCurrentState(WAIT_FOR_SUPPLY);
		// ADD: notify commander about low ammo
	}
	
	// fsm behavior
	switch (fsm.getCurrentState())
	{
	case IDLE:
		// do nothing
		break;

	case MOVE_TO_GIVEN_POSITION:
		// ADD: implement A* movement to given position
		break;

	case MOVE_TO_ATTACK_POSITION:
		// ADD: implement A* movement to attack position
		break;

	case ATTACKING:
		// ADD: implement attacking behavior
		break;

	case SEARCH_FOR_NEAREST_COVER_POSITION:
		// ADD: implement BFS search for nearest cover position
		break;

	case MOVE_TO_COVER_POSITION:
		// ADD: implement A* movement to cover position
		break;

	case WAIT_FOR_SUPPLY:
		// ADD: implement waiting for supply behavior
		break;

	case IN_SUPPLY_PROCESS:
		// ADD: implement in supply process behavior
		break;

	case WAIT_FOR_HEALING:
		// ADD: implement waiting for healing behavior
		break;

	case IN_HEALING_PROCESS:
		// ADD: implement in healing process behavior
		break;

	case DEAD:
		// do nothing
		break;

	default:
		break;
	}
}

void WarriorNPC::draw() const
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

	// draw warrior as a square
	glBegin(GL_POLYGON);
	glVertex2d(position.col, position.row);
	glVertex2d(position.col, position.row + 1);
	glVertex2d(position.col + 1, position.row + 1);
	glVertex2d(position.col + 1, position.row);
	glEnd();

	// Draw letter 'W' above the square
	glColor3d(0, 0, 0); // black text
	glRasterPos2d(position.col + 0.35, position.row + 1.2); // position slightly above square
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'W');

}
