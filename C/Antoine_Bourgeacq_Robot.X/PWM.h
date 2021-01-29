/* 
 * File:   PWM.h
 * Author: TABLE 6
 *
 * Created on 3 septembre 2020, 09:48
 */

#ifndef PWM_H
#define	PWM_H

void InitPWM(void);
void PWMUpdateSpeed(void);
void PWMSetSpeedConsigne(float vitesseEnPourcents, char moteur);

/*void PWMSetSpeed(float vitesseEnPourcents, unsigned char MoteurLR);*/

#endif