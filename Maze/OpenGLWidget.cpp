#include "OpenGLWidget.h"
#include <iostream>
#include "MazeWidget.h"
#include <gl\gl.h>
#include <gl\GLU.h>
#include <QMatrix4x4>
#include <QLineF>

OpenGLWidget::OpenGLWidget(QWidget *parent) : QGLWidget(parent)
{
	
	top_z = 1.5f;
	but_z = -1;

	QDir dir("Pic");
	if(dir.exists())
		pic_path = "Pic/";
	else
		pic_path = "../x64/Release/Pic/";
}
void OpenGLWidget::initializeGL()
{
	glClearColor(0,0,0,1);
	glEnable(GL_TEXTURE_2D);
	loadTexture2D(pic_path + "grass.png",grass_ID);
	loadTexture2D(pic_path + "sky.png",sky_ID);
}
void OpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(MazeWidget::maze!=NULL)
	{
		//View 1
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0 , 0 , MazeWidget::w/2 , MazeWidget::h);
		float d = std::max(MazeWidget::maze->max_xp, MazeWidget::maze->max_yp);
		glOrtho (-0.1, d +0.1, -0.1 , d + 0.1, 0 , 10);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Mini_Map();

		//View 2
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(MazeWidget::w/2,0, MazeWidget::w/2, MazeWidget::h);
		glOrtho(-10, 10, -10, 10, 0, 999);
		/*gluPerspective 定義透視
		//視野大小, nearplane, farplane, distance
		//Note: You shouldn't use this function to get view matrix, otherwise you will get 0.
		*/
		//gluPerspective(MazeWidget::maze->viewer_fov, 1 , 0.01 , 200);

		/* gluLookAt
		//原本相機位置
		//看的方向
		//哪邊是上面
		//Note: You shouldn't use this function to get view matrix, otherwise you will get 0.
		*/
		float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
		float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
		float viewerPosZ = MazeWidget::maze->viewer_posn[Maze::Z];

		/*gluLookAt(viewerPosX, viewerPosZ, viewerPosY,
			viewerPosX + cos(degree_change(MazeWidget::maze->viewer_dir)), viewerPosZ, viewerPosY + sin(degree_change(MazeWidget::maze->viewer_dir)),
			0.0, -1.0, 0.0);*/
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Map_3D();
	}
}
void OpenGLWidget::resizeGL(int w,int h)
{
}

//Draw Left Part
void OpenGLWidget::Mini_Map()	
{					
	glBegin(GL_LINES);

	float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
	float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
	float viewerPosZ = MazeWidget::maze->viewer_posn[Maze::Z];

	for(int i = 0 ; i < (int)MazeWidget::maze->num_edges; i++)
	{
		float edgeStartX = MazeWidget::maze->edges[i]->endpoints[Edge::START]->posn[Vertex::X];
		float edgeStartY = MazeWidget::maze->edges[i]->endpoints[Edge::START]->posn[Vertex::Y];
		float edgeEndX = MazeWidget::maze->edges[i]->endpoints[Edge::END]->posn[Vertex::X];
		float edgeEndY = MazeWidget::maze->edges[i]->endpoints[Edge::END]->posn[Vertex::Y];

		glColor3f(MazeWidget::maze->edges[i]->color[0] , MazeWidget::maze->edges[i]->color[1], MazeWidget::maze->edges[i]->color[2]);
		if(MazeWidget::maze->edges[i]->opaque)
		{
			glVertex2f(edgeStartX, edgeStartY);
			glVertex2f(edgeEndX, edgeEndY);
		}
	}

	//draw frustum
	float len = 10.1;
	glColor3f(1, 1, 1);
	glVertex2f(viewerPosX, viewerPosY);
	glVertex2f(viewerPosX + (MazeWidget::maze->max_xp) * len * cos(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov/2)) ,
		viewerPosY + (MazeWidget::maze->max_yp) * len * sin(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov/2)));

	glVertex2f(viewerPosX, viewerPosY);
	glVertex2f(viewerPosX + (MazeWidget::maze->max_xp) * len * cos(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov/2)) ,
		viewerPosY + (MazeWidget::maze->max_yp) * len *  sin(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov/2)));
	glEnd();
}


//**********************************************************************
//
// * Draws the first-person view of the maze.
//   THIS IS THE FUINCTION YOU SHOULD MODIFY.
//
//Note: You must not use any openGL build-in function to set model matrix, view matrix and projection matrix.
//		ex: gluPerspective, gluLookAt, glTraslatef, glRotatef... etc.
//		Otherwise, You will get 0 !
//======================================================================

bool intersection(double sX, double sY, double eX, double eY, double csX, double csY, double angleL, double angleR, double* isX, double* isY, double* ieX, double* ieY) {
	double maxLen = std::max(pow(sX - csX, 2) + pow(sY - csY, 2), pow(eX - csX, 2) + pow(eY - csY, 2));
	double leX = csX + maxLen * cos(angleL * M_PI / 180);
	double leY = csY + maxLen * sin(angleL * M_PI / 180);
	double reX = csX + maxLen * cos(angleR * M_PI / 180);
	double reY = csY + maxLen * sin(angleR * M_PI / 180);

	QLineF line(sX, sY, eX, eY);
	QPointF intersectPointL, intersectPointR;
	bool startL, startR, endL, endR;
	int typeL = line.intersect(QLineF(csX, csY, leX, leY), &intersectPointL);
	int typeR = line.intersect(QLineF(csX, csY, reX, reY), &intersectPointR);
	startL = (leX - csX) * (sY - csY) > (leY - csY) * (sX - csX);
	startR = (reX - csX) * (sY - csY) > (reY - csY) * (sX - csX);
	endL   = (leX - csX) * (eY - csY) > (leY - csY) * (eX - csX);
	endR   = (reX - csX) * (eY - csY) > (reY - csY) * (eX - csX);
	if (!startL && !endL && startR && endR) {
		*isX = sX;
		*isY = sY;
		*ieX = eX;
		*ieY = eX;
	}
	else if (typeR == 1 && !startL && !endL && !startR && endR) {
		*isX = intersectPointR.toPoint().x();
		*isY = intersectPointR.toPoint().y();
		*ieX = eX;
		*ieY = eX;
	}
	else if (typeL == 1 && !startL && endL && startR && endR) {
		*isX = sX;
		*isY = sY;
		*ieX = intersectPointL.toPoint().x();
		*ieY = intersectPointL.toPoint().y();
	}
	else if (typeR == 1 && !startL && !endL && !endR && startR) {
		*isX = sX;
		*isY = sY;
		*ieX = intersectPointR.toPoint().x();
		*ieY = intersectPointR.toPoint().y();
	}
	else if (typeL == 1 && startL && !endL && startR && endR) {
		*isX = intersectPointL.toPoint().x();
		*isY = intersectPointL.toPoint().y();
		*ieX = eX;
		*ieY = eY;
	}
	else if (typeL == 1 && typeR == 1 && (startL && !endL && startR && !endR || !startL && endL && !startR && endR)) {
		*isX = intersectPointL.toPoint().x();
		*isY = intersectPointL.toPoint().y();
		*ieX = intersectPointR.toPoint().x();
		*ieY = intersectPointR.toPoint().y();
	}
	else {
		return 0;
	}
	return 1;
}

void OpenGLWidget::Map_3D()
{
	glLoadIdentity();
	// 畫右邊區塊的所有東西
	float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
	float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
	float viewerPosZ = MazeWidget::maze->viewer_posn[Maze::Z];
	float viewerDir  = MazeWidget::maze->viewer_dir;
	float verticalAngle = MazeWidget::maze->viewer_vertical_angle;
	float fov  = MazeWidget::maze->viewer_fov;

	QMatrix4x4 rotation;
	rotation.rotate(-viewerDir + 90, 0, 1, 0);
	QMatrix4x4 translation;
	translation.translate(-viewerPosX, 0, -viewerPosY);

	QMatrix4x4 projectionMatrix;
	projectionMatrix.perspective(atan(9.0 / 16.0 * tan(fov * M_PI / 360.0)) * 360.0 / M_PI, 16.0 / 9.0, 0.001, 100);

	Edge **edges = MazeWidget::maze->view_cell->edges;

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		
		double startX, startY, endX, endY;
		bool visible = intersection(edges[i]->endpoints[0]->posn[0], edges[i]->endpoints[0]->posn[1], edges[i]->endpoints[1]->posn[0], edges[i]->endpoints[1]->posn[1], viewerPosX, viewerPosY, viewerDir + fov / 2, viewerDir - fov / 2, &startX, &startY, &endX, &endY);
		std::cout << visible << "\r";
		if (visible) {
			QVector4D worldStartTop =  QVector4D(startX, 2, startY, 1);
			QVector4D worldStartDown = QVector4D(startX, -2, startY, 1);

			QVector4D worldEndTop =    QVector4D(endX, 2, endY, 1);
			QVector4D worldEndDown =   QVector4D(endX, -2, endY, 1);

			QVector4D screenStartTop = projectionMatrix * rotation * translation * worldStartTop;
			screenStartTop /= abs(screenStartTop.z());
			QVector4D screenStartDown = projectionMatrix * rotation * translation * worldStartDown;
			screenStartDown /= abs(screenStartDown.z());

			QVector4D screenEndTop = projectionMatrix * rotation * translation * worldEndTop;
			screenEndTop /= abs(screenEndTop.z());
			QVector4D screenEndDown = projectionMatrix * rotation * translation * worldEndDown;
			screenEndDown /= abs(screenEndDown.z());

			glColor3f(edges[i]->color[0], edges[i]->color[1], edges[i]->color[2]);

			glVertex2f(screenStartTop.x() , screenStartTop.y());
			glVertex2f(screenEndTop.x(), screenEndTop.y());
			glVertex2f(screenEndDown.x(), screenEndDown.y());
			glVertex2f(screenStartDown.x(), screenStartDown.y());
		}
	}
	glEnd();

	/*若有興趣的話, 可以為地板或迷宮上貼圖, 此項目不影響評分*/
	glBindTexture(GL_TEXTURE_2D, sky_ID);
	
	// 畫貼圖 & 算 UV
	
	glDisable(GL_TEXTURE_2D);
}
void OpenGLWidget::loadTexture2D(QString str,GLuint &textureID)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
	QImage img(str);
	QImage opengl_grass = QGLWidget::convertToGLFormat(img);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, opengl_grass.width(), opengl_grass.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_grass.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_2D);
}
float OpenGLWidget::degree_change(float num)
{
	return num /180.0f * M_PI;
}