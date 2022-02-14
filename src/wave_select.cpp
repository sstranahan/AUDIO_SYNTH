//
// Created by Stephen Stranahan on 2/13/2022.
//

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// INCLUDES /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <defs.h>

#include "wave_select.h"
#include "keyboard.h"


/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// GLOBAL DEFINES //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////// TYPES //////////////////////////////////////////////

enum waveSelect_T {
    SQUARE,
    TRIANGLE,
    SINE,
};

unsigned int WS_INPUT_PINS[] = {IN_SQUARE_SEL, IN_TRI_SEL, IN_SINE_SEL};

unsigned int WS_OUTPUT_PINS[] = {OUT_SQUARE_SEL, OUT_TRI_SEL_1, OUT_SINE_SEL_1,
                                 OUT_SINE_SEL_2, OUT_SINE_SEL_3, OUT_SINE_SEL_4,
                                 OUT_SINE_SEL_5};

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// STATIC VARIABLES /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

static waveSelect_T waveSelect;

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// GLOBAL VARIABLES /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////



/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// STATIC FUNCTIONS /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// GLOBAL FUNCTIONS /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void initWaveSelect(void) {

    unsigned int i;

    // Set pin polarities
    for (i = 0; i < NUM_INPUT_PINS; i++){
        pinMode(WS_INPUT_PINS[i], INPUT);
    }
    for (i = 0; i < NUM_OUTPUT_PINS; i++){
        pinMode(WS_OUTPUT_PINS[i], OUTPUT);
    }
}

void TaskWaveSelect( void *pvParameters ) {

    int i = 0;

    while(TRUE) {

        for (i = 0; i < 3; i++) {
            if (digitalRead(WS_INPUT_PINS[i]) == LOW) {
                waveSelect = static_cast<waveSelect_T>(i);
            }
        }

        switch (waveSelect) {

            case SQUARE :
                digitalWrite(OUT_SQUARE_SEL, HIGH);
                digitalWrite(OUT_TRI_SEL_1, LOW);
                digitalWrite(OUT_SINE_SEL_1, LOW);
                digitalWrite(OUT_SINE_SEL_2, LOW);
                digitalWrite(OUT_SINE_SEL_3, LOW);
                digitalWrite(OUT_SINE_SEL_4, LOW);
                digitalWrite(OUT_SINE_SEL_5, LOW);
                break;

            case TRIANGLE :

                switch (getOctaveIndex()) {

                    case 0 :
                        digitalWrite(OUT_SQUARE_SEL, LOW);
                        digitalWrite(OUT_SINE_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_2, LOW);
                        digitalWrite(OUT_SINE_SEL_3, LOW);
                        digitalWrite(OUT_SINE_SEL_4, LOW);
                        digitalWrite(OUT_SINE_SEL_5, LOW);
                        digitalWrite(OUT_TRI_SEL_1, HIGH);
                        break;
                    case 1 :
                        digitalWrite(OUT_SQUARE_SEL, LOW);
                        digitalWrite(OUT_TRI_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_1, HIGH);
                        digitalWrite(OUT_SINE_SEL_2, LOW);
                        digitalWrite(OUT_SINE_SEL_3, LOW);
                        digitalWrite(OUT_SINE_SEL_4, LOW);
                        digitalWrite(OUT_SINE_SEL_5, LOW);
                        break;
                    case 2 :
                        digitalWrite(OUT_SQUARE_SEL, LOW);
                        digitalWrite(OUT_TRI_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_2, HIGH);
                        digitalWrite(OUT_SINE_SEL_3, LOW);
                        digitalWrite(OUT_SINE_SEL_4, LOW);
                        digitalWrite(OUT_SINE_SEL_5, LOW);
                        break;
                    case 3 :
                        digitalWrite(OUT_SQUARE_SEL, LOW);
                        digitalWrite(OUT_TRI_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_2, LOW);
                        digitalWrite(OUT_SINE_SEL_3, HIGH);
                        digitalWrite(OUT_SINE_SEL_4, LOW);
                        digitalWrite(OUT_SINE_SEL_5, LOW);
                        break;
                    case 4 :
                        digitalWrite(OUT_SQUARE_SEL, LOW);
                        digitalWrite(OUT_TRI_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_2, LOW);
                        digitalWrite(OUT_SINE_SEL_3, LOW);
                        digitalWrite(OUT_SINE_SEL_4, HIGH);
                        digitalWrite(OUT_SINE_SEL_5, LOW);
                        break;
                    default :
                        digitalWrite(OUT_SQUARE_SEL, HIGH);
                        digitalWrite(OUT_TRI_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_2, LOW);
                        digitalWrite(OUT_SINE_SEL_3, LOW);
                        digitalWrite(OUT_SINE_SEL_4, LOW);
                        digitalWrite(OUT_SINE_SEL_5, LOW);
                        break;
                }
                break;
            case SINE :

                switch (getOctaveIndex()) {

                    case 0 :
                        digitalWrite(OUT_SQUARE_SEL, LOW);
                        digitalWrite(OUT_TRI_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_1, HIGH);
                        digitalWrite(OUT_SINE_SEL_2, LOW);
                        digitalWrite(OUT_SINE_SEL_3, LOW);
                        digitalWrite(OUT_SINE_SEL_4, LOW);
                        digitalWrite(OUT_SINE_SEL_5, LOW);
                        break;
                    case 1 :
                        digitalWrite(OUT_SQUARE_SEL, LOW);
                        digitalWrite(OUT_TRI_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_2, HIGH);
                        digitalWrite(OUT_SINE_SEL_3, LOW);
                        digitalWrite(OUT_SINE_SEL_4, LOW);
                        digitalWrite(OUT_SINE_SEL_5, LOW);
                        break;
                    case 2 :
                        digitalWrite(OUT_SQUARE_SEL, LOW);
                        digitalWrite(OUT_TRI_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_2, LOW);
                        digitalWrite(OUT_SINE_SEL_3, HIGH);
                        digitalWrite(OUT_SINE_SEL_4, LOW);
                        digitalWrite(OUT_SINE_SEL_5, LOW);
                        break;
                    case 3 :
                        digitalWrite(OUT_SQUARE_SEL, LOW);
                        digitalWrite(OUT_TRI_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_2, LOW);
                        digitalWrite(OUT_SINE_SEL_3, LOW);
                        digitalWrite(OUT_SINE_SEL_4, HIGH);
                        digitalWrite(OUT_SINE_SEL_5, LOW);
                        break;
                    case 4 :
                        digitalWrite(OUT_SQUARE_SEL, LOW);
                        digitalWrite(OUT_TRI_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_2, LOW);
                        digitalWrite(OUT_SINE_SEL_3, LOW);
                        digitalWrite(OUT_SINE_SEL_4, LOW);
                        digitalWrite(OUT_SINE_SEL_5, HIGH);
                        break;
                    default :
                        digitalWrite(OUT_SQUARE_SEL, HIGH);
                        digitalWrite(OUT_TRI_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_1, LOW);
                        digitalWrite(OUT_SINE_SEL_2, LOW);
                        digitalWrite(OUT_SINE_SEL_3, LOW);
                        digitalWrite(OUT_SINE_SEL_4, LOW);
                        digitalWrite(OUT_SINE_SEL_5, LOW);
                        break;
                }
                break;

            default:
                digitalWrite(OUT_SQUARE_SEL, HIGH);
                digitalWrite(OUT_TRI_SEL_1, LOW);
                digitalWrite(OUT_SINE_SEL_1, LOW);
                digitalWrite(OUT_SINE_SEL_2, LOW);
                digitalWrite(OUT_SINE_SEL_3, LOW);
                digitalWrite(OUT_SINE_SEL_4, LOW);
                digitalWrite(OUT_SINE_SEL_5, LOW);
                break;

        }
    }
}
/*******************************************************************************************/

/*******************************************************************************************/