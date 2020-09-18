/*
 * File:   IO.c
 */
#include <xc.h>

#include "IO.h"

void InitIO()
{

    // IMPORTANT : d�sactiver les entr�es analogiques, sinon on perd les entr�es num�riques

    ANSELA = 0; // 0 desactive
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;
    ANSELF = 0;
    ANSELG = 0;

    //********** Configuration des sorties : _TRISxx = 0 ********************************

    // LED

    _TRISC10 = 0; // LED Orange

    _TRISG6 = 0;  // LED Blanche

    _TRISG7 = 0;  // LED Bleue

        

    // Moteurs 
    
    _TRISB14 = 0; // moteur G 1
    _TRISB15 = 0; // moteur G 2

    _TRISC6 = 0; // moteur D 1
    _TRISC7 = 0; // moteur D 2
    
    //********** Configuration des entr�es : _TRISxx = 1 ********************************   
    
    _TRISE12 = 1;

    //********** Liaison Serie *************************************************
    _U1RXR = 36;
    _RP36R = 0b00001;    

}