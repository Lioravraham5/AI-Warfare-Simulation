#pragma once
#include "Types.h"
#include "Map.h"

// Bullet.h:

const double BULLET_SPEED = 0.1;
const int BULLET_DAMAGE = 5;
const double BULLET_RANGE = 5.0;
const int SECURITY = 1;

class Bullet
{
private:
	double x, y;
	double dirX, dirY;
	double distanceTraveled = 0.0;
	bool isActive = true;
	Map* pMap = nullptr;

public:
	Bullet(double xPos, double yPos, double angle,Map* m);
	void move();
	void draw() const;

	// Getters:
	double getX() const { return x; }
	double getY() const { return y; }
	bool getIsActive() const { return isActive; }
	
	// Setters:
	void setIsActive(bool active) { isActive = active; }
};

