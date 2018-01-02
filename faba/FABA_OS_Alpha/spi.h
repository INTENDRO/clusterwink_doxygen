/** ***************************************************************************
 * @file spi.h
 * @brief spi.h is the header file of spi.c which is for interfacing the SPI interface
 *
 * Fetches received data form SPI.
 * A FSM checks if the received data is valid.
 * If data is valid, it's saved in the internal storrage system
 *
 * @author Manuel Boebel, Marcel Schreiner
 * @date 11.11.2017
 *****************************************************************************/

#ifndef _SPI_H_
#define _SPI_H_

#include <avr/io.h>

//**************************************************************************************
//***                                    defines                                     ***
//**************************************************************************************

// SPI FSM states
#define			STATE_IDLE				0		///< State "idle" used for FSM in SPI receiver 
#define			STATE_ADDR_RECEIVED		1   	///< State "address received" used for FSM in SPI receiver
#define			STATE_RD_RECEIVED		2   	///< State "color red reveived" used for FSM in SPI receiver
#define			STATE_GN_RECEIVED		3   	///< State "color green reveived" used for FSM in SPI receiver
#define			STATE_BL_RECEIVED		4   	///< State "color blue reveived" used for FSM in SPI receiver
#define			STATE_WH_RECEIVED		5   	///< State "color white reveived" used for FSM in SPI receiver

void FetchSpi(void);


//**************************************************************************************
//***                                   variables                                    ***
//**************************************************************************************

// State FSM SPI
uint8_t fsmState;				///< Current State of SPI FSM
                                
// Flags SPI                    
uint8_t flagColorChanged;       ///< True if new color received
uint8_t flagModeChanged;        ///< True if new mode received


#endif /* _SPI_H_ */
