#include "Map.h"
#include <ctime>
#include <cstdlib>
#include "glut.h"

// Map.cpp:

Map::Map()
{
	srand(time(0));

	// Generate rock clusters
	for (int k = 0; k < NUM_ROCK_CLUSTERS; k++)
	{
		int row = rand() % MAP_SIZE;
		int col = rand() % MAP_SIZE;

		int radius = 1 + rand() % MAX_ROCK_RADIUS; // radius from 1 to MAX_ROCK_RADIUS (inclusive)
		generateCluster(row, col, radius, ROCK);
	}

	// Generate tree clusters
	for (int k = 0; k < NUM_TREE_CLUSTERS; k++)
	{
		int row = rand() % MAP_SIZE;
		int col = rand() % MAP_SIZE;

		int radius = 1 + rand() % MAX_TREE_RADIUS; // radius from 1 to MAX_TREE_RADIUS (inclusive)
		generateCluster(row, col, radius, TREE);
	}

	// Generate water clusters
	for (int k = 0; k < NUM_WATER_CLUSTERS; k++) {
		int row = rand() % MAP_SIZE;
		int col = rand() % MAP_SIZE;
		int radius = 1 + rand() % MAX_WATER_RADIUS; // radius from 1 to MAX_WATER_RADIUS (inclusive)
		generateCluster(row, col, radius, WATER); 
	}

	// Place warehouses
		


	for (TeamID team : { TEAM_1, TEAM_2 }) { // Assuming 2 teams
		// Place Weapons Warehouse
		while (true) {
			int row = rand() % MAP_SIZE;
			int col = rand() % MAP_SIZE;
			if (isEmptyAndValidCell(row, col)) {
				map[row][col] = WEAPONS_WAREHOUSE;
				warehouses.push_back(Warehouse(row, col, team, WEAPONS));
				break;
			}
		}
		// Place Medicines Warehouse
		while (true) {
			int row = rand() % MAP_SIZE;
			int col = rand() % MAP_SIZE;
			if (isEmptyAndValidCell(row, col)) {
				map[row][col] = MEDICIES_WAREHOUSE;
				warehouses.push_back(Warehouse(row, col, team, MEDICINES));
				break;
			}
		}
	}	
}

void Map::drawMap() const
{
	for (int i = 0; i < MAP_SIZE; i++) {
		for (int j = 0; j < MAP_SIZE; j++) {
			switch (map[i][j]) {
			case EMPTY:
				drawEmptyCell(i,j);
				break;
			case ROCK:
				drawRockCell(i, j);
				break;
			case TREE:
				drawTreeCell(i, j);
				break;
			case WATER:
				drawWaterCell(i, j);
				break;
			}
		}
	}

	// Draw warehouses on top
	for (const Warehouse& w : warehouses)
		w.draw();
}

void Map::generateCluster(int centerRow, int centerCol, int radius, CellType type)
{
	for (int i = -1 * radius; i <= radius; i++) {
		for (int j = -1 * radius; j <= radius; j++)
		{
			int row = centerRow + i;
			int col = centerCol + j;
			if(isEmptyAndValidCell(row, col))
				if (i * i + j * j <= radius * radius) // check if map[row][col] is within the circle
					map[row][col] = type;
		}
	}
}

bool Map::isEmptyAndValidCell(int row, int col)
{
	return (row >= 0 && row < MAP_SIZE && col >= 0 && col < MAP_SIZE && map[row][col] == EMPTY);
}

bool Map::canPlaceWarehouse(int row, int col)
{
	for(int i = 0; i < WAREHOUSE_SIZE; i++)
		for (int j = 0; j < WAREHOUSE_SIZE; j++)
			if (!isEmptyAndValidCell(row + i, col + j))
				return false;
	return true;
}

bool Map::isPassable(int row, int col) const
{
	// Check bounds
	if (row < 0 || row >= MAP_SIZE || col < 0 || col >= MAP_SIZE) 
		return false;

	// Check for obstacles
	if(map[row][col] == ROCK || map[row][col] == WATER)
		return false;

	// Check for warehouses
	for (const Warehouse& w : warehouses)
	{
		int wRow = w.getRow();
		int wCol = w.getCol();
		if (row >= wRow && row < wRow + WAREHOUSE_SIZE &&
			col >= wCol && col < wCol + WAREHOUSE_SIZE)
			return false;
	}

	// Cell is passable
	return true;
}

void Map::drawEmptyCell(int row, int col) const
{
	glColor3d(0.6, 0.8, 0.6); // light green

	// Draw cell as a square
	glBegin(GL_POLYGON);
	glVertex2d(col, row); // bottom-left
	glVertex2d(col, row + 1); // top-left
	glVertex2d(col + 1, row + 1); // top-right
	glVertex2d(col + 1, row); // bottom-right
	glEnd();

}

void Map::drawRockCell(int row, int col) const
{
	glColor3d(0.5, 0.5, 0.5); // gray
	// Draw cell as a square
	glBegin(GL_POLYGON);
	glVertex2d(col, row); // bottom-left
	glVertex2d(col, row + 1); // top-left
	glVertex2d(col + 1, row + 1); // top-right
	glVertex2d(col + 1, row); // bottom-right
	glEnd();
}

void Map::drawTreeCell(int row, int col) const
{
	glColor3d(0, 0.5, 0); // dark green
	// Draw cell as a triangle
	glBegin(GL_POLYGON);
	glVertex2d(col + 0.5, row + 1); // top
	glVertex2d(col, row); // bottom-left
	glVertex2d(col + 1, row); // bottom-right
	glEnd();
}

void Map::drawWaterCell(int row, int col) const
{
	glColor3d(0, 0, 1); // blue
	// Draw cell as a square
	glBegin(GL_POLYGON);
	glVertex2d(col, row); // bottom-left
	glVertex2d(col, row + 1); // top-left
	glVertex2d(col + 1, row + 1); // top-right
	glVertex2d(col + 1, row); // bottom-right
	glEnd();
}


