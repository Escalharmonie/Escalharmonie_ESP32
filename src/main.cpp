// INCLUDES
#include <Arduino.h>
#include <MessageManager.h>
#include <Sensor.h>


// PREPROCESSOR DIRECTIVES
#define MULTIPLEXER_1_ADDRESS 0x70
#define DEV_I2C Wire
#define NB_SENSORS 8


// GLOBAL VARIABLES DEFINITION
MessageManager msg = MessageManager();

VL53L4CX sensor_VL53L4CX_1(&DEV_I2C, 4);
VL53L4CX sensor_VL53L4CX_2(&DEV_I2C, 5);
VL53L4CX sensor_VL53L4CX_3(&DEV_I2C, 6);
VL53L4CX sensor_VL53L4CX_4(&DEV_I2C, 7);
VL53L4CX sensor_VL53L4CX_5(&DEV_I2C, 8);
VL53L4CX sensor_VL53L4CX_6(&DEV_I2C, 9);
VL53L4CX sensor_VL53L4CX_7(&DEV_I2C, 10);
VL53L4CX sensor_VL53L4CX_8(&DEV_I2C, 11);
Sensor sensor1 = Sensor(&sensor_VL53L4CX_1, 1, MULTIPLEXER_1_ADDRESS, 0, &DEV_I2C, &msg);
Sensor sensor2 = Sensor(&sensor_VL53L4CX_2, 2, MULTIPLEXER_1_ADDRESS, 1, &DEV_I2C, &msg);
Sensor sensor3 = Sensor(&sensor_VL53L4CX_3, 3, MULTIPLEXER_1_ADDRESS, 2, &DEV_I2C, &msg);
Sensor sensor4 = Sensor(&sensor_VL53L4CX_4, 4, MULTIPLEXER_1_ADDRESS, 3, &DEV_I2C, &msg);
Sensor sensor5 = Sensor(&sensor_VL53L4CX_5, 5, MULTIPLEXER_1_ADDRESS, 4, &DEV_I2C, &msg);
Sensor sensor6 = Sensor(&sensor_VL53L4CX_6, 6, MULTIPLEXER_1_ADDRESS, 5, &DEV_I2C, &msg);
Sensor sensor7 = Sensor(&sensor_VL53L4CX_7, 7, MULTIPLEXER_1_ADDRESS, 6, &DEV_I2C, &msg);
Sensor sensor8 = Sensor(&sensor_VL53L4CX_8, 8, MULTIPLEXER_1_ADDRESS, 7, &DEV_I2C, &msg);
Sensor* sensors[] = {
    &sensor1,
    &sensor2,
    &sensor3,
    &sensor4,
    &sensor5,
    &sensor6,
    &sensor7,
    &sensor8,
};


// FUNCTIONS DEFINITION
void initSensors();
void readSensors();


// MAIN PROGRAM
void setup()
{
    Serial.begin(115200);
    DEV_I2C.begin();

    // Optional: change I2C bus clock speed (in Hz) to increase FPS
    // Higher speed means higher FPS but also higher risk of errors
    // Default speed is 100kHz ?
    DEV_I2C.setClock(400000);

    initSensors();
}

void loop()
{
    readSensors();
}

void initSensors()
{
    for (int i = 0; i < NB_SENSORS; i++)
    {        
        sensors[i]->switchMultiplexerChannel();
        sensors[i]->init();
    }
}

void readSensors()
{
    msg.init();

    for (int i = 0; i < NB_SENSORS; i++)
    {        
        VL53L4CX_MultiRangingData_t* sensorData_ptr;
        sensors[i]->switchMultiplexerChannel();
        sensorData_ptr = sensors[i]->getMeasure();

        if(sensorData_ptr == nullptr)
        {        
            continue;
        }

        msg.parse(sensorData_ptr, i + 1);
    }

    msg.sendMsg();
}
