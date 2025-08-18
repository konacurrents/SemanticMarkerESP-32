//
//  ULN2003_StepperClass.cpp
//  ESP_IOT
//
//  Created by Scott Moody on 8/13/25.
//

#include "ULN2003_StepperClass.h"


// from Dispense it calls getFeederType_mainModule()
//#include "StepperModule.h"
//! 8.14.25 grabbed our own copy..
#include "../AtomStepMotor/Stepper.h"
///#include <Stepper.h>

// Steps per revolution for 28BYJ-48 in half-step mode
//int _STEPS_PER_REV = 2048;

// M5Atom GPIO pins connected to ULN2003 IN1–IN4
#define IN1 22
#define IN2 19
#define IN3 23
#define IN4 33

// Create Stepper object
Stepper *_stepperMotor;//(STEPS_PER_REV, IN1, IN3, IN2, IN4);


#ifdef NOT_USED
//#include "TimerDelayClass.h"
//! create instance of the timer class
TimerDelayClass *_timerDelayClass_ULN2003_StepperClass;
#endif

#ifdef NOT_USED
//! default .. this could be set via Preferences (TODO)
float _motorSpeedRPM_ULN2003_Stepper_setting = 0.5;
#endif

/******************stepper declarations******************************/

ULN2003_StepperClass::ULN2003_StepperClass(char *config) : MotorStepperClassType(config)
{
    SerialDebug.printf("ULN2003_StepperClass init %s\n", config);
#ifdef NOT_USED
    _timerDelayClass_ULN2003_StepperClass = new TimerDelayClass(_motorSpeedRPM_ULN2003_Stepper_setting);
#endif
}


/************* Set all motor pins off which turns off the motor ************************************************/

//! stop the motor
void ULN2003_StepperClass::stop_MotorStepper()
{
    SerialDebug.println("ULN2003_StepperClass::stop_MotorStepper");
    //! 7.9.25 if the pin was set use it otherwise use the hard coded values
    //! LOW is off
//    digitalWrite(_pin1, LOW);
//    digitalWrite(_pin2, LOW);
    
#ifdef NOT_USED
    //!user timer class instance
    _timerDelayClass_ULN2003_StepperClass->stopDelay();
#endif
}

//! NOTE: this is not right as only 1 instance is around .. so this class cannot be instantiated more than 1 time..
boolean _isSetup_ULN2003_Stepper = false;

//! These are called from StepperModule
//!Prepare motor controller
void ULN2003_StepperClass::setup_MotorStepper()
{
    if (_isSetup_ULN2003_Stepper)
        return;
    
    SerialDebug.println("ULN2003_StepperClass::setup_MotorStepper");
    SerialDebug.printf("PINS = %d, %d, %d, %d\n",  IN1, IN3, IN2, IN4);
    
    //! FIRST: normalize in case use goofed up, especially 0
    //! grab this value.
    int motorSpeedRPM = (int) getPreferenceFloat_mainModule(PREFERENCE_STEPPER_RPM_SETTING);
    if (motorSpeedRPM == 0 || motorSpeedRPM > 30)
        motorSpeedRPM = 15;
    
    //! grab this value.
    int stepsPerRevolution = (int) getPreferenceFloat_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING);
    if (stepsPerRevolution == 0 || stepsPerRevolution > 4000)
        stepsPerRevolution = 2048;
    
    SerialDebug.printf("Motor RPM = %d, stepsPerRev = %d\n", motorSpeedRPM, stepsPerRevolution);

    //! create stepper motor
    _stepperMotor = new Stepper(stepsPerRevolution, IN1, IN3, IN2, IN4);
    
    //! Set motor speed (RPM)
    //! 15 is default...
    _stepperMotor->setSpeed(motorSpeedRPM);
    
    //! 5.3.25 trying to figure out the PIN use
    registerPinUse_mainModule(IN1, "IN1", "ULN2003_StepperClass", false);
    registerPinUse_mainModule(IN2, "IN2", "ULN2003_StepperClass", false);
    registerPinUse_mainModule(IN3, "IN3", "ULN2003_StepperClass", false);
    registerPinUse_mainModule(IN4, "IN4", "ULN2003_StepperClass", false);
    
    
    _isSetup_ULN2003_Stepper = true;
}

//!This will advance the stepper clockwise once by the angle specified in SetupStepper. Example 16 pockets in UNO is 22.5 degrees
//!This is the FEED message .. the comments mention the Stepper Motor .. which this isn't
void ULN2003_StepperClass::start_MotorStepper()
{
    SerialDebug.println("ULN2003_StepperClass::start_MotorStepper");
    ULN2003_StepperClass::setup_MotorStepper();
    
    SerialDebug.println("***** ULN2003_StepperClass::Starting ULN2003_Stepper *********");
    
    //! 5.15.25 try the async CLICK
    //! click call  5.26.25 SYNC version
    main_dispatchSyncCommand(SYNC_CLICK_SOUND);
    
#pragma mark NEW_CODE_HERE
    //Set the four pins to their proper state for the current step in the sequence,
    //and for the current direction
    
    int startTime = getTimeStamp_mainModule();
    
    //! FIRST: normalize in case use goofed up, especially 0
    //! grab this value.
    int motorSpeedRPM = (int) getPreferenceFloat_mainModule(PREFERENCE_STEPPER_RPM_SETTING);
    if (motorSpeedRPM == 0 || motorSpeedRPM > 30)
        motorSpeedRPM = 15;
    
    //! grab this value.
    int stepsPerRevolution = (int) getPreferenceFloat_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING);
    if (stepsPerRevolution == 0 || stepsPerRevolution > 4000)
        stepsPerRevolution = 2048;
    SerialDebug.printf("Motor RPM = %d, stepsPerRev = %d\n", motorSpeedRPM, stepsPerRevolution);

    //! create stepper motor
    //! 8.14.25 Setting the Speed and Steps each time..
    //! Set motor speed (RPM)
    //! 15 is default...
    _stepperMotor->setSpeedSteps(motorSpeedRPM, stepsPerRevolution);
    
    if (this->isClockwiseDirection())
    {
        SerialDebug.println("Rotating one revolution clockwise...");
        _stepperMotor->step(stepsPerRevolution);
    }
    else
    {
        SerialDebug.println("Rotating one revolution counterclockwise...");
        _stepperMotor->step(-stepsPerRevolution);
    }
       
    SerialDebug.println("**************** ULN2003_StepperClass::Ending ULN2003_Stepper *************");
    int endTime = getTimeStamp_mainModule();

    SerialMin.printf("Turn Length: %d seconds \n",endTime - startTime);

}


//!loop the PTStepper (so timer can run)
void ULN2003_StepperClass::loop_MotorStepper()
{
#ifdef NOT_USED

    //!user timer class instance
    if (_timerDelayClass_ULN2003_StepperClass->delayFinished())
    {
        SerialDebug.println("ULN2003_StepperClass::delayFinished");
        
        this->stop_MotorStepper();
    }
#endif
}

