#include "AStar.h"
#include <queue>
#include <iostream>

AStar::AStar(Map* m)
{
	pMap = m;
}

NodeAStar* AStar::findPath(int startRow, int startCol, int targetRow, int targetCol)
{
	std::cout << "DEBUG A*: findPath from ("
		<< startRow << "," << startCol << ") to ("
		<< targetRow << "," << targetCol << ")" << std::endl;

	priority_queue<NodeAStar*, std::vector<NodeAStar*>, CompareNodes> open;
	bool visited[MAP_SIZE][MAP_SIZE] = { false };
	int bestG[MAP_SIZE][MAP_SIZE];

	initBestG(bestG); // Initialize bestG with infinity (INT_MAX)

	// Start Node:
	int h = getManhattanDistance(startRow, startCol, targetRow, targetCol);
	NodeAStar* pStart = new NodeAStar(startRow, startCol, 0, h, nullptr);
	open.push(pStart);
	bestG[startRow][startCol] = 0;

	NodeAStar* pGoalNode = nullptr;

	// Movement directions: up, down, left, right
	static int directionRow[4] = { 1, -1, 0, 0 };
	static int directionCol[4] = { 0, 0, -1, 1 };

	while (!open.empty())
	{
		NodeAStar* pCurrent = open.top();
		open.pop();

		// Skip if already visited
		if (visited[pCurrent->getRow()][pCurrent->getCol()])
			continue;

		// Set Node as visited
		visited[pCurrent->getRow()][pCurrent->getCol()] = true;

		// Check if target reached
		if (pCurrent->getRow() == targetRow && pCurrent->getCol() == targetCol) {
			pGoalNode = pCurrent; // We found the best path to the target
			break;
		}

		// Explore neighbors
		for (int k = 0; k < 4; k++)
		{
			int neighborRow = pCurrent->getRow() + directionRow[k];
			int neighborCol = pCurrent->getCol() + directionCol[k];

			if(!pMap->isPassable(neighborRow, neighborCol))
				continue; // Skip non-passable cells

			int newG = pCurrent->getG() + pMap->getSecurityValue(neighborRow,neighborCol) + 1; 
			int h = getManhattanDistance(neighborRow, neighborCol, targetRow, targetCol);

			if (newG < bestG[neighborRow][neighborCol]) {
				bestG[neighborRow][neighborCol] = newG;
				NodeAStar* pNeighbor = new NodeAStar(neighborRow, neighborCol, newG, h, pCurrent);
				open.push(pNeighbor);
			}
		}
	}
	std::cout << "DEBUG A*: findPath, pGoalNode="
		<<pGoalNode
		<< std::endl;
	return pGoalNode; // May be nullptr if no path found
}

NodeAStar* AStar::getNextStepTowardsTarget(NodeAStar* pGoalNode, int startRow, int startCol)
{
	NodeAStar* pCurrent = pGoalNode;

	// Go back to the start node to find the next step
	while (pCurrent->getParent() != nullptr)
	{
		NodeAStar* pParent = pCurrent->getParent();
		if(pParent->getRow() == startRow && pParent->getCol() == startCol)
			return pCurrent; // pCurrent is the next step towards the target

		// Move one step backwards along the path
		pCurrent = pParent;
	}
	return nullptr; // Should not reach here if path is valid
}

void AStar::initBestG(int bestG[MAP_SIZE][MAP_SIZE])
{
	for (int i = 0; i < MAP_SIZE; i++)
		for (int j = 0; j < MAP_SIZE; j++)
			bestG[i][j] = INT_MAX;
}

int AStar::getManhattanDistance(int startRow, int startCol, int targetRow, int targetCol)
{
	return abs(startRow - targetRow) + abs(startCol - targetCol);
}
