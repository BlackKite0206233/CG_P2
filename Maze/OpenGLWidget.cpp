#include "OpenGLWidget.h"
#include <iostream>
#include "MazeWidget.h"
#include <gl\gl.h>
#include <gl\GLU.h>


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
	loadTexture2D(pic_path + "grass.png", grass_ID);
	loadTexture2D(pic_path + "sky.png", sky_ID);
}
void OpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(MazeWidget::maze!=NULL)
	{
		MazeWidget::maze->grass_ID = grass_ID;
		MazeWidget::maze->sky_ID = sky_ID;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//float maxLength = std::max(MazeWidget::maze->max_xp, MazeWidget::maze->max_yp);
		
		//View 2
		
		Map_3D();

		//View 1
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(MazeWidget::w / 2.0 - MazeWidget::w / 4.0, MazeWidget::h / 2.0 - MazeWidget::w * MazeWidget::maze->max_yp / MazeWidget::maze->max_xp / 4.0, MazeWidget::w / 2.0, MazeWidget::w * MazeWidget::maze->max_yp / MazeWidget::maze->max_xp / 2.0);
		//glViewport(MazeWidget::w / 2 + 10, 0, MazeWidget::w / 2 - 10, MazeWidget::h - 10);
		glOrtho(-0.1, MazeWidget::maze->max_xp + 0.1, -0.1, MazeWidget::maze->max_yp + 0.1, 0, 10);
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
	if (MazeWidget::showMap) {
		glBegin(GL_POLYGON);
		glColor4f(0, 0, 0, 0.6);
		glVertex2d(-0.1, -0.1);
		glVertex2d(-0.1, MazeWidget::maze->max_yp + 0.1);
		glVertex2d(MazeWidget::maze->max_xp + 0.1, MazeWidget::maze->max_yp + 0.1);
		glVertex2d(MazeWidget::maze->max_xp + 0.1, -0.1);
		glEnd();
		glLineWidth(1);
		glBegin(GL_LINES);

		float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
		float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];

		for(int i = 0 ; i < (int)MazeWidget::maze->num_edges; i++)
		{
			float edgeStartX = MazeWidget::maze->edges[i]->edgeBoundary[0].x();
			float edgeStartY = MazeWidget::maze->edges[i]->edgeBoundary[0].y();
			float edgeEndX = MazeWidget::maze->edges[i]->edgeBoundary[1].x();
			float edgeEndY = MazeWidget::maze->edges[i]->edgeBoundary[1].y();

			glColor3f(MazeWidget::maze->edges[i]->color[0] , MazeWidget::maze->edges[i]->color[1], MazeWidget::maze->edges[i]->color[2]);
			if(MazeWidget::maze->edges[i]->opaque)
			{
				glVertex2f(edgeStartX, edgeStartY);
				glVertex2f(edgeEndX, edgeEndY);
			}
		}

		//draw frustum
		float len = 1.1;
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
	float fov = MazeWidget::maze->viewer_fov;
	float aspectRatio = MazeWidget::maze->viewer_aspect;
	float fovVertical = MazeWidget::maze->viewer_fov_vertical;
	
	float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
	float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
	float viewerPosZ = MazeWidget::maze->viewer_posn[Maze::Z];
	float viewerDir  = MazeWidget::maze->viewer_dir;
	float viewerDirVertical = -MazeWidget::maze->viewer_dir_vertical; 
	float viewerHeight = MazeWidget::maze->viewer_height;
	double viewerHeadRotation = MazeWidget::maze->headRotation;

	Vec4D headIdentity(0, 0, 0.3, 1);
	Mat4D mat;
	Vec4D tmp;
	
	mat.SetToIdentity();
	mat.Rotate(viewerHeadRotation, 1, 0, 0);
	mat.Rotate(viewerDir, 0, 0, 1);
	mat.Rotate(viewerDirVertical, 0, 1, 0);
	tmp = mat * headIdentity;
	tmp /= tmp.w();
	Vec3D headOffset = Vec3D(tmp);

	Vec3D viewerPos(viewerPosX + headOffset.x(), viewerPosY + headOffset.y(), viewerPosZ + viewerHeight + headOffset.z());
	
	MazeWidget::maze->viewMatrix.SetToIdentity();
	MazeWidget::maze->viewMatrix.Rotate(-viewerDirVertical, 1, 0, 0);
	MazeWidget::maze->viewMatrix.Rotate(-viewerHeadRotation, 0, 0, 1);
	MazeWidget::maze->viewMatrix.Rotate(-viewerDir, 0, 1, 0);
	MazeWidget::maze->viewMatrix.Translate(-viewerPos.y(), -viewerPos.z(), -viewerPos.x());

	MazeWidget::maze->projectionMatrix.SetToIdentity();
	MazeWidget::maze->projectionMatrix.Perspective(fov, aspectRatio, 0.001, 100);


	Vec4D identity(1, 0, 0, 1);

	mat.SetToIdentity();
	mat.Translate(viewerPos.x(), viewerPos.y(), viewerPos.z());
	mat.Rotate(viewerHeadRotation, 1, 0, 0);
	mat.Rotate(viewerDir + fov / 2, 0, 0, 1);
	mat.Rotate(viewerDirVertical + fovVertical / 2, 0, 1, 0);
	tmp = mat * identity;
	tmp /= tmp.w();
	Vec3D leftTop = Vec3D(tmp);

	mat.SetToIdentity();
	mat.Translate(viewerPos.x(), viewerPos.y(), viewerPos.z());
	mat.Rotate(viewerHeadRotation, 1, 0, 0);
	mat.Rotate(viewerDir + fov / 2, 0, 0, 1);
	mat.Rotate(viewerDirVertical - fovVertical / 2, 0, 1, 0);
	tmp = mat * identity;
	tmp /= tmp.w();
	Vec3D leftBottom = Vec3D(tmp);

	mat.SetToIdentity();
	mat.Translate(viewerPos.x(), viewerPos.y(), viewerPos.z());
	mat.Rotate(viewerHeadRotation, 1, 0, 0);
	mat.Rotate(viewerDir - fov / 2, 0, 0, 1);
	mat.Rotate(viewerDirVertical + fovVertical / 2, 0, 1, 0);
	tmp = mat * identity;
	tmp /= tmp.w();
	Vec3D rightTop = Vec3D(tmp);

	mat.SetToIdentity();
	mat.Translate(viewerPos.x(), viewerPos.y(), viewerPos.z());
	mat.Rotate(viewerHeadRotation, 1, 0, 0);
	mat.Rotate(viewerDir - fov / 2, 0, 0, 1);
	mat.Rotate(viewerDirVertical - fovVertical / 2, 0, 1, 0);
	tmp = mat * identity;
	tmp /= tmp.w();
	Vec3D rightBottom = Vec3D(tmp);

	vector<Vec3D> boundary = vector<Vec3D>({ leftTop, rightTop, rightBottom, leftBottom });

	Vec3D center;
	for (auto& p : boundary) {
		center += p;
	}
	center /= boundary.size();

	identity = Vec4D(0, 0, 1, 1);
	mat.SetToIdentity();
	mat.Rotate(viewerHeadRotation, 1, 0, 0);
	mat.Rotate(viewerDir, 0, 0, 1);
	mat.Rotate(viewerDirVertical, 0, 1, 0);
	tmp = mat * identity;
	tmp /= tmp.w();
	Vec3D up = Vec3D(tmp);

	/*glBindTexture(GL_TEXTURE_2D, MazeWidget::maze->sky_ID);
	glBindTexture(GL_TEXTURE_2D, MazeWidget::maze->grass_ID);*/

	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, MazeWidget::w, MazeWidget::h);

	gluPerspective(fov, aspectRatio, 0.001, 100);

	headOffset.Normalize();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(viewerPos.y(), viewerPos.z(), viewerPos.x(), center.y(), center.z(), center.x(), 0, 1, 0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, MazeWidget::maze->grass_ID);
	glBegin(GL_POLYGON);

	glTexCoord2f(0, 0); glVertex3f(MazeWidget::maze->min_yp, 0, MazeWidget::maze->min_xp);
	glTexCoord2f(0, 1); glVertex3f(MazeWidget::maze->min_yp, 0, MazeWidget::maze->max_xp);
	glTexCoord2f(1, 1); glVertex3f(MazeWidget::maze->max_yp, 0, MazeWidget::maze->max_xp);
	glTexCoord2f(1, 0); glVertex3f(MazeWidget::maze->max_yp, 0, MazeWidget::maze->min_xp);
	
	glEnd();
	glDisable(GL_TEXTURE_2D);*/


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, MazeWidget::w, MazeWidget::h);
	//glViewport(0, 0, MazeWidget::w / 2, MazeWidget::h);
#ifdef DEBUG
	glOrtho(-0.1, MazeWidget::maze->max_xp * 16.0 / 9.0 + 0.1, -0.1, MazeWidget::maze->max_yp + 0.1, 0, 10);
#else
	glOrtho(-1, 1, -1.0 * 9.0 / 16.0 / 2.0, 1.0 * 9.0 / 16.0 / 2.0, -100, 100);
#endif

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/*ClipData skyBox = ClipData(MazeWidget::maze->skyBox, vector<vector<Vec3D>>({ boundary }), -1);
	Cell::Draw(0, skyBox, viewerPos, nullptr);*/

	MazeWidget::maze->Find_View_Cell(viewerPos, MazeWidget::maze->view_cell);

	ClipData clipData = ClipData(MazeWidget::maze->view_cell, vector<vector<Vec3D>>({ boundary }), -1);
	Cell::Draw(MazeWidget::maze->view_cell->index, clipData, viewerPos, nullptr);
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