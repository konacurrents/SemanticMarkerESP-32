//! \link DCMotorStepper
//  DCMotorStepper.cpp
//  
//
//  Created by Scott Moody on 3/16/25.
//

#include "DCMotorStepperClass.h"

// from Dispense it calls getFeederType_mainModule()
#include "StepperModule.h"

//#include "TimerDelayClass.h"
//! create instance of the timer class
TimerDelayClass *_timerDelayClass_DCMotorStepperClass;

//! default .. this could be set via Preferences (TODO)
float _delaySeconds_DCMotorStepper_setting = 0.5;


/******************stepper declarations******************************/

DCMotorStepperClass::DCMotorStepperClass(char *config) : MotorStepperClassType(config)
{
    SerialDebug.printf("DCMotorStepperClass init %s\n", config);
    _timerDelayClass_DCMotorStepperClass = new TimerDelayClass(_delaySeconds_DCMotorStepper_setting);

}


//! from Orion's small program 3.16.25
#define MOTOR_IN1 13
#define MOTOR_IN2 12
#define BUZZER_IN1 14
#define BUZZER_IN2 27

#define NUM_PINS 2
////Put all the pins in an array to make them easy to work with
int _pins_DCMotorStepperClass[NUM_PINS]{
    MOTOR_IN1,  //IN1 on the ULN2003 Board, BLUE end of the Blue/Yellow motor coil
    MOTOR_IN2  //IN2 on the ULN2003 Board, PINK end of the Pink/Orange motor coil
};
/**
 This replaces the following, when clearPins() called
 #ifdef  USE_MOTOR
 pinMode(MOTOR_IN1, OUTPUT);
 pinMode(MOTOR_IN2, OUTPUT);
 pinMode(BUZZER_IN1, OUTPUT);
 pinMode(BUZZER_IN2, OUTPUT);
 digitalWrite(MOTOR_IN1, LOW);
 digitalWrite(MOTOR_IN2, LOW);
 digitalWrite(BUZZER_IN1, LOW);
 digitalWrite(BUZZER_IN2, LOW);
 #endif
 */


//Keeps track of the current direction
//Relative to the face of the motor.
//Clockwise (true) or Counterclockwise(false)
//We'll default to clockwise
bool _clockwise_DCMotorStepperClass = true;


/************* Set all motor pins off which turns off the motor ************************************************/
void clearPins_DCMotorStepper()
{
    for (int pin = 0; pin < NUM_PINS; pin++)
    {
        pinMode(_pins_DCMotorStepperClass[pin], OUTPUT);
        digitalWrite(_pins_DCMotorStepperClass[pin], LOW);
    }
}

//! These are called from StepperModule
//Prepare motor controller
void DCMotorStepperClass::setup_MotorStepper()
{
    clearPins_DCMotorStepper();                     // Go turn all motor pins off
    
    //! 3.26.25 put buzzer code here for now..
    pinMode(BUZZER_IN1, OUTPUT);
    pinMode(BUZZER_IN2, OUTPUT);
    digitalWrite(BUZZER_IN1, LOW);
    digitalWrite(BUZZER_IN2, LOW);
}


//! stop the motor
void DCMotorStepperClass::stop_MotorStepper()
{
    Serial.println("DCMotorStepper: STOP");
    
    //!stops motor
    clearPins_DCMotorStepper();
    
    //!user timer class instance
    _timerDelayClass_DCMotorStepperClass->stopDelay();
}

//This will advance the stepper clockwise once by the angle specified in SetupStepper. Example 16 pockets in UNO is 22.5 degrees
void DCMotorStepperClass::start_MotorStepper()
{
    //!call super
    _clockwise_DCMotorStepperClass = this->isClockwiseDirection();

    SerialDebug.println("**************** DCMotorStepper::Starting DCMotorStepper *******************");
    
#define NEW_CODE_HERE
    //Set the four pins to their proper state for the current step in the sequence,
    //and for the current direction
    
    if (_clockwise_DCMotorStepperClass) {
        digitalWrite(MOTOR_IN1, LOW);
        digitalWrite(MOTOR_IN2, HIGH);
    } else {
        digitalWrite(MOTOR_IN1, HIGH);
        digitalWrite(MOTOR_IN2, LOW);
    }
   
    //! grab this value.
    _delaySeconds_DCMotorStepper_setting = getPreferenceFloat_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING);

    
    //! start the delay..  (which at .5 might not be needed...)
    _timerDelayClass_DCMotorStepperClass->startDelay(_delaySeconds_DCMotorStepper_setting);
    
#define BUZZER_CODE
    //! 3.26.25 put buzzer code here for now ..
    boolean buzzerEnabled = getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE);
    if (buzzerEnabled)
    {
        digitalWrite(BUZZER_IN1, HIGH);
        digitalWrite(BUZZER_IN2, LOW);
        delay(500);
        digitalWrite(BUZZER_IN1, LOW);
        digitalWrite(BUZZER_IN2, LOW);
    }
    
    SerialDebug.println("**************** DCMotorStepper::Ending DCMotorStepper *************");

}


//!loop the PTStepper (so timer can run)
void DCMotorStepperClass::loop_MotorStepper()
{
    //!user timer class instance
    if (_timerDelayClass_DCMotorStepperClass->delayFinished())
    {
        this->stop_MotorStepper();
    }
}
