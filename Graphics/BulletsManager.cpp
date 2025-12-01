#include "BulletsManager.h"
#include <cmath>

// BulletsManager.cpp:

BulletsManager::BulletsManager(Map* m)
{
	pMap = m;
}

void BulletsManager::addBullet(Bullet* b)
{
	b->setIsActive(true);
	bullets.push_back(b);
}

void BulletsManager::addGrenade(Grenade* g)
{

	Bullet** grenadesBullets = g->getBullets();

	for (int i = 0; i < GRENADE_BULLETS; i++)
	{
		addBullet(grenadesBullets[i]);
	}
}

void BulletsManager::addNPC(BaseNPC* npc)
{
	allNPCs.push_back(npc);
}

void BulletsManager::update()
{
	moveBullets();
	detectHits();
}

void BulletsManager::drawBullets() const
{
	for (Bullet* bullet : bullets)
	{
		if (bullet != nullptr)
			bullet->draw();
	}
}

void BulletsManager::moveBullets()
{
	for (Bullet* b : bullets)
	{
		if (b != nullptr)
			b->move();
	}
}

// Check collision between bullet and NPCs
void BulletsManager::detectHits()
{
	for (Bullet* bullet : bullets) {
		if (bullet == nullptr)
			continue;

		if (!bullet->getIsActive())
			continue;

		int bulletRow = (int)bullet->getY();
		int bulletCol = (int)bullet->getX();

		// Iterate on all the NPCs and check collision
		for (BaseNPC* npc : allNPCs){
			
			if (npc == nullptr)
				continue;

			if (!npc->getIsAlive())
				continue;

			int npcRow = npc->getPosition().row;
			int npcCol = npc->getPosition().col;

			if (abs(bulletRow - npcRow) <= 1 && abs(bulletCol - npcCol) <= 1) {
				
				// Collision detected !
				npc->subHealth(BULLET_DAMAGE);
				bullet->setIsActive(false);
				break;

			}
		}
	}
}
