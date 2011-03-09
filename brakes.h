#ifndef BRAKES_H
#define BRAKES_H

void Brakes_Init(void);
void Set_Speed(unsigned char L);//, unsigned char R);
void Reset_Hall_Counts(void);
unsigned float get_Hall_R(void);
unsigned float get_Hall_L(void);

#endif
