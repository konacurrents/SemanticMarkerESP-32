//! \link SG90ServoStepperStepperClass
//
//  SG90ServoStepperClass.cpp
//
//
//  Created by Scott Moody on 3/31/25.

#include "SG90ServoStepperClass.h"

// from Dispense it calls getFeederType_mainModule()
#include "StepperModule.h"

//! see https://github.com/pablomarquez76/AnalogWrite_ESP32
//! library:  <analogWrite.h>
//!
//TODO ... #include <analogWrite.h>

//#include "TimerDelayClass.h"
//! create instance of the timer class
TimerDelayClass *_timerDelayClass_SG90ServoStepperClass;

//! default .. this could be set via Preferences (TODO)
float _delaySeconds_SG90ServoStepper_setting = 0.5;

/******************stepper declarations******************************/

SG90ServoStepperClass::SG90ServoStepperClass(char *config) : MotorStepperClassType(config)
{
    SerialDebug.printf("SG90ServoStepperClass init %s\n", config);
    _timerDelayClass_SG90ServoStepperClass = new TimerDelayClass(_delaySeconds_SG90ServoStepper_setting);
    
}

//! This works with the ESP_M5

const int IN1_PIN = 19;


/************* Set all motor pins off which turns off the motor ************************************************/
void clearPins_SG90ServoStepper()
{

}


//! stop the motor
void SG90ServoStepperClass::stop_MotorStepper()
{
    SerialDebug.println("SG90ServoStepperClass::stop_MotorStepper");
    
    
    //!user timer class instance
    _timerDelayClass_SG90ServoStepperClass->stopDelay();
}

boolean _isSetup_SG90ServoStepper = false;
//! These are called from StepperModule
//!Prepare motor controller
void SG90ServoStepperClass::setup_MotorStepper()
{
    SerialDebug.println("SG90ServoStepperClass::setup_MotorStepper");

}


//!This will advance the stepper clockwise once by the angle specified in SetupStepper. Example 16 pockets in UNO is 22.5 degrees
//!This is the FEED message .. the comments mention the Stepper Motor .. which this isn't
void SG90ServoStepperClass::start_MotorStepper()
{
    SerialDebug.println("SG90ServoStepperClass::start_MotorStepper");
    
    //! ask the class wide method for the clockwise direction
    // this->isClockwiseDirection();
    
    SerialDebug.println("**************** SG90ServoStepperStepper::Starting SG90ServoStepper *******************");
    
#define NEW_CODE__not_working
#ifdef NEW_CODE
    //!see https://github.com/pablomarquez76/AnalogWrite_ESP32
    analogServo(IN1_PIN, 90); // Sends 90 degrees signal to pin 2 (0 degrees = 600ms 180 degrees = 2400ms at 50Hz)

#endif
    
    //! grab this value.
    _delaySeconds_SG90ServoStepper_setting = getPreferenceFloat_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING);
    
    //! start the delay..  (which at .5 might not be needed...)
    _timerDelayClass_SG90ServoStepperClass->startDelay(_delaySeconds_SG90ServoStepper_setting);
    
    SerialDebug.println("**************** SG90ServoStepperStepper::Ending SG90ServoStepper *************");
    
}


//!loop the PTStepper (so timer can run)
void SG90ServoStepperClass::loop_MotorStepper()
{
    //!user timer class instance
    if (_timerDelayClass_SG90ServoStepperClass->delayFinished())
    {
        SerialDebug.println("SG90ServoStepperClass::delayFinished");
        
        this->stop_MotorStepper();
    }
    
}
