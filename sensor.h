#ifndef _SENSOR_H
#define _SENSOR_H

#include <stdio.h>

#define SENSOR_0    0
#define SENSOR_1    1
#define SENSOR_2    2
#define SENSOR_3    3
#define SENSOR_4    4
#define SENSOR_5    5
#define SENSOR_6    6
#define SENSOR_7    7
#define MAX_SENSORS 8

uint16_t sensor[MAX_SENSORS] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t  sensor_updated      = 0x00;

void init_sensor();

void sensor_start();
void sensor_stop();

#endif /* _SENSOR_H */
