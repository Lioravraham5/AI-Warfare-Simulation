#include "NodeBFS.h"

// NodeBFS.cpp:

NodeBFS::NodeBFS()
{
	parent = nullptr;
}

NodeBFS::NodeBFS(int r, int c, int d, NodeBFS* p)
{
	row = r;
	col = c;
	depth = d;
	parent = p;
}
