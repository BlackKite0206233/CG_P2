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


#define EPS 1e-10

const char Edge::LEFT    = 0;
const char Edge::RIGHT   = 1;

double Edge::wallHeight = 2;
double Edge::thickness = 2;

//***********************************************************************
//
// * Constructor to set up the start and end and the color of the edge
//=======================================================================
Edge::Edge(int i, Vec3D topLeft, Vec3D topRight, Vec3D bottomLeft, Vec3D bottomRight, float r, float g, float b, bool _opaque) {
	index = i;

	neighbors[0] = neighbors[1] = NULL;

	color[0] = r;
	color[1] = g;
	color[2] = b;

	opaque = _opaque;

	edgeBoundary = vector<Vec3D>({ topLeft, topRight, bottomRight, bottomLeft });
}

void Edge::AddIfNotExist(vector<Vec3D>& list, const Vec3D& element) {
	for (auto& i : list) {
		if (abs(i.x() - element[0]) < EPS && abs(i.y() - element[1]) < EPS && abs(i.z() - element[2]) < EPS) {
			return;
		}
	}
	list.push_back(element);
}

bool Edge::IsLeft(const Edge& p, const Vec3D& x) {
	return Vec3D::DotProduct(x - p.edgeBoundary[0], Vec3D::CrossProduct(p.edgeBoundary[1] - p.edgeBoundary[0], p.edgeBoundary[2] - p.edgeBoundary[0])) <= EPS;
}

bool Edge::IsSameSide(const Edge& p, const Vec3D& x, const Vec3D& y) {
	Vec3D d1 = p.edgeBoundary[1] - p.edgeBoundary[0];
	Vec3D d2 = p.edgeBoundary[2] - p.edgeBoundary[0];
	double det1 = Vec3D::DotProduct(x - p.edgeBoundary[0], Vec3D::CrossProduct(d1, d2));
	double det2 = Vec3D::DotProduct(y - p.edgeBoundary[0], Vec3D::CrossProduct(d1, d2));
	return !(det1 < 0) ^ (det2 < 0);
}

Edge::Edge(vector<Vec3D> boundary): edgeBoundary(boundary) {
}

Edge::Edge(Vec3D o, Vec3D p1, Vec3D p2) {
	edgeBoundary.push_back(o);
	edgeBoundary.push_back(p1);
	edgeBoundary.push_back(p2);
}

QLineF::IntersectType Edge::Intersect(const Vec3D& p1, const Vec3D& p2, Vec3D& intersection) {
	Vec3D result;
	Vec3D d01 = edgeBoundary[1] - edgeBoundary[0];
	Vec3D d02 = edgeBoundary[2] - edgeBoundary[0];
	Vec3D dl = p2 - p1;
	Vec3D d0 = p1 - edgeBoundary[0];

	double n = Vec3D::DotProduct(dl * -1, Vec3D::CrossProduct(d01, d02));
	double t = Vec3D::DotProduct(d0, Vec3D::CrossProduct(d01, d02));
	if (abs(n) < EPS) {
		if (abs(t) >= EPS) {
			return QLineF::IntersectType::NoIntersection;
		}
		else {
			intersection = p2;
			return QLineF::IntersectType::UnboundedIntersection;
		}
	}
	intersection = p1 + dl * t / n;
	return QLineF::IntersectType::BoundedIntersection;
}

void Edge::LeftToRight(vector<Vec3D>& boundary) {
	boundary.clear();
	for (auto& b : edgeBoundary) {
		boundary.push_back(b);
	}

	if (edgeBoundary[0].x() == edgeBoundary[1].x() &&
			(edgeBoundary[0].x() < MazeWidget::maze->viewer_posn[0] && edgeBoundary[0].y() < edgeBoundary[1].y() ||
				edgeBoundary[0].x() > MazeWidget::maze->viewer_posn[0] && edgeBoundary[0].y() > edgeBoundary[1].y()) ||
		edgeBoundary[0].y() == edgeBoundary[1].y() &&
			(edgeBoundary[0].y() < MazeWidget::maze->viewer_posn[1] && edgeBoundary[0].x() > edgeBoundary[1].x() ||
				edgeBoundary[0].y() > MazeWidget::maze->viewer_posn[1] && edgeBoundary[0].x() < edgeBoundary[1].x())) {
		boundary[0] = edgeBoundary[1];
		boundary[1] = edgeBoundary[0];
		boundary[2] = edgeBoundary[3];
		boundary[3] = edgeBoundary[2];
	}
}

bool Edge::Clip(const Vec3D& o, const vector<Vec3D>& boundary, const Vec3D& center, vector<Vec3D>& newBoundary, bool clipTop) {
	newBoundary = edgeBoundary;

	for (int i = 0; i < boundary.size(); i++) {
		if (!newBoundary.size()) return false;

		vector<Vec3D> tmpBoundary;
		Edge p(o, boundary[i], boundary[(i + 1) % boundary.size()]);
		Vec3D intersection;
		QLineF::IntersectType type;

		bool prev = Edge::IsSameSide(p, newBoundary.back(), center);
		for (int j = 0; j < newBoundary.size(); j++) {
			bool sameSide = Edge::IsSameSide(p, newBoundary[j], center);
			if (prev && sameSide) {
				tmpBoundary.push_back(newBoundary[j]);
			}
			else if (prev ^ sameSide) {
				type = p.Intersect(newBoundary[(newBoundary.size() + j - 1) % newBoundary.size()], newBoundary[j], intersection);
				if (type != QLineF::IntersectType::NoIntersection) tmpBoundary.push_back(intersection);
				if (!prev) tmpBoundary.push_back(newBoundary[j]);

				prev = sameSide;
			}
		}

		newBoundary = tmpBoundary;
	}
	vector<Vec3D> tmpBoundary = newBoundary;
	newBoundary.clear();
	for (auto b : tmpBoundary) {
		Edge::AddIfNotExist(newBoundary, b);
	}
	return newBoundary.size() > 2;
}

void Edge::Draw(const vector<Vec3D>& boundary) {
#ifdef DEBUG
	glColor3f(color[0], color[1], color[2]);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < boundary.size(); i++) {
		glVertex2f(boundary[i][0], boundary[i][1]);
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
		Vec4D p(point[1], point[2], point[0], 1);
		Vec4D v = MazeWidget::maze->projectionMatrix * (MazeWidget::maze->viewMatrix * p);
		v /= v.w();
		pointList.push_back(QPointF(v.x(), v.y()));
		//centerX += v.x();
		//centerY += v.y();
	}
	//centerX /= boundary.size();
	//centerY /= boundary.size();

	/*std::sort(pointList.begin(), pointList.end(), [=](const QPointF& a, const QPointF& b) {
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
	});*/

	glBegin(GL_POLYGON);
	glColor3f(color[0], color[1], color[2]);

	for (auto& p : pointList) {
		glVertex2f(p.x(), -p.y());
	}
	/*if (!isFloor && !isCeiling) {

	}*/
	/*else {
		if (isCeiling) {
			glBindTexture(GL_TEXTURE_2D, MazeWidget::maze->sky);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, MazeWidget::maze->ground);
		}

		for (auto& p : pointList) {
			glVertex2f(p.x(), -p.y());
		}

		glDisable(GL_TEXTURE_2D);
	}*/

	glEnd();
}