/* 
 * File:   UART_protocol_vars.h
 * Author: TP-EO-5
 *
 * Created on 13 octobre 2020, 15:24
 */

#ifndef UART_PROTOCOL_VARS_H
#define	UART_PROTOCOL_VARS_H

#define WAITING 0
#define FX_MSB 1
#define FX_LSB 2
#define PL_MSB 3
#define PL_LSB 4
#define PAYLOAD 5
#define CHECKSUM 6

int rcvState = WAITING;

int isValidSOF;
char lastInvalidChar;
int msgDecodedFunction;
int msgDecodedPayloadLength;
int msgDecodedPayloadIndex;
unsigned char msgDecodedPayload[128];
unsigned char receivedChecksum;
unsigned char calculatedChecksum;
int isValidChecksum;
int decodeFinishedFlag;

#endif	/* UART_PROTOCOL_VARS_H */

