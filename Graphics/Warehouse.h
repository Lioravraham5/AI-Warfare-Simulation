#pragma once
#include "Types.h"

// Warehouse.h:

enum WarehouseType {
    WEAPONS,
    MEDICINES
};

const int WAREHOUSE_SIZE = 3;

class Warehouse
{
private:
	int row;
	int col;
	TeamID teamID; 
	WarehouseType type;

public:
	Warehouse(int r, int c, TeamID team, WarehouseType t);
	int getRow() const { return row; }
	int getCol() const { return col; }
	TeamID getTeamID() const { return teamID; }
	WarehouseType getType() const { return type; }

	void draw() const;

private:
	void drawWeapons() const;
	void drawMedicines() const;
};

