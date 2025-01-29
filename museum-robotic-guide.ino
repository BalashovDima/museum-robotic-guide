#include "GyverTM1637.h"
#include <IRremote.h>
#include <DFMiniMp3.h>
#include <SoftwareSerial.h>
#include "disp-control-ir.h"

#define IR_RECEIVE_PIN 12
IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;

#define CLK 2
#define DIO 3
GyverTM1637 disp(CLK, DIO);

Disp_Control_IR dispController(disp); 

#define STARTING_VOLUME 20
class Mp3Notify; // forward declare the notify class, just the name

// typedef DFMiniMp3<HardwareSerial, Mp3Notify> DfMp3; // define a handy type using serial and our notify class
// DfMp3 dfmp3(Serial3); // instance a DfMp3 object

// Some arduino boards only have one hardware serial port, so a software serial port is needed instead.
// comment out the above definitions and use these
SoftwareSerial secondarySerial(9, 8); // RX (DF player's TX), TX (DF player's RX)
typedef DFMiniMp3<SoftwareSerial, Mp3Notify> DfMp3;
DfMp3 dfmp3(secondarySerial);

bool secondFinishCall = false;
uint16_t trackCountInFolder1;
uint16_t trackCountInFolder2;
uint8_t action = 0; // 0 - unknown, 1 - play track
uint8_t tour = dispController.getCurrentNumber();
uint8_t tourActionIndex = 0;
uint8_t tourAudioIndex = 0;
uint8_t tourPointerIndex = 0;
char tours[2][100] = {
  {
    "t",
  },
  {
    "t",
  }
};


// implement a notification class,
// its member methods will get called
class Mp3Notify {
    public:
        static void PrintlnSourceAction(DfMp3_PlaySources source, const char *action) {
            if (source & DfMp3_PlaySources_Sd) {
                Serial.print("SD Card, ");
            }

            if (source & DfMp3_PlaySources_Usb) {
                Serial.print("USB Disk, ");
            }

            if (source & DfMp3_PlaySources_Flash) {
                Serial.print("Flash, ");
            }

            Serial.println(action);
        }

        static void OnError([[maybe_unused]] DfMp3 &mp3, uint16_t errorCode) {
            // see DfMp3_Error for code meaning
            Serial.println();
            Serial.print("Com Error ");
            Serial.println(errorCode);
        }

        // from wiki on library's github (important that it gets called twice)
        // A track has finished playing. There are cases where this will get called more than once for the same track. If you are using a repeating mode or random play, you will receive only one between the tracks and a double when it stops. If you play a single track, you should get called twice, one for the finish of the track, and another for the finish of the command. This is a nuance of the chip.
        static void OnPlayFinished([[maybe_unused]] DfMp3 &mp3, [[maybe_unused]] DfMp3_PlaySources source, uint16_t track) {
            if(secondFinishCall) {
                switch(action) {
                    default:
                        break;
                }

                secondFinishCall = false;
            } else {
                secondFinishCall = true;
            }
        }

        static void OnPlaySourceOnline([[maybe_unused]] DfMp3 &mp3, DfMp3_PlaySources source) {
            PrintlnSourceAction(source, "online");
        }

        static void OnPlaySourceInserted([[maybe_unused]] DfMp3 &mp3, DfMp3_PlaySources source) {
            PrintlnSourceAction(source, "inserted");
        }

        static void OnPlaySourceRemoved([[maybe_unused]] DfMp3 &mp3, DfMp3_PlaySources source) {
            PrintlnSourceAction(source, "removed");
        }
};

void setup(){
  Serial.begin(9600);

  disp.clear();         // Clear the display
  disp.brightness(7);   // Set maximum brightness
  disp.displayInt(dispController.getCurrentNumber());

  // Initialize the IR receiver
  irrecv.enableIRIn();

  dfmp3.begin();
  // for boards that support hardware arbitrary pins
  // dfmp3.begin(6, 5); // RX, TX

  // during development, it's a good practice to put the module
  // into a known state by calling reset().
  // You may hear popping when starting and you can remove this
  // call to reset() once your project is finalized
  dfmp3.reset();

  uint16_t version = dfmp3.getSoftwareVersion();
  uint16_t total_track_count = dfmp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
  trackCountInFolder1 = dfmp3.getFolderTrackCount(1);  
  trackCountInFolder2 = dfmp3.getFolderTrackCount(2);
  dfmp3.setVolume(STARTING_VOLUME);

  Serial.println("done initializing DFPlayer...");
  Serial.print("version ");
  Serial.println(version); 
  Serial.print("files total ");
  Serial.println(total_track_count);
  Serial.print("volume ");
  Serial.println(STARTING_VOLUME);


  pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  dispController.updateDisp();
  
  if (irrecv.decode(&results)){
    uint8_t action = dispController.handleClick(results.value);

    switch(action) {
      case 0:
        break;
      case 1: // 'ok' pressed for confirming number
        tour = dispController.getCurrentNumber();
        dfmp3.playFolderTrack16(1, 1); // just to check
        break;
      case 2: // 'ok' pressed for pausing/resuming
        DfMp3_Status status = dfmp3.getStatus();
        // Check the current state of the player and toggle play/pause
        if (status.state == DfMp3_StatusState_Playing) {
          dfmp3.pause();  // Pause playback
          Serial.println("Paused");
        } 
        else if (status.state == DfMp3_StatusState_Paused || status.state == DfMp3_StatusState_Idle) {
          dfmp3.start();  // Resume or start playback
          Serial.println("Resumed/Started");
        }
        break;
    }
    

    Serial.println(results.value, HEX);
    irrecv.resume();
  }
}
