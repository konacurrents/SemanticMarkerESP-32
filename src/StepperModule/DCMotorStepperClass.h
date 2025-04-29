//
//  DCMotorStepperClass.hpp
//
//
//  Created by Scott Moody on 3/16/25.
//

#ifndef DCMotorStepperClass_hpp
#define DCMotorStepperClass_hpp

#include "../../Defines.h"

#include "MotorStepperClassType.h"

//! An concrete class
class DCMotorStepperClass : public MotorStepperClassType
{
public:
    //! constructor
    DCMotorStepperClass(char *config);
    
    //! destructor
    ~DCMotorStepperClass();
    
    //! Data members of class
    //! Generic config info .. specific the the sensor type
    
    //!starts the PTStepper
    void start_MotorStepper();
    
    //!setup the PTStepper
    void setup_MotorStepper();
    
    //! stops motor
    void stop_MotorStepper();
    
    //!setup the PTStepper
    void loop_MotorStepper();
};

#endif


