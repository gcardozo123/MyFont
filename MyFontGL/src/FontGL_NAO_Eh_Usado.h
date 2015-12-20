#pragma once

//FreeType Headers
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

//OpenGL Headers 
#include <windows.h>		//(the GL headers need it)
#include <GL/gl.h>
#include <GL/glu.h>

//Some STL headers
#include <vector>

#include <fstream>
#include <sstream>

#define NUM_CHARS 95
#define ASCII_ALPHABET_BEGIN 32
#define ASCII_ALPHABET_END 127
#define MAX_CHAR_SIZE 2000

class FontGL
{
public:
	//Creates a font of the height h from the file fname.
	FontGL(const char * fname, unsigned int h);

	void getDimensions(int *width, int *height);
	void getCharModeA(char c, unsigned char arr[], int *length);
	void getCharModeB(char c, unsigned char arr[], int *length);
	
	//armazena o caractere "c" em "arr[]" (no formato textura GL_RGBA) 
	//"arr[]" tem length = width * height * 4
	void getCharGL(char c, unsigned char arr[], unsigned char r, 
		unsigned char g, unsigned char b, unsigned char a); 

	void printAlphabet();				//imprime no console a fonte gerada 
	void print(char c);					//imprime um caractere no console
	void printFreetypeOnConsole();		//imprime alfabeto original da freetype no console

private:

	//Coisas da minha fonte/////////////////
	int width, height;									//dimensao de um caractere
	std::vector<std::vector<int>> fontAlphabet;			//alfabeto como matriz de bits (guardado como inteiros 0s e 1s)
	unsigned char alphabetB[NUM_CHARS][MAX_CHAR_SIZE];	//Alfabeto no modo B
	int charSizesAlph_B[NUM_CHARS];						//length de cada caractere do alphabetB

	void genFontAlphabet();		//gera alfabeto da nossa fonte (guarda em fontAlphabet). Soh deve ser chamado no construtor
	void genModeBAlphabet();	//gera o alfabeto no modo B (guarda em alphabetB)
	void writeAlphabetFile();   //gera o arquivo contendo o alfabeto no modo B
	///////////////////////////////////////

	//Coisas da Freetype//////////////
	FT_Face face;			//holds information on a given font
	FT_Library library;		//freetype library

	std::vector<std::vector<int>> freetypeAlphabet; //caracteres da Freetype guardados como 0s (background) e 1s (foreground)
	int freetypeWidths[NUM_CHARS];					//Largura de cada caractere gerado pela freetype
	int freetypeHeights[NUM_CHARS];					//Altura de cada caractere gerado pela freetype
	int freetypeGlyphs_Top[NUM_CHARS];						//Distancia da baseline ateh o topo de um caractere

	void genFreetypeAlphabet(); //gera alfabeto da freetye (guarda em freetypeAlphabet). Soh deve ser chamado no construtor
	int minTopIndex();          //retorna o indice do menor freetypeGlyphs_Top
	/////////////////////////////////

};

