//#define DEBUG

#ifndef MAZE_H
#define MAZE_H

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QKeyEvent>
#include <QTimer>
#include <QDir>
#include "ui_maze.h"
#include <Maze.h>


class MazeWidget : public QMainWindow
{
	Q_OBJECT
public:
	MazeWidget(QWidget *parent = 0);
	static Maze *maze;
	static const int w = 1600;
	static const int h = 900;
	static bool showMap;
private:
	Ui::MazeClass ui;

	QString filename;
	QTimer *timer;
	void keyPressEvent(QKeyEvent *);
	void keyReleaseEvent(QKeyEvent *);

	float offsetDirX;
	float offsetDirY;

	float mouseSensitivity;
	bool lockCursor;
	
	float move_speed;
	//LB 前面後面 LR左右
	double move_FB, move_LR, move_UD, move_Dir, move_DirUD;

	bool jump = false;
	double forceUp = 0;
	double forceDown = 0;
	double gravity = 0.045;
	
	bool headRotateRight = false;
	bool headRotateLeft = false;
	double rotationSpeed = 1;
private slots:
	void Read_Maze();
	void Refrush_Widget();

	void CollisionDetection(float,float);
	int Check_Same_X_or_Y(int,int,int,int,int,int);		//0代表是邊界，1代表同X，2代表同Y，3代表斜線
	void Exit();
	void mouseMove();
};

#endif // MAZE_H
