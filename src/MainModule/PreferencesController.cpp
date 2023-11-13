
#include "../../Defines.h"
#include "PreferencesController.h"
//! PREFERENCES for the main module
//1075325  vs 1074301
//1075781  after refactor (removed stpper preferences)
//1075833

//!for returning a 'String"  NOTE: THIS IS NOT GREAT .. as if you don't grab it before another query, and use it -- the result might be 1 instead of deviceName..
char _preferenceBuffer[100];
//!buffer for the string
char _preferenceBufferString[100];


//!Issue #103
//!NOTE: the EPROM space might be limiting: https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/default.csv
//!to 4KB .. https://www.esp32.com/viewtopic.php?t=9136
//! So we should start limiting our use, and start with these string indexes..
//! main module preferences
//! gateway is on  THESE MUST ALL BE UNIQUE
#define EPROM_MAIN_GATEWAY_VALUE "1gt"
//!BLEServer mode
#define EPROM_MAIN_BLE_SERVER_VALUE "2bs"
//!BLEClient mode
#define EPROM_MAIN_BLE_CLIENT_VALUE "3bc"

//!buzzer on or off
#define EPROM_STEPPER_BUZZER_VALUE "4bz"
//! single feed mode
#define EPROM_STEPPER_SINGLE_FEED_VALUE "5sf"
//!auto feed
#define EPROM_STEPPER_AUTO_FEED_VALUE "6af"
//!jackpot feed
#define EPROM_STEPPER_JACKPOT_FEED_VALUE "7jf"
//! the step kind
#define EPROM_STEPPER_KIND_VALUE "8sk"
//num feeds in jackpot mode
#define EPROM_STEPPER_FEEDS_PER_JACKPOT_VALUE "9jp"
//! sensor preferences for tilt on or off
#define EPROM_SENSOR_TILT_VALUE "TILT_10" //10tt"
                                          //!proximity PIR
#define EPROM_SENSOR_PIR_VALUE "11pr"
//! display preferences zoomed or not zoomed
#define EPROM_SEMANTIC_MARKER_ZOOMED_VALUE "12sz"

//! Display preferences - show messages on blank screen- boolean
#define EPROM_DISPLAY_ON_BLANK_SCREEN_VALUE "13bk"
//! sets the timeout value
#define EPROM_DISPLAY_SCREEN_TIMEOUT_VALUE "14to"
//! sets the max temp for a poweroff
#define EPROM_HIGH_TEMP_POWEROFF_VALUE "15pw"
//!whether to show the minimal or expanded menu
#define EPROM_IS_MINIMAL_MENU_SETTING "16mm"
//! sets  stepper angle, a floating point number
#define EPROM_STEPPER_ANGLE_FLOAT_SETTING "17sa"

//! sets  screentimeout if not button clicks (set,noclick,val,seconds)
#define EPROM_NO_BUTTON_CLICK_POWEROFF_SETTING "18po"

//!for now, save 2 WIFI Credentials
#define EPROM_WIFI_CREDENTIAL_1_SETTING "19w1"
//!second wifi credential
#define EPROM_WIFI_CREDENTIAL_2_SETTING "20w2"

//! the guest device paired with this M5 device (or NONE)
#define EPROM_PAIRED_DEVICE_SETTING "21pd"

//! the  device name
#define EPROM_DEVICE_NAME_SETTING "22dn"

//!a firsttime feature flag (only 1 per build) 7.12.22 defaulting to TRUE
#define EPROM_FIRST_TIME_FEATURE_SETTING "23fs"

//! if true, only BLEClient connect to GEN3 feeders..
#define EPROM_ONLY_GEN3_CONNECT_SETTING  "23gs"

//!color of the M5 screen
#define EPROM_SCREEN_COLOR_SETTING "24sc"

//!if set, the BLE Server (like PTFeeder) will tack on the device name (or none if not defined).
#define EPROM_BLE_SERVER_USE_DEVICE_NAME_SETTING "25bs"
//!if set, the BLE Client will look for its service base name (PTFeeder or PTClicker), but if that name
//!also has an extension (:ScoobyDoo), and this is set, it will only connect if the PREFERENCE_PAIRED_DEVICE_SETTING has the same device name in the discovered name
#define EPROM_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING "26bd"
//!NOTE: these EPROM have to be lockstep the same as the PREFERENCE definitions in MainModule.h

#define EPROM_USE_DOC_FOLLOW_SETTING "27df"

//! 8.17.22 to turn on/off subscribing to the dawgpack topic
#define EPROM_SUB_DAWGPACK_SETTING "28dg"

//! 8.22.22 to turn on/off SPIFF use
#define EPROM_USE_SPIFF_SETTING "29sf"

//! 9.3.22 eprom of the Address of desired BLE
#define EPROM_PAIRED_DEVICE_ADDRESS_SETTING "30da"

//!retreives the motor direction| true default, clockwise; false = REVERSE, counterclockwise 9.8.22
//! false = reverse == counterclockwise
//! true = default
#define EPROM_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING "31md"

//!send WIFI to all except our device (and our paired) when
#define EPROM_SENDWIFI_WITH_BLE "32wb"

//! starts the BLE Discovery notification process - which might be internal or externa (via messages).
#define EPROM_NOTIFY_BLE_DISCOVERY "33bd"

//! the preference timer
#define EPROM_PREFERENCE_TIMER_INT_SETTING "34tm"

//! the preference for supporting GROUPS (*default true)*
#define EPROM_PREFERENCE_SUPPORT_GROUPS_SETTING "35g"

//! the preference setting group names to subscribe (but empty or # go to wildcard, this also supports wildcard in the future)
#define EPROM_PREFERENCE_GROUP_NAMES_SETTING "36g"

//! a place to put some kind of Last Will of what went wrong .. for now (> max tries)
#define EPROM_PREFERENCE_DEBUG_INFO_SETTING "37db"


//!9.28.23 #272   only show Semantic Markers that are sent directly to the device
#define EPROM_DEV_ONLY_SM_SETTING "38dsm"

//!the EPROM is in preferences.h
#include <Preferences.h>
//!name of main prefs eprom
#define PREFERENCES_EPROM_MAIN_NAME "MainPrefs"

//! preferences for MAIN
Preferences _preferencesMainModule;
//!array of preference names (those used in process()
char *_preferenceMainModuleLookupEPROMNames[MAX_MAIN_PREFERENCES];
//!array of default values to store in the EPROM if not defined..
char *_preferenceMainModuleLookupDefaults[MAX_MAIN_PREFERENCES];
//!initialize the _preferencesMainLookup with EPROM lookup names
void initPreferencesMainModule();

//!Cache for heavy hitter boolean values (those referenced every loop.. seems to be hard on the EPROM to keep up:
//! This array will only have the Boolean values set.. the 'save' will fill them, and the 'get' will return
boolean _cachedPreferenceBooleanValues[MAX_MAIN_PREFERENCES];

//!array of boolean if the ID is cached..
boolean _isCachedPreferenceBoolean[MAX_MAIN_PREFERENCES];

//! another cache for the Int values..
int _cachedPreferenceIntValues[MAX_MAIN_PREFERENCES];

//!array of boolean if the ID is cached..
boolean _isCachedPreferenceInt[MAX_MAIN_PREFERENCES];

//! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
void savePreference_mainModule(int preferenceID, String preferenceValue)
{
    if (preferenceID != PREFERENCE_DEBUG_INFO_SETTING)
        SerialLots.printf("savePreference .. %d, %s\n", preferenceID, preferenceValue.c_str());
    // cannot invoke the preference, as this would be an infinite loop back to here..
    
    //save in EPROM
    _preferencesMainModule.begin(PREFERENCES_EPROM_MAIN_NAME, false);  //readwrite..
    _preferencesMainModule.putString(_preferenceMainModuleLookupEPROMNames[preferenceID], preferenceValue);
    
    // Close the Preferences
    _preferencesMainModule.end();

}

//! special preference string for saving and printing back later..
#define MAX_APPEND 1500
//!storage for the appending string
String _appendingPreferenceString = "";
//! called to init the preference. This won't save anything until storePreference called
void readAppendingPreference_mainModule(int preferenceID)
{
    _appendingPreferenceString = String(getPreference_mainModule(preferenceID));
}

//! called to init the preference. This won't save anything until storePreference called
void initAppendingPreference_mainModule(int preferenceID)
{
    _appendingPreferenceString = "";
}
//! called to append to a a preference (which will be an identifier and a string, which can be converted to a number or boolean)
void appendPreference_mainModule(int preferenceID, String preferenceValue)
{
    if (_appendingPreferenceString.length() > MAX_APPEND)
    {
        SerialDebug.println(" *** preference > max, emptying .. ");
        _appendingPreferenceString = "";
    }
    _appendingPreferenceString += "\n" + preferenceValue;
}
//! called to append to a a preference (which will be an identifier and a string, which can be converted to a number or boolean)
void storePreference_mainModule(int preferenceID, String preferenceValue)
{
    appendPreference_mainModule(preferenceID, preferenceValue);
   // SerialDebug.printf("storePref(%d): %s\n", _appendingPreferenceString.length(), _appendingPreferenceString.c_str());
    savePreference_mainModule(preferenceID, _appendingPreferenceString);
}

//! called to reset to blank a preference (which will be an identifier and a string, which can be converted to a number or boolean)
void resetPreference_mainModule(int preferenceID)
{
#ifdef DO_ERASE
    savePreference_mainModule(preferenceID,"");
    initAppendingPreference_mainModule(preferenceID);
#endif
}


//!sets an int preference
void savePreferenceInt_mainModule(int preferenceID, int val)
{
    if (_isCachedPreferenceInt[preferenceID])
    {
        //! CACHE SETTINGS 1
        _cachedPreferenceIntValues[preferenceID] = val;
    }
    
    //!convert to a string..
    char str[20];
    sprintf(str,"%d",val);
    savePreference_mainModule(preferenceID, str);
}

//!sets an int, but only if a valid integer, and no signs. If bad, then a 0 is stored
void savePreferenceIntFromString_mainModule(int preferenceID, char* val)
{
    //This function returns the converted integral number as an int value. If no valid conversion could be performed, it returns zero.
    int num = atoi(val);
    savePreferenceInt_mainModule(preferenceID, num);
}

//!toggles a preference boolean
void togglePreferenceBoolean_mainModule(int preferenceID)
{
    boolean val = getPreferenceBoolean_mainModule(preferenceID);
    val = !val;
    savePreferenceBoolean_mainModule(preferenceID, val);
}



//! called to get a preference (which will be an identifier and a string, which can be converted to a number or boolean)
//! Note: no CACHE is looked at here. It's up to the Boolean or Int to do that..
char* getPreference_mainModule(int preferenceID)
{
    // cannot invoke the preference, as this would be an infinite loop back to here..
    
    //!get from EPROM
    _preferencesMainModule.begin(PREFERENCES_EPROM_MAIN_NAME, true);  //read
    
    strcpy(_preferenceBuffer, _preferencesMainModule.getString( _preferenceMainModuleLookupEPROMNames[preferenceID]).c_str());
#ifdef TOOMUCH
    SerialLots.printf("getPreference_mainModule[%d] = %s\n", preferenceID, _preferenceBuffer);
#endif
    // Close the Preferences
    _preferencesMainModule.end();
    return _preferenceBuffer;
}


//! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
boolean getPreferenceBoolean_mainModule(int preferenceID)
{
    boolean valBool;
    // check some of the boolean ones
    if (_isCachedPreferenceBoolean[preferenceID])
    {
        //! CACHE SETTINGS 5
        valBool = _cachedPreferenceBooleanValues[preferenceID];
#ifdef TOOMUCH
        SerialLots.print(" CACHE preference = ");
        SerialLots.printf(" [%d] = ",preferenceID);
        SerialLots.println(valBool);
#endif
    }
    else
    {
        char* val =  getPreference_mainModule(preferenceID);
#ifdef TOOMUCH
        SerialLots.print(" preference = ");
        SerialLots.printf(" [%d] = ",preferenceID);
        SerialLots.println(val);
#endif
        valBool = (strcmp(val,"1")==0)?true:false;
    }
    return valBool;
}

//! save a boolean preference
void savePreferenceBoolean_mainModule(int preferenceID, boolean flag)
{
    if (_isCachedPreferenceBoolean[preferenceID])
    {
        //! CACHE SETTINGS 5
        _cachedPreferenceBooleanValues[preferenceID] = flag;
        
        SerialLots.print(" set CACHE preference");
        SerialLots.printf(" [%d] = ",preferenceID);
        SerialLots.println(flag);
    }
    
    savePreference_mainModule(preferenceID, flag?(char*)"1":(char*)"0");
}



//!returns the preference but in it's own string buffer. As long as you use it before calling getPreferenceString again, it won't be overwritten
char* getPreferenceString_mainModule(int preferenceID)
{
    strcpy(_preferenceBufferString, getPreference_mainModule(preferenceID));
    return _preferenceBufferString;
}
//! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
int getPreferenceInt_mainModule(int preferenceID)
{
    int ival = 0;
    if (_isCachedPreferenceInt[preferenceID])
    {
        ival = _cachedPreferenceIntValues[preferenceID];
    }
    else
    {
        ival = atoi(getPreference_mainModule(preferenceID));
    }
    
    return ival;
}

//! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
float getPreferenceFloat_mainModule(int preferenceID)
{
    char* val = getPreference_mainModule(preferenceID);
    float fval = atof(val);
    return fval;
}
//! clean the preferencesMainModule in EPROM
void cleanEPROM_mainModule()
{
    _preferencesMainModule.begin(PREFERENCES_EPROM_MAIN_NAME, false);  //readwrite..
    _preferencesMainModule.clear();
    _preferencesMainModule.end();
}

//!set some defaults on boot - that override EPROM
//!This is also called when going back tot he MAIN menu (HOME SCREEN)
void setOnBootPreferences_mainModule()
{
    SerialTemp.println("setOnBootPreferences_mainModule");
    //!set zoomed = true (no semantic marker)
    savePreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE, true);
    
    //!use the minimal menu on boot
    savePreferenceBoolean_mainModule(PREFERENCE_IS_MINIMAL_MENU_SETTING, true);
    
    
}

//! reads the preferences. Save is everytime the savePreference is called
void readPreferences_mainModule()
{
    SerialDebug.printf("readPreferences_mainModule(%s)\n",PREFERENCES_EPROM_MAIN_NAME);
    
    //!clean the cached, and initialize what are cached..
    for (int i = 0; i < MAX_MAIN_PREFERENCES; i++)
    {
        _cachedPreferenceBooleanValues[i] = false;
        _cachedPreferenceIntValues[i] = 0;
        _isCachedPreferenceBoolean[i] = false;
        _isCachedPreferenceInt[i] = false;
    }
    
    //!intiialize the preferences arrays from EPROM. This also updates the defaults
    //! BUT: the caches below are from the value retrieved (and defult if need be)
    initPreferencesMainModule();
    
    //!start the read-write of the EPROM
    _preferencesMainModule.begin(PREFERENCES_EPROM_MAIN_NAME, false);  //readwrite..
    
    for (int i = 0; i < MAX_MAIN_PREFERENCES; i++)
    {
        String preferenceValue;
        int preferenceID = i;
        preferenceValue = _preferencesMainModule.getString(_preferenceMainModuleLookupEPROMNames[preferenceID]);
        //SerialLots.printf("preverenceValue[%s] = %s\n", _preferenceMainModuleLookupEPROMNames[preferenceID], preferenceValue);
        if (preferenceValue && preferenceValue.length() > 0)
        {
            // already set
        }
        else
        {
            //otherwise go to the preference defaults
            preferenceValue = _preferenceMainModuleLookupDefaults[i];
            _preferencesMainModule.putString(_preferenceMainModuleLookupEPROMNames[preferenceID], preferenceValue);
        }
        SerialLots.printf("Preference[%s] = ",_preferenceMainModuleLookupEPROMNames[preferenceID]);
        SerialLots.println(preferenceValue);
        
        //! check some of the boolean ones to cache .. so don't have to go to the EPROM everytime..
        switch (i)
        {
                //!**** NOTE: THis is where whether things are cached or not is set! Eventually all boolean and int could be cached..
                //! CACHE SETTINGS (boolean)
            case PREFERENCE_SUPPORT_GROUPS_SETTING:
            case PREFERENCE_SENDWIFI_WITH_BLE:
            case PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING:
            case PREFERENCE_IS_MINIMAL_MENU_SETTING:
            case PREFERENCE_ONLY_GEN3_CONNECT_SETTING:
            case PREFERENCE_MAIN_BLE_SERVER_VALUE:
            case PREFERENCE_MAIN_BLE_CLIENT_VALUE:
            case PREFERENCE_SENSOR_TILT_VALUE:
            case PREFERENCE_USE_DOC_FOLLOW_SETTING:
            case PREFERENCE_DEV_ONLY_SM_SETTING:

                //SerialLots.printf("setting Cached[%d] = %s\n", i, preferenceValue);
                _isCachedPreferenceBoolean[i] = true;
                _cachedPreferenceBooleanValues[i] = (preferenceValue.compareTo("1")==0)?true:false;
                break;
                //! CACHE SETTINGS (int)
            case PREFERENCE_SCREEN_COLOR_SETTING:
            case PREFERENCE_STEPPER_KIND_VALUE:
            case PREFERENCE_TIMER_INT_SETTING:
                _isCachedPreferenceInt[i] = true;
                _cachedPreferenceIntValues[i] = atoi(&preferenceValue[0]);
                break;
                
            default:
                break;
        }
    }
    //! Close the Preferences
    _preferencesMainModule.end();
    
    //! set onbootPreferences
    setOnBootPreferences_mainModule();
    
}

//!initialize the _preferencesMainLookup with EPROM lookup names
//!BUT these are not stored in EPROM. The next method 
void initPreferencesMainModule()
{
    
    strcpy(_preferenceBufferString,(char*)"");
    strcpy(_preferenceBuffer,(char*)"");
    
    for (int i = 0; i < MAX_MAIN_PREFERENCES; i++)
    {
        switch (i)
        {
            case PREFERENCE_MAIN_BLE_SERVER_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_MAIN_BLE_SERVER_VALUE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#ifdef ESP_M5
#ifdef   ESP_M5_CAMERA
                //!default on for the M5 Camera (but it can be turned on later..)
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#else
#ifdef    ATOM_QRCODE_MODULE
                //! 12.8.22  default back TRUE
                //!default off for the M5 (but it can be turned on later..)
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#else
                //! 12.8.22  default back TRUE
                //!default off for the M5 (but it can be turned on later..)
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
#endif //ATOM
#endif //ESP_M5_CAMERA
#else
                //! 12.8.22  default back TRUE
                //!default off for the M5 (but it can be turned on later..)
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#endif //ESP_M5
                SerialTemp.printf(" **** setting PREFERENCE_MAIN_BLE_SERVER_VALUE[%d]: %s\n", i,_preferenceMainModuleLookupDefaults[i]);
                break;
            case PREFERENCE_MAIN_BLE_CLIENT_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_MAIN_BLE_CLIENT_VALUE;
#ifdef ESP_M5
#ifdef ESP_M5_ATOM_LITE_QR_SCANNER_CONFIGURATION
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";

#else
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#endif // ESP_M5_ATOM_LITE_QR_SCANNER_CONFIGURATION
                
#else
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
#endif
                break;
                //! Sensor preferences
            case PREFERENCE_SENSOR_TILT_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_SENSOR_TILT_VALUE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
                break;
            case PREFERENCE_IS_MINIMAL_MENU_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*) EPROM_IS_MINIMAL_MENU_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
                break;
                //!used for first time features..
            case PREFERENCE_ONLY_GEN3_CONNECT_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*) EPROM_ONLY_GEN3_CONNECT_SETTING;
                //! This is a feeder that gateways to a GEN3
#ifdef ESP_32_FEEDER_BLE_GEN3
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#else
                //! This is a feeder that gateways to a GEN3
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
#endif
                break;
                
                //INT SETTINGS..
            case PREFERENCE_SCREEN_COLOR_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*) EPROM_SCREEN_COLOR_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
                break;
                
                
                //! REST ARE NOT CACHED
                //These are hard coded default values for the preferences
            case PREFERENCE_MAIN_GATEWAY_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_MAIN_GATEWAY_VALUE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
                break;
                
                //! STEPPER preferences  (no ifdef.. )
            case PREFERENCE_STEPPER_SINGLE_FEED_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_STEPPER_SINGLE_FEED_VALUE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
                break;
            case PREFERENCE_STEPPER_AUTO_FEED_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_STEPPER_AUTO_FEED_VALUE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
                break;
            case PREFERENCE_STEPPER_JACKPOT_FEED_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_STEPPER_JACKPOT_FEED_VALUE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
                break;
            case PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING:
                /*
                 per issue @269, stepper angle default = 45 (which it has been for awhile)
                 */
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*) EPROM_STEPPER_ANGLE_FLOAT_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"45";
                break;
            case PREFERENCE_TIMER_INT_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_PREFERENCE_TIMER_INT_SETTING;
                //! 3.28.23 change default to 5 (from 30)
                _preferenceMainModuleLookupDefaults[i] = (char*)"5";
                break;
            case PREFERENCE_STEPPER_KIND_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_STEPPER_KIND_VALUE;
                /*
#define STEPPER_IS_UNO 1
#define STEPPER_IS_MINI 2
#define STEPPER_IS_TUMBLER 3
                 per issue @269, default is now Tumbler
                 */
                _preferenceMainModuleLookupDefaults[i] = (char*)"3";
                break;
            case PREFERENCE_STEPPER_BUZZER_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_STEPPER_BUZZER_VALUE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
                break;
            case PREFERENCE_STEPPER_FEEDS_PER_JACKPOT:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_STEPPER_FEEDS_PER_JACKPOT_VALUE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"3";
                break;
                
            case PREFERENCE_SENSOR_PIR_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_SENSOR_PIR_VALUE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
                break;
            case PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_SEMANTIC_MARKER_ZOOMED_VALUE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
                break;
            case PREFERENCE_DISPLAY_ON_BLANK_SCREEN_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_DISPLAY_ON_BLANK_SCREEN_VALUE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
                break;
            case PREFERENCE_DISPLAY_SCREEN_TIMEOUT_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char* )EPROM_DISPLAY_SCREEN_TIMEOUT_VALUE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"480";
                break;
            case PREFERENCE_HIGH_TEMP_POWEROFF_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_HIGH_TEMP_POWEROFF_VALUE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"80";
                break;
            case PREFERENCE_NO_BUTTON_CLICK_POWEROFF_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*) EPROM_NO_BUTTON_CLICK_POWEROFF_SETTING;
                _preferenceMainModuleLookupDefaults[i] = NO_POWEROFF_AMOUNT_STRING_MAIN; // 10000 == no poweroff
                break;
            case PREFERENCE_WIFI_CREDENTIAL_1_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*) EPROM_WIFI_CREDENTIAL_1_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"NONE";
                break;
            case PREFERENCE_WIFI_CREDENTIAL_2_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*) EPROM_WIFI_CREDENTIAL_2_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"NONE";
                break;
            case PREFERENCE_PAIRED_DEVICE_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*) EPROM_PAIRED_DEVICE_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"NONE";
                break;
            case PREFERENCE_DEVICE_NAME_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*) EPROM_DEVICE_NAME_SETTING;
#ifdef ESP_M5
#ifdef ESP_M5_CAMERA
                _preferenceMainModuleLookupDefaults[i] = (char*)"M5Camera";
#else
#ifdef ESP_M5_ATOM_LITE_QR_SCANNER_CONFIGURATION
                _preferenceMainModuleLookupDefaults[i] = (char*)"M5Atom";
#else
                _preferenceMainModuleLookupDefaults[i] = (char*)"M5";
#endif //ESP_m5
#endif //ESP_M5_Camera
#else
                _preferenceMainModuleLookupDefaults[i] = (char*)"ESP32";
#endif
                break;
                
                //!set with message: set:bleusedevicename,val:on/off
            case PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_BLE_SERVER_USE_DEVICE_NAME_SETTING;
                //! 12.8.22 setting to TRUE as default
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#ifdef ESP_M5
                //! 1.6.23 .. PetTutor Blue app still not always discovering new syntax
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#else
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
#endif
                break;
                //!set with message: set:bleusepaireddevicename,val:on/off
            case PREFERENCE_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
                break;
                
            case PREFERENCE_USE_DOC_FOLLOW_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_USE_DOC_FOLLOW_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
                break;
                //!used for first time features..
            case PREFERENCE_FIRST_TIME_FEATURE_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_FIRST_TIME_FEATURE_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
                break;
                
                //! 8.17.22 to turn on/off subscribing to the dawgpack topic
            case PREFERENCE_SUB_DAWGPACK_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_SUB_DAWGPACK_SETTING;
#ifdef ESP_M5
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#else
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
#endif
                break;
                
                //!//! 8.22.22 to turn on/off SPIFF use  (not cached yet as it's an infrequent event)
            case PREFERENCE_USE_SPIFF_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_USE_SPIFF_SETTING;
#ifdef ESP_M5_CAMERA
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#else
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
                
#endif
                break;
                
                //!the paired device for guest device feeding (6.6.22) .. but the Address 9.3.22
            case PREFERENCE_PAIRED_DEVICE_ADDRESS_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_PAIRED_DEVICE_ADDRESS_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"";

                break;
                
                ///!retreives the motor direction| 0 (false) = default, clockwise; 1 (true) = REVERSE, counterclockwise 9.8.22
                //! TRUE = reverse == counterclockwise
                //! FALSE = default
            case PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
                
                break;
                
                //! 10.4.22
            case PREFERENCE_SENDWIFI_WITH_BLE:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_SENDWIFI_WITH_BLE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
                
                break;
                
                //! 11.1.22  TODO.. messages for this..
            case PREFERENCE_NOTIFY_BLE_DISCOVERY:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_NOTIFY_BLE_DISCOVERY;
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
                break;
                
                //! 7.26.23 added group message support (or turn it off)
                //! PREFERENCE_SUPPORT_GROUPS_SETTING
            case PREFERENCE_SUPPORT_GROUPS_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_PREFERENCE_SUPPORT_GROUPS_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
                break;
                
                //! 7.26.23 added group message support (or turn it off)
                //! PREFERENCE_GROUP_NAMES_SETTING
                //! the preference setting group names to subscribe (but empty or # go to wildcard, this also supports wildcard in the future)
            case PREFERENCE_GROUP_NAMES_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_PREFERENCE_GROUP_NAMES_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"#";
                break;
                
                //! a place to put some kind of Last Will of what went wrong .. for now (> max tries)
                //! 9.16.23
            case PREFERENCE_DEBUG_INFO_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_PREFERENCE_DEBUG_INFO_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"none";
                break;
                
                //! 9.28.23 #272   only show Semantic Markers that are sent directly to the device, default OFF
            case PREFERENCE_DEV_ONLY_SM_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_DEV_ONLY_SM_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
                break;
                                
            default:
                SerialError.printf(" ** NO default for preference[%d]\n", i);
        }
    }
}


//!print the preferences to SerialDebug
void printPreferenceValues_mainModule()
{
    //!this inits the string to the EPROM value
    readAppendingPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING);
    
#ifdef SERIAL_DEBUG_TEMP
    SerialTemp.println("******************");
    SerialTemp.println(VERSION);
    //ouch.. this sets 2 values ..
    //  readPreferences_mainModule();
    SerialTemp.printf("STEPPER_KIND: %d  1=UNO,2=MINI,3=TUMBLER\n", getPreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE));
    SerialTemp.printf("PREFERENCE_TIMER_INT_SETTING: %d\n", getPreferenceInt_mainModule(PREFERENCE_TIMER_INT_SETTING));
    SerialTemp.printf("STEPPER_AUTO_FEED: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_AUTO_FEED_VALUE));
    SerialTemp.printf("STEPPER_JACKPOT_FEED: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_JACKPOT_FEED_VALUE));
    SerialTemp.printf("PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING: %f\n", getPreferenceFloat_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING));
    SerialTemp.printf("STEPPER_FEEDS_PER_JACKPOT: %d\n", getPreferenceInt_mainModule(PREFERENCE_STEPPER_FEEDS_PER_JACKPOT));
    SerialTemp.printf("DISPLAY_SCREEN_TIMEOUT: %d\n", getPreferenceInt_mainModule(PREFERENCE_DISPLAY_SCREEN_TIMEOUT_VALUE));
    SerialTemp.printf("PREFERENCE_HIGH_TEMP_POWEROFF_VALUE: %d\n", getPreferenceInt_mainModule(PREFERENCE_HIGH_TEMP_POWEROFF_VALUE));
    SerialTemp.printf("PREFERENCE_IS_MINIMAL_MENU_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_IS_MINIMAL_MENU_SETTING));
    SerialTemp.printf("PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE));
    SerialTemp.printf("PREFERENCE_NO_BUTTON_CLICK_POWEROFF_SETTING: %d\n", getPreferenceInt_mainModule(PREFERENCE_NO_BUTTON_CLICK_POWEROFF_SETTING));
    SerialTemp.printf("PREFERENCE_MAIN_GATEWAY_VALUE: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE));
    SerialTemp.printf("PREFERENCE_SENSOR_TILT_VALUE: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE));
    SerialTemp.printf("WIFI_CREDENTIAL_1: %s\n", getPreference_mainModule(PREFERENCE_WIFI_CREDENTIAL_1_SETTING));
    SerialTemp.printf("WIFI_CREDENTIAL_2: %s\n", getPreference_mainModule(PREFERENCE_WIFI_CREDENTIAL_2_SETTING));
    SerialTemp.printf("PREFERENCE_PAIRED_DEVICE_SETTING: %s\n", getPreference_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING));
    SerialTemp.printf("PREFERENCE_PAIRED_DEVICE_ADDRESS_SETTING: %s\n", getPreference_mainModule(PREFERENCE_PAIRED_DEVICE_ADDRESS_SETTING));
    SerialTemp.printf("PREFERENCE_DEVICE_NAME_SETTING: %s\n", getPreference_mainModule(PREFERENCE_DEVICE_NAME_SETTING));
    SerialTemp.printf("PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING));

    SerialTemp.printf("PREFERENCE_MAIN_BLE_CLIENT_VALUE: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE));
    SerialTemp.printf("PREFERENCE_MAIN_BLE_SERVER_VALUE: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE));
    SerialTemp.printf("PREFERENCE_FIRST_TIME_FEATURE_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_FIRST_TIME_FEATURE_SETTING));
    SerialTemp.printf("PREFERENCE_SCREEN_COLOR_SETTING: %d\n", getPreferenceInt_mainModule(PREFERENCE_SCREEN_COLOR_SETTING));
    
    SerialTemp.printf("PREFERENCE_SUB_DAWGPACK_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_SUB_DAWGPACK_SETTING));
    SerialTemp.printf("PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING));
    SerialTemp.printf("PREFERENCE_SENDWIFI_WITH_BLE: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_SENDWIFI_WITH_BLE));
    SerialTemp.printf("PREFERENCE_ONLY_GEN3_CONNECT_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_ONLY_GEN3_CONNECT_SETTING));
    SerialTemp.printf("PREFERENCE_SUPPORT_GROUPS_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_SUPPORT_GROUPS_SETTING));
    SerialTemp.printf("PREFERENCE_GROUP_NAMES_SETTING: %s\n", getPreference_mainModule(PREFERENCE_GROUP_NAMES_SETTING));
    SerialTemp.printf("PREFERENCE_DEV_ONLY_SM_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_DEV_ONLY_SM_SETTING));

#if (SERIAL_DEBUG_CALL)
    // this is many lines long .. so only show in the CALL settting..
    SerialTemp.printf("PREFERENCE_DEBUG_INFO_SETTING: %s\n", getPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING));
#endif
  
    
    SerialTemp.printf("WIFI_CREDENTIAL: %s\n", main_JSONStringForWIFICredentials());
    //!retrieve a JSON string for the ssid and ssid_password: {'ssid':<ssid>,'ssidPassword':<pass>"}
    
#endif
}


//!resets preferences.. Currently only reset all, but eventually reset(groups..)
void resetAllPreferences_mainModule()
{
//    savePreferenceBoolean_mainModule(PREFERENCE_SENDWIFI_WITH_BLE, false);
//    savePreferenceInt_mainModule(PREFERENCE_TIMER_INT_SETTING, 30);
    
    SerialLots.println("Clean EPROM.. ");
    //! dispatches a call to the command specified. This is run on the next loop()
    main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_EPROM);
    
}

boolean _DiscoverM5PTClicker = false;
//!transient for now...  10.4.22
//! set the M5 PTClicker discovery option..
void setDiscoverM5PTClicker(boolean flag)
{
    SerialTemp.println("Setting _DiscoverM5PTClicker");
    _DiscoverM5PTClicker = flag;
}
//! get option
boolean getDiscoverM5PTClicker()
{
    return _DiscoverM5PTClicker;
}
