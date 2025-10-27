#pragma once
#include "Types.h"

// Order.h:

enum OrderType { NONE, MOVE_TO_POSITION, ATTACK, DEFEND, RESUPPLY, HEAL };

class Order
{
private:
	OrderType type = NONE;
	Position* pTarget = nullptr;
public:
	Order(OrderType t);
	Order(OrderType t, Position* pt);
	OrderType getType() const { return type; }
	Position* getTarget() const { return pTarget; }
};

