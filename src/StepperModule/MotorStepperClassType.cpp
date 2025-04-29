//
//  MotorStepperClass.cpp
//  
//
//  Created by Scott Moody on 3/16/25.
//

#include "MotorStepperClassType.h"

//!saves the identity
char _MotorStepperClassIdentity[100];

MotorStepperClassType::MotorStepperClassType(char *config)
{
    
    SerialDebug.printf("MotorStepperClassType init %s\n", config);
    strcpy(_MotorStepperClassIdentity, config);
}

MotorStepperClassType::~MotorStepperClassType()
{
    SerialDebug.printf("MotorStepperClassType destroy\n");
}

//! returns the identity .. actaully the config
//! This can be used to see if this class instance matches
//! a known name (like DCMotorStepper, etc)
char *MotorStepperClassType::classIdentity()
{
    return _MotorStepperClassIdentity;
}

//! returns if clockwise
boolean MotorStepperClassType::isClockwiseDirection()
{
    //!see if the motor direction is clockwise == true
    boolean clockwise = getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING);
    SerialDebug.printf("MOTOR_DIRECTION_CLOCKWISE = %d\n", clockwise);
    return clockwise;
}

