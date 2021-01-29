/* 
 * File:   CB_TX1.h
 * Author: TABLE 8
 *
 * Created on 17 novembre 2020, 09:14
 */

#ifndef CB_TX1_H
#define	CB_TX1_H

void SendMessage(unsigned char*, int);
void CB_TX1_Add(unsigned char);
unsigned char CB_TX1_Get(void);
unsigned char CB_TX1_IsTranmitting(void);
int CB_TX1_RemainingSize(void);
int CB_TX1_GetDataSize();
void SendOne();

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* CB_TX1_H */

