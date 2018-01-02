/** ***************************************************************************
 * @file usart.c
 * @brief Initializing the USART master interface
 *
 * This is only used for debugging (sending data to a PC terminal) and has no
 * significant impact on the main program itself.
 *
 * @author lopeslen, nosedmar
 * @date 29.12.2017
 *****************************************************************************/


 #include <avr/io.h>
 #include "usart.h"


/** ***************************************************************************
 * @brief Initializes the USART peripheral as a master
 * 
 * - Clear all flags
 * - enable the usart
 * - disable interrupts
 * - 9600 Baud (@20MHz)
 * - pin directions (TX: output, RX: input)
 * - package size: 8bit. 
 * 
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
 void USART_Init(void)
 {
	 UCSR0A = (1<<TXC0); // clear transmit flag
	 UCSR0B = (1<<RXEN0) | (1<<TXEN0); // enable receiver and transmitter
	 UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // package size: 8bit
	 UBRR0 = 129; // 9600 Baud @ 20MHz
	 
	 DDRD |= (1<<PIND1); //TX pin as output
	 DDRD &= ~(1<<PIND0); //RX pin as input
	 PORTD &= ~(1<<PIND1);
 }


/** ***************************************************************************
 * @brief Send byte with the USART master interface
 *
 * @param [in] cData: data byte to be sent
 * @return no return value
 *****************************************************************************/
 void USART_SendByte(char cData)
 {
	 while(!(UCSR0A&(1<<UDRE0))); // wait if last not done transmitting
	 
	 UDR0 = cData; // this will directly start the transmission
 }


/** ***************************************************************************
 * @brief Send zero-terminated string with the USART master interface
 *
 * @param [in] cData_ptr: char pointer to data array
 * @return no return value
 *****************************************************************************/
 void USART_SendStr(char* cData_ptr)
 {
	 while(*cData_ptr != 0) //string needs to be zero-terminated. otherwise this might loop infinitely
	 {
		 USART_SendByte(*cData_ptr);
		 cData_ptr++;
	 }
 }