#pragma once
#include "Map.h"
#include "NodeAStar.h"

// AStar.h:

class AStar
{
private:
	Map* pMap = nullptr;

public:
	AStar(Map* m);
	NodeAStar* findPath(int startRow, int startCol, int targetRow, int targetCol);
	NodeAStar* getNextStepTowardsTarget(NodeAStar* pGoalNode, int startRow, int startCol);

private:
	void initBestG(int bestG[MAP_SIZE][MAP_SIZE]);
	int getManhattanDistance(int startRow, int startCol, int targetRow, int targetCol);
};

