/** ***************************************************************************
 * @file spi.c
 * @brief Initializing the SPI slave interface
 *
 * The SPI interface is used to exchange data with the Raspberry Pi (RPi).
 * Only the slave functionality is implemented since the RPi will always be
 * the master. The Interrupt Service Routine (ISR) in the main.c file processes
 * the received data.
 *
 * @author lopeslen, nosedmar
 * @date 29.12.2017
 *****************************************************************************/


#include <avr/io.h>
#include "spi.h"


/** ***************************************************************************
 * @brief Initializes the SPI peripheral as slave
 * 
 * Enables the SPI peripheral and its interrupt source. Pin directions of 
 * !SS, SCK and MOSI are automatically overriden to "INPUT". MISO is user
 * defined and is set as an output to be able to send data back to the master.
 * SCK is idle low and data is sampled on the rising edge of SCK.
 * 
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void SPISlave_Init(void)
{
	volatile unsigned char ucTemp;
	SPCR = (1<<SPIE) | (1<<SPE);	// SPI interrupt enable | SPI enable
	SPSR = 0x00;					// normal speed
	ucTemp = SPDR;					//clear read buffer (not sure if necessary)
	ucTemp = SPDR;
	SPDR = 0;						//clear write buffer (not sure if necessary)
	
	DDRB |= (1<<PINB4);				// MISO needs to be an output
}