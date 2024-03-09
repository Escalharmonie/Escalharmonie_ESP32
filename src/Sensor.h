#pragma once

#include <vl53l4cx_class.h>
#include <Wire.h>

class Sensor {
    private:
        VL53L4CX_Error _status;
        uint8_t _multiplexerAddress;
        uint8_t _multiplexerChannel;
        TwoWire* _i2cBus;

        uint8_t _setMultiplexerChannel(uint8_t channel);

    public:
        VL53L4CX* VL53L4CX_sensor_ptr;
        int id;
        bool isHealthy = true;

        Sensor(VL53L4CX *sensor_ptr, int id, uint8_t multiplexerAddress, uint8_t multiplexerChannel, TwoWire *i2cBus);
        void switchMultiplexerChannel();
        void init();
        int setAddress();
        int start();
        void printMeasure();
};

