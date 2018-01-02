/** ***************************************************************************
 * @file utils.h
 * @brief Standard utilities for uC internals (timer,adc,gpio,...)
 *
 * - GPIO
 * @n Set pin directions and enable/disable the power LED driver circuit.
 * 
 * - PWM
 * @n Setup and control the PWM pin used for dimming the power LED
 * 
 * - ADC
 * @n Measure the voltage of the temperature sensor to determine the power LED temperature.
 * 
 * - Utilities
 * @n Map function for converting variable to a different number range and a 1ms wait routine.
 *
 * @author lopeslen, nosedmar
 * @date 14.11.2017
 *****************************************************************************/


#ifndef UTILS_H_
#define UTILS_H_

#define PORT_PLED			PORTB	///< output port for power LED
#define PIN_PLED			PINB	///< input port for power LED
#define DDR_PLED			DDRB	///< data direction register for power LED
#define PLED_DISABLE		0		///< power LED disable pin
#define PLED_PWM			1		///< power LED PWM pin


//GPIO
void portInit(void);
void enablePLED(void);
void disablePLED(void);
//PWM
void initPWM(unsigned char ucPercent);
void startPWM(void);
void stopPWM(void);
void setDuty(unsigned char ucPercent);
//ADC
void AD8bit_Init(void);
unsigned char AD8bit_Measurement(void);
//UTILITIES
void wait_1ms(unsigned int uiFactor);
long Map(long lData, long InMin, long InMax, long OutMin, long OutMax);

#endif /* UTILS_H_ */