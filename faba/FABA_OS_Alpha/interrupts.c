/** ***************************************************************************
 * @file interrupts.c
 * @brief in the file interrupts.c interrupts (by timer / by external interrupt) is handled
 *
 * timer interrupt for shifting colors in the "shift-mode"
 * extern interrupt for sending setting new data to the output
 *
 * @author Manuel Boebel, Marcel Schreiner
 * @date 02.11.2017
 *****************************************************************************/


#include <avr/interrupt.h>
#include "defines.h"
#include "interrupts.h"
#include "colors.h"
#include "spi.h"

uint8_t color_index					= GREEN;	///< Saves color that will be refreshed
uint8_t led_index					= 0;        ///< Saves index of currently refreshed LED in a panel
uint8_t panel_index					= 0;        ///< Saves index of currently refreshed panel
uint8_t flag_last_led_written		= FALSE;    ///< True if last LED was refteshed
uint8_t flag_sending_in_progress	= FALSE;    ///< True if currently LED are getting refreshed
uint8_t increase_BufferOffset		= TRUE;     ///< True if "fade-animation" is displayed
uint8_t bufferOffset				= 0;        ///< Used to create "fade-animation"

 

//*****************************************************************************
//***                          interrupt timer 1                            ***
//*****************************************************************************

/** ***************************************************************************
 * @brief shifting bufferOffset (Offset in Ringbuffer) and calling LED-Protocoll Interrupt
 
 * Timer is only activated in MODE_TWO_COLOR_FADER. There a color gradient is calculated with
 * @n "NUM_COLOR_GRADIENTS" color values. The colors are saved into the ring buffer.
 * @n Now, the attached modules are shifting in this ring buffer. At the begin, they are at the
 * @n one end of it and then shifting to the other end and back again.
 
 * @param [in] Timer1_OVF_vec: timer overflow vector
 * @return no return value
 *****************************************************************************/
ISR(TIMER1_OVF_vect)
{
	PORTB ^= (1<<PORTB1);
	if (flag_sending_in_progress == FALSE)
	{
		if(increase_BufferOffset)
		{
			//Increase Buffer offset (for fading effect)
			bufferOffset += 1;
			
			//if buffer offset at the end
            //(for example (100 color gradients; 20 attached modules --> max offset = 80)
            //--> state to decrease
			if (bufferOffset >= (NUM_COLOR_GRADIENTS - NUM_ATTACHED_MODULES))
			{
				increase_BufferOffset = FALSE;
			}
		}
		else
		{
			//Decrease Buffer offset (for fading effect)
			bufferOffset -= 1;
			
			//if buffer offset = 0 --> state = increase
			if (bufferOffset == 0)
			{
				increase_BufferOffset = TRUE;
			}
		}
		INT1_vect();
	}
}


//*****************************************************************************
//***                        interrupt LED-Portocol                         ***
//*****************************************************************************

/** ***************************************************************************
 * @brief sending LED Protocoll to RGBooster
 
 * The interrupt is forced the first time. Then it sets the byte of the first color to the output.
 * @n When the RGBooster finished sending the data it sets the interrupt pin to high and the interrupt
 * fires again and sends the next byte (next color).
 * @n The interrupt function sends the same color data NUM_LEDS_PER_MODULE times (the same color within 1 module)
 * @n When finished all Bytes for each LED, the flag flag_sending_in_progress is set to false.
 
 * @param [in] INT1_vect: External Interrupt 1
 * @return no return value
 *****************************************************************************/
ISR(INT1_vect) // External Interrupt 1
{
	flag_sending_in_progress = TRUE;
	if(flag_last_led_written == FALSE)
	{
		// Load Color Data To Outputs
		switch(color_index)
		{
			case GREEN:
				PORTD = ringBufferColor[panel_index + bufferOffset].gn&0xF0;
				PORTC = ringBufferColor[panel_index + bufferOffset].gn&0x0F;
				color_index = RED;		// Set next color
				break;
			case RED:
				PORTD = ringBufferColor[panel_index + bufferOffset].rd&0xF0;
				PORTC = ringBufferColor[panel_index + bufferOffset].rd&0x0F;
				color_index = BLUE;		// Set next color
				break;
			case BLUE:
				PORTD = ringBufferColor[panel_index + bufferOffset].bl&0xF0;
				PORTC = ringBufferColor[panel_index + bufferOffset].bl&0x0F;
				color_index = WHITE;	// Set next color
				break;
			case WHITE:
				PORTD = ringBufferColor[panel_index + bufferOffset].wh&0xF0;
				PORTC = ringBufferColor[panel_index + bufferOffset].wh&0x0F;
				color_index = GREEN;	// Set next color
				break;
			default:
				PORTD = 0;
				PORTC = 0;
				color_index = GREEN;	// Set next color
				break;
		}
		
		// Start LED-Protocol sending process
		PORTD |= (1<<PIN_SEND);	// Set Send-Pulse
		PORTD &= ~(1<<PIN_SEND);	// Clear Send-Pulse

		if (color_index == GREEN)
		{
			//increase led index by 1 unless it already is at number of leds per panel (-> reset)
			if (led_index >= (NUM_LEDS_PER_MODULE-1))
			{
				led_index = 0;
				//increase panel_index by 1 unless it is already at the num of attached modules
				if(panel_index < (NUM_ATTACHED_MODULES-1))
				{
					panel_index += 1;
				}
				else
				{
					panel_index = 0;
					flag_last_led_written = TRUE;		
				}
			}
			else
			{
				led_index += 1;
			}
		}
	}
	else
	{
		flag_last_led_written = FALSE; // Reset Flag
		flag_sending_in_progress = FALSE;
	}
}