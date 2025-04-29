//
//  SG90ServoStepperClass.hpp
//  
//
//  Created by Scott Moody on 3/31/25.
//

#ifndef SG90ServoStepperClass_hpp
#define SG90ServoStepperClass_hpp

#include "../../Defines.h"

#include "MotorStepperClassType.h"

//! An concrete class
class SG90ServoStepperClass : public MotorStepperClassType
{
public:
    //! constructor
    SG90ServoStepperClass(char *config);
    
    //! destructor
    ~SG90ServoStepperClass();
    
    //! Data members of class
    //! Generic config info .. specific the the sensor type
    
    //!starts the SG90Servo
    void start_MotorStepper();
    
    //!setup the SG90Servo
    void setup_MotorStepper();
    
    //! stops motor
    void stop_MotorStepper();
    
    //!setup the SG90Servo
    void loop_MotorStepper();
};

#endif /* SG90ServoStepperClass_hpp */
