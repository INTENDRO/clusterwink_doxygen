/** ***************************************************************************
 * @file rgbooster.h
 * @brief Software layer to interface the RGBooster hardware
 * 
 * The RGBooster generates the asynchronous protocol needed for WS2812. This
 * relieves the microcontroller (uC) from this time consuming task and enables it
 * to do more important functions at the same time. The uC only needs to output
 * the next byte to the 8 data lines and generate a rising edge on the send pin.
 * The RGBooster saves the data and generates the full protocol by hardware.
 * Upon completion, the RGBooster returns a handshake with a falling edge on
 * the !done/busy pin. Using an external interrupt pin for this !done/busy
 * greatly increases the speed and "multitasking" capability of the system.
 *
 * @author lopeslen, nosedmar
 * @date 14.11.2017
 *****************************************************************************/


#ifndef RGBOOSTER_H_
#define RGBOOSTER_H_


#define PORT_DATA_LOW		PORTC 	///< output port for lower nibble of data lines
#define PIN_DATA_LOW		PINC 	///< input port for lower nibble of data lines
#define DDR_DATA_LOW		DDRC 	///< data direction register of lower nibble of data lines
#define DATA_LOW_BITMASK	0x0F 	///< bitmask for lower nibble

#define PORT_DATA_HIGH		PORTD 	///< output port for higher nibble of data lines
#define PIN_DATA_HIGH		PIND 	///< input port for higher nibble of data lines
#define DDR_DATA_HIGH		DDRD 	///< data direction register of higher nibble of data lines
#define DATA_HIGH_BITMASK	0xF0 	///< bitmask for higher nibble

#define PORT_CONTROL		PORTD 	///< output port for control pins
#define PIN_CONTROL			PIND 	///< input port for control pins
#define DDR_CONTROL			DDRD 	///< data direction register of control pins
#define SEND				2 		///< send pin (output)
#define DONE_BUSY			3 		///< !done/busy pin (input)

void INT1_Init(void);
void initRGBooster(void);
void SendBit(unsigned char ucRed,unsigned char ucGreen, unsigned char ucBlue);
void SendStrip(unsigned char* ucRed_ptr, unsigned char* ucGreen_ptr, unsigned char* ucBlue_ptr, unsigned char ucLength);
void SendStrip_Off(unsigned char ucLength);

#endif /* RGBOOSTER_H_ */