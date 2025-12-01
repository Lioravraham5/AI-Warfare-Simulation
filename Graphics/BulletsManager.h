#pragma once
#include <vector>
#include "Bullet.h"
#include "Grenade.h"
#include "BaseNPC.h"
#include "Map.h"

// BulletsManager.h:

class BulletsManager
{
private:
	vector<Bullet*> bullets;
	vector<BaseNPC*> allNPCs;
	Map* pMap = nullptr;

public:
    BulletsManager(Map* m);
    void addBullet(Bullet* b);
    void addGrenade(Grenade* g);
    void addNPC(BaseNPC* npc);
    void update();
    void drawBullets() const;

private:
    void moveBullets();
    void detectHits();
};

