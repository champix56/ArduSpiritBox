//#include "config.h"
#include <stdio.h>
#include <stdlib.h>

void setMenu(byte touchPressed, bool longPressed);
/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
//https://www.electromaker.io/project/view/amfmsw-radio-receiver-si4730-si4735
#include <LiquidCrystal.h>
#define SCREEN_HEIGHT 2
#define SCREEN_WIDTH 16
struct _MenuItem{
    char ** textContent;
    uint8_t sz;
    void(*action)(byte,bool);
    struct _MenuItem *children;
};
typedef struct _MenuItem MenuItem;
MenuItem *fullMenuContent;
typedef struct MenuStruct
{
    uint8_t levelSelector;
    uint8_t level;
    char **menuContent;
    uint8_t menuSz;
    MenuItem *selectedContent;
} MenuStruct;
MenuStruct *menustruct = NULL;
const char  MENU_LEVEL1[][SCREEN_WIDTH+1] = {"see range base","set range", "set record", "exit"};
#define MENU_LEVEL1_SZ 3
typedef enum
{
    AM=1,
    FM=2,
    SW=3,
    LW=4
} FREQ_TYPE;
typedef struct {
  float start;
  float end;
  uint8_t freqPow;
  FREQ_TYPE type;
}FreqRange;
//AM 520->1710Khz FM 64->108Mhz SW 2.300->21.850Mhz LW 153 -> 279Khz
#define RADIO_RANGE (float[][2]){{520.00,1710.00},{64.00,108.00},{2.300,21.850},{153.00,279.00}}
#define RADIO_STEP (float[]){1.00,0.5,0.01,1}
#define POW_FREQ  (uint_8_t[]) {3,6,6,3}
typedef struct
{
    float start;
    float end;
} freqRange;
typedef struct
{
    FREQ_TYPE freqType;
    float freq;
    unsigned short sweepSpeed;
    float temp, lastTemp, lastTemp_1;
    FreqRange *ranges;

} WorkingValues;
WorkingValues *wValues = NULL;

#define BUTTON_VALUE_UP 235
#define BUTTON_VALUE_DW 225
#define BUTTON_VALUE_LT 255
#define BUTTON_VALUE_RT 162
#define BUTTON_VALUE_OK 197
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);

const byte ledPin = 13;
const byte interruptPin = 2;
volatile byte state = LOW;
#define TEMP_PIN A3
void getTemp()
{
    float readVal = (float)analogRead(TEMP_PIN) / 1024;
    wValues->temp = ((readVal * 5) - 0.5) * 100;
}
void setup()
{
    Serial.begin(9600);
    lcd.begin(16, SCREEN_HEIGHT);
    // Print a message to the LCD.
    lcd.print("   Spirit-box");

    pinMode(interruptPin, INPUT);
    //init initial struct
    wValues = (WorkingValues *)calloc(1, sizeof(WorkingValues));
    wValues->freq = 88.900;
    wValues->sweepSpeed = 300;

    attachInterrupt(digitalPinToInterrupt(interruptPin), inputKbPressed, RISING);

    delay(900);
    lcd.clear();
}
void setScreen()
{
    char  buf[7]="";
   // lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FM");
    lcd.setCursor(SCREEN_WIDTH - 5, 1);
    dtostrf(wValues->temp,5,2,buf);//wValues->temp);
  //Serial.println(buf);
    lcd.print(buf);
    lcd.setCursor(SCREEN_WIDTH - 6, 0);
    lcd.print(wValues->sweepSpeed);
    lcd.setCursor(0, 1);
    //sprintf(buf,"%2d",wValues->freq);
    dtostrf(wValues->freq,6,2,buf);
  lcd.print(buf);
}
void loop()
{
    getTemp();
    if (menustruct == NULL)
    {
      setScreen();
  delay(500);
    }
    
}

void setMenu(byte touchPressed, bool longPressed)
{
    if (touchPressed != BUTTON_VALUE_OK && menustruct == NULL)
    {
        Serial.println("no menu execution");
        return;
    }

    ((*fullMenuContent->action)(touchPressed,longPressed));
    lcd.clear();
  
    //

    //show menu
    lcd.setCursor(0, 0);
    lcd.print('>');
    for (int i = menustruct->levelSelector, y = 0; y < SCREEN_HEIGHT && i < menustruct->menuSz; i++, y++)
    {
        Serial.println((char *)menustruct->menuContent + i * (SCREEN_WIDTH+1));
        lcd.setCursor(1, y);

        lcd.print((char *)menustruct->menuContent + i * (SCREEN_WIDTH+1));
    }
}
void inputKbPressed()
{
    Serial.println("Pushed");
    const byte pushedButton = analogRead(A5);
    delay(2000);
    const byte pushedButton2 = analogRead(A5);

    bool longPush = false;
    if (pushedButton == pushedButton2)
        longPush = true;
    /*
    Serial.print(longPush);
    Serial.print(" ");
    Serial.print(pushedButton);
    Serial.print("->");
    Serial.println(pushedButton2);
    */
    setMenu(pushedButton, longPush);
}



void actionMenu1(byte pressed,bool isLong){
   switch (pressed)
    {
    case BUTTON_VALUE_OK:

    if (menustruct == NULL)
        {
            menustruct = (MenuStruct *)calloc(1, sizeof(MenuStruct));
            menustruct->menuContent = (char **)MENU_LEVEL1;
            menustruct->menuSz = MENU_LEVEL1_SZ;
            menustruct->levelSelector = 0;
            Serial.println("menu enter");
        }
        else if (isLong)
        {
            free(menustruct);
            menustruct = NULL;
            menustruct->levelSelector = 0;
            Serial.println("menu exit");
            return;
        }
        else{
          //insideMenu
          
        }

        break;
    case BUTTON_VALUE_DW:
        menustruct->levelSelector = (menustruct->levelSelector == (menustruct->menuSz - 1) ? 0 : menustruct->levelSelector + 1);
        break;
    case BUTTON_VALUE_UP:
        menustruct->levelSelector = (menustruct->levelSelector == 0 ? menustruct->menuSz - 1 : menustruct->levelSelector - 1);
        break;
    default:
        break;
    }
  Serial.print(isLong);
}
/*  uint8_t levelSelector;
    uint8_t level;
    char **menuContent;
    uint8_t menuSz;
    MenuItem *items;
    MenuItem *selectedContent;
    
    
    struct _MenuItem{
    char ** textContent;
    uint8_t sz;
    void(*action)(byte,bool);
    struct _MenuItem *children;
};
    
    */
void initMenu(){
  MenuItem *level=(MenuItem*)calloc(1,sizeof(MenuItem));
  level->textContent=(char**)MENU_LEVEL1;
  level->sz=MENU_LEVEL1_SZ;
  level->action=actionMenu1;
  fullMenuContent=level;
  
}