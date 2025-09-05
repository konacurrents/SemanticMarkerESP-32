//
//  MotorStepperClass.cpp
//  
//
//  Created by Scott Moody on 3/16/25.
//

#include "MotorStepperClassType.h"

//!saves the identity (OOPS.. not working.. it's shared by all class instances.. so last one wins..
//char _MotorStepperClassIdentity[100];

//! class wide method to set identity and check if pins were specified
MotorStepperClassType::MotorStepperClassType(char *config)
{
    //SerialDebug.printf("MotorStepperClassType init %s\n", config);
    //! 5.3.25 create storage here
    _identityString = (char*)calloc(strlen(config)+1, sizeof(char));
    strcpy(_identityString, config);
    
    //! 7.9.25
    //! return the sensor specified or null
    SensorStruct* sensorStruct = getSensor_mainModule(_identityString);
    if (sensorStruct)
    {
        SerialDebug.printf("Setting sensor type %s pins (%d,%d)\n", sensorStruct->sensorName, sensorStruct->pin1, sensorStruct->pin2);
        setPinValues(sensorStruct->pin1, sensorStruct->pin2);
    }

}

//! 9.4.25
//! delay amount after a start_MotorStepper
//! defaults = 0
int MotorStepperClassType::delayAmountBetweenMotor()
{
    return 0;
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
    SerialDebug.printf("MotorStepperClassType::classIdentity = %s\n", _identityString);
    return _identityString;
}

//! returns if clockwise
boolean MotorStepperClassType::isClockwiseDirection()
{
    //!see if the motor direction is clockwise == true
    boolean clockwise = getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING);
    SerialDebug.printf("MOTOR_DIRECTION_CLOCKWISE = %d\n", clockwise);
    return clockwise;
}


//! set PIN 1 and PIN 2 .
//! the class instance has to decide what to do with the pin definitions
void MotorStepperClassType::setPinValues(int pin1, int pin2)
{
    _pin1 = pin1;
    _pin2 = pin2;
}
