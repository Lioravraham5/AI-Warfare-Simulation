#include "Warehouse.h"
#include "glut.h"
#include <string>
#include "Bullet.h"
#include "BaseNPC.h"
using namespace std;

Warehouse::Warehouse(int r, int c, TeamID team, WarehouseType t)
{
	row = r;
	col = c;
	teamID = team;
	type = t;
}

void Warehouse::draw() const
{
	switch (type)
	{
	case WEAPONS:
		drawWeapons();
		break;
	case MEDICINES:
		drawMedicines();
		break;
	default:
		break;
	}
}

void Warehouse::drawWeapons() const
{
	glColor3d(1.0, 1.0, 0.0); // Yellow

	// Draw cell as a square
	glBegin(GL_POLYGON);
	glVertex2d(col, row); // bottom-left
	glVertex2d(col, row + WAREHOUSE_SIZE); // top-left
	glVertex2d(col + WAREHOUSE_SIZE, row + WAREHOUSE_SIZE); // top-right
	glVertex2d(col + WAREHOUSE_SIZE, row); // bottom-right
	glEnd();

	// Write "WW" in the center of the cell
	glColor3d(0.0, 0.0, 0.0); // Black
	string label = "WW";
	glRasterPos2d(col + 0.5, row + 1.7);
	for (char ch : label)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ch);

	//Write team ID
	string teamIDStr = to_string(teamID);
	glRasterPos2d(col + 1.1, row + 0.2);
	for (char ch : teamIDStr)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ch);
}

void Warehouse::drawMedicines() const
{
	glColor3d(1.0, 1.0, 0.0); // Yellow

	// Draw cell as a square
	glBegin(GL_POLYGON);
	glVertex2d(col, row); // bottom-left
	glVertex2d(col, row + WAREHOUSE_SIZE); // top-left
	glVertex2d(col + WAREHOUSE_SIZE, row + WAREHOUSE_SIZE); // top-right
	glVertex2d(col + WAREHOUSE_SIZE, row); // bottom-right
	glEnd();

	// Write "MW" in the center of the cell
	glColor3d(0.0, 0.0, 0.0); // Black
	string label = "MW";
	glRasterPos2d(col + 0.5, row + 1.7);
	for (char ch : label)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ch);
	
	//Write team ID
	string teamIDStr = to_string(teamID);
	glRasterPos2d(col + 1.1, row + 0.2);
	for (char ch : teamIDStr)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ch);
}
