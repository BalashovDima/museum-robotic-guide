#define GS_NO_ACCEL

#include <Servo.h>
#include "PCF8574_Control.h"
#include <GyverStepper2.h>

// ------------------ Stepper motor
#define GS_NO_ACCEL
GStepper2<STEPPER4WIRE> stepper(2048, 7, 5, 6, 4); // Steps per revolution and pins
#define START_POINT_SENSOR_PIN 10
bool look_for_start_point = true;

// ------------------ Servo motor
Servo servo;

// ------------------ Laser pointer
#define LASER_PIN A3
#define LAZER_ANALOG_PIN // if using analog pin for laser pointer
void pointerOn() {
  #ifdef LAZER_ANALOG_PIN
    analogWrite(LASER_PIN, 255);
  #else
    digitalWrite(LASER_PIN, HIGH);
  #endif
}

void pointerOff() {
  #ifdef LAZER_ANALOG_PIN
    analogWrite(LASER_PIN, 0);  
  #else 
    digitalWrite(LASER_PIN, LOW);
  #endif
}

// Function to handle serial commands
void handleSerialCommand(String command) {
  if (command.length() < 2) return; // Invalid command

  char cmd = command[0]; // First character is the command
  int value = command.substring(1).toInt(); // Rest is the value

  switch (cmd) {
    case 'S': // Servo motor control
      if (value >= 0 && value <= 180) {
        servo.write(value);
        Serial.print("Servo set to ");
        Serial.println(value);
      } else {
        Serial.println("Invalid servo angle (0-180)");
      }
      break;

    case 'M': // Stepper motor control
      stepper.setTargetDeg((float)value);
      Serial.print("Stepper set to ");
      Serial.println(value);
      break;

    case 'H': // Turn GPIO expander pin HIGH (off)
      if (value >= 0 && value <= 15) { // Pins 0-7 for module 1, 8-15 for module 2
        uint8_t module = (value >= 8) ? 2 : 1; // Determine module based on pin number
        uint8_t pin = (value >= 8) ? value - 8 : value; // Map pin to 0-7 for the module
        setPCF8574Pin(module, pin, HIGH);
        Serial.print("GPIO module ");
        Serial.print(module);
        Serial.print(", pin ");
        Serial.print(pin);
        Serial.println(" turned ON");
      } else {
        Serial.println("Invalid GPIO pin (0-15)");
      }
      break;

    case 'L': // Turn GPIO expander pin LOW (on)
      if (value >= 0 && value <= 15) { // Pins 0-7 for module 1, 8-15 for module 2
        uint8_t module = (value >= 8) ? 2 : 1; // Determine module based on pin number
        uint8_t pin = (value >= 8) ? value - 8 : value; // Map pin to 0-7 for the module
        setPCF8574Pin(module, pin, LOW);
        Serial.print("GPIO module ");
        Serial.print(module);
        Serial.print(", pin ");
        Serial.print(pin);
        Serial.println(" turned OFF");
      } else {
        Serial.println("Invalid GPIO pin (0-15)");
      }
      break;

    case 'A': // Turn ALL GPIO pins ON (LOW)
      for (uint8_t module = 1; module <= 2; module++) {
        for (uint8_t pin = 0; pin < 8; pin++) {
          setPCF8574Pin(module, pin, LOW);
        }
      }
      Serial.println("All GPIO pins turned ON");
      break;

    case 'Z': // Turn ALL GPIO pins OFF (HIGH)
      for (uint8_t module = 1; module <= 2; module++) {
        for (uint8_t pin = 0; pin < 8; pin++) {
          setPCF8574Pin(module, pin, HIGH);
        }
      }
      Serial.println("All GPIO pins turned OFF");
      break;

    case 'X': // Turn laser ON
      pointerOn();
      Serial.println("Laser turned ON");
      break;

    case 'Y': // Turn laser OFF
      pointerOff();
      Serial.println("Laser turned OFF");
      break;

    default:
      Serial.println("Unknown command");
      break;
  }
}

void setup() {
  Serial.begin(9600);

  Wire.begin(); // Initialize I2C communication

  stepper.reverse(true); // First go backward for finding starting point
  stepper.setMaxSpeed(500); // Speed for setTargetDeg()

  servo.attach(11); // Attach servo to pin 11
  servo.write(0); // Set servo to 0 degrees

  pinMode(START_POINT_SENSOR_PIN, INPUT);
}

void loop() {
  stepper.tick();

  if (look_for_start_point) {
    if (digitalRead(START_POINT_SENSOR_PIN) == LOW) {
      stepper.stop();
      stepper.reset();
      stepper.reverse(false);
      look_for_start_point = false;
      Serial.println("Stepper motor starting point found");
    } else {
      stepper.setSpeed(500); // Rotate at 500 steps per second
    }
  }

  // Handle serial input
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove any extra whitespace
    handleSerialCommand(command);
  }
}