#include "Node.h"

// Node.cpp:

Node::Node(int r, int c, int gVal, int hVal, Node* p)
{
	row = r;
	col = c;
	g = gVal;
	h = hVal;
	f = g + h;
	parent = p;
}
