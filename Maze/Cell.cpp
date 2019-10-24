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

void Cell::Draw(int init, ClipData& initData, const Vec3D& o, Edge* mirrorPlane) {
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
				if (i == data.noDraw) continue;
				if (clipData[init].cell->isTop && data.cell->isTop && data.cell->edges[i]->opaque == false && i < 4) {
					Cell* newCell = edge->Neighbor(data.cell);
					if (newCell != NULL && clipData.find(newCell->index) == clipData.end()) {
						clipData[newCell->index] = ClipData(newCell, vector<vector<Vec3D>>({ boundary }), -1);
						q.push(newCell->index);
					}
				}
				else if (edge->Clip(o, boundary, center, newBoundary, false)) {
					if (edge->opaque) {
						edge->Draw(newBoundary, mirrorPlane, MazeWidget::maze->mode);
						if (edge->mirror != nullptr) {
							vector<Vec3D> mirrorBoundary;
							Vec3D c;
							for (auto& b : newBoundary) {
								c += b;
							}
							c /= newBoundary.size();
							if (edge->mirror->Clip(o, newBoundary, c, mirrorBoundary, false)) {
								Vec3D o_ = Edge::Mirror(*edge, o);
								ClipData clip = ClipData(data.cell, vector<vector<Vec3D>>({ mirrorBoundary }), i);
								edge->mirror->Draw(mirrorBoundary, edge->mirror, MazeWidget::maze->mode);
								Cell::Draw(data.cell->index, clip, o_, edge->mirror);
							}
						}
					}
					else {
						Cell* newCell = edge->Neighbor(data.cell);
						if (newCell == NULL) continue;
						if (clipData.find(newCell->index) != clipData.end()) {
							if (alreadyVisit.find(newCell->index) == alreadyVisit.end()) {
								clipData[newCell->index].boundarys.push_back(newBoundary);
							}
						}
						else {
							clipData[newCell->index] = ClipData(newCell, vector<vector<Vec3D>>({ newBoundary }), -1);
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