#pragma once
#include "Bullet.h"
#include <vector>

// Grenade.h:

const int GRENADE_BULLETS = 20; // Number of bullets released by grenade

class Grenade
{
private:
	double x, y;
	bool isExploded = true;
	Bullet* bullets[GRENADE_BULLETS];
	Map* pMap = nullptr;

public:
	Grenade(double xPos, double yPos, Map* m);
	void Explode();
	void setIsExploded(bool value);
	void draw();
};

