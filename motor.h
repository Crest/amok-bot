#ifndef _MOTOR_H
#define _MOTOR_H

#include <stdint.h>

void motor_accelerate( uint8_t motor, int8_t percent );
void motor_break     ( uint8_t motor, int8_t percent );

#endif /* _MOTOR_H */
