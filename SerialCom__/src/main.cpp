#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include "SerialClass.h"
#include "Font.h"

int main()
{
	printf("Welcome to the MyFont Project!\n\n");
   Serial *SP = new Serial("COM6"); //Set the serial port you are currently using
	SP->setPort();

	if (SP->IsConnected())
		printf("We're connected");

	char incomingData[256] = "";
	int dataLength = 256;
	int readResult = 0;

    Sleep(4000);

   Font myFontB("Courier New_Bold_36.txt", ALTERNADO);

   char c[2];
   c[0] = 32;
   c[1] = '\0';
   int posX = 0;
   int posY = 0;
   int w = 33;
   int h = 48;

   for(int i = 0; i < 1000; i++){
      myFontB.print(c,SP,posX,posY);
      c[0] = c[0] + 1;
      if(c[0]>126)
         c[0]=32;

      posX+=33;
      if(posX + w > 320){
        posX  =  0;
        posY +=  h;
        if(posY + h > 240){
          posX  =  0;
          posY  =  0;
        }
      }
    }

	return 0;
}
