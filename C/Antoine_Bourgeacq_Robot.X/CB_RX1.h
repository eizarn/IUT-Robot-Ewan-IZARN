/* 
 * File:   CB_RX1.h
 * Author: TABLE 8
 *
 * Created on 19 novembre 2020, 15:11
 */

#ifndef CB_RX1_H
#define	CB_RX1_H

void SendMessage(unsigned char*, int);
void SendOne(void);

int CB_RX1_RemainingSize(void);
//unsigned char CB_RX1_IsTransmitting(void);

unsigned char CB_RX1_Get(void);
void CB_RX1_Add(unsigned char);


int CB_RX1_GetDataSize(void) ;
int CB_RX1_GetRemainingSize(void);
unsigned char CB_RX1_IsDataAvailable(void);



#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* CB_RX1_H */

