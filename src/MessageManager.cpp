#include <MessageManager.h>


MessageManager::MessageManager()
{
}

void MessageManager::parse(VL53L4CX_MultiRangingData_t* sensorData_ptr, int sensorId)
{
    JsonDocument sensor;
    sensor["id"] = sensorId;

    for(int i = 0; i < sensorData_ptr->NumberOfObjectsFound; i++){
        sensor["objects"][i]["distance"] = sensorData_ptr->RangeData[i].RangeMilliMeter;
        sensor["objects"][i]["status"] = sensorData_ptr->RangeData[i].RangeStatus;
    }

    msg["params"]["sensors"].add(sensor);
}

void MessageManager::sendMsg()
{
    serializeJson(msg, Serial);
    Serial.println("");
}

void MessageManager::init()
{
    msg.clear();
    msg["jsonrpc"] = "2.0";
    msg["method"] = "update";
}