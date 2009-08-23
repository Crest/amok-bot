#include "sensor.h"

#define ADC_ENABLE           (1 << ADEN)
#define ADC_FREE_RUNNING     (1 << ADATE)
#define ADC_INTERRUPT_ENABLE (1 << ADIE)
#define ADC_PRESCALER_128    ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))
#define ADC_EXTERNAL_AVCC    (1 << REFS0)
#define ADC_FIRST            (0x01)
#define ADC_ALL_BUT_FIRST    (0xFF &~ (ADC_FIRST))

#define TIMER2_OVERFLOW_INTERRUPT_ENABLE (1 << TOIE2)
#define TIMER2_PRESCALER_256             ((1 << CS22) | (1 << CS21))

#include <avr/interrupt.h>
#include <avr/io.h>

static volatile uint8_t next_mask, mask, index;

ISR( TIMER2_OVF_vect ) {
	index     = 0x00;
	mask      = ADC_FIRST;
	next_mask = ADC_ALL_BUT_FIRST;
	
	PORTA  = ADC_ALL_BUT_FIRST;
	DDRA   = ADC_ALL_BUT_FIRST;
	ADMUX  = ADC_EXTERNAL_AVCC;
	ADCSRA = ADC_ENABLE
               | ADC_FREE_RUNNING
	       | ADC_INTERRUPT_ENABLE
	       | ADC_PRESCALER_128;
}

ISR( ADC_vect ) {
	uint16_t sample;
	
	PORTA     = next_mask;
	DDRA      = next_mask;
	next_mask = (next_mask << 1) | 1;
	
	sample            = ADC;
	sensor[index++]   = sample;
	sensor_updated   |= mask;
	mask            <<= 1;
	ADMUX             = index | ADC_EXTERNAL_AVCC;
	
	if ( index == 7 )
		ADCSRA &= ~ADC_ENABLE;
}

inline static void init_porta() {
	DDRA  = 0x7F;
	PORTA = 0x7F;
}


inline static void init_timer2() {
	TIMSK2 = TIMER2_OVERFLOW_INTERRUPT_ENABLE;
	TCCR2B = TIMER2_PRESCALER_256;
}

void init_sensor() {
	init_porta();
	init_timer2();
}

void sensor_start() {
	TIMSK2 |= TIMER2_OVERFLOW_INTERRUPT_ENABLE;
}

void sensor_stop() {
	TIMSK2 &= ~TIMER2_OVERFLOW_INTERRUPT_ENABLE;
	TCNT2 = 0x00;
}
