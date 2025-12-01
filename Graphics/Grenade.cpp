#include "Grenade.h"

Grenade::Grenade(double xPos, double yPos, Map* m)
{
	// Calculate angle increment for circular bullet distribution
	double angle, teta = (360 / GRENADE_BULLETS) * 3.14 / 180.0;

	// Set grenade position
	x = xPos;
	y = yPos;

	// Initialize bullets in a circular pattern
	for (int i = 0, angle = 0; i < GRENADE_BULLETS; i++, angle += teta)
	{
		bullets[i] = new Bullet(x, y, angle, m);
	}
	
	// Initially, grenade is not exploded
	isExploded = false;

	pMap = m;
}

void Grenade::draw()
{
	for (int i = 0; i < GRENADE_BULLETS; i++)
	{
		bullets[i]->draw();
	}
}
