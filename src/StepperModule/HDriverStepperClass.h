//
//  HDriverStepperClass.hpp
//  
//
//  Created by Scott Moody on 3/24/25.
//

#ifndef HDriverStepperClass_hpp
#define HDriverStepperClass_hpp

#include "../../Defines.h"

#include "MotorStepperClassType.h"

//! An concrete class
class HDriverStepperClass : public MotorStepperClassType
{
public:
    //! constructor
    HDriverStepperClass(char *config);
    
    //! destructor
    ~HDriverStepperClass();
    
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

#endif /* HDriverStepperClass_hpp */
