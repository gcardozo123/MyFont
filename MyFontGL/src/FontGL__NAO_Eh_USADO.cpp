#include "FontGL.h"
#include <iostream>
using std::vector;

inline int next_p2(int a)
{
	int rval = 1;
	while (rval<a) rval <<= 1;
	return rval;
}

template <class T> T findMin(T arr[], int n)
{
	int i;
	T min;
	min = arr[0];
	for (i = 0; i<n; i++)
	{
		if (min > arr[i])
			min = arr[i];
	}
	return min;
}

template <class T> T findMax(T arr[], int n)
{
	int i;
	T max;
	max = arr[0];
	for (i = 0; i < n; i++)
	{
		if (arr[i] > max)
			max = arr[i];
	}
	return max;
}

FontGL::FontGL(const char * fname, unsigned int h)
{
	width = 0;
	height = 0;

	//Create and initilize a freetype font library.
	if (FT_Init_FreeType(&library))
		throw std::runtime_error("FT_Init_FreeType failed");

	//This is where we load in the font information from the file.
	if (FT_New_Face(library, fname, 0, &face))
		throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)");

	//Freetype measures font size in terms of 1/64ths of pixels. h<<6 = h*64
	FT_Set_Char_Size(face, h << 6, h << 6, 96, 96);

	//gera alfabeto da freetye (guarda em freetypeAlphabet). Soh deve ser chamado aqui no construtor.
	genFreetypeAlphabet();

	genFontAlphabet();
	genModeBAlphabet();
	//printAlphabet();

	writeAlphabetFile();

	//print('!');
	//print('\\');

	//We don't need the face information anymore
	FT_Done_Face(face);
	//Ditto for the library.
	FT_Done_FreeType(library);
}


//Cria o alfabeto da freetype das posicoes ASCII_ALPHABET_BEGIN ateh ASCII_ALPHABET_BEGIN + NUM_CHARS da tabela Ascii
void FontGL::genFreetypeAlphabet() {

	for (int ch = ASCII_ALPHABET_BEGIN; ch < ASCII_ALPHABET_BEGIN + NUM_CHARS; ch++){
		//Load the Glyph for our character.
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, (char)ch), FT_LOAD_DEFAULT))
			throw std::runtime_error("FT_Load_Glyph failed");

		//Move the face's glyph into a Glyph object.
		FT_Glyph glyph;
		if (FT_Get_Glyph(face->glyph, &glyph))
			throw std::runtime_error("FT_Get_Glyph failed");

		//Convert the glyph to a bitmap.
		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

		//This reference will make accessing the bitmap easier
		FT_Bitmap& bitmap = bitmap_glyph->bitmap;
		//bitmap.palette_mode = ft_pixel_mode_mono;

		//Use our helper function to get the widths of
		//the bitmap data that we will need in order to create
		//our texture.
		int cwidth = next_p2(bitmap.width);
		int cheight = next_p2(bitmap.rows);

		//bitmap
		unsigned char *expanded_data = new unsigned char[2 * cwidth * cheight];

		vector<int> currentCharacter;
		
		for (int j = 0; j <bitmap.rows; j++) {
			for (int i = 0; i < bitmap.width; i++){
				unsigned char aux = (i >= bitmap.width || j >= bitmap.rows) ? 0 : bitmap.buffer[i + bitmap.width*j];
				expanded_data[2 * (i + j*cwidth)] = expanded_data[2 * (i + j*cwidth) + 1] = aux;

				if (aux > 100){
					//(".");
					currentCharacter.push_back(1);
				}
				else{
					//printf("0");
					currentCharacter.push_back(0);
				}
			}
			//printf("\n");
		}
		freetypeAlphabet.push_back(currentCharacter);

		//we don't need to expanded data anymore
		delete[] expanded_data;

		//("\n%d x %d\n", bitmap.width, bitmap.rows);

		freetypeWidths[ch - ASCII_ALPHABET_BEGIN] = bitmap.width;
		freetypeHeights[ch - ASCII_ALPHABET_BEGIN] = bitmap.rows;
		freetypeGlyphs_Top[ch - ASCII_ALPHABET_BEGIN] = bitmap_glyph->top;
	}
}

void FontGL::genFontAlphabet(){
	int maxTop = findMax(freetypeGlyphs_Top, NUM_CHARS);

	width = findMax(freetypeWidths, NUM_CHARS);
	height = maxTop + abs(findMin(freetypeGlyphs_Top, NUM_CHARS) - freetypeHeights[minTopIndex()]);

	int h_ = height;
	int w_ = width;

	for (int c = 0; c < NUM_CHARS; c++){

		int extraCols = (w_ - freetypeWidths[c]) / 2;
		int extraRows = maxTop - freetypeGlyphs_Top[c];

		vector<int> currentChar;
		for (int i = 0; i < h_; i++){
			for (int j = 0; j < w_; j++){
				int aux = extraCols + freetypeWidths[c];
				int aux2 = extraRows + freetypeHeights[c];
				//se j estiver na area correta, guarda info do caractere
				if ((i >= extraRows) && (i < aux2) && (j >= extraCols) && (j < aux)){
					currentChar.push_back(freetypeAlphabet[c][((i - extraRows)* freetypeWidths[c]) + (j - extraCols)]);
				}
				//se nao, coloca 0
				else{
					currentChar.push_back(0);
				}
			}
		}
		fontAlphabet.push_back(currentChar);
	}
}

int FontGL::minTopIndex(){
	int min = freetypeGlyphs_Top[0];
	int minTopIndex = 0;
	for (int i = 0; i < NUM_CHARS; i++){
		if (freetypeGlyphs_Top[i] < min){
			min = freetypeGlyphs_Top[i];
			minTopIndex = i;
		}
	}
	return minTopIndex;
}

void FontGL::genModeBAlphabet(){

	//std::cout << fontAlphabet[33][46] << std::endl;

	bool flagBack = true; //background pixel (flag)
	int contBack = 0, contForegr = 0, cnt = 0;

	for (int c = 0; c < NUM_CHARS; c++){

		for (int i = 0; i < height; i++){
			for (int j = 0; j < width; j++){
				//Background pixel
				if (fontAlphabet[c][(i*width) + j] == 0){
					//printf("0");

					//Keep going with background pixels
					if (flagBack == true)
					{
						contBack++;
						if (contBack == 254)
						{
							alphabetB[c][cnt] = 253;
							alphabetB[c][cnt + 1] = 0;
							contBack = 1;
							cnt += 2;
						}
					}
					//Changed state to background pixels
					else
					{
						flagBack = true;
						alphabetB[c][cnt] = contForegr;
						contBack = 1;
						cnt++;
					}

				}
				else{
					//printf(".");

					//Changed state to foreground pixels
					if (flagBack == true)
					{
						flagBack = false;
						alphabetB[c][cnt] = contBack;
						contForegr = 1;
						cnt++;
					}
					//Keep going with foreground pixels
					else
					{
						contForegr++;
						if (contForegr == 254)
						{
							alphabetB[c][cnt] = 253;
							alphabetB[c][cnt + 1] = 0;
							contForegr = 1;
							cnt += 2;
						}
					}
				}
			}

		}
		//troca caractere "\n"
		if (flagBack == true)
		{
			alphabetB[c][cnt] = contBack;
			cnt++;
		}
		else
		{
			alphabetB[c][cnt] = contForegr;
			cnt++;
		}
		charSizesAlph_B[c] = cnt;

		//charSizesAlph_B[c] = cnt + 1; //cnt + 1 because we're going to put 255 at the end of our character
		//alphabetB[c][cnt] = 255; //255 means the end of our character
		//std::cout << "CharSize: " << charSizesAlph_B[a] << std::endl;

		cnt = 0;
		contForegr = 0;
		contBack = 0;
		flagBack = true;
	}
}

void FontGL::writeAlphabetFile(){
	std::ofstream myFile("FONT_MODE_B.txt");

	if (myFile.is_open())
	{
		//File Header
		myFile << "Font: .ttf. Character size " << width << " x " << height << " px. ModeB." << std::endl;

		//Character dimensions
		myFile << width << " " << height << " pixels\n";

		for (int c = 0; c < NUM_CHARS; c++){
			myFile << charSizesAlph_B[c] << std::endl; //printa a length do caractere
			for (int i = 0; i < charSizesAlph_B[c]; i++)
			{
				auto aux = alphabetB[c][i];
				myFile << (int)aux << " ";
			}
			myFile << std::endl;
		}
		myFile.close();
	}
	else std::cout << "Unable to open file" << std::endl;
}

//printa no console a fonte gerada
void FontGL::printAlphabet(){

	for (int c = 0; c < NUM_CHARS; c++){
		for (int i = 0; i < height; i++){
			for (int j = 0; j < width; j++){
				if (fontAlphabet[c][(i*width) + j] == 1)
					printf(".");
				else
					printf("0");
			}
			printf("\n");
		}
		printf("\n\n\n");
	}
}

void FontGL::print(char c){
	std::ofstream myFile("ONE_CHAR.txt");
	int index = (int)c - ASCII_ALPHABET_BEGIN;

	if (myFile.is_open())
	{
		//File Header
		myFile << "Font: .ttf. Character size " << width << "x " << height << " px. ModeB." << std::endl;

		//Character dimensions
		myFile << width << " " << height << " pixels\n";
		//Write character (mode b)
		for (int i = 0; i < charSizesAlph_B[index]; i++){
			auto aux = alphabetB[index][i];
			myFile << (int)aux << " ";
		}
		myFile << std::endl;
		//Print character on console 
		for (int i = 0; i < height; i++){
			for (int j = 0; j < width; j++){
				if (fontAlphabet[index][(i * width) + j] == 1){
					printf(".");
					myFile << ".";
				}
				else{
					printf("0");
					myFile << "0";
				}
			}
			printf("\n");
			myFile << std::endl;
		}
		myFile << std::endl;
		printf("\n");
		myFile.close();
	}
	else std::cout << "Unable to open file" << std::endl;
}

void FontGL::getDimensions(int *width, int *height){
	*width = this->width;
	*height = this->height;
}

void FontGL::getCharModeA(char c, unsigned char arr[], int *length){
	int bit = 0, byte = 0, cont = 0;
	unsigned char mask = 0;
	*length = 0;
	int pixColor = 0;
	//le o bitmap do caractere
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = (y*width) + x;
			//se for foreground, coloca um bit 1
			if (fontAlphabet[(int)c - ASCII_ALPHABET_BEGIN][pos] == 1)
			{
				mask = 1 << (7 - bit);
				arr[byte] |= mask;
				pixColor++;
			}
			cont++;
			bit = cont % 8;
			byte = cont / 8;
		}

	}

	*length = ((width * height) / 8) + 1; 

	//printf("\nPixels em foreground: %d", pixColor);
	//printf("\nGerou vetor com %d bytes.\n", byte);
}

void FontGL::getCharModeB(char c, unsigned char arr[], int *length){
	int index = (int)c - ASCII_ALPHABET_BEGIN;
	*length = charSizesAlph_B[index];

	for (int i = 0; i < *length; i++)
		arr[i] = alphabetB[index][i];
}

void FontGL::getCharGL(char c, unsigned char arr[], unsigned char r,
	unsigned char g, unsigned char b, unsigned char a){
	
	int w_ = next_p2(width);   //freetype trabalha com dimensoes em potencias de 2
	int h_ = next_p2(height);  //freetype trabalha com dimensoes em potencias de 2
	
	int length = w_ * h_ * 4;  //4 vezes maior que um caractere para conter RGBA
	memset(arr, 0, length);

	int index = (int)c - ASCII_ALPHABET_BEGIN;

	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			int aux = fontAlphabet[index][(i * width) + j];
			int posTexture = (i*w_) + j;
			if (aux){
				arr[4 * posTexture	  ] = r;
				arr[4 * posTexture + 1] = g;
				arr[4 * posTexture + 2] = b;
				arr[4 * posTexture + 3] = a;

			}
			else{
				arr[4 * posTexture    ] = 0;
				arr[4 * posTexture + 1] = 0;
				arr[4 * posTexture + 2] = 0;
				arr[4 * posTexture + 3] = a;
			}
		}
	}

}


void FontGL::printFreetypeOnConsole(){
	for (int c = 0; c < NUM_CHARS; c++){
		for (int i = 0; i < freetypeHeights[c]; i++){
			for (int j = 0; j < freetypeWidths[c]; j++){
				if (freetypeAlphabet[c][(i*freetypeWidths[c]) + j] == 1)
					printf(".");
				else
					printf("0");
			}
			printf("\n");
		}
		printf("\n\n\n");
	}
}