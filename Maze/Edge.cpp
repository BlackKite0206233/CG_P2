/************************************************************************
     File:        Edge.cpp

     Author:     
                  Stephen Chenney, schenney@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu

     Comment:    
						(c) 2001-2002 Stephen Chenney, University of Wisconsin at Madison
						Class header file for Edge class. Stores data about edges in the maze.
		

     Platform:    Visio Studio.Net 2003 (converted to 2005)

*************************************************************************/

#include <stdio.h>
#include "Edge.h"
#include "Cell.h"
#include "MazeWidget.h"
#include <windows.h>
#include <gl\gl.h>
#include <gl\GLU.h>
#include <QLineF>
#include <iostream>


#define EPS 0.000001

const char Edge::LEFT    = 0;
const char Edge::RIGHT   = 1;
const char Edge::ON	     = 2;
const char Edge::NEITHER = 3;

const char Edge::START   = 0;
const char Edge::END     = 1;

double Edge::wallHeight = 2;
double Edge::thickness = 2;

//***********************************************************************
//
// * Constructor to set up the start and end and the color of the edge
//=======================================================================
Edge::
Edge(int i, Vertex *start, Vertex *end, float r, float g, float b, double z, bool _opaque)
//=======================================================================
{
	index = i;
	endpoints[START] = start;
	endpoints[END] = end;

	neighbors[0] = neighbors[1] = NULL;

	opaque = _opaque;

	color[0] = r;
	color[1] = g;
	color[2] = b;

	double leftX, rightX, leftY, rightY;
	leftX = endpoints[0]->posn[0];
	rightX = endpoints[1]->posn[0];
	leftY = endpoints[0]->posn[1];
	rightY = endpoints[1]->posn[1];

	plane = Plane(vector<Vec3D>({ Vec3D(leftX, leftY, z + wallHeight), Vec3D(rightX, rightY, z + wallHeight), Vec3D(rightX, rightY, z + 0), Vec3D(leftX, leftY, z + 0) }));
}

Edge::Edge(int i, Vec3D topLeft, Vec3D topRight, Vec3D bottomLeft, Vec3D bottomRight) {
	index = i;
	opaque = false;
	plane = Plane(vector<Vec3D>({ topLeft, topRight, bottomRight, bottomLeft }));
}

//***********************************************************************
//
// * Returns which side of the edge a cell lies. Valid return values are
//	  LEFT, RIGHT or NEITHER if the cell is not a neighbor of the edge.
//   The left side of the edge is the side that would be on the left
//   if you were standing at the START of the edge and looking along it,
//   with your head in the z direction.
//=======================================================================
char Edge::
Cell_Side(Cell *cell)
//=======================================================================
{
	if ( cell == neighbors[LEFT] )
		return LEFT;
	else if ( cell == neighbors[RIGHT] )
		return RIGHT;

	return NEITHER;
}


//***********************************************************************
//
// * Returns which side of the edge the line (x,y) is on. The return value
//   is one of the constants defined above (LEFT, RIGHT, ON). See above
//   for a discussion of which side is left and which is right.
//=======================================================================
char Edge::
Point_Side(float x, float y)
//=======================================================================
{
	// Compute the determinant: | xs ys 1 |
	//                          | xe ye 1 |
	//                          | x  y  1 |
	// Use its sign to get the answer.

	float   det;

	det = endpoints[START]->posn[Vertex::X] * 
			( endpoints[END]->posn[Vertex::Y] - y ) - 
			endpoints[START]->posn[Vertex::Y] * 
			( endpoints[END]->posn[Vertex::X] - x ) +
			endpoints[END]->posn[Vertex::X] * y	- 
			endpoints[END]->posn[Vertex::Y] * x;
    
	if ( det == 0.0 )
		return ON;
	else if ( det > 0.0 )
		return LEFT;
	else
		return RIGHT;
}

bool isLeft(Plane p, Vec3D& x) {
	return Vec3D::DotProduct(x - p.boundary[0], Vec3D::CrossProduct(p.boundary[1] - p.boundary[0], p.boundary[2] - p.boundary[0])) <= 0;
}

bool isSameSide(Plane p, Vec3D& x, Vec3D& y) {
	double det1 = Vec3D::DotProduct(x - p.boundary[0], Vec3D::CrossProduct(p.boundary[1] - p.boundary[0], p.boundary[2] - p.boundary[0]));
	double det2 = Vec3D::DotProduct(y - p.boundary[0], Vec3D::CrossProduct(p.boundary[1] - p.boundary[0], p.boundary[2] - p.boundary[0]));
	return !(det1 < 0) ^ (det2 < 0);
}

Plane::Plane(vector<Vec3D> boundary): boundary(boundary) {
	o = boundary[0];
	planeVector = Vec3D::Normal(boundary[1], boundary[0], boundary[2]);
}

Plane::Plane(Vec3D o, Vec3D p1, Vec3D p2): o(o) {
	planeVector = Vec3D::Normal(o, p1, p2);
	boundary.push_back(o);
	boundary.push_back(p1);
	boundary.push_back(p2);
}

QLineF::IntersectType Plane::intersect(Line l, Vec3D& intersection) {
	Vec3D result;
	Vec3D d01 = boundary[1] - boundary[0];
	Vec3D d02 = boundary[2] - boundary[0];
	Vec3D dl = l.p2 - l.p1;
	Vec3D d0 = l.p1 - boundary[0];

	double n = Vec3D::DotProduct(dl * -1, Vec3D::CrossProduct(d01, d02));
	double t = Vec3D::DotProduct(d0, Vec3D::CrossProduct(d01, d02));
	if (n == 0) {
		if (t != 0) {
			return QLineF::IntersectType::NoIntersection;
		}
		else {
			intersection = l.p2;
			return QLineF::IntersectType::UnboundedIntersection;
		}
	}
	intersection = l.p1 + dl * t / n;
	return QLineF::IntersectType::BoundedIntersection;
}

Line::Line(Vec3D left, Vec3D right) {
	p1 = left;
	p2 = right;
}

void addIfNotExist(vector<Vec3D>& list, Vec3D element) {
	for (auto& i : list) {
		if (abs(i.x() - element.x()) < EPS && abs(i.y() - element.y()) < EPS && abs(i.z() - element.z()) < EPS) {
			return;
		}
	}
	list.push_back(element);
}

void Edge::LeftToRight(vector<Vec3D>& boundary) {
	boundary.clear();
	for (auto& b : plane.boundary) {
		boundary.push_back(b);
	}

	if (plane.boundary[0].x() == plane.boundary[1].x() && 
			(plane.boundary[0].x() < MazeWidget::maze->viewer_posn[0] && plane.boundary[0].y() < plane.boundary[1].y() || 
			plane.boundary[0].x() > MazeWidget::maze->viewer_posn[0] && plane.boundary[0].y() > plane.boundary[1].y()) ||
		plane.boundary[0].y() == plane.boundary[1].y() &&
			(plane.boundary[0].y() < MazeWidget::maze->viewer_posn[1] && plane.boundary[0].x() > plane.boundary[1].x() || 
			plane.boundary[0].y() > MazeWidget::maze->viewer_posn[1] && plane.boundary[0].x() < plane.boundary[1].x())) {
		boundary[0] = plane.boundary[1];
		boundary[1] = plane.boundary[0];
		boundary[2] = plane.boundary[3];
		boundary[3] = plane.boundary[2];
	}
}

bool Edge::Clip(Vec3D o, vector<Vec3D> boundary, vector<Vec3D>& newBoundary, bool clipTop) {
	LeftToRight(newBoundary);

	if (clipTop) {
		newBoundary[2].SetZ(newBoundary[0].z());
		newBoundary[3].SetZ(newBoundary[1].z());
		newBoundary[0].SetZ(999);
		newBoundary[1].SetZ(999);
	}
	else if (!this->opaque) {
		newBoundary[0].SetZ(999);
		newBoundary[1].SetZ(999);
		newBoundary[2].SetZ(-999);
		newBoundary[3].SetZ(-999);
	}

	Vec3D center;
	for (auto& b : boundary) {
		center += b;
	}
	center /= boundary.size();

	for (int i = 0; i < boundary.size(); i++) {
		if (!newBoundary.size()) return false;

		vector<Vec3D> tmpBoundary;
		Plane p(o, boundary[i], boundary[(i + 1) % boundary.size()]);
		Vec3D intersection;
		QLineF::IntersectType type;

		bool prev = isSameSide(p, newBoundary.back(), center);
		for (int j = 0; j < newBoundary.size(); j++) {
			bool sameSide = isSameSide(p, newBoundary[j], center);
			if (prev && sameSide) {
				tmpBoundary.push_back(newBoundary[j]);
			}
			else if (prev ^ sameSide) {
				type = p.intersect(Line(newBoundary[(newBoundary.size() + j - 1) % newBoundary.size()], newBoundary[j]), intersection);
				if (type != QLineF::IntersectType::NoIntersection) tmpBoundary.push_back(intersection);
				if (!prev) tmpBoundary.push_back(newBoundary[j]);

				prev = sameSide;
			}
		}

		newBoundary.clear();
		for (auto b : tmpBoundary) {
			addIfNotExist(newBoundary, b);
		}
	}
	return newBoundary.size() > 2;
}

bool Edge::ClipTop(Vec3D o, vector<Vec3D> boundary, vector<Vec3D>& newBoundary) {
	LeftToRight(newBoundary);

	newBoundary[2].SetZ(newBoundary[0].z());
	newBoundary[3].SetZ(newBoundary[1].z());
	newBoundary[0].SetZ(999);
	newBoundary[1].SetZ(999);

	for (int i = 0; i < boundary.size(); i++) {
		if (!newBoundary.size()) return false;

		vector<Vec3D> tmpBoundary;
		Plane p(o, boundary[i], boundary[(i + 1) % boundary.size()]);
		Vec3D intersection;
		QLineF::IntersectType type;

		bool prev = isLeft(p, newBoundary.back());
		for (int j = 0; j < newBoundary.size(); j++) {
			bool sameSide = isLeft(p, newBoundary[j]);
			if (prev && sameSide) {
				tmpBoundary.push_back(newBoundary[j]);
			}
			else if (prev && !sameSide || !prev && sameSide) {
				type = p.intersect(Line(newBoundary[(newBoundary.size() + j - 1) % newBoundary.size()], newBoundary[j]), intersection);
				if (type != QLineF::IntersectType::NoIntersection) tmpBoundary.push_back(intersection);
				if (!prev) tmpBoundary.push_back(newBoundary[j]);

				prev = sameSide;
			}
		}

		newBoundary.clear();
		for (auto b : tmpBoundary) {
			addIfNotExist(newBoundary, b);
		}
	}
	return newBoundary.size() > 2;
}

void Edge::Draw(vector<Vec3D> boundary) {
#ifdef DEBUG
	glColor3f(color[0], color[1], color[2]);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < boundary.size(); i++) {
		glVertex2f(boundary[i].x(), boundary[i].y());
	}
	glEnd();
	return;
#endif
	//Vec3D thick = Vec3D::crossProduct(Vec3D(left.y(), wallHeight, left.x()), Vec3D(left.y(), 0, left.x()));
	//thick.normalize();
	//thick *= thickness / 2;
	vector<QPointF> pointList;
	
	double centerX = 0;
	double centerY = 0;
	for (auto& point : boundary) {
		Vec4D p(point.y(), point.z(), point.x(), 1);
		Vec4D v = MazeWidget::maze->projectionMatrix * (MazeWidget::maze->viewMatrix * p);
		v /= v.w();
		pointList.push_back(QPointF(v.x(), v.y()));
		centerX += v.x();
		centerY += v.y();
	}
	centerX /= boundary.size();
	centerY /= boundary.size();

	std::sort(pointList.begin(), pointList.end(), [=](QPointF& a, QPointF& b) {
		double dax = a.x() - centerX;
		double dbx = b.x() - centerX;
		double day = a.y() - centerY;
		double dby = b.y() - centerY;

		if (dax >= 0 && dbx < 0) {
			return true;
		}
		if (dax < 0 && dbx >= 0) {
			return false;
		}

		if (dax == 0 && dbx == 0) {
			if (day >= 0 && dby >= 0) {
				return a.y() > b.y();
			}
			return b.y() > a.y();
		}

		double det = dax * dby - dbx * day;
		if (det < 0) {
			return true;
		}
		if (det > 0) {
			return false;
		}

		double d1 = dax * dax + day * day;
		double d2 = dbx * dbx + dby * dby;
		return d1 > d2;
	});

	glBegin(GL_POLYGON);

	glColor3f(color[0], color[1], color[2]);

	for (auto& p : pointList) {
		glVertex2f(p.x(), -p.y());
	}

	glEnd();
}