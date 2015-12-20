// MyFont.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <iostream>
#include <ctime> //Usado para pegar o tempo do sistema
#include "../../MyFontClass/Font.h"
#include <stdexcept> //Tratamento de excecoes

int main()
{
	int start_s = clock();
	
	Font *myFont;
	///////Tratamento de erros ao criar a fonte//////////
	try{
		myFont = new Font("./resources/Consolas.ttf", 15);
	}
	catch (std::runtime_error& e){
		std::cout << e.what() <<  std::endl;
		system("Pause");
		exit(EXIT_FAILURE);
	}
	/////////////////////////////////////////////////////

	int stop_s = clock();
	//Tempo de geracao da fonte (em milisegundos)
	std::cout << "Font generated in " << 
		(stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << " milliseconds." << std::endl; /**/

	/*
	unsigned char oneGlyph[1000];
	memset(oneGlyph, 0, 1000);
	int length = 0; 
	
	myFont->getCharAlternado('k', oneGlyph, &length);
	//Imprime no console o caractere gerado (modo Alternado)
	for (int i = 0; i < length; i++){
		std::cout << (int)oneGlyph[i] << " ";
	}
	std::cout << std::endl << std::endl;

	int width, height;
	myFont->getDimensions(&width, &height);
	std::cout << width << " x " << height << " pixels" << std::endl;
	*/
	myFont->print('G');		//imprime o glifo G no console (com '0' e '.')
	//myFont->printAlphabet();	//imprime todos os glifos no console
	//myFont->printFreetypeOnConsole();		//imprime no console os glifos originais da Freetype 2

	system("Pause");
	return EXIT_SUCCESS;
}

