
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include "Map.h"

// main.cpp:

Map* pMap;

void init()
{
	glClearColor(0.6, 0.8, 0.6, 0);// color of window background
	glOrtho(0, MAP_SIZE, 0, MAP_SIZE, -1, 1); // set the coordinates system

	pMap = new Map();
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	pMap->drawMap();

	glutSwapBuffers(); // show all
}

void idle() 
{
	glutPostRedisplay();
}


void main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(500, 0);
	glutCreateWindow("AI Battle");

	glutDisplayFunc(display);
	glutIdleFunc(idle);

	init();

	glutMainLoop();
}