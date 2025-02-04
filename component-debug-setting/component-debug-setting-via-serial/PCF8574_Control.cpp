#include "Arduino.h"
#include "PCF8574_Control.h"

// Variables to store the current state of all pins for each module
byte pcf8574_state_1 = 0xFF; // Default all pins HIGH (off) for module 1
byte pcf8574_state_2 = 0xFF; // Default all pins HIGH (off) for module 2

// Function to set the state of a specific pin on a specific PCF8574 module
void setPCF8574Pin(uint8_t module, uint8_t pin, bool state) {
  // Validate the module number (1 or 2)
  if (module != 1 && module != 2) {
    Serial.println("Error: Module number must be 1 or 2.");
    return;
  }

  // Validate the pin number (0-7)
  if (pin > 7) {
    Serial.println("Error: Pin number must be between 0 and 7.");
    return;
  }

  // Determine which module's state to update
  byte* pcf8574_state = (module == 1) ? &pcf8574_state_1 : &pcf8574_state_2;
  uint8_t pcf8574_address = (module == 1) ? PCF8574_ADDRESS_1 : PCF8574_ADDRESS_2;

  // Update the state of the specific pin
  if (state == LOW) {
    *pcf8574_state &= ~(1 << pin); // Set the pin LOW (turn on)
  } else {
    *pcf8574_state |= (1 << pin);  // Set the pin HIGH (turn off)
  }

  // Send the updated state to the PCF8574 module
  Wire.beginTransmission(pcf8574_address);
  Wire.write(*pcf8574_state);
  Wire.endTransmission();
}

// Function to set all pins on both modules to a specific state
void setAllPCF8574Pins(bool state) {
  // Determine the new state for all pins
  byte new_state = (state == LOW) ? 0x00 : 0xFF;

  // Update the state for module 1
  pcf8574_state_1 = new_state;
  Wire.beginTransmission(PCF8574_ADDRESS_1);
  Wire.write(pcf8574_state_1);
  Wire.endTransmission();

  // Update the state for module 2
  pcf8574_state_2 = new_state;
  Wire.beginTransmission(PCF8574_ADDRESS_2);
  Wire.write(pcf8574_state_2);
  Wire.endTransmission();
}