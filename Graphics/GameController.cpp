#include "GameController.h"

GameController::GameController()
{
    pMap = new Map();
    bulletsManager = new BulletsManager(pMap);

    initGame(); 		// init all NPCs
    connectTeams();		// give each commander access to its team warriors
}

GameController::~GameController()
{
    delete pMap;
    delete bulletsManager;

    for (BaseNPC* npc : allNPCs)
        delete npc;
}

void GameController::initGame()
{
    // Team 1

    medic1 = new MedicNPC({ 3,3 }, TEAM_1, pMap);
    supplier1 = new SupplierNPC({ 3,1 }, TEAM_1, pMap);

    warrior1A = new WarriorNPC({ 4,2 }, TEAM_1, pMap, nullptr, bulletsManager);
    warrior1B = new WarriorNPC({ 5,4 }, TEAM_1, pMap, nullptr, bulletsManager);

    vector<WarriorNPC*> team1Warriors = { warrior1A, warrior1B };

    commander1 = new CommanderNPC({ 2,2 }, TEAM_1, pMap, medic1, supplier1, team1Warriors);

    warrior1A->setCommander(commander1);
    warrior1B->setCommander(commander1);

    // Team 2
    medic2 = new MedicNPC({ MAP_SIZE - 4, MAP_SIZE - 2 }, TEAM_2, pMap);
    supplier2 = new SupplierNPC({ MAP_SIZE - 4, MAP_SIZE - 5 }, TEAM_2, pMap);

    warrior2A = new WarriorNPC({ MAP_SIZE - 5, MAP_SIZE - 4 }, TEAM_2, pMap, nullptr, bulletsManager);
    warrior2B = new WarriorNPC({ MAP_SIZE - 6, MAP_SIZE - 3 }, TEAM_2, pMap, nullptr, bulletsManager);

    vector<WarriorNPC*> team2Warriors = { warrior2A, warrior2B };

    commander2 = new CommanderNPC({ MAP_SIZE - 3, MAP_SIZE - 3 },
        TEAM_2, pMap,
        medic2, supplier2,
        team2Warriors);

    warrior2A->setCommander(commander2);
    warrior2B->setCommander(commander2);

    registerNPC(commander1);
    registerNPC(warrior1A);
    registerNPC(warrior1B);
    registerNPC(medic1);
    registerNPC(supplier1);

    registerNPC(commander2);
    registerNPC(warrior2A);
    registerNPC(warrior2B);
    registerNPC(medic2);
    registerNPC(supplier2);
}

void GameController::registerNPC(BaseNPC* npc)
{
    allNPCs.push_back(npc);
    bulletsManager->addNPC(npc);
}

void GameController::connectTeams()
{
    vector<BaseNPC*> team1Enemies = {
       commander2, warrior2A, warrior2B, medic2, supplier2
    };

    vector<BaseNPC*> team2Enemies = {
        commander1, warrior1A, warrior1B, medic1, supplier1
    };

    warrior1A->setEnemies(team1Enemies);
    warrior1B->setEnemies(team1Enemies);

    warrior2A->setEnemies(team2Enemies);
    warrior2B->setEnemies(team2Enemies);
}

void GameController::tick()
{
    if (isGameOver())
        return;

    bulletsManager->update();

    // tick for all NPCs:
    for (BaseNPC* npc : allNPCs)
        npc->tick();
}

void GameController::drawGame()
{
    pMap->drawMap();

    for (BaseNPC* npc : allNPCs)
        npc->draw();

    bulletsManager->drawBullets();
}

bool GameController::isGameOver() const
{
    bool isTeam1Dead = !commander1->getIsAlive() &&
        !warrior1A->getIsAlive() &&
        !warrior1B->getIsAlive() &&
        !medic1->getIsAlive() &&
        !supplier1->getIsAlive();

    bool isTeam2Dead = !commander2->getIsAlive() &&
        !warrior2A->getIsAlive() &&
        !warrior2B->getIsAlive() &&
        !medic2->getIsAlive() &&
        !supplier2->getIsAlive();

    return isTeam1Dead || isTeam2Dead;
}