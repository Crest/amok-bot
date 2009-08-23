#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>
#include "pwm.h"

#define ALL       0xFF
#define UART_MASK 0x03
#define I2C_MASK  0x03
#define MASK_A    ((ALL) & ~(UART_MASK))
#define MASK_B    ((ALL) & ~(I2C_MASK ))


#define PD0 0
#define PD1 1
#define PD2 2
#define PD3 3 
#define PD4 4
#define PD5 5
#define PD6 6
#define PD7 7

#define PC0 0
#define PC1 1
#define PC2 2
#define PC3 3
#define PC4 4
#define PC5 5
#define PC6 6
#define PC7 7

#define _ENABLE_A1  PD5
#define _ENABLE_A2  PD7
#define _ENABLE_B1  PC4
#define _ENABLE_B2  PC7
#define _FORWARD_A1 PD3
#define _FORWARD_A2 PD2
#define _FORWARD_B1 PC2
#define _FORWARD_B2 PC5
#define _REVERSE_A1 PD4
#define _REVERSE_A2 PD6
#define _REVERSE_B1 PC3
#define _REVERSE_B2 PC6

#define ENABLE_A1  (1 << (_ENABLE_A1))
#define ENABLE_A2  (1 << (_ENABLE_A2))
#define ENABLE_B1  (1 << (_ENABLE_B1))
#define ENABLE_B2  (1 << (_ENABLE_B2))
#define ENABLE_A   ((ENABLE_A1)  | (ENABLE_A2))
#define ENABLE_B   ((ENABLE_B1)  | (ENABLE_B2))
#define FORWARD_A1 (1 << (_FORWARD_A1))
#define FORWARD_A2 (1 << (_FORWARD_A2))
#define FORWARD_B1 (1 << (_FORWARD_B1))
#define FORWARD_B2 (1 << (_FORWARD_B2))
#define FORWARD_A  ((FORWARD_A1) | (FORWARD_A2))
#define FORWARD_B  ((FORWARD_B1) | (FORWARD_B2))
#define REVERSE_A1 (1 << (_REVERSE_A1))
#define REVERSE_A2 (1 << (_REVERSE_A2))
#define REVERSE_B1 (1 << (_REVERSE_B1))
#define REVERSE_B2 (1 << (_REVERSE_B2))
#define REVERSE_A  ((REVERSE_A1) | (REVERSE_A2))
#define REVERSE_B  ((REVERSE_B1) | (REVERSE_B2))
#define BREAK_A1   ((FORWARD_A1) | (REVERSE_A1))
#define BREAK_A2   ((FORWARD_A2) | (REVERSE_A2))
#define BREAK_B1   ((FORWARD_B1) | (REVERSE_B1))
#define BREAK_B2   ((FORWARD_B2) | (REVERSE_B2))
#define BREAK_A    ((BREAK_A1  ) | (BREAK_A2  ))
#define BREAK_B    ((BREAK_B1  ) | (BREAK_B2  ))

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) < (x) ? (y) : (x))

static uint16_t speed_a[2] , speed_b[2];
static uint8_t  initial_a  , initial_b;
static uint16_t slower_a   , slower_b;
static uint16_t faster_a   , faster_b;
static uint8_t  direction_a, direction_b;
static uint8_t  enable_a[2] = { 0, 0 };
static uint8_t  enable_b[2] = { 0, 0 };

ISR( TIMER1_COMPA_vect ) {
	static uint8_t index = 0;
	PORTC = enable_a[index] | direction_a;
	OCR1A = faster_a;
	index++; index &= 1;
}

ISR( TIMER1_COMPB_vect ) {
	static uint8_t index = 0;
	PORTD = enable_b[index] | direction_b;
	OCR1B = faster_b;
	index++; index &= 1;
}

ISR( TIMER1_OVF_vect ) {
	PORTD = initial_a | direction_a;
	PORTC = initial_b | direction_b;
	OCR1A = slower_a;
	OCR1B = slower_b;
}

static inline void set_speed_a( uint8_t index, uint16_t speed ) {
	uint8_t  initial = ( speed      ? ENABLE_A1 : 0 )
			 | ( speed_a[1] ? ENABLE_A2 : 0 );
	uint8_t  enable;
	uint16_t slower;
	uint16_t faster;
	
	index &= 1;
	if ( index == 0 ) {
		enable  = speed < speed_a[1] ? ENABLE_A2
			                     : ( speed == speed_a[1] ? 0 : ENABLE_A1 );
		slower  = MIN( speed, speed_a[1] );
		faster  = MAX( speed, speed_a[1] );

	} else {
		enable  = speed < speed_a[0] ? ENABLE_A1
		                             : ( speed == speed_a[0] ? 0 : ENABLE_A2 );
		slower  = MIN( speed, speed_a[0] );
		faster  = MAX( speed, speed_a[0] );
	}
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		speed_a[index] = speed;
		initial_a      = initial;
		slower_a       = slower;
		faster_a       = faster;
		enable_a[0]    = enable;
	}
}

static inline void set_speed_b( uint8_t index, uint16_t speed ) {
	uint8_t  initial = ( speed      ? ENABLE_B1 : 0 )
			 | ( speed_b[1] ? ENABLE_B2 : 0 );
	uint8_t  enable;
	uint16_t slower;
	uint16_t faster;
	
	index &= 1;
	if ( index == 0 ) {
		enable  = speed < speed_b[1] ? ENABLE_B2
			                     : ( speed == speed_b[1] ? 0 : ENABLE_B1 );
		slower  = MIN( speed, speed_b[1] );
		faster  = MAX( speed, speed_b[1] );

	} else {
		enable  = speed < speed_b[0] ? ENABLE_B1
		                             : ( speed == speed_b[0] ? 0 : ENABLE_B2 );
		slower  = MIN( speed, speed_b[0] );
		faster  = MAX( speed, speed_b[0] );
	}
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		speed_b[index] = speed;
		initial_b      = initial;
		slower_b       = slower;
		faster_b       = faster;
		enable_b[0]    = enable;
	}
}

void set_speed( uint8_t index, uint16_t speed ) {
	if ( index <= MOTOR_A2 )
		set_speed_a( index, speed );
	else if ( index <= MOTOR_B2 )
		set_speed_b( index, speed );
}


void set_direction( uint8_t index, uint8_t direction ) {
	index &= 3;
	if ( direction > BREAK ) return;

	uint8_t lookup[12] = { FORWARD_A1, FORWARD_A2, FORWARD_B1, FORWARD_B2,
			       REVERSE_A1, REVERSE_A2, REVERSE_B1, REVERSE_B2,
			       BREAK_A1  , BREAK_A2  , BREAK_B1  , BREAK_B2    };
	switch ( index ) {
	case 0:
		direction_a = lookup[( direction << 2 ) + index] | ( direction_a & ~BREAK_A1 );
		break;
	case 1:
		direction_a = lookup[( direction << 2 ) + index] | ( direction_a & ~BREAK_A2 );
		break;
	case 2:
		direction_b = lookup[( direction << 2 ) + index] | ( direction_b & ~BREAK_B1 );
		break;
	case 3:
		direction_b = lookup[( direction << 2 ) + index] | ( direction_b & ~BREAK_B2 );
		break;
	}
}

inline static void init_jtag() {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		MCUCR = (1 << JTD);
		MCUCR = (1 << JTD);
	}
}

inline static void init_pwm_ports() {
	DDRD |= MASK_A;
	DDRC |= MASK_B;
}

inline static void init_pwm_timer() {
	TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B) | (1 << TOIE1);
	TCCR1B  = (1 << CS10);
}

inline static void init_speed() {
	set_speed( MOTOR_A1, INITIAL_SPEED_A1 );
	set_speed( MOTOR_A2, INITIAL_SPEED_A2 );
	set_speed( MOTOR_B1, INITIAL_SPEED_B1 );
	set_speed( MOTOR_B2, INITIAL_SPEED_B2 );
}

inline static void init_direction() {
	set_direction( MOTOR_A1, INITIAL_DIRECTION_A1 );
	set_direction( MOTOR_A2, INITIAL_DIRECTION_A2 );
	set_direction( MOTOR_B1, INITIAL_DIRECTION_B1 );
	set_direction( MOTOR_B2, INITIAL_DIRECTION_B2 );
}

void init_pwm() {
	init_speed();
	init_direction();
	init_jtag();
	init_pwm_ports();
	init_pwm_timer();
}
