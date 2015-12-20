#include<iostream>
#include "Font.h"
#include<string>
#include<sstream>
#include<windows.h>

using namespace std;

Font::Font(string fileName, char fontMode_){
    inptFile.open(fileName.c_str());

    if(fontMode_ != BIT_A_BIT && fontMode_ != ALTERNADO){
        printf("\n\nERROR: select font mode a or b.\n\n");
        exit(1);
    }

    fontMode = fontMode_;

    if(fontMode == BIT_A_BIT)
        loadAlphabetA();
    else if(fontMode == ALTERNADO)
        loadAlphabetB();

    if (inptFile.is_open())
       inptFile.close();
}

void Font::print(char msg[], Serial* SP, int x, int y)
{
     //printf("Msg: %s", msg);

    for(int i=0; msg[i] != '\0'; i++){
         int c = (int) msg[i];
        //printf("\n CHAR = %d", c);
        if(c < 0){
            printf("\n\nERROR: Unidentified character.\n\n");
            exit(1);
        }
        int sz;

        Sleep(10);

       //Envia caractere no modo Bit a Bit
       if(fontMode == BIT_A_BIT){
            //printf("Char: %c Modo Bit a Bit length: %d\n", (char)c, charLengthA);

            sz = charLengthA;
            for(int k=0; k < sz; k++){
                //Sleep(1);
                SP->WriteData(&alphabetA[c-32][k], 1);
            }
        }//Envia caractere no modo Alternado
         else if(fontMode == ALTERNADO){

            //inicia comunicacao enviando o byte 254
            unsigned char startCom = 254;
            SP->WriteData(&startCom, 1);

            //Envia posicao x,y (2 bytes para x e 2 para y)
            if(x > 253){
               unsigned char pX = 253;
               SP->WriteData(&pX, 1);
               unsigned char complement = (unsigned char) x - pX;
               SP->WriteData(&complement, 1);
            }
            else{
                unsigned char pX = (unsigned char) x;
                SP->WriteData(&pX, 1);
                unsigned char complement = 0;
                SP->WriteData(&complement, 1);
            }

            if(y > 253){
               unsigned char pY = 253;
               SP->WriteData(&pY, 1);
               unsigned char complement = (unsigned char) y - pY;
               SP->WriteData(&complement, 1);
            }
            else{
                unsigned char pY = (unsigned char) y;
                SP->WriteData(&pY, 1);
                unsigned char complement = 0;
                SP->WriteData(&complement, 1);
            }
            sz = charSizeAlph_B[c-32];

            //Envia dimensoes do glifo
            unsigned char width = (unsigned char) charWidth;
            unsigned char height = (unsigned char) charHeight;
            SP->WriteData(&width, 1);
            SP->WriteData(&height, 1);
            //printf("lengthB: %d\n", sz);

            for(int k=0; k < sz; k++) {
               // Sleep(1);
                SP->WriteData(&alphabetB[c-32][k], 1);
            }

            //Finaliza a comunicacao enviando o byte 255
            unsigned char eoc = 255;
            SP->WriteData(&eoc, 1);
        }
    }
}

void Font::loadAlphabetA(){
    string aux;
    getline(inptFile, fontHeader); //1st line: font header
    cout << "\nHeader: \n" << fontHeader << endl;

    stringstream ss;
    getline(inptFile, aux); //2nd line: character width and height in pixels
    ss.str("");
    ss << aux;
    ss >> charWidth;
    ss >> charHeight;

    cout << "Character size " << charWidth << "x" << charHeight << " pixels" << endl;

    getline(inptFile, aux); //3rd line: size of one character in bytes
    ss.str("");
    ss << aux;
    ss >> charLengthA; //store the size of one character in bytes

    //Verify if the character fits the maximum length
    if(charLengthA > MAX_CHAR_SIZE){
        cout << "ERROR: charLengthA > MAX_CHAR_SIZE." << endl;
        exit(1);
    }
    cout << "Size of one character in bytes: " << charLengthA << endl;

    int lineCnt = 0;
    printf("\nReading alphabet... \n");
    //Sleep(1000);
    while(getline(inptFile, aux)){
        ss.str("");
        ss.clear();
        ss << aux;
        for(int i=0; i < charLengthA; i++){
            //Sleep(10);
            string bf;
            ss >> bf;
            alphabetA[lineCnt][i] = atoi(bf.c_str());

            //printf("%d,",alphabetA[lineCnt][i]);
            //cout << atoi(bf.c_str()) << " ";
        }
        //printf("\n");
        lineCnt++;
    }
    printf("\nThe alphabet file has been read. \n");
}

void Font::loadAlphabetB(){
    string aux;
    getline(inptFile, fontHeader); //1st line: font header
    cout << "\nHeader: \n" << fontHeader << endl;

    stringstream ss;
    getline(inptFile, aux);        //2nd line: character width and height in pixels
    ss.str("");
    ss << aux;
    ss >> charWidth;
    ss >> charHeight;
    cout << "Character size " << charWidth << "x" << charHeight << " pixels" << endl;

    int lineCnt = 0;
    printf("\nReading alphabet... \n");
    while(getline(inptFile, aux)){
        ss.str("");
        ss.clear();
        charSizeAlph_B[lineCnt] = atoi(aux.c_str()); //Size in bytes of the current character
        if(charSizeAlph_B[lineCnt] > MAX_CHAR_SIZE){
            cout << "ERROR: charLengthB > MAX_CHAR_SIZE." << endl;
            exit(1);
        }
        //cout <<"\n Char size: " << charSizesAlph_B[lineCnt] << endl;

        ss.str("");
        ss.clear();
        getline(inptFile, aux); //reads the line containing a character's bytes
        ss << aux;
        for(int i=0; i < charSizeAlph_B[lineCnt]; i++){
            string bf;
            ss >> bf;
            alphabetB[lineCnt][i] = atoi(bf.c_str());
            //printf("%d,",alphabetB[lineCnt][i]);
            //cout << (int) alphabetB[lineCnt][i] << " ";
        }
        //printf("\n");
        lineCnt++;
    }
    printf("\nThe alphabet has been read.\n");
}

void Font::printOnScreen(char c){
    if(fontMode == BIT_A_BIT)
        printOnScreenModeA(c);
    else if(fontMode == ALTERNADO)
        printOnScreenModeB(c);
}

void Font::printOnScreenModeA(char c){
    printf("\n\n");
    unsigned char mask;
    int bit = 0, byte = 0, cont = 0;
    int asciiInt = (int) c;

    if(asciiInt < 0){
        printf("\n\nERROR: Unidentified character.\n\n");
        exit(1);
    }


    for(int i=0; i < 95; i++){
        printf("\n\nChar[%d]\n\n", i);

        printf("\n\nWidth %d\n", charWidth);
        printf("Height %d\n\n", charHeight);

        bit = 0; byte = 0; cont = 0;

        for(int y = 0; y< charHeight; y++)
        //for(int y = charHeight - 1; y >= 0; y--)
        {
            for(int x = 0; x < charWidth; x++) {
                //mask = alphabetA[asciiInt-32][byte] & (1 << (7 - bit) );
                mask = alphabetA[i][byte] & (1 << (7 - bit) );

                if( mask > 0 ) {
                    printf(".");
                }
                else {
                    printf("0");
                }
                cont++;
                bit  = cont % 8;
                byte = cont / 8;
            }
            printf("  %d", y);
            printf("\n");
        }/**/
        printf("\n\n");
    }

}

void Font::printOnScreenModeB(char c){
    bool flagBack   = true;
    int col         = 0;
    int line        = 0;
    int asciiInt = (int) c;

    if(asciiInt < 0){
        printf("\n\nERROR: Unidentified character.\n\n");
        exit(1);
    }

    printf("\n\n");
    for(int idx = 0; idx < charSizeAlph_B[asciiInt-32] - 1; idx++){ //-1 because we don't need the (255) terminator
        for(int i = 0; i < alphabetB[asciiInt-32][idx]; i++){

            if( flagBack == true)
            {
                printf("0");
            }
            else
            {
                printf(".");
            }
            col++;
            if( col == charWidth )
            {
                printf("  %d\n",line);
                col = 0;
                line++;
            }
        }
        flagBack = !flagBack;
    }
}

