#pragma once
// Types.h:

struct Position
{
	int row;
	int col;
	bool operator==(const Position& other) const { return row == other.row && col == other.col; } // Equality operator
};

enum Role { COMMANDER, WARRIOR, MEDIC, PROVIDER };

enum TeamID { TEAM_1 = 1, TEAM_2 = 2 };