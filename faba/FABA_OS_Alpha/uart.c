/** ***************************************************************************
 * @file uart.c
 * @brief uart.c is for the uart interface
 *
 * functions for sending data to the uart interface.
 *
 * @author Manuel Boebel, Marcel Schreiner
 * @date 19.11.2017
 *****************************************************************************/

#include "defines.h"
#include "uart.h"

/// Buffer for receiving uart
typedef struct u8buf{
	uint8_t buffer[USART_BUFSIZE];          ///< Buffer with the size USART_BUFSIZE
	uint8_t index;                          ///< Bufferindex
	bool receiveComplete;                   ///< True if receive finished
}u8buf;

volatile u8buf buf;							///< Buffer for Rx and Tx of UART


//*****************************************************************************
//***                       function "usart_initBuf"                        ***
//*****************************************************************************

/** ***************************************************************************
 * @brief Initialize UART Buffer for receiving UART data
 * @param [in] b: pointer on a uart buffer
 * @return No value is getting returned
 *****************************************************************************/
void usart_initBuf(volatile u8buf* b)
{
	b->index = 0;                       //set index to 0
	b->receiveComplete = false;  		//received state = "receive not completed"
}


//*****************************************************************************
//***                      function "usart_writeBuf"                        ***
//*****************************************************************************

/** ***************************************************************************
 * @brief UART save incoming data; data is written into the UART buffer
 * @param [in] b: pointer on uart buffer
 * @param [in] data: data
 * @return 0: no error, 1: error (index >= Buffersize)
 *****************************************************************************/
uint8_t usart_writeBuf(volatile u8buf* b, uint8_t data)
{
	if (b->index < USART_BUFSIZE){
		b->buffer[b->index] = data;
		b->index ++;
		if (data == 2)
		{
			b->receiveComplete = true;
		}
		return 0;
	} else{
		return 1;
	}
}


//*****************************************************************************
//***                      function "usart_readBuf"                         ***
//*****************************************************************************

/** ***************************************************************************
 * @brief UART read received Data out of UART buffer
 * @param [in] b: pointer on uart buffer
 * @param [in] data: pointer on data
 * @return 0: no error, 1: error (index is negative)
 *****************************************************************************/
uint8_t usart_readBuf(u8buf* b, uint8_t* data){
	if (b->index > 0){
		b->index--;
		*data = b->buffer[b->index];
		return 0;
	}
	else{
		return 1;
	}
}


//*****************************************************************************
//***                        function "uart_putc"                           ***
//*****************************************************************************

/** ***************************************************************************
 * @brief UART: writes one char to the UART interface
 * @param [in] c: char to send
 * @return 0: when finished
 *****************************************************************************/
int uart_putc(unsigned char c)
{
	while (!(UCSR0A & (1<<UDRE0))) 	// Wait untill sending is possible
	{
	}
	UDR0 = c;                      	// Send character
	return 0;
}


//*****************************************************************************
//***                        function "uart_puts"                           ***
//*****************************************************************************

/** ***************************************************************************
 * @brief UART: this function writes a whole string (char-array) to the UART interface
 * @param [in] s: pointer of char-array
 * @return No value is returned
 *****************************************************************************/
void uart_puts (char *s)
{
    while (*s)
	{
		uart_putc(*s);
		s++;
	}
}
