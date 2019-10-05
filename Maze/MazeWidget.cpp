#include "MazeWidget.h"
#include <iostream>

#define LOOK_UP_LIMIT 35

Maze *MazeWidget::maze;					//Static 一定有一個實體，不然會出現link error
bool MazeWidget::showMap = false;

MazeWidget::MazeWidget(QWidget *parent): QMainWindow(parent)
{
	ui.setupUi(this);
	ui.centralWidget->setFixedSize(w,h);
	timer = new QTimer(this);
	//ui.centralWidget->set
	connect(ui.actionRead_Maze,		SIGNAL(triggered()),	this,	SLOT(Read_Maze()));
	connect(ui.actionExit,			SIGNAL(triggered()),	this,	SLOT(Exit()));
	connect(timer,					SIGNAL(timeout()),		this,	SLOT(Refrush_Widget()));

	move_speed = 0.1f;
	move_FB = move_LR = move_Dir = move_DirUD = 0;
	mouseSensitivity = 0.2f;
	lockCursor = false;
}
//讀檔案進來
void MazeWidget::Read_Maze()
{
	QDir dir("Maze Map");
	QString temp;
	if(dir.exists())
		temp = "Maze Map/";
	else
		temp = "../x64/Release/Maze Map/";
	filename = QFileDialog::getOpenFileName(this,tr("Read Maze"),temp,tr("Any File (*.*)"));
	maze = new Maze(filename.toStdString().c_str());
	ui.widget->updateGL();
	timer->start(20);
	//maze->ReadFile_byName(filename);
}
void MazeWidget::Refrush_Widget()
{
	if(maze != NULL)
	{
		mouseMove();
		float angle_0 = maze->viewer_dir / 180 * 3.14;
		float angle_90 = (maze->viewer_dir - 90) / 180 * 3.14;
		float inX = move_FB * cos(angle_0) + move_LR * cos(angle_90);
		float inY = move_FB * sin(angle_0) + move_LR * sin(angle_90);
		if(inX != 0 || inY != 0)
			CollisionDetection(inX,inY);
		maze->viewer_dir += 5 * move_Dir;
		maze->viewer_dir_vertical += move_DirUD;
		ui.widget->updateGL();
		forceDown += gravity;
		//maze->viewer_posn[Maze::Z] = 1;
		maze->viewer_posn[Maze::Z] -= (forceDown - forceUp);
		if (maze->viewer_posn[Maze::Z] <= 0) {
			maze->viewer_posn[Maze::Z] = 0;
			forceDown = forceUp = 0;
			jump = false;
		}
	}
	else
		timer->stop();
}
void MazeWidget::CollisionDetection(float inx,float iny)
{
	int big_X = maze->edges[maze->num_edges - 1]->edgeBoundary[1].x();
	int big_Y = maze->edges[maze->num_edges - 1]->edgeBoundary[1].y();
	float const_dis = 0.01f;									//撞牆前面的誤差值
	bool collision_bool_X = false;
	bool collision_bool_Y = false;
	for (int i = 0 ; i < (int)maze->num_edges && (!collision_bool_X || !collision_bool_Y); i++)
		if (maze->edges[i]->opaque) {

			float edgeStartX = maze->edges[i]->edgeBoundary[0].x();
			float edgeStartY = maze->edges[i]->edgeBoundary[0].y();
			float edgeEndX = maze->edges[i]->edgeBoundary[1].x();
			float edgeEndY = maze->edges[i]->edgeBoundary[1].y();
			float viewerPosX = maze->viewer_posn[Maze::X];
			float viewerPosY = maze->viewer_posn[Maze::Y];

			switch (Check_Same_X_or_Y(edgeStartX, edgeStartY, edgeEndX, edgeEndY, big_X, big_Y))
			{
			case 1:
				if (((viewerPosX < edgeStartX + const_dis && edgeStartX - const_dis < viewerPosX + inx) || (viewerPosX + inx < edgeStartX + const_dis && edgeStartX - const_dis < viewerPosX))
					&& edgeStartY <= viewerPosY && edgeEndY >= viewerPosY)
					collision_bool_X = true;
				break;
			case 2:
				if (((viewerPosY < edgeStartY + const_dis && edgeStartY - const_dis < viewerPosY + iny) || (viewerPosY + iny  < edgeStartY + const_dis && edgeStartY - const_dis < viewerPosY))
					&& edgeStartX <= viewerPosX && edgeEndX >= viewerPosX)
					collision_bool_Y = true;
				break;
			}
		}
			
	if(!collision_bool_X)
		maze->viewer_posn[Maze::X] += inx;
	if(!collision_bool_Y)
		maze->viewer_posn[Maze::Y] += iny;
}
int MazeWidget::Check_Same_X_or_Y(int maze_X1,int maze_Y1,int maze_X2,int maze_Y2,int big_X,int big_Y)
{
	//不考慮斜的邊
	if (maze_X1 == maze_X2)
		return 1;
		/*
		if(maze_X1 == 0 || maze_X1 == big_X)
			return 0;
		else
			return 1;*/
	else if(maze_Y1 == maze_Y2)
		return 2;
		/*
		if(maze_Y1 == 0 || maze_Y1 == big_Y)
			return 0;
		else
			return 2;*/
	else 
		return 3;
}
void MazeWidget::Exit()
{
	QApplication::exit();
}

//Keyboard Down
void MazeWidget::keyPressEvent(QKeyEvent *event)
{
	if(maze != NULL)
		switch (event->key())
		{
			//Forward Backward
			case (Qt::Key_W):
				move_FB = move_speed;
				break;
			case (Qt::Key_S):
				move_FB = -move_speed;
				break;

			//Lift Right
			case (Qt::Key_A):
				move_LR = -move_speed;
				break;
			case (Qt::Key_D):
				move_LR = move_speed;
				break;

			case (Qt::Key_Left):
				move_Dir = move_speed;
				break;
			case (Qt::Key_Right):
				move_Dir = -move_speed;
				break;

			case (Qt::Key_Shift):
				showMap = true;
				break;

			case (Qt::Key_Space):
				if (!jump) {
					jump = true;
					forceUp = 0.5;
				}
				break;
		}
}
void MazeWidget::keyReleaseEvent(QKeyEvent *event)
{
	if(maze != NULL)
		switch (event->key())
		{
			case (Qt::Key_W):
			case (Qt::Key_S):
				move_FB = 0;
				break;

			case (Qt::Key_A):
			case (Qt::Key_D):
				move_LR = 0;
				break;

			case (Qt::Key_Left):
			case (Qt::Key_Right):
				move_Dir = 0;
				break;

			case (Qt::Key_Shift):
				showMap = false;
				break;

			case(Qt::Key_Escape):
				lockCursor = !lockCursor;
				ShowCursor(!lockCursor);
				break;
		}
}

void MazeWidget::mouseMove() {

	QPoint cusorPos = mapFromGlobal(QCursor::pos());

	int x = cusorPos.x();
	int y = cusorPos.y();

	double unit = 720.0 / MazeWidget::w;
	double unitUD = 180.0 / MazeWidget::h;

	y = (y < 0) ? (0) : (y > MazeWidget::h ? MazeWidget::h : y);

	float dirX = unit * (MazeWidget::w * 0.5 - x);
	float dirY = unitUD * (MazeWidget::h / 2 - y);


	maze->viewer_dir += (lockCursor ? dirX : (dirX - offsetDirX)) * mouseSensitivity;
	maze->viewer_dir_vertical += (lockCursor ? dirY : (dirY - offsetDirY)) * mouseSensitivity;

	offsetDirX = dirX;
	offsetDirY = dirY;

	maze->viewer_dir_vertical = (maze->viewer_dir_vertical < -LOOK_UP_LIMIT) ? (-LOOK_UP_LIMIT) : (maze->viewer_dir_vertical > LOOK_UP_LIMIT ? LOOK_UP_LIMIT : maze->viewer_dir_vertical);

	if (lockCursor) {
		QPoint zeroPos = mapToGlobal(QPoint(MazeWidget::w / 2, MazeWidget::h / 2));
		SetCursorPos(zeroPos.x(), zeroPos.y());
	}
}