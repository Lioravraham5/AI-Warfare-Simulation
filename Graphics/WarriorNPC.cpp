#include "WarriorNPC.h"
#include "glut.h"
#include "Bullet.h"
#include "Grenade.h"
#include <queue>
#include "AStar.h"
#include "NodeBFS.h"
#include <iostream>

// WarriorNPC.cpp:

WarriorNPC::WarriorNPC(Position p, TeamID t, Map* m, CommanderNPC* c, BulletsManager* bm) : BaseNPC(p, t, m), fsm(WARRIOR_IDLE)
{
	bullets = MAX_BULLETS;
	grenades = MAX_GRENADES;
	pAStar = new AStar(m);
	pCommander = c;
	bulletsManager = bm;
}

WarriorNPC::~WarriorNPC()
{
	delete pAStar;
}

void WarriorNPC::tick()
{
	std::cout << "DEBUG: WarriorNPC="
		<< this
		<< ", state="
		<< warriorStateToString(this->fsm.getCurrentState()) << std::endl;

	// Check if alive
	if(!isAlive) {
		fsm.setCurrentState(WARRIOR_DEAD);
		return;
	}

	// Check for death
	if(health <= 0) {
		isAlive = false;
		fsm.setCurrentState(WARRIOR_DEAD);
		return;
	}

	// Check health status
	if (health < 0.25 * MAX_HEALTH) {
		fsm.setCurrentState(WARRIOR_WAIT_FOR_HEALING);
		pCommander->requestHeal(this);
		fsm.setCurrentState(WARRIOR_WAIT_FOR_HEALING);
		return;
	}

	if(bullets < 0.25 * MAX_BULLETS && grenades < 0.25 * MAX_GRENADES) {
		fsm.setCurrentState(WARRIOR_WAIT_FOR_SUPPLY);
		pCommander->requestSupply(this);
		fsm.setCurrentState(WARRIOR_WAIT_FOR_SUPPLY);
	}

	updateVisibilityMap();
	
	// fsm behavior
	switch (fsm.getCurrentState())
	{
	case WARRIOR_IDLE:
		// do nothing
		break;

	case WARRIO_MOVE_TO_GIVEN_POSITION:
	case WARRIOR_MOVE_TO_COVER_POSITION: {
		NodeAStar* pNextStep = pAStar->getNextStepTowardsTarget(
			pGoalNode,
			position.row,
			position.col);
		if (pNextStep) {
			position.row = pNextStep->getRow();
			position.col = pNextStep->getCol();

			if (position.row == pSafestNode->getRow() &&
				position.col == pSafestNode->getCol()) {
				fsm.setCurrentState(WARRIOR_IDLE);
			}
		}
		else {
			// Reached destination
			fsm.setCurrentState(WARRIOR_IDLE);
		}
		break;
	}

	case WARRIOR_MOVE_TO_ATTACK_POSITION: {
		NodeAStar* pNextStep = pAStar->getNextStepTowardsTarget(
			pGoalNode,
			position.row,
			position.col);
		if (pNextStep) {
			position.row = pNextStep->getRow();
			position.col = pNextStep->getCol();
		}
		else {
			// Reached for the best attack position
			fsm.setCurrentState(WARRIOR_ATTACKING);
		}
		break;
	}

	case WARRIOR_ATTACKING:
		attackBehavior();
		break;

	case WARRIOR_WAIT_FOR_SUPPLY:
		// ADD: implement waiting for supply behavior
		break;

	case WARRIOR_IN_SUPPLY_PROCESS:
		// ADD: implement in supply process behavior
		break;

	case WARRIOR_WAIT_FOR_HEALING:
		// ADD: implement waiting for healing behavior
		break;

	case WARRIOR_IN_HEALING_PROCESS:
		// ADD: implement in healing process behavior
		if (health == MAX_HEALTH)
			fsm.setCurrentState(WARRIOR_IDLE);
		break;

	case WARRIOR_DEAD:
		// do nothing
		break;

	default:
		break;
	}
}

void WarriorNPC::handleOrder(Order* pOrder)
{
	std::cout << "DEBUG Warrior::handleOrder currentState="
		<< warriorStateToString(fsm.getCurrentState())
		<< " order=" << pOrder->getType()
		<< std::endl;

	switch (pOrder->getType())
	{
	case MOVE_TO_POSITION: {
		pGoalNode = pAStar->findPath(
			position.row,
			position.col,
			pOrder->getTarget()->row,
			pOrder->getTarget()->col);

		if (!pGoalNode)
			std::cout << "DEBUG Warrior: A* returned NULL path" << std::endl;
		else
			std::cout << "DEBUG Warrior: A* path found for MOVE order" << std::endl;

		fsm.setCurrentState(WARRIO_MOVE_TO_GIVEN_POSITION);
		break;
	}

	case ATTACK: {
		updateVisibilityMap();
		BaseNPC* pTargetEnemy = findBestVisibleEnemy();
		std::cout << "DEBUG Warrior: ATTACK order, bestEnemy=" << pTargetEnemy
			<< std::endl;
		if (pTargetEnemy) {
			Position bestPosition = findBestAttackPosition(pTargetEnemy);
			pGoalNode = pAStar->findPath(
				position.row,
				position.col,
				bestPosition.row,
				bestPosition.col);
			fsm.setCurrentState(WARRIOR_MOVE_TO_ATTACK_POSITION);
		}
		else {
			fsm.setCurrentState(WARRIOR_IDLE);
		}
		break;
	}
	case DEFEND: {
		// Search for cover position using BFS
		pSafestNode = findSafestPosition();
		if (pSafestNode) {
			pGoalNode = pAStar->findPath(
				position.row,
				position.col,
				pSafestNode->getRow(),
				pSafestNode->getCol());
			fsm.setCurrentState(WARRIOR_MOVE_TO_COVER_POSITION);
		}
		else {
			fsm.setCurrentState(WARRIOR_IDLE);
		}
		break;
	}
	default:
		fsm.setCurrentState(WARRIOR_IDLE);
		break;
	}
}

void WarriorNPC::draw() const
{
	drawNPC('w');
}

void WarriorNPC::addSupply(int val)
{
	bullets += val;
	grenades += val;
	if (bullets >= MAX_BULLETS)
		bullets = MAX_BULLETS;
	if (grenades >= MAX_GRENADES)
		grenades = MAX_GRENADES;
}

void WarriorNPC::subBullets(int val)
{
	bullets -= val;
	if (bullets <= 0)
		bullets = 0;
}

void WarriorNPC::subGrenades(int val)
{
	grenades -= val;
	if (grenades <= 0)
		grenades = 0;
}

// returns true if there is clear line of sight from (row1,col1) to (row2,col2)
bool WarriorNPC::hasLineOfSight(Map* pMap, int row1, int col1, int row2, int col2)
{
	int x = col1;
	int y = row1;

	int dx = abs(col2 - col1);
	int dy = -abs(row2 - row1);

	int sx = (col1 < col2) ? 1 : -1;
	int sy = (row1 < row2) ? 1 : -1;

	int err = dx + dy; // Bresenham base error

	while (true)
	{
		// Ignore starting tile
		if (!(x == col1 && y == row1))
		{
			if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE)
			{
				std::cout << "DEBUG LOS: out of bounds (" << y << "," << x << ")" << std::endl;
				return false;
			}

			int cell = pMap->getCellType(y, x);
			if (cell == ROCK || cell == TREE)
			{
				std::cout << "DEBUG LOS: blocked at (" << y << "," << x
					<< ") cell=" << cell << std::endl;
				return false;
			}
		}

		if (x == col2 && y == row2)
			return true; // reached target

		int err2 = 2 * err;

		if (err2 >= dy)
		{
			err += dy;
			x += sx;
		}
		if (err2 <= dx)
		{
			err += dx;
			y += sy;
		}
	}
}

void WarriorNPC::updateVisibilityMap()
{
	// clear visibility map
	for (int i = 0; i < MAP_SIZE; i++)
		for (int j = 0; j < MAP_SIZE; j++)
			visibilityMap[i][j] = false;

	// update visibility map based on line of sight
	for (int dr = -VIEW_RADIUS; dr <= VIEW_RADIUS; dr++)
		for (int dc = -VIEW_RADIUS; dc <= VIEW_RADIUS; dc++)
		{
			int row = position.row + dr;
			int col = position.col + dc;

			// check bounds
			if (row < 0 || row >= MAP_SIZE || col < 0 || col >= MAP_SIZE)
				continue;

			// check within circular view radius
			double distance = sqrt(dr * dr + dc * dc);
			if (distance <= VIEW_RADIUS) {
				if (hasLineOfSight(pMap, position.row, position.col, row, col)) {
					visibilityMap[row][col] = true;
				}
			}
		}

}

BaseNPC* WarriorNPC::findBestVisibleEnemy()
{
	BaseNPC* pClosestEnemy = nullptr;
	double closestDistance = 1e9; // large initial value

	for (BaseNPC* pEnemy : allEnemies)
	{
		// skip if same team or not alive
		if (pEnemy->getTeamID() == teamID || !pEnemy->getIsAlive())
			continue;

		int enemyRow = pEnemy->getPosition().row;
		int enemyCol = pEnemy->getPosition().col;

		std::cout << "DEBUG FVE: checking enemy at ("
			<< enemyRow << "," << enemyCol
			<< ") visible=" << visibilityMap[enemyRow][enemyCol]
			<< " alive=" << pEnemy->getIsAlive()
			<< std::endl;

		// skip if enemy position not visible
		if (!visibilityMap[enemyRow][enemyCol]) {
			std::cout << "DEBUG FVE: enemy at ("
				<< enemyRow << "," << enemyCol
				<< ") NOT visible in visibilityMap" << std::endl;
			continue;
		}
			

		// compute distance to enemy
		double distance = calculateDistanceToEnemy(pEnemy);

		std::cout << "DEBUG FVE: distance to enemy ("
			<< enemyRow << "," << enemyCol
			<< ") = " << distance << std::endl;

		if (distance < closestDistance) {
			closestDistance = distance;
			pClosestEnemy = pEnemy;
		}
	}

	return pClosestEnemy;
}

Position WarriorNPC::findBestAttackPosition(const BaseNPC* targetEnemy)
{
	Position bestPosition = position;
	double bestSecurityValue = 1e9; // large initial value 

	int enemyRow = targetEnemy->getPosition().row;
	int enemyCol = targetEnemy->getPosition().col;

	// update visibility map based on line of sight
	for (int dr = -VIEW_RADIUS; dr <= VIEW_RADIUS; dr++)
		for (int dc = -VIEW_RADIUS; dc <= VIEW_RADIUS; dc++)
		{
			int row = enemyRow + dr;
			int col = enemyCol + dc;

			// check bounds
			if (row < 0 || row >= MAP_SIZE || col < 0 || col >= MAP_SIZE)
				continue;

			// check if it's passable cell
			if (!pMap->isPassable(row, col))
				continue;
			
			// check whether there is a clear line of sight from the given position to the enemy
			if (!hasLineOfSight(pMap, row, col, enemyRow, enemyCol))
				continue;

			// check if the enemy is in bullet range from the given position
			double distanceToEnemy = hypot(enemyRow - row, enemyCol - col);
			if (distanceToEnemy > BULLET_RANGE)
				continue;

			double securityValue = pMap->getSecurityValue(row, col);
			if (securityValue < bestSecurityValue) {
				bestSecurityValue = securityValue;
				bestPosition = { row, col };
			}	
		}
	return bestPosition;
}

double WarriorNPC::calculateDistanceToEnemy(const BaseNPC* enemy)
{
	int er = enemy->getPosition().row;
	int ec = enemy->getPosition().col;
	return hypot(er - position.row, ec - position.col);
}

void WarriorNPC::attackBehavior()
{
	updateVisibilityMap();

	// find best visible enemy
	BaseNPC* pTargetEnemy = findBestVisibleEnemy();
	if (!pTargetEnemy) {
		// No enemies in sight
		fsm.setCurrentState(WARRIOR_IDLE);
		return; 
	}

	double distanceToEnemy = calculateDistanceToEnemy(pTargetEnemy);

	// If enemy is out of range, move closer
	if (distanceToEnemy > BULLET_RANGE) {
		Position bestAttackPosition = findBestAttackPosition(pTargetEnemy);
		pGoalNode = pAStar->findPath(
			position.row, 
			position.col,
			bestAttackPosition.row, 
			bestAttackPosition.col);
		fsm.setCurrentState(WARRIOR_MOVE_TO_ATTACK_POSITION);
		return;
	}

	//Check line of sight
	bool hasSight = hasLineOfSight(
		pMap, 
		position.row, 
		position.col,
		pTargetEnemy->getPosition().row, 
		pTargetEnemy->getPosition().col);

	// Perform attack
	if (bullets > 0 && hasSight) {
		double angle = atan2(
			pTargetEnemy->getPosition().row - position.row,
			pTargetEnemy->getPosition().col - position.col);

		Bullet* b = new Bullet(position.col, position.row, angle, pMap);
		bulletsManager->addBullet(b);
		bullets--;
	}
	else if (grenades > 0 && distanceToEnemy <= BULLET_RANGE) {
		// Throw a grenade (backup weapon)
		Grenade* g = new Grenade(position.col + 0.5, position.row + 0.5, pMap);
		bulletsManager->addGrenade(g);
		grenades--;
	}
	else if (bullets == 0 && grenades == 0) {
		// pCommander->requestSupply(this)
		fsm.setCurrentState(WARRIOR_WAIT_FOR_SUPPLY);
	}
	else {
		// No clear line of sight, reposition to a better attack position
		Position bestAttackPosition = findBestAttackPosition(pTargetEnemy);

		pGoalNode = pAStar->findPath(
			position.row,
			position.col,
			bestAttackPosition.row,
			bestAttackPosition.col);

		fsm.setCurrentState(WARRIOR_MOVE_TO_ATTACK_POSITION);
	}
}

NodeBFS* WarriorNPC::findSafestPosition()
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
		for (int k = 0; k < 4; k++)
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

NodeBFS* WarriorNPC::getNextStepToSafestPosition(NodeBFS* pSafestNode)
{
	if (!pSafestNode)
		return nullptr;

	NodeBFS* pCurrent = pSafestNode;

	// Traverse backwards along the parent chain
	// until the parent is the warrior's current position.
	while (pCurrent->getParent() &&
		!(pCurrent->getParent()->getRow() == position.row &&
			pCurrent->getParent()->getCol() == position.col))
	{
		pCurrent = pCurrent->getParent();
	}

	return pCurrent;
}

const char* WarriorNPC::warriorStateToString(WarriorState state)
{
	switch (state) {
	case WARRIOR_IDLE:    return "WARRIOR_IDLE";
	case WARRIO_MOVE_TO_GIVEN_POSITION:  return "WARRIO_MOVE_TO_GIVEN_POSITION";
	case WARRIOR_MOVE_TO_ATTACK_POSITION:  return "WARRIOR_MOVE_TO_ATTACK_POSITION";
	case WARRIOR_ATTACKING:  return "WARRIOR_ATTACKING";
	case WARRIOR_MOVE_TO_COVER_POSITION:  return "WARRIOR_MOVE_TO_COVER_POSITION";
	case WARRIOR_WAIT_FOR_SUPPLY:    return "WARRIOR_WAIT_FOR_SUPPLY";
	case WARRIOR_IN_SUPPLY_PROCESS:  return "WARRIOR_IN_SUPPLY_PROCESS";
	case WARRIOR_WAIT_FOR_HEALING:  return "WARRIOR_WAIT_FOR_HEALING";
	case WARRIOR_IN_HEALING_PROCESS:  return "WARRIOR_IN_HEALING_PROCESS";
	case WARRIOR_DEAD:  return "WARRIOR_DEAD";
	default:return "UNKNOWN";
	}
}






