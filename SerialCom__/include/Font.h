#ifndef FONT_H
#define FONT_H

#include <string>
#include<fstream>

#include "SerialClass.h"

#define NUM_OF_CHARS 99
#define MAX_CHAR_SIZE 2000

#define BIT_A_BIT 0 // (bit 0 = background, bit 1 = foreground)
#define ALTERNADO 1 // alternates between background and foreground bytes

using namespace std;
class Font
{
    public:
        Font(string fileName, char fontMode_);
        void print(char msg[], Serial *SP, int x, int y);
        void printOnScreen(char c);

    private:
        void loadAlphabetA();
        void loadAlphabetB();
        void printOnScreenModeA(char c);
        void printOnScreenModeB(char c);

        int fontMode;      //Font mode 'a' (bit 0 = background, bit 1 = foreground) or 'b' (background/foreground alternation)
        string fontHeader;
        int charHeight;    //character height in pixels
        int charWidth;     //character width in pixels
        int charLengthA;   //character size in bytes (mode A)

        unsigned char alphabetA[NUM_OF_CHARS][MAX_CHAR_SIZE]; //A maximum of NUM_OF_CHARS characters inside the alphabet, containg MAX_CHAR_SIZE bytes each character.
        int charSizeAlph_B[NUM_OF_CHARS]; //Array that indicates the size in bytes for each character in the alphabetB
        unsigned char alphabetB[NUM_OF_CHARS][MAX_CHAR_SIZE]; //A maximum of NUM_OF_CHARS characters inside the alphabet, containg MAX_CHAR_SIZE bytes each character.

        ifstream inptFile;  //File containing the font
};

#endif // FONT_H
