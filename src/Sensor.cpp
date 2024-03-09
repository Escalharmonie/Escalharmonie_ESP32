#include <Sensor.h>

Sensor::Sensor(VL53L4CX *sensor_ptr, int id, uint8_t multiplexerAddress, uint8_t multiplexerChannel, TwoWire *i2cBus_ptr, MessageManager *msg_ptr) {
    this->VL53L4CX_sensor_ptr = sensor_ptr;
    this->id = id;
    this->_multiplexerAddress = multiplexerAddress;
    this->_multiplexerChannel = this->_setMultiplexerChannel(multiplexerChannel);
    this->_i2cBus_ptr = i2cBus_ptr;
    this->_msg_ptr = msg_ptr;
}

uint8_t Sensor::_setMultiplexerChannel(uint8_t channel){
    if(channel > 7){
        Serial.print("Sensor #");
        Serial.print(this->id);
        Serial.println(" error: Multiplexer channel must be between 0 and 7");
        return 255;
    }

    return channel;
}

void Sensor::switchMultiplexerChannel(){
    Serial.print("Switching to channel #");
    Serial.print(this->_multiplexerChannel);
    Serial.print(" of multiplexer at address 0x");
    Serial.println(this->_multiplexerAddress, HEX);

    this->_i2cBus_ptr->beginTransmission(this->_multiplexerAddress);
    this->_i2cBus_ptr->write(1 << this->_multiplexerChannel);
    this->_i2cBus_ptr->endTransmission();
}

void Sensor::init(){
    this->setAddress();
    this->start();
}

int Sensor::setAddress(){
    this->_status = this->VL53L4CX_sensor_ptr->InitSensor(VL53L4CX_DEFAULT_DEVICE_ADDRESS);

    if(this->_status == VL53L4CX_ERROR_NONE){
        Serial.print("Succesfully set address 0x");
        Serial.print(VL53L4CX_DEFAULT_DEVICE_ADDRESS, HEX);
        Serial.print(" for sensor #");
        Serial.println(this->id);
        return 0;
    }
    else{
        Serial.print("Error setting address 0x");
        Serial.print(VL53L4CX_DEFAULT_DEVICE_ADDRESS, HEX);
        Serial.print(" for sensor #");
        Serial.print(this->id);
        Serial.print(" (code: ");
        Serial.print(this->_status);
        Serial.println("). Ignoring sensor for the rest of the program.");
        this->isHealthy = false;
        return -1;
    }
}

int Sensor::start(){
    this->_status = this->VL53L4CX_sensor_ptr->VL53L4CX_StartMeasurement();

    if(this->_status == VL53L4CX_ERROR_NONE){
        Serial.print("Succesfully started measurements for sensor #");
        Serial.println(this->id);
        return 0;
    }
    else{
        Serial.print("Error starting measurements for sensor #");
        Serial.print(this->id);
        Serial.print(" (code: ");
        Serial.print(this->_status);
        Serial.println("). Ignoring sensor for the rest of the program.");
        this->isHealthy = false;
        return -1;
    }
}

void Sensor::printMeasure()
{
    if(!this->isHealthy){
        return;
    }

    VL53L4CX_MultiRangingData_t MultiRangingData;
    VL53L4CX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
    uint8_t NewDataReady = 0;
    int nbObjectsFound = 0, j;
    char report[64];
    int status;

    do {
        status = this->VL53L4CX_sensor_ptr->VL53L4CX_GetMeasurementDataReady(&NewDataReady);
    } while (!NewDataReady);

    if ((!status) && (NewDataReady != 0))
    {
        status = this->VL53L4CX_sensor_ptr->VL53L4CX_GetMultiRangingData(pMultiRangingData);
        nbObjectsFound = pMultiRangingData->NumberOfObjectsFound;
        snprintf(report, sizeof(report), "Sensor #%d: Count=%1d, #Objs=%2d ",this->id, pMultiRangingData->StreamCount, nbObjectsFound);
        Serial.print(report);

        for (j = 0; j < nbObjectsFound; j++)
        {
            if (j != 0)
            {
                Serial.print("\r\n                               ");
            }

            Serial.print("status=");
            Serial.print(pMultiRangingData->RangeData[j].RangeStatus);
            Serial.print(", D=");
            Serial.print(pMultiRangingData->RangeData[j].RangeMilliMeter);
            Serial.print("mm");
        }

        Serial.println("");

        if (status == 0)
        {
            status = this->VL53L4CX_sensor_ptr->VL53L4CX_ClearInterruptAndStartMeasurement();
        }
    }
}

void Sensor::getMeasure()
{
    if(!this->isHealthy){
        return;
    }

    VL53L4CX_MultiRangingData_t MultiRangingData;
    VL53L4CX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
    uint8_t NewDataReady = 0;
    int status;

    do {
        status = this->VL53L4CX_sensor_ptr->VL53L4CX_GetMeasurementDataReady(&NewDataReady);
    } while (!NewDataReady);

    if ((!status) && (NewDataReady != 0))
    {
        status = this->VL53L4CX_sensor_ptr->VL53L4CX_GetMultiRangingData(pMultiRangingData);
        if (status == 0)
        {
            this->_msg_ptr->parse(pMultiRangingData, this->id);
            status = this->VL53L4CX_sensor_ptr->VL53L4CX_ClearInterruptAndStartMeasurement();
        }
    }
}
