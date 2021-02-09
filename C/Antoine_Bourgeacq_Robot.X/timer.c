#include <xc.h>
#include "timer.h"
#include "IO.h"
#include "ADC.h"
#include "main.h"
#include "Robot.h"
#include "PWM.h"
#include "QEI.h"

unsigned long timestamp;
unsigned char toggle = 0;

//Initialisation Timer 23
void InitTimer23(void) {
    T3CONbits.TON = 0; // Stop any 16-bit Timer3 operation
    T2CONbits.TON = 0; // Stop any 16/32-bit Timer3 operation
    T2CONbits.T32 = 1; // Enable 32-bit Timer mode
    T2CONbits.TCS = 0; // Select internal instruction cycle clock
    T2CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
    TMR3 = 0x00; // Clear 32-bit Timer (msw)
    TMR2 = 0x00; // Clear 32-bit Timer (lsw)
    PR3 = 0x0262; // Load 32-bit period value (msw)
    PR2 = 0x5A00; // Load 32-bit period value (lsw)
    IPC2bits.T3IP = 0x01; // Set Timer3 Interrupt Priority Level
    IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
    IEC0bits.T3IE = 1; // Enable Timer3 interrupt
    T2CONbits.TON = 1; // Start 32-bit Timer
    /* Example code for Timer3 ISR */
}

//Initialisation Timer 1
void InitTimer1(void) {
    //Timer1 pour horodater les mesures (1ms)
    T1CONbits.TON = 0; // Disable Timer
    T1CONbits.TCS = 0; //clock source = internal clock
    SetFreqTimer1(50);

    IFS0bits.T1IF = 0; // Clear Timer Interrupt Flag
    IEC0bits.T1IE = 1; // Enable Timer interrupt
    T1CONbits.TON = 1; // Enable Timer
}

//Initialisation Timer 4
void InitTimer4(void) {
    //Timer1 pour horodater les mesures (1ms)
    T4CONbits.TON = 0; // Disable Timer
    T4CONbits.TCS = 0; //clock source = internal clock
    SetFreqTimer4(1000); // La fréquence

    IFS1bits.T4IF = 0; // Clear Timer Interrupt Flag
    IEC1bits.T4IE = 1; // Enable Timer interrupt
    T4CONbits.TON = 1; // Enable Timer
}

//Interruption du Timer 23
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
    IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
}

//Interruption du Timer 1
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    IFS0bits.T1IF = 0;
    
//    ADC1StartConversionSequence();
//    ADCUpdateValues();
//    ADCSendValues();
    
    OperatingSystemLoop();
    
    
    
    QEIUpdateData();
    PWMsetSpeedConsignePolaire();
    PWMUpdateSpeed();
    
    SendPositionData();
}

//Interruption du Timer 4 
void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void) {
    IFS1bits.T4IF = 0;
    timestamp++;
}

// Set Frequence Timer 1
void SetFreqTimer1(float freq) {
    T1CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
    if (FCY / freq > 65535) {
        T1CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
        if (FCY / freq / 8 > 65535) {
            T1CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
            if (FCY / freq / 64 > 65535) {
                T1CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                PR1 = (int) (FCY / freq / 256);
            } else
                PR1 = (int) (FCY / freq / 64);
        } else
            PR1 = (int) (FCY / freq / 8);
    } else
        PR1 = (int) (FCY / freq);
}


// Set Frequence Timer 4
void SetFreqTimer4(float freq) {
    T4CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
    if (FCY / freq > 65535) {
        T1CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
        if (FCY / freq / 8 > 65535) {
            T1CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
            if (FCY / freq / 64 > 65535) {
                T1CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                PR1 = (int) (FCY / freq / 256);
            } else
                PR1 = (int) (FCY / freq / 64);
        } else
            PR1 = (int) (FCY / freq / 8);
    } else
        PR1 = (int) (FCY / freq);
}

void resetTimestamp(void)
{
    timestamp = 0;
}