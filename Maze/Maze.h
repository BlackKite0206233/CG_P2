/************************************************************************
     File:        Maze.h

     Author:     
                  Stephen Chenney, schenney@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu

     Comment:    
						(c) 2001-2002 Stephen Chenney, University of Wisconsin at Madison

						Class header file for Maze class. Manages the maze.
		

     Platform:    Visio Studio.Net 2003 (converted to 2005)

*************************************************************************/

#ifndef _MAZE_H_
#define _MAZE_H_
#define M_PI 3.1415926

#include "Cell.h"
#include "Vec3D.h"
#include "Mat4D.h"

//************************************************************************
//
// * A class for exceptions. Used by the constructor to pass file I/O errors
//   back.
//
//************************************************************************
class MazeException {
	private:
		char *message;

  public:
		MazeException(const char *m = "");
		~MazeException() { delete message; };

		// Return the error message string associated with the exception.
		const char* Message(void) { return message; };
};


//************************************************************************
//
// * The maze consists of cells, separated by edges. NOTE: The maze is defined
//   assuming that z is up, with xy forming the ground plane. This is different
//   to the OpenGL viewing assumption (which has y up and xz in the ground
//   plane). You will have to take this into account when drawing the view.
//   Also, assume that the floor of the maze is at z = -1, and the ceiling is
//   at z = 1.
//
//************************************************************************
class Maze {

	public:
		// The second constructor takes a maze file name to load. It may throw
		// exceptions of the MazeException class if there is an error.
		Maze(const char *f);

		~Maze(void);

	public:
		// Set the viewer's location 
		//void Set_View_Posn(float x, float y, float z);

		// Set the angle in which the viewer is looking.
		//void Set_View_Dir(const float);

		// Set the horizontal field of view.
		//void Set_View_FOV(const float);


		// Save the maze to a file of the given name.
		bool Save(const char*);

		// Functions to convert between degrees and radians.
		static double To_Radians(double deg) { return deg / 180.0 * M_PI; };
		static double To_Degrees(double rad) { return rad * 180.0 / M_PI; };

		void Find_View_Cell(Cell*);

	private:
		void Set_Extents(void);

	private:
		
		

		static const float BUFFER;	// The viewer must be at least this far inside
												// an exterior wall of the maze.
												// Not implemented
	public:
		static const char X; // Used to index into the viewer's position
		static const char Y;
		static const char Z;

		unsigned int frame_num;	// The frame number we are currently drawing.
											// It isn't necessary, but you might find it
											// helpful for debugging or something.

		int		num_vertices;	// The number of vertices in the maze
		Vertex	**vertices;		// An array of pointers to the vertices.

		int	   num_edges;		// The number of edges in the maze.
		Edge** edges;			// An array of pointers to the edges.
		Edge** aboveEdge;
		Edge** floorEdge;
		Edge** roundEdge;
		Edge** ceilingEdge;

		int    num_cells;     // The number of cells in the maze
		Cell** cells;       // An array of pointers to the cells.
		Cell** aboveCell;

		float viewer_posn[3];	// The x,y location of the viewer.
		float viewer_dir;			// The direction in which the viewer is
											// looking. Measured in degrees about the z
											// axis, in the usual way.
		float viewer_fov;			// The horizontal field of view, in degrees.
		float viewer_dir_vertical;
		float viewer_aspect;
		float viewer_fov_vertical;
		float viewer_height;

		float min_xp;	// The minimum x location of any vertex in the maze.
		float min_yp;	// The minimum y location of any vertex in the maze.
		float max_xp;	// The maximum x location of any vertex in the maze.
		float max_yp;	// The maximum y location of any vertex in the maze.

		Cell* view_cell;// The cell that currently contains the view
										  // point. You will need to use this.

		Mat4D viewMatrix;
		Mat4D projectionMatrix;

};


#endif

