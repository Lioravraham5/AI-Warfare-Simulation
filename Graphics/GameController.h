#pragma once
#include "Map.h"
#include "CommanderNPC.h"
#include "WarriorNPC.h"
#include "MedicNPC.h"
#include "SupplierNPC.h"
#include "BulletsManager.h"
#include <vector>

class GameController
{
private:
    Map* pMap;
    BulletsManager* bulletsManager;

    // Team 1
    CommanderNPC* commander1;
    WarriorNPC* warrior1A;
    WarriorNPC* warrior1B;
    MedicNPC* medic1;
    SupplierNPC* supplier1;

    // Team 2
    CommanderNPC* commander2;
    WarriorNPC* warrior2A;
    WarriorNPC* warrior2B;
    MedicNPC* medic2;
    SupplierNPC* supplier2;

    vector<BaseNPC*> allNPCs;

public:
    GameController();
    ~GameController();

    void initGame();
    void tick();
    void drawGame();

private:
    void registerNPC(BaseNPC* npc);
    void connectTeams();
    bool isGameOver() const;
};

