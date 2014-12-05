	
/*
File name		:	DisplayText.h
Author			:	Sreejith Sreekantan (krssreejith@gmail.com)
Version			:	3.0
Date			:	June 2012
*/
#ifndef __DisplayText_h__
#define __DisplayText_h__

#ifdef __APPLE__
	#include <GLUT/glut.h>
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
#else	
	#include <GL/glut.h>
#endif

#include <iostream>

using namespace std;

namespace DisplayText {
	void DisplayText(float posx, float posy, void* font, char string[]) {
		glRasterPos2f(posx, posy);
		while(*string) {
			glutBitmapCharacter(font, *string);
			string++;
		}
	}
	
	void DisplayText(float posx, float posy, void* font, int value) {
		int iTemp1=0, iTemp2 = value;
		while(iTemp2>0) {
			iTemp1++;
			iTemp2 /= 10;
		}
		char *string = (char*)malloc(sizeof(char)*iTemp1+1);
		sprintf(string, "%d\0", value);
		//clog << "string=" << *string << endl;
		DisplayText(posx, posy, font, string);
		
	}
}

#endif // __DisplayText_h__
