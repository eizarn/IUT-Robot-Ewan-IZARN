#ifndef QEI_H
#define	QEI_H

//#define DISTROUES 281.2 //DIST
#define FREQ_ECH_QEI 250
#define DIAM_CODEUR_ROUE 0.0426 //en m
#define POINT_TO_METER  (PI*DIAM_CODEUR_ROUE)/8192
#define DISTROUES 0.2812

void InitQEI1();
void InitQEI2();
void QEIUpdateData();


void SendPositionData();




#endif	/* QEI_H */

