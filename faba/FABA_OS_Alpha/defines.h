/** ***************************************************************************
 * @file defines.h
 * @brief in the file defines.h core definitions are made
 *
 * Clock Frequency, Pin Names, Baudrates, constants
 *
 * @author Manuel Boebel, Marcel Schreiner
 * @date 11.10.2017
 *****************************************************************************/

#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <avr/io.h>

//*****************************************************************************
//***                               defines                                 ***
//*****************************************************************************

// Frequency
#ifndef F_CPU
#define F_CPU 16000000UL								///< CPU Frequency is 16Mhz
#endif

//define UART Baudrate
#define			USART_BAUDRATE 19200UL 					///< Set baud rate to 19200
#define			UBRR_VALUE ((F_CPU / (USART_BAUDRATE * 16UL)) - 1) ///< Number of CPU cycles per package
#define			USART_BUFSIZE 20						///< The uart buffer has a size of 20 bytes

// Pin defines I2C
#define			PIN_SCL					PORTC5			///< The I2C SCL pin is bit 5 on port c
#define			PIN_SDA					PORTC4			///< The I2C SDA pin is bit 4 on port c

// Pin defines SPI
#define			PIN_SS					PORTB2			///< The SPI SS pin is bit 2 on port b
#define			PIN_MOSI				PORTB3			///< The SPI MOSI pin is bit 3 on port b
#define			PIN_MISO				PORTB4			///< The SPI MISO pin is bit 4 on port b
#define			PIN_SCK					PORTB5			///< The SPI SCK pin is bit 5 on port b

// Pin defines UART
#define			PIN_RX					PORTD0			///< The UART RX pin is bit 0 on port d
#define			PIN_TX					PORTD1			///< The UART TX pin is bit 1 on port d

// Pin defines LED
#define			PIN_SEND				PORTD2          ///< This input is high as long as the RGBooster is sending the LED protocoll
#define			PIN_DATA0				PORTC0          ///< This Output is used as bit 0 in the led data byte
#define			PIN_DATA1				PORTC1          ///< This Output is used as bit 1 in the led data byte
#define			PIN_DATA2				PORTC2          ///< This Output is used as bit 2 in the led data byte
#define			PIN_DATA3				PORTC3          ///< This Output is used as bit 3 in the led data byte
#define			PIN_DATA4				PORTD4          ///< This Output is used as bit 4 in the led data byte
#define			PIN_DATA5				PORTD5          ///< This Output is used as bit 5 in the led data byte
#define			PIN_DATA6				PORTD6          ///< This Output is used as bit 6 in the led data byte
#define			PIN_DATA7				PORTD7			///< This Output is used as bit 7 in the led data byte

// Defines for PROJ_FABA_AMBIANCE
#define			NUM_ATTACHED_MODULES	5				///< Specifies the number of attached light panels
#define			NUM_LEDS_PER_MODULE		15				///< Specifies the number of LEDs in a light panel
#define			NUM_COLOR_GRADIENTS		100				///< Specifies the size of storage used for color animations
#define			NUM_EFFECT_COLORS		3				///< Specifies the number of bytes of additional storage for effect colors ; "0" color for "1 color mode ; "1" color A for "2 color fader mode" ; "2" color B for "2 color fader mode"
														
// Define MODES
#define			NUM_MODES				5				///< ***IMPORTANT*** Number of different modes 
#define			MODE_OFF				0				///< All modules are off
#define			MODE_INDIVIDUAL_COLOR	1				///< Every module has it's individual color
#define			MODE_ONECOLOR			2				///< All modules are the same color
#define			MODE_TWOCOLOR_FADER		3				///< Fading in between two colors (animation)
#define			MODE_TWOCOLOR_GRADIENT	4				///< Color gradient with two colors (static)

// Misc. Defines                                        
#define			TRUE					1               ///< Logic state true
#define			FALSE					0               ///< Logic state false

// Check user input
#if 100 < NUM_ATTACHED_MODULES
#error "More modules attached than defined! (max. is 100)"
#endif

#if NUM_EFFECT_COLORS > 20
#error "More storage for effect-colors requested than possible! (max. is 20)"
#endif

#if NUM_MODES > 8
#error "More modes defined than possible! (max. is 8)"
#endif

#endif /* _DEFINES_H_ */
