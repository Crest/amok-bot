#include "pwm.h"
#include "sensor.h"
#include "tick.h"

#define ever (;;)

// WARNING: doesn't work for pointers!
#define bzero(p,n) memset( (p), 0, (n) )

int main() {
	init_pwm();
	init_sensor();
	init_tick();
	
	init_sensor_buffer();	
	
	while ( !booted ) {
		booted = spin_up_rotor();
	}
	
	for ever {
		read_sensors();
		calculate_average();
		choose_path();
		follow_path();
	}
}
