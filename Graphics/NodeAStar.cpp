#include "NodeAStar.h"

// Node.cpp:

NodeAStar::NodeAStar(int r, int c, int gVal, int hVal, NodeAStar* p)
{
	row = r;
	col = c;
	g = gVal;
	h = hVal;
	f = g + h;
	parent = p;
}
