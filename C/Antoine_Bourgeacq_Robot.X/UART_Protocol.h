/* 
 * File:   UART_Protocol.h
 * Author: Table2
 *
 * Created on 7 octobre 2020, 12:44
 */

#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H

unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char *msgPayload);
unsigned char CalculateCheckSum(int msgFunction, int msgPayloadLength, unsigned char *msgPayload);
void UartDecodeMessage(unsigned char c);
void UartProcessDecodedMessage(int function, int payloadLength, unsigned char *payload);
void UartEncodeAndSendMessage (int msgFunction, int msgPayloadLength, unsigned char *msgPayload);

#endif	/* UART_PROTOCOL_H */

