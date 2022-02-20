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

// OCCR Values for C4 - C5 octave - will achieve physically correct
// frequencies for square wave output
const unsigned int FREQ_LOOKUP[5][13] {
        {15288, 14430, 13620, 12857, 12134, 11453, 10811, 10204, 9631, 9091, 8581, 8099, 7645},
        {7645 , 7215 , 6811 , 6428 , 6068 , 5727 , 5405 , 5102 , 4816, 4545, 4290, 4049, 3822},
        {3822 , 3608 , 3405 , 3214 , 3034 , 2863 , 2703 , 2551 , 2408, 2272, 2145, 2025, 1911},
        {1911 , 1804 , 1703 , 1607 , 1517 , 1432 , 1351 , 1275 , 1204, 1136, 1073, 1012, 955},
        {955  , 902  , 851  , 803  , 758  , 716  , 676  , 638  , 602 , 568 , 536 , 506 , 477}
};

bool OUT_STATE = false;               // Drives hi/low logic levels for square wave
// output to speaker module

unsigned int KEY_INPUT_PINS[] = {KEY_IN_C0, KEY_IN_CS0, KEY_IN_D0, KEY_IN_DS0, KEY_IN_E0,
                                 KEY_IN_F0, KEY_IN_FS0, KEY_IN_G0, KEY_IN_GS0, KEY_IN_A0,
                                 KEY_IN_AS0, KEY_IN_B0, KEY_IN_C1, KEY_IN_CS1, KEY_IN_D1,
                                 KEY_IN_DS1, KEY_IN_E1, KEY_IN_F1, KEY_IN_FS1, KEY_IN_G1,
                                 KEY_IN_GS1, KEY_IN_A1, KEY_IN_AS1, KEY_IN_B1, KEY_IN_C2};

unsigned int KEY_OCTAVE_PINS[] = {KEY_OCT_DWN, KEY_OCT_UP};

unsigned int octaveIdx = 2;           // Default octave is middle octave

bool btnFlags[KEY_NUM_INPUT_KEYS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Flags prevent putting button on stack mult. times

StackArray<unsigned int> btnStack;    // Stack to keep track of order of button inputs

unsigned int stackSize = 0;

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

void initOutput(void) {
    pinMode(SPKR_OUT, OUTPUT);
    digitalWrite(SPKR_OUT, LOW);

    /* First we reset the control register to make sure we start with everything disabled */
    TCCR1A = 0;                       // Reset entire TCCR1A to 0
    TCCR1B = 0;                       // Reset entire TCCR1B to 0

    /*2. We set the prescalar to the desired value by changing the CS10 CS12 and CS12 bits. */
    TCCR1B = B00000010;               // Prescaler = 8
    TIMSK1 |= B00000000;              // Start with output compare disabled, will enable on button input
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
        for (loopIdx = 0; loopIdx < KEY_NUM_INPUT_KEYS; loopIdx++) {
            if (digitalRead(KEY_INPUT_PINS[loopIdx]) == LOW) {
                if (btnFlags[loopIdx] == 0) {         // If button already on stack, ignore
                    btnFlags[loopIdx] = 1;
                    btnStack.push(loopIdx);
                    Serial.print("Key in: ");
                    Serial.print(loopIdx);
                }
                delay(10);
            } else {
                if (btnFlags[loopIdx] == 1) {
                    btnFlags[loopIdx] = 0;
                }
            }
        }

        if (digitalRead(KEY_OCT_DWN) == LOW) {     // Octave shift down
            if (octaveIdx == 0) {
                octaveIdx = 4;
            } else {
                octaveIdx--;
            }
            delay(10);                     // Debounce
        }

        if (digitalRead(KEY_OCT_UP) == LOW) {     // Octave shift up
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

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// PRODUCE OUTPUT TASK ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// NOTES : Will check stack - if button on stack is held, will set output compare and
// enable interrupts, causing ISR to execute and toggle output pin whenever
// one half period has elapsed.
//
// If button not held, will pop value from stack.
/////////////////////////////////////////////////////////////////////////////////////////////

void TaskProduceOutput( void *pvParameters __attribute__((unused)) ) {

    unsigned int loopIdx = 0;
    unsigned int btnNumber;

    while(TRUE) {

        unsigned int i;

//        outBtnStack = getBtnStack();                 // Copy in note stack - dont need since global to this file
//        btnFlagPtr = getBtnFlags();

//        for (i = 0; i < KEY_NUM_INPUT_KEYS; i++) {   // Copy in flag array
//            outBtnFlags[i] = *btnFlagPtr;
//            btnFlagPtr++;
//        }

        stackSize = btnStack.count();

        if (!btnStack.isEmpty()) {
            for (loopIdx = 0; loopIdx <= stackSize; loopIdx++) {
                btnNumber = btnStack.peek();
                if (digitalRead(KEY_INPUT_PINS[btnNumber]) == HIGH) {
                    Serial.println("Popping: ");
                    Serial.println(loopIdx);
                    btnStack.pop();
                    btnFlags[btnNumber] = 0;
                    stackSize--;
                } else {
                    OCR1A = FREQ_LOOKUP[getOctaveIndex()][btnNumber];
                    TIMSK1 |= B00000010;             // Set OCIE1A to 1 so we enable compare match A
                }
            }
        }

        //Disable output if no buttons held
        if (btnStack.isEmpty()) {
            TIMSK1 &= B00000000;   // Turn off timer system
        }
    }
}
/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// OUTPUT ISR ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// NOTES : Simply toggles output pin state every time interrupt timer hits one half period
// for frequency of note at top of stack
/////////////////////////////////////////////////////////////////////////////////////////////
ISR(TIMER1_COMPA_vect) {
    TCNT1 = 0;                        //First, set the timer back to 0 so it resets for next interrupt
    OUT_STATE = !OUT_STATE;           //Invert output state

    digitalWrite(SPKR_OUT, OUT_STATE);  //Write new state to the output pin
}
/*******************************************************************************************/

/*******************************************************************************************/

/*******************************************************************************************/