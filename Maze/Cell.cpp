/************************************************************************
     File:        Cell.cpp

     Author:     
                  Stephen Chenney, schenney@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu

     Comment:    
						(c) 2001-2002 Stephen Chenney, University of Wisconsin at Madison
						Class header file for Cell class. Stores data about cells in the maze
		

     Platform:    Visio Studio.Net 2003 (converted to 2005)

*************************************************************************/

#include <stdio.h>
#include "Maze.h"
#include "MazeWidget.h"
#include <iostream>

const char  Cell::PLUS_X	= 0;
const char  Cell::PLUS_Y	= 1;
const char  Cell::MINUS_X	= 2;
const char  Cell::MINUS_Y	= 3;

//***********************************************************************
//
// * Constructor.
//=======================================================================
Cell::
Cell(int i, Edge *px, Edge *py, Edge *mx, Edge *my, Edge* floor, Edge* ceiling)
//=======================================================================
{
	index = i;

	edges[PLUS_X] = px;
	edges[PLUS_Y] = py;
	edges[MINUS_X] = mx;
	edges[MINUS_Y] = my;

	edges[4] = floor;
	edges[5] = ceiling;

	center = Vec3D(0, 0, 0);
	center += edges[0]->edgeBoundary[0];
	center += edges[0]->edgeBoundary[1];
	center += edges[0]->edgeBoundary[2];
	center += edges[0]->edgeBoundary[3];
	center += edges[2]->edgeBoundary[0];
	center += edges[2]->edgeBoundary[1];
	center += edges[2]->edgeBoundary[2];
	center += edges[2]->edgeBoundary[3];
	center /= 8;

	counter = 0;
}


//***********************************************************************
//
// * Returns true if the given x,y,z pt is inside the cell. If not, sets
//   neighbor to be the cell we think the point might be in, and returns false.
//=======================================================================
bool Cell::
Point_In_Cell(const double x, const double y, const double z,
              Cell* &neighbor)
//=======================================================================
{
	// Check the point against each edge in turn.
	for ( int i = 0 ; i < 6 ; i++ ) {
		// Test whether the cell and the point lie on the same side of the
		// edge. If not, the point is outside.
		if ( !Edge::IsSameSide(*(edges[i]), center, Vec3D(x, y, z)) ) {
			// Found an edge that we are on the wrong side of.
			// Return the neighboring cell, so we know where to look next.
			neighbor = edges[i]->Neighbor(this);
			return false;
		}
	}

	// Inside all edges, so we must be inside the cell.
	return true;
}


void Cell::Draw(const Vec3D& o, const vector<Vec3D>& boundary) {
	if (counter <= MazeWidget::maze->frame_num) {
		counter = MazeWidget::maze->frame_num + 1;
	}
	else {
		return;
	}
	vector<Vec3D> newBoundary;
	vector<Vec3D> nextBoundary;
	for (int i = 0; i < 6; i++) {
		Edge *edge = edges[i];
		if (edge->Clip(o, boundary, newBoundary, false)) {
			if (edge->opaque) {
#ifdef DEBUG
				glColor3f(1, 0, 0);
				glBegin(GL_LINES);
				for (int i = 0; i < newBoundary.size(); i++) {
					glVertex2f(o.x(), o.y());
					glVertex2f(newBoundary[i].x(), newBoundary[i].y());
				}
				glEnd();
#endif
				edge->Draw(newBoundary);
			}
			else {
				Cell* newCell = edge->Neighbor(this);
				if (newCell != NULL) {
					newCell->Draw(o, newBoundary);
				}
			}
		}
	}
	counter = 0;
}
