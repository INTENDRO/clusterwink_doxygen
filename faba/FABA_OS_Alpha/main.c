/*************************************************************************//**
 * @mainpage FABA Ambiance Alpha
 *
 * This project is about building a moodlight. A moodlight is a light for 
 * decoration, which can change its color. The idea of FABA Ambiance is to 
 * design a modular light consisting of many panels which can be connected.
 * They can all show the same color, but it’s also possible to display a 
 * color gradient or do a smooth transition fade. In the end, it should be 
 * possible to control the whole light with a smartphone or maybe even by a 
 * voice assistant (for example google assistant) by integrating the FABA Ambiance 
 * into a smart home system.
 *
 * The project is realized as a part of the subject <b>Electonics-Project (ETP)</b>
 * starting with fall term 2017
 * in the <b>second year of the electronics engineering curriculum</b>
 * at the School of Engineering of the Zurich University of Applied Sciences.
 * 
 * <img src="../../doxygen_resources/FABA_AMBIANCE_ORIGINAL.jpg" height="450" width="800" border="2">
 *
 * Color is mixed with four different leds (white, red, green and blue)
 * Each module contains 15 LEDs and up to 100 moudules can be attached.
 *
 * <br>
 * <h2>Software Tests</h2>
 * To test the software the following tests were made.
 * <br>
 * 	
 * <table style="width:600px" border="2px" >
 *   <tr>
 *     <th>Mode</th>
 *     <th>Description</th> 
 *     <th>Expected result</th>
 *     <th>Okay?</th>
 *   </tr>
 *   <tr>
 *     <td>Off</td>
 *     <td>Smith</td> 
 *     <td>50</td> 
 *     <td>94</td>
 *   </tr>
 *   <tr>
 *     <td>Individual Color</td>
 *     <td>Jackson</td> 
 *     <td>94</td> 
 *     <td>94</td>
 *   </tr>
 *	 <tr>
 *     <td>One Color</td>
 *     <td>Jackson</td> 
 *     <td>94</td> 
 *     <td>94</td>
 *   </tr>
 *   <tr>
 *     <td>Fade</td>
 *     <td>Jackson</td> 
 *     <td>94</td> 
 *     <td>94</td>
 *   </tr>
 *   <tr>
 *     <td>Gradient</td>
 *     <td>Jackson</td> 
 *     <td>94</td> 
 *     <td>94</td>
 *   </tr>
 * </table>
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 *
 * Hardware:  Atmega 328p Development Board 5V 16MHz
 * Device:    Atmega 328p
 *
 * @author Manuel Boebel, Marcel Schreiner
 * @date 09.10.2017
 *****************************************************************************/
 
 
 /** ***************************************************************************
 * @file main.c
 * @brief main software file
 *
 * @author Manuel Boebel, Marcel Schreiner
 * @date 02.11.2017
 *****************************************************************************/


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "defines.h"
#include "inits.h"
#include "interrupts.h"
#include "spi.h"
#include "colors.h"


//*****************************************************************************
//***                           function "main"                             ***
//*****************************************************************************


/** ***************************************************************************
 * @brief setup process and main while(1) loop
 * 
 * call initialization functions, setup generals, call fetchSPI function and 
 * @n if mode or color changed --> call calculateColorAndFillBuffer function and
 * update timer properties
 * @n update flags
 *
 * @param [void] no input
 * @return 0 after execution
 *****************************************************************************/
int main(void)
{
	// Do initialization stuff
	PortInit();
	SPI_SlaveInit();
	InterruptInit();
	TimerInit();
	UsartInit();
	BufferInit();
	
	// General interrupt enable
	sei();
	
	mode = MODE_OFF;
	
	//counter deaktivieren --> prescaler alle auf 0
	TCCR1B  &= 0xF8;
	
	flagColorChanged = TRUE;
	
	while (1)
    {
		// This function fetches data from SPI and saves it
		FetchSpi();
		
		// Calculate colors
		if(flagColorChanged||flagModeChanged)
		{
			calculateColorAndFillBuffer();
			
			//if mode == static
			if (mode != MODE_TWOCOLOR_FADER)
			{
				//counter deaktivieren --> prescaler alle auf 0
				TCCR1B  &= 0xF8;
				// reset buffer offset
				bufferOffset = 0;	
				// Call ISR for sending led Protocol
				INT1_vect();
			}
			else
			{
				// counter aktivieren --> prescaler auf 8 (CS11)
				TCCR1B  |= (1 << CS11);
			}
			
			//Reset Flags
			flagColorChanged = FALSE;
			flagModeChanged	 = FALSE;
		}
	}
    return 0;
}