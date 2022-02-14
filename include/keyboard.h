//
// Created by Stephen Stranahan on 2/13/2022.
//

#ifndef AUDIO_SYNTH_KEYBOARD_H
#define AUDIO_SYNTH_KEYBOARD_H

#include <StackArray.h>

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// DEFINES //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#define KEY_NUM_INPUT_KEYS  13
#define KEY_NUM_OCT_PINS     2

#define KEY_IN_C0      36
#define KEY_IN_CS0     37
#define KEY_IN_D0      38
#define KEY_IN_DS0     39
#define KEY_IN_E0      40
#define KEY_IN_F0      41
#define KEY_IN_FS0     42
#define KEY_IN_G0      43
#define KEY_IN_GS0     44
#define KEY_IN_A0      45
#define KEY_IN_AS0     46
#define KEY_IN_B0      47
#define KEY_IN_C1      48

#define KEY_OCT_DWN    11
#define KEY_OCT_UP     12

// Primary output signal
#define SPKR_OUT   13

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// FUNCTION PROTOS //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

unsigned int getOctaveIndex(void);

StackArray<unsigned int> getBtnStack(void);

bool* getBtnFlags(void);

void initKeyboard(void);

void TaskScanKeyboard( void *pvParameters );

void initOutput(void);

void TaskProduceOutput( void *pvParameters );

/*******************************************************************************************/
#endif //AUDIO_SYNTH_KEYBOARD_H
