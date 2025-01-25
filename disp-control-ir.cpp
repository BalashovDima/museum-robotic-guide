#include "Arduino.h"
#include "disp-control-ir.h"

bool Disp_Control_IR::handleClick(unsigned long btnCode) {
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
			settingMode = true;
			currentNumber = 1; 
			break;
		case BUTTON_OK:
      disp.displayInt(currentNumber);
			settingMode = false; 
			return true;
			break;
		default: // in case unknow code received, don't change anything
			if(settingMode) return false;
			else return true;
	}

	disp.displayInt(currentNumber);
	blinkTimer = millis();
	blinkState = true;
	return false;
}

void Disp_Control_IR::updateDisp() {
	if(settingMode) {
		if(millis() - blinkTimer >= BLINK_DELAY) {
			if(blinkState) {
        disp.clear();
        blinkTimer = millis();
        blinkState = false;
      } else {
        disp.displayInt(currentNumber);
        blinkTimer = millis();
        blinkState = true;
      }
		}
	}
}

bool Disp_Control_IR::isSetting() {
	return settingMode;
}

int Disp_Control_IR::getCurrentNumber() {
	return currentNumber;
}

void Disp_Control_IR::appendDigit(int digit) {  
	settingMode = true;
	currentNumber = currentNumber*10 + digit;
  currentNumber = currentNumber % 100; // Ensure the number stays within the 4-digit range
}

void Disp_Control_IR::incrementDigit() {
	settingMode = true;
	if(currentNumber == 99) currentNumber = 1;
	else currentNumber++;
}

void Disp_Control_IR::decrementDigit() {
	settingMode = true;
	if(currentNumber == 1) currentNumber = 9;
	else currentNumber--;
}

