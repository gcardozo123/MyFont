//Include GLEW  
#include "GL/glew.h"

//Include GLFW  
#include <GLFW/glfw3.h>  

// GLM
#include <glm\glm.hpp>

//Include the standard C++ headers  
#include <stdio.h>  
#include <stdlib.h>
#include <iostream>
#include <cassert>

#include <ctime> //used to get system time
#include <stdexcept> //Tratamento de excecoes
#include "../../MyFontClass/Font.h"

inline int next_p2(int a)
{
	int rval = 1;
	while (rval<a) rval <<= 1;
	return rval;
}

inline void pushScreenCoordinateMatrix() {
	glPushAttrib(GL_TRANSFORM_BIT);
	GLint	viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(viewport[0], viewport[2], viewport[1], viewport[3]);
	glPopAttrib();
}

//Define an error callback  
void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback  
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	return true;
}

void displayTexture(int width, int height, GLuint texId, unsigned char *tex){
	int w_ = next_p2(width);    //freetype trabalha com dimensoes em potencias de 2
	int h_ = next_p2(height);   //freetype trabalha com dimensoes em potencias de 2
	
	float	x = (float)width / (float)w_;	//coordenada para mapear textura
	float	y = (float)height / (float)h_;  //coordenada para mapear textura

	// We want a coordinate system where things coresponding to window pixels.
	pushScreenCoordinateMatrix();
	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Now we just setup some texture paramaters.
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_, h_,
		0, GL_RGBA, GL_UNSIGNED_BYTE, tex);

	glLoadIdentity();

	glPushMatrix();
	//glTranslatef(300.0f, 300.0f, 0.0f);

	glBegin(GL_QUADS); //Here we draw the texturemaped quads.
	glTexCoord2d(0, 0); glVertex2f(0, height);
	glTexCoord2d(0, y); glVertex2f(0, 0);
	glTexCoord2d(x, y); glVertex2f(width, 0);
	glTexCoord2d(x, 0); glVertex2f(width, height);
	glEnd();
	glPopMatrix();
}


unsigned char* genTextureForOneChar(Font* f, char c, unsigned char r, unsigned char g, 
	unsigned char b, unsigned char a)
{
	int width, height;
	f->getDimensions(&width, &height);

	int w_ = next_p2(width);    //freetype trabalha com dimensoes em potencias de 2
	int h_ = next_p2(height);   //freetype trabalha com dimensoes em potencias de 2

	int length = w_ * h_ * 4;   //4 vezes maior que um glifo, para conter RGBA
	unsigned char *arr = new unsigned char[length];
	memset(arr, 0, length);
	f->getCharGL(c, arr, r, g, b, a);

	return arr;
}
int main(void)
{
	//Set the error callback  
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW  
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Declare a window object  
	GLFWwindow* window;

	//Create a window and create its OpenGL context  
	window = glfwCreateWindow(640, 480, "MyFont!", NULL, NULL);

	//If the window couldn't be created  
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread.   
	glfwMakeContextCurrent(window);

	//Sets the key callback  
	glfwSetKeyCallback(window, key_callback);

	//Initialize GLEW  
	GLenum err = glewInit();

	//If GLEW hasn't initialized  
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}
	
	//Set a background color  
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);

	InitGL();
	int start_s = clock();
	Font *myFont; 
	///////Tratamento de erros ao gerar a fonte//////////
	try{
		myFont = new Font("./resources/courbd.ttf", 15); //Build freetype font
	}
	catch (std::runtime_error& e){
		std::cout << e.what() << std::endl;
		
		glfwSetWindowShouldClose(window, GL_TRUE);
		glfwDestroyWindow(window);
		glfwTerminate();
		
		system("Pause");
		exit(EXIT_FAILURE);
	}
	/////////////////////////////////////////////////////

	int stop_s = clock();
	//Tempo de geracao da fonte (em milisegundos)
	std::cout << "Font generated in " << 
		(stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << " milliseconds."<< std::endl;

	unsigned char *tex;
	tex = genTextureForOneChar(myFont, 'G', 255,0,255,200); //Gera textura para um caractere 
	int width, height;
	myFont->getDimensions(&width, &height);

	GLuint tex_base[95];
	glGenTextures(95, tex_base);
	//Main Loop  
	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
		
		displayTexture(width, height, tex_base[0], tex); //Mostra a textura tex

		//Swap buffers  
		glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();

	} //Check if the ESC key had been pressed or if the window had been closed  
	while (!glfwWindowShouldClose(window));

	delete[] tex;

	//Close OpenGL window and terminate GLFW  
	glfwDestroyWindow(window);
	//Finalize and clean up GLFW  
	glfwTerminate();

	exit(EXIT_SUCCESS);
}