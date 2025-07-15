//
//  L9110SDCMotorStepperClass.hpp
//  
//
//  Created by Scott Moody on 5/19/25.
//

#ifndef L9110SDCMotorStepperClass_hpp
#define L9110SDCMotorStepperClass_hpp

#include "../../Defines.h"

#include "MotorStepperClassType.h"

//! An concrete class
class L9110S_DCStepperClass : public MotorStepperClassType
{
public:
    //! constructor
    L9110S_DCStepperClass(char *config);
    
    //! destructor
    ~L9110S_DCStepperClass();
    
    //! Data members of class
    //! Generic config info .. specific the the sensor type
    
    //!starts the PTStepper
    void start_MotorStepper();
    
    //!setup the PTStepper
    void setup_MotorStepper();
    
    //! stops motor
    void stop_MotorStepper();
    
    //!loop the PTStepper (so timer can run)
    void loop_MotorStepper();
    
};


#endif /* L9110SDCMotorStepperClass_hpp */
