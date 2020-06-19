#ifndef IO_H
#define IO_H

//Affectation des pins des LEDS
#define LED_BLANCHE _LATC10 
#define LED_ORANGE _LATG7
#define LED_BLEUE _LATG6
#define MOTEUR1_IN1 _LATB14
#define MOTEUR1_IN2 _LATB15
#define MOTEUR6_IN1 _LATC6
#define MOTEUR6_IN2 _LATC7

//Configuration spécifique du moteur gauche
#define MOTEUR_GAUCHE_INH MOTEUR1_IN1
#define MOTEUR_GAUCHE_INL MOTEUR1_IN2
#define MOTEUR_GAUCHE_ENL IOCON1bits.PENL
#define MOTEUR_GAUCHE_ENH IOCON1bits.PENH
#define MOTEUR_GAUCHE_DUTY_CYCLE PDC1

#define MOTEUR_DROIT_INH MOTEUR6_IN1
#define MOTEUR_DROIT_INL MOTEUR6_IN2
#define MOTEUR_DROIT_ENL IOCON6bits.PENL
#define MOTEUR_DROIT_ENH IOCON6bits.PENH
#define MOTEUR_DROIT_DUTY_CYCLE PDC6

#define JACQUES _RE12
// Prototypes fonctions
void InitIO();

#endif /* IO_H */