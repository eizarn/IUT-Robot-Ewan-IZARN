#ifndef ROBOT_H
#define ROBOT_H
typedef struct robotStateBITS {
union {

struct {
unsigned char taskEnCours;
float vitesseGaucheConsigne;
float vitesseGaucheCommandeCourante;
float vitesseDroiteConsigne;
float vitesseDroiteCommandeCourante;

unsigned char acceleration;

float distanceTelemetreDroit;
float distanceTelemetreCentre;
float distanceTelemetreGauche;
float distanceTelemetreEGauche;
float distanceTelemetreEDroit;

float distanceTelemetreDroitPrec;
float distanceTelemetreCentrePrec;
float distanceTelemetreGauchePrec;
float distanceTelemetreEGauchePrec;
float distanceTelemetreEDroitPrec;}
;}
;} ROBOT_STATE_BITS;

extern volatile ROBOT_STATE_BITS robotState;
#endif /* ROBOT_H */