#ifndef PCF8574_CONTROL_H
#define PCF8574_CONTROL_H

#include <Wire.h>

// I2C addresses for the two PCF8574 modules
#define PCF8574_ADDRESS_1 0x20 // Address of the first module
#define PCF8574_ADDRESS_2 0x21 // Address of the second module

// Function declarations
void setPCF8574Pin(uint8_t module, uint8_t pin, bool state);
void setAllPCF8574Pins(bool state);

#endif