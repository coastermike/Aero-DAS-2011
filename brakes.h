#ifndef BRAKES_H
#define BRAKES_H

void Brakes_Init(void);
unsigned int get_Hall_Takeoff(void);
unsigned int get_Hall_Land(void);
void calibrateNoLoad (void);
void calibrateLoad(void);
void takeOff(void);
void PWMInit(void);

#endif
