
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include "Map.h"
#include "GameController.h"

// main.cpp:

GameController* game;
int lastUpdateTime = 0;
const int FRAME_DELAY = 150;

void onTimer(int value)
{
	glutPostRedisplay();     // Request to render again
	glutTimerFunc(FRAME_DELAY, onTimer, 0); // The timing of the next frame
}

void init()
{
	glClearColor(0.6, 0.8, 0.6, 0);// color of window background
	glOrtho(0, MAP_SIZE, 0, MAP_SIZE, -1, 1); // set the coordinates system

	game = new GameController();
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	if(game)
		game->drawGame();

	glutSwapBuffers(); // show all
}

void idle() 
{
	
	int currentTime = glutGet(GLUT_ELAPSED_TIME); // milliseconds since start

	if (game && !game->isGameOver()) {
		if (currentTime - lastUpdateTime > FRAME_DELAY) {
			game->tick();
			lastUpdateTime = currentTime;
		}		
	}
	
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

	delete game;
}