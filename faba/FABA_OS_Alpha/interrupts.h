/** ***************************************************************************
 * @file interrupts.h
 * @brief interrupt.h is the header file of interrupts.c in which interrupts (by timer / by external interrupt) are handled
 *
 * timer interrupt for shifting colors in the "shift-mode"
 * extern interrupt for sending setting new data to the output
 *
 * @author Manuel Boebel, Marcel Schreiner
 * @date 02.11.2017
 *****************************************************************************/

#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include <avr/io.h>

#define GREEN	0		///< Represents color green
#define RED		1       ///< Represents color red
#define BLUE	2       ///< Represents color blue
#define WHITE	3       ///< Represents color white

uint8_t bufferOffset;	///< Used to create "fade-animation"

#endif /* _INTERRUPTS_H_ */
