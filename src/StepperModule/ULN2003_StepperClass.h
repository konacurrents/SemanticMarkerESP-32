//
//  ULN2003_StepperClass.hpp
//  ESP_IOT
//
//  Created by Scott Moody on 8/13/25.
//

#ifndef ULN2003_StepperClass_hpp
#define ULN2003_StepperClass_hpp

#include "../../Defines.h"

#include "MotorStepperClassType.h"

//! An concrete class
class ULN2003_StepperClass : public MotorStepperClassType
{
public:
    //! constructor
    ULN2003_StepperClass(char *config);
    
    //! destructor
    ~ULN2003_StepperClass();
    
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
#endif /* ULN2003_StepperClass_hpp */
