/**
*  \link ButtonModule
*/
/*
 *******************************************************************************
  // https://randomnerdtutorials.com/esp32-pinout-reference-gpios/ 

  Enable (EN)
Enable (EN) is the 3.3V regulatorâs enable pin. Itâs pulled up, so connect to ground to disable the 3.3V regulator. This means that you can use this pin connected to a pushbutton to restart your ESP32, for example.

 Scott Version: 1.1.2022
 *******************************************************************************
 */
#include "ButtonModule.h"

#ifdef USE_BUTTON_MODULE

//extension of ButtonModule - includes Sensors
#include "ButtonProcessing.h"


//#define ESP_M5
#ifdef ESP_M5
#include <M5StickCPlus.h>
#else
#include <Arduino.h>
#endif

//! called by the feed operation to say the device is still running.. and count it as a button click.
void refreshDelayButtonTouched_ButtonModule()
{
    refreshDelayButtonTouched_ButtonProcessing();
}

// See https://forum.arduino.cc/t/arduino-push-button-double-click-function/409353

// set PushButton pin number
const int _buttonPin = 0;

// set LED pin numbers
const int GPIO0 = _buttonPin;

const int LED1 = 8;
const int LED2 = 9;

// set LED pin 13
const int LEDP13 = 13;

//initialising led
int LED1Status = LOW;
int LED2Status = LOW;

int _buttonState;

//state of the button..
boolean _buttonPressed;

void setup_ButtonModule()
{

    // put your setup code here, to run once:
    _buttonPressed = false;
    
#ifdef ESP_32
    // initialize the LED pin as an output:
    pinMode(_buttonPin ,INPUT);
#endif
    //!calls the extension of ButtonModule
    setup_ButtonProcessing();
    
}
//if _buttonPressed (the LOW state == 0)
// then wait for the HIGH state == 1
// and that is a button click..

void loop_ButtonModule()
{
    
#ifdef ESP_32
    // put your main code here, to run repeatedly:
    
    _buttonState = digitalRead(_buttonPin);
    switch (_buttonState)
    {
        case LOW: //0
            _buttonPressed = true;
            break;
        default:
            
        case HIGH: // 1  (nothing touched)
            if (_buttonPressed)
            {
                // then the button was unpressed..
                SerialLots.println("BUTTON PRESSED and RELEASED");
                _buttonPressed = false;
                
                callCallbackMain(CALLBACKS_BUTTON_MODULE, SINGLE_CLICK_BM, (char*)"B1");

            }
            break;
    }
    //SerialLots.printf("_buttonState = %d\n", _buttonState);
#endif
    //!calls the extension of ButtonModule
    loop_ButtonProcessing();
}

//!This is only thing exposed to others.. (Kinda which only 1 button module)
//!short press on buttonA (top button)
void buttonA_ShortPress_ButtonModule()
{
    buttonA_ShortPress();
}
//!long press on buttonA (top button)
void buttonA_LongPress_ButtonModule()
{
    buttonA_LongPress();
}
//!the long press of the side button
void buttonB_LongPress_ButtonModule()
{
    buttonB_LongPress();
}
//!the short press of the side button
void buttonB_ShortPress_ButtonModule()
{
    buttonB_ShortPress();
}

#endif //USE_BUTTON_MODULE
