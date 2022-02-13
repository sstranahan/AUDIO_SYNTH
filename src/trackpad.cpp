//
// Created by Stephen Stranahan on 2/13/2022.
//

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// DEFINES /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#define IQS5xx_ADDR            0x74
#define RDY_PIN               50
#define RST_PIN               51
#define END_WINDOW        (uint16_t)0xEEEE

#define BitIsSet(VAR,Index)   (VAR & (1<<Index)) != 0
#define BitIsNotSet(VAR,Index)  (VAR & (1<<Index)) == 0

#define SetBit(VAR,Index)   VAR |= (1<<Index)
#define ClearBit(VAR,Index)   VAR &= (uint8_t)(~(1<<Index))
