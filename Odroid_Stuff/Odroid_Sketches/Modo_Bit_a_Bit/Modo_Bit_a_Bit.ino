#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9340.h>
#include <Adafruit_GFX.h>
#include "TimerOne.h"

#include <avr/pgmspace.h>
#include <limits.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

// These are the pins used for the UNO
// for Due/Mega/Leonardo use the hardware SPI pins (which are different)
#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8

#define NOTSPECIAL	1
#define GOTESCAPE	2
#define GOTBRACKET	3
#define INNUM           4
#define IMGSHOW         5

#define LEFT_EDGE0             0
#define RIGHT_EDGE320          319
#define TOP_EDGE0              0
#define BOTTOM_EDGE240         239

#define DEBUG

//max length (in bytes) for our buffer
//feel free to change if you need a different length
#define MAX_GLYPH_LENGTH 1000

const char version[] = "v1.5";

typedef struct cursor {
        uint32_t row;
        uint32_t col;
}
cursor;
cursor cursor_sav = { 
        0, 0 };
cursor startImage, endImage;

uint32_t x = 0;
uint8_t c;
uint8_t carr[64] = {0};

uint8_t current_state = NOTSPECIAL;
uint8_t previous_state = NOTSPECIAL;
uint32_t tmpnum;

uint16_t num, row, col;
uint16_t bottom_edge0 = BOTTOM_EDGE240;
uint16_t right_edge0  = RIGHT_EDGE320;

uint8_t pwm = 255;
uint8_t textSize = 1; //inicia em 1, apos correcao da funcao drawPixel em Adafruit_ILI9430.cpp. 
uint8_t rotation = 1;
uint16_t foregroundColor, backgroundColor;

uint32_t imgsize = 0;
uint32_t sizecnt = 0;

uint8_t ledPin = 5; // PWM LED Backlight control to digital pin 5
uint8_t rgb565hi, rgb565lo;
uint8_t cntenable = 0;


float ang = 0;
int x_screen = 0, y_screen = 0;

// Using software SPI is really not suggested, its incredibly slow
// Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _mosi, _sclk, _rst, _miso);
// Use hardware SPI
Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);
uint32_t contSerial = 0;

uint32_t cnt = 0; 
uint32_t posXCnt = 0; 
uint32_t posX = 0, posY = 0;
unsigned char myChar[MAX_GLYPH_LENGTH];
unsigned char receivedByte;
bool isReading = true;

void setup() {
  Serial.begin(500000);//500000    115200
  //Serial.println("Sent by Odroid: Welcome to MyFont - Project");

  tft.begin();
  // initialize the digital pin as an output for LED Backlight
  initPins();

  tft.setRotation(rotation);
  tft.setTextSize(textSize);
  tft.setCursor(50, 50);
  tft.print("MyFont - Project Loading...");
  tft.setCursor(250, 200);
  tft.print(version);

  delay(1000);
  tft.fillScreen(backgroundColor);
  tft.setCursor(0, 0);
}

void loop() {
  
  if(Serial.available() > 0 ){ 
    receivedByte = Serial.read();
    myChar[cnt] = receivedByte;
    cnt++;
  }
  /*You need to set the glyph's length manually*/
  /*cnt == glyph's length*/   
  if(cnt == 199){
    printCharA(50,50,33,48,myChar, cnt-1);    
    cnt = 0;
    receivedByte = 0;
  }
}
/*This function is used to print glyphs on mode BIT_A_BIT*/ 
void printCharA(int x, int y, int w, int h, unsigned char c__[], int cSize){
  /*You need the modified version of Adafruit_ILI9340 to use this method*/
  tft.printTextPozzer(x,y,w,h,c__);
  /*Use this code if you want Odroid
  to increment glyph's position
  posX+=w;
  if(posX + w > right_edge0){
    posX  =  0;
    posY +=  h;    
    if(posY + h > bottom_edge0){
      posX  =  0;
      posY  =  40;
    }
  }*/
}

void initPins(){
  pinMode(ledPin, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  analogWrite(ledPin, pwm);
}
