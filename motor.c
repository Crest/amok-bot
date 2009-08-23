#include "pwm.h"

#define SCALING_FACTOR (655)

void motor_accelerate( uint8_t motor, int8_t percent ) {
	if ( percent > 100 || percent < -100 )
		return;
	
	set_direction( motor, percent < 0 ? REVERSE : FORWARD );
	set_speed( motor, abs(percent) * SCALING_FACTOR );
}

void motor_break( uint8_t motor, uint8_t percent ) {
	if ( percent > 100 )
	set_direction( motor, BREAK );
	set_speed( motor, percent * SCALING_FACTOR );
}
