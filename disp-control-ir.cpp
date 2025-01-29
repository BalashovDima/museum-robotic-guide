#include "Arduino.h"
#include "GyverTM1637.h"
#include "disp-control-ir.h"

uint8_t Disp_Control_IR::handleClick(unsigned long btnCode) {
	switch(btnCode) {
		case BUTTON_1: appendDigit(1); break;
		case BUTTON_2: appendDigit(2); break;
		case BUTTON_3: appendDigit(3); break;
		case BUTTON_4: appendDigit(4); break;
		case BUTTON_5: appendDigit(5); break;
		case BUTTON_6: appendDigit(6); break;
		case BUTTON_7: appendDigit(7); break;
		case BUTTON_8: appendDigit(8); break;
		case BUTTON_9: appendDigit(9); break;
		case BUTTON_0: appendDigit(0); break;
		case BUTTON_UP: incrementDigit(); break;
		case BUTTON_DOWN: decrementDigit(); break;
		case BUTTON_RIGHT: incrementDigit(); break;
		case BUTTON_LEFT: decrementDigit(); break;
		case BUTTON_HASHTAG: 
			mode = 1;
			currentNumber = 1; 
			break;
		case BUTTON_OK:
			if(mode == 1) {
				disp.displayInt(currentNumber);
				mode = 0; 
				return 1;
			} else if(mode == 0) {
				mode = 2;
				return 2;
				break;
			} else if(mode == 2) {
				disp.displayInt(currentNumber);
				mode = 0;
				return 2;
				break;
			}
		default: // in case unknow code received, don't change anything
			return 0;
	}

	disp.displayInt(currentNumber);
	blinkTimer = millis();
	blinkState = true;
	return 0;
}

void Disp_Control_IR::updateDisp() {
	if(mode) {
		if(millis() - blinkTimer >= BLINK_DELAY) {
			if(blinkState) {
				if(mode == 1) {
	        disp.clear();
				} else if(mode == 2){
					disp.displayByte(0, 0x00); // clear the most left digit place
				}

        blinkTimer = millis();
        blinkState = false;
      } else {
      	if(mode == 1) {
	        disp.displayInt(currentNumber);
				} else if(mode == 2){
					disp.displayByte(0, 0x73); // show 'P' in the most left digit place
				}
		
        blinkTimer = millis();
        blinkState = true;
      }
		}
	}
}

bool Disp_Control_IR::isSetting() {
	if(mode == 1) return true;
}

int Disp_Control_IR::getCurrentNumber() {
	return currentNumber;
}

void Disp_Control_IR::appendDigit(int digit) {  
	currentNumber = currentNumber*10 + digit;
  currentNumber = currentNumber % 100; // Ensure the number stays within the 4-digit range
}

void Disp_Control_IR::incrementDigit() {
	if(currentNumber == 99) currentNumber = 1;
	else currentNumber++;
}

void Disp_Control_IR::decrementDigit() {
	if(currentNumber == 1) currentNumber = 9;
	else currentNumber--;
}

