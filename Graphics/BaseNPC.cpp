#include "BaseNPC.h"

// BaseNPC.cpp:

BaseNPC::BaseNPC(Position p, TeamID t, Map* m)
{
	position = p;
	teamID = t;
	pMap = m;
}

void BaseNPC::addHealth(double val)
{
	health += val;
	if (health >= MAX_HEALTH)
		health = MAX_HEALTH;
}

void BaseNPC::subHealth(double val)
{
	health -= val;
	if (health <= 0)
		health = 0;
}

void BaseNPC::drawNPC(char symbol) const
{
    // Set color based on teamID
    switch (teamID)
    {
    case TEAM_1:
        glColor3d(0, 0, 1); // Blue
        break;
    case TEAM_2:
        glColor3d(1, 0, 0); // Red
        break;
    }

    // Draw square
    glBegin(GL_POLYGON);
    glVertex2d(position.col, position.row);
    glVertex2d(position.col, position.row + 1);
    glVertex2d(position.col + 1, position.row + 1);
    glVertex2d(position.col + 1, position.row);
    glEnd();

    // Draw character above square
    glColor3d(0, 0, 0); // black text
    glRasterPos2d(position.col + 0.35, position.row + 1.2);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, symbol);

    // Draw Health Bar
    double healthRatio = health / MAX_HEALTH;  // value between 0 and 1
    double barLength = healthRatio * 1.0;      // max width = 1 world unit

    // Bar start position (below square)
    double barY = position.row - 0.2;
    double barX = position.col;

    // Background bar (gray) – full width
    glColor3d(0.4, 0.4, 0.4);
    glBegin(GL_POLYGON);
    glVertex2d(barX, barY);
    glVertex2d(barX + 1.0, barY);
    glVertex2d(barX + 1.0, barY - 0.12);
    glVertex2d(barX, barY - 0.12);
    glEnd();

    // Choose bar color based on health
    if (healthRatio > 0.66)
        glColor3d(0.0, 1.0, 0.0);   // Green
    else if (healthRatio > 0.33)
        glColor3d(1.0, 1.0, 0.0);   // Yellow
    else
        glColor3d(1.0, 0.0, 0.0);   // Red

    // Health bar (green, scaled by life)
    glBegin(GL_POLYGON);
    glVertex2d(barX, barY);
    glVertex2d(barX + barLength, barY);
    glVertex2d(barX + barLength, barY - 0.12);
    glVertex2d(barX, barY - 0.12);
    glEnd();
}
