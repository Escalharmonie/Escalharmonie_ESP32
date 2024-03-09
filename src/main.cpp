#include <main.h>


void setup()
{
    Serial.begin(115200);
    DEV_I2C.begin();

    for (int i = 0; i < NB_SENSORS; i++)
    {        
        sensors[i]->switchMultiplexerChannel();
        sensors[i]->init();
    }
}

void loop()
{
    for (int i = 0; i < NB_SENSORS; i++)
    {        
        sensors[i]->switchMultiplexerChannel();
        sensors[i]->printMeasure();
    }
}

void i2cScanner()
{
    Serial.println("i2cScanner()");
    byte error, address;
    int nDevices;
    Serial.println("Scanning...");
    nDevices = 0;
    for (address = 1; address < 127; address++)
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
            {
                Serial.print("0");
            }
            Serial.println(address, HEX);
            nDevices++;
        }
        else if (error == 4)
        {
            Serial.print("Unknow error at address 0x");
            if (address < 16)
            {
                Serial.print("0");
            }
            Serial.println(address, HEX);
        }
    }
    if (nDevices == 0)
    {
        Serial.println("No I2C devices found\n");
    }
    else
    {
        Serial.println("done\n");
    }
    delay(5000);
}

void multiplexerI2cScanner(uint8_t address){
    Serial.println("multiplexerI2cScanner()");

    for (uint8_t t=0; t<8; t++) {
      
        DEV_I2C.beginTransmission(address);
        DEV_I2C.write(1 << t);
        DEV_I2C.endTransmission();

        Serial.print("PCA Port #"); Serial.println(t);

        for (uint8_t addr = 0; addr<=127; addr++) {
            if (addr == address) continue;

            DEV_I2C.beginTransmission(addr);
            if (!DEV_I2C.endTransmission()) {
            Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
            }
        }
        }
    Serial.println("\ndone");
}