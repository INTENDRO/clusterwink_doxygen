/** ***************************************************************************
 * @file inits.c
 * @brief inits.c Initializes the hardware/software
 *
 * Various functions do each initialize a certian part of the hardware/software:
 * @n These Parts are: GPIO, Timer, SPI, UART and initial color values
 *
 * @author Manuel Boebel, Marcel Schreiner
 * @date 02.11.2017
 *****************************************************************************/

#include <avr/interrupt.h>
#include <avr/io.h>
#include "defines.h"
#include "inits.h"
#include "colors.h"
 
//*****************************************************************************
//***                         function "PortInit"                           ***
//*****************************************************************************

/** ***************************************************************************
 * @brief Initialize GPIOs (Input or Output) 
 * 
 * Is used to set all the GPIOs either as input or output
 *
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void PortInit(void)
{
	// 0 = Input   1 = Output
	DDRB = 0x00;
	DDRC = 0x0F;
	DDRD = 0xF4;			
} 


//*****************************************************************************
//***                       function "SPI_SlaveInit"                        ***
//*****************************************************************************

/** ***************************************************************************
 * @brief Initialize SPI interface
 * 
 * Initialize the SPI interface with the following settings:
 * @n  SPI interrupt: 					disabled
 * @n  SPI interface general enable: 	true
 * @n  Data Order: 						MSB first
 * @n  Master/Slave Select:        		Slave
 * @n  Clock Polarity:             		LOW when idle
 * @n  Clock Phase:         	    	Sample data at the leading (first) edge of SCK
 *
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void SPI_SlaveInit(void)
{
	//SPCR – SPI Control Register
	SPCR = 0x40;		
		// Bit 7 SPI Interrupt Enable       "0"=Disabled
		// Bit 6 SPI Enable                 "1"=Enabled
		// Bit 5 Data Order					"0"=MSB first
		// Bit 4 Master/Slave Select        "0"=Slave
		// Bit 3 Clock Polarity             "0"=LOW when idle
		// Bit 2 Clock Phase         	    "1"=Sample data at the leading (first) edge of SCK
		// Bit 1 SPI Clock Rate Select      Irrelevant
		// Bit 0 SPI Clock Rate Select      Irrelevant	
}


//*****************************************************************************
//***                       function "InterruptInit"                        ***
//*****************************************************************************

/** ***************************************************************************
 * @brief Initialize Interrupt 
 * 
 * Initialize interrupt with following settings:
 * @n Enable triger on INT1
 * @n Triger INT1 on falling edge
 *
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void InterruptInit(void)
{
	EICRA = 0x08;					// Trigger INT1 on falling edge
	EIMSK |= (1 << INT1);			// Enable  INT1
	TIMSK1 |= (1 << TOIE1);			// Enable overflow interrupt
}


//*****************************************************************************
//***                         function "TimerInit"                          ***
//*****************************************************************************

/** ***************************************************************************
 * @brief Initialize timer (used for animations)
 * 
 * Initialize the timer that is use for animations with the following settings:
 * @n Prescaler: 8 (~33ms per cycle)
 * @n Counter value init with zero
 *
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void TimerInit(void)
{
	TCCR1B |= (1 << CS11);		// set up timer1 with a prescaler (CS12)=256 -> 1.04856 s
								//								  (CS11)=8   -> 0.03276 s
	TCNT1 = 0;					// initialize counter
}


//*****************************************************************************
//***                          function "UsartInit"                         ***
//*****************************************************************************

/** ***************************************************************************
 * @brief Initialize uart
 * 
 * Initialize the uart interface with the following settings:
 * @n Baudrate according to USART_BAUDRATE in defines.h
 * @n Datatype: 8bit, no parity, 1 stop bit
 *
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void UsartInit(void){
	UBRR0 = UBRR_VALUE;						// Set baudrate
	UCSR0B |= (1<<TXEN0);					// Enable UART transmitter
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);		// Dataform: 8bit, no parity, 1 stop
}


//*****************************************************************************
//***                          function "BufferInit                         ***
//*****************************************************************************

/** ***************************************************************************
 * @brief Initialize color buffer (used for animations)
 * 
 * Initialize color buffer with preset colors 
 * (For defined colors after start-up)
 *
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void BufferInit(void){
	
	int i=0;
	while(i<(NUM_ATTACHED_MODULES))
	{
		individualPanelColor[i].rd = 0;
		individualPanelColor[i].gn = 0;
		individualPanelColor[i].bl = 0;
		individualPanelColor[i].wh = 50;
		i++;
	}
	
	effectColorTarget[0].rd = 10;
	effectColorTarget[0].gn = 0;
	effectColorTarget[0].bl = 10;
	effectColorTarget[0].wh = 0;
	
	effectColorTarget[1].rd = 0;
	effectColorTarget[1].gn = 0;
	effectColorTarget[1].bl = 10;
	effectColorTarget[1].wh = 0;
	
	effectColorTarget[2].rd = 0;
	effectColorTarget[2].gn = 10;
	effectColorTarget[2].bl = 0;
	effectColorTarget[2].wh = 0;
}
