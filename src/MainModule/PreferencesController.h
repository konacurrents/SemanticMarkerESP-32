

#ifndef PreferencesController_h
#define PreferencesController_h

/*STEPPER feederType*/
#define STEPPER_IS_UNO 1
#define STEPPER_IS_MINI 2
#define STEPPER_IS_TUMBLER 3
//! These are the "preferenceID" in the calls below..
//!  **** Preferences Setup, called from reading EPROM, and from someone saving a preference ****
#define PREFERENCE_MAIN_GATEWAY_VALUE 0
#define PREFERENCE_MAIN_BLE_SERVER_VALUE 1
#define PREFERENCE_MAIN_BLE_CLIENT_VALUE 2

//! stepper preferences
#define PREFERENCE_STEPPER_BUZZER_VALUE 3
//these 3 are mutually exclutive
#define PREFERENCE_STEPPER_SINGLE_FEED_VALUE 4
#define PREFERENCE_STEPPER_AUTO_FEED_VALUE 5
#define PREFERENCE_STEPPER_JACKPOT_FEED_VALUE 6
#define PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING 7
//! uses STEPPER type
#define PREFERENCE_STEPPER_KIND_VALUE 8
#define PREFERENCE_STEPPER_FEEDS_PER_JACKPOT 9

//! Sensor preferences
#define PREFERENCE_SENSOR_TILT_VALUE 10
#define PREFERENCE_SENSOR_PIR_VALUE 11
//! Display preferences (SemanticMarker etc) - boolean
#define PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE 12
//! Display preferences - show messages on blank screen- boolean
#define PREFERENCE_DISPLAY_ON_BLANK_SCREEN_VALUE 13
//! sets the timeout value
#define PREFERENCE_DISPLAY_SCREEN_TIMEOUT_VALUE 14
//! sets the max temp for a poweroff
#define PREFERENCE_HIGH_TEMP_POWEROFF_VALUE 15
//! sets the max temp for a poweroff
#define PREFERENCE_IS_MINIMAL_MENU_SETTING 16
//! sets  stepper angle, a floating point number
//! sets  screentimeout if not button clicks (set,noclick,val,seconds)
//! NO poweroff with the GATEWAY mode
#define PREFERENCE_NO_BUTTON_CLICK_POWEROFF_SETTING 17

//!for now, save 2 WIFI Credentials
#define WIFI_CREDENTIALS_MAX 2
#define PREFERENCE_WIFI_CREDENTIAL_1_SETTING 18
#define PREFERENCE_WIFI_CREDENTIAL_2_SETTING 19
//!the paired device for guest device feeding (6.6.22)
#define PREFERENCE_PAIRED_DEVICE_SETTING 20
//!the device name itself (6.6.22)
#define PREFERENCE_DEVICE_NAME_SETTING 21
//!a firsttime feature flag (only 1 per build) 7.12.22 defaulting to TRUE
#define PREFERENCE_FIRST_TIME_FEATURE_SETTING 22
//! if true, only BLEClient connect to GEN3 feeders..
#define PREFERENCE_ONLY_GEN3_CONNECT_SETTING 23
//! ithe color of the screen 0..n
#define PREFERENCE_SCREEN_COLOR_SETTING 24

//!if set, the BLE Server (like PTFeeder) will tack on the device name (or none if not defined).
#define PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING 25
//!if set, the BLE Client will look for its service base name (PTFeeder or PTClicker), but if that name
//!also has an extension (:ScoobyDoo), and this is set, it will only connect if the PREFERENCE_PAIRED_DEVICE_SETTING has the same device name in the discovered name
#define PREFERENCE_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING 26
//! for
#define PREFERENCE_USE_DOC_FOLLOW_SETTING 27

//! 8.17.22 to turn on/off subscribing to the dawgpack topic
#define PREFERENCE_SUB_DAWGPACK_SETTING 28
//! 8.22.22 to turn on/off SPIFF use (more below..)
#define PREFERENCE_USE_SPIFF_SETTING 29

//!the paired device for guest device feeding (6.6.22) .. but the Address 9.3.22
#define PREFERENCE_PAIRED_DEVICE_ADDRESS_SETTING 30

//!retreives the motor direction| 1) = default, clockwise; 0 = REVERSE, counterclockwise 9.8.22
//! false = reverse == counterclockwise
//! TRUE == default
//! 8.18.24 note this changes so you can switch directions. the FACTORY_CLOCKWISE
//! below is the original one time setting.
//! Renamed to FACTORY for backward compatible
#define PREFERENCE_STEPPER_FACTORY_CLOCKWISE_MOTOR_DIRECTION_SETTING 31

//! sends the WIFI to all except current device if set
#define PREFERENCE_SENDWIFI_WITH_BLE 32

//! starts the BLE Discovery notification process - which might be internal or externa (via messages).
#define PREFERENCE_NOTIFY_BLE_DISCOVERY 33

//! the preference timer
#define PREFERENCE_TIMER_INT_SETTING 34

//! the preference for supporting GROUPS (*default true)*
#define PREFERENCE_SUPPORT_GROUPS_SETTING 35


//! the preference setting group names to subscribe (but empty or # go to wildcard, this also supports wildcard in the future)
#define PREFERENCE_GROUP_NAMES_SETTING 36

//! a place to put some kind of Last Will of what went wrong .. for now (> max tries)
#define PREFERENCE_DEBUG_INFO_SETTING 37
//! adding AP_DEBUG_MODE to let others know that DEBUG eprom is available. Turn this OFF for non dev
//#define AP_DEBUG_MODE

//!9.28.23 #272   only show Semantic Markers that are sent directly to the device
#define PREFERENCE_DEV_ONLY_SM_SETTING 38

//! the preference timer MAX (pairs with PREFERENCE_TIMER_INT_SETTING)
#define PREFERENCE_TIMER_MAX_INT_SETTING 39

//!1.1.24  the preference for all the ATOM plugs (format:  atomType:value} .. for now just use socket:on
#define PREFERENCE_ATOMS_SETTING 40

//!1.4.24  What kind of ATOM plug (set, M5AtomKind, val= {M5AtomSocket, M5AtomScanner}
#define PREFERENCE_ATOM_KIND_SETTING 41

//! 1.10.24 Flag on whether a Semantic Marker command is sent on PIR, and the Command to send
#define PREFERENCE_SM_ON_PIR_SETTING 42
//! 1.10.24 The  Semantic Marker command is sent on PIR, and the Command to send
#define PREFERENCE_SM_COMMAND_PIR_SETTING 43
//! 1.11.24 The  Semantic Marker command is sent on PIR, and the Command to send on OFF (or opposite)
#define PREFERENCE_SM_COMMAND_PIR_OFF_SETTING 44
//! 1.12.24 Whether the AtomSocket accepts global on/off messages
#define PREFERENCE_ATOM_SOCKET_GLOBAL_ONOFF_SETTING 45

//! 4.4.24 Adding SPIFF for MQTT and QRATOM seperately
//! For MQTT writing to the SPIFF
#define PREFERENCE_USE_SPIFF_MQTT_SETTING 46
//! For MQTT writing to the QRATOM
#define PREFERENCE_USE_SPIFF_QRATOM_SETTING 47

//! 8.2.24 to let older Tumbler NOT do the auto direction (back and forth)
//! Isue #332
//! it will set via message: autoMotorDirection
//! {"set":"autoMotorDirection","val":"true"}
#define PREFERENCE_STEPPER_AUTO_MOTOR_DIRECTION_SETTING 48

//! 8.2.24 don't change subscription but include these groups (eg. safeHouse,atlasDogs)
#define PREFERENCE_INCLUDE_GROUP_NAMES_SETTING 49

//!retreives the motor direction| 1) = default, clockwise; 0 = REVERSE, counterclockwise 9.8.22
//! false = reverse == counterclockwise
//! TRUE == default
//! 8.18.24 this one changes ...
#define PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING 50

//! issue #338 sensor definition (in work)
//! This will be a string in JSON format with various PIN and BUS information
#define PREFERENCE_SENSOR_PLUGS_SETTING 51

//!5.14.25 Dead 5.14.74 Montana
//! issue #365 Object Oriented Sensors as well
//! define the sensors (not sensorPlugs). MQTT:  set:sensor,  set:sensors
#define PREFERENCE_SENSORS_SETTING 52

//!8.14.25 Dead Movie from 10.19.1974 tonight..
//! issue #394 stepperRPM
//! stepper RPM
#define PREFERENCE_STEPPER_RPM_SETTING 53

//! 9.3.25 back from LA, Horses out. Tyler on lap. Europe next week
//! sets the 2feed option (go back and forth)
#define PREFERENCE_STEPPER_2FEED_SETTING 54

#pragma mark MAX_PREFERENCE Set here
//! *******  1 greater than last value **** IMPORTANT *** and no gaps..
#define MAX_MAIN_PREFERENCES 55

//! 8.2.24 retrieve the includeGroup
//! really ask a topic if it's in the include group
boolean topicInIncludeGroup(char *topic);
//! 8.2.24 set the include group (and cache it), called from MQTT
void setIncludeGroups(char *groups);

//!initialize the _preferencesMainLookup with EPROM lookup names
void initPreferencesMainModule();

//!print the preferences to SerialDebug
void printPreferenceValues_mainModule();

//! Preferences for the MainModule .. basically generic settings. So any settings you want, add them here.
//!  NOTE: These are just "state" values, no methods or events are generated on saving these values.. It's up to the caller to perform events
//!  such as turning on the buzzer..

//! called to init the preference. This won't save anything until storePreference called
void initAppendingPreference_mainModule(int preferenceID);
//! called to append to a a preference (which will be an identifier and a string, which can be converted to a number or boolean)
void appendPreference_mainModule(int preferenceID, String preferenceValue);
//! called to append to a a preference (which will be an identifier and a string, which can be converted to a number or boolean)
void storePreference_mainModule(int preferenceID, String preferenceValue);

//! called to reset to blank a preference (which will be an identifier and a string, which can be converted to a number or boolean)
void resetPreference_mainModule(int preferenceID);



//! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
void savePreference_mainModule(int preferenceID, String preferenceValue);

//! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
boolean getPreferenceBoolean_mainModule(int preferenceID);

//!TODO: put them in somewhere that is saving EPROM .. a SettingsModule
//! these are implemented in the MainModule (since they are writing to the EPROM)
//!sets and gets the "gateway" functionality
void savePreferenceBoolean_mainModule(int preferenceID, boolean flag);

//!toggles a preference boolean
void togglePreferenceBoolean_mainModule(int preferenceID);

//! called to get a preference (which will be an identifier and a string, which can be converted to a number or boolean)
//! NOTE: this reuses same string buffer so will be overwritten on next call. Save value if you need more than one result (such as WIFI credentials, ouch)
//! This mean any getPreference will use the same string .. so unless the result is a number or boolean, then copy it locally..
char* getPreference_mainModule(int preferenceID);

//!returns the preference but in it's own string buffer. As long as you use it before calling getPreferenceString again, it won't be overwritten
char* getPreferenceString_mainModule(int preferenceID);

//! called to get a preference (which will be an identifier and a string, which can be converted to a number or boolean)
int getPreferenceInt_mainModule(int preferenceID);

//!sets an int
void savePreferenceInt_mainModule(int preferenceID, int val);

//!sets an int, but only if a valid integer, and no signs
void savePreferenceIntFromString_mainModule(int preferenceID, char* val);

//! called to get a preference (which will be an identifier and a string, which can be converted to a number or boolean)
float getPreferenceFloat_mainModule(int preferenceID);

//! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
void savePreferenceFloat_mainModule(int preferenceID, float val);

//!set some defaults on boot - that override EPROM this can be called on the HOME screen to set back to normal mode..
void setOnBootPreferences_mainModule();

//!returns if the paired device is not NONE
boolean isValidPairedDevice_mainModule();

//!returns if the paired device is not NONE
char * getPairedDevice_mainModule();

//!returns if the paired device is not NONE
char * getPairedDeviceAddress_mainModule();


//!returns if the paired device is not NONE .. returns address or device
char * getPairedDeviceOrAddress_mainModule();

//!resets preferences.. Currently only reset all, but eventually reset(groups..)
void resetAllPreferences_mainModule();

//!transient for now...  10.4.22
//! set the M5 PTClicker discovery option..
void setDiscoverM5PTClicker(boolean flag);
//! get option
boolean getDiscoverM5PTClicker();

#pragma mark ATOM Processing

//! new 1.4.24 setting ATOM kind (eg. M5AtomSocket, M5AtomScanner). MQTT message "set":"M5AtomKind", val=
void savePreferenceATOMKind_MainModule(String value);
//! new 1.4.24 setting ATOM kind (eg. M5AtomSocket, M5AtomScanner)
char *getPreferenceATOMKind_MainModule();

//!returned from mainModule
#define ATOM_KIND_M5_SCANNER 0
#define ATOM_KIND_M5_SOCKET 1
//! new 1.4.24 setting ATOM kind (eg. ATOM_KIND_M5_SCANNER, ATOM_KIND_M5_SOCKET)
int getM5ATOMKind_MainModule();



#define NEW_SENSORS_PREFERENCE
//! store the SensorClassType instance as well
#include "../SensorClass/SensorClassType.h"
//!issue #365 sensor plugs and more
//! 5.14.25 Hanging with Tyler,
//! Dead Montana 5.14.74 great stuff
//! add the Sensors Preference .. first the parsing

//!forward definition
class SensorClassType;

//! sensor struct
//! defines the structure
typedef struct sensorStruct
{
    char *sensorName;
    int pin1;
    int pin2;
    //! and the pointer to matching SensorClassType
    SensorClassType* sensorClassType;
} SensorStruct;

//! wrap sensors
typedef struct sensorsStruct
{
    int count;
    //! array of sensorStruct
    SensorStruct *sensors;
} SensorsStruct;

//! return the sensor specified or null
SensorsStruct* getSensors_mainModule();

//! return the sensor specified or null
SensorStruct* getSensor_mainModule(char *sensorName);

//! Only 1 setSensorsString now .. will always append
//! message:  set:sensors, val:<val> or empty to clean it out
//! unless a null or blank "" string
//! set a sensor val (array of  sensor,pin,pin,sensor,pin,pin...)
//! After each set, the SensorsStruct will be updated
void setSensorsString_mainModule(char *sensorsString);

//!  init the sensorString from EPROM
//!PREFERENCE_SENSOR_PLUGS_SETTING
void initSensorStringsFromEPROM_mainModule();

//! print sensors, passing in a struct
void printSensors_mainModule(SensorsStruct* sensors);

//! 7.9.25 reset SENSORS to default
//! "BuzzerSensorClass,23,33,L9110S_DCStepperClass,21,25"
void resetSensorToDefault_mainModule();

#endif // PreferensesController_h
