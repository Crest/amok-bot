#ifndef _PWM_H
#define _PWM_H

#include <inttypes.h>

#define FORWARD 0
#define REVERSE 1
#define BREAK   2

#define MOTOR_A1 0
#define MOTOR_A2 1
#define MOTOR_B1 2
#define MOTOR_B2 3

#define VALID_MOTOR(x)   ((x) >= (MOTOR_A1) && (x) <= (MOTOR_B2))
#define INVALID_MOTOR(x) ((x) <  (MOTOR_A1) && (x) >  (MOTOR_B2))

#define ZERO 0

#define INITIAL_SPEED_A1 (ZERO)
#define INITIAL_SPEED_A2 (ZERO)
#define INITIAL_SPEED_B1 (ZERO)
#define INITIAL_SPEED_B2 (ZERO)

#define INITIAL_DIRECTION_A1 (BREAK)
#define INITIAL_DIRECTION_A2 (BREAK)
#define INITIAL_DIRECTION_B1 (BREAK)
#define INITIAL_DIRECTION_B2 (BREAK)

void init_pwm();

void set_speed( uint8_t index, uint16_t speed );
void set_direction( uint8_t index, uint8_t direction );

#endif /* _PWM_H */
