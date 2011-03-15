#include <NewSoftSerial.h>
#include <ParallaxLCD.h>

ParallaxLCD lcd(8,2,16);
#define INLENGTH 16
int landing = 0, takeoff = 0;
char takeofftext[16], landtext[16];
char inCount = 0, temp = 0;
float takeofffloat = 0, landfloat = 0;

void setup()
{
   lcd.setup();
   Serial.begin(4800);
}

void loop()
{
if(Serial.available())
{
    inCount = 0;
    do
    {
       takeofftext[inCount] = Serial.read();     // lees serial ui
     } while (++inCount < INLENGTH);

     lcd.print(takeofftext);
     lcd.cr();
     for(int i = 0; i++; i<INLENGTH)
     {
       takeofftext[i] = 0;
     }
}
delay(1000);
}

