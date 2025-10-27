#include "Bullet.h"
#include <math.h>
#include "glut.h"

// Bullet.cpp:

Bullet::Bullet(double xPos, double yPos, double angle, Map* m)
{
	x = xPos;
	y = yPos;
	dirX = cos(angle);
	dirY = sin(angle);
	pMap = m;

}

void Bullet::move()
{
	// If bullet is not active, do nothing
	if (!isActive)
		return;
	
	// Compute potential next position
	double nextX = x + BULLET_SPEED * dirX;
	double nextY = y + BULLET_SPEED * dirY;

	// Convert to map indices for collision detection
	int cellX = (int)nextX;
	int cellY = (int)nextY;

	// Check for collision with obstacles or out of bounds
	if (cellX < 0 || cellX >= MAP_SIZE || cellY < 0 || cellY >= MAP_SIZE ||
		pMap->getCellType(cellX, cellY) == ROCK || 
		pMap->getCellType(cellX, cellY) == TREE)
	{
		isActive = false; // Deactivate bullet on collision
		return;
	}

	// Update bullet position
	x = nextX;
	y = nextY;

	// Update security map danger level
	double current = pMap->getSecurityValue(cellY, cellX);
	pMap->setSecurityValue(cellY, cellX, current + SECURITY);
}

void Bullet::draw()
{
	if(!isActive)
		return;

	glColor3d(0.3, 0.3, 0.3); // Dark gray color for bullet
	glBegin(GL_POLYGON);
	glVertex2d(x - 0.5, y); 
	glVertex2d(x, y + 0.5);
	glVertex2d(x + 0.5, y);
	glVertex2d(x, y - 0.5);
	glEnd();
}

