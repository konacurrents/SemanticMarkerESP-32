//
//  PTStepperClass.hpp
//  
//
//  Created by Scott Moody on 3/16/25.
//

#ifndef PTStepperClass_hpp
#define PTStepperClass_hpp

#include "MotorStepperClassType.h"

//! An concrete class
class PTStepperClass : public MotorStepperClassType
{
public:
    //! constructor
    PTStepperClass(char *config);
    
    //! destructor
    ~PTStepperClass();
    
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
private:
   
};

#endif /* PTStepperClass_hpp */
