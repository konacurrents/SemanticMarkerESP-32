
#ifndef MainModule_h
#define MainModule_h

#include "../../Defines.h"

//!added 9.29.22 to support wildcards #196
#include "TokenParser.h"

//! always avaialble to others.. the model controller part of MVC. The View is in DisplayModule. The other part of Controller is the button clicks in ButtonProcess.h (but with this, there is no logic there..)
#include "ModelController.h"

//! preferences
#include "PreferencesController.h"

//! 3.29.25 add the TimerDelayClass
#include "TimerDelayClass.h"

//! 6.6.25 include to get the current one..
#include "../M5AtomClassModule/M5AtomClassType.h"

#ifdef TODO_CANT_GET_TO_COMPILE
//! 6.6.25 get the current M5AtomClassType
M5AtomClassType* whichM5AtomClassType();
#else
//! 6.7.25 hot air balloons over house..
//! stops the motor
void stopMotor_mainModule();
//! gets if PTFeeder a surrogate for the M5Atom class
boolean isPTFeeder_mainModule();
#endif

//!THIS IS the setup() and loop() but using the "component" name, eg MQTTNetworking()
//!This will perform preference initializtion as well
//! called from the setup()
void setup_mainModule();

//! called for the loop() of this plugin
void loop_mainModule();

//!cleans the main module EPROM
void cleanEPROM_mainModule();

//!stop all loops... while OTA working..
void stopProcessesForOTAUpdate_mainModule();
//! if stopped
boolean stopAllProcesses_mainModule();
//!restart all loops... while OTA working..
void restartProcessesForOTAUpdate_mainModule();

//! 8.18.24 setting this will check for the factory setting..
void setClockwiseMotorDirection_mainModule(boolean isClockwiseFlag);

#define PT_SERVICE_UUID        "b0e6a4bf-cccc-ffff-330c-0000000000f0"  //Pet Tutor feeder service for feed  NOTE: Lower case for GEN3 compatability
#define PT_CHARACTERISTIC_UUID "b0e6a4bf-cccc-ffff-330c-0000000000f1"  //Pet Tutor feeder characteristic  NOTE: Lower case for GEN3 compatability

#ifdef ESP_M5
#define LED 2
#endif

#define TOPIC_TO_SEND (char*)"usersP/bark"
//#define GROUP_TOPIC_TO_SEND (char*)"/usersP/groups"
//!returns a groupTopic to use as a topic
char *groupTopicFullName(char *groupName);

//!10000 == no poweroff
#define NO_POWEROFF_AMOUNT_MAIN 10000
#define NO_POWEROFF_AMOUNT_STRING_MAIN (char*)"10000"


//! 1.12.24 add a temporary LUX dark
//! threshholdKind = 0 (LIGHT), 1=(DARK) .. others might be 2=super dark
#define THRESHOLD_KIND_LIGHT 0
#define THRESHOLD_KIND_DARK 1
void setLUXThreshold_mainModule(int thresholdKind, int luxVal);
int  getLUXThreshold_mainModule(int thresholdKind);

//! processes a message that might save in the EPROM.. the cmd is still passed onto other (like the stepper module)
//! returns true if finished processing, otherwise this can be sent onto other modules (like stepper)
//void processClientCommand_mainModule(String message);
//! single character version of processClientCommand (since many used that already)
void processClientCommandChar_mainModule(char cmd);
//! New RegisterCallback that works across a number of callback modules
//!
//! callbacksModuleId
#define CALLBACKS_MQTT 0
#define CALLBACKS_BUTTON_MODULE 1
#define CALLBACKS_BLE_CLIENT 2
#define CALLBACKS_BLE_SERVER 3
#define CALLBACKS_MODULE_MAX 4


#define SINGLE_CLICK_BM 0
#define MAX_CALLBACKS_BM 1

//!register the callback based on the callbackType. use the callbacksModuleId for which one..
void registerCallbackMain(int callbacksModuleId, int callbackType, void (*callback)(char*));
//!performs the indirect callback based on the callbackType
void callCallbackMain(int callbacksModuleId, int callbackType, char *message);

//!adding a synchronous call to send a message over the network (assuming MQTT but not specified), this tacks on {device} and {t:time}
void sendMessageString_mainModule(char *messageString);

//!adding a synchronous call to send a message over the network (assuming MQTT but not specified), this tacks on {device} and {t:time}
void sendMessageStringTopic_mainModule(char *messageString, char*topicString);

//! 8.16.25 MQTT
//!example callback: but the scope would have the pCharacteristic defined, etc..
//!This is pased just before the setupMQTTNetworking() is called..
void feedMessageCallback(char *message);

//!supports turning on the solid light.. 
void solidLightOnOff(boolean flag);

//COMMON CALLBACKS
//NOTE: This is where the true Object Oriented dynamic plug-in approaches would come in..
// We are using #ifdef to bound the plug-in but there is still too much knowledge about wiring events to methods
// in this code.. TODO, make more dynmaic event based..

//*** The callback for "onWrite" of the bluetooth "onWrite'
void onWriteBLEServerCallback(char *message);

/*******************************BLE Server*************************************/
//*** The callback for "onWrite" of the bluetooth "onWrite'
void onBLEServerCallback(char *message);

//! ** The callback for "status messages" of the bluetooth
void onStatusMessageBLEServerCallback(char *message);

//!prints the module configuration by looking at defines
//! Eventually this might be an object returned letting the code
//!know a capability is included for runtime (vs compile time) decisions
void main_printModuleConfiguration();

//!called on single click
//!NOTE: with BLE_CLIENT_NETWORKING, the right button and top button send a BLE command for feeding..
void singleClickTouched(char *whichButton);

//!callback for SOLID blinking led
void solidLight(char *message);

//!callback for blinking led
void blinkMessageCallback(char *message);


//!take a picture (calls the camera module).. what to do with picture??? TODO
//void takePicture_MainModule();

//!clean the SSID eprom (MQTT_CLEAN_SSID_EPROM)
void cleanSSID_EPROM_MessageCallback(char *message);

//!callback for SOLID blinking led
void solidLightMessageCallback(char *message);

//!clean the saved WIFI credential, otherwise the AP mode doesn't work (6.3.22)
void main_cleanSavedWIFICredentials();

//!TESTING for saying credentials are filled in via BLE (while in AP mode), so kick out of AP and continue..
//! set that the credentials are set. Called from the MQTT after processJSONMessage() found things were good..
//! April 8, 2022
void main_credentialsUpdated();

//!retrieve a JSON string for the ssid and ssid_password: {'ssid':<ssid>,'ssidPassword':<pass>"}
char *main_JSONStringForWIFICredentials();

//!! cycle through the next WIFI saved credential - return next one that isn't our current one..
char *main_nextJSONWIFICredential();

//! sets the WIFI and MQTT user/password. It's up to the code to decide who needs to know (currently this calls APmodule..)
void main_updateMQTTInfo(char *ssid, char *ssid_password, char *username, char *password, char *guestPassword, char *deviceName, char * host, char * port, char *locationString);

//! return the username and password
//! 12.14.23 to support calling the SMART buttons (smrun) with parameters
char *main_getUsername();
//! return password
char *main_getPassword();
//! return devicename
char *main_getScannedDeviceName();
//! set the scanned device name
void main_setScannedDeviceName(char *deviceName);
//! set the scanned group name
//! 1.7.24
void main_setScannedGroupName(char *groupName);
//! return groupname -- returns "" or nil if not set,
//! or the FULL group name topic, or nil
char *main_getScannedGroupNameTopic();
//! TODO: make this a registeration approach

//! 3.21.22 these are to setup for the next time the main loop() runs to call these commands..
//! The implementation is hard coded in the ESP_IO.ino
#define ASYNC_CALL_OTA_UPDATE 0
//! cleans out the credentials and restarts in AP (Access Point) mode.
#define ASYNC_CALL_CLEAN_CREDENTIALS 1
//!cleans the EPROM totally, and reboots
#define ASYNC_CALL_CLEAN_EPROM 2
//!sends a 'c' to the BLE end of the code (assuming a feeder is connected). Need to morph if a real feeder
#define ASYNC_CALL_FEED_COMMAND 3
//!sends a 'B' to the BLE end of the code (assuming a feeder is connected).
#define ASYNC_CALL_BUZZ_ON 4
//!sends a 'b' to the BLE end of the code (assuming a feeder is connected).
#define ASYNC_CALL_BUZZ_OFF 5
//!sends a message (like #FEED) on the users topic
#define ASYNC_SEND_MQTT_FEED_MESSAGE 6
//!sends the status from the main module URL
#define ASYNC_SEND_MQTT_STATUS_URL_MESSAGE 7

//!sets the GATEWAY mode on
#define ASYNC_SET_GATEWAY_ON 8
//!sets the GATEWAY mode off
#define ASYNC_SET_GATEWAY_OFF 9

//!sets the GATEWAY mode off
#define ASYNC_REBOOT 10
//!sets the GATEWAY mode off
#define ASYNC_POWEROFF 11
//!blank the screen
#define ASYNC_BLANKSCREEN 12

//! swaps WIFI 'w'
#define ASYNC_SWAP_WIFI 13
//! next WIFI 'n'
#define ASYNC_NEXT_WIFI 14
//! restarts the WIFI (after BLE interrupt over)
#define ASYNC_RESTART_WIFI_MQTT 15
//! 5.15.25 add a BUZZ command (or CLICK)
#define ASYNC_CLICK_SOUND 16
//! the max one greater than last one
#define ASYNC_CALL_MAX 17  //don't forget to update this MAX (last one + 1)

//!these are the async with a string parameter. This sends a BLE command unless MQTT
#define ASYNC_CALL_BLE_CLIENT_PARAMETER 0
//!these are the async with a string parameter
#define ASYNC_CALL_OTA_FILE_UPDATE_PARAMETER 1
//!these are the async with a string parameter
#define ASYNC_JSON_MESSAGE_PARAMETER 2
//!these are the async with a string parameter
#define ASYNC_JSON_MQTT_MESSAGE_PARAMETER 3
//!send REST call 
#define ASYNC_REST_CALL_MESSAGE_PARAMETER 4
//! the max one greater than last one
#define ASYNC_CALL_PARAMETERS_MAX 5

//! dispatches a call to the command specified. This is run on the next loop()
void main_dispatchAsyncCommand(int asyncCallCommand);

//!send an async call with a string parameter. This will set store the value and then async call the command (passing the parameter)
//!These are the ASYNC_CALL_PARAMETERS_MAX
void main_dispatchAsyncCommandWithString(int asyncCallCommand, char *parameter);

//!storage for asyncCallCommands
void initAsyncCallFlags();

//!checks if any async commands are in 'dispatch' mode, and if so, invokes them, and sets their flag to false
void invokeAsyncCommands();

//! 5.16.25 Fountainhead, Raining cold weekend
//! start SYNC calls starting with the SYNC_CLICK_SOUND
#define SYNC_CLICK_SOUND 0
#define SYNC_CALL_MAX 1
//! the main sync command (no parameters yet)
void main_dispatchSyncCommand(int syncCallCommand);

//! 3.17.24 get the chip id 
uint32_t getChipId();
//! 3.17.24 get the chip id as a string
char* getChipIdString();

// **** Helper Methods
//!If nil it create one with just the null, so strlen = 0
//!NOTE: the strdup() might be used later..
char* createCopy(char * stringA);
//!a char* version of startsWith (after skipping spaces)
boolean startsWithChar(char *str, char c);
//! if an empty string
boolean isEmptyString(char *stringA);
//!check if the string contains the other string (if substring is "" then no match)
bool containsSubstring(String message, String substring);
//!check if the string matches
bool stringMatch(String message, String substring);
//gets unix time..
int getTimeStamp_mainModule();

//!returns a string in JSON format, such that {'battery':'84'}, {'buzzon':'off'} .. etc
char * main_currentStatusJSON();

//!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
//!if fullStatus, return everything, else just the ATOM stuff
char* main_currentStatusURL(boolean fullStatus);

#ifdef UNUSED
//!sets status parts cia a a string in JSON format, such that {'battery':'84'}, {'buzzon':'off'} .. etc
void main_setStatusJSON(const char* JSONString);
//!sets status parts cia a a string in JSON format, such that {'battery':'84'}, {'buzzon':'off'} .. etc
void main_setStatusKeyValue(const char* key, const char* value);
#endif

//!retrieves the temperature  in F.
float getTemperature_mainModule();

//!start of the sensor updates ... TODO: tie these to the MQTT messaging as well..
float getBatPercentage_mainModule();

#ifdef NOT_USED
//!see the external IP .. try 2.21.22
void getExternalIP();
#endif

//!feedcount info..
int getFeedCount_mainModule();
//increments .. and if MAX goes to 0 -- and sends a message on MQTT
void incrementFeedCount_mainModule();
//! sets the feed count max
void resetFeedCount_mainModule();

//! called by the feed operation to say the device is still running.. and count it as a button click.
//!  Issue #145  8.8.22
void refreshDelayButtonTouched_MainModule();

/*STEPPER feederType*/
/*
#define STEPPER_IS_UNO 1
#define STEPPER_IS_MINI 2
#define STEPPER_IS_TUMBLER 3
 */
//!get the feeder type (Sepper 1,2,3 ...)
int getFeederType_mainModule();

//!returns the max for this feeder
int feedCountMax_mainModule();

//! shows a FEED (or whatever) then blanks the screen after N seconds
//! NOTE: This will be a scrolling text as sometime ..
void showText_mainModule(String text);

//!SemanticMarker events
//!This would see a DOCFollow message, and set the value.. then SM10 can display it..
void setSemanticMarkerDocFollow_mainModule(char* SMDocFollowAddress);
//!get the latest semantic marker for doc follow
char* getSemanticMarkerDocFollow_mainModule();

//!sends the SM on the DOCFOLLOW channel (publish it..)
void sendSemanticMarkerDocFollow_mainModule(const char* SMDocFollowAddress);

//!gets the device name
char *deviceName_mainModule();

//!power of devices
//!reboot
void rebootDevice_mainModule();

//!power off
void poweroff_mainModule();

//!Semantic Marker definitions
//! defines the semantic markers for now..
//! Minimal 0..6 (max = 7)
#define SM_home_simple 0     //tilt
#define SM_home_simple_1 1   //buzz
#define SM_home_simple_2 2   //FEED PAGE
#define SM_home_simple_3 3   //advanced
#define SM_HOME_SIMPLE_LAST SM_home_simple_3
// update: MAX_SM_MIN_MODES when adding one..
// and update the MINI-1 .. passed from MQTT to BUTTON

//! //! homepage
#define SM_smart_clicker_homepage     4
//! //status
#define SM_status     5
//!WIFI ssid
#define SM_WIFI_ssid  6
//!guest page
#define SM_guest_page 7

//!AP MODE
#define SM_ap_mode    8


//!guest feed
#define SM_guest_feed 9
//!guest feed device
#define SM_pair_dev 10
//!HELP
#define SM_help 11

//! docfollow
#define SM_doc_follow 12
//! timer .. todo
#define SM_timer 13
//!REboot
#define SM_reboot     14
#define SM_LAST SM_reboot

//!note for now, heep the max the same -- so only goes through zoom or not zoom
//#define MAX_SM_MIN_MODES (SM_help + 1)
#define MAX_SM_MIN_MODES (SM_HOME_SIMPLE_LAST + 1)
#define MAX_SM_EXPANDED_MODES (SM_LAST + 1)


//! This is needed now as message from external (not button pressing) causes a state change,
//! but the button processing isn't knowing about it...
//!sets the current screen mode .. which can be used by Button and Display processing
//! whichSMMode 0..SM_LAST
void setCurrentSMMode_mainModule(int whichSMMode);
//!returns the current SM Mode
int getCurrentSMMode_mainModule();
//!increment the currentSMMode, wrapping and using the max menu
void incrementSMMode_mainModule();
//!increment the currentSMMode, wrapping and using the max menu
void decrementSMMode_mainModule();

//! returns the current max of the menu modes (using the setting of min or expanded) to determine
int maxMenuModes_mainModule();
//! returns the current max of the MIN menu modes (using the setting of min or expanded) to determine
int minMenuModesMax_mainModule();
//!returns an index from 0..max of SM matching cmd, or -1 if none
int whichSMMode_mainModule(char *cmd);
//!returns if a match the mode. whichSMMode is 0..12 and == sm0 .. smn
boolean matchesSMMode_mainModule(char *cmd, int whichSMMode);
//!returns string form whichSMMode, sg "sm0", sm1 ...
char* charSMMode_mainModule(int whichSMMode);
//!returns which mode in (min or expanded)
boolean isMinimalMenuMode_mainModule();
//!toggles the menu mode 
void toggleMinimalMenuMode_mainModule();

//! BLE Discovery Methods
//! Connected to a BLE device with the advertised name. The syntax can include (PTFeeder:NAME) or just PTFeeder
//! Being disconnected is already a flag isConnectedBLE ...
void setConnectedBLEDevice_mainModule(char *deviceName, boolean isGEN3);
//!whether the connected is a GEN3 (so the name isn't valid).
boolean connectedBLEDeviceIsGEN3_mainModule();
//! returns the connected BLE Device name (the :NAME of advertisment, Address: 01:39:3f:33 part of name, or "" if nothing (not null)
char* connectedBLEDeviceName_mainModule();
//!returns full name and address, and service.
char *getFullBLEDeviceName_mainModule();
//!returns address part of name.
char *connectedBLEDeviceNameAddress_mainModule();

//! 1.22.24 refactored to only ButtonModule knows about plugins..
//! BUTTON PROCESSING abstraction  (NOTE these are only for the M5 since the M5 Atom has it backwards)
//!short press on buttonA (top button)
void buttonA_ShortPress_mainModule();
//!long press on buttonA (top button)
void buttonA_LongPress_mainModule();
//!the long press of the side button
void buttonB_LongPress_mainModule();
//!the short press of the side button
void buttonB_ShortPress_mainModule();
//!restarts all the menu states to the first one .. useful for getting a clean start. This doesn't care if the menu is being shown
void restartAllMenuStates_mainModule();
//! 1.22.24 add setup and loop at main so it can call appropriate plugs
void loop_Sensors_mainModule();
//! setup of buttons and sensors
void setup_Sensors_mainModule();

//!whether the string is TRUE, ON, 1
boolean isTrueString_mainModule(String valCmdString);

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val. 
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified 
void messageSetVal_mainModule(char *setName, char* valValue, boolean deviceNameSpecified);
//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 5.21.25 SEND and CMD will be treated the same and put to "send"
void messageSend_mainModule(char *sendValue, boolean deviceNameSpecified);
//!TODO: have a callback regist approach


//! 1.1.24 send status of this device after events..
void sendStatusMQTT_mainModule();


//! 2.21.25 add a way to change the button color (if any)
void changeButtonColor_MainModule();

//! 3.23.25 rainy weekend
//! create a JSON string from the SemanticMarker
//! https://semanticmarker.org/bot/setdevice/scott@konacurrents.com/PASS/M5AtomSocket/socket/on}
//! Create a JSON knowing the "bot" syntax, eg.  setdevice/USER/PASS/device/<set>/<flag>
//! defined in TokenParser.h
char *semanticMarkerToJSON_mainModule(char* semanticMarker);

//! 5.3.25 add a central clearing house for defining PIN use
//! central clearing house for all pins used to we can analyze if there are overlaps
//! pin is the actual number, pinName is the local name (eg. IN1_PIN or VIN_PIN).
//! moduleName is the module in the code,
//! isI2C is whether this is a I2C bus (which we aren't using much yet)
void registerPinUse_mainModule(long pin, String pinName, String moduleName, boolean isI2C);

//! note these are the sets of pin use
//! 8.18.25 OUCH>.. 10 .. now 20 max
#define PIN_USE_MAX 20
typedef struct  {
    int pinUseCount;
    char *pinUseArray[PIN_USE_MAX];
} PinUseStruct;
//! get the pin use array
PinUseStruct getPinUseStruct_mainModule();

#endif /* MainModule_h */
