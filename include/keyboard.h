//
// Created by Stephen Stranahan on 2/13/2022.
//

#ifndef AUDIO_SYNTH_KEYBOARD_H
#define AUDIO_SYNTH_KEYBOARD_H

#include <StackArray.h>

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// DEFINES //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#define KEY_NUM_INPUT_KEYS  25
#define KEY_NUM_OCT_PINS     2

#define KEY_IN_C0      30
#define KEY_IN_CS0     31
#define KEY_IN_D0      32
#define KEY_IN_DS0     33
#define KEY_IN_E0      34
#define KEY_IN_F0      35
#define KEY_IN_FS0     36
#define KEY_IN_G0      37
#define KEY_IN_GS0     38
#define KEY_IN_A0      39
#define KEY_IN_AS0     40
#define KEY_IN_B0      41
#define KEY_IN_C1      42
#define KEY_IN_CS1     43
#define KEY_IN_D1      44
#define KEY_IN_DS1     45
#define KEY_IN_E1      46
#define KEY_IN_F1      47
#define KEY_IN_FS1     48
#define KEY_IN_G1      49
#define KEY_IN_GS1     50
#define KEY_IN_A1      51
#define KEY_IN_AS1     52
#define KEY_IN_B1      53
#define KEY_IN_C2      A13     // !!

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
