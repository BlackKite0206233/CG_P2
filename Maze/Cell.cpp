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
#include "LineSeg.h"
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
Cell(int i, Edge *px, Edge *py, Edge *mx, Edge *my)
//=======================================================================
{
	index = i;

	edges[PLUS_X] = px;
	edges[PLUS_Y] = py;
	edges[MINUS_X] = mx;
	edges[MINUS_Y] = my;

	counter = 0;
}


//***********************************************************************
//
// * Returns true if the given x,y,z pt is inside the cell. If not, sets
//   neighbor to be the cell we think the point might be in, and returns false.
//=======================================================================
bool Cell::
Point_In_Cell(const float x, const float y, const float z,
              Cell* &neighbor)
//=======================================================================
{
	int i;

	// Check the point against each edge in turn.
	for ( i = 0 ; i < 4 ; i++ ) {
		// Test whether the cell and the point lie on the same side of the
		// edge. If not, the point is outside.
		if ( edges[i]->Cell_Side(this) != edges[i]->Point_Side(x, y) ) {
			// Found an edge that we are on the wrong side of.
			// Return the neighboring cell, so we know where to look next.
			neighbor = edges[i]->Neighbor(this);
			return false;
		}
	}

	// Inside all edges, so we must be inside the cell.
	return true;
}

//***********************************************************************
//
// * Clip the segment (xs,ys)->(xe,ye) to the cell. If the segment
//   crosses an opaque edge, clip it to that edge and set (xe,ye) to be
//   the intersection point. If it crosses a transparent edge, clip
//   it and set (xs,ys) to be the intersection point. If the segment lies
//   inside the cell or was clipped to an opaque edge, then 0 is
//   returned, otherwise the cell on the other side of the transparent
//   clip edge is returned (the cell the segment is entering).
//   This is used in tracking the viewer, but much of it is also useful
//   for clipping edges, so you should try to understand how it works.
//=======================================================================
Cell* Cell::
Clip_To_Cell(float &xs, float &ys,
             float &xe, float &ye, const float buffer)
//=======================================================================
{
	LineSeg in_seg(xs, ys, xe, ye);
	float   min_crossing = 1.0f;
	int	  min_cross_edge;
	int     i;


	// Check each edge for a valid crossing.
	for ( i = 0 ; i < 4 ; i++ ) {
		LineSeg e(edges[i]);
		float	cross = in_seg.Cross_Param(e);

		if ( cross > 0.0 && cross < min_crossing ) {
			min_crossing = cross;
			// Keep track of which edge the segment crosses first.
			min_cross_edge = i;
		}
	}

	// If the nearest crossing is within the segment...
	if ( min_crossing < 1.0 ) {
		if ( edges[min_cross_edge]->opaque )	{
			min_crossing -= 1.0e-3f; // Make sure we stay inside.
			if ( min_crossing < 0.0 )	
				min_crossing = 0.0f;
			xe = xs + min_crossing * ( xe - xs );
			ye = ys + min_crossing * ( ye - ys );
			return NULL;
		}
		else
		{
			min_crossing += 1.0e-3f; // Make sure the new point is outside.
			if ( min_crossing > 1.0 )
				min_crossing = 1.0f;
			xs = xs + min_crossing * ( xe - xs );
			ys = ys + min_crossing * ( ye - ys );

			// Return the cell the segment is entering
			return edges[min_cross_edge]->Neighbor(this);
		}
	}

	return NULL;
}



void Cell::Draw(QVector3D o, vector<QVector3D> boundary, int count) {
#ifdef DEBUG
	if (count > 3) return;
#endif
	if (counter <= MazeWidget::maze->frame_num) {
		counter = MazeWidget::maze->frame_num + 1;
	}
	else {
		return;
	}
#ifdef DEBUG
	std::cout << count << ", " << boundary.size() << std::endl;
	/*for (auto& b : boundary) {
		std::cout << b.x() << ", " << b.y() << ", " << b.z() << std::endl;
	}
	std::cout << std::endl;*/
	glColor3f(1, 1, 0);
	glBegin(GL_LINES);
	for (int i = 0; i < boundary.size(); i++) {
		glVertex2f(o.x(), o.y());
		glVertex2f(boundary[i].x(), boundary[i].y());
	}
	glEnd();
#endif
	vector<QVector3D> newBoundary;
	for (int i = 0; i < 4; i++) {
		Edge *edge = edges[i];
		if (edge->Clip(o, boundary, newBoundary)) {
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
				if (edge->ClipTop(o, boundary, newBoundary)) {
					Cell* newCell = edge->Neighbor(this);
					if (newCell != NULL) {
						newCell->Draw(o, newBoundary, count + 1);
					}
				}
			}
			else {
				Cell* newCell = edge->Neighbor(this);
				if (newCell != NULL) {
					newCell->Draw(o, newBoundary, count + 1);
				}
			}
		}
	}
#ifdef DEBUG
	glColor3f(1, 1, 0);
	glBegin(GL_LINES);
	for (int i = 0; i < boundary.size(); i++) {
		glVertex2f(o.x(), o.y());
		glVertex2f(boundary[i].x(), boundary[i].y());
	}
	glEnd();
#endif
}
