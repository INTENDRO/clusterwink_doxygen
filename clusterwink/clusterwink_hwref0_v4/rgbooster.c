/** ***************************************************************************
 * @file rgbooster.c
 * @brief Software layer to interface the RGBooster hardware
 * 
 * The RGBooster generates the asynchronous protocol needed for WS2812. This
 * relieves the microcontroller (uC) from this time consuming task and enables it
 * to do more important functions at the same time. The uC only needs to output
 * the next byte to the 8 data lines and generate a rising edge on the send pin.
 * The RGBooster saves the data and generates the full protocol by hardware.
 * Upon completion, the RGBooster returns a handshake with a falling edge on
 * the !done/busy pin. Using an external interrupt pin for this !done/busy
 * greatly increases the speed and "multitasking" capability of the system.
 *
 * This file also includes the functions to send data to the RGBooster without
 * the interrupt functionality.
 *
 * @author lopeslen, nosedmar
 * @date 14.11.2017
 *****************************************************************************/


 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include "utils.h"
 #include "rgbooster.h"
 

/** ***************************************************************************
 * @brief Initializes the external interrupt pin INT1 on falling edges
 * 
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
 void INT1_Init(void)
 {
	 EICRA = (1<<ISC11); // interrupt on falling edge
	 EIFR  = (1<<INTF1); // clear flag
	 EIMSK = (1<<INT1);  // INT1 enable
 }


/** ***************************************************************************
 * @brief Initializes the RGBooster data and control pins
 * 
 * Pins required:
 * - 8 data pins
 * - 1 send pin
 * - 1 !done/busy pin
 * 
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
 void initRGBooster(void)
 {
	DDR_DATA_LOW |= DATA_LOW_BITMASK; //RGB DATA LOWER NIBBLE -> OUTPUT
	PORT_DATA_LOW &= ~DATA_LOW_BITMASK; //RGB DATA LOWER NIBBLE -> LOW
	DDR_DATA_HIGH |= DATA_HIGH_BITMASK; //RGB DATA HIGHER NIBBLE -> OUTPUT
	PORT_DATA_HIGH &= ~DATA_HIGH_BITMASK; //RGB DATA HIGHER NIBBLE -> LOW

	DDR_CONTROL |= (1<<SEND); //SEND PIN -> OUTPUT
	PORT_CONTROL &= ~(1<<SEND); //SEND PIN LOW
	DDR_CONTROL &= ~(1<<DONE_BUSY); //DONE BUSY PIN -> INPUT
 }


/** ***************************************************************************
 * @brief Send RGB data to one LED (polling the !done/busy pin)
 * 
 * @param [in] ucRed: red data byte [0-255]
 * @param [in] ucGreen: green data byte [0-255]
 * @param [in] ucBlue: blue data byte [0-255]
 * @return no return value
 *****************************************************************************/
 void SendBit(unsigned char ucRed,unsigned char ucGreen, unsigned char ucBlue)
 {
	 while(PIN_CONTROL & (1<<DONE_BUSY)); //WAIT FOR IDLE

	 PORT_DATA_HIGH = (PORT_DATA_HIGH & ~DATA_HIGH_BITMASK) | (ucGreen & DATA_HIGH_BITMASK);
	 PORT_DATA_LOW = (PORT_DATA_LOW & ~DATA_LOW_BITMASK) | (ucGreen & DATA_LOW_BITMASK);
	 PORT_CONTROL |= (1<<SEND); //SEND
	 PORT_CONTROL &= ~(1<<SEND);

	 while(PIN_CONTROL & (1<<DONE_BUSY)); //WAIT FOR IDLE

	 PORT_DATA_HIGH = (PORT_DATA_HIGH & ~DATA_HIGH_BITMASK) | (ucRed & DATA_HIGH_BITMASK);
	 PORT_DATA_LOW = (PORT_DATA_LOW & ~DATA_LOW_BITMASK) | (ucRed & DATA_LOW_BITMASK);
	 PORT_CONTROL |= (1<<SEND); //SEND
	 PORT_CONTROL &= ~(1<<SEND);

	 while(PIN_CONTROL & (1<<DONE_BUSY)); //WAIT FOR IDLE

	 PORT_DATA_HIGH = (PORT_DATA_HIGH & ~DATA_HIGH_BITMASK) | (ucBlue & DATA_HIGH_BITMASK);
	 PORT_DATA_LOW = (PORT_DATA_LOW & ~DATA_LOW_BITMASK) | (ucBlue & DATA_LOW_BITMASK);
	 PORT_CONTROL |= (1<<SEND); //SEND
	 PORT_CONTROL &= ~(1<<SEND);
 }


/** ***************************************************************************
 * @brief Send RGB data to multiple LEDs (polling the !done/busy pin)
 * 
 * @param [in] ucRed_ptr: pointer to red char array
 * @param [in] ucGreen_ptr: pointer to green char array
 * @param [in] ucBlue_ptr: pointer to blue char array
 * @param [in] ucLength: length of the char arrays (all need to be the same length)
 * @return no return value
 *****************************************************************************/
 void SendStrip(unsigned char* ucRed_ptr, unsigned char* ucGreen_ptr, unsigned char* ucBlue_ptr, unsigned char ucLength)
 {
	 unsigned int i;

	 for(i=0;i<(ucLength);i++)
	 {
		 SendBit(*(ucRed_ptr+i),*(ucGreen_ptr+i),*(ucBlue_ptr+i));
	 }
 }


/** ***************************************************************************
 * @brief Turn off multiple LEDs (polling the !done/busy pin)
 * 
 * @param [in] ucLength: length of the strip to turn off
 * @return no return value
 *****************************************************************************/
 void SendStrip_Off(unsigned char ucLength)
 {
	 unsigned int i;

	 for(i=0;i<(ucLength);i++)
	 {
		 SendBit(0,0,0);
	 }
 }