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
#include <stdlib.h>
#include <lib_includes/StackArray.h>
#include "lib_includes/I2C.h"
#include "lib_includes/IQS5xx.h"
#include "defs.h"
#include <lib_includes/Adafruit_MCP4725.h>

// RTOS INCLUDES ////////////////////////////////////////////////////////////////////////////
#include <lib_includes/Arduino_FreeRTOS.h>
#include <lib_includes/semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).
/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// GLOBAL VARIABLES /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

// OCCR Values for C4 - C5 octave - will achieve physically correct
// frequencies for square wave output
const uint16_t FREQ_LOOKUP[5][13] {
  {15288, 14430, 13620, 12857, 12134, 11453, 10811, 10204, 9631, 9091, 8581, 8099, 7645},
  {7645 , 7215 , 6811 , 6428 , 6068 , 5727 , 5405 , 5102 , 4816, 4545, 4290, 4049, 3822},
  {3822 , 3608 , 3405 , 3214 , 3034 , 2863 , 2703 , 2551 , 2408, 2272, 2145, 2025, 1911},
  {1911 , 1804 , 1703 , 1607 , 1517 , 1432 , 1351 , 1275 , 1204, 1136, 1073, 1012, 955},
  {955  , 902  , 851  , 803  , 758  , 716  , 676  , 638  , 602 , 568 , 536 , 506 , 477}
};

bool btnFlags[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Flags prevent putting button on stack mult. times

StackArray<unsigned int> btnStack;    // Stack to keep track of order of button inputs

bool OUT_STATE = false;               // Drives hi/low logic levels for square wave output
// to speaker module


unsigned int octaveIdx = 2;           // Default octave is middle octave
unsigned int stackSize = 0;

unsigned int outPin;

// Vars for trackpad comms
uint8_t   Data_Buff[44];
uint16_t  ui16SnapStatus[15], ui16PrevSnap[15];
uint16_t  xCoord;
uint16_t  yCoord;



TwoWire wire;

/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// SETUP RITUAL ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// NOTES : Setup ritual will initialize inputs and outputs, initialize serial terminal,
////////// and set up timer interrupt subsystem.
/////////////////////////////////////////////////////////////////////////////////////////////
SemaphoreHandle_t xSerialSemaphore;

// define two Tasks for DigitalRead & AnalogRead
void TaskScanKeyboard( void *pvParameters );
void TaskProduceOutput( void *pvParameters );
void TaskWaveSelect( void *pvParameters );
void TaskPollTrackpad( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {

  unsigned int loopIdx = 0;

//  int SDA = PIN_A0;
//  int SCL = PIN_A1;
//
//  Adafruit_MCP4725 mcp1;
//  Adafruit_MCP4725 mcp2;

  cli();                              // Atomize interrupt system initialization

  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }

  //  // Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
  //  // because it is sharing a resource, such as the Serial port.
  //  // Semaphores should only be used whilst the scheduler is running, but we can set it up here.
  //  if ( xSerialSemaphore == NULL )  // Check to confirm that the Serial Semaphore has not already been created.
  //  {
  //    xSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
  //    if ( ( xSerialSemaphore ) != NULL )
  //      xSemaphoreGive( ( xSerialSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  //  }

  pinMode(speakerOutPin, OUTPUT);

  outPin = speakerOutPin;

  pinMode(outPin, OUTPUT);
  digitalWrite(outPin, LOW);
  pinMode(RDY_PIN, INPUT);    // Trackpad ready pin
  pinMode(RST_PIN, OUTPUT);    // Trackpad reset pin

  digitalWrite(RST_PIN, HIGH);

  for (loopIdx = 26; loopIdx <= 32; loopIdx++) {
    pinMode(loopIdx, OUTPUT);
  }

  for (loopIdx = 36; loopIdx <= 48; loopIdx++) {
    pinMode(loopIdx, INPUT);
  }

  for (loopIdx = 22; loopIdx <= 24; loopIdx++) {
    pinMode(loopIdx, INPUT);
  }

  /* First we reset the control register to make sure we start with everything disabled */
  TCCR1A = 0;                       // Reset entire TCCR1A to 0
  TCCR1B = 0;                       // Reset entire TCCR1B to 0

  /*2. We set the prescalar to the desired value by changing the CS10 CS12 and CS12 bits. */
  TCCR1B = B00000010;               // Prescaler = 8

  TIMSK1 |= B00000000;              // Start with output compare disabled, will enable on button input

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

  // dac1.begin(MCP4725_I2CADDR_DEFAULT);
  // dac2.begin(MCP4725_I2CADDR_DEFAULT);

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
  sei();                            // Enable back global interrupts

  // Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
}
/*******************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************/

void loop()
{
  // Empty. Things are done in Tasks.
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// TASKS ///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// KEYBOARD SCAN TASK ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// NOTES : Will push index of button pressed on stack and set flag to avoid redundancy
/////////////////////////////////////////////////////////////////////////////////////////////
void TaskScanKeyboard( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  unsigned int loopIdx = 0;

  for (;;) // A Task shall never return or exit.
  {
    for (loopIdx = 36; loopIdx <= 48; loopIdx++) {
      if (digitalRead(loopIdx) == LOW) {
        if (btnFlags[loopIdx - 36] == 0) {         // If button already on stack, ignore
          btnFlags[loopIdx - 36] = 1;
          btnStack.push(loopIdx - 36);
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
      delay(200);                     // Debounce
    }

    if (digitalRead(12) == LOW) {     // Octave shift up
      if (octaveIdx == 4) {
        octaveIdx = 0;
      } else {
        octaveIdx++;
      }
      delay(200);                     // Debounce
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}
/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// PRODUCE OUTPUT TASK ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// NOTES : Will check stack - if button on stack is held, will set output compare and
////////// enable interrupts, causing ISR to execute and toggle output pin whenever
////////// one half period has elapsed.
//////////
////////// If button not held, will pop value from stack.
/////////////////////////////////////////////////////////////////////////////////////////////
void TaskProduceOutput( void *pvParameters __attribute__((unused)) )  // This is a Task.
{

  unsigned int loopIdx = 0;
  unsigned int btnNumber;

  for (;;)
  {

    stackSize = btnStack.count();

    if (!btnStack.isEmpty()) {
      for (loopIdx = 0; loopIdx <= stackSize; loopIdx++) {
        btnNumber = btnStack.peek();
        if (digitalRead(btnNumber + 36) == HIGH) {
          btnStack.pop();
          btnFlags[btnNumber] = 0;
          stackSize--;
        } else {
          OCR1A = FREQ_LOOKUP[octaveIdx][btnNumber];
          TIMSK1 |= B00000010;        // Set OCIE1A to 1 so we enable compare match A
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
/////////////////////////////// READ WAVE SELECT TASK ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// NOTES :
/////////////////////////////////////////////////////////////////////////////////////////////
void TaskWaveSelect( void *pvParameters ) {

  int i = 0;

  for (;;)
  {
    for (i = 0; i < 3; i++) {
      if (digitalRead(i + 22) == LOW) {
        waveSelect = i;
      }
    }

    switch (waveSelect) {

      case SQUARE :
        digitalWrite(OUT_SQUARE_SEL, HIGH);
        digitalWrite(OUT_TRI_SEL_1, LOW);
        digitalWrite(OUT_SINE_SEL_1, LOW);
        digitalWrite(OUT_SINE_SEL_2, LOW);
        digitalWrite(OUT_SINE_SEL_3, LOW);
        digitalWrite(OUT_SINE_SEL_4, LOW);
        digitalWrite(OUT_SINE_SEL_5, LOW);
        break;

      case TRIANGLE :

        switch (octaveIdx) {

          case 0 :
            digitalWrite(OUT_SQUARE_SEL, LOW);
            digitalWrite(OUT_SINE_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_2, LOW);
            digitalWrite(OUT_SINE_SEL_3, LOW);
            digitalWrite(OUT_SINE_SEL_4, LOW);
            digitalWrite(OUT_SINE_SEL_5, LOW);
            digitalWrite(OUT_TRI_SEL_1, HIGH);
            break;
          case 1 :
            digitalWrite(OUT_SQUARE_SEL, LOW);
            digitalWrite(OUT_TRI_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_1, HIGH);
            digitalWrite(OUT_SINE_SEL_2, LOW);
            digitalWrite(OUT_SINE_SEL_3, LOW);
            digitalWrite(OUT_SINE_SEL_4, LOW);
            digitalWrite(OUT_SINE_SEL_5, LOW);
            break;
          case 2 :
            digitalWrite(OUT_SQUARE_SEL, LOW);
            digitalWrite(OUT_TRI_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_2, HIGH);
            digitalWrite(OUT_SINE_SEL_3, LOW);
            digitalWrite(OUT_SINE_SEL_4, LOW);
            digitalWrite(OUT_SINE_SEL_5, LOW);
            break;
          case 3 :
            digitalWrite(OUT_SQUARE_SEL, LOW);
            digitalWrite(OUT_TRI_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_2, LOW);
            digitalWrite(OUT_SINE_SEL_3, HIGH);
            digitalWrite(OUT_SINE_SEL_4, LOW);
            digitalWrite(OUT_SINE_SEL_5, LOW);
            break;
          case 4 :
            digitalWrite(OUT_SQUARE_SEL, LOW);
            digitalWrite(OUT_TRI_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_2, LOW);
            digitalWrite(OUT_SINE_SEL_3, LOW);
            digitalWrite(OUT_SINE_SEL_4, HIGH);
            digitalWrite(OUT_SINE_SEL_5, LOW);
            break;
          default :
            digitalWrite(OUT_SQUARE_SEL, HIGH);
            digitalWrite(OUT_TRI_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_2, LOW);
            digitalWrite(OUT_SINE_SEL_3, LOW);
            digitalWrite(OUT_SINE_SEL_4, LOW);
            digitalWrite(OUT_SINE_SEL_5, LOW);
            break;
        }
        break;
      case SINE :

        switch (octaveIdx) {

          case 0 :
            digitalWrite(OUT_SQUARE_SEL, LOW);
            digitalWrite(OUT_TRI_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_1, HIGH);
            digitalWrite(OUT_SINE_SEL_2, LOW);
            digitalWrite(OUT_SINE_SEL_3, LOW);
            digitalWrite(OUT_SINE_SEL_4, LOW);
            digitalWrite(OUT_SINE_SEL_5, LOW);
            break;
          case 1 :
            digitalWrite(OUT_SQUARE_SEL, LOW);
            digitalWrite(OUT_TRI_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_2, HIGH);
            digitalWrite(OUT_SINE_SEL_3, LOW);
            digitalWrite(OUT_SINE_SEL_4, LOW);
            digitalWrite(OUT_SINE_SEL_5, LOW);
            break;
          case 2 :
            digitalWrite(OUT_SQUARE_SEL, LOW);
            digitalWrite(OUT_TRI_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_2, LOW);
            digitalWrite(OUT_SINE_SEL_3, HIGH);
            digitalWrite(OUT_SINE_SEL_4, LOW);
            digitalWrite(OUT_SINE_SEL_5, LOW);
            break;
          case 3 :
            digitalWrite(OUT_SQUARE_SEL, LOW);
            digitalWrite(OUT_TRI_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_2, LOW);
            digitalWrite(OUT_SINE_SEL_3, LOW);
            digitalWrite(OUT_SINE_SEL_4, HIGH);
            digitalWrite(OUT_SINE_SEL_5, LOW);
            break;
          case 4 :
            digitalWrite(OUT_SQUARE_SEL, LOW);
            digitalWrite(OUT_TRI_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_2, LOW);
            digitalWrite(OUT_SINE_SEL_3, LOW);
            digitalWrite(OUT_SINE_SEL_4, LOW);
            digitalWrite(OUT_SINE_SEL_5, HIGH);
            break;
          default :
            digitalWrite(OUT_SQUARE_SEL, HIGH);
            digitalWrite(OUT_TRI_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_1, LOW);
            digitalWrite(OUT_SINE_SEL_2, LOW);
            digitalWrite(OUT_SINE_SEL_3, LOW);
            digitalWrite(OUT_SINE_SEL_4, LOW);
            digitalWrite(OUT_SINE_SEL_5, LOW);
            break;
        }
        break;

      default:
        digitalWrite(OUT_SQUARE_SEL, HIGH);
        digitalWrite(OUT_TRI_SEL_1, LOW);
        digitalWrite(OUT_SINE_SEL_1, LOW);
        digitalWrite(OUT_SINE_SEL_2, LOW);
        digitalWrite(OUT_SINE_SEL_3, LOW);
        digitalWrite(OUT_SINE_SEL_4, LOW);
        digitalWrite(OUT_SINE_SEL_5, LOW);
        break;
      
    }
    vTaskDelay(1);
  }
}
/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////   READ TRACKPAD TASK  ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// NOTES :
/////////////////////////////////////////////////////////////////////////////////////////////

void TaskPollTrackpad( void *pvParameters ) {

  int id;
  uint8_t   ui8TempData[30];
  int i = 0;

  for (;;) {

    // RDY_wait();
    //
    // Wait for RDY to be set, this means that a communication window is
    // available.  Then read the 'important' data from the IQS5xx (Address
    // 0x000D to 0x0038).  This includes gesture status, system info flags and
    // all XY data.  NOTE: if less multi-touches are enabled, then less XY
    // data can be read.  It would be good to configure the RDY pin as an
    // interrupt, and then trigger the i2c when the RDY interrupt sees a rising
    // edge.
    //

    I2C_Read(GestureEvents0_adr, &Data_Buff[0], 44);

    Close_Comms();

    Process_XY(&xCoord, &yCoord);

    Serial.print("X: ");
    Serial.print(xCoord);
    Serial.print("  Y: ");
    Serial.println(yCoord);

    vTaskDelay(1);
  }
}
/*******************************************************************************************/
/*******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// OUTPUT ISR ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// NOTES : Simply toggles output pin state every time interrupt timer hits one half period
////////// for frequency of note at top of stack
/////////////////////////////////////////////////////////////////////////////////////////////
ISR(TIMER1_COMPA_vect) {
  TCNT1 = 0;                        //First, set the timer back to 0 so it resets for next interrupt
  OUT_STATE = !OUT_STATE;           //Invert output state

  digitalWrite(outPin, OUT_STATE);  //Write new state to the output pin
}
/*******************************************************************************************/

/*********************************** END FILE **********************************************/
/*******************************************************************************************/
