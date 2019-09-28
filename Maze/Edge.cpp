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

bool isLeft(Plane p, QVector3D& c) {
	// corss product
	return 0;
}

Plane::Plane(QVector3D lt, QVector3D lb, QVector3D rt, QVector3D rb): lt(lt), lb(lb), rt(rt), rb(rb) {
	o = lt;
	planeVector = QVector3D::normal(lt, rt, lb);
}

Plane::Plane(QVector3D o, QVector3D p1, QVector3D p2): o(o) {
	planeVector = QVector3D::normal(o, p1, p2);
	lt = o;
	lb = o;
	rt = p1;
	rb = p2;
}

QLineF::IntersectType Plane::intersect(Plane p, QVector3D& intersection) {

}

Line::Line(QVector3D left, QVector3D right) {
	p1 = left;
	p2 = right;
}

bool Edge::Clip(QVector3D o, QVector3D leftTop, QVector3D leftBottom, QVector3D rightTop, QVector3D rightBottom, QVector3D& newLeftTop, QVector3D& newLeftBottom, QVector3D& newRightTop, QVector3D& newRightBottom) {
	Plane plane(QVector3D(endpoints[0]->posn[0], endpoints[0]->posn[1], wallHeight), 
				QVector3D(endpoints[0]->posn[0], endpoints[0]->posn[1], 0), 
				QVector3D(endpoints[1]->posn[0], endpoints[1]->posn[1], wallHeight),
				QVector3D(endpoints[1]->posn[0], endpoints[1]->posn[1], 0));

	Plane leftPlane(o, leftTop, leftBottom);
	Plane rightPlane(o, rightTop, rightBottom);
	Plane topPlane(o, leftTop, rightTop);
	Plane bottomPlane(o, leftBottom, rightBottom);

	QLineF::IntersectType type;
	QVector3D intersection;

	bool pltInsideLeft = !isLeft(leftPlane, plane.lt);
	bool plbInsideLeft = !isLeft(leftPlane, plane.lb);
	bool prtInsideLeft = !isLeft(leftPlane, plane.rt);
	bool prbInsideLeft = !isLeft(leftPlane, plane.rb);

	if (!pltInsideLeft && !plbInsideLeft && !prtInsideLeft && !prbInsideLeft) {
		return false;
	}
	if (!pltInsideLeft) {
		type = plane.intersect(leftPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.lt = intersection;
	}
	if (!plbInsideLeft) {
		type = plane.intersect(leftPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.lb = intersection;
	}
	if (!prtInsideLeft) {
		type = plane.intersect(leftPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.rt = intersection;
	}
	if (!prbInsideLeft) {
		type = plane.intersect(leftPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.rb = intersection;
	}

	bool pltInsideRight = isLeft(rightPlane, plane.lt);
	bool plbInsideRight = isLeft(rightPlane, plane.lb);
	bool prtInsideRight = isLeft(rightPlane, plane.rt);
	bool prbInsideRight = isLeft(rightPlane, plane.rb);

	if (!pltInsideRight && !plbInsideRight && !prtInsideRight && !prbInsideRight) {
		return false;
	}
	if (!pltInsideRight) {
		type = plane.intersect(rightPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.lt = intersection;
	}
	if (!plbInsideRight) {
		type = plane.intersect(rightPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.lb = intersection;
	}
	if (!prtInsideRight) {
		type = plane.intersect(rightPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.rt = intersection;
	}
	if (!prbInsideRight) {
		type = plane.intersect(rightPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.rb = intersection;
	}

	bool pltInsideTop = !isLeft(topPlane, plane.lt);
	bool plbInsideTop = !isLeft(topPlane, plane.lb);
	bool prtInsideTop = !isLeft(topPlane, plane.rt);
	bool prbInsideTop = !isLeft(topPlane, plane.rb);

	if (!pltInsideTop && !plbInsideTop && !prtInsideTop && !prbInsideTop) {
		return false;
	}
	if (!pltInsideTop) {
		type = plane.intersect(topPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.lt = intersection;
	}
	if (!plbInsideTop) {
		type = plane.intersect(topPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.lb = intersection;
	}
	if (!prtInsideTop) {
		type = plane.intersect(topPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.rt = intersection;
	}
	if (!prbInsideTop) {
		type = plane.intersect(topPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.rb = intersection;
	}

	bool pltInsideBottom = isLeft(bottomPlane, plane.lt);
	bool plbInsideBottom = isLeft(bottomPlane, plane.lb);
	bool prtInsideBottom = isLeft(bottomPlane, plane.rt);
	bool prbInsideBottom = isLeft(bottomPlane, plane.rb);

	if (!pltInsideBottom && !plbInsideBottom && !prtInsideBottom && !prbInsideBottom) {
		return false;
	}
	if (!pltInsideBottom) {
		type = plane.intersect(bottomPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.lt = intersection;
	}
	if (!plbInsideBottom) {
		type = plane.intersect(bottomPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.lb = intersection;
	}
	if (!prtInsideBottom) {
		type = plane.intersect(bottomPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.rt = intersection;
	}
	if (!prbInsideBottom) {
		type = plane.intersect(bottomPlane, intersection);
		if (type == QLineF::IntersectType::NoIntersection) {
			return false;
		}
		plane.rb = intersection;
	}

	bool p0IsLeft = isLeft(o, edge.p2(), edge.p1());
	if (p0IsLeft) {
		newLeft = edge.p1();
		newRight = edge.p2();
	}
	else {
		newRight = edge.p1();
		newLeft = edge.p2();
	}
	return true;
}

void Edge::Draw(QVector3D leftTop, QVector3D leftBottom, QVector3D rightTop, QVector3D rightBottom) {
	/*glColor3f(color[0], color[1], color[2]);
	glBegin(GL_LINES);
	glVertex2f(left.x(), left.y());
	glVertex2f(right.x(), right.y());
	glEnd();
	return;*/
	//QVector3D thick = QVector3D::crossProduct(QVector3D(left.y(), wallHeight, left.x()), QVector3D(left.y(), 0, left.x()));
	//thick.normalize();
	//thick *= thickness / 2;
	QVector4D s0(leftTop.y(), leftTop.z(), leftTop.x(), 1);
	QVector4D s1(leftBottom.y(), leftBottom.z(), leftBottom.x(), 1);
	QVector4D e0(rightTop.y(), rightTop.z(), rightTop.x(), 1);
	QVector4D e1(rightBottom.y(), rightBottom.z(), rightBottom.x(), 1);
	/*
	QVector4D s0(left.y() - thick.y(), wallHeight - thick.z(), left.x() - thick.x(), 1);
	QVector4D s1(left.y() - thick.y(),          0 - thick.z(), left.x() - thick.x(), 1);
	QVector4D s2(left.y() + thick.y(), wallHeight + thick.z(), left.x() + thick.x(), 1);
	QVector4D s3(left.y() + thick.y(),          0 + thick.z(), left.x() + thick.x(), 1);

	QVector4D e0(right.y() - thick.y(), wallHeight - thick.z(), right.x() - thick.x(), 1);
	QVector4D e1(right.y() - thick.y(),          0 - thick.z(), right.x() - thick.x(), 1);
	QVector4D e2(right.y() + thick.y(), wallHeight + thick.z(), right.x() + thick.x(), 1);
	QVector4D e3(right.y() + thick.y(),          0 + thick.z(), right.x() + thick.x(), 1);
	*/

	QVector4D sv0 = MazeWidget::maze->projectionMatrix * (MazeWidget::maze->viewMatrix * s0);
	sv0 /= sv0.w();
	QVector4D sv1 = MazeWidget::maze->projectionMatrix * (MazeWidget::maze->viewMatrix * s1);
	sv1 /= sv1.w();
	
	//QVector4D sv2 = MazeWidget::maze->projectionMatrix * (MazeWidget::maze->viewMatrix * s2);
	//sv2 /= sv2.w();
	//QVector4D sv3 = MazeWidget::maze->projectionMatrix * (MazeWidget::maze->viewMatrix * s3);
	//sv3 /= sv3.w();

	QVector4D ev0 = MazeWidget::maze->projectionMatrix * (MazeWidget::maze->viewMatrix * e0);
	ev0 /= ev0.w();
	QVector4D ev1 = MazeWidget::maze->projectionMatrix * (MazeWidget::maze->viewMatrix * e1);
	ev1 /= ev1.w();

	//QVector4D ev2 = MazeWidget::maze->projectionMatrix * (MazeWidget::maze->viewMatrix * e2);
	//ev2 /= ev2.w();
	//QVector4D ev3 = MazeWidget::maze->projectionMatrix * (MazeWidget::maze->viewMatrix * e3);
	//ev3 /= ev3.w();
	
	glBegin(GL_QUADS);
	glColor3f(color[0], color[1], color[2]);
	glVertex2f(sv0.x(), sv0.y());
	glVertex2f(sv1.x(), sv1.y());
	glVertex2f(ev1.x(), ev1.y());
	glVertex2f(ev0.x(), ev0.y());
	/*
	glVertex2f(ev2.x(), ev2.y());
	glVertex2f(ev3.x(), ev3.y());
	glVertex2f(sv3.x(), sv3.y());
	glVertex2f(sv2.x(), sv2.y());

	glVertex2f(sv3.x(), sv3.y());
	glVertex2f(sv1.x(), sv1.y());
	glVertex2f(ev1.x(), ev1.y());
	glVertex2f(ev3.x(), ev3.y());

	glVertex2f(sv2.x(), sv2.y());
	glVertex2f(sv0.x(), sv0.y());
	glVertex2f(ev0.x(), ev0.y());
	glVertex2f(ev2.x(), ev2.y());
	*/
	glEnd();
}