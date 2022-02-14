//
// Created by Stephen Stranahan on 2/13/2022.
//

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// INCLUDES /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <StackArray.h>
#include <stdlib.h>

#include "keyboard.h"
#include "defs.h"

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// STATIC VARIABLES /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// GLOBAL VARIABLES /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

unsigned int KEY_INPUT_PINS[] = {KEY_IN_C0, KEY_IN_CS0, KEY_IN_D0, KEY_IN_DS0, KEY_IN_E0,
                                 KEY_IN_F0, KEY_IN_FS0, KEY_IN_G0, KEY_IN_GS0, KEY_IN_A0,
                                 KEY_IN_AS0, KEY_IN_B0, KEY_IN_C1};

unsigned int KEY_OCTAVE_PINS[] = {KEY_OCT_DWN, KEY_OCT_UP};

unsigned int octaveIdx = 2;           // Default octave is middle octave

bool btnFlags[KEY_NUM_INPUT_KEYS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Flags prevent putting button on stack mult. times

StackArray<unsigned int> btnStack;    // Stack to keep track of order of button inputs

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// STATIC FUNCTIONS /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// GLOBAL FUNCTIONS /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

unsigned int getOctaveIndex(void) {
    return octaveIdx;
}

StackArray<unsigned int> getBtnStack(void) {
    return btnStack;
}

bool* getBtnFlags(void) {
   return &btnFlags[0];
}

void initKeyboard(void) {
    unsigned int i;

    for (i = 0 ; i < KEY_NUM_INPUT_KEYS; i++) {
        pinMode(KEY_INPUT_PINS[i], INPUT);
    }

    for (i = 0; i < KEY_NUM_OCT_PINS; i++) {
        pinMode(KEY_OCTAVE_PINS[i], INPUT);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// KEYBOARD SCAN TASK ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// NOTES : Will push index of button pressed on stack and set flag to avoid redundancy
/////////////////////////////////////////////////////////////////////////////////////////////
void TaskScanKeyboard( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
    unsigned int loopIdx = 0;

    while(TRUE) // A Task shall never return or exit.
    {
        for (loopIdx = 36; loopIdx <= 48; loopIdx++) {
            if (digitalRead(loopIdx) == LOW) {
                if (btnFlags[loopIdx - 36] == 0) {         // If button already on stack, ignore
                    btnFlags[loopIdx - 36] = 1;
                    btnStack.push(loopIdx - 36);
                    Serial.print("Key in: ");
                    Serial.print(loopIdx - 36);
                }
                delay(10);
            } else {
                if (btnFlags[loopIdx - 36] == 1) {
                    btnFlags[loopIdx - 36] = 0;
                }
            }
        }

        if (digitalRead(11) == LOW) {     // Octave shift down
            if (octaveIdx == 0) {
                octaveIdx = 4;
            } else {
                octaveIdx--;
            }
            delay(10);                     // Debounce
        }

        if (digitalRead(12) == LOW) {     // Octave shift up
            if (octaveIdx == 4) {
                octaveIdx = 0;
            } else {
                octaveIdx++;
            }
            delay(10);                     // Debounce
        }

        vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
    }
}
/*******************************************************************************************/

/*******************************************************************************************/