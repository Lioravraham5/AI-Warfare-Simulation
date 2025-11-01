#pragma once
#include "Types.h"
#include "Map.h"
#include "Order.h"

// BaseNPC.h:

static const int MAX_HEALTH = 100;

class BaseNPC
{
protected:
	Position position;
	TeamID teamID;
	Map* pMap = nullptr;
	int health = MAX_HEALTH; // Default health value
	bool isAlive = true;


public:
	BaseNPC(Position p, TeamID t, Map* m);
	virtual ~BaseNPC() = default; // Virtual destructor for proper cleanup in derived classes

	// Getters 
	Position getPosition() const { return position; }
	TeamID getTeamID() const { return teamID; }
	Map* getMap() const { return pMap; }
	bool getIsAlive() const { return isAlive; }
	
	// Setters
	void setPosition(Position p) { position = p; }

	// Virtual Methods
	virtual void tick() = 0; // Pure virtual function for BaseNPC behavior each tick
	virtual void draw() const = 0; // Pure virtual function to draw the BaseNPC


	// Utilities

};

