#ifndef DISP_CONTROL_IR
#define DISP_CONTROL_IR

#include <Arduino.h>
#include <IRremote.h>
#include "GyverTM1637.h"

#define BLINK_DELAY 500

#define BUTTON_1 0xFFA25D
#define BUTTON_2 0xFF629D
#define BUTTON_3 0xFFE21D
#define BUTTON_4 0xFF22DD
#define BUTTON_5 0xFF02FD
#define BUTTON_6 0xFFC23D
#define BUTTON_7 0xFFE01F
#define BUTTON_8 0xFFA857
#define BUTTON_9 0xFF906F
#define BUTTON_0 0xFF9857
#define BUTTON_UP 0xFF18E7
#define BUTTON_DOWN 0xFF4AB5
#define BUTTON_LEFT 0xFF10EF
#define BUTTON_RIGHT 0xFF5AA5
#define BUTTON_OK 0xFF38C7
#define BUTTON_HASHTAG 0xFFB04F
#define BUTTON_ASTERISK 0xFF6897

class Disp_Control_IR {
  public:
    Disp_Control_IR(GyverTM1637& disp) : disp(disp) {}

    bool handleClick(unsigned long btnCode); // returns true when number set (pressed ok)
    void updateDisp();
    bool isSetting();
    int getCurrentNumber();

  private:
    GyverTM1637& disp;

    int currentNumber = 1;
    bool settingMode = true; // when setting, number blinks
    unsigned long blinkTimer = 0;
    bool blinkState = true; // on when true; off when false

    void appendDigit(int digit);
    void incrementDigit();
    void decrementDigit();
};

#endif
