#pragma once

//FreeType Headers
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <windows.h>

#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#define NUM_CHARS 95
#define ASCII_ALPHABET_BEGIN 32
#define ASCII_ALPHABET_END 127
#define MAX_CHAR_SIZE 2000

class Font
{
public:
	Font(const char * fname, unsigned int h);
	
	//Armazena em "width" e "height" as dimensoes da fonte	
	void getDimensions(int *width, int *height);
	//Armazena glifo de "c" em "arr[]" no modo Bit a Bit	
	void getCharBit_a_Bit(char c, unsigned char arr[], int *length);
	//Armazena glifo de "c" em "arr[]" no modo Alternado
	void getCharAlternado(char c, unsigned char arr[], int *length);

	//Armazena glifo de "c" em "arr[]" (no formato textura GL_RGBA) 
	//"arr[]" tem length = (proxima potencia de 2 de width) * (proxima potencia de 2 de height) * 4
	void getCharGL(char c, unsigned char arr[], unsigned char r,
		unsigned char g, unsigned char b, unsigned char a);
	
	void print(char c);					//Imprime um glifo no console (feito de caracteres zeros e pontos)
	void printAlphabet();				//Imprime no console a fonte gerada 
	void printFreetypeOnConsole();		//Imprime no console os glifos originais da FreeType 2 

private: 
	
	//Coisas da minha fonte/////////////////
	int width, height;									//dimensao de um caractere
	int size;											//size usado ao gerar a fonte com a freetype
	std::string familyName;								//family name da fonte (ex: Courier New)
	std::string style;									//estilo da fonte (ex: Bold)
	std::string fontPath;								//path do arquivo usado para gerar a fonte
	std::vector<std::vector<int>> fontAlphabet;			//alfabeto como matriz de bits
	unsigned char alphabetB[NUM_CHARS][MAX_CHAR_SIZE];	//Alfabeto no modo Alternado
	unsigned char alphabetA[NUM_CHARS][MAX_CHAR_SIZE];	//Alfabeto no modo Bit a Bit
	int charSizesAlph_B[NUM_CHARS];						//length de cada caractere do alphabetB
	
	void genFontAlphabet();		//gera alfabeto da nossa fonte (guarda em fontAlphabet). Soh deve ser chamado no construtor
	int getMaxLengthB();	    //a maior length de um caractere no modo Alternado

	void genModeBAlphabet();	//gera o alfabeto no modo Alternado (guarda em alphabetB)
	void writeAlphabetFileModeB();   //gera o arquivo contendo o alfabeto no modo Alternado
	
	void genModeAAlphabet();		 //gera o alfabeto no modo Bit a Bit (guarda em alphabetA)
	void writeAlphabetFileModeA();   //gera o arquivo contendo o alfabeto no modo Bit a Bit
	
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

