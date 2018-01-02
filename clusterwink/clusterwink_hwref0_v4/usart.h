/** ***************************************************************************
 * @file usart.h
 * @brief Initializing the USART master interface
 *
 * This is only used for debugging (sending data to a PC terminal) and has no
 * significant impact on the main program itself.
 *
 * @author lopeslen, nosedmar
 * @date 29.12.2017
 *****************************************************************************/


#ifndef USART_H_
#define USART_H_

void USART_Init(void);
void USART_SendByte(char cData);
void USART_SendStr(char* cData_ptr);

#endif /* USART_H_ */