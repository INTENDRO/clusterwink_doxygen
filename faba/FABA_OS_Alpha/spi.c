/** ***************************************************************************
 * @file spi.c
 * @brief spi.c is for interfacing the SPI interface
 *
 * Fetches received data form SPI.
 * A FSM checks if the received data is valid.
 * If data is valid, it's saved in the internal storrage system
 *
 * @author Manuel Boebel, Marcel Schreiner
 * @date 11.11.2017
 *****************************************************************************/

#include "defines.h"
#include "colors.h"
#include "spi.h"

uint8_t	fsmState          = STATE_IDLE;		///< Current State of SPI FSM
uint8_t flagColorChanged  = FALSE;          ///< True if new color received
uint8_t flagModeChanged   = FALSE;          ///< True if new mode received

 
//*****************************************************************************
//***                         function "FetchSpi"                           ***
//*****************************************************************************

/** ***************************************************************************
 * @brief Receives data via SPI and svea it for further usage
 * 
 * If new data was received the function checks it's validity with a FSM.
 * If a valid data backage was received it will be saved for further usage.
 * @n The FSM has the following states:
 * @n STATE_IDLE:			Waiting for data to receive
 * @n STATE_ADDR_RECEIVED:	First  byte (address)     is valid and saved in cache 
 * @n STATE_RD_RECEIVED:	Second byte (red value)   is valid and saved in cache
 * @n STATE_GN_RECEIVED:	Third  byte (green value) is valid and saved in cache
 * @n STATE_BL_RECEIVED:	Fourth byte (blue value)  is valid and saved in cache
 * @n STATE_WH_RECEIVED:	Fifth  byte (white value) is valid and saved in cache
 * 
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void FetchSpi(void)
{
	// Declare and define local variables
	uint8_t			dataByte;
	static uint8_t	cacheAddr;
	static uint8_t	cacheRd;
	static uint8_t	cacheGn;
	static uint8_t	cacheBl;
	static uint8_t	cacheWh;
	
	// If available read data from SPI (Check interrupt flag)
	if(SPSR & 0x80)
	{
		// reset interrupt flag
		SPSR = SPSR & 0x7F;
		
		// Fetch received data byte 
		dataByte = SPDR;
		
		// FSM
		switch (fsmState)
		{
			case STATE_IDLE:	
				//Check if MSB is HIGH
				if(dataByte & 0x80)
				{
					cacheAddr = (dataByte & 0x7F);
					fsmState = STATE_ADDR_RECEIVED;
				}
				break;
			case STATE_ADDR_RECEIVED:	
				//Check if MSB is LOW
				if((dataByte & 0x80) == 0)
				{
					cacheRd = dataByte;
					fsmState = STATE_RD_RECEIVED;
				}
				else
				{
					cacheAddr = (dataByte & 0x7F);
					fsmState = STATE_ADDR_RECEIVED;
				}
				break;	
			case STATE_RD_RECEIVED:		
				//Check if MSB is LOW
				if((dataByte & 0x80) == 0)
				{
					cacheGn = dataByte;
					fsmState = STATE_GN_RECEIVED;
				}
				else
				{
					cacheAddr = (dataByte & 0x7F);
					fsmState = STATE_ADDR_RECEIVED;
				}
				break;
			case STATE_GN_RECEIVED:		
				//Check if MSB is LOW
				if((dataByte & 0x80) == 0)
				{
					cacheBl = dataByte;
					fsmState = STATE_BL_RECEIVED;
				}
				else
				{
					cacheAddr = (dataByte & 0x7F);
					fsmState = STATE_ADDR_RECEIVED;
				}
				break;
			case STATE_BL_RECEIVED:		
				//Check if MSB is LOW
				if((dataByte & 0x80) == 0)
				{
					cacheWh = dataByte;
					fsmState = STATE_WH_RECEIVED;
				}
				else
				{
					cacheAddr = (dataByte & 0x7F);
					fsmState = STATE_ADDR_RECEIVED;
				}
				break;
			case STATE_WH_RECEIVED:		
				//Check if the four MSB are LOW
				if((dataByte & 0xF0) == 0)
				{
					// Save data according to address
					// Is data for individualPanelColor?
					if(cacheAddr < NUM_ATTACHED_MODULES)
					{
						individualPanelColor[cacheAddr].rd = cacheRd | ((dataByte & 0x01) << 7);
						individualPanelColor[cacheAddr].gn = cacheGn | ((dataByte & 0x02) << 6);
						individualPanelColor[cacheAddr].bl = cacheBl | ((dataByte & 0x04) << 5);
						individualPanelColor[cacheAddr].wh = cacheWh | ((dataByte & 0x08) << 4);
						flagColorChanged = TRUE;
					}
					// Is data for effectColorTarget?
					else if((99 < cacheAddr) && (cacheAddr < (100 + NUM_EFFECT_COLORS)))
					{
						effectColorTarget[cacheAddr-100].rd = cacheRd | ((dataByte & 0x01) << 7);
						effectColorTarget[cacheAddr-100].gn = cacheGn | ((dataByte & 0x02) << 6);
						effectColorTarget[cacheAddr-100].bl = cacheBl | ((dataByte & 0x04) << 5);
						effectColorTarget[cacheAddr-100].wh = cacheWh | ((dataByte & 0x08) << 4);
						flagColorChanged = TRUE;
					}
					// Is data for mode?
					else if((119 < cacheAddr) && (cacheAddr < (120 + NUM_MODES)))
					{
						mode = cacheAddr - 120;
						flagModeChanged = TRUE;
					}
					// If no case is true, an invalid address was sent	
					fsmState = STATE_IDLE;				
				}
				else if ((dataByte & 0x80) == 0x80)
				{
					cacheAddr = (dataByte & 0x7F);
					fsmState = STATE_ADDR_RECEIVED;
				}
				else
				{
					fsmState = STATE_IDLE;
				}
				break;
			default:
				fsmState = STATE_IDLE;
				break;
		}		
	}
}