#include <Sensor.h>

/**
 * @brief Constructor for the Sensor class.
 * 
 * @param sensor_ptr Pointer to the VL53L4CX sensor object.
 * @param id The ID of the sensor.
 * @param multiplexerAddress The I2C address of the multiplexer.
 * @param multiplexerChannel The output channel of the multiplexer.
 * @param i2cBus_ptr Pointer to the TwoWire object for I2C communication.
 * @param msg_ptr Pointer to the MessageManager object for message handling.
 */
Sensor::Sensor(VL53L4CX *sensor_ptr, int id, uint8_t multiplexerAddress, uint8_t multiplexerChannel, TwoWire *i2cBus_ptr, MessageManager *msg_ptr) {
    this->VL53L4CX_sensor_ptr = sensor_ptr;
    this->id = id;
    this->_multiplexerAddress = multiplexerAddress;
    this->_multiplexerChannel = this->_setMultiplexerChannel(multiplexerChannel);
    this->_i2cBus_ptr = i2cBus_ptr;
    this->_msg_ptr = msg_ptr;
}

/**
 * @brief Sets the multiplexer output channel for the sensor.
 * 
 * @param channel The channel number to set. Must be between 0 and 7.
 * @return The channel number if it is valid, otherwise returns 255.
 */
uint8_t Sensor::_setMultiplexerChannel(uint8_t channel){
    if(channel > 7){
        Serial.print("Sensor #");
        Serial.print(this->id);
        Serial.println(" error: Multiplexer channel must be between 0 and 7");
        return 255;
    }

    return channel;
}

/**
 * @brief Switches to the multiplexer output channel to which the sensor is connected.
 * @note This function assumes that the I2C bus has been initialized and the multiplexer address and channel have been set.
 */
void Sensor::switchMultiplexerChannel(){
    this->_i2cBus_ptr->beginTransmission(this->_multiplexerAddress);
    this->_i2cBus_ptr->write(1 << this->_multiplexerChannel);
    this->_i2cBus_ptr->endTransmission();
}

/**
 * @brief Initializes the sensor.
 */
void Sensor::init(){
    this->_setAddress();

    // Optional: set the timing budget. Allows to reduce the time between measurements.
    // this->_setTimingBudget();

    this->_start();
}

/**
 * @brief Sets the address for the sensor, default is 0x52.
 * If the address is not set successfully, the sensor is marked as unhealthy
 * and ignored for the rest of the program.
 * 
 * @return 0 if the address is set successfully, -1 otherwise.
 */
int Sensor::_setAddress(){
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

/**
 * @brief Sets the timing budget for the sensor (in micro seconds).
 * @note Optionnal, defaul value is 33ms. Make test to find the right value for speed and stability.
 * If the timing budget is not set successfully, the sensor is marked as
 * unhealthy and ignored for the rest of the program. 
 * 
 * @return 0 if the timing budget is set successfully, -1 otherwise.
 */
int Sensor::_setTimingBudget(){
    this->_status = this->VL53L4CX_sensor_ptr->VL53L4CX_SetMeasurementTimingBudgetMicroSeconds(10000);

    if(this->_status == VL53L4CX_ERROR_NONE){
        Serial.print("Succesfully set timing budget for sensor #");
        Serial.println(this->id);
        return 0;
    }
    else{
        Serial.print("Error setting timing budget for sensor #");
        Serial.print(this->id);
        Serial.print(" (code: ");
        Serial.print(this->_status);
        Serial.println(").");
        this->isHealthy = false;
        return -1;
    }
}

/**
 * @brief Starts the measurement for the sensor.
 * If the measurement is not started successfully, the sensor is marked as
 * unhealthy and ignored for the rest of the program.
 * 
 * @return 0 if the measurement is started successfully, -1 otherwise.
 */
int Sensor::_start(){
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

/**
 * @brief Get the measurement data from the sensor.
 * 
 * This method retrieves the measurement data from the VL53L4CX sensor.
 * If the sensor is not healthy, it returns nullptr.
 * If new data is ready, it gets the data and clears the interrupt.
 * It returns a pointer to the data, whether it has been updated or not.
 * 
 * @return VL53L4CX_MultiRangingData_t* A pointer to the measurement data.
 */
VL53L4CX_MultiRangingData_t* Sensor::getMeasure()
{
    if(!this->isHealthy){
        return nullptr;
    }

    int status = this->VL53L4CX_sensor_ptr->VL53L4CX_GetMeasurementDataReady(&this->_newDataReady);

    // If new data is ready, get it and clear the interrupt
    if ((!status) && (this->_newDataReady != 0))
    {
        status = this->VL53L4CX_sensor_ptr->VL53L4CX_GetMultiRangingData(_multiRangingData_ptr);
        if (status == 0)
        {
            status = this->VL53L4CX_sensor_ptr->VL53L4CX_ClearInterruptAndStartMeasurement();
        }
    }

    // Return a pointer to the data, whether it's has been updated or not
    return _multiRangingData_ptr;
}
