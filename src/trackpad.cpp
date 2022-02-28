//
// Created by Stephen Stranahan on 2/13/2022.
//

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// INCLUDES /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <Arduino.h>

#include "defs.h"
#include "trackpad.h"
#include "lib_includes/I2C.h"
#include "lib_includes/IQS5xx.h"

#include "lib_includes/Arduino_FreeRTOS.h"

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// STATIC VARIABLES /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// GLOBAL VARIABLES /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

// Vars for trackpad comms
uint8_t   Data_Buff[44];
uint16_t  ui16SnapStatus[15], ui16PrevSnap[15];
uint16_t  xCoord;
uint16_t  yCoord;

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// STATIC FUNCTIONS /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// GLOBAL FUNCTIONS /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void initTrackpad(void) {

    pinMode(RDY_PIN, INPUT);    // Trackpad ready pin
    pinMode(RST_PIN, OUTPUT);    // Trackpad reset pin

    digitalWrite(RST_PIN, HIGH);

    // Initialize trackpad
    I2C_Setup();
    //
    // Clear the reset indication bit, so that from here on a reset can be
    // detected if the bit becomes set
    //
    IQS5xx_AcknowledgeReset();
    //
    // Read the version and display on the serial terminal
    //
    IQS5xx_CheckVersion();
    //
    // End the communication window
    //
    Close_Comms();
}

void TaskPollTrackpad( void *pvParameters ) {

    int id;
    uint8_t   ui8TempData[30];
    int i = 0;

    while(TRUE) {

        // RDY_wait();

        // Wait for RDY to be set, this means that a communication window is
        // available.  Then read the 'important' data from the IQS5xx (Address
        // 0x000D to 0x0038).  This includes gesture status, system info flags and
        // all XY data.  NOTE: if less multi-touches are enabled, then less XY
        // data can be read.  It would be good to configure the RDY pin as an
        // interrupt, and then trigger the i2c when the RDY interrupt sees a rising
        // edge.

        I2C_Read(GestureEvents0_adr, &Data_Buff[0], 44);

        Close_Comms();

        Process_XY(&xCoord, &yCoord);

//        Serial.print("X: ");
//        Serial.print(xCoord);
//        Serial.print("  Y: ");
//        Serial.println(yCoord);

        vTaskDelay(1);
    }
}

/*******************************************************************************************/
