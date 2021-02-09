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
            
            float distanceTelemetreDroit;
            float distanceTelemetreXDroit;
            float distanceTelemetreCentre;
            float distanceTelemetreGauche;
            float distanceTelemetreXGauche;
            
            float vitesseDroitFromOdometry;
            float vitesseGaucheFromOdometry;
            float vitesseLineaireFromOdometry;
            float vitesseAngulaireFromOdometry;
            
            float xPosFromOdometry;
            float xPosFromOdometry_1;
            float yPosFromOdometry;
            float yPosFromOdometry_1;
            float angleRadianFromOdometry;
            float angleRadianFromOdometry_1;
            
            float vitesseAngulaireConsigne;
            float vitesseLineaireConsigne;
        };
    };
} ROBOT_STATE_BITS;

extern volatile ROBOT_STATE_BITS robotState;
#endif /* ROBOT_H */