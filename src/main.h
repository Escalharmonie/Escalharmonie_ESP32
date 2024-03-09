#pragma once


#include <Arduino.h>
#include <Sensor.h>


#define MULTIPLEXER_1_ADDRESS 0x70
#define DEV_I2C Wire
#define NB_SENSORS 2


VL53L4CX sensor_VL53L4CX_1(&DEV_I2C, 4);
VL53L4CX sensor_VL53L4CX_2(&DEV_I2C, 5);
Sensor sensor1 = Sensor(&sensor_VL53L4CX_1, 1, MULTIPLEXER_1_ADDRESS, 0, &DEV_I2C);
Sensor sensor2 = Sensor(&sensor_VL53L4CX_2, 2, MULTIPLEXER_1_ADDRESS, 1, &DEV_I2C);
Sensor* sensors[] = {
    &sensor1,
    &sensor2
};


void i2cScanner();
void multiplexerI2cScanner(uint8_t address);
