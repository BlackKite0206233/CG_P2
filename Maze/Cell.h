/************************************************************************
     File:        Cell.h

     Author:     
                  Stephen Chenney, schenney@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu

     Comment:    
						(c) 2001-2002 Stephen Chenney, University of Wisconsin at Madison
						Class header file for Cell class. Stores data about cells in the maze
		

     Platform:    Visio Studio.Net 2003 (converted to 2005)

*************************************************************************/

#ifndef _CELL_H_
#define _CELL_H_

#include "Edge.h"
#include <vector>
#include "Vec3D.h"
#include <queue>
#include <map>


using std::vector;
using std::map;
using std::queue;

class Cell;

class ClipData {
public:
	ClipData() {

	}
	ClipData(Cell* c, vector<vector<Vec3D>> b, int n) : cell(c), boundarys(b), noDraw(n) {

	};
	Cell* cell;
	vector<vector<Vec3D>> boundarys;
	int noDraw;
};

class Cell {
public:

	// Constructor takes the index and the four edges
	Cell(int, Edge*, Edge*, Edge*, Edge*, Edge*, Edge*, bool);

public:
	static void AddIfNotExist(vector<Vec3D>& list, const Vec3D& element);
	static void Draw(int init, ClipData& initData, const Vec3D& o, Edge* mirrorPlane);
	// Returns true if the given point (x,y) is inside the cell, otherwise
	// returns false and sets the new_cell to be the neighboring cell across
	// the edge for which the inside test failed. Used in tracking the viewer.
	bool Point_In_Cell(const Vec3D&, Cell *&new_cell);
public:
// Constants for accessing edges.
	static const char PLUS_X;         // The edge in the positive x direction
	static const char PLUS_Y;         // The edge in the positive y direction
	static const char MINUS_X;        // The edge in the negative x direction
	static const char MINUS_Y;        // The edge in the negative y direction

	int	index;		// The index of this cell (just an identifier).
	Edge* edges[6];  // Pointers to the edges of the cell.

	unsigned int counter;	// Used in building a maze. It is reset to
									// 0 after construction, and you should
									// set it equal to the frame number each time
									// you draw a cell. You then use it to prvent
									// drawing cells twice when you recurse.
	unsigned int topCounter = 0;
	Vec3D center;
	bool isTop;
};

#endif

