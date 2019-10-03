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

const char  Edge::LEFT		= 0;
const char  Edge::RIGHT		= 1;
const char  Edge::ON			= 2;
const char  Edge::NEITHER	= 3;

const char  Edge::START		= 0;
const char  Edge::END		= 1;


//***********************************************************************
//
// * Constructor to set up the start and end and the color of the edge
//=======================================================================
Edge::
Edge(int i, Vertex *start, Vertex *end, float r, float g, float b)
//=======================================================================
{
	index = i;
	endpoints[START] = start;
	endpoints[END] = end;

	neighbors[0] = neighbors[1] = NULL;

	opaque = true;

	color[0] = r;
	color[1] = g;
	color[2] = b;

	double leftX, rightX, leftY, rightY;
	leftX = endpoints[0]->posn[0];
	rightX = endpoints[1]->posn[0];
	leftY = endpoints[0]->posn[1];
	rightY = endpoints[1]->posn[1];

	plane = Plane(vector<QVector3D>({ QVector3D(leftX, leftY, wallHeight), QVector3D(rightX, rightY, wallHeight), QVector3D(rightX, rightY, 0), QVector3D(leftX, leftY, 0) }));
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

double det3D(double mat[][3]) {
	double det =  mat[0][0] * mat[1][1] * mat[2][2] +
		mat[0][1] * mat[1][2] * mat[2][0] +
		mat[0][2] * mat[1][0] * mat[2][1] -
		mat[0][2] * mat[1][1] * mat[2][0] -
		mat[0][1] * mat[1][0] * mat[2][2] -
		mat[0][0] * mat[1][2] * mat[2][1];

	if (abs(det) < EPS) return 0;

	return det;
}

double* inverse(double mat[][3]) {
	double res[9];
	double det = det3D(mat);

	if (abs(det) < EPS) return 0;

	res[0] = (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]) / det;
	res[1] = (mat[0][2] * mat[2][1] - mat[0][1] * mat[2][2]) / det;
	res[2] = (mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1]) / det;
	res[3] = (mat[1][2] * mat[2][0] - mat[1][0] * mat[2][2]) / det;
	res[4] = (mat[0][0] * mat[2][2] - mat[0][2] * mat[2][0]) / det;
	res[5] = (mat[0][2] * mat[0][0] - mat[0][0] * mat[1][2]) / det;
	res[6] = (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]) / det;
	res[7] = (mat[0][1] * mat[2][0] - mat[0][0] * mat[2][1]) / det;
	res[8] = (mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0]) / det;
	return res;
}

QVector3D mul(double* mat, QVector3D v) {
	QVector3D res;
	res.setX(mat[0] * v.x() + mat[1] * v.y() + mat[2] * v.z());
	res.setY(mat[3] * v.x() + mat[4] * v.y() + mat[5] * v.z());
	res.setZ(mat[6] * v.x() + mat[7] * v.y() + mat[8] * v.z());
	return res;
}

bool isSameSide(Plane p, QVector3D& x, QVector3D& y) {
	QVector3D A = p.boundary[0];
	QVector3D B = p.boundary[1] - A;
	QVector3D C = p.boundary[2] - A;
	QVector3D X = x - A;
	QVector3D Y = y - A;
	double mat1[3][3] = {
		{B.x(), B.y(), B.z()},
		{C.x(), C.y(), C.z()},
		{X.x(), X.y(), X.z()}
	};
	double mat2[3][3] = {
		{B.x(), B.y(), B.z()},
		{C.x(), C.y(), C.z()},
		{Y.x(), Y.y(), Y.z()}
	};
	return !(((det3D(mat1) < 0) ^ (det3D(mat2) < 0)));
}

bool isLeft(Plane p, QVector3D& x) {
	QVector3D B = p.boundary[1] - p.boundary[0];
	QVector3D C = p.boundary[2] - p.boundary[0];
	QVector3D X = x - p.boundary[0];
	double mat1[3][3] = {
		{B.x(), B.y(), B.z()},
		{C.x(), C.y(), C.z()},
		{X.x(), X.y(), X.z()}
	};

	return det3D(mat1) <= 0;
}

bool isBounded(QVector3D v) {
	return v.y() >= 0 && v.y() <= 1 && v.z() >= 0 && v.z() <= 1 && v.y() + v.z() <= 1;
}

Plane::Plane(vector<QVector3D> boundary): boundary(boundary) {
	o = boundary[0];
	planeVector = QVector3D::normal(boundary[1], boundary[0], boundary[2]);
}

Plane::Plane(QVector3D o, QVector3D p1, QVector3D p2): o(o) {
	planeVector = QVector3D::normal(o, p1, p2);
	boundary.push_back(o);
	boundary.push_back(p1);
	boundary.push_back(p2);
}

QLineF::IntersectType Plane::intersect(Line l, QVector3D& intersection) {
	QVector3D result;
	QVector3D p0 = boundary[0];
	QVector3D d01 = boundary[1] - p0;
	QVector3D d02 = boundary[2] - p0;
	QVector3D dl = l.p1 - l.p2;
	QVector3D d0 = l.p1 - p0;

	double mat[3][3] = {
		{dl.x(), d01.x(), d02.x()},
		{dl.y(), d01.y(), d02.y()},
		{dl.z(), d01.z(), d02.z()}
	};
	double* inv = inverse(mat);
	if (!inv) return QLineF::IntersectType::NoIntersection;

	QVector3D res = mul(inv, d0);
	if (res.x() < 0) return QLineF::IntersectType::NoIntersection;

	QVector3D d = dl * res.x();
	intersection = l.p1 - d;

	if (abs(d.length()) < EPS) {
		return QLineF::IntersectType::UnboundedIntersection;
	}

	return QLineF::IntersectType::BoundedIntersection;
}

Line::Line(QVector3D left, QVector3D right) {
	p1 = left;
	p2 = right;
}

void addIfNotExist(vector<QVector3D>& list, QVector3D element) {
	for (auto& i : list) {
		if (abs(i.x() - element.x()) < EPS && abs(i.y() - element.y()) < EPS && abs(i.z() - element.z()) < EPS) {
			return;
		}
	}
	list.push_back(element);
}

bool Edge::Clip(QVector3D o, vector<QVector3D> boundary, vector<QVector3D>& newBoundary) {
	newBoundary.clear();
	for (int i = 0; i < plane.boundary.size(); i++) {
		newBoundary.push_back(plane.boundary[i]);
	}

	QVector4D p0 = MazeWidget::maze->viewMatrix * QVector4D(plane.boundary[0].y(), plane.boundary[0].z(), plane.boundary[0].x(), 1);
	QVector4D p1 = MazeWidget::maze->viewMatrix * QVector4D(plane.boundary[1].y(), plane.boundary[1].z(), plane.boundary[1].x(), 1);
	p0 /= p0.w();
	p1 /= p1.w();

	if (p0.x() > p1.x() || p0.x() == p1.x() && p0.z() > p1.z()) {
		newBoundary[0] = plane.boundary[1];
		newBoundary[1] = plane.boundary[0];
		newBoundary[2] = plane.boundary[3];
		newBoundary[3] = plane.boundary[2];
	}

	for (auto& b : boundary) {

	}

	if (!this->opaque) {
		newBoundary[0].setZ(999);
		newBoundary[1].setZ(999);
		newBoundary[2].setZ(-999);
		newBoundary[3].setZ(-999);
	}

	QVector3D center(0, 0, 0);
	for (auto& b : boundary) {
		center += b;
	}
	center /= boundary.size();
	for (int i = 0; i < boundary.size(); i++) {
		if (!newBoundary.size()) return false;

		vector<QVector3D> tmpBoundary;
		Plane p(o, boundary[i], boundary[(i + 1) % boundary.size()]);
		QVector3D intersection;
		QLineF::IntersectType type;

		bool prev = isSameSide(p, newBoundary.back(), center);
		for (int j = 0; j < newBoundary.size(); j++) {
			bool sameSide = isSameSide(p, newBoundary[j], center);
			if (prev && sameSide) {
				tmpBoundary.push_back(newBoundary[j]);
			}
			else if (prev && !sameSide || !prev && sameSide) {
				type = p.intersect(Line(newBoundary[j], newBoundary[(newBoundary.size() + j - 1) % newBoundary.size()]), intersection);
				if (type != QLineF::IntersectType::NoIntersection) {
					tmpBoundary.push_back(intersection);
				}
				if (!prev) 
					tmpBoundary.push_back(newBoundary[j]);

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

bool Edge::ClipHorizontal(QVector3D o, vector<QVector3D> boundary, vector<QVector3D>& newBoundary) {
	newBoundary.clear();
	for (int i = 0; i < plane.boundary.size(); i++) {
		newBoundary.push_back(plane.boundary[i]);
	}

	QVector3D p0 = MazeWidget::maze->viewMatrix * QVector3D(plane.boundary[0].y(), plane.boundary[0].z(), plane.boundary[0].x());
	QVector3D p1 = MazeWidget::maze->viewMatrix * QVector3D(plane.boundary[1].y(), plane.boundary[1].z(), plane.boundary[1].x());

	if (p0.x() > p1.x() || p0.x() == p1.x() && p0.z() > p1.z()) {
		newBoundary[0] = plane.boundary[1];
		newBoundary[1] = plane.boundary[0];
		newBoundary[2] = plane.boundary[3];
		newBoundary[3] = plane.boundary[2];
	}

	QVector3D center(0, 0, 0);
	for (auto& b : boundary) {
		center += b;
	}
	center /= boundary.size();
	for (int i = 1; i < boundary.size(); i += 2) {
		if (!newBoundary.size()) return false;

		vector<QVector3D> tmpBoundary;
		Plane p(o, boundary[i], boundary[(i + 1) % boundary.size()]);
		QVector3D intersection;
		QLineF::IntersectType type;

		bool prev = isLeft(p, newBoundary.back());
		for (int j = 0; j < newBoundary.size(); j++) {
			bool sameSide = isLeft(p, newBoundary[j]);
			if (prev && sameSide) {
				tmpBoundary.push_back(newBoundary[j]);
			}
			else if (prev && !sameSide || !prev && sameSide) {
				type = p.intersect(Line(newBoundary[j], newBoundary[(newBoundary.size() + j - 1) % newBoundary.size()]), intersection);
				if (type != QLineF::IntersectType::NoIntersection) {
					tmpBoundary.push_back(intersection);
				}
				if (!prev)
					tmpBoundary.push_back(newBoundary[j]);

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

bool Edge::ClipTop(QVector3D o, vector<QVector3D> boundary, vector<QVector3D>& newBoundary) {
	newBoundary.clear();
	for (int i = 0; i < plane.boundary.size(); i++) {
		newBoundary.push_back(plane.boundary[i]);
	}

	QVector3D p0 = MazeWidget::maze->viewMatrix * QVector3D(plane.boundary[0].y(), plane.boundary[0].z(), plane.boundary[0].x());
	QVector3D p1 = MazeWidget::maze->viewMatrix * QVector3D(plane.boundary[1].y(), plane.boundary[1].z(), plane.boundary[1].x());

	if (p0.x() > p1.x() || p0.x() == p1.x() && p0.z() > p1.z()) {
		newBoundary[0] = plane.boundary[1];
		newBoundary[1] = plane.boundary[0];
		newBoundary[2] = plane.boundary[3];
		newBoundary[3] = plane.boundary[2];
	}

	newBoundary[2].setZ(newBoundary[0].z());
	newBoundary[3].setZ(newBoundary[1].z());
	newBoundary[0].setZ(999);
	newBoundary[1].setZ(999);

	QVector3D center(0, 0, 0);
	for (auto& b : boundary) {
		center += b;
	}
	center /= boundary.size();
	for (int i = 0; i < boundary.size(); i++) {
		if (!newBoundary.size()) return false;

		vector<QVector3D> tmpBoundary;
		Plane p(o, boundary[i], boundary[(i + 1) % boundary.size()]);
		QVector3D intersection;
		QLineF::IntersectType type;

		bool prev = isSameSide(p, newBoundary.back(), center);
		for (int j = 0; j < newBoundary.size(); j++) {
			bool sameSide = isSameSide(p, newBoundary[j], center);
			if (prev && sameSide) {
				tmpBoundary.push_back(newBoundary[j]);
			}
			else if (prev && !sameSide || !prev && sameSide) {
				type = p.intersect(Line(newBoundary[j], newBoundary[(newBoundary.size() + j - 1) % newBoundary.size()]), intersection);
				if (type != QLineF::IntersectType::NoIntersection) {
					tmpBoundary.push_back(intersection);
				}
				if (!prev)
					tmpBoundary.push_back(newBoundary[j]);

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

void Edge::Draw(vector<QVector3D> boundary) {
#ifdef DEBUG
	glColor3f(color[0], color[1], color[2]);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < boundary.size(); i++) {
		glVertex2f(boundary[i].x(), boundary[i].y());
	}
	glEnd();
	return;
#endif
	//QVector3D thick = QVector3D::crossProduct(QVector3D(left.y(), wallHeight, left.x()), QVector3D(left.y(), 0, left.x()));
	//thick.normalize();
	//thick *= thickness / 2;
	vector<QPointF> pointList;
	
	double centerX = 0;
	double centerY = 0;
	for (auto& point : boundary) {
		QVector4D p(point.y(), point.z(), point.x(), 1);
		QVector4D v = MazeWidget::maze->projectionMatrix * (MazeWidget::maze->viewMatrix * p);
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