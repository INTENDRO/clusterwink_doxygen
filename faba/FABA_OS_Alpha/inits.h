/** ***************************************************************************
 * @file inits.h
 * @brief in the file inits.c the hardware/software gets initialized
 *
 * Various functions do each initialize a certian part of the hardware/software:
 * @n These Parts are: GPIO, Timer, SPI, UART and initial color values
 *
 * @author Manuel Boebel, Marcel Schreiner
 * @date 02.11.2017
 *****************************************************************************/

#ifndef _INITS_H_
#define _INITS_H_

#include <avr/io.h>

//*****************************************************************************
//***                               defines                                 ***
//*****************************************************************************

void PortInit(void);
void SPI_SlaveInit(void);
void InterruptInit(void);
void TimerInit(void);
void UsartInit(void);
void BufferInit(void);

#endif /* _INITS_H_ */
