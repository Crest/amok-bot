#include <avr/interrupt.h>
#include <avr/io.h>
#include "tick.h"

ISR( TIMER2_COMPA_vect ) {
	uint8_t counter = OCR2A;
	counter += 78;
	OCR2A = counter;
	tick++;
}

void init_tick() {
	TIMSK2 |= TIMER2_COMPARE_INTERRUPT_A_ENABLE;
}

void tick_start() {
	TIMSK2 |= TIMER2_COMPARE_INTERRUPT_A_ENABLE;
}

void tick_stop() {
	TIMSK2 &= ~TIMER2_COMPARE_INTERRUPT_A_ENABLE;
}
