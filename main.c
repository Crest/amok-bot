#include "pwm.h"
#include "sensor.h"
#include "tick.h"

int main() {
	init_pwm();
	init_sensor();
	init_tick();
	
}
