

#ifndef ModelController_h
#define ModelController_h

//*** For the MenuSelection MODEL
//! a pseudo Class

//!the ModelKindEnum is what state we are holding. There will be state models behind each of these.
typedef enum
{
    pairedDeviceModel = 0,
    rebootModel = 1,
    menusModel = 2,
    timerModel = 3,
} ModelKindEnum;
#define ModelKindEnumMax 4

//!MENUS
//! Pairing
//! APMode
//! Timer
//! Advanced
//! Status
//! WIFI sharing
//! Guest Feed
//! Settings
//! DocFollow
//! help
//! HomePage
//! /WIFI Feed
//! WIFI Share


//!state variables
typedef enum
{
    //paired to a device, but BLE NOT connected right now
    pairedButNotConnectedEnum,
    //paired to a device, and BLE connected right now
    pairedAndConnectedEnum,
    //factory default: not paired (but could be paired as it's a named device])
    pairableAndConnectedEnum,
    //!factory default but not connecte to anything..
    notConnectedEnum
} PairedDeviceStateEnum;

//!the struct for the models. Since this isn't straight OO, we are overlaying information..
typedef struct
{
    //!What kind of model
    ModelKindEnum modelKindEnum;
    
    //! which SM mode 0.. SM_LAST
    int SM_Mode;
    
    //!a preference to the View that big text is desired
    boolean perfersBigText;
    
    //!the number of items in the model
    int maxItems;
    //!the current item
    int currentItem;
    
    //! if pairedDeviceModel then..
    //! what is the state of the pairing..
    PairedDeviceStateEnum pairedDeviceStateEnum;
    
    //!Timer info  - When the timer was started
    int delayStartMillis;
    //!the value returned from random .. the diff is taken off this number
    //! but this number doesn't change except next random call
    int counterLoopAbsoluteSeconds;
    //! true if still waiting for delay to finish
    boolean delayRunning = false;
    //! length of delay (changable..)
    int delaySeconds;
    //! 11.29.23 max Delay Seconds .. if different than delay then next delay will be random() * (max-min) + min
    int delaySecondsMax;
    
    //! finished flag (let user know .. then restart)
    boolean finishedTimer;
    //! current seconds (<= delaySeconds)
    int currentCounterSeconds;
    
    //! for the next page state 0..n
    int currentPageState;
    // the max before goes back to 0
    int maxPageState;
    //! the stream number
    int currentStreamNum;
    // max streams (usually 3 1..3)
    int maxStream;
    //!which wave
    char *waveName;
    
    //! feed local instead of MQTT or BLE..
    boolean feedLocalOnly;
    
} ModelStateStruct;

//!updates the model for the menu state (this will initialize if not done yet..)
void updateMenuState(ModelKindEnum modelKind);
//!returns the menu string for the deviceState's item number (use woudl go 0..maxItems -1
char *menuForState(ModelKindEnum modelKind, int item);
//!increments the device states deviceState (wrapping around)
void incrementMenuState(ModelKindEnum modelKind);
//!restarts all the menu states to the first one .. useful for getting a clean start. This doesn't care if the menu is being shown
void restartAllMenuStates_ModelController();
//!invokes the menu state, return true if the model state has change enough to refreesh your View (such as new menu items)
boolean invokeMenuState(ModelKindEnum modelKind);
//!retrieves the state model for the modelKind
ModelStateStruct *getModel(ModelKindEnum modelKind);
//!retrieves the state model for the SM_Mode (SM_0 .. SM_LAST)l. NULL if none
ModelStateStruct *hasModelForSM_Mode(int SM_Mode);

//!retrieves a semantic marker for the current selected item. Return null if none specified YET
//!TODO.. right now the logis is in MQTTNetworking .. so need to figure something nice out..
char *getModelSemanticMarker(ModelKindEnum modelKind);

//!initialize the objects
void initModelStructs_ModelController();

//!! TIMER Remote control set delay seconds
//!MQTT:  set: timerdelay, val:seconds
void setTimerDelaySeconds_mainModule(int delaySeconds);

//!! TIMER Remote control set delay seconds
//!MQTT:  set: timerdelayMax, val:seconds
void setTimerDelaySecondsMax_mainModule(int delaySecondsMax);

//!! TIMER Remote control start
//! MQTT:  set: starttimer, val: true/false  (true == start timer, false = stop timer)
void startStopTimer_mainModule(boolean startTimer);

//!performs the pairing..  to whatever is currently connected, this means a message could make that happen
//!for a device (ESP-32) with no user interface.
void invokePair_ModelController();

//! just unpair .. don't skip
//!performs the unpairing
void invokeUnpairNoName_ModelController();

#endif
