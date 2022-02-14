//
// Created by Stephen Stranahan on 2/13/2022.
//

#ifndef AUDIO_SYNTH_RTOS_H
#define AUDIO_SYNTH_RTOS_H

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// DEFINES //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#define MCP4725_I2CADDR_DEFAULT (0x62) ///< Default i2c address

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// FUNCTION PROTOS //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void initRtos(void);

/*******************************************************************************************/

#endif //AUDIO_SYNTH_RTOS_H
