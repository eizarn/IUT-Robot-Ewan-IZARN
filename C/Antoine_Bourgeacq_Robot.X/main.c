#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <libpic30.h>
#include "ChipConfig.h"
#include "ADC.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "Toolbox.h"
#include "Robot.h"
#include "main.h"
#include "UART.h"
#include "CB_RX1.h"
#include "CB_TX1.h"
#include "UART_Protocol.h"

unsigned char stateRobot;
unsigned char nextStateRobot = 0;

int main(void) {
    /****************************************************************************************************/
    // Initialisation des fonctions
    /****************************************************************************************************/

    InitIO();
    InitPWM();
    InitADC1();
    InitTimer1();
    InitTimer4();
    InitOscillator();
    //InitTimer23();
    InitUART();
    
    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
        if (timestamp%1000 == 0)
        {
            unsigned char peilaude[] = {'C', 'O', 'N', 'N', 'A', 'R', 'D', '\0'};
            UartEncodeAndSendMessage(0x0080, 8, peilaude);
        }
        
//        int i;
//        for(i=0; i<CB_RX1_GetDataSize(); i++)
//        {
//            unsigned char c = CB_RX1_Get();
//            UartDecodeMessage(c);
//        }        
    }
}

void SetRobotState(unsigned char state)
{
    stateRobot = state;
    UartUpdateState();
}

void SetRobotAutoControlState(unsigned char state)
{
    unsigned char payload[] = {'B', 'o', 'n', 'j', 'o', 'u', 'r'};
    UartEncodeAndSendMessage(0x80, 7, (unsigned char *)payload);
}

void UartUpdateState(void)
{
    unsigned char stateSent[5] = {stateRobot, timestamp>>24,
                                              timestamp>>16,
                                              timestamp>>8,
                                              timestamp>>0};
    UartEncodeAndSendMessage(0x0050, 5, stateSent);
}

void OperatingSystemLoop(void) {
    if (JACQUES) 
    {
        stateRobot = STATE_ATTENTE;
        LED_BLANCHE = 1;
        resetTimestamp();
    } 
    else 
    {
//        stateRobot = STATE_AVANCE;
        LED_BLANCHE = 0;
    }
    
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;
            UartUpdateState();
            break;

        case STATE_ATTENTE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_AVANCE:
            PWMSetSpeedConsigne(40, MOTEUR_DROIT);
            PWMSetSpeedConsigne(40, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            UartUpdateState();
            break;
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(20, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-5, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            UartUpdateState();
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(-5, MOTEUR_DROIT);
            PWMSetSpeedConsigne(20, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            UartUpdateState();
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(13, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-13, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            UartUpdateState();
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-13, MOTEUR_DROIT);
            PWMSetSpeedConsigne(13, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            UartUpdateState();
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_AVANCE_LENTEMENT:
            PWMSetSpeedConsigne(20, MOTEUR_DROIT);
            PWMSetSpeedConsigne(20, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_LENTEMENT_EN_COURS;
            UartUpdateState();
            break;

        case STATE_AVANCE_LENTEMENT_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_LENTEMENT_DROITE:
            PWMSetSpeedConsigne(20, MOTEUR_DROIT);
            PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_LENTEMENT_DROITE_EN_COURS;
            UartUpdateState();
            break;

        case STATE_TOURNE_LENTEMENT_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_LENTEMENT_GAUCHE:
            PWMSetSpeedConsigne(30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(20, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_LENTEMENT_GAUCHE_EN_COURS;
            UartUpdateState();
            break;

        case STATE_TOURNE_LENTEMENT_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}


void SetNextRobotStateInAutomaticMode() {
//    unsigned char positionObstacle = PAS_D_OBSTACLE;
//
//    //Détermination de la position des obstacles en fonction des télémètres
//    if (robotState.distanceTelemetreDroit < 30 &&
//            robotState.distanceTelemetreCentre > 20 &&
//            robotState.distanceTelemetreGauche > 20) //Obstacle à droite
//        positionObstacle = OBSTACLE_A_DROITE;
//    
//    else if (robotState.distanceTelemetreDroit > 20 &&
//            robotState.distanceTelemetreCentre > 20 &&
//            robotState.distanceTelemetreGauche < 30) //Obstacle à gauche
//        positionObstacle = OBSTACLE_A_GAUCHE;
//    
//    else if (robotState.distanceTelemetreCentre < 20 &&
//             robotState.distanceTelemetreDroit < 20 &&
//            robotState.distanceTelemetreGauche < 20) 
//        positionObstacle = OBSTACLE_EN_FACE; //Obstacle en face
//    
//    else if (robotState.distanceTelemetreCentre < 20 && 
//            robotState.distanceTelemetreDroit < 20  &&
//            robotState.distanceTelemetreGauche > 20 )
//        positionObstacle = OBSTACLE_EN_FACE_DROIT; //Obstacle en face et à droit 
//    
//    else if (robotState.distanceTelemetreCentre < 20 && 
//            robotState.distanceTelemetreGauche < 20 &&
//            robotState.distanceTelemetreDroit > 20 )
//        positionObstacle = OBSTACLE_EN_FACE_GAUCHE; //Obstacle en face et à gauche
//    
//    else if (robotState.distanceTelemetreDroit > 20 &&
//            robotState.distanceTelemetreCentre > 20 &&
//            robotState.distanceTelemetreGauche > 20) //pas d?obstacle
//        positionObstacle = PAS_D_OBSTACLE;
//    
//     else if (robotState.distanceTelemetreDroit < 20 &&
//            robotState.distanceTelemetreCentre > 20 &&
//            robotState.distanceTelemetreGauche < 20) //Problème face
//        positionObstacle = PROBLEME_FACE;
//    
//     else if (robotState.distanceTelemetreDroit > 20 &&
//            robotState.distanceTelemetreCentre < 20 &&
//            robotState.distanceTelemetreGauche > 20) //Problème face 2
//        positionObstacle = PROBLEME_FACE;
//     
//     else if (robotState.distanceTelemetreXDroit < 20 && 
//             robotState.distanceTelemetreDroit > 20 &&
//            robotState.distanceTelemetreCentre > 20 &&
//            robotState.distanceTelemetreGauche > 20 &&
//             robotState.distanceTelemetreXGauche > 20 ) //Obstacle Extreme DROIT
//         positionObstacle = OBSTACLE_X_DROIT;
//     
//     else if (robotState.distanceTelemetreXDroit > 20 && 
//             robotState.distanceTelemetreDroit > 20 &&
//            robotState.distanceTelemetreCentre > 20 &&
//            robotState.distanceTelemetreGauche > 20 &&
//             robotState.distanceTelemetreXGauche < 20 ) //Obstacle Extreme DROIT
//         positionObstacle = OBSTACLE_X_GAUCHE;
//    
//
//    //Détermination de l?état à venir du robot
//    if (positionObstacle == PAS_D_OBSTACLE)
//        nextStateRobot = STATE_AVANCE;
//    else if (positionObstacle == OBSTACLE_A_DROITE)
//        nextStateRobot = STATE_TOURNE_GAUCHE;
//    else if (positionObstacle == OBSTACLE_A_GAUCHE)
//        nextStateRobot = STATE_TOURNE_DROITE;
//    else if (positionObstacle == OBSTACLE_EN_FACE)
//        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
//    else if (positionObstacle == OBSTACLE_EN_FACE_DROIT)
//        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
//    else if (positionObstacle == OBSTACLE_EN_FACE_GAUCHE)
//        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
//    else if (positionObstacle == PROBLEME_FACE)
//        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
//    else if (positionObstacle == OBSTACLE_X_DROIT)
//        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
//    else if (positionObstacle == OBSTACLE_X_GAUCHE)
//        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
//    //Si l?on n?est pas dans la transition de l?étape en cours
//    if (nextStateRobot != stateRobot-1)
//        stateRobot = nextStateRobot; // Il y a un retour en arrière sur la dernière étape
// 
}