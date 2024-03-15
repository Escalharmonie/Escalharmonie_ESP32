#pragma once


#include <MessageManager.h>
#include <vl53l4cx_class.h>
#include <Wire.h>


/**
 * @class Sensor
 * @brief Represents a sensor object.
 * 
 * The Sensor class provides functionality to interact with a sensor device.
 * It includes methods to switch the multiplexer channel, initialize the sensor,
 * print the measurement, and retrieve the measurement data.
 */
class Sensor
{
    private:
        VL53L4CX_Error _status;
        uint8_t _multiplexerAddress;
        uint8_t _multiplexerChannel;
        TwoWire* _i2cBus_ptr;
        MessageManager* _msg_ptr;
        VL53L4CX_MultiRangingData_t _multiRangingData;
        VL53L4CX_MultiRangingData_t* _multiRangingData_ptr = &_multiRangingData;
        uint8_t _newDataReady = 0;

        uint8_t _setMultiplexerChannel(uint8_t channel);
        int _setAddress();
        int _setTimingBudget();
        int _start();

    public:
        VL53L4CX* VL53L4CX_sensor_ptr;
        int id;
        bool isHealthy = true;

        Sensor(VL53L4CX *sensor_ptr, int id, uint8_t multiplexerAddress, uint8_t multiplexerChannel, TwoWire *i2cBus_ptr, MessageManager *msg_ptr);
        void switchMultiplexerChannel();
        void init();
        VL53L4CX_MultiRangingData_t* getMeasure();
};

