// TODO: interface RTOS, I2C transmissions to DACs, octave select and wave select GPIO outs

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// FILE      : arduino_synth_poc.ino -- v1.2.0 -- DEV branch
/////////////////////////////////////////////////////////////////////////////////////////////
// PROJECT   : AUDIO_SYNTHESIZER
/////////////////////////////////////////////////////////////////////////////////////////////
// AUTHOR(S) : Stephen Stranahan, Colin Olesky
/////////////////////////////////////////////////////////////////////////////////////////////
// DESC      : Arduino sketch to drive audio synthesizer primary wave output, read trackpad
////////////// X and Y coordinates, output quantized control voltages to DAC
//////////////
////////////// Uses stack to sote button press index, will remain on stack while button is
////////////// held and pop button index from stack when button is released.
//////////////
////////////// Implements octave shifting functionality, indexes in to output compare timer
////////////// value 2D lookup table on octave and note indeces. Will use timer interrupt to
////////////// output proper frequency square wave

/*******************************************************************************************/
/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// INCLUDES /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
//#include <avr8-stub.h>

#include "keyboard.h"
#include "dac.h"
#include "rtos.h"
#include "wave_select.h"
#include "trackpad.h"
#include "output.h"

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// SETUP RITUAL ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// NOTES : Setup ritual will initialize inputs and outputs, initialize serial terminal,
// and set up timer interrupt subsystem.
/////////////////////////////////////////////////////////////////////////////////////////////

// the setup function runs once when you press reset or power the board
void setup() {

  //debug_init();
  //breakpoint();

  cli();                              // Atomize interrupt system initialization

  initKeyboard();
  initDac();
  initOutput();
  initTrackpad();
  initWaveSelect();
  initRtos();                         // Tasks assigned to RTOS in here

  sei();                              // Enable back global interrupts

  // Now the Task scheduler, which takes over control
  // of scheduling individual Tasks, is automatically started.

}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*******************************************************************************************/
/*********************************** END FILE **********************************************/
/*******************************************************************************************/


// Hello Colin!!! / / / / / / / /asdasd
//hello