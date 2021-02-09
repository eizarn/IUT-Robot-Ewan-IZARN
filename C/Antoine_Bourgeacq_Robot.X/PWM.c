//Partie PWM
#include <xc.h> // library xc.h inclut tous les uC
#include "IO.h"
#include "PWM.h"
#include "Robot.h"
#include "ToolBox.h"

#define PWMPER 40.0
unsigned char acceleration = 3;
unsigned char deceleration = 4;//14
unsigned char accD;
unsigned char accG;

#define COEFF_VITESSE_LINEAIRE_PERCENT 250/25
#define COEFF_VITESSE_ANGULAIRE_PERCENT 400/50

void InitPWM(void)
{
    PTCON2bits.PCLKDIV = 0b000; //Divide by 1
    PTPER = 100*PWMPER; //Période en pourcentage

    //Réglage PWM moteur 1 sur hacheur 1
    IOCON1bits.POLH = 1; //High = 1 and active on low =0
    IOCON1bits.POLL = 1; //High = 1
    IOCON1bits.PMOD = 0b01; //Set PWM Mode to Redundant
    FCLCON1 = 0x0003; //Désactive la gestion des faults

    //Reglage PWM moteur 2 sur hacheur 6
    IOCON6bits.POLH = 1; //High = 1
    IOCON6bits.POLL = 1; //High = 1
    IOCON6bits.PMOD = 0b01; //Set PWM Mode to Redundant
    FCLCON6 = 0x0003; //Désactive la gestion des faults

    /* Enable PWM Module */
    PTCONbits.PTEN = 1;
}

void PWMSetSpeed(float vitesseEnPourcents, char moteur) // Ne pas utiliser car changement brutal de vitesse                                                                  à moins que...
{
    if (moteur == MOTEUR_GAUCHE) {
        if (vitesseEnPourcents > 0) {
            robotState.vitesseGaucheCommandeCourante = vitesseEnPourcents;
            MOTEUR_GAUCHE_ENL = 0; //Pilotage de la pin en mode IO
            MOTEUR_GAUCHE_INL = 1; //Mise à 1 de la pin
            MOTEUR_GAUCHE_ENH = 1; //Pilotage de la pin en mode PWM
            MOTEUR_GAUCHE_DUTY_CYCLE = Abs(robotState.vitesseGaucheCommandeCourante*PWMPER);
        } else {
            robotState.vitesseGaucheCommandeCourante = vitesseEnPourcents;
            MOTEUR_GAUCHE_ENH = 0; //Pilotage de la pin en mode IO
            MOTEUR_GAUCHE_INH = 1; //Mise à 1 de la pin
            MOTEUR_GAUCHE_ENL = 1; //Pilotage de la pin en mode PWM
            MOTEUR_GAUCHE_DUTY_CYCLE = Abs(robotState.vitesseGaucheCommandeCourante*PWMPER);
        }
        
    } else {
        if (vitesseEnPourcents < 0) {
            robotState.vitesseDroiteCommandeCourante = vitesseEnPourcents;
            MOTEUR_DROIT_ENL = 0; //Pilotage de la pin en mode IO
            MOTEUR_DROIT_INL = 1; //Mise à 1 de la pin
            MOTEUR_DROIT_ENH = 1; //Pilotage de la pin en mode PWM
            MOTEUR_DROIT_DUTY_CYCLE = Abs(robotState.vitesseDroiteCommandeCourante*PWMPER);
        } else {
            robotState.vitesseDroiteCommandeCourante = vitesseEnPourcents;
            MOTEUR_DROIT_ENH = 0; //Pilotage de la pin en mode IO
            MOTEUR_DROIT_INH = 1; //Mise à 1 de la pin
            MOTEUR_DROIT_ENL = 1; //Pilotage de la pin en mode PWM
            MOTEUR_DROIT_DUTY_CYCLE = Abs(robotState.vitesseDroiteCommandeCourante*PWMPER);
        }
    }
}

void PWMSetSpeedConsigne(float vitesseEnPourcents, char moteur) {
    if (moteur == MOTEUR_GAUCHE) robotState.vitesseGaucheConsigne = vitesseEnPourcents;
    else robotState.vitesseDroiteConsigne = vitesseEnPourcents;
}

void PWMUpdateSpeed()
{
    if (JACQUES){
        PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
        PWMSetSpeedConsigne(0, MOTEUR_DROIT);
    }
    
    // Cette fonction est appelée sur timer et permet de suivre des rampes d?accélération
   if (Abs(robotState.vitesseDroiteCommandeCourante) < Abs(robotState.vitesseDroiteConsigne))
       accD = acceleration;
   else
       accD = deceleration;

   if (robotState.vitesseDroiteCommandeCourante < robotState.vitesseDroiteConsigne)
       robotState.vitesseDroiteCommandeCourante = Min(
           robotState.vitesseDroiteCommandeCourante + accD,
           robotState.vitesseDroiteConsigne);
   if (robotState.vitesseDroiteCommandeCourante > robotState.vitesseDroiteConsigne)
       robotState.vitesseDroiteCommandeCourante = Max(
           (robotState.vitesseDroiteCommandeCourante - accD),
           robotState.vitesseDroiteConsigne);

   if (robotState.vitesseDroiteCommandeCourante > 0)
   {
       MOTEUR_DROIT_ENH = 0; //pilotage de la pin en mode IO
       MOTEUR_DROIT_INH = 1; //Mise à 1 de la pin
       MOTEUR_DROIT_ENL = 1; //Pilotage de la pin en mode PWM
   }
   else
   {
       MOTEUR_DROIT_ENL = 0; //pilotage de la pin en mode IO
       MOTEUR_DROIT_INL = 1; //Mise à 1 de la pin
       MOTEUR_DROIT_ENH = 1; //Pilotage de la pin en mode PWM
   }
   MOTEUR_DROIT_DUTY_CYCLE = Abs(robotState.vitesseDroiteCommandeCourante)*PWMPER;

   if (Abs(robotState.vitesseGaucheCommandeCourante) < Abs(robotState.vitesseGaucheConsigne))
       accG = acceleration;
   else
       accG = deceleration;

   if (robotState.vitesseGaucheCommandeCourante < robotState.vitesseGaucheConsigne)
       robotState.vitesseGaucheCommandeCourante = Min(
           robotState.vitesseGaucheCommandeCourante + accG,
           robotState.vitesseGaucheConsigne);
   if (robotState.vitesseGaucheCommandeCourante > robotState.vitesseGaucheConsigne)
       robotState.vitesseGaucheCommandeCourante = Max(
           robotState.vitesseGaucheCommandeCourante - accG,
           robotState.vitesseGaucheConsigne);

   if (robotState.vitesseGaucheCommandeCourante > 0)
   {
       MOTEUR_GAUCHE_ENL = 0; //pilotage de la pin en mode IO
       MOTEUR_GAUCHE_INL = 1; //Mise à 1 de la pin
       MOTEUR_GAUCHE_ENH = 1; //Pilotage de la pin en mode PWM
   }
   else
   {
       MOTEUR_GAUCHE_ENH = 0; //pilotage de la pin en mode IO
       MOTEUR_GAUCHE_INH = 1; //Mise à 1 de la pin
       MOTEUR_GAUCHE_ENL = 1; //Pilotage de la pin en mode PWM
   }
    MOTEUR_GAUCHE_DUTY_CYCLE = Abs(robotState.vitesseGaucheCommandeCourante) * PWMPER;   
}

void PWMsetSpeedConsignePolaire(){
    //Correction Angulaire
    double erreurVitesseAngulaire = robotState.vitesseAngulaireFromOdometry - robotState.vitesseAngulaireConsigne;
//    double sortieCorrectionAngulaire = ;
    double correctionVitesseAngulaire = robotState.vitesseAngulaireConsigne;
    double correctionVitesseAngulairePourcent = correctionVitesseAngulaire * COEFF_VITESSE_ANGULAIRE_PERCENT;
    
    //Correction Linéaire
    double erreurVitesseLineaire = robotState.vitesseLineaireFromOdometry - robotState.vitesseLineaireConsigne;
//    double sortieCorrectionLineaire = ;
    double correctionVitesseLineaire = robotState.vitesseLineaireConsigne; 
    double correctionVitesseLineairePourcent = correctionVitesseLineaire * COEFF_VITESSE_LINEAIRE_PERCENT;
    
    //Génération des consignes droite et gauche 
    robotState.vitesseDroiteConsigne = correctionVitesseLineairePourcent + correctionVitesseAngulairePourcent;
    robotState.vitesseDroiteConsigne = LimitToInterval(robotState.vitesseDroiteConsigne, -100, 100);
    robotState.vitesseGaucheConsigne = correctionVitesseLineairePourcent - correctionVitesseAngulairePourcent;
    robotState.vitesseGaucheConsigne =  LimitToInterval(robotState.vitesseGaucheConsigne, -100, 100);
}