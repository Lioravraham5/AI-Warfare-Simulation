#pragma once

// Node.h:

class NodeAStar {
private:
	int row, col; // Define the position of the node
	int g; // "distance" from start
	int h; // Heuristic - assessed distance to solution (target)
	int f; // f = g + h
	NodeAStar* parent; // To restore the path to the target

public:
	NodeAStar(int r, int c, int gVal, int hVal, NodeAStar* p);
	int getRow() { return row; }
	int getCol() { return col; }
	void setRow(int val) { row = val; }
	void setCol(int val) {}
	int getF() { return f; }
	int getH() { return h; }
	int getG() { return g; }
	NodeAStar* getParent() { return parent; }

};

// Comparison class for priority queue
class CompareNodes
{
public:
	bool operator() (NodeAStar* pNode1, NodeAStar* pNode2) { return pNode1->getF() > pNode2->getF(); }
};
