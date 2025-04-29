//! \link HDriverStepperStepperClass
//  HDriverStepperStepperClass.cpp
//
//
//  Created by Scott Moody on 3/24/25.
//

#include "HDriverStepperClass.h"

// from Dispense it calls getFeederType_mainModule()
#include "StepperModule.h"

//#include "TimerDelayClass.h"
//! create instance of the timer class
TimerDelayClass *_timerDelayClass_HDriverStepperClass;

//! default .. this could be set via Preferences (TODO)
float _delaySeconds_HDriverStepper_setting = 0.5;

/******************stepper declarations******************************/

HDriverStepperClass::HDriverStepperClass(char *config) : MotorStepperClassType(config)
{
    SerialDebug.printf("HDriverStepperClass init %s\n", config);
    _timerDelayClass_HDriverStepperClass = new TimerDelayClass(_delaySeconds_HDriverStepper_setting);

}

//! This works with the ESP_M5

const int IN1_PIN = 19;
const int IN2_PIN = 23;
int _freq_HDriverStepper          = 10000;
int _ledChannel1_HDriverStepper   = 0;
int _ledChannel2_HDriverStepper   = 1;
int _resolution_HDriverStepper    = 10;
int VIN_PIN       = 33;
int FAULT_PIN     = 22;

//https://randomnerdtutorials.com/esp32-pwm-arduino-ide/

#define NUM_PINS 2
////Put all the pins in an array to make them easy to work with
int _pins_HDriverStepperClass[NUM_PINS]{
    IN1_PIN,  //IN1 on the ULN2003 Board, BLUE end of the Blue/Yellow motor coil
    IN2_PIN  //IN2 on the ULN2003 Board, PINK end of the Pink/Orange motor coil
};


//Keeps track of the current direction
//Relative to the face of the motor.
//Clockwise (true) or Counterclockwise(false)
//We'll default to clockwise
bool _clockwise_HDriverStepperClass = true;


/************* Set all motor pins off which turns off the motor ************************************************/
void clearPins_HDriverStepper()
{
#ifdef NOT_USED_FOR_H_DRIVER
    for (int pin = 0; pin < NUM_PINS; pin++)
    {
        pinMode(_pins_HDriverStepperClass[pin], OUTPUT);
        digitalWrite(_pins_HDriverStepperClass[pin], LOW);
    }
#endif
}


//! stop the motor
void HDriverStepperClass::stop_MotorStepper()
{
    SerialDebug.println("HDriverStepperClass::stop_MotorStepper");

    //M5.dis.drawpix(0, 0x000000);
    ledcWrite(_ledChannel1_HDriverStepper, 0);
    ledcWrite(_ledChannel2_HDriverStepper, 0);
    
    //!user timer class instance
    _timerDelayClass_HDriverStepperClass->stopDelay();
}

boolean _isSetup_HDriverStepper = false;
//! These are called from StepperModule
//!Prepare motor controller
void HDriverStepperClass::setup_MotorStepper()
{
    SerialDebug.println("HDriverStepperClass::setup_MotorStepper");
#define NOT_TRY_ON_FIRST_FEED
#ifdef  NOT_TRY_ON_FIRST_FEED
    ledcSetup(_ledChannel1_HDriverStepper, _freq_HDriverStepper, _resolution_HDriverStepper);
    ledcSetup(_ledChannel2_HDriverStepper, _freq_HDriverStepper, _resolution_HDriverStepper);
    ledcAttachPin(IN1_PIN, _ledChannel1_HDriverStepper);
    ledcAttachPin(IN2_PIN, _ledChannel2_HDriverStepper);
    pinMode(VIN_PIN, INPUT);
    pinMode(FAULT_PIN, INPUT);
    _isSetup_HDriverStepper = true;
#endif
    //clearPins_HDriverStepper();                     // Go turn all motor pins off
}

#ifdef NOT_TRY_ON_FIRST_FEED
#else
//! 3.26.25 try to only setup on the first feed
void setup_again()
{
    if (!_isSetup_HDriverStepper)
    {
        _isSetup_HDriverStepper = true;
        ledcSetup(_ledChannel1_HDriverStepper, _freq_HDriverStepper, _resolution_HDriverStepper);
        ledcSetup(_ledChannel2_HDriverStepper, _freq_HDriverStepper, _resolution_HDriverStepper);
        ledcAttachPin(IN1_PIN, _ledChannel1_HDriverStepper);
        ledcAttachPin(IN2_PIN, _ledChannel2_HDriverStepper);
        pinMode(VIN_PIN, INPUT);
        pinMode(FAULT_PIN, INPUT);
    }
}
#endif
//!This will advance the stepper clockwise once by the angle specified in SetupStepper. Example 16 pockets in UNO is 22.5 degrees
//!This is the FEED message .. the comments mention the Stepper Motor .. which this isn't
void HDriverStepperClass::start_MotorStepper()
{
    SerialDebug.println("HDriverStepperClass::start_MotorStepper");

#ifdef  NOT_TRY_ON_FIRST_FEED
#else
    //! 3.26.25 try to only setup on the first feed
    setup_again();
#endif
    //! ask the class wide method for the clockwise direction
    _clockwise_HDriverStepperClass = this->isClockwiseDirection();
    
    SerialDebug.println("**************** HDriverStepperStepper::Starting HDriverStepper *******************");
    
#define NEW_CODE_HERE
    //Set the four pins to their proper state for the current step in the sequence,
    //and for the current direction
    
    if (_clockwise_HDriverStepperClass) {
        //M5.dis.drawpix(0, 0x0000f0);
        ledcWrite(_ledChannel1_HDriverStepper, 1000);
        ledcWrite(_ledChannel2_HDriverStepper, 0);
    } else {
        //M5.dis.drawpix(0, 0xff00f0);
        ledcWrite(_ledChannel1_HDriverStepper, 0);
        ledcWrite(_ledChannel2_HDriverStepper, 1000);
    }
    
    //! grab this value.
    _delaySeconds_HDriverStepper_setting = getPreferenceFloat_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING);
    
    //! start the delay..  (which at .5 might not be needed...)
    _timerDelayClass_HDriverStepperClass->startDelay(_delaySeconds_HDriverStepper_setting);

    SerialDebug.println("**************** HDriverStepperStepper::Ending HDriverStepper *************");
    
}


//!loop the PTStepper (so timer can run)
void HDriverStepperClass::loop_MotorStepper()
{
    //!user timer class instance
    if (_timerDelayClass_HDriverStepperClass->delayFinished())
    {
        SerialDebug.println("HDriverStepperClass::delayFinished");

        this->stop_MotorStepper();
    }
  
}
