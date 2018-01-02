/** ***************************************************************************
 * @file utils.c
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
 * @n Map function for converting variables to a different number range and a 1ms wait routine.
 *
 * @author lopeslen, nosedmar
 * @date 14.11.2017
 *****************************************************************************/


 #include <avr/io.h>
 #include "utils.h"


///////////////////////////////////////////////////////////////////////////////
// GPIO
///////////////////////////////////////////////////////////////////////////////


/** ***************************************************************************
 * @brief Initialize the GPIOs (power LED enable and PWM pin)
 * 
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void portInit(void)
{
	DDR_PLED |= (1<<PLED_DISABLE) | (1<<PLED_PWM);
	PORT_PLED |= (1<<PLED_DISABLE); // disable pled driver
	PORT_PLED &= ~(1<<PLED_PWM); // PWM pin low
}


/** ***************************************************************************
 * @brief Enables the power LED by pulling the enable pin low
 * 
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void enablePLED(void)
{
	PORT_PLED &= ~(1<<PLED_DISABLE); 
}


/** ***************************************************************************
 * @brief Disables the power LED by pulling the enable pin high
 * 
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void disablePLED(void)
{
	PORT_PLED |= (1<<PLED_DISABLE);
}


///////////////////////////////////////////////////////////////////////////////
// PWM
///////////////////////////////////////////////////////////////////////////////


/** ***************************************************************************
 * @brief Initialize the PWM pin and setup timer1.
 *
 * - non-interted output of pwm signal
 * - mode: 9bit fast pwm
 * - frequency @20MHz and prescaler 1: 39kHz
 * - no interrupts
 * 
 * @param [in] ucPercent: dutycycle in percent [0-100]
 * @return no return value
 *****************************************************************************/
void initPWM(unsigned char ucPercent)
{
	TCCR1A = (1<<COM1A1) | (1<<WGM11);	// non inverting output on pwm pin | mode: fast pwm 9bit
	TCCR1B = (1<<WGM12);				// mode: fast pwm 9bit
	TCCR1C = 0;
	TCNT1 = 0;
	OCR1A = Map(ucPercent,0,100,0,511); // dutycycle
	TIMSK1 = 0;							// no interrupts
}


/** ***************************************************************************
 * @brief Start the PWM output
 *
 * Timer1 is started by setting the prescaler to a non-zero value.
 * A prescaler of 1 yields the desired 39kHz.
 * 
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void startPWM(void)
{
	TCCR1B |= 0b00000001; // set prescaler to 1
}


/** ***************************************************************************
 * @brief Stop the PWM output
 *
 * Timer1 is stopped by setting the prescaler to zero.
 * 
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void stopPWM(void)
{
	TCCR1B &= ~0b00000111; // clear prescaler (turn off the counter)
}


/** ***************************************************************************
 * @brief Set the dutycycle of the PWM output
 *
 * The map function converts the percentage to the corresponding 9bit value
 * 
 * @param [in] ucPercent: dutycycle in percent [0-100]
 * @return no return value
 *****************************************************************************/
void setDuty(unsigned char ucPercent)
{
	OCR1A = Map(ucPercent,0,100,0,511);
}


///////////////////////////////////////////////////////////////////////////////
// ADC
///////////////////////////////////////////////////////////////////////////////


/** ***************************************************************************
 * @brief Initialize the ADC for the temperature measurment (ADC6)
 *
 * - reference: AVCC pin
 * - left adjusted (8bit)
 * - channel: ADC6 pin
 * - ADC clock prescaler: 128
 * 
 * @param [void] no input
 * @return no return value
 *****************************************************************************/
void AD8bit_Init(void)
{
	ADMUX = (1<<REFS0) | (1<<ADLAR) | (1<<MUX2) | (1<<MUX1); // AVCC as reference | left adjusted result (8bit) | ADC6 pin
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // ADC enable | ADC clock prescaler 128
	ADCSRB = 0x00;
}


/** ***************************************************************************
 * @brief Initialize the ADC for the temperature measurment (ADC6)
 *
 * - reference: AVCC pin
 * - left adjusted (8bit)
 * - channel: ADC6 pin
 * - ADC clock prescaler: 128
 * 
 * @param [void] no input
 * @return 8bit analog value
 *****************************************************************************/
unsigned char AD8bit_Measurement(void)
{
	ADCSRA |= (1<<ADSC); // start conversion

	while(!(ADCSRA & (1<<ADIF))); // poll busy flag
	ADCSRA = ADCSRA | (1<<ADIF); // clear busy flag

	return (ADCH);
}

///////////////////////////////////////////////////////////////////////////////
// UTILITIES
///////////////////////////////////////////////////////////////////////////////


/** ***************************************************************************
 * @brief wait 1ms*factor
 * 
 * Use timer0 to wait a multiple of 1ms. Timermode: CTC
 *
 * @param [in] uiFactor: 1ms multiplier
 * @return no return value
 *****************************************************************************/
void wait_1ms(unsigned int uiFactor)
{
	unsigned int i;
	TCCR0A = (1<<WGM01);
	TCCR0B = 0x00;
	TIFR0 =  (1<<OCF0B) | (1<<OCF0A) | (1<<TOV0); // clear all flags
	TIMSK0 = 0x00; // no interrupts
	TCNT0 = 0;
	OCR0A = 249; // used formula on page 99 in datasheet to calculate this value and prescaler for 1ms
	
	TCCR0B |= 0x03; // set prescaler to 64
	
	for(i=0;i<uiFactor;i++)
	{
		while(!(TIFR0&0x02));	// poll flag
		TIFR0 = 0x03;			// clear flags
	}
	TCCR0B &= ~(0x03);	// set prescaler to 0 (stop timer)
}


/** ***************************************************************************
 * @brief Map function for converting variables to a different number range.
 *
 * @param [in] lData: data to be converted
 * @param [in] InMin: minimal value of input range
 * @param [in] InMax: maximal value of input range
 * @param [in] OutMin: minimal value of output range
 * @param [in] OutMax: maximal value of output range
 * @return data in new number range
 *****************************************************************************/
long Map(long lData, long InMin, long InMax, long OutMin, long OutMax)
{
	return((lData-InMin)*(OutMax-OutMin)/(InMax-InMin)+OutMin);
}
