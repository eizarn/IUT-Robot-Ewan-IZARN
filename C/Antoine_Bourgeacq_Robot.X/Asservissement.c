#include "Asservissement.h"
#include "QEI.h"
#include "timer.h"

double Te = 1 / FREQ_ECH_QEI;

//***********************CORRECTEUR VITSSE ANGULAIRE*************************
double eAng0, eAng1, eAng2; //valeurs de l'entrée du correcteur gauche à t, t-1, t-2
double sAng0, sAng1, sAng2; //valeurs de la sortie du correcteur gauche à t, t-1, t-2

double alphaAng = 0;
double betaAng = 0;
double deltaAng = 0;

void SetUpPiAsservissementVitesseAngulaire(double Ku, double Tu)
{
    double Kp = 0.45;
    double Ti = 0.83;
    double Td = 0;
    double Ki = Kp / Ti;
    double Kd = Kp * Td;
    
    
}

double CorrecteurVitesseAngulaire(double e)
{
    eAng2 = eAng1;
    eAng1 = eAng0;
    eAng0 = e;
    sAng1 = sAng0;
    sAng0 = sAng1 + eAng0 * alphaAng + eAng1 * betaAng + eAng2*deltaAng;
    return sAng0;
}