#include "Order.h"

// Order.cpp:

Order::Order(OrderType t)
{
	type = t;
}

Order::Order(OrderType t, Position* pt)
{
	type = t;
	this->pTarget = pt;
}
