//
// Created by Stephen Stranahan on 2/13/2022.
//

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// INCLUDES /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "semphr.h"

#include "rtos.h"

#include "keyboard.h"
#include "dac.h"
#include "rtos.h"
#include "wave_select.h"
#include "trackpad.h"
#include "output.h"

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// STATIC VARIABLES /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

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

void initRtos(void) {

     // initialize serial communication at 115200 bits per second:
     Serial.begin(115200);

     while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
     }

    // Now set up Tasks to run independently.
    xTaskCreate(
            TaskScanKeyboard
            ,  "ScanKey"  // A name just for humans
            ,  512  // This stack size can be checked & adjusted by reading the Stack Highwater
            ,  NULL //Parameters for the task
            ,  4  // Priority, with 5 (config -> MAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
            ,  NULL ); //Task Handle

    xTaskCreate(
            TaskProduceOutput
            ,  "ProduceOut" // A name just for humans
            ,  512  // Stack size
            ,  NULL //Parameters for the task
            ,  3  // Priority
            ,  NULL ); //Task Handle

    xTaskCreate(    // Problem here
            TaskWaveSelect
            ,  "WaveSel" // A name just for humans
            ,  256  // Stack size
            ,  NULL //Parameters for the task
            ,  2  // Priority
            ,  NULL ); //Task Handle

    xTaskCreate(
            TaskPollTrackpad
            ,  "PollTrack" // A name just for humans
            ,  512  // Stack size
            ,  NULL //Parameters for the task
            ,  5  // Priority
            ,  NULL ); //Task Handle
}

/*******************************************************************************************/