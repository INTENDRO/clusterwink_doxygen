/** ***************************************************************************
 * @file colors.c
 * @brief in the file colors.c the colors for every pannel are calculated and allocated
 *
 * @author Manuel Boebel, Marcel Schreiner
 * @date 11.11.2017
 *****************************************************************************/

#include "defines.h"
#include "colors.h"


// The SPI/UART interface-Part saves the target color in here.
struct color individualPanelColor[NUM_ATTACHED_MODULES];            ///< Contains color for each attached module

// Every struct saves a additional color used for effects
struct color effectColorTarget[NUM_EFFECT_COLORS];                  ///< Contains color for one_color_mode as well as two colors (start and endvalue) for color_gradient_mode / color_fade_mode

// The 2-Color Fader and color gradient do need this for pre calculating color gradients
struct color ringBufferColor[NUM_COLOR_GRADIENTS];                  ///< Contains calculated colors for color_fade_mode

// Global state/mode of programm
uint8_t mode = MODE_OFF;											///< Global state/mode of programm

 
//*****************************************************************************
//***             function "CalculateColorAndFillBuffer"                    ***
//*****************************************************************************

/** ***************************************************************************
 * @brief Calculates colors for each panel for different color modes
 * 
 * contains a switch for the color mode:
 * @n MODE_OFF:                 set every color of the whole RingBuffer to 0.
 * @n MODE_INDIVIDUAL_COLOR:    set the colors of the ringbuffer for every attached module to the received color for this panel
 * @n MODE_ONECOLOR:            set every color of each attached module to the received colors (every panel has now the same color)
 * @n MODE_TWO_COLOR_FADER:     calculate "NUM_COLOR_GRADIENTS" of color values (between the received two colors) and save them to the RingBuffer
 * @n MODE_TWO_COLOR_GRADIENT:  calculate "NUM_NUM_ATTACHED_MOUDLES" of color values (between the received two colors) and save them to the RingBuffer
 *
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void calculateColorAndFillBuffer(void)
{
	// Declare and define local variables
	int i = 0;
	float redStep;
	float greenStep;
	float blueStep;
	float whiteStep;

	switch(mode)
	{
		case MODE_OFF:
			for(i = 0; i < NUM_COLOR_GRADIENTS; i += 1)
			{
				//Set everything to 0
				ringBufferColor[i].rd = 0;
				ringBufferColor[i].gn = 0;
				ringBufferColor[i].bl = 0;
				ringBufferColor[i].wh = 0;
			}
			break;
            
		case MODE_INDIVIDUAL_COLOR:
			for(i = 0; i < NUM_ATTACHED_MODULES; i += 1)
            {
                //Fill the ringBuffer with every received color --> for each module
                ringBufferColor[i].rd = individualPanelColor[i].rd;
                ringBufferColor[i].gn = individualPanelColor[i].gn;
                ringBufferColor[i].bl = individualPanelColor[i].bl;
                ringBufferColor[i].wh = individualPanelColor[i].wh;
            }
			break;
            
		case MODE_ONECOLOR:
            for(i = 0; i < NUM_ATTACHED_MODULES; i += 1)
            {
                //Fill the whole ringbuffer with just one color
                ringBufferColor[i].rd = effectColorTarget[0].rd;
                ringBufferColor[i].gn = effectColorTarget[0].gn;
                ringBufferColor[i].bl = effectColorTarget[0].bl;
                ringBufferColor[i].wh = effectColorTarget[0].wh;
            }
			break;
            
		case MODE_TWOCOLOR_FADER:
            redStep   = (float)(effectColorTarget[2].rd - effectColorTarget[1].rd) / (float)(NUM_COLOR_GRADIENTS);
            greenStep = (float)(effectColorTarget[2].gn - effectColorTarget[1].gn) / (float)(NUM_COLOR_GRADIENTS);
            blueStep  = (float)(effectColorTarget[2].bl - effectColorTarget[1].bl) / (float)(NUM_COLOR_GRADIENTS);
            whiteStep = (float)(effectColorTarget[2].wh - effectColorTarget[1].wh) / (float)(NUM_COLOR_GRADIENTS);
            
            for(i = 0; i < NUM_COLOR_GRADIENTS; i += 1)
            {
                //fill the ringBuffer
                ringBufferColor[i].rd = (int)(effectColorTarget[1].rd + i * redStep);
                ringBufferColor[i].gn = (int)(effectColorTarget[1].gn + i * greenStep);
                ringBufferColor[i].bl = (int)(effectColorTarget[1].bl + i * blueStep);
                ringBufferColor[i].wh = (int)(effectColorTarget[1].wh + i * whiteStep);
            }
            break;
            
        case MODE_TWOCOLOR_GRADIENT:
            redStep   = (float)(effectColorTarget[2].rd - effectColorTarget[1].rd) / (float)(NUM_ATTACHED_MODULES - 1);
            greenStep = (float)(effectColorTarget[2].gn - effectColorTarget[1].gn) / (float)(NUM_ATTACHED_MODULES - 1);
            blueStep  = (float)(effectColorTarget[2].bl - effectColorTarget[1].bl) / (float)(NUM_ATTACHED_MODULES - 1);
            whiteStep = (float)(effectColorTarget[2].wh - effectColorTarget[1].wh) / (float)(NUM_ATTACHED_MODULES - 1);
            
            for(i = 0; i < NUM_ATTACHED_MODULES; i += 1)
            {
                //fill the ringBuffer
                ringBufferColor[i].rd = (int)(effectColorTarget[1].rd + i * redStep);
                ringBufferColor[i].gn = (int)(effectColorTarget[1].gn + i * greenStep);
                ringBufferColor[i].bl = (int)(effectColorTarget[1].bl + i * blueStep);
                ringBufferColor[i].wh = (int)(effectColorTarget[1].wh + i * whiteStep);
            }
			break;
            
		default:
			//default: --> "Mode off"
			for(i = 0; i < NUM_COLOR_GRADIENTS; i += 1)
			{
				//Set everything to 0
				ringBufferColor[i].rd = 0;
				ringBufferColor[i].gn = 0;
				ringBufferColor[i].bl = 0;
				ringBufferColor[i].wh = 0;
			}
			mode = MODE_OFF;
			break;
	}
}
