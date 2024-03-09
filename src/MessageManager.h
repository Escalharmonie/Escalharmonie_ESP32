#pragma once


#include <ArduinoJson.h>
#include <vl53l4cx_class.h>


class MessageManager
{
    private:
        JsonDocument msg;

    public:
        MessageManager();
        void parse(VL53L4CX_MultiRangingData_t* sensorData_ptr, int sensorId);
        void sendMsg();
        void init();
};
