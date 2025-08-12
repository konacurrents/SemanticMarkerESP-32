/**
*  \link StepperModule
*/

#include "StepperModule.h"


//! 3.16.25 using a class version (thisi is for ESP_32 and ESP_M5 now)

#include "PTStepperClass.h"
#include "DCMotorStepperClass.h"
#include "MotorStepperClassType.h"
//! 3.24.25 warm raining day
#include "HDriverStepperClass.h"
//! 3.31..25
#include "SG90ServoStepperClass.h"
//! 5.18.25 45 years Mt St Hellens Eruption
#include "L9110S_DCStepperClass.h"

//! MAKE SURE THIS IS UPDATED...
#define NUM_STEPPER_CLASS 5

//! instances of the PTStepeprClass
DCMotorStepperClass* _DCMotorStepperClass;
PTStepperClass* _PTStepperClass;
HDriverStepperClass* _HDriverStepperClass;
//! 3.31.25 try Servo ..
SG90ServoStepperClass* _SG90ServoStepperClass;
//! 5.18.25 45 years Mt St Hellens Eruption
L9110S_DCStepperClass* _L9110S_DCStepperClass;

//! use this one...
MotorStepperClassType *_whichMotorStepper;

//! 3.31.25 create array of plugs
MotorStepperClassType* _motorSteppers[NUM_STEPPER_CLASS];

void setup_StepperClasses()
{
    int whichStepperClasssIndex = 0;
    SerialDebug.println("setup_StepperClasses");
        
    //! instances of the PTStepeprClass
    //! NOTE: These are the "identity' so that is also the "message" sent around. So SELF defining.
    _DCMotorStepperClass = new DCMotorStepperClass((char*)"DCMotorStepperClass");
    _motorSteppers[whichStepperClasssIndex++] = _DCMotorStepperClass;
    
    _PTStepperClass = new PTStepperClass((char*)"PTStepperClass");
    _motorSteppers[whichStepperClasssIndex++] = _PTStepperClass;

    _HDriverStepperClass = new HDriverStepperClass((char*)"HDriverStepperClass");
    _motorSteppers[whichStepperClasssIndex++] = _HDriverStepperClass;

    //! 3.31.25 try Servo ..
    _SG90ServoStepperClass = new SG90ServoStepperClass((char*)"SG90ServoStepperClass");
    _motorSteppers[whichStepperClasssIndex++] = _SG90ServoStepperClass;

    //! 5.18.25 45 years Mt St Hellens Eruption
    _L9110S_DCStepperClass = new L9110S_DCStepperClass((char*)"L9110S_DCStepperClass");
    _motorSteppers[whichStepperClasssIndex++] = _L9110S_DCStepperClass;

    if (whichStepperClasssIndex != NUM_STEPPER_CLASS)
    {
        SerialError.printf("stepperIndex %d not %d\n", whichStepperClasssIndex, NUM_STEPPER_CLASS);
    }
    //!TODO: create an array of plug options .. then call their "identity" method
    SerialDebug.println("Done setup_StepperClasses");
}

//! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
void stepperModule_savePreference(int preferenceID, String preferenceValue);

/*feederState*/
#define FEED_STOPPED    0
#define SINGLE_FEED    1
#define AUTO_FEED    2
#define JACKPOT_FEED    3

int _feedState = FEED_STOPPED;
int _feedsPerJackpot = 3; //this will be set by user at some point
int _feedCount = _feedsPerJackpot; //init to max feeds per Jackpot

#define FEED_INTERVAL 1000           // make this a variable determined by user input and stored in JSON
unsigned long feedPreviousMillis = 0;

//proteced
// someone else the "savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE) knows where to save it..

//!returns the buzzer status
boolean getBuzzStatus_StepperModule()
{
    boolean buzzStatus = getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE);
    return buzzStatus;
}

//! retreives the feeder type (versus grabbing a global variable)
int getFeederType()
{
    return getFeederType_mainModule();
}

//!called on setup()
void setup_StepperModule()
{
    
    _feedState = FEED_STOPPED;
    //! 3.28.24 feed on startup..
#ifdef FEED_ON_STARTUP
    _feedState = SINGLE_FEED;
#endif

    //! 3.16.25 use the class ..
    setup_StepperClasses();
    
    //! NOTE: this is only re-evaluated on reboot .. for now
    //! 3.16.25  for now this is compile option ..
    //! but no reason the same build cannot support it..
    //! sensor kind (see the M5AtomSensor .. as example)
    char *sensorPlug = getPreference_mainModule(PREFERENCE_SENSOR_PLUGS_SETTING);
    SerialDebug.printf("sensorPlug = %s\n", sensorPlug);
    _whichMotorStepper = NULL;

    //! find it dynamically
    //! 3.31.25
    for (int i=0; i< NUM_STEPPER_CLASS; i++)
    {
        if (!_motorSteppers[i])
        {
            SerialDebug.printf("sensorPlug[%d] == null\n", i);
            continue;
        }
        if (strcmp(sensorPlug,_motorSteppers[i]->classIdentity())==0)
        {
            _whichMotorStepper = _motorSteppers[i];
            SerialDebug.printf("Match motorSteppers[%d] = %s\n", i, sensorPlug);
            break;
        }
    }
    //! if nothing found .. use default
    if (!_whichMotorStepper)
    {
#ifdef ESP_32
        //! DEFAULT .. if (strcmp(sensorPlug,"PTStepperClass")==0)
        //! use this one...
        _whichMotorStepper = _PTStepperClass;
#else
        //! the motorStepper can be nil
        //_whichMotorStepper = NULL;
        //! 5.2.25 default to HDriver..
        //_whichMotorStepper = _HDriverStepperClass;
        
        //! 7.31.25 use default..
        _whichMotorStepper = _L9110S_DCStepperClass;
#endif
    }

#define SETUP_FIRST_TIME
#ifdef  SETUP_FIRST_TIME
    if (_whichMotorStepper)
        _whichMotorStepper->setup_MotorStepper();
#endif

}
//! 4.1.25 April Fools day. Mt Peak hike (162 bmp strange spike)
//! get the identity of the SenasorPlug
//! nil if non
char *stepperIdentity_StepperModule()
{
    char *identity = NULL;
    if (_whichMotorStepper)
    {
        identity = _whichMotorStepper->classIdentity();
    }
    return identity;
}
//TODO: this isn't directly called.. but called from the main processsClientCommand(String cmd)
////Keep ProcessClientCmd short to let the callback run. instead change the feeder state flag
void stepperModule_ProcessClientCmdFinal(char cmd) {
    SerialLots.printf("***** ProcessClientCmd(%c) from client*****\n", cmd);
    
    if ((cmd == 0x00) || (cmd == 's') || (cmd == 'c'))
    {
        SerialLots.println("Setting FeedState = SINGLE_FEED");
        _feedState = SINGLE_FEED;  // Gen 3 Pet Tutor used 0x00 to feed so 0x00 is to make it backward compatible  's' is the new version
                                   //not used .. for awhile.. So let's stop writing to EPROM all the time.
    }
    else if (cmd == 'a')
    {
        SerialLots.println("Setting FeedState = AUTO_FEED");
        _feedState = AUTO_FEED;

    }
    else if (cmd == 'j')
    {
        SerialLots.println("Setting FeedState = JACKPOT_FEED");
        _feedState = JACKPOT_FEED;

    }
    else if (cmd == 'B')
    {
        SerialLots.println("1.Setting buzzStatus = BUZZON");
        
        //set the flag, then buz..  (or don't care, and the value is stored in mainModule.. TODO: maybe that should cache the result)
#ifdef USE_UI_MODULE
        //make a buzz sound..
        blinkLED_UIModule();
        setBuzzerLight_UIModule(true);
#endif

    }
    else if (cmd == 'b')
    {
        SerialLots.println("2.Setting buzzStatus = BUZZOFF");
#ifdef USE_UI_MODULE
        setBuzzerLight_UIModule(false);
#endif
    }

    //!NOTE: There are other commands, but these are only the ones that require a action.
    //!TODO: make these "events" on the setting of those values. eg. register for stepperValue true or false, etc.. They would be invoked from the 'savePreferencesBoolean' call.
}
//void FeederStateMachine() {

//!the main loop fro the StepperModule. This used be called FeederStateMachine()
void loop_StepperModule()
{
    //! 3.24.25 warm rain all day.. no ski
    //! call the loop (which might have a timer to stop a motor)
    if (_whichMotorStepper)
        _whichMotorStepper->loop_MotorStepper();
    // FeedState is NO LONGER public and can be set by BLE client via the GATT for the characteristic
    
    switch (_feedState)
    {
        case SINGLE_FEED:
        {
#ifdef USE_UI_MODULE
            blinkLED_UIModule();
#endif
            SerialDebug.println("SINGLE Feed"); //this should write a 0x01 for feed ack

            if (_whichMotorStepper)
                _whichMotorStepper->start_MotorStepper();

            _feedState = FEED_STOPPED; //this will cancel Continous feed if it is set
            
            //! 5.3.24 Issue #332 if TUMBLER .. reverse direction
            if (getFeederType_mainModule() == STEPPER_IS_TUMBLER)
            {
                //! 8.2.24 have the auto a mode as well.. for older Tumblers. Default is ON (so we can change the old ones),
                //! message: {"set":"autoMotorDirection","val":"true"}
                boolean autoMotorDirection = getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_AUTO_MOTOR_DIRECTION_SETTING);
                if (autoMotorDirection)
                {
                    // reverse direction, the Q command
                    //! note: reboot not needed as the next time a feed happens, it reads this value
                    // motor direction ==  (reverse)
                    boolean  currentDirection = getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING);
                    currentDirection = !currentDirection;
                    savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING,currentDirection);
                }
            }

            break;
        }

            //NOT USED..
        case JACKPOT_FEED:  //note: the only difference in this case is the FEED_STOPPED does NOT get set
        {
            if (_feedCount > 0) {
                unsigned long feedCurrentMillis = millis();
                if (feedCurrentMillis - feedPreviousMillis >= FEED_INTERVAL) {  // changed this to check a timer to see when to fire the feeder
#ifdef USE_UI_MODULE
                    blinkLED_UIModule();                          // Moved 021622 WJL
#endif
                    
                    if (_whichMotorStepper)
                        _whichMotorStepper->start_MotorStepper();

                    SerialDebug.println("JACKPOT Feed");
                    _feedCount--;
                    feedPreviousMillis = feedCurrentMillis;
                }
            }
            else {
                _feedCount = _feedsPerJackpot; //reset counter after all feeds for Jackpot completed
                _feedState = FEED_STOPPED;
            }
            break;
        }
        case FEED_STOPPED:
        {
            //SerialDebug.println("Feed Stopped --  ");
            //do nothing
            break;
        }
        default:
        {
            _feedState = FEED_STOPPED;
            break;
        }
    }
}

