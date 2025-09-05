/// \link L9110S_DCStepperStepperClass
//  L9110S_DCStepperClass.cpp
//
//
//  Created by Scott Moody on 5/19/25.
//

#include "L9110S_DCStepperClass.h"

// from Dispense it calls getFeederType_mainModule()
#include "StepperModule.h"

//#include "TimerDelayClass.h"
//! create instance of the timer class
TimerDelayClass *_timerDelayClass_L9110S_DCStepperClass;

//! default .. this could be set via Preferences (TODO)
float _delaySeconds_L9110S_DCStepper_setting = 0.5;

/******************stepper declarations******************************/

L9110S_DCStepperClass::L9110S_DCStepperClass(char *config) : MotorStepperClassType(config)
{
    SerialDebug.printf("L9110S_DCStepperClass init %s\n", config);
    _timerDelayClass_L9110S_DCStepperClass = new TimerDelayClass(_delaySeconds_L9110S_DCStepper_setting);
}

//! This works with the ESP_M5
//! 7.9.25 after F1 movie (make Brad Pitt Great Again)
//! hard coded .. unless changed via Sensors message
//! overridded by _pin1 _pin2
const int IN1_PIN = 21;  //IN1 on the ULN2003 Board, BLUE end of the Blue/Yellow motor coil
const int IN2_PIN = 25;  //IN2 on the ULN2003 Board, PINK end of the Pink/Orange motor coil

#ifdef NOT_WORKING
//! 9.4.25
//! delay amount after a start_MotorStepper
//! defaults = 0
int L9110S_DCStepperClass::delayAmountBetweenMotor()
{
    SerialDebug.println("L9110S_DCStepperClass::delayAmountBetweenMotor = 600");
    return 600;
}
#endif
//https://randomnerdtutorials.com/esp32-pwm-arduino-ide/

/************* Set all motor pins off which turns off the motor ************************************************/

//! stop the motor
void L9110S_DCStepperClass::stop_MotorStepper()
{
    SerialDebug.println("L9110S_DCStepperClass::stop_MotorStepper");
    //! 7.9.25 if the pin was set use it otherwise use the hard coded values
    //! LOW is off
    digitalWrite(_pin1, LOW);
    digitalWrite(_pin2, LOW);
    
    //!user timer class instance
    _timerDelayClass_L9110S_DCStepperClass->stopDelay();
}

boolean _isSetup_L9110S_DCStepper = false;

//! These are called from StepperModule
//!Prepare motor controller
void L9110S_DCStepperClass::setup_MotorStepper()
{
    if (_isSetup_L9110S_DCStepper)
        return;
    
    SerialDebug.println("L9110S_DCStepperClass::setup_MotorStepper");
    SerialDebug.printf("PINS = %d, %d\n", _pin1, _pin2);
    //! 7.9.25 if the pin was set use it otherwise use the hard coded values
    if (_pin1 == 0)
    {
        _pin1 = IN1_PIN;
        _pin2 = IN2_PIN;
        SerialDebug.printf("NULL PINS setting to = %d, %d\n", _pin1, _pin2);
    }
    
    //! set output pins
    pinMode(_pin1, OUTPUT);
    pinMode(_pin2, OUTPUT);
 
    //! stop the motor (or something) THis seems to be working.. the LOW worked sometimes..
    for (int i=0; i< 3; i++)
    {
        digitalWrite(_pin1, LOW);
        digitalWrite(_pin2, LOW);
    }
    
    //! 5.3.25 trying to figure out the PIN use
    registerPinUse_mainModule(_pin1, "IN1_PIN", "L9110S_DCStepperClass", false);
    registerPinUse_mainModule(_pin2, "IN2_PIN", "L9110S_DCStepperClass", false);
    
    _isSetup_L9110S_DCStepper = true;
}

//!This will advance the stepper clockwise once by the angle specified in SetupStepper. Example 16 pockets in UNO is 22.5 degrees
//!This is the FEED message .. the comments mention the Stepper Motor .. which this isn't
void L9110S_DCStepperClass::start_MotorStepper()
{
    SerialDebug.println("L9110S_DCStepperClass::start_MotorStepper");
    L9110S_DCStepperClass::setup_MotorStepper();

    SerialDebug.println("***** L9110S_DCStepperStepper::Starting L9110S_DCStepper *********");
    
    //! 5.15.25 try the async CLICK
    //! click call  5.26.25 SYNC version
    main_dispatchSyncCommand(SYNC_CLICK_SOUND);
    
#define NEW_CODE_HERE
    //Set the four pins to their proper state for the current step in the sequence,
    //and for the current direction
    
    if (this->isClockwiseDirection())
    {
        digitalWrite(_pin1, LOW);
        digitalWrite(_pin2, HIGH);
    }
    else
    {
        digitalWrite(_pin1, HIGH);
        digitalWrite(_pin2, LOW);
    }
    
    //! grab this value.
    _delaySeconds_L9110S_DCStepper_setting = getPreferenceFloat_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING);
    
    //! start the delay..  (which at .5 might not be needed...)
    _timerDelayClass_L9110S_DCStepperClass->startDelay(_delaySeconds_L9110S_DCStepper_setting);
    
    SerialDebug.println("**************** L9110S_DCStepperStepper::Ending L9110S_DCStepper *************");
}


//!loop the PTStepper (so timer can run)
void L9110S_DCStepperClass::loop_MotorStepper()
{
    //!user timer class instance
    if (_timerDelayClass_L9110S_DCStepperClass->delayFinished())
    {
        SerialDebug.println("L9110S_DCStepperClass::delayFinished");
        
        this->stop_MotorStepper();
    }
    
}
