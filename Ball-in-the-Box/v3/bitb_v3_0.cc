
/*
File name		:	BitB (Ball in the Box)
Author			:	Sreejith Sreekantan (krssreejith@gmail.com)
Version			:	3.0
Date			:	June 2012
*/
#ifdef __APPLE__
	#include <GLUT/glut.h>
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
#else	
	#include <GL/glut.h>
#endif
#include "DisplayText.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>

using namespace std;

#define NAME_OF_GAME "Ball in the Box!"

// Game states
#define GAME_INIT 0
#define GAME_START 1
#define GAME_END 2
#define GAME_CLOSE 3

#define SCORE_OFFSET 5

#define SCREEN_WIDTH_PIX 120
#define SCREEN_HEIGHT_PIX 80
#define DASHBOARD_WIDTH_PIX SCREEN_WIDTH_PIX
#define DASHBOARD_HEIGHT_PIX 10
#define BALL_RADIUS 1.5
#define BALLSPEED_OFFSET 0.05
#define BOARD_WIDTH 5.0
#define BOARD_LENGTH 20.0
#define BOARD_HEIGHT 3

#define PLAYER_NAME_LEN 11

int iGameStatus;
int iScore;
int iScoreTemp=0;

GLfloat fBallSpeed = 0.01;
GLfloat fBoardSpeed = 4.0;

GLfloat afBallSpecular[] = {0.5, 0.0, 0.0, 1.0};
GLfloat afBallAmbient[] = {0.5, 0.0, 0.0, 1.0};
GLfloat afBallShininess[] = {50.0};

GLfloat fBallTransX = SCREEN_WIDTH_PIX/2;
GLfloat fBallTransY = SCREEN_HEIGHT_PIX/2;
GLfloat fBallTransXOffset = fBallSpeed;
GLfloat fBallTransYOffset = fBallSpeed;

GLfloat fBoardTransX = 0.0;
GLfloat fBoardTransY = 0.0;
GLfloat fBoardTransXOffset = fBoardSpeed;

char acPlayerName[PLAYER_NAME_LEN];

GLuint iSphereGenList;
GLuint iBoardGenList;
GLuint iDashboardGenList;
GLuint iGetNameGenList;
GLuint iShowScore;

time_t startTime;

void fnDisplay() {

	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (iGameStatus == GAME_INIT){
		glPushMatrix();
			glCallList(iGetNameGenList);
			
			glColor3f(0.75, 0.0, 0.0);
			DisplayText::DisplayText(SCREEN_WIDTH_PIX/2-8, SCREEN_HEIGHT_PIX/2, GLUT_BITMAP_HELVETICA_18, acPlayerName);
		glPopMatrix();
	} 
	else if (iGameStatus == GAME_START) {
		
		iScore = ((time(NULL) - startTime)/SCORE_OFFSET);
		if(iScore > 0 && iScore % (SCORE_OFFSET*2) == 0 && iScore != iScoreTemp) {
			fBallTransX *= 1.025;
			fBallTransY *= 1.025;
			fBoardTransX *= 1.025;
			fBoardTransY *= 1.025;
			iScoreTemp = iScore;
			clog << "Speed incremented:" << fBallTransX << "," << fBallTransY << endl;
		}
		//clog << "iScore=" << iScore << endl;
		glPushMatrix();
			glTranslatef(fBallTransX, fBallTransY, 0.0);	
			glCallList(iSphereGenList);
		glPopMatrix();

		glPushMatrix();	
			glTranslatef(fBoardTransX, fBoardTransY, 0.0);	
			glCallList(iBoardGenList);
		glPopMatrix();
		
		glPushMatrix();
			glCallList(iDashboardGenList);
			glColor3f(1.0, 1.0, 1.0);
			DisplayText::DisplayText(10.0, SCREEN_HEIGHT_PIX+5, GLUT_BITMAP_HELVETICA_18, acPlayerName);
			DisplayText::DisplayText(SCREEN_WIDTH_PIX-10, SCREEN_HEIGHT_PIX+5, GLUT_BITMAP_HELVETICA_12, iScore);
		glPopMatrix();
	}
	else if (iGameStatus == GAME_END){
		glPushMatrix();
			glCallList(iShowScore);
			
			glColor3f(0.75, 0.0, 0.0);
			DisplayText::DisplayText(SCREEN_WIDTH_PIX/2-8, SCREEN_HEIGHT_PIX/2, GLUT_BITMAP_HELVETICA_18, iScore);
		glPopMatrix();
	} 
	else if (iGameStatus == GAME_CLOSE){
		exit(0);
	}
	
	glFlush();
}

void fnReshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH_PIX, 0, SCREEN_HEIGHT_PIX+DASHBOARD_HEIGHT_PIX, 0.5, 2);
	
	glMatrixMode(GL_MODELVIEW);
}

void fnIdle() {
	//clog << "fnIdle():" << endl;
	bool flag_x_offset_changed = false;
	bool flag_y_offset_changed = false;
	
	if (fBallTransX == SCREEN_WIDTH_PIX || fBallTransX == 0) {
		fBallTransXOffset *= -1; // inverse x direction
		flag_x_offset_changed = true;
	}
	if (fBallTransY == SCREEN_HEIGHT_PIX) {
		fBallTransYOffset *= -1; // inverse y direction
		flag_y_offset_changed = true;
	}
	if ((fBallTransX+BALL_RADIUS) >= fBoardTransX && 
		(fBallTransX-BALL_RADIUS) <= (fBoardTransX + BOARD_LENGTH) &&
		(fBallTransY-BALL_RADIUS) <= BOARD_HEIGHT &&
		(fBallTransY) >= BOARD_HEIGHT)	// if ball is hit by the board
	{
		fBallTransYOffset *= -1; // inverse y direction
		iScore += SCORE_OFFSET;
		flag_y_offset_changed = true;
	}
	if (fBallTransY == 0) {
		glutIdleFunc(NULL);
		iGameStatus = GAME_END;
		clog << "Game status changed to GAME_END" << endl;
	}
	
	fBallTransX += fBallTransXOffset;
	fBallTransY += fBallTransYOffset;
	
	if (!flag_x_offset_changed) {
		if (fBallTransX > SCREEN_WIDTH_PIX) {
			fBallTransX = SCREEN_WIDTH_PIX;
		}
		else if (fBallTransX < 0) {
			fBallTransX = 0.0;
		}
	}
	if (!flag_y_offset_changed) {
		if (fBallTransY > SCREEN_HEIGHT_PIX) {
			fBallTransY = SCREEN_HEIGHT_PIX;
		}
		else if (fBallTransY < 0) {
			fBallTransY = 0.0;
		}
	}
	glutPostRedisplay();
}

void fnSpecial(int key, int x, int y) {
	//clog << "fnSpecial():" << "key=" << key << " x=" << x << " y=" << y << endl;
	switch (key) {
	case 100: // left arrow key
		if (fBoardTransX > 0.0 && iGameStatus == GAME_START) {
			fBoardTransX -= fBoardTransXOffset;
		}
		break;
	case 102: // right arrow key
		if ( (fBoardTransX + BOARD_LENGTH) < SCREEN_WIDTH_PIX && iGameStatus == GAME_START) {
			fBoardTransX += fBoardTransXOffset;
		}
		break;
	//case 101: // up arrow key
	//case 103: // down arrow key
	default:
		break;
	}
	glutPostRedisplay();
}

int iTemp =0;

void fnKeyboard(unsigned char key, int x, int y) {
	clog << "fnKeyboard():" << "key=" << key << "(int)key=" << (int)key << " x=" << x << " y=" << y << endl;
	switch (key) {
	case 13: // if enter key pressed
		if ( acPlayerName[0] != '\0' && iGameStatus == GAME_INIT) {
			fBallTransX = SCREEN_WIDTH_PIX/2;
			fBallTransY = SCREEN_HEIGHT_PIX/2;
			fBallTransXOffset = fBallSpeed;
			fBallTransYOffset = fBallSpeed;
			glutIdleFunc(fnIdle);
			iScore = 0;
			startTime = time(NULL);
			clog << "startTime=" << startTime << endl;
			iGameStatus = GAME_START;
			clog << "Game status changed to GAME_START" << endl;
		}
		else if (iGameStatus == GAME_END) {
			iGameStatus = GAME_CLOSE;
			clog << "Game status changed to GAME_CLOSE" << endl;			
		}
		break;
	case 8: // if backspace key pressed
		if (iTemp >= 0 && iGameStatus == GAME_INIT) {
			acPlayerName[--iTemp] = '\0';
		}
		break;
	case 27: // if escape key is pressed
		if (iGameStatus == GAME_END) {
			iGameStatus = GAME_INIT;
			fnKeyboard(13, x, y);
			return;
		}
	default:
		if ( (key >= 'a' || key >= 'A') && 
				(key <= 'z' || key <= 'Z') && 
				iTemp < PLAYER_NAME_LEN-1 && 
				iGameStatus == GAME_INIT
			)
		{
			acPlayerName[iTemp++] = key;
			acPlayerName[iTemp] = '\0';
			clog << "iTemp=" << iTemp << endl;
		}
		break;
	}
	glutPostRedisplay();
}
void init() {
	//clog << "init():" << endl;
	iGameStatus = GAME_INIT;
	
	GLfloat light0_position[] = {-1.0, -1.0, -1.0, 0.0};
	
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	
	iBoardGenList = glGenLists(1);
	glNewList(iBoardGenList, GL_COMPILE);
		glEnable (GL_LINE_STIPPLE);
		glLineStipple (1, 0x0F0F);
		glColor3f(0.5, 0.75, 0.75);
		glBegin(GL_LINES);
			glVertex2d(0.0, BOARD_HEIGHT);
			glVertex2d(BOARD_LENGTH, BOARD_HEIGHT);
		glEnd();
		glDisable(GL_LINE_STIPPLE);
	glEndList();
	
	iSphereGenList = glGenLists(1);
	glNewList(iSphereGenList, GL_COMPILE);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glMaterialfv(GL_FRONT, GL_SPECULAR, afBallSpecular);
		glMaterialfv(GL_FRONT, GL_AMBIENT, afBallAmbient);
		glMaterialfv(GL_FRONT, GL_SHININESS, afBallShininess);
		glutSolidSphere(BALL_RADIUS, 10, 10);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
	glEndList();
	
	iDashboardGenList = glGenLists(1);
	glNewList(iDashboardGenList, GL_COMPILE);
		glColor3f(0.0, 0.5, 0.5);
		glBegin(GL_POLYGON);
			glVertex2d(0, SCREEN_HEIGHT_PIX);
			glVertex2d(0, SCREEN_HEIGHT_PIX+DASHBOARD_HEIGHT_PIX);
			glVertex2d(SCREEN_WIDTH_PIX, SCREEN_HEIGHT_PIX+DASHBOARD_HEIGHT_PIX);
			glVertex2d(SCREEN_WIDTH_PIX, SCREEN_HEIGHT_PIX);
		glEnd();
		
		glColor3f(1.0, 1.0, 1.0);
		DisplayText::DisplayText(1.0, SCREEN_HEIGHT_PIX+5, GLUT_BITMAP_HELVETICA_12, "Player:");
		DisplayText::DisplayText(SCREEN_WIDTH_PIX-20, SCREEN_HEIGHT_PIX+5, GLUT_BITMAP_HELVETICA_12, "Score:");
	glEndList();
	
	iGetNameGenList = glGenLists(1);
	glNewList(iGetNameGenList, GL_COMPILE);
		glColor3f(0.0, 0.5, 0.5);
		glBegin(GL_POLYGON);
			glVertex2d(20, SCREEN_HEIGHT_PIX/2-20);
			glVertex2d(20, SCREEN_HEIGHT_PIX/2+20);
			glVertex2d(SCREEN_WIDTH_PIX-20, SCREEN_HEIGHT_PIX/2+20);
			glVertex2d(SCREEN_WIDTH_PIX-20, SCREEN_HEIGHT_PIX/2-20);
		glEnd();
		
		glColor3f(0.8, 0.8, 0.8);
		DisplayText::DisplayText(23.0, SCREEN_HEIGHT_PIX/2+10, GLUT_BITMAP_HELVETICA_18, 
				"Enter your name and press enter to continue.");
		
		glBegin(GL_POLYGON);
			glVertex2d(SCREEN_WIDTH_PIX/2-20, SCREEN_HEIGHT_PIX/2-7);
			glVertex2d(SCREEN_WIDTH_PIX/2-20, SCREEN_HEIGHT_PIX/2+7);
			glVertex2d(SCREEN_WIDTH_PIX/2+20, SCREEN_HEIGHT_PIX/2+7);
			glVertex2d(SCREEN_WIDTH_PIX/2+20, SCREEN_HEIGHT_PIX/2-7);
		glEnd();
	glEndList();
	
	iShowScore= glGenLists(1);
	glNewList(iShowScore, GL_COMPILE);
		glColor3f(0.0, 0.5, 0.5);
		glBegin(GL_POLYGON);
			glVertex2d(20, SCREEN_HEIGHT_PIX/2-20);
			glVertex2d(20, SCREEN_HEIGHT_PIX/2+20);
			glVertex2d(SCREEN_WIDTH_PIX-20, SCREEN_HEIGHT_PIX/2+20);
			glVertex2d(SCREEN_WIDTH_PIX-20, SCREEN_HEIGHT_PIX/2-20);
		glEnd();
		
		glColor3f(0.8, 0.8, 0.8);
		DisplayText::DisplayText(30.0, SCREEN_HEIGHT_PIX/2+10, GLUT_BITMAP_HELVETICA_18, 
				"Your present score is");
		DisplayText::DisplayText(23.0, SCREEN_HEIGHT_PIX/2-17, GLUT_BITMAP_HELVETICA_18, 
				"Press enter to exit or escape to continue..");
				
		glBegin(GL_POLYGON);
			glVertex2d(SCREEN_WIDTH_PIX/2-20, SCREEN_HEIGHT_PIX/2-7);
			glVertex2d(SCREEN_WIDTH_PIX/2-20, SCREEN_HEIGHT_PIX/2+7);
			glVertex2d(SCREEN_WIDTH_PIX/2+20, SCREEN_HEIGHT_PIX/2+7);
			glVertex2d(SCREEN_WIDTH_PIX/2+20, SCREEN_HEIGHT_PIX/2-7);
		glEnd();
	glEndList();

	
	glLineWidth(BOARD_WIDTH);
	
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(600, 400);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(NAME_OF_GAME);

	init();

	glutDisplayFunc(fnDisplay);
	glutReshapeFunc(fnReshape);
	//glutIdleFunc(fnIdle);
	glutSpecialFunc(fnSpecial);
	glutKeyboardFunc(fnKeyboard);
	
	glutMainLoop();
}
