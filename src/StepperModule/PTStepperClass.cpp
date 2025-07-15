//! \link PTStepperClass
//  PTStepperClass.cpp
//  
//
//  Created by Scott Moody on 3/16/25.
//

#include "PTStepperClass.h"

//! for now .. use the PTStepper.h
//#include "PTStepper.h"

// from Dispense it calls getFeederType_mainModule()
#include "StepperModule.h"

/******************stepper declarations******************************/
int _ourSteps_PTStepperClass(0);
int _targetSteps_PTStepperClass;
int _lastType_PTStepperClass = 0;
int _fudgeFactor_PTStepperClass = 0;

PTStepperClass::PTStepperClass(char *config) : MotorStepperClassType(config)
{
    SerialDebug.printf("PTStepperClass init %s\n", config);
}

//

#ifdef BOARD
////Put all the pins in an array to make them easy to work with
int _pins_PTStepperClass[]{
    12,  //BLUE end of the Blue/Yellow motor coil    RED is ground
    14,  //PINK end of the Pink/Orange motor coil
    27,  //YELLOW end of the Blue/Yellow motor coil
    26   //ORANGE end of the Pink/Orange motor coil
};

#else


////Put all the pins in an array to make them easy to work with
int _pins_PTStepperClass[]{
    13,  //IN1 on the ULN2003 Board, BLUE end of the Blue/Yellow motor coil
    12,  //IN2 on the ULN2003 Board, PINK end of the Pink/Orange motor coil
    14,  //IN3 on the ULN2003 Board, YELLOW end of the Blue/Yellow motor coil
    27   //IN4 on the ULN2003 Board, ORANGE end of the Pink/Orange motor coil
};
#endif //BOARD

//Define the full step sequence.
//With the pin (coil) states as an array of arrays
int _fullStepCount_PTStepperClass = 4;
int _fullSteps_PTStepperClass[][4] = {
    {HIGH,HIGH,LOW,LOW},
    {LOW,HIGH,HIGH,LOW},
    {LOW,LOW,HIGH,HIGH},
    {HIGH,LOW,LOW,HIGH}
};

//Keeps track of the current step.
//We'll use a zero based index.
int _currentStep_PTStepperClass = 0;
//int _cycleCnt = 0;
int _cycleCounter_PTStepperClass = 0;

//Keeps track of the current direction
//Relative to the face of the motor.
//Clockwise (true) or Counterclockwise(false)
//We'll default to clockwise
bool _clockwise_PTStepperClass = true;


void step_PTStepperClass(int steps[][4], int stepCount);
/****************  end stepper declarations  *******************************************************************/

/***************** begin stepper actions     *******************************************************************/

/************* Set all motor pins off which turns off the motor ************************************************/
void clearPins_PTStepperClass() {
    for (int pin = 0; pin < 4; pin++) {
        pinMode(_pins_PTStepperClass[pin], OUTPUT);
        digitalWrite(_pins_PTStepperClass[pin], LOW);
    }
    
}

//! calculate the steps etc
void whoAreWe_PTStepperClass()
{
    // Define number of steps per rotation:
    // Calculate how many micro-steps to complete one feed sb 130 for Uno or 512 for Mini
    
    // Uno makes 16 steps per revolution
    if (getFeederType_mainModule() == UNO)
        _targetSteps_PTStepperClass = 128;
    // Mini makes 4 steps per revolution but reverses
    else if (getFeederType_mainModule() == MINI)
        _targetSteps_PTStepperClass = 512;
    // Tumbler accepts a variable number of degrees
    else if (getFeederType_mainModule() == TUMBLER)
        //converts degrees to motor cycles
        _targetSteps_PTStepperClass = (ceil) ( getPreferenceFloat_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING) / 0.17665361523586);
    
    // Fall thru handling *** new information from https://lastminuteengineers.com/28byj48-stepper-motor-arduino-tutorial/
    else
        _targetSteps_PTStepperClass = 2038;
    SerialDebug.printf("STEPPER STEPS = %d\n", _targetSteps_PTStepperClass);
    
    //! 6.14.2024 put this back as the if-else was goofed up.
    /**************** Fudge Factor code to correct registration *********************
     ** This code is added to make the stepper keep registration in the long run.  **
     ** it ran for 20 hours with a stepper angle of 15 and registarion was perfect.**
     ********************************************************************************/
    switch(_fudgeFactor_PTStepperClass) {
        case(0):
            _targetSteps_PTStepperClass = _targetSteps_PTStepperClass + 2;
            _fudgeFactor_PTStepperClass = 1;
            break;
        case(1):
            _targetSteps_PTStepperClass = _targetSteps_PTStepperClass + 2;
            _fudgeFactor_PTStepperClass = 2;
            break;
        case(2):
            _targetSteps_PTStepperClass = _targetSteps_PTStepperClass + 3;
            _fudgeFactor_PTStepperClass = 0;
            break;
    }
    
    _lastType_PTStepperClass = getFeederType_mainModule();          // let's not do this again until needed
    
    //!see if the motor direction is clockwise == true
    _clockwise_PTStepperClass = getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING);
    SerialDebug.printf("MOTOR_DIRECTION_clockwise_PTStepperClass = %d\n", _clockwise_PTStepperClass);
    
}

//Prepare motor controller
void PTStepperClass::setup_MotorStepper()
{
    clearPins_PTStepperClass();                     // Go turn all motor pins off
                                     //SerialDebug.print("ourSteps after  ");
                                     //SerialDebug.print(ourSteps);
                                     //SerialDebug.println(" ");
}

void step_PTStepperClass(int steps[][4], int stepCount)
{
    //Then we can figure out what our current step within the sequence from the overall current step
    //and the number of steps in the sequence
    //SerialDebug.print("current step ");
    //SerialDebug.println(currentStep);
    int currentStepInSequence = _currentStep_PTStepperClass % stepCount;
    
    //Figure out which step to use. If clock wise, it is the same is the current step
    //if not clockwise, we fire them in the reverse order...
    int directionStep = _clockwise_PTStepperClass ? currentStepInSequence : (stepCount - 1) - currentStepInSequence;
    
    //Set the four pins to their proper state for the current step in the sequence,
    //and for the current direction
    for (int pin = 0; pin < 4; pin++) {
        digitalWrite(_pins_PTStepperClass[pin], steps[directionStep][pin]);
    }
}

//! cycle
void cycle_PTStepperClass()
{
    //Get a local reference to the number of steps in the sequence
    //And call the step method to advance the motor in the proper direction
    
    //Full Step
    int stepCount = _fullStepCount_PTStepperClass;
    step_PTStepperClass(_fullSteps_PTStepperClass, _fullStepCount_PTStepperClass);
    
    // Increment the program field tracking the current step we are on
    _currentStep_PTStepperClass++;
    
    // If targetSteps has been specified, and we have reached
    // that number of steps, reset the currentStep, and reverse directions
    if (_targetSteps_PTStepperClass != 0 && _currentStep_PTStepperClass == _targetSteps_PTStepperClass) {
        _currentStep_PTStepperClass = 0;
        clearPins_PTStepperClass(); // Turn off motor
    }
    else if (_targetSteps_PTStepperClass == 0 && _currentStep_PTStepperClass == stepCount) {
        // don't reverse direction, just reset the currentStep to 0
        // resetting this will prevent currentStep from
        // eventually overflowing the int variable it is stored in.
        _currentStep_PTStepperClass = 0;
        clearPins_PTStepperClass();
    }
    
    //2 milliseconds seems to be about the shortest delay that is usable.  Anything
    //lower and the motor starts to freeze.
    delay(2);
}

//This will advance the stepper clockwise once by the angle specified in SetupStepper. Example 16 pockets in UNO is 22.5 degrees
void PTStepperClass::start_MotorStepper() {
    if (_ourSteps_PTStepperClass == 0 || getFeederType_mainModule() != _lastType_PTStepperClass)
    {             // Not assigned yet, is zero or is different type
        whoAreWe_PTStepperClass();                                           // go figure who we are, Mini or Uno and calculate steps needed
        SerialDebug.print("Feeder type currently is ");
        SerialDebug.println(getFeederType_mainModule());
    }
    SerialDebug.println("**************** PTStepperClass::Starting Stepper *******************");
    
    //! 5.15.25 try the async CLICK
    //! click call
    //! click call  5.26.25 SYNC version
    main_dispatchSyncCommand(SYNC_CLICK_SOUND);
    
    while (_cycleCounter_PTStepperClass < _targetSteps_PTStepperClass) {
        // Step one unit in forward
        _cycleCounter_PTStepperClass++;
        cycle_PTStepperClass();
    }
    _cycleCounter_PTStepperClass = 0;       // Reset when done
    SerialDebug.println("**************** PTStepperClass::Ending Stepper *************");
    clearPins_PTStepperClass();
    
    
    if (getFeederType_mainModule() == MINI) {
        SerialDebug.println("**************** PTStepperClass::Starting Return *******************");
        _clockwise_PTStepperClass = !_clockwise_PTStepperClass;     // Time to go backwards
        while (_cycleCounter_PTStepperClass < _targetSteps_PTStepperClass - 5) {        //Don't quite go all the way back
                                                          // Step one unit backwards
            _cycleCounter_PTStepperClass++;
            cycle_PTStepperClass();
        }
        clearPins_PTStepperClass();
        _cycleCounter_PTStepperClass = 0;           // Reset when done
        _clockwise_PTStepperClass = !_clockwise_PTStepperClass;     // next time go forward
        SerialDebug.println("**************** PTStepperClass::Ending Return ***********");
        
    }
    
}
//! stops motor
void PTStepperClass::stop_MotorStepper()
{
    
}
//!loop the PTStepper (so timer can run)
void PTStepperClass::loop_MotorStepper()
{
    
}
