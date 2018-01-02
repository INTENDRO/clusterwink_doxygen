/** ***************************************************************************
 * @file uart.h
 * @brief uart.h is the header file of uart.c which is for the uart interface
 *
 * functions for sending data to the uart interface.
 *
 * @author Manuel Boebel, Marcel Schreiner
 * @date 19.11.2017
 *****************************************************************************/


#ifndef UART_H_
#define UART_H_


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>


void usart_initBuf(volatile u8buf* b);
uint8_t usart_writeBuf(volatile u8buf* b, uint8_t data);
uint8_t usart_readBuf(u8buf* b, uint8_t* data);
int uart_putc(unsigned char c);
void uart_puts (char *s)


#endif /* UART_H_ */
