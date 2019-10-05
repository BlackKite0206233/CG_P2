/************************************************************************
     File:        Maze.cpp

     Author:     
                  Stephen Chenney, schenney@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu

     Comment:    
						(c) 2001-2002 Stephen Chenney, University of Wisconsin at Madison

						Class header file for Maze class. Manages the maze.
		

     Platform:    Visio Studio.Net 2003 (converted to 2005)

*************************************************************************/

#include "Maze.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

const char Maze::X = 0;
const char Maze::Y = 1;
const char Maze::Z = 2;

const float Maze::BUFFER = 0.1f;

//**********************************************************************
//
// * Constructor for the maze exception
//======================================================================
MazeException::
MazeException(const char *m)
//======================================================================
{
	message = new char[strlen(m) + 4];
	strcpy(message, m);
}

//**********************************************************************
//
// * Construtor to read in precreated maze
//======================================================================
Maze::
Maze(const char *filename)
//======================================================================
{
	char    err_string[128];
	FILE    *f;
	int	    i;

	// Open the file
	if ( ! ( f = fopen(filename, "r") ) )
		throw new MazeException("Maze: Couldn't open file");
	
	// Get the total number of vertices
	if ( fscanf(f, "%d", &num_vertices) != 1 )
		throw new MazeException("Maze: Couldn't read number of vertices");

	// Read in each vertices
	vertices = new Vertex*[num_vertices];
	for ( i = 0 ; i < num_vertices ; i++ ) {
		float x, y;
		if ( fscanf(f, "%g %g", &x, &y) != 2 )	{
			sprintf(err_string, "Maze: Couldn't read vertex number %d", i);
			throw new MazeException(err_string);
		}
		vertices[i] = new Vertex(i, x, y);
	}
	
	// Get the number of edges
	if ( fscanf(f, "%d", &num_edges) != 1 )
		throw new MazeException("Maze: Couldn't read number of edges");

	// read in all edges
	edges = new Edge*[num_edges];
	aboveEdge = new Edge*[num_edges];
	for ( i = 0 ; i < num_edges ; i++ ){
		int     vs, ve, cl, cr, o;
		float	r, g, b;
		if ( fscanf(f, "%d %d %d %d %d %g %g %g",
						&vs, &ve, &cl, &cr, &o, &r, &g, &b) != 8) {
			sprintf(err_string, "Maze: Couldn't read edge number %d", i);
			throw new MazeException(err_string);
		}
		Vec3D leftTop(vertices[vs]->posn[0], vertices[vs]->posn[1], Edge::wallHeight);
		Vec3D rightTop(vertices[ve]->posn[0], vertices[ve]->posn[1], Edge::wallHeight);
		Vec3D leftBottom(vertices[vs]->posn[0], vertices[vs]->posn[1], 0);
		Vec3D rightBottom(vertices[ve]->posn[0], vertices[ve]->posn[1], 0);
		edges[i] = new Edge(i, leftTop, rightTop, leftBottom, rightBottom, r, g, b, o ? true : false);
		edges[i]->Add_Cell((Cell*)cl, Edge::LEFT);
		edges[i]->Add_Cell((Cell*)cr, Edge::RIGHT);
		edges[i]->endpointId[0] = vertices[vs]->index;
		edges[i]->endpointId[1] = vertices[ve]->index;

		leftTop[2] += Edge::wallHeight;
		rightTop[2] += Edge::wallHeight;
		leftBottom[2] += Edge::wallHeight;
		rightBottom[2] += Edge::wallHeight;
		aboveEdge[i] = new Edge(i, leftTop, rightTop, leftBottom, rightBottom, r, g, b, false);
		aboveEdge[i]->Add_Cell((Cell*)cl, Edge::LEFT);
		aboveEdge[i]->Add_Cell((Cell*)cr, Edge::RIGHT);
	}

	// Read in the number of cells
	if ( fscanf(f, "%d", &num_cells) != 1 )
		throw new MazeException("Maze: Couldn't read number of cells");

	floorEdge = new Edge*[num_cells];
	roundEdge = new Edge*[num_cells];
	ceilingEdge = new Edge*[num_cells];

	// Read in all cells
	cells = new Cell*[num_cells];
	aboveCell = new Cell*[num_cells];
	for ( i = 0 ; i < num_cells ; i++ )	{
		int epx, epy, emx, emy;
		if ( fscanf(f, "%d %d %d %d", &epx, &epy, &emx, &emy) != 4 ){
			sprintf(err_string, "Maze: Couldn't read cell number %d", i);
			throw new MazeException(err_string);
		}
		floorEdge[i] = new Edge(i, edges[epx]->edgeBoundary[2], edges[epx]->edgeBoundary[3], edges[emx]->edgeBoundary[2], edges[emx]->edgeBoundary[3], 0, 0, 0, false);
		floorEdge[i]->Add_Cell((Cell*)0, Edge::LEFT);
		floorEdge[i]->Add_Cell((Cell*)0, Edge::RIGHT);
		roundEdge[i] = new Edge(i, edges[epx]->edgeBoundary[0], edges[epx]->edgeBoundary[1], edges[emx]->edgeBoundary[0], edges[emx]->edgeBoundary[1], 0, 0, 0, false);
		roundEdge[i]->Add_Cell((Cell*)0, Edge::LEFT);
		roundEdge[i]->Add_Cell((Cell*)0, Edge::RIGHT);
		ceilingEdge[i] = new Edge(i, aboveEdge[epx]->edgeBoundary[2], aboveEdge[epx]->edgeBoundary[3], aboveEdge[emx]->edgeBoundary[2], aboveEdge[emx]->edgeBoundary[3], 0, 0, 0, false);
		ceilingEdge[i]->Add_Cell((Cell*)0, Edge::LEFT);
		ceilingEdge[i]->Add_Cell((Cell*)0, Edge::RIGHT);

		cells[i] = new Cell(i, epx >= 0 ? edges[epx] : NULL,
			epy >= 0 ? edges[epy] : NULL,
			emx >= 0 ? edges[emx] : NULL,
			emy >= 0 ? edges[emy] : NULL,
			floorEdge[i], roundEdge[i]);
		aboveCell[i] = new Cell(i, epx >= 0 ? aboveEdge[epx] : NULL,
			epy >= 0 ? aboveEdge[epy] : NULL,
			emx >= 0 ? aboveEdge[emx] : NULL,
			emy >= 0 ? aboveEdge[emy] : NULL,
			roundEdge[i], ceilingEdge[i]);

		for (int j = 0; j < 4; j++) {
			if (cells[i]->edges[j]) {
				if (cells[i]->edges[j]->neighbors[0] == (Cell*)i)
					cells[i]->edges[j]->neighbors[0] = cells[i];
				else if (cells[i]->edges[j]->neighbors[1] == (Cell*)i)
					cells[i]->edges[j]->neighbors[1] = cells[i];
				else {
					sprintf(err_string,
						"Maze: Cell %d not one of edge %d's neighbors",
						i, cells[i]->edges[j]->index);
					throw new MazeException(err_string);
				}
			}
			if (aboveCell[i]->edges[j]) {
				if (aboveCell[i]->edges[j]->neighbors[0] == (Cell*)i) {
					aboveCell[i]->edges[j]->neighbors[0] = aboveCell[i];
				}
				else if (aboveCell[i]->edges[j]->neighbors[1] == (Cell*)i) {
					aboveCell[i]->edges[j]->neighbors[1] = aboveCell[i];
				}
			}
		}
		cells[i]->edges[4]->neighbors[1] = cells[i];
		cells[i]->edges[5]->neighbors[0] = cells[i];
		aboveCell[i]->edges[4]->neighbors[1] = aboveCell[i];
		aboveCell[i]->edges[5]->neighbors[0] = aboveCell[i];
	}

	if ( fscanf(f, "%g %g %g %g %g",
					 &(viewer_posn[X]), &(viewer_posn[Y]), &(viewer_posn[Z]),
					 &(viewer_dir), &(viewer_fov)) != 5 )
		throw new MazeException("Maze: Error reading view information.");

	// Some edges have no neighbor on one side, so be sure to set their
	// pointers to NULL. (They were set at -1 by the save/load process.)
	for ( i = 0 ; i < num_edges ; i++ )	{
		if ( edges[i]->neighbors[0] == (Cell*)-1 )
			edges[i]->neighbors[0] = NULL;
		if ( edges[i]->neighbors[1] == (Cell*)-1 )
			edges[i]->neighbors[1] = NULL;
		if (aboveEdge[i]->neighbors[0] == (Cell*)-1)
			aboveEdge[i]->neighbors[0] = NULL;
		if (aboveEdge[i]->neighbors[1] == (Cell*)-1)
			aboveEdge[i]->neighbors[1] = NULL;
	}

	fclose(f);

	Set_Extents();

	// Figure out which cell the viewer is in, starting off by guessing the
	// 0th cell.
	viewer_dir = 0;
	viewer_dir_vertical = 0;
	viewer_aspect = 9.0 / 16.0;
	viewer_height = 1.2;
	viewer_fov_vertical = 2 * atan2(sin(viewer_fov / 2 * M_PI / 180) * 10, cos(viewer_fov / 2 * M_PI / 180)) * 180 / M_PI;
	frame_num = 0;
	Find_View_Cell(cells[0]);
}


//**********************************************************************
//
// * Destructor must free all the memory allocated.
//======================================================================
Maze::
~Maze(void)
//======================================================================
{
	int i;

	for ( i = 0 ; i < num_vertices ; i++ )
		delete vertices[i];
	delete[] vertices;

	for ( i = 0 ; i < num_edges ; i++ )
		delete edges[i];
	delete[] edges;

	for ( i = 0 ; i < num_cells ; i++ )
		delete cells[i];
	delete[] cells;
}

//**********************************************************************
//
// * Add edges from cell to the set that are available for removal to
//   grow the maze.
//======================================================================
static void
Add_To_Available(Cell *cell, int *available, int &num_available)
//======================================================================
{
	int i, j;

	// Add edges from cell to the set that are available for removal to
	// grow the maze.

	for ( i = 0 ; i < 4 ; i++ ){
		Cell    *neighbor = cell->edges[i]->Neighbor(cell);

		if ( neighbor && ! neighbor->counter )	{
			int candidate = cell->edges[i]->index;
			for ( j = 0 ; j < num_available ; j++ )
				if ( candidate == available[j] ) {
					printf("Breaking early\n");
					break;
			}
			if ( j == num_available )  {
				available[num_available] = candidate;
				num_available++;
			}
		}
	}

	cell->counter = 1;
}


//**********************************************************************
//
// * Go through all the vertices looking for the minimum and maximum
//   extents of the maze.
//======================================================================
void Maze::
Set_Extents(void)
//======================================================================
{
	int i;

	min_xp = vertices[0]->posn[Vertex::X];
	max_xp = vertices[0]->posn[Vertex::X];
	min_yp = vertices[0]->posn[Vertex::Y];
	max_yp = vertices[0]->posn[Vertex::Y];
	for ( i = 1 ; i < num_vertices ; i++ ) {
		if ( vertices[i]->posn[Vertex::X] > max_xp )
			 max_xp = vertices[i]->posn[Vertex::X];
		if ( vertices[i]->posn[Vertex::X] < min_xp )
			 min_xp = vertices[i]->posn[Vertex::X];
		if ( vertices[i]->posn[Vertex::Y] > max_yp )
			 max_yp = vertices[i]->posn[Vertex::Y];
		if ( vertices[i]->posn[Vertex::Y] < min_yp )
			 min_yp = vertices[i]->posn[Vertex::Y];
    }
}


//**********************************************************************
//
// * Figure out which cell the view is in, using seed_cell as an
//   initial guess. This procedure works by repeatedly checking
//   whether the viewpoint is in the current cell. If it is, we're
//   done. If not, Point_In_Cell returns in new_cell the next cell
//   to test. The new cell is the one on the other side of an edge
//   that the point is "outside" (meaning that it might be inside the
//   new cell).
//======================================================================
void Maze::
Find_View_Cell(Cell *seed_cell)
//======================================================================
{
	Cell    *new_cell;

	// 
	while ( ! ( seed_cell->Point_In_Cell(viewer_posn[X], viewer_posn[Y],
													 viewer_posn[Z] + viewer_height, new_cell) ) ) {
		if ( new_cell == 0 ) {
			// The viewer is outside the top or bottom of the maze.
			throw new MazeException("Maze: View not in maze\n");
		}

		seed_cell = new_cell;
    }
    
    view_cell = seed_cell;
}

//**********************************************************************
//
// * Save the maze to a file of the given name.
//======================================================================
bool Maze::
Save(const char *filename)
//======================================================================
{
	FILE    *f = fopen(filename, "w");
	int	    i;

	// Dump everything to a file of the given name. Returns false if it
	// couldn't open the file. True otherwise.

	if ( ! f )  {
		return false;
   }

	fprintf(f, "%d\n", num_vertices);
	for ( i = 0 ; i < num_vertices ; i++ )
		fprintf(f, "%g %g\n", vertices[i]->posn[Vertex::X],
			      vertices[i]->posn[Vertex::Y]);

		fprintf(f, "%d\n", num_edges);
	for ( i = 0 ; i < num_edges ; i++ )
	fprintf(f, "%d %d %d %d %d %g %g %g\n",
				edges[i]->endpointId[0],
				edges[i]->endpointId[1],
				edges[i]->neighbors[Edge::LEFT] ?
				edges[i]->neighbors[Edge::LEFT]->index : -1,
				edges[i]->neighbors[Edge::RIGHT] ?
				edges[i]->neighbors[Edge::RIGHT]->index : -1,
				edges[i]->opaque ? 1 : 0,
				edges[i]->color[0], edges[i]->color[1], edges[i]->color[2]);

	fprintf(f, "%d\n", num_cells);
	for ( i = 0 ; i < num_cells ; i++ )
		fprintf(f, "%d %d %d %d\n",
					cells[i]->edges[0] ? cells[i]->edges[0]->index : -1,
					cells[i]->edges[1] ? cells[i]->edges[1]->index : -1,
					cells[i]->edges[2] ? cells[i]->edges[2]->index : -1,
					cells[i]->edges[3] ? cells[i]->edges[3]->index : -1);

	   fprintf(f, "%g %g %g %g %g\n",
					viewer_posn[X], viewer_posn[Y], viewer_posn[Z],
					viewer_dir, viewer_fov);

	fclose(f);

	return true;
}
