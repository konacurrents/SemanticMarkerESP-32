//
//  DCMotorStepperClass.hpp
//
//
//  Created by Scott Moody on 3/16/25.
//

#ifndef MotorStepperClass_h
#define MotorStepperClass_h

#include "../../Defines.h"

//! An mostly virtual class
class MotorStepperClassType
{
public:
    //! constructor. Note: config is currently just
    //! the name of the plug (eg. HDriverStepperClass
    //! or DCMotorStepperClass
    //! BUT: it could be extended in the future
    //! to include the PIN numbers, etc.
    //! This might be JSON:  {'id':'DCMotorStepperClass,'pin1':'33'}
    //! class wide method to set identity and check if pins were specified
    MotorStepperClassType(char *config);
    
    //! destructor
    ~MotorStepperClassType();
    
    //! Data members of class
    //! Generic config info .. specific the the sensor type
    //! note: no loop()
    //!starts the PTStepper
    virtual void start_MotorStepper() = 0;
    
    //!setup the PTStepper
    virtual void setup_MotorStepper() = 0;
    
    //! stop the motor
    virtual void stop_MotorStepper() = 0;
    
    //!loop the PTStepper (so timer can run)
    virtual void loop_MotorStepper() = 0;
    
    //! returns if clockwise
    boolean isClockwiseDirection();
    
    /* Other members class-wide, Not Abstract, so in SensorClassType.cpp*/
    //! set PIN 1 and PIN 2 .
    //! the class instance has to decide what to do with the pin definitions
    void setPinValues(int pin1, int pin2);
    
    //! returns the identity .. actaully the config
    //! This can be used to see if this class instance matches
    //! a known name (like DCMotorStepper, etc)
    char *classIdentity();
    
    //! pin definitions
    //! pin1
    int _pin1 = 0;
    //! pin2
    int _pin2 = 0;
private:
    //! 5.3.25 create storage here. This will create dynamic memory
    //! and answer to the classIIdentity() method
    char *_identityString;
    
   
};

#endif /* MotorStepperClass_hpp */
