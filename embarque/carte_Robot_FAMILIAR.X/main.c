#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "Robot.h"
#include "PWM.h"
#include "ADC.h"
#include "main.h"
#include "ToolBox.h"
#include "UART.h"


/*A FAIRE
- "sécurité" sur les cotés gauche et droit
 */

unsigned char positionObstacle = PAS_D_OBSTACLE;
unsigned char stateRobot = 0;
unsigned char nextStateRobot = 0;
unsigned char coteDemiTour = 0;
unsigned char coteChoisi = 0;
unsigned long timestamp_1;
unsigned long nombreTours = 0;

//float ddroit =
//float dgauche =


float distanceECotes = 20;
float distanceECotesEloigne = 3;
float distanceCotes = 22;
float distanceCotesEloigne = 30;
float distanceCentre = 23;
float distanceCentreEloigne = 35;
float distanceSecurite = 18;


//Machine à états

void SetNextRobotStateInAutomaticMode() {
    unsigned char positionObstacle = PAS_D_OBSTACLE;

    //Détermination de la position des obstacles en fonction des télémètres
    if (robotState.distanceTelemetreCentre < distanceCentre ||
            ((robotState.distanceTelemetreGauche < distanceSecurite || robotState.distanceTelemetreEGauche < distanceSecurite) &&
            (robotState.distanceTelemetreDroit < distanceSecurite || robotState.distanceTelemetreEDroit < distanceSecurite)))
        positionObstacle = OBSTACLE_EN_FACE;
    else if (robotState.distanceTelemetreDroit < distanceCotes)
        //            (robotState.distanceTelemetreDroit > 500 && 
        //            robotState.distanceTelemetreEDroit < distanceSecurite && robotState.distanceTelemetreCentre < distanceSecurite)) //Obstacle à droite
        positionObstacle = OBSTACLE_CRITIQUE_DROITE;
    else if (robotState.distanceTelemetreGauche < distanceCotes)
        positionObstacle = OBSTACLE_CRITIQUE_GAUCHE;
    else if (robotState.distanceTelemetreDroit < distanceCotes &&
            robotState.distanceTelemetreEDroit < distanceECotes)
        positionObstacle = OBSTACLE_A_DROITE;
    else if (robotState.distanceTelemetreGauche < distanceCotes &&
            robotState.distanceTelemetreEGauche < distanceECotes)
        positionObstacle = OBSTACLE_A_GAUCHE;
    else if (robotState.distanceTelemetreGauche > distanceCotes &&
            robotState.distanceTelemetreCentre > distanceCentre &&
            robotState.distanceTelemetreEGauche < distanceECotes)
        positionObstacle = OBSTACLE_E_GAUCHE;
    else if (robotState.distanceTelemetreDroit > distanceCotes &&
            robotState.distanceTelemetreCentre > distanceCentre &&
            robotState.distanceTelemetreEDroit < distanceECotes)
        positionObstacle = OBSTACLE_E_DROIT;
    else if (robotState.distanceTelemetreCentre < distanceCentreEloigne ||
            robotState.distanceTelemetreDroit < distanceCotesEloigne ||
            robotState.distanceTelemetreGauche < distanceCotesEloigne ||
            robotState.distanceTelemetreEDroit < distanceECotesEloigne ||
            robotState.distanceTelemetreEGauche < distanceECotesEloigne)
        positionObstacle = OBSTACLE_ELOIGNE;
    else if (robotState.distanceTelemetreDroit > distanceCotes &&
            robotState.distanceTelemetreCentre > distanceCentre &&
            robotState.distanceTelemetreGauche > distanceCotes &&
            robotState.distanceTelemetreEGauche > distanceECotes &&
            robotState.distanceTelemetreEDroit > distanceECotes) //pas d?obstacle
        positionObstacle = PAS_D_OBSTACLE;

    //Détermination de l?état à venir du robot
    if (positionObstacle == PAS_D_OBSTACLE) {
        nextStateRobot = STATE_AVANCE;
        if (timestamp_1 - timestamp > 200) {
            coteChoisi = 0;
            nombreTours = 0;
        }
    } else if (positionObstacle == OBSTACLE_EN_FACE || nombreTours > 200) {
        timestamp_1 = timestamp;
        if (Abs(robotState.distanceTelemetreGauche - robotState.distanceTelemetreDroit) > 1 && coteChoisi == 0) {
            if (robotState.distanceTelemetreGauche > robotState.distanceTelemetreDroit) {
                nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
                coteChoisi = 1;
            } else {
                nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
                coteChoisi = 2;
            }
        } else {
            nextStateRobot = 6 + coteChoisi * 2;
        }
    } else if (positionObstacle == OBSTACLE_A_DROITE) {
        nombreTours++;
        nextStateRobot = STATE_TOURNE_GAUCHE;
    } else if (positionObstacle == OBSTACLE_A_GAUCHE) {
        nombreTours++;
        nextStateRobot = STATE_TOURNE_DROITE;
    } else if (positionObstacle == OBSTACLE_ELOIGNE) {
        nextStateRobot = STATE_AVANCE_LENTEMENT;
        if (timestamp_1 - timestamp > 200) {
            coteChoisi = 0;
            nombreTours = 0;
        }
    } else if (positionObstacle == OBSTACLE_CENTRE_DROIT)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    else if (positionObstacle == OBSTACLE_CENTRE_GAUCHE)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
    else if (positionObstacle == OBSTACLE_E_DROIT) {
        nextStateRobot = STATE_TOURNE_LENTEMENT_GAUCHE;
        nombreTours++;
    } else if (positionObstacle == OBSTACLE_E_GAUCHE) {
        nextStateRobot = STATE_TOURNE_LENTEMENT_DROITE;
        nombreTours++;
    } else if (positionObstacle == OBSTACLE_CRITIQUE_DROITE) {
        nombreTours++;
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    } else if (positionObstacle == OBSTACLE_CRITIQUE_GAUCHE) {
        nombreTours++;
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
    }

    //Si l?on n?est pas dans la transition de l?étape en cours
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;
}

void OperatingSystemLoop(void) {
    
    if (JACQUES) {
        resetTimestamp();
        positionObstacle = PAS_D_OBSTACLE;
        stateRobot = 0;
        nextStateRobot = 0;
        coteDemiTour = 0;
        coteChoisi = 0;
        timestamp_1 = 0;
        nombreTours = 0;
        stateRobot = STATE_ATTENTE;
    }
    switch (stateRobot) {
            case STATE_ATTENTE:
                timestamp = 0;
                PWMSetSpeedConsigne(0, MOTEUR_DROIT);
                PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
                stateRobot = STATE_ATTENTE_EN_COURS;

            case STATE_ATTENTE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;

            case STATE_AVANCE:
                PWMSetSpeedConsigne(40, MOTEUR_DROIT);
                PWMSetSpeedConsigne(40, MOTEUR_GAUCHE);
                stateRobot = STATE_AVANCE_EN_COURS;
                break;
            case STATE_AVANCE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;

            case STATE_TOURNE_GAUCHE:
                PWMSetSpeedConsigne(20, MOTEUR_DROIT);
                PWMSetSpeedConsigne(-5, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
                break;
            case STATE_TOURNE_GAUCHE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;

            case STATE_TOURNE_DROITE:
                PWMSetSpeedConsigne(-5, MOTEUR_DROIT);
                PWMSetSpeedConsigne(20, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_DROITE_EN_COURS;
                break;
            case STATE_TOURNE_DROITE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;

            case STATE_TOURNE_SUR_PLACE_GAUCHE:
                PWMSetSpeedConsigne(13, MOTEUR_DROIT);
                PWMSetSpeedConsigne(-13, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
                break;
            case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;

            case STATE_TOURNE_SUR_PLACE_DROITE:
                PWMSetSpeedConsigne(-13, MOTEUR_DROIT);
                PWMSetSpeedConsigne(13, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
                break;
            case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;

            case STATE_AVANCE_LENTEMENT:
                PWMSetSpeedConsigne(20, MOTEUR_DROIT);
                PWMSetSpeedConsigne(20, MOTEUR_GAUCHE);
                stateRobot = STATE_AVANCE_LENTEMENT_EN_COURS;
                break;

            case STATE_AVANCE_LENTEMENT_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;

            case STATE_TOURNE_LENTEMENT_DROITE:
                PWMSetSpeedConsigne(20, MOTEUR_DROIT);
                PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_LENTEMENT_DROITE_EN_COURS;
                break;

            case STATE_TOURNE_LENTEMENT_DROITE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;

            case STATE_TOURNE_LENTEMENT_GAUCHE:
                PWMSetSpeedConsigne(30, MOTEUR_DROIT);
                PWMSetSpeedConsigne(20, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_LENTEMENT_GAUCHE_EN_COURS;
                break;

            case STATE_TOURNE_LENTEMENT_GAUCHE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;

            default:
                stateRobot = STATE_ATTENTE;
                break;
        }
}

int main(void) {

    /****************************************************************************************************/
    //Initialisation de l?oscillateur
    /****************************************************************************************************/
    InitOscillator();
    /****************************************************************************************************/
    // Configuration des entrées sorties
    /****************************************************************************************************/
    InitIO();
    /****************************************************************************************************/
    InitTimer23();
    InitTimer1();
    InitTimer4();
    /****************************************************************************************************/
    InitPWM();
    /****************************************************************************************************/
    InitADC1();
    /****************************************************************************************************/
    InitUART();
    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/


    //    PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
    //    PWMSetSpeedConsigne(15, MOTEUR_DROIT);

    while (1) {
        
        SendMessageDirect((unsigned char*) "trizt", 5);
        __delay32(40000000);
        
        //        if (ADCIsConversionFinished()) {
        //            ADCClearConversionFinishedFlag();
        //            unsigned int *result = ADCGteResult();
        //            ADCValue0 = result[0];  //Droite    20cm == 510
        //            ADCValue1 = result[1];  //Milieu    30cm == 350
        //            ADCValue2 = result[2];  //Gauche    40cm == 270

        //        
        if (ADCIsConversionFinished() == 1) {
            ADCClearConversionFinishedFlag();

            robotState.distanceTelemetreDroitPrec = robotState.distanceTelemetreDroit;
            robotState.distanceTelemetreCentrePrec = robotState.distanceTelemetreCentre;
            robotState.distanceTelemetreGauchePrec = robotState.distanceTelemetreGauche;
            robotState.distanceTelemetreEGauchePrec = robotState.distanceTelemetreEGauche;
            robotState.distanceTelemetreEDroitPrec = robotState.distanceTelemetreEDroit;

            unsigned int *result = ADCGetResult();
            float volts = ((float) result[1]) * 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreDroit = 34 / volts * 5;
            volts = ((float) result[2]) * 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreCentre = 34 / volts * 5;
            volts = ((float) result[4]) * 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreGauche = 34 / volts * 5;
            volts = ((float) result[3]) * 3.3 / 4096;
            robotState.distanceTelemetreEGauche = 34 / volts * 5;
            volts = ((float) result[0]) * 3.3 / 4096;
            robotState.distanceTelemetreEDroit = 34 / volts * 5;

            robotState.distanceTelemetreGauche = robotState.distanceTelemetreGauche / 6;
            robotState.distanceTelemetreDroit = robotState.distanceTelemetreDroit / 6;
            robotState.distanceTelemetreCentre = robotState.distanceTelemetreCentre / 6;
            robotState.distanceTelemetreEDroit = robotState.distanceTelemetreEDroit / 6;
            robotState.distanceTelemetreEGauche = robotState.distanceTelemetreEGauche / 6;

            ADCClearConversionFinishedFlag();

        }
                if (robotState.vitesseDroiteCommandeCourante == 40 &&robotState.vitesseGaucheCommandeCourante == 40)
                        LED_ORANGE = 1; else LED_ORANGE = 0;
                if (JACQUES ||
                   (robotState.vitesseDroiteCommandeCourante == 40 &&robotState.vitesseGaucheCommandeCourante == 40))
                        LED_BLEUE = 1; else LED_BLEUE = 0;
                if (robotState.vitesseDroiteCommandeCourante == 40 &&robotState.vitesseGaucheCommandeCourante == 40)
                        LED_BLANCHE = 1; else LED_BLANCHE = 0;
    }


} // fin main