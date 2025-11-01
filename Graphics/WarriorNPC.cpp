#include "WarriorNPC.h"
#include "glut.h"
#include "Bullet.h"
#include "Grenade.h"
#include <queue>
#include "NodeBFS.h"

// WarriorNPC.cpp:

WarriorNPC::WarriorNPC(Position p, TeamID t, Map* m) : BaseNPC(p, t, m), fsm(IDLE)
{
	bullets = MAX_BULLETS;
	grenades = MAX_GRENADES;
	pAStar = new AStar(m);

}

WarriorNPC::~WarriorNPC()
{
	delete pAStar;
}

void WarriorNPC::tick()
{
	// Check if alive
	if(!isAlive) {
		fsm.setCurrentState(DEAD);
		return;
	}

	// Check for death
	if(health <= 0) {
		isAlive = false;
		fsm.setCurrentState(DEAD);
		return;
	}

	// Check health status
	if (health < 0.25 * MAX_HEALTH) {
		fsm.setCurrentState(WAIT_FOR_HEALING);
		// ADD: notify commander about low health
		// pCommander->requestMedic(this);
		fsm.setCurrentState(WAIT_FOR_HEALING);
		return;
	}

	if(bullets < 0.25 * MAX_BULLETS && grenades < 0.25 * MAX_GRENADES) {
		fsm.setCurrentState(WAIT_FOR_SUPPLY);
		// ADD: notify commander about low ammo
		// pCommander->requestSupply(this)
		fsm.setCurrentState(WAIT_FOR_SUPPLY);
	}

	updateVisibilityMap();
	
	// fsm behavior
	switch (fsm.getCurrentState())
	{
	case IDLE:
		// do nothing
		break;

	case MOVE_TO_GIVEN_POSITION:
	case MOVE_TO_COVER_POSITION:
		NodeAStar* pNextStep = pAStar->getNextStepTowardsTarget(
			pGoalNode,
			position.row, 
			position.col);
		if (pNextStep) {
			position.row = pNextStep->getRow();
			position.col = pNextStep->getCol();

			if (position.row == pSafestNode->getRow() &&
				position.col == pSafestNode->getCol()) {
				fsm.setCurrentState(IDLE);
			}
		}
		else {
			// Reached destination
			fsm.setCurrentState(IDLE);
		}
		break;

	case MOVE_TO_ATTACK_POSITION:
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
			fsm.setCurrentState(ATTACKING);
		}
		break;

	case ATTACKING:
		attackBehavior();
		break;

	case WAIT_FOR_SUPPLY:
		// ADD: implement waiting for supply behavior
		break;

	case IN_SUPPLY_PROCESS:
		// ADD: implement in supply process behavior
		break;

	case WAIT_FOR_HEALING:
		// ADD: implement waiting for healing behavior
		break;

	case IN_HEALING_PROCESS:
		// ADD: implement in healing process behavior
		if (health == MAX_HEALTH)
			fsm.setCurrentState(IDLE);
		break;

	case DEAD:
		// do nothing
		break;

	default:
		break;
	}
}

void WarriorNPC::handleOrder(Order* pOrder)
{
	switch (pOrder->getType())
	{
		case MOVE_TO_POSITION:
			pGoalNode = pAStar->findPath(
				position.row,
				position.col, 
				pOrder->getTarget()->row,
				pOrder->getTarget()->col);
			fsm.setCurrentState(MOVE_TO_GIVEN_POSITION);
		break;

		case ATTACK:
			updateVisibilityMap();
			BaseNPC* pTargetEnemy = findBestVisibleEnemy();
			if (pTargetEnemy) {
				Position bestPosition = findBestAttackPosition(pTargetEnemy);
				pGoalNode = pAStar->findPath(
					position.row,
					position.col,
					bestPosition.row,
					bestPosition.col);
				fsm.setCurrentState(MOVE_TO_ATTACK_POSITION);
			} 
			else {
				fsm.setCurrentState(IDLE);
			}
			break;
		case DEFEND:
			// Search for cover position using BFS
			pSafestNode = findSafestPosition();
			if (pSafestNode) {
				pGoalNode = pAStar->findPath(
					position.row,
					position.col, 
					pSafestNode->getRow(), 
					pSafestNode->getCol());
				fsm.setCurrentState(MOVE_TO_COVER_POSITION);
			}
			else {
				fsm.setCurrentState(IDLE);
			}
			break;
		default:
			fsm.setCurrentState(IDLE);
			break;
	}
}

void WarriorNPC::draw() const
{
	switch (teamID)
	{
	case TEAM_1:
		glColor3d(0, 0, 1); // Blue for Team 1
		break;
	case TEAM_2:
		glColor3d(1, 0, 0); // Red for Team 2
		break;
	}

	// draw warrior as a square
	glBegin(GL_POLYGON);
	glVertex2d(position.col, position.row);
	glVertex2d(position.col, position.row + 1);
	glVertex2d(position.col + 1, position.row + 1);
	glVertex2d(position.col + 1, position.row);
	glEnd();

	// Draw letter 'W' above the square
	glColor3d(0, 0, 0); // black text
	glRasterPos2d(position.col + 0.35, position.row + 1.2); // position slightly above square
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'W');

}

// returns true if there is clear line of sight from (row1,col1) to (row2,col2)
bool WarriorNPC::hasLineOfSight(Map* pMap, int row1, int col1, int row2, int col2)
{
	int dx = abs(col2 - col1); // difference in x
	int dy = abs(row2 - row1); // difference in y
	int sx = (col1 < col2) ? 1 : -1; // step in x
	int sy = (row1 < row2) ? 1 : -1; // step in y
	int err = dx + dy; // error value
	int x = col1;
	int y = row1;

	// Bresenham's line algorithm
	while (true) {
		if (!(y == row1 && x == col1)) {
			// skip starting point
			int cellType = pMap->getCellType(y, x);
			if (cellType == ROCK || cellType == TREE)
				return false; // line of sight blocked
		}

		if (x == col2 && y == row2)
			break; // reached endpoint

		int err2 = 2 * err;
		
		// move in y direction
		if (err2 >= dy) {
			err += dy;
			x += sx;
		}

		// move in x direction
		if (err2 <= dx) {
			err += dx;
			y += sy;
		}
	}
	return true; // clear line of sight
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

		// skip if enemy position not visible
		if (!visibilityMap[enemyRow][enemyCol])
			continue; 

		// compute distance to enemy
		double distance = calculateDistanceToEnemy(pEnemy);
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
		fsm.setCurrentState(IDLE);
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
		fsm.setCurrentState(MOVE_TO_ATTACK_POSITION);
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
		b->setIsActive(true);
		activeBullets.push_back(b);
		bullets--;
	}
	else if (grenades > 0 && distanceToEnemy <= BULLET_RANGE) {
		// Throw a grenade (backup weapon)
		Grenade* g = new Grenade(position.col + 0.5, position.row + 0.5, pMap);
		g->setIsExploded(true);
		activeGrenades.push_back(g);
		grenades--;
	}
	else if (bullets == 0 && grenades == 0) {
		// pCommander->requestSupply(this)
		fsm.setCurrentState(WAIT_FOR_SUPPLY);
	}
	else {
		// No clear line of sight, reposition to a better attack position
		Position bestAttackPosition = findBestAttackPosition(pTargetEnemy);

		pGoalNode = pAStar->findPath(
			position.row,
			position.col,
			bestAttackPosition.row,
			bestAttackPosition.col);

		fsm.setCurrentState(MOVE_TO_ATTACK_POSITION);
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
		for (int k; k < 4; k++)
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






