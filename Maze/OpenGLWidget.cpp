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
		glEnable(GL_BLEND);
		float maxLength = std::max(MazeWidget::maze->max_xp, MazeWidget::maze->max_yp);
		
		//View 2
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//glViewport(0, 0, MazeWidget::w, MazeWidget::h);
		glViewport(0, 0, MazeWidget::w / 2, MazeWidget::h);
#ifdef DEBUG
		glOrtho(-0.1, maxLength + 0.1, -0.1, maxLength + 0.1, 0, 10);
#else
		glOrtho(-1, 1, -1, 1, -100, 100);
#endif

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Map_3D();

		//View 1
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//glViewport(MazeWidget::w / 2 - 10, MazeWidget::h / 2 - 10, MazeWidget::w / 2 - 10, MazeWidget::h / 2 - 10);
		glViewport(MazeWidget::w / 2 + 10, 0, MazeWidget::w / 2 - 10, MazeWidget::h - 10);
		glOrtho(-0.1, maxLength + 0.1, -0.1, maxLength + 0.1, 0, 10);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Mini_Map();
		glDisable(GL_BLEND);
	}
}
void OpenGLWidget::resizeGL(int w,int h)
{
}

//Draw Left Part
void OpenGLWidget::Mini_Map()	
{				
	if (MazeWidget::showMap || true) {
		glBegin(GL_POLYGON);
		glColor4f(0, 0, 0, 0.6);
		glVertex2d(-0.1, -0.1);
		glVertex2d(-0.1, MazeWidget::maze->max_yp);
		glVertex2d(MazeWidget::maze->max_xp, MazeWidget::maze->max_yp);
		glVertex2d(MazeWidget::maze->max_xp, -0.1);
		glEnd();
		glBegin(GL_LINES);

		float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
		float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];

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
		float len = 100.1;
		glColor3f(1, 1, 1);
		glVertex2f(viewerPosX, viewerPosY);
		glVertex2f(viewerPosX + len * cos(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov/2)) ,
			viewerPosY + len * sin(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov/2)));

		glVertex2f(viewerPosX, viewerPosY);
		glVertex2f(viewerPosX + len * cos(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov/2)) ,
			viewerPosY + len *  sin(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov/2)));
		glEnd();
	}
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

void OpenGLWidget::Map_3D()
{
	MazeWidget::maze->frame_num += 1;
	// �e�k��϶����Ҧ��F��
	MazeWidget::maze->Find_View_Cell(MazeWidget::maze->view_cell);
	float fov = MazeWidget::maze->viewer_fov;
	float aspectRatio = MazeWidget::maze->viewer_aspect;
	float fovVertical = MazeWidget::maze->viewer_fov_vertical;
	
	float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
	float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
	float viewerPosZ = MazeWidget::maze->viewer_posn[Maze::Z];
	float viewerDir  = MazeWidget::maze->viewer_dir;
	float viewerDirVertical = -MazeWidget::maze->viewer_dir_vertical; 
	float viewerHeight = MazeWidget::maze->viewer_height;

	//std::cout << viewerDirVertical << "                 \r";

	MazeWidget::maze->viewMatrix.setToIdentity();
	MazeWidget::maze->viewMatrix.rotate(-viewerDirVertical, 1, 0, 0);
	MazeWidget::maze->viewMatrix.rotate(-viewerDir, 0, 1, 0);
	MazeWidget::maze->viewMatrix.translate(-viewerPosY, -viewerPosZ - viewerHeight, -viewerPosX);

	MazeWidget::maze->projectionMatrix.setToIdentity();
	MazeWidget::maze->projectionMatrix.perspective(fov, aspectRatio, 0.001, 100);

	QMatrix4x4 mat;

	QVector4D identity(1, 0, 0, 1);

	mat.setToIdentity();
	mat.translate(viewerPosX, viewerPosY, viewerPosZ + viewerHeight);
	mat.rotate(viewerDir + fov / 2, 0, 0, 1);
	mat.rotate(viewerDirVertical + fovVertical / 2, 0, 1, 0);
	QVector3D leftTop = QVector3D(mat * identity);

	mat.setToIdentity();
	mat.translate(viewerPosX, viewerPosY, viewerPosZ + viewerHeight);
	mat.rotate(viewerDir + fov / 2, 0, 0, 1);
	mat.rotate(viewerDirVertical - fovVertical / 2, 0, 1, 0);
	QVector3D leftBottom = QVector3D(mat * identity);

	mat.setToIdentity();
	mat.translate(viewerPosX, viewerPosY, viewerPosZ + viewerHeight);
	mat.rotate(viewerDir - fov / 2, 0, 0, 1);
	mat.rotate(viewerDirVertical + fovVertical / 2, 0, 1, 0);
	QVector3D rightTop = QVector3D(mat * identity);

	mat.setToIdentity();
	mat.translate(viewerPosX, viewerPosY, viewerPosZ + viewerHeight);
	mat.rotate(viewerDir - fov / 2, 0, 0, 1);
	mat.rotate(viewerDirVertical - fovVertical / 2, 0, 1, 0);
	QVector3D rightBottom = QVector3D(mat * identity);

	QVector3D viewerPos(viewerPosX, viewerPosY, viewerPosZ + viewerHeight);
	vector<QVector3D> boundary = vector<QVector3D>({ leftTop, rightTop, rightBottom, leftBottom });

	glBindTexture(GL_TEXTURE_2D, sky_ID);
	
	// �e�K�� & �� UV
	
	glDisable(GL_TEXTURE_2D);

	MazeWidget::maze->view_cell->Draw(viewerPos, boundary, 0);
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