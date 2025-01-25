#include "GyverTM1637.h"
#include <IRremote.h>
#include "disp-control-ir.h"

#define IR_RECEIVE_PIN 12
IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;

#define CLK 2
#define DIO 3
GyverTM1637 disp(CLK, DIO);

Disp_Control_IR dispController(disp); 

void setup(){
  Serial.begin(9600);

  disp.clear();         // Clear the display
  disp.brightness(7);   // Set maximum brightness
  disp.displayInt(dispController.getCurrentNumber());

  // Initialize the IR receiver
  irrecv.enableIRIn();

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  dispController.updateDisp();
  
  if (irrecv.decode(&results)){
    if(dispController.handleClick(results.value)) {
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }

    Serial.println(results.value, HEX);
    irrecv.resume();
  }
}
