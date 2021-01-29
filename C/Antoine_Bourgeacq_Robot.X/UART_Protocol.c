#include <xc.h>
#include "UART_Protocol.h"
#include "UART.h"
#include "UART_protocol_vars.h"
#include "IO.h"
#include "PWM.h"
#include "main.h"
#include "CB_TX1.h"

int msgDecodedFunction =0;
int msgDecodedPayloadLength =0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex =0;

unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* msgPayload){
    char actuel, sortie = 0;

    sortie ^= 0xFE;
    sortie ^= (char)((msgFunction & 0xFF00) / 0x100);
    sortie ^= (char)(msgFunction & 0xFF);
    sortie ^= (char)((msgPayloadLength & 0xFF00) / 0x100);
    sortie ^= (char)(msgPayloadLength & 0xFF);
    
    int i;
    for (i=0; i<msgPayloadLength; i++)
    {
        actuel = msgPayload[i];
        sortie ^= actuel;
    }

    return sortie;
            
}

void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, unsigned char* msgPayload){
    unsigned char trame[msgPayloadLength + 6];
    int pos=0;

    trame[pos++] = 0xFE;

    trame[pos++] = (unsigned char)(msgFunction>>8);
    trame[pos++] = (unsigned char)(msgFunction>>0);

    trame[pos++] = (unsigned char)(msgPayloadLength>>8);
    trame[pos++] = (unsigned char)(msgPayloadLength>>0);
    
    int i;
    for (i = 0; i < msgPayloadLength; i++)
    {
        trame[pos++] = msgPayload[i];
    }

    trame[pos++] = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);

    SendMessage/*Direct*/(trame, pos);
}

void UartDecodeMessage (unsigned char c)
{// Fonction prenant en entree un octet et servant a reconstituer les trames
    switch (rcvState)
    {
        case WAITING :
            if (c == 0xFE)
                rcvState = FX_MSB;
            break;

        case FX_MSB :
            msgDecodedFunction = c<<8;
            rcvState = FX_LSB;
            break;

        case FX_LSB :
            msgDecodedFunction += c;
            rcvState = PL_MSB;
            break;

        case PL_MSB :
            msgDecodedPayloadLength = c<<8;
            rcvState = PL_LSB;
            break;

        case PL_LSB :
            msgDecodedPayloadLength += c;
            if(msgDecodedPayloadLength==0)
                rcvState = CHECKSUM;
            else if(msgDecodedPayloadLength>=256)
                rcvState = WAITING;
            else
            {
                rcvState = PAYLOAD;
                msgDecodedPayloadIndex=0;                
            }
            break;

        case PAYLOAD :
            msgDecodedPayload[msgDecodedPayloadIndex++] = c;
            if(msgDecodedPayloadIndex>=msgDecodedPayloadLength)
            {
                rcvState = CHECKSUM;
            }
            break;

        case CHECKSUM:
            receivedChecksum = c;
            calculatedChecksum = UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            if (calculatedChecksum == receivedChecksum)
            {
                isValidChecksum = 1;// Success ,  on a un message  valide
                UartProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            }
            else
            {
                isValidChecksum = 0;
            }
            rcvState = WAITING;
            break;

        default:
            rcvState = WAITING;
            break;
    }
}

void UartProcessDecodedMessage (int function, int payloadLength, unsigned char *payload)
{// Fonction appelee apres le decodage pour executer l?action
//  correspondant au message recu
    switch (function)
    {
        case 0x0080 :
            UartEncodeAndSendMessage (msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            break;
            
        case 0x0020 :
            switch(msgDecodedPayload[0])
            {
                case 0 : // LED Blanche
                    if (msgDecodedPayload[1] < 2)
                        LED_BLANCHE = msgDecodedPayload[1];
                    else
                        LED_BLANCHE = !LED_BLANCHE;
                    break;
                    
                case 1 : // Led Bleue
                    if (msgDecodedPayload[1] < 2)
                        LED_BLEUE = msgDecodedPayload[1];
                    else
                        LED_BLEUE = !LED_BLEUE;
                    break;
                    
                case 2 : // LED Orange
                    if (msgDecodedPayload[1] < 2)
                        LED_ORANGE = msgDecodedPayload[1];
                    else
                        LED_ORANGE = !LED_ORANGE;
                    break;
            }
            break;
            
        case 0x0030 :
            // pk on recevrait ça mdr
            break;
            
        case 0x0040 :
            PWMSetSpeedConsigne((float) msgDecodedPayload[0] * 100.0/255.0, MOTEUR_GAUCHE);
            PWMSetSpeedConsigne((float) msgDecodedPayload[1] * 100.0/255.0, MOTEUR_DROIT);
            break;
            
        case 0x0050 : // etape
            SetRobotState(msgDecodedPayload[0]);
            break;
            
        case 0x0051 : // etape
            SetRobotState(msgDecodedPayload[0]);
            break;
            
        case 0x0052 : // mode automatique
            SetRobotAutoControlState(msgDecodedPayload[0]);
            break;
    }
}
