#include "CommanderNPC.h"
#include "glut.h"
#include "WarriorNPC.h"

CommanderNPC::CommanderNPC(Position p, TeamID t, Map* m) : BaseNPC(p, t, m)
{
	pAStar = new AStar(m);
}

CommanderNPC::~CommanderNPC()
{
	delete pAStar;
}

void CommanderNPC::tick()
{
	// Check if alive
	if (!isAlive) {
		return;
	}

	if (health <= 0) {
		isAlive = false;
		return;
	}

	generateCombinedVisibilityMap();



}

void CommanderNPC::draw() const
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

	// draw commander as a square
	glBegin(GL_POLYGON);
	glVertex2d(position.col, position.row);
	glVertex2d(position.col, position.row + 1);
	glVertex2d(position.col + 1, position.row + 1);
	glVertex2d(position.col + 1, position.row);
	glEnd();

	// Draw letter 'C' above the square
	glColor3d(0, 0, 0); // black text
	glRasterPos2d(position.col + 0.35, position.row + 1.2); // position slightly above square
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'C');
}

void CommanderNPC::requestSupply(BaseNPC* soldier)
{
	if (!soldier)
		return;

	// avoid duplicate requests:
	for (BaseNPC* request : supplyRequests)
		if (request == soldier)
			return;

	supplyRequests.push_back(soldier);
}

void CommanderNPC::requestHeal(BaseNPC* soldier)
{
	if (!soldier)
		return;

	// avoid duplicate requests:
	for (BaseNPC* request : healRequests)
		if (request == soldier)
			return;

	healRequests.push_back(soldier);
}

void CommanderNPC::generateCombinedVisibilityMap()
{
	// Reset combinedVisibilityMap:
	for (int i = 0; i < MAP_SIZE; i++)
		for (int j = 0; j < MAP_SIZE; j++)
			combinedVisibilityMap[i][j] = false;
	
	for (BaseNPC* soldier : teamMembers) {
		auto pWarrior = dynamic_cast<WarriorNPC*>(soldier);

		if (!pWarrior) 
			continue;

		const bool (*currentVisibilityMap)[MAP_SIZE] = pWarrior->getVisibilityMap();

		for (int i = 0; i < MAP_SIZE; i++)
			for (int j = 0; j < MAP_SIZE; j++) {
				if (currentVisibilityMap[i][j])
					combinedVisibilityMap[i][j] = true;
			}
	}
}
