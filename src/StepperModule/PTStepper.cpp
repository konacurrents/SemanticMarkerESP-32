//! \link PTStepper

// 
// 
// 
// 

#include "PTStepper.h"

// from Dispense it calls getFeederType_mainModule
#include "StepperModule.h"

#ifdef ESP_M5
#else
/******************stepper declarations******************************/
int _ourSteps(0);
int _targetSteps;
int _lastType = 0;
int _fudgeFactor = 0;
// 

#ifdef BOARD
////Put all the pins in an array to make them easy to work with
int _pins[]{
    12,  //BLUE end of the Blue/Yellow motor coil    RED is ground
    14,  //PINK end of the Pink/Orange motor coil
    27,  //YELLOW end of the Blue/Yellow motor coil
    26   //ORANGE end of the Pink/Orange motor coil
};

#else


////Put all the pins in an array to make them easy to work with
int _pins[]{
    13,  //IN1 on the ULN2003 Board, BLUE end of the Blue/Yellow motor coil
    12,  //IN2 on the ULN2003 Board, PINK end of the Pink/Orange motor coil
    14,  //IN3 on the ULN2003 Board, YELLOW end of the Blue/Yellow motor coil
    27   //IN4 on the ULN2003 Board, ORANGE end of the Pink/Orange motor coil
};
#endif //BOARD

//Define the full step sequence.  
//With the pin (coil) states as an array of arrays
int _fullStepCount = 4;
int _fullSteps[][4] = {
    {HIGH,HIGH,LOW,LOW},
    {LOW,HIGH,HIGH,LOW},
    {LOW,LOW,HIGH,HIGH},
    {HIGH,LOW,LOW,HIGH}
};

//Keeps track of the current step.
//We'll use a zero based index. 
int _currentStep = 0;
//int _cycleCnt = 0;
int _cycleCounter = 0;

//Keeps track of the current direction
//Relative to the face of the motor. 
//Clockwise (true) or Counterclockwise(false)
//We'll default to clockwise
bool _clockwise = true;

void setup_PTStepper();
void cycle_PTStepper();
void step(int steps[][4], int stepCount);
/****************  end stepper declarations  *******************************************************************/

/***************** begin stepper actions     *******************************************************************/

/************* Set all motor pins off which turns off the motor ************************************************/
void clearPins() {
    for (int pin = 0; pin < 4; pin++) {
        pinMode(_pins[pin], OUTPUT);
        digitalWrite(_pins[pin], LOW);
    }

}

void whoAreWe() {
    // Define number of steps per rotation:
    // Calculate how many micro-steps to complete one feed sb 130 for Uno or 512 for Mini

    // Uno makes 16 steps per revolution
    if (getFeederType_mainModule() == UNO)
        _targetSteps = 128;
    // Mini makes 4 steps per revolution but reverses
    else if (getFeederType_mainModule() == MINI)
        _targetSteps = 512;
    // Tumbler accepts a variable number of degrees
    else if (getFeederType_mainModule() == TUMBLER)
        //converts degrees to motor cycles
        _targetSteps = (ceil) ( getPreferenceFloat_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING) / 0.17665361523586);
    
    // Fall thru handling *** new information from https://lastminuteengineers.com/28byj48-stepper-motor-arduino-tutorial/
    else
        _targetSteps = 2038;
    SerialDebug.printf("STEPPER STEPS = %d\n", _targetSteps);

    //! 6.14.2024 put this back as the if-else was goofed up.
    /**************** Fudge Factor code to correct registration *********************
     ** This code is added to make the stepper keep registration in the long run.  **
     ** it ran for 20 hours with a stepper angle of 15 and registarion was perfect.**
     ********************************************************************************/
    switch(_fudgeFactor) {
        case(0):
            _targetSteps = _targetSteps + 2;
            _fudgeFactor = 1;
            break;
        case(1):
            _targetSteps = _targetSteps + 2;
            _fudgeFactor = 2;
            break;
        case(2):
            _targetSteps = _targetSteps + 3;
            _fudgeFactor = 0;
            break;
    }

    _lastType = getFeederType_mainModule();          // let's not do this again until needed

    //!see if the motor direction is clockwise == true
    _clockwise = getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING);
    SerialDebug.printf("MOTOR_DIRECTION_CLOCKWISE = %d\n", _clockwise);
    
}

//Prepare motor controller
void setup_PTStepper() {
    clearPins();                     // Go turn all motor pins off
    //SerialDebug.print("ourSteps after  ");
    //SerialDebug.print(ourSteps);
    //SerialDebug.println(" ");
}

void step(int steps[][4], int stepCount) {
    //Then we can figure out what our current step within the sequence from the overall current step
    //and the number of steps in the sequence
    //SerialDebug.print("current step ");
    //SerialDebug.println(currentStep);
    int currentStepInSequence = _currentStep % stepCount;

    //Figure out which step to use. If clock wise, it is the same is the current step
    //if not clockwise, we fire them in the reverse order...
    int directionStep = _clockwise ? currentStepInSequence : (stepCount - 1) - currentStepInSequence;

    //Set the four pins to their proper state for the current step in the sequence, 
    //and for the current direction
    for (int pin = 0; pin < 4; pin++) {
        digitalWrite(_pins[pin], steps[directionStep][pin]);
    }
}

void cycle_PTStepper() {
    //Get a local reference to the number of steps in the sequence
    //And call the step method to advance the motor in the proper direction

    //Full Step
    int stepCount = _fullStepCount;
    step(_fullSteps, _fullStepCount);

    // Increment the program field tracking the current step we are on
    _currentStep++;

    // If targetSteps has been specified, and we have reached
    // that number of steps, reset the currentStep, and reverse directions
    if (_targetSteps != 0 && _currentStep == _targetSteps) {
        _currentStep = 0;
        clearPins(); // Turn off motor
    }
    else if (_targetSteps == 0 && _currentStep == stepCount) {
        // don't reverse direction, just reset the currentStep to 0
        // resetting this will prevent currentStep from
        // eventually overflowing the int variable it is stored in.
        _currentStep = 0;
        clearPins();
    }

    //2 milliseconds seems to be about the shortest delay that is usable.  Anything
    //lower and the motor starts to freeze. 
    delay(2);
}

//This will advance the stepper clockwise once by the angle specified in SetupStepper. Example 16 pockets in UNO is 22.5 degrees
void start_PTStepper() {
    if (_ourSteps == 0 || getFeederType_mainModule() != _lastType)
    {             // Not assigned yet, is zero or is different type
        whoAreWe();                                           // go figure who we are, Mini or Uno and calculate steps needed
        SerialDebug.print("Feeder type currently is ");
        SerialDebug.println(getFeederType_mainModule());
    }
    SerialDebug.println("**************** Starting Stepper *******************");
    
    while (_cycleCounter < _targetSteps) {
        // Step one unit in forward
        _cycleCounter++;
        cycle_PTStepper();
    }
    _cycleCounter = 0;       // Reset when done
    SerialDebug.println("**************** Ending Stepper *************");
    clearPins();
    
    
    if (getFeederType_mainModule() == MINI) {
        SerialDebug.println("**************** Starting Return *******************");
        _clockwise = !_clockwise;     // Time to go backwards
        while (_cycleCounter < _targetSteps - 5) {        //Don't quite go all the way back
                                                          // Step one unit backwards
            _cycleCounter++;
            cycle_PTStepper();
        }
        clearPins();
        _cycleCounter = 0;           // Reset when done
        _clockwise = !_clockwise;     // next time go forward
        SerialDebug.println("**************** Emding Return ***********");
        
    }
    
}

#endif //ESP_M5
