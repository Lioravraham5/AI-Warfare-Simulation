#include "CommanderNPC.h"
#include "glut.h"
#include "WarriorNPC.h"
#include <iostream>

CommanderNPC::CommanderNPC(Position p, TeamID t, Map* m, MedicNPC* med, SupplierNPC* s, vector<WarriorNPC*> warriors) : BaseNPC(p, t, m), fsm(COMMANDER_IDLE)
{
	pAStar = new AStar(m);
	medic = med;
	supplier = s;
	this->warriors = warriors;
}

CommanderNPC::~CommanderNPC()
{
	delete pAStar;
}

void CommanderNPC::tick()
{
	std::cout << "DEBUG: CommanderNPC="
		<<this
		<< ", state="
		<<commanderStateToString(this->fsm.getCurrentState()) << std::endl;

	// Check if alive
	if (!isAlive) {
		fsm.setCurrentState(COMMANDER_DEAD);
		return;
	}

	// Check for death
	if (health <= 0) {
		isAlive = false;
		fsm.setCurrentState(COMMANDER_DEAD);
		return;
	}

	// Generate visibility map
	generateCombinedVisibilityMap();

	// Detect if any enemy is seen by any warrior
	bool isEnemyDetected = isEnemyDetectedByWarrior();

	std::cout << "DEBUG Commander: enemyDetected=" << isEnemyDetected << std::endl;
	
	if (isEnemyDetected) {

		// Set ATTACK mode:

		for (WarriorNPC* warrior : warriors) {
			if (warrior == nullptr || !warrior->getIsAlive())
				continue;

			Order* attackOrder = new Order(ATTACK, nullptr);
			warrior->handleOrder(attackOrder);
			std::cout << "DEBUG Commander: sending ATTACK order to warrior " << warrior
				<< std::endl;

		}
	}
	else {

		// Set DEFEND mode:

		for (WarriorNPC* warrior : warriors) {
			if (warrior == nullptr || !warrior->getIsAlive())
				continue;

			Order* defendOrder = new Order(DEFEND, nullptr);
			warrior->handleOrder(defendOrder);
			std::cout << "DEBUG Commander: sending DEFEND order to warrior " << warrior
				<< std::endl;
		}
	}

	// Commander moves to safest position in VIEW_RADIUS
	NodeBFS* pSafestPosition = findSafestPosition();
	moveOneStepToward(pSafestPosition);
}

void CommanderNPC::draw() const
{
	drawNPC('c');
}

void CommanderNPC::requestSupply(WarriorNPC* soldier)
{
	supplier->addSupplyRequest(soldier);	
}

void CommanderNPC::requestHeal(WarriorNPC* injuredSoldier)
{
	medic->addHealRequests(injuredSoldier);
}

void CommanderNPC::generateCombinedVisibilityMap()
{
	// Reset combinedVisibilityMap:
	for (int i = 0; i < MAP_SIZE; i++)
		for (int j = 0; j < MAP_SIZE; j++)
			combinedVisibilityMap[i][j] = false;
	
	for (WarriorNPC* pWarrior : warriors) {

		if (!pWarrior) 
			continue;

		const bool (*currentVisibilityMap)[MAP_SIZE] = pWarrior->getVisibilityMap();

		for (int i = 0; i < MAP_SIZE; i++)
			for (int j = 0; j < MAP_SIZE; j++) {
				if (currentVisibilityMap[i][j])
					combinedVisibilityMap[i][j] = true;
			}
	}
}

bool CommanderNPC::isEnemyDetectedByWarrior() const
{
	for (WarriorNPC* w : warriors)
	{
		if (!w || !w->getIsAlive())
			continue;

		auto vis = w->getVisibilityMap();

		for (BaseNPC* e : allEnemies)
		{
			if (!e->getIsAlive())
				continue;


			int r = e->getPosition().row;
			int c = e->getPosition().col;

			std::cout << "DEBUG: CHECK ENEMY: commander checking ("
				<< e->getPosition().row << "," << e->getPosition().col
				<< ") vis=" << vis[r][c]
				<< std::endl;

			if (vis[r][c]) {
				std::cout << "DEBUG: "
					<< "ENEMY DETECTED by commander" << std::endl;
				return true;
			}
			else {
				std::cout << "DEBUG: "
					<< "NOT VISIBLE: enemy at (" << r << "," << c << ")" << std::endl;
			}
				
		}
	}
	return false;
}

NodeBFS* CommanderNPC::findSafestPosition()
{
	queue<NodeBFS*> q;
	bool visited[MAP_SIZE][MAP_SIZE] = { false };

	NodeBFS* pStart = new NodeBFS(position.row, position.col, 0, nullptr);
	q.push(pStart);
	visited[position.row][position.col] = true;

	const int directionRow[4] = { 1, -1, 0, 0 };
	const int directionCol[4] = { 0, 0, 1, -1 };

	double bestSecurityValue = pMap->getSecurityValue(position.row, position.col);
	NodeBFS* pBestNode = pStart;

	while (!q.empty())
	{
		NodeBFS* pCurrent = q.front();
		q.pop();

		int currentRow = pCurrent->getRow();
		int currentCol = pCurrent->getCol();
		int currentDepth = pCurrent->getDepth();

		if (currentDepth > VIEW_RADIUS)
			continue;

		if (pMap->isPassable(currentRow, currentCol)) {
			double currentSecurityValue = pMap->getSecurityValue(currentRow, currentCol);
			if (currentSecurityValue < bestSecurityValue) {
				bestSecurityValue = currentSecurityValue;
				pBestNode = pCurrent;
			}
		}

		// Explore neighbors:
		for (int k=0; k < 4; k++)
		{
			int neighborRow = currentRow + directionRow[k];
			int neighborCol = currentCol + directionCol[k];

			if (!pMap->isPassable(neighborRow, neighborCol))
				continue;

			if (visited[neighborRow][neighborCol])
				continue;

			visited[neighborRow][neighborCol] = true;
			q.push(new NodeBFS(neighborRow, neighborCol, currentDepth + 1, pCurrent));

		}
	}
	return pBestNode;
}

void CommanderNPC::moveOneStepToward(NodeBFS* pSafestNode)
{
	if (!pSafestNode)
		return;

	// Already at the safest location
	if (pSafestNode->getRow() == position.row &&
		pSafestNode->getCol() == position.col)
		return;

	// Plan path using A*
	NodeAStar* path = pAStar->findPath(
		position.row, 
		position.col,
		pSafestNode->getRow(), 
		pSafestNode->getCol());

	if (path == nullptr)
		return;

	// Get next step
	NodeAStar* nextStepNode = pAStar->getNextStepTowardsTarget(
		path, 
		position.row, 
		position.col);

	if (nextStepNode == nullptr)
		return;

	// Set next step
	position.row = nextStepNode->getRow();
	position.col = nextStepNode->getCol();	
}

const char* CommanderNPC::commanderStateToString(CommanderState state)
{
	switch (state) {
	case COMMANDER_IDLE:    return "COMMANDER_IDLE";
	case COMMANDER_DEAD:  return "DOWN";
	default:return "UNKNOWN";
	}
}


