#include <MessageManager.h>


MessageManager::MessageManager()
{
}

/**
 * @brief Parses the sensor data and adds it to the message.
 * 
 * This function takes a pointer to the `VL53L4CX_MultiRangingData_t` structure containing the sensor data
 * and the sensor ID as parameters. It parses the sensor data and adds it to the message in JSON format.
 * 
 * @param sensorData_ptr A pointer to the `VL53L4CX_MultiRangingData_t` structure containing the sensor data.
 * @param sensorId The ID of the sensor.
 */
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

/**
 * @brief Sends the message by serializing it to JSON and printing it to the Serial port.
 */
void MessageManager::sendMsg()
{
    serializeJson(msg, Serial);
    Serial.println("");
}

/**
 * @brief Clears the message and sets the JSON-RPC version and method.
 */
void MessageManager::init()
{
    msg.clear();
    msg["jsonrpc"] = "2.0";
    msg["method"] = "update";
}