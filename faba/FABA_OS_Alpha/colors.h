/** ***************************************************************************
 * @file colors.h
 * @brief colors.h is the header file of colors.c in which the colors for every 
 * pannel are calculated and allocated
 *
 * @author Manuel Boebel, Marcel Schreiner
 * @date 11.11.2017
 *****************************************************************************/

#ifndef _COLORS_H_
#define _COLORS_H_

#include <avr/io.h>

//*****************************************************************************
//***                               defines                                 ***
//*****************************************************************************

void calculateColorAndFillBuffer(void);


//*****************************************************************************
//***                              variables                                ***
//*****************************************************************************

/// Every struct element represents the color of one module
extern volatile struct color {
	uint8_t rd;			              ///< Represents brightness green
	uint8_t gn;                       ///< Represents brightness red
	uint8_t bl;                       ///< Represents brightness blue
	uint8_t wh;                       ///< Represents brightness white
};

// The SPI/UART interface-Part saves the target color in here.
struct color individualPanelColor[];

// Every struct saves a additional color used for effects
struct color effectColorTarget[];

// The 2-Color Fader and color gradient do need this for pre calculating color gradients
struct color ringBufferColor[];

uint8_t mode;

#endif /* _COLORS_H_ */
