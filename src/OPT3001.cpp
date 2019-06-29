#include "OPT3001.h"

/**
 * Set the address of the sensor.
 * The address is set with hardware, depending on the configuration of the ADDR pin.
 *
 * ADDR -> GND = 0x44
 * ADDR -> VDD = 0x45
 * ADDR -> SDA = 0x46
 * ADDR -> SCL = 0x47
 *
 * @param address: Address to set the driver to.
 */
void OPT3001::set_address(uint8_t address)
{
    // Restrict the input address to the valid range
    address |= 0b1000100;
    address = address & 0b1000111;
    device_address = address;
}

/**
 * Write a value to a register using I2C
 *
 * @param input: Byte to write to the register.
 * @param address: Address of register to write to.
 * @return: Success/error result of the write.
 */
bool OPT3001::write(uint8_t *input, OPT3001_reg_t address, uint8_t length)
{
    bool result = true;
    Wire.beginTransmission(device_address);
    Wire.write(address);
    for (size_t i = 0; i < length; i++)
    {
        Wire.write(input[i]);
    }

    if (Wire.endTransmission() != 0)
    {
        result = false;
    }
    return result;
}

/**
 * Read a specified number of bytes using the I2C bus.
 * @param output: The buffer in which to store the read values.
 * @param address: Register address to read (or starting address in burst reads)
 * @param length: Number of bytes to read.
 */
bool OPT3001::read(uint8_t *output, OPT3001_reg_t address, uint8_t length)
{
    bool result = true;
    Wire.beginTransmission(address);
    Wire.write(address);
    if (Wire.endTransmission() != 0)
        result = false;

    else // OK, all worked, keep going
    {
        Wire.requestFrom(address, length);
        for (size_t i = 0; (i < length) and Wire.available(); i++)
        {
            uint8_t c = Wire.read();
            output[i] = c;
        }
    }
    return result;
}

/**
 *
 */
void OPT3001::apply_config() { write((uint8_t *)&config, OPT3001_REGISTER::OPT3001_CONFIG); }

/**
 *
 */
OPT3001_config_t OPT3001::read_config()
{
    OPT3001_config_t current_config;
    read((uint8_t *)&current_config, OPT3001_REGISTER::OPT3001_CONFIG);
    return current_config;
}

/**
 * Check that things work // TODO - documentation
 */
bool OPT3001::check_comms()
{
    uint16_t manufacturer_id;
    read((uint8_t *)&manufacturer_id, OPT3001_REGISTER::OPT3001_MANUFACTURER_ID);

    // Make sure the manufacturer's ID matches the expected value ('TI')
    bool success = false;
    if (manufacturer_id == OPT3001_MANUFACTURER_ID)
        success = true;

    return success;
}

/**
 * Calculate the optical power measured by the sensor.
 * @return: Optical power of incident light in nW/cm^2
 */
uint32_t OPT3001::get_illuminance()
{
    OPT3001_result_t result;
    read((uint8_t *)&result, OPT3001_REGISTER::OPT3001_RESULT);

    // Calculate optical power [ref: Equation 1, OPT3001 Datasheet]
    // Optical_Power = R[11:0] * 2^(E[3:0]) * 1.2 nW/cm^2
    uint32_t exponent = 1 << result.exponent;
    uint32_t illuminance = result.reading * exponent * 0.01;
    return illuminance;
}

bool OPT3001::begin(uint8_t address)
{
    set_address(address);
    bool working = check_comms();
    if (working)
    {
        apply_config();
    }

    return working;
}

void OPT3001::set_high_limit(OPT3001_result_t high_limit)
{
    write((uint8_t *)&high_limit, OPT3001_REGISTER::OPT3001_HIGH_LIMIT);
}

OPT3001_result_t OPT3001::get_high_limit()
{
    OPT3001_result_t limit;
    read((uint8_t *)&limit, OPT3001_REGISTER::OPT3001_HIGH_LIMIT);
    return limit;
}

void OPT3001::set_low_limit(OPT3001_result_t low_limit)
{
    write((uint8_t *)&low_limit, OPT3001_REGISTER::OPT3001_LOW_LIMIT);
}

/**
 * Get the low limit level from the sensor.
 * The default low-level after reset is 0.
 */
OPT3001_result_t OPT3001::get_low_limit()
{
    OPT3001_result_t limit;
    read((uint8_t *)&limit, OPT3001_REGISTER::OPT3001_LOW_LIMIT);
    return limit;
}
