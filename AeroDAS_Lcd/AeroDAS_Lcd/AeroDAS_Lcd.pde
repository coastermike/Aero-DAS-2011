#include <stdio.h>
//Aero LCD display

float takeoffvalue = 0, landingvalue = 0;
char takeoff[32], landing[32];
unsigned int temp;
char choose;
unsigned char takeoffH = 0, takeoffL = 0, landH = 0, landL = 0;
unsigned char forceL = 255, forceR = 255;

void setup()
{
  background(0,0,0);
  fill(0,0,0);
  text("Helios DAS", 10, 10, 15);
  text("Takeoff distance:", 20, 60, 15);
  text("Landing distance:", 20, 140, 15);
  Serial.begin(19200);
}

void loop()
{
  if(Serial.available())
  {
    choose = Serial.read();
    if(choose == 0)
    {
      while(!Serial.available());
      takeoffH = Serial.read();
      while(!Serial.available());
      takeoffL = Serial.read();
      takeoffvalue = ((takeoffH << 8) | takeoffL) * 0.05;
      temp = (takeoffvalue - (int)takeoffvalue) * 100;
      sprintf(takeoff, "%0d.%d feet", (int)takeoffvalue, temp);
      stroke(0,0,0);
      rect(10, 80, 300, 50);
      stroke(255,255,255);
      text(takeoff, 20, 90, 15);
    }
    else if(choose == 1)
    {
      while(!Serial.available());
      landH = Serial.read();
      while(!Serial.available());
      landL = Serial.read();
      landingvalue = ((landH << 8) | landL) * 0.05;
      temp = (landingvalue - (int)landingvalue) * 100;
      sprintf(landing,"%0d.%d feet", (int)landingvalue, temp); 
      stroke(0,0,0);
      rect(10, 160, 300, 50);
      stroke(255,255,255);
      text(landing, 20, 170, 15);
    }
    else if(choose == 2)
    {
      while(!Serial.available());
      forceL = Serial.read();
      stroke(0,0,0);
      rect(10, 210, 80, 20);
      stroke(255,255,255);
      text(forceL, 20, 220);
    }
    else if(choose == 3)
    {
      while(!Serial.available());
      forceR = Serial.read();
      stroke(0,0,0);
      rect(90, 210, 80, 20);
      stroke(255,255,255);
      text(forceR, 100, 220);
    }
  } 
}


  
