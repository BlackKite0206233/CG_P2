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
#include "cell.h"
#include <iostream>
#include <set>

#define EPS 1e-10

const char  Cell::PLUS_X	= 0;
const char  Cell::PLUS_Y	= 1;
const char  Cell::MINUS_X	= 2;
const char  Cell::MINUS_Y	= 3;

void Cell::AddIfNotExist(vector<Vec3D>& list, const Vec3D& element) {
	for (auto& i : list) {
		if (abs(i.x() - element[0]) < EPS && abs(i.y() - element[1]) < EPS && abs(i.z() - element[2]) < EPS) {
			return;
		}
	}
	list.push_back(element);
}

void Cell::Draw(int init, ClipData& initData, const Vec3D& o) {
	set<int> alreadyVisit;
	map<int, ClipData> clipData;
	clipData[init] = initData;
	queue<int> q;
	q.push(init);
	vector<Vec3D> newBoundary;
	ClipData data;
	int idx;
	while (!q.empty()) {
		idx = q.front();
		alreadyVisit.insert(idx);
		q.pop();
		data = clipData[idx];

		for (auto& boundary : data.boundarys) {
			Vec3D center;
			for (auto& b : boundary) {
				center += b;
			}
			center /= boundary.size();
			for (int i = 0; i < 6; i++) {
				Edge* edge = data.cell->edges[i];
				if (data.cell->isTop && i < 4) {
					Cell* newCell = edge->Neighbor(data.cell);
					if (newCell != NULL && clipData.find(newCell->index) == clipData.end()) {
						clipData[newCell->index] = ClipData(newCell, vector<vector<Vec3D>>({ boundary }), data.cell->index);
						q.push(newCell->index);
					}
				}
				else if (edge->Clip(o, boundary, center, newBoundary, false)) {
					if (edge->opaque) {
#ifdef DEBUG
						glColor3f(1, 0, 0);
						glPointSize(2);
						glBegin(GL_POINTS);
						glVertex2f(o[0], o[2]);
						glEnd();
						/*glColor3f(1, 0, 0);
						glBegin(GL_LINES);
						for (int i = 0; i < newBoundary.size(); i++) {
							glVertex2f(o[0], o[2]);
							glVertex2f(newBoundary[i].x(), newBoundary[i].z());
						}
						glEnd();*/
#endif
						edge->Draw(newBoundary);
					}
					else {
#ifdef DEBUG
						edge->Draw(newBoundary);
#endif
						Cell* newCell = edge->Neighbor(data.cell);
						if (newCell == NULL) continue;
						if (clipData.find(newCell->index) != clipData.end()) {
							if (alreadyVisit.find(newCell->index) == alreadyVisit.end()) {
								clipData[newCell->index].boundarys.push_back(newBoundary);
							}
						}
						else {
							clipData[newCell->index] = ClipData(newCell, vector<vector<Vec3D>>({ newBoundary }), data.cell->index);
							q.push(newCell->index);
						}
					}
				}
			}
		}
	}
}

//***********************************************************************
//
// * Constructor.
//=======================================================================
Cell::
Cell(int i, Edge *px, Edge *py, Edge *mx, Edge *my, Edge* floor, Edge* ceiling, bool top)
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
	isTop = top;
}


//***********************************************************************
//
// * Returns true if the given x,y,z pt is inside the cell. If not, sets
//   neighbor to be the cell we think the point might be in, and returns false.
//=======================================================================
bool Cell::
Point_In_Cell(const Vec3D& pos, Cell* &neighbor)
//=======================================================================
{
	// Check the point against each edge in turn.
	for ( int i = 0 ; i < 6 ; i++ ) {
		// Test whether the cell and the point lie on the same side of the
		// edge. If not, the point is outside.
		if ( !Edge::IsSameSide(*(edges[i]), center, pos) ) {
			// Found an edge that we are on the wrong side of.
			// Return the neighboring cell, so we know where to look next.
			neighbor = edges[i]->Neighbor(this);
			return false;
		}
	}

	// Inside all edges, so we must be inside the cell.
	return true;
}


void Cell::Draw(const Vec3D& o, const vector<Vec3D>& boundary, bool fromTop) {
	/*if (!fromTop && counter <= MazeWidget::maze->frame_num) {
		counter = MazeWidget::maze->frame_num + 1;
	}
	else if (fromTop && topCounter <= MazeWidget::maze->frame_num) {
		topCounter = MazeWidget::maze->frame_num + 1;
	}
	else {
		return;
	}*/
	if (counter <= MazeWidget::maze->frame_num) {
		counter = MazeWidget::maze->frame_num + 1;
	}
	else {
		return;
	}
	vector<Vec3D> newBoundary;
	for (int i = 0; i < 6; i++) {
		Edge *edge = edges[i];
		Vec3D center;
		for (auto& b : boundary) {
			center += b;
		}
		center /= boundary.size();
		if (edge->Clip(o, boundary, center, newBoundary, false)) {
			if (edge->opaque) {
#ifdef DEBUG
				glColor3f(1, 0, 0);
				glBegin(GL_LINES);
				for (int i = 0; i < newBoundary.size(); i++) {
					glVertex2f(o[0], o[1]);
					glVertex2f(newBoundary[i].x(), newBoundary[i].y());
				}
				glEnd();
#endif
				edge->Draw(newBoundary);
			}
			else {
				Cell* newCell = edge->Neighbor(this);
				if (newCell != NULL) {
					newCell->Draw(o, newBoundary, i > 3);
				}
			}
		}
	}
	counter = 0;
}