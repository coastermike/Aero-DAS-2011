#ifndef BRAKES_H
#define BRAKES_H

void Brakes_Init(void);
void Set_Speed(unsigned char L);//, unsigned char R);
unsigned float get_Hall_Takeoff_R(void);
unsigned float get_Hall_Takeoff_L(void);
unsigned float get_Hall_Land_L(void);
unsigned float get_Hall_Land_R(void);
void calibrateNoLoad (void);
void calibrateLoad(void);
void takeOff(void);
void PWMInit(void);

#endif
