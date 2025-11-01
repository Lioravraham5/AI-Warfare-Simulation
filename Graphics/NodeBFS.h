#pragma once

// NodeBFS.h:

class NodeBFS
{
private:
	int row, col, depth; // depth - Number of steps we did from beginning
	NodeBFS* parent;

public:
	NodeBFS();
	NodeBFS(int r, int c, int d, NodeBFS* p);
	int getRow() { return row; }
	int getCol() { return col; }
	int getDepth() { return depth; }
	NodeBFS* getParent() { return parent; }

};

