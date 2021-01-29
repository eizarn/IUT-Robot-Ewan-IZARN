#ifndef TIMER_H
#define	TIMER_H

extern unsigned long timestamp;

void InitTimer23(void);
void InitTimer4(void);
void InitTimer1(void);

void SetFreqTimer1(float freq);
void SetFreqTimer4(float freq);

void resetTimestamp(void);

#endif /* TIMER_H */