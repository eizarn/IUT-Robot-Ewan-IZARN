#include "IO.h"
#include "QEI.h"
#include <xc.h>
#include <math.h>
#include "Utilities.h"
#include "Robot.h"

float QeiDroitPosition;
float QeiGauchePosition;

float QeiGauchePosition_T_1;
float QeiDroitPosition_T_1;

float delta_d;
float delta_g;

float delta_theta;
float ds;

void InitQEI1()
{
    QEI1IOCbits .SWPAB = 1;  //QEAx and QEBx are  swapped
    QEI1GECL = 0xFFFF;
    QEI1GECH = 0xFFFF;
    QEI1CONbits .QEIEN = 1;  //  Enable QEI Module
}

void InitQEI2()
{
    QEI2IOCbits .SWPAB = 1;  //QEAx and QEBx are  not swapped
    QEI2GECL = 0xFFFF;
    QEI2GECH = 0xFFFF;
    QEI2CONbits .QEIEN = 1;  //  Enable QEI Module
}

void QEIUpdateData()
{
    //On sauvegarde les anciennes valeurs 
    QeiDroitPosition_T_1 = QeiDroitPosition;
    QeiGauchePosition_T_1 = QeiGauchePosition;
    
    //On réactualise les valeurs des positions
    long GEI1RawValue = POS1CNTL;
    GEI1RawValue += ((long)POS1HLD<<16);
    
    long QEI2RawValue = POS2CNTL;
    QEI2RawValue += ((long)POS2HLD<<16);
    
    //Conversion en mm (r\'egl\'e pour la taille des roues codeuses 4,223)
    QeiDroitPosition = 0.01620*GEI1RawValue;
    QeiGauchePosition = 0.01620*QEI2RawValue;
    
    //Calcul des deltas de position
    delta_d = QeiDroitPosition - QeiDroitPosition_T_1;
    delta_g = QeiGauchePosition - QeiGauchePosition_T_1;
    //delta_theta = atan((delta_d-delta_g) / DISTROUES);
    delta_theta = ((delta_d-delta_g) / DISTROUES);
    ds = (delta_d + delta_g)/2;
    
    //Calcul des vitesses
    //Attention à remultiplier par la fréquence d'échantillonnage
    robotState.vitesseDroitFromOdometry = delta_d*FREQ_ECH_QEI;
    robotState.vitesseGaucheFromOdometry = delta_g*FREQ_ECH_QEI;
    robotState.vitesseLineaireFromOdometry = (robotState.vitesseDroitFromOdometry + robotState.vitesseGaucheFromOdometry)/2;
    robotState.vitesseAngulaireFromOdometry = delta_theta*FREQ_ECH_QEI;
   
    //Mise à jour du positionnement terrain à t-1
    robotState.xPosFromOdometry_1 = robotState.xPosFromOdometry;
    robotState.yPosFromOdometry_1 = robotState.yPosFromOdometry;
    robotState.angleRadianFromOdometry_1 =robotState.angleRadianFromOdometry;
    
    //Calcul des positions dans le referentiel du terrain
    robotState.xPosFromOdometry = robotState.xPosFromOdometry_1 + robotState.vitesseLineaireFromOdometry*cos(robotState.angleRadianFromOdometry_1 + delta_theta)/FREQ_ECH_QEI;
    robotState.yPosFromOdometry = robotState.yPosFromOdometry_1 + robotState.vitesseLineaireFromOdometry*sin(robotState.angleRadianFromOdometry_1 + delta_theta)/FREQ_ECH_QEI;
    robotState.angleRadianFromOdometry = robotState.angleRadianFromOdometry_1 + robotState.vitesseAngulaireFromOdometry/FREQ_ECH_QEI;
    if(robotState.angleRadianFromOdometry > PI)
        robotState.angleRadianFromOdometry -= 2*PI;
    if(robotState.angleRadianFromOdometry < -PI)
        robotState.angleRadianFromOdometry +=2*PI;
}