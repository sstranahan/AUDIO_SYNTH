//
// Created by Stephen Stranahan on 2/13/2022.
//

#ifndef AUDIO_SYNTH_WAVE_SELECT_H
#define AUDIO_SYNTH_WAVE_SELECT_H

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// DEFINES //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#define NUM_INPUT_PINS   3
#define NUM_OUTPUT_PINS  7

// Wave select rotary input pins
#define IN_SQUARE_SEL   22
#define IN_TRI_SEL      23
#define IN_SINE_SEL     24

// Wave select gate driver output pins
#define OUT_SQUARE_SEL  26
#define OUT_TRI_SEL_1   27
#define OUT_SINE_SEL_1  28
#define OUT_SINE_SEL_2  29
#define OUT_SINE_SEL_3  30
#define OUT_SINE_SEL_4  31
#define OUT_SINE_SEL_5  32

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// FUNCTION PROTOS //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void TaskWaveSelect( void *pvParameters );

void initWaveSelect(void);

/*******************************************************************************************/

#endif //AUDIO_SYNTH_WAVE_SELECT_H
