#include "OPT3001.h"

OPT3001 sensor;

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting up OPT3001...");

    Wire.begin();

    // Set configuration parameters.
    sensor.config.conversion_time = OPT3001_CONVERSION_TIME::ILLU_800MS;
    sensor.config.mode = OPT3001_MODE::CONTINUOUS;
    sensor.config.range = OPT3001_RANGE::OPT3001_RANGE_AUTO;
    sensor.begin();
}

void loop()
{
    uint32_t reading = sensor.get_illuminance();
    Serial.print("Reading: ");
    Serial.print(reading);
    Serial.println(" lux");
    delay(1000);
}
