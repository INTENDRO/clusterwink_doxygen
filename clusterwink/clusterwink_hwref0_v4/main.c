/*************************************************************************//**
 * @mainpage Clusterwink wake-up light
 *
 * <strong>"Project Clusterwink"</strong> was developed during the "Electronics Project 1 (ETP1)" 
 * module at the Zurich University of Applied Sciences. 
 * It is part of the <strong>second year of the electronics engineering curriculum</strong> starting with fall term 2017.
 * <br>
 * The original task encompassed developing hard- and software for an RGB mood light. 
 * "Project Clusterwink" expanded on this task and incorporates a high power led and driver circuit. 
 * Those are intended to be used as a "sunrise alarm clock" which gently wakes you by simulating a sunrise. 
 * Doing so will prevent you being awoken during a deep sleep phase, which causes irritation and sleepiness during the day. 
 * The mood light functionality is retained thanks to the integrated RGB-LEDs, which can produce a wide variety of colors.
 * <br>
 * 
 * Process of ETP1:
 * <ol>
 * 	<li>Defining Goals</li>
 * 	<li>Creating the Schematic</li>
 * 	<li>Creating the Layout</li>
 * 	<li>External manufacturing of PCB</li>
 * 	<li>Assembling the PCB</li>
 * 	<li>Writing the Software</li>
 * 	<li>Testing</li>
 * 	<li>Designing a Enclosure</li>
 * 	<li>Lasercutting of Enclosure</li>
 * </ol>
 * <img src="img/cw1.jpg" height="450">
 * <br>
 * <h3>ETP2</h3>
 * The goal of next Semesters Electronics Project (ETP2) will be to develop a web-application 
 * so that the user can interact with the alarm clock. 
 * This will allow setting of different alarm times and durations, selecting colors or color progressions 
 * and open many possibilities for further applications. This will be achieved by running a webserver on the Raspberry Pi, on which the application will run. 
 * All settings can be accessed there. The users inputs will be processed by the Raspberry Pi and the approptiate data is sent to the Clusterwinks Microcontroller.
 * This approach has a few benefits over a similiar Android Application. In this specific instance, the ability to access the web application from any device is key. This means that the User Interface can not only be accessed with android devices, but any device capable of connecting to the internet.
 * <br>
 * The final assembly of "project clusterwink" will include the following:
 * <ul>
 * 	<li>clusterwink pcb</li>
 * 	<li>rgbooster (TM)</li>
 * 	<li>raspberry pi</li>
 * 	<li>power led and temperature sensor</li>
 * 	<li>lasercut enclosure</li>
 * 	<li>external 24V power adapter</li>
 * </ul>
 * <br>
 * <img src="img/case1.jpg" height="450"><br>
 * Board: clusterwink rev0 <br>
 * Device: Atmel ATMega328
 *
 * @author lopeslen, nosedmar
 * @date 29.12.2017
 *****************************************************************************/
 
 
 /** ***************************************************************************
 * @file main.c
 * @brief main file, entry point
 *
 * @author lopeslen, nosedmar
 * @date 29.12.2017
 *****************************************************************************/


#include <avr/io.h>
#include <avr/interrupt.h>
#include "utils.h"
#include "spi.h"
#include "usart.h"
#include "ringbuffer.h"
#include "rgbooster.h"


#define LED_COUNT		20		///< RGB LED strip length

volatile unsigned char aucRed[LED_COUNT] =		{0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF}; ///< red data buffer used by ISR for RGB LEDs
volatile unsigned char aucGreen[LED_COUNT] =	{0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF}; ///< green data buffer used by ISR for RGB LEDs
volatile unsigned char aucBlue[LED_COUNT] =		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; ///< blue data buffer used by ISR for RGB LEDs

volatile unsigned char ucRGBIdx = LED_COUNT;		///< RGB LED counter variable (ISR)
volatile unsigned char ucByteIdx = 0;				///< byte counter for color (ISR)

volatile unsigned char ucSPIData = 0;				///< received SPI data (ISR)
volatile unsigned char ucCommandBuffer = 0;			///< last received SPI data (ISR)
volatile unsigned char ucDataCounter = 0;			///< SPI data counter (ISR)
volatile unsigned char ucDutyBuffer = 0;			///< dutycycle buffer register. (readable by RPi)
volatile unsigned char ucTemperatureBuffer = 0;		///< temperature buffer register. (readable by RPi)
volatile unsigned char ucStatusBuffer = 0;			///< status buffer register. (readable by RPi)

static RingBuff_t RINGBUFFER;						///< ringbuffer instance for storing long commands (ISR)


/** ***************************************************************************
 * @brief SPI data received
 *
 * The received data is a command if the MSB (bit 7) is set. Otherwise it is
 * treated as data related to the last received command.
 * "Short commands" (short execution time) get executed immediately while all
 * other commands get stored into the ringbuffer for later execution inside of
 * the main loop.
 *
 * Command list (as seen from the RPi side):
 * - 0x80: enable power LED
 * - 0x81: disable power LED
 * - [0x82, XX]: set the dutycycle of the power LED to XX (percent -> XX = [0-100]) 
 * - 0x83: clear all RGB LEDs
 * - [0x84, RR, GG, BB]: set all RGB LEDs to a specified color [0-0x7F]
 * - [0x8D, 0x00]: read the dutycycle buffer register
 * - [0x8E, 0x00]: read the temperature buffer register
 * - [0x8F, 0x00]: read the status buffer register
 * 
 * @param [in] SPI_STC_vect: "Serial Transfer Complete" vector
 * @return no return value
 *****************************************************************************/
ISR(SPI_STC_vect) // SPI receive complete
{
	ucSPIData = SPDR;
	SPDR = 0;

	if(ucSPIData & 0x80) // command
	{
		ucDataCounter = 0;
		ucCommandBuffer = ucSPIData&0x0F;

		switch(ucCommandBuffer) // "short commands" will be executed instantly. others get stored in the ringbuffer
		{
			case 0: // enable power led
			enablePLED();
			ucStatusBuffer |= (1<<0);
			break;

			case 1: // disable power led
			disablePLED();
			ucStatusBuffer &= ~(1<<0);
			break;
			
			case 3: // clear RGBs
			RingBuffer_Insert(&RINGBUFFER,ucSPIData);
			RingBuffer_Insert(&RINGBUFFER,0xFF);		//0xFF marks the end of a command sequence in the ringbuffer
			break;
			
			case 4: // display single color on all RGBs
			RingBuffer_Insert(&RINGBUFFER,ucSPIData);
			break;
			
			case 13: // read dutycycle register
			SPDR = ucDutyBuffer;
			break;
			
			case 14: // read temperature register
			SPDR = ucTemperatureBuffer;
			break;
			
			case 15: // read status register
			SPDR = ucStatusBuffer;
			break;
			
			default: // unknown command
			break;
		}
	}
	else // data
	{
		ucDataCounter++;
		
		switch(ucCommandBuffer)
		{
			case 2: // set dutycycle
			ucDutyBuffer = ucSPIData;
			if(ucDutyBuffer>100) ucDutyBuffer = 100;
			setDuty(ucDutyBuffer);
			break;
			
			case 4: // display single color on all RGBs
			if(ucDataCounter<=3)
			{
				RingBuffer_Insert(&RINGBUFFER,ucSPIData);
				if(ucDataCounter==3)
				{
					RingBuffer_Insert(&RINGBUFFER,0xFF);
				}
			}
			break;
			
			default: // last received command does not require any additional data. do nothing
			break;
		}
		
		
	}
}


/** ***************************************************************************
 * @brief External interrupt on INT1
 *
 * This ISR outputs the next data byte according to the RGB LED counter and 
 * color counter. The handshake of the RGBooster board will bring the program
 * execution back to this ISR. After the last byte is sent this ISR will not
 * send any more data and thus no more handshakes will invoke this ISR.
 * The RGB LED counter (ucRGBIdx) and the color counter (ucByteIdx) need to be
 * set to zero and the ISR should be called manually with "INT1_vect();" to
 * start the protocol generation.
 * 
 * @param [in] INT1_vect: External Interrupt Request 1
 * @return no return value
 *****************************************************************************/
ISR(INT1_vect)	// external interrupt (handshake from RGBooster board)
{				// start RGBooster send sequence: reset "ucRGBIdx" and "ucByteIdx" to zero. then start with calling the ISR directly "INT1_vect();"
	if(ucRGBIdx<(LED_COUNT))
	{
		switch(ucByteIdx) // red green and blue are sent in 3 separate bytes. this variable remembers the next color to be sent
		{
			case 0:
			PORT_DATA_HIGH = (PORT_DATA_HIGH & ~DATA_HIGH_BITMASK) | (aucGreen[ucRGBIdx] & DATA_HIGH_BITMASK);
			PORT_DATA_LOW = (PORT_DATA_LOW & ~DATA_LOW_BITMASK) | (aucGreen[ucRGBIdx] & DATA_LOW_BITMASK);
			PORT_CONTROL |= (1<<SEND); // generate send impulse
			PORT_CONTROL &= ~(1<<SEND);
			ucByteIdx++;
			break;

			case 1:
			PORT_DATA_HIGH = (PORT_DATA_HIGH & ~DATA_HIGH_BITMASK) | (aucRed[ucRGBIdx] & DATA_HIGH_BITMASK);
			PORT_DATA_LOW = (PORT_DATA_LOW & ~DATA_LOW_BITMASK) | (aucRed[ucRGBIdx] & DATA_LOW_BITMASK);
			PORT_CONTROL |= (1<<SEND); // generate send impulse
			PORT_CONTROL &= ~(1<<SEND);
			ucByteIdx++;
			break;

			case 2:
			PORT_DATA_HIGH = (PORT_DATA_HIGH & ~DATA_HIGH_BITMASK) | (aucBlue[ucRGBIdx] & DATA_HIGH_BITMASK);
			PORT_DATA_LOW = (PORT_DATA_LOW & ~DATA_LOW_BITMASK) | (aucBlue[ucRGBIdx] & DATA_LOW_BITMASK);
			PORT_CONTROL |= (1<<SEND); // generate send impulse
			PORT_CONTROL &= ~(1<<SEND);
			ucByteIdx=0;
			ucRGBIdx++;
			break;
		}
	}
}


/** ***************************************************************************
 * @brief Main function - Entry point
 *
 * All initialization functions get called first and interrupts are globally
 * enabled. The RGB buffers already contain data to check the strip at the
 * beginning. Data is manually inserted into the ringbuffer to test how the
 * main programm handles received commands (through SPI).
 * 
 * The main loop constantly measures the temperature and then checks if the
 * ringbuffer is empty. If not, the contained commands are executed.
 * 
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void main(void)
{
	unsigned char ucTemp;
	unsigned int i;
	unsigned char aucCommandString[64];
	
	// INITIALIZATION
	portInit();
	RingBuffer_InitBuffer(&RINGBUFFER);
	AD8bit_Init();
	initRGBooster();
	INT1_Init();
	SendStrip_Off(LED_COUNT);
	SPISlave_Init();
	USART_Init();
	disablePLED();
	initPWM(ucDutyBuffer);
	startPWM();

	sei();
	
	wait_1ms(1000);
	
	//RGB TEST
	ucByteIdx = 0;
	ucRGBIdx = 0;
	INT1_vect();
	
	wait_1ms(2000);
	
	//RING BUFFER TEST (INSERTING COMMANDS)
	RingBuffer_Insert(&RINGBUFFER,0x83);
	RingBuffer_Insert(&RINGBUFFER,0xFF);
	
	RingBuffer_Insert(&RINGBUFFER,0x84);
	RingBuffer_Insert(&RINGBUFFER,0x00);
	RingBuffer_Insert(&RINGBUFFER,0x20);
	RingBuffer_Insert(&RINGBUFFER,0x00);
	RingBuffer_Insert(&RINGBUFFER,0xFF);
	
	
	while(1)
	{
		ucTemp = AD8bit_Measurement();
		ucTemperatureBuffer = (unsigned char)(((double)ucTemp*100/51-50)+0.5); // round data to integer
		
		if(RingBuffer_GetCount(&RINGBUFFER) > 0) // data in ringbuffer
		{
			if(RingBuffer_CountChar(&RINGBUFFER,0xFF)>0) // complete commands in ringbuffer
			{
				RingBuffer_RemoveUntilChar(&RINGBUFFER,aucCommandString,0xFF,false); // get command
				
				switch(aucCommandString[0])
				{
					case 0x83: // clear RGB leds
					for(i=0;i<LED_COUNT;i++)
					{
						aucRed[i] = 0;
						aucGreen[i] = 0;
						aucBlue[i] = 0;
					}
					ucRGBIdx = 0;
					ucRGBIdx = 0;
					INT1_vect(); //start transmission
					break;
					
					case 0x84: //single color for all RGB leds
					for(i=0;i<LED_COUNT;i++)
					{
						aucRed[i] = aucCommandString[1];
						aucGreen[i] = aucCommandString[2];
						aucBlue[i] = aucCommandString[3];
					}
					
					ucRGBIdx = 0;
					ucRGBIdx = 0;
					INT1_vect(); //start transmission
					break;
					
					default:
					break;
				}
			}
		}
		wait_1ms(1000);
	}
}