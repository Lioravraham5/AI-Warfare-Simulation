#pragma once
#include <vector>
#include "Warehouse.h"

// Map.h:

using namespace std;

enum CellType
{
	EMPTY,
	ROCK,
	TREE,
	WATER,
	WEAPONS_WAREHOUSE,
	MEDICIES_WAREHOUSE
};

const int MAP_SIZE = 100;

const int NUM_ROCK_CLUSTERS = 10;   
const int NUM_TREE_CLUSTERS = 10;  
const int NUM_WATER_CLUSTERS = 10;    

const int MAX_ROCK_RADIUS = 4;
const int MAX_TREE_RADIUS = 6;
const int MAX_WATER_RADIUS = 7;

class Map
{
private:
	int map[MAP_SIZE][MAP_SIZE] = { EMPTY };
	vector<Warehouse> warehouses;
	double securityMap[MAP_SIZE][MAP_SIZE] = { 0.0 }; // Security value of each cell - higher == dangerous

public:
	Map();
	
	// Getters:
	int getCellType(int row, int col) const { return map[row][col]; }
	const vector<Warehouse>& getWarehouses() const { return warehouses; }
	double getSecurityValue(int row, int col) const { return securityMap[row][col]; }
	
	// Setters:
	void setCellType(int row, int col, CellType type) { map[row][col] = type; }
	void setSecurityValue(int row, int col, double value) { securityMap[row][col] = value; }
	
	void drawMap() const;

	bool isPassable(int row, int col) const;


private:
	void generateCluster(int centerRow, int centerCol, int radius, CellType type);
	bool isEmptyAndValidCell(int row, int col) const;
	bool canPlaceWarehouse(int row, int col);
	void drawEmptyCell(int row, int col) const;
	void drawRockCell(int row, int col) const;
	void drawTreeCell(int row, int col) const;
	void drawWaterCell(int row, int col) const;

};

