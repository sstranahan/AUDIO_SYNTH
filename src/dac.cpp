//
// Created by Stephen Stranahan on 2/13/2022.
//
/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// INCLUDES /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#include "../include/lib_includes/Adafruit_MCP4725.h"

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// STATIC VARIABLES /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// GLOBAL VARIABLES /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

Adafruit_MCP4725 dac1;
Adafruit_MCP4725 dac2;

TwoWire wire;

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// STATIC FUNCTIONS /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// GLOBAL FUNCTIONS /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void initDac(void) {

    // dac1.begin(MCP4725_I2CADDR_DEFAULT);
    // dac2.begin(MCP4725_I2CADDR_DEFAULT);

}

/*******************************************************************************************/