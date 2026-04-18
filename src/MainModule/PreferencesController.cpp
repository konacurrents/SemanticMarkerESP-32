
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

//! 1.27.26 Chain Buttons .. which non-chain can use too
//! storage tor a message
char _mqttMessage_ChainButton[100];

//! 8.2.24 includeGroups
//! 8.2.24 break up the list..
#define NUMBER_GROUPS 4
#define STRING_MAX_SIZE 40
int _includeGroupLen = 0;
//!resulting group names
char _includeGroupsStringArray[NUMBER_GROUPS][STRING_MAX_SIZE];


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
#define EPROM_STEPPER_FACTORY_CLOCKWISE_MOTOR_DIRECTION_SETTING "31md"

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

//! 11.29.23 add the max time .. so a random can be used
#define EPROM_PREFERENCE_TIMER_MAX_INT_SETTING "39x"

//! 1.1.24 the preference for all the ATOM plugs (format:  atomType:value} .. for now just use socket:on
#define EPROM_PREFERENCE_ATOMS_SETTING "40a"

//!1.4.24  What kind of ATOM plug (set, M5AtomKind, val= {M5AtomSocket, M5AtomScanner}
#define EPROM_PREFERENCE_ATOM_KIND_SETTING "41Atom"

//! 1.10.24 Flag on whether a Semantic Marker command is sent on PIR, and the Command to send
#define EPROM_PREFERENCE_SM_ON_PIR_SETTING "42pir"
//! 1.10.24 The  Semantic Marker command is sent on PIR, and the Command to send
#define EPROM_PREFERENCE_SM_COMMAND_PIR_SETTING "43pir"
//! 1.11.24 The  Semantic Marker command is sent on PIR, and the Command to send
#define EPROM_PREFERENCE_SM_COMMAND_PIR_OFF_SETTING "44pir"
//! 1.12.24 Whether the AtomSocket accepts global on/off messages
#define EPROM_PREFERENCE_ATOM_SOCKET_GLOBAL_ONOFF_SETTING "45sock"

//! 4.4.24 for MQTT use of spiff (or not)
#define EPROM_USE_SPIFF_MQTT_SETTING "46spiff"
//! 4.4.24 for QRATOM use of spiff (or not)
#define EPROM_USE_SPIFF_QRATOM_SETTING "47spiff"

//! 8.2.24 to let older Tumbler NOT do the auto direction (back and forth)
//! Isue #332
//! it will set via message: autoMotorDirection
//! {"set":"autoMotorDirection","val":"true"}
#define EPROM_STEPPER_AUTO_MOTOR_DIRECTION_SETTING "48a"

//! include these topics groups..
#define EPROM_INCLUDE_GROUP_NAMES_SETTING "49e"

//!retreives the  FACTORY motor direction| true default, clockwise; false = REVERSE, counterclockwise 9.8.22
//! false = reverse == counterclockwise
//! true = default
#define EPROM_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING "50sf"


//! issue #338 sensor definition (in work)
//! This will be a string in JSON format with various PIN and BUS information
#define EPROM_SENSOR_PLUGS_SETTING "51sp"

//!5.14.25 Dead 5.14.74 Montana
//! issue #365 Object Oriented Sensors as well
//! define the sensors (not sensorPlugs). MQTT:  set:sensor,  set:sensors
#define EPROM_SENSORS_SETTING "52sensors"

//!8.14.25 Dead Movie from 10.19.1974 tonight..
//! issue #394 stepperRPM
//! stepper RPM
#define EPROM_STEPPER_RPM_SETTING "53sRPM"

//! 9.3.25 back from LA, Horses out. Tyler on lap. Europe next week
//! sets the 2feed option (go back and forth)
#define EPROM_STEPPER_2FEED_SETTING "542feed"

//!12.21.25 Winter Solstice party ..
//! issue #365 Object Oriented Sensors as well
//! define the sensors (not sensorPlugs). MQTT:  set:sensor,  set:sensors
#define EPROM_CHAIN_SENSORS_SETTING "6Chain5snsors"


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

//! 8.2.24 retrieve the includeGroup
//! really ask a topic if it's in the include group
//! modifies the _
boolean topicInIncludeGroup(char *topic)
{
    // for now just see if topc in groups..
    //! 8.2.24 just string match for now..
    //! NOTE: topic is a full path ..  but
    //! MessageArrived: '#FEED {'deviceName':'MaggieMae'}', onTopic=usersP/groups/atlasDogs
    boolean found = false;
    
    //! find the topic (eg. usersP/groups/atlasDogs , is atlasDogs
    char *topicName = rindex(topic,'/');
    //! go past the "/"
    topicName++;
    
    //! go through list..
    for (int i=0; i< _includeGroupLen; i++)
    {
        char *group = _includeGroupsStringArray[i];
        if (group && strlen(group) == 0)
        {
            //!empty is a find .. not specified
            found = true;
            break;
        }
        //! note "group" the short name is 2nd so it's asking if "/userP/groups/atlasDogs contains string atlasDogs
        //! that way the topic doesn't need to be parsed.. (although it could be wrong :eg  atlas also matches..
        //if (containsSubstring(topic, group))
        SerialTemp.printf("compare: %s to %s\n", topicName, group);
        if (strcmp(topicName, group) == 0)
        {
            found = true;
            break;
        }
    }
    if (_includeGroupLen == 0)
    {
        found = true;
    }

    SerialTemp.printf("topicInIncludeGroup(%s) =%d\n", topicName, found);
    return found;
}

//! 8.2.24 set the include group (and cache it), called (indirectly from MQTT via setIncludeGroups
void parseIncludeGroups(char *groups)
{
    SerialTemp.printf("parseIncludeGroups %s\n", groups);

    //! parse the groups (if nothing, then it's ok if no "," use full string)
    /**
     The strtok_r() function is a reentrant version strtok(). The saveptr argument is a pointer to a char * variable that is used internally by strtok_r() in order to maintain context between successive calls that parse the same string.
     On the first call to strtok_r(), str should point to the string to be parsed, and the value of saveptr is ignored. In subsequent calls, str should be NULL, and saveptr should be unchanged since the previous call.
     
     char *strtok_r(char *str, const char *delim, char **saveptr);
     @see https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
     @see https://linux.die.net/man/3/strtok_r
     */
    
    _includeGroupLen = 0;
    if (!index(groups,','))
    {
        //!no comma
        strcpy(_includeGroupsStringArray[_includeGroupLen], groups);
        _includeGroupLen++;
        SerialTemp.printf("Add Group[%d] %s\n", _includeGroupLen, groups);
    }
    else
    {
        char *str = groups;
        char *rest = NULL;
        char *token;
        for (token = strtok_r(str,",",&rest); token != NULL; token = strtok_r(NULL, ",", &rest))
        {
            strcpy(_includeGroupsStringArray[_includeGroupLen], token);
            SerialTemp.printf("Add Group[%d] %s\n", _includeGroupLen, _includeGroupsStringArray[_includeGroupLen]);
            _includeGroupLen ++;

        }
    }
}

//! 8.2.24 set the include group (and cache it), called from MQTT
void setIncludeGroups(char *groups)
{
    //!process the groups..
    parseIncludeGroups(groups);
    
    //!save persistently
    savePreference_mainModule(PREFERENCE_INCLUDE_GROUP_NAMES_SETTING, groups);
}

//! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
void savePreference_mainModule(int preferenceID, String preferenceValue)
{
#ifdef NOT_NOW
    if (preferenceID != PREFERENCE_DEBUG_INFO_SETTING)
        SerialTemp.printf("savePreference .. %d = '%s'\n", preferenceID, preferenceValue.c_str());
#endif
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
#ifdef AP_DEBUG_MODE

    appendPreference_mainModule(preferenceID, preferenceValue);
   // SerialDebug.printf("storePref(%d): %s\n", _appendingPreferenceString.length(), _appendingPreferenceString.c_str());
    savePreference_mainModule(preferenceID, _appendingPreferenceString);
#else
    //!turn AP_DEBUG_MODe off for now...
    //!1.1.24 seems the append is goofing things..
    savePreference_mainModule(preferenceID, preferenceValue);

#endif
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
    //!TODO: 8.18.24 add a cache for floats ..
    //!actially cache the string value??
    char* val = getPreference_mainModule(preferenceID);
    float fval = atof(val);
    return fval;
}
//! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
void savePreferenceFloat_mainModule(int preferenceID, float val)
{
    //!convert to a string..
    char str[20];
    sprintf(str,"%2f",val);
    savePreference_mainModule(preferenceID, str);
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
//! 5.14.25 (Dead 5.14.74 3rd Wall of Sound)
//! add the Sensors as well..
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
            SerialLots.printf(" *** alreadySet %s\n",_preferenceMainModuleLookupEPROMNames[preferenceID] );
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
                //! 9.4.25 STRANGE: the BOOLEAN aren't working too good
                //! so removing CLOCKWISE and STEPPER_2FEED
                //!**** NOTE: THis is where whether things are cached or not is set! Eventually all boolean and int could be cached..
                //! CACHE SETTINGS (boolean)
            case PREFERENCE_SUPPORT_GROUPS_SETTING:
            case PREFERENCE_SENDWIFI_WITH_BLE:
            case PREFERENCE_IS_MINIMAL_MENU_SETTING:
            case PREFERENCE_ONLY_GEN3_CONNECT_SETTING:
            case PREFERENCE_MAIN_BLE_SERVER_VALUE:
            case PREFERENCE_MAIN_BLE_CLIENT_VALUE:
            case PREFERENCE_SENSOR_TILT_VALUE:
            case PREFERENCE_USE_DOC_FOLLOW_SETTING:
            case PREFERENCE_DEV_ONLY_SM_SETTING:
                //!8.2.24
            case PREFERENCE_STEPPER_AUTO_MOTOR_DIRECTION_SETTING:
                //! 9.3.25 strange wasn't working ... if in cache..
           // case PREFERENCE_STEPPER_2FEED_SETTING:
           // case PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING:

                //SerialLots.printf("setting Cached[%d] = %s\n", i, preferenceValue);
                _isCachedPreferenceBoolean[i] = true;
                _cachedPreferenceBooleanValues[i] = (preferenceValue.compareTo("1")==0)?true:false;
                break;
                //! CACHE SETTINGS (int)
            case PREFERENCE_SCREEN_COLOR_SETTING:
            case PREFERENCE_STEPPER_KIND_VALUE:
            case PREFERENCE_TIMER_INT_SETTING:
            case PREFERENCE_TIMER_MAX_INT_SETTING:

                _isCachedPreferenceInt[i] = true;
                _cachedPreferenceIntValues[i] = atoi(&preferenceValue[0]);
                break;
                
                //! 8.2.24 set the includeGroups
            case PREFERENCE_INCLUDE_GROUP_NAMES_SETTING:
                //! grab the state at the start, then it's only modified from a MQTT message
            {
                char *groups = getPreferenceString_mainModule(PREFERENCE_INCLUDE_GROUP_NAMES_SETTING);
                //!save
                //! parse (do it this way, instead of setIncludeGroups (as that stored in eprom again)
                parseIncludeGroups(groups);
            }
                break;
#ifdef TODO_THIS_CACHE
                //! 8.18.24 figure this out ..
                //! the UNO is not using this .. so let's not worry about it for now ... only Tumbler.
            case PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING:
            {
                //! do this in the "string" part, not the float part..
                break;
            }
#endif
            default:
                break;
        }
    }
    //! Close the Preferences
    _preferencesMainModule.end();
    
    //! set onbootPreferences
    setOnBootPreferences_mainModule();
    
    //! 5.14.25 (Dead 5.14.74 3rd Wall of Sound)
    //! add the Sensors as well..
    initSensorStringsFromEPROM_mainModule();
    
#pragma mark CHAIN
    //! 1.23.26 do the same for chain
    //! 1.30.26 THIS is done whether we have the actual buttons or not,
    //! that is defined which SensorClass -- ChainButtonClassType
    initChainSensorStringsFromEPROM_mainModule();
    
}

//!initialize the _preferencesMainLookup with EPROM lookup names
//!BUT these are not stored in EPROM. The next method 
void initPreferencesMainModule()
{
    SerialDebug.println("***initPreferencesMainModule");
#ifdef AP_DEBUG_MODE
#else
   // savePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING,"");
    resetPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING);
#endif
    
    strcpy(_preferenceBufferString,(char*)"");
    strcpy(_preferenceBuffer,(char*)"");
    
    //! 9.4.25 NOTE: tese are in case the EPROM wasn't set. So the EPROM is used over these values..
    for (int i = 0; i < MAX_MAIN_PREFERENCES; i++)
    {
        switch (i)
        {
            case PREFERENCE_MAIN_BLE_SERVER_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_MAIN_BLE_SERVER_VALUE;
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#ifdef ESP_M5
                //! 12.27.23 ON for all M5 (the reason is credentials ...)
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
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#endif //ATOM
#endif //ESP_M5_CAMERA
#else
                //! 12.8.22  default back TRUE
                //!default off for the M5 (but it can be turned on later..)
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#endif //ESP_M5
                SerialLots.printf(" **** setting PREFERENCE_MAIN_BLE_SERVER_VALUE[%d]: %s\n", i,_preferenceMainModuleLookupDefaults[i]);
                break;
            case PREFERENCE_MAIN_BLE_CLIENT_VALUE:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_MAIN_BLE_CLIENT_VALUE;
#ifdef ESP_M5
                //! 12.27.23  ON for most M5 , but off for the SOCKET
#ifdef ATOM_SOCKET_MODULE
                //! only the SOCKET will be off by default for now...
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
#else
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#endif // ESP_M5_ATOM_LITE
                
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
#ifdef ESP_M5
                //! 5.2.25 default 0.5 SECONDS (not angle for the HDriver
                _preferenceMainModuleLookupDefaults[i] = (char*)"1.25";
#else
                _preferenceMainModuleLookupDefaults[i] = (char*)"22.5";
#endif
                break;
            case PREFERENCE_TIMER_INT_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_PREFERENCE_TIMER_INT_SETTING;
                //! 3.28.23 change default to 5 (from 30)
                _preferenceMainModuleLookupDefaults[i] = (char*)"5";
                break;
            case PREFERENCE_TIMER_MAX_INT_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] = (char*) EPROM_PREFERENCE_TIMER_MAX_INT_SETTING;
                //! 11.29.23 add a max so random can be used
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
#ifdef ESP_M5_ATOM_LITE
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
                //! 3.24.25 use the BLE name in the feeder too..
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
                //_preferenceMainModuleLookupDefaults[i] = (char*)"0";

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
                //! 8.18.24 the factory setting
                ///!retreives the motor direction| 0 (false) = default, clockwise; 1 (true) = REVERSE, counterclockwise 9.8.22
                //! TRUE = reverse == counterclockwise
                //! FALSE = default
            case PREFERENCE_STEPPER_FACTORY_CLOCKWISE_MOTOR_DIRECTION_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_STEPPER_FACTORY_CLOCKWISE_MOTOR_DIRECTION_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
                
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
                //! default OFF 1.15.24
                //! @see https://github.com/konacurrents/ESP_IOT/issues/300
            case PREFERENCE_SUPPORT_GROUPS_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_PREFERENCE_SUPPORT_GROUPS_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
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
                
                //! 1.1.24  first version of preferences for the ATOMs depending on which ATOM kind
                //! first version, only the socket and the value is on/off
                //! syntaxURL   socket=off&smscanner=on
            case PREFERENCE_ATOMS_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_PREFERENCE_ATOMS_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"socket=off";
                break;
                
                //!1.4.24  What kind of ATOM plug (set, M5AtomKind, val= {M5AtomSocket, M5AtomScanner}
            case PREFERENCE_ATOM_KIND_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_PREFERENCE_ATOM_KIND_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"M5AtomScanner";
                break;
                
                //! 1.10.24 The  Semantic Marker command is sent on PIR, and the Command to send
            case PREFERENCE_SM_ON_PIR_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_PREFERENCE_SM_ON_PIR_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
                break;
                
                //! 1.10.24 The  Semantic Marker command is sent on PIR, and the Command to send
            case PREFERENCE_SM_COMMAND_PIR_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_PREFERENCE_SM_COMMAND_PIR_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"{'set':'socket','val':'on'}";
                break;
                //! 1.11.24 The  Semantic Marker command is sent on PIR, and the Command to send for OFF
            case PREFERENCE_SM_COMMAND_PIR_OFF_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_PREFERENCE_SM_COMMAND_PIR_OFF_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"{'set':'socket','val':'off'}";
                break;
                
                //! 1.12.24 whether global on/off is allowed. Default on..
            case PREFERENCE_ATOM_SOCKET_GLOBAL_ONOFF_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_PREFERENCE_ATOM_SOCKET_GLOBAL_ONOFF_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
                break;
                
                //!//! 4.4.24 to turn on/off SPIFF use  (not cached yet as it's an infrequent event)
            case PREFERENCE_USE_SPIFF_MQTT_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_USE_SPIFF_MQTT_SETTING;
#ifdef USE_SPIFF_MQTT_SETTING
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#endif //USE_SPIFF_MQTT_SETTING
                break;
                //!//! 4.4.24 to turn on/off SPIFF use  (not cached yet as it's an infrequent event)
            case PREFERENCE_USE_SPIFF_QRATOM_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_USE_SPIFF_QRATOM_SETTING;
#ifdef USE_SPIFF_QRATOM_SETTING
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
#endif //USE_SPIFF_QRATOM_SETTING
                break;
                
                
                //! 8.2.24 to let older Tumbler NOT do the auto direction (back and forth)
                //! Isue #332
                //! it will set via message: autoMotorDirection
                //! {"set":"autoMotorDirection","val":"true"}
            case PREFERENCE_STEPPER_AUTO_MOTOR_DIRECTION_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_STEPPER_AUTO_MOTOR_DIRECTION_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"1";
                break;
              
                //! 8.2.24 include these groups (or none)
                //! {"set":"includeGroups","val":"group1,group2"}
            case PREFERENCE_INCLUDE_GROUP_NAMES_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_INCLUDE_GROUP_NAMES_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"";
                break;
                
                //! issue #338 sensor definition (in work)
                //! This will be a string in JSON format with various PIN and BUS information
            case PREFERENCE_SENSOR_PLUGS_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_SENSOR_PLUGS_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"L9110S_DCStepperClass";
                break;
                
                //! 12.21.25 Winter Solstice, Eagles, Party,
                //! these are the CHAIN_SENSORS
            case PREFERENCE_CHAIN_SENSORS_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_CHAIN_SENSORS_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"";
                break;
                
                //!5.14.25 Dead 5.14.74 Montana
                //! issue #365 Object Oriented Sensors as well
                //! define the sensors (not sensorPlugs). MQTT:  set:sensor,  set:sensors
                //! 7.9.25 default to BuzzerSensorClass and L9110S_DCStepperClass
            case PREFERENCE_SENSORS_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_SENSORS_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"BuzzerSensorClass,23,33,L9110S_DCStepperClass,21,25";
                break;
              
                
                //!8.14.25 Dead Movie from 10.19.1974 tonight..
                //! issue #394 stepperRPM
                //! stepper RPM
            case PREFERENCE_STEPPER_RPM_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_STEPPER_RPM_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"15.0";
                break;
                                
                //!9.3.25 Dead Movie.. let it grow
                //!
            case PREFERENCE_STEPPER_2FEED_SETTING:
                _preferenceMainModuleLookupEPROMNames[i] =
                (char*)EPROM_STEPPER_2FEED_SETTING;
                _preferenceMainModuleLookupDefaults[i] = (char*)"0";
                SerialLots.println(" ** setting PREFERENCE_STEPPER_2FEED_SETTING = 0");
                break;
                
                
            default:
                SerialError.printf(" ** NO default for preference[%d]\n", i);
        }
        SerialLots.printf("** setting [%d] = %s\n", i, _preferenceMainModuleLookupDefaults[i]);
    }
}

#ifdef NOT_USED
//! 7.9.25 reset SENSORS to default
//! "BuzzerSensorClass,23,33,L9110S_DCStepperClass,21,25"
void resetSensorToDefault_mainModule()
{
    setSensorsString_mainModule((char*)"BuzzerSensorClass,23,33,L9110S_DCStepperClass,21,25");
}
#endif

//!print the preferences to SerialDebug
void printPreferenceValues_mainModule()
{
    //!this inits the string to the EPROM value
    readAppendingPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING);
    
#ifdef SERIAL_DEBUG_TEMP
    SerialTemp.println("******************");
    SerialTemp.println(VERSION);
    SerialTemp.printf("CHIP_ID: %s\n", getChipIdString());
    SerialTemp.printf("PREFERENCE_DEVICE_NAME_SETTING: %s\n", getPreference_mainModule(PREFERENCE_DEVICE_NAME_SETTING));
    {
        const char *BLEDeviceName = connectedBLEDeviceName_mainModule()?connectedBLEDeviceName_mainModule():"none";

        SerialTemp.printf("CONNECTED_BLE_DEVICE: %s\n", BLEDeviceName);
    }
    //! 12.24.25 return the username and password, Kevin feature .. he was setting to WIFI acidently. I updated APP to make that harder
    //! note on reboot this won't be set until later, so do another ""
    SerialTemp.printf("USERNAME: %s\n", main_getUsername());
    SerialTemp.printf("PASSWORD: %s\n", main_getPassword());
    
    //ouch.. this sets 2 values ..
    //  readPreferences_mainModule();
    SerialTemp.printf("STEPPER_KIND: %d  1=UNO,2=MINI,3=TUMBLER\n", getPreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE));
    SerialTemp.printf("PREFERENCE_TIMER_INT_SETTING: %d\n", getPreferenceInt_mainModule(PREFERENCE_TIMER_INT_SETTING));
    SerialTemp.printf("STEPPER_AUTO_FEED: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_AUTO_FEED_VALUE));
    SerialTemp.printf("STEPPER_JACKPOT_FEED: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_JACKPOT_FEED_VALUE));
    SerialTemp.printf("PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING: %f\n", getPreferenceFloat_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING));
    SerialTemp.printf("STEPPER_FEEDS_PER_JACKPOT: %d\n", getPreferenceInt_mainModule(PREFERENCE_STEPPER_FEEDS_PER_JACKPOT));
    SerialTemp.printf("DISPLAY_SCREEN_TIMEOUT: %d\n", getPreferenceInt_mainModule(PREFERENCE_DISPLAY_SCREEN_TIMEOUT_VALUE));
    SerialTemp.printf("PREFERENCE_STEPPER_BUZZER_VALUE: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE));

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
    SerialTemp.printf("PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING));

    SerialTemp.printf("PREFERENCE_MAIN_BLE_CLIENT_VALUE: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE));
    SerialTemp.printf("PREFERENCE_MAIN_BLE_SERVER_VALUE: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE));
    
#ifdef TOO_SOON
    //! DARN: the class hasn't been instantiated yet ...
    //! 10.10.25 #405 #406
    //! see if the device is a PTClicker if the M5Atom class is one..
    //! return the service name:  PTClicker or PTFeeder
    SerialTemp.printf("PREFERENCE_BLE_SERVER_NAME: %s:%s\n", getServerServiceName_mainModule(),getPreference_mainModule(PREFERENCE_DEVICE_NAME_SETTING));
#endif
    SerialTemp.printf("PREFERENCE_FIRST_TIME_FEATURE_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_FIRST_TIME_FEATURE_SETTING));
    SerialTemp.printf("PREFERENCE_SCREEN_COLOR_SETTING: %d\n", getPreferenceInt_mainModule(PREFERENCE_SCREEN_COLOR_SETTING));
    
    SerialTemp.printf("PREFERENCE_SUB_DAWGPACK_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_SUB_DAWGPACK_SETTING));
    SerialTemp.printf("PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING));
    SerialTemp.printf("PREFERENCE_STEPPER_FACTORY_CLOCKWISE_MOTOR_DIRECTION_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_FACTORY_CLOCKWISE_MOTOR_DIRECTION_SETTING));
    SerialTemp.printf("AUTO_MOTOR_DIRECTION: %d  1=reverseEachTime,2=dont\n", getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_AUTO_MOTOR_DIRECTION_SETTING));
    //! don't change subscription but include these groups (eg. safeHouse,atlasDogs)
    SerialTemp.printf("PREFERENCE_INCLUDE_GROUP_NAMES_SETTING: %s\n", getPreference_mainModule(PREFERENCE_INCLUDE_GROUP_NAMES_SETTING));
    SerialTemp.printf("PREFERENCE_SENDWIFI_WITH_BLE: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_SENDWIFI_WITH_BLE));
    SerialTemp.printf("PREFERENCE_ONLY_GEN3_CONNECT_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_ONLY_GEN3_CONNECT_SETTING));
    SerialTemp.printf("PREFERENCE_SUPPORT_GROUPS_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_SUPPORT_GROUPS_SETTING));
    SerialTemp.printf("PREFERENCE_GROUP_NAMES_SETTING: %s\n", getPreference_mainModule(PREFERENCE_GROUP_NAMES_SETTING));
    SerialTemp.printf("PREFERENCE_DEV_ONLY_SM_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_DEV_ONLY_SM_SETTING));

    SerialTemp.printf("PREFERENCE_ATOMS_SETTING: %s\n", getPreference_mainModule(PREFERENCE_ATOMS_SETTING));
    SerialTemp.printf("PREFERENCE_ATOM_KIND_SETTING (M5AtomClassType): %s\n", getPreference_mainModule(PREFERENCE_ATOM_KIND_SETTING));

    //! 1.10.24 Flag on whether a Semantic Marker command is sent on PIR, and the Command to send
    SerialTemp.printf("PREFERENCE_SM_ON_PIR_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_SM_ON_PIR_SETTING));
    //! 1.10.24 The  Semantic Marker command is sent on PIR, and the Command to send
    SerialTemp.printf("PREFERENCE_SM_COMMAND_PIR_SETTING: %s\n", getPreference_mainModule(PREFERENCE_SM_COMMAND_PIR_SETTING));
    //! 1.11.24 The  Semantic Marker command is sent on PIR, and the Command to send
    SerialTemp.printf("PREFERENCE_SM_COMMAND_PIR_OFF_SETTING: %s\n", getPreference_mainModule(PREFERENCE_SM_COMMAND_PIR_OFF_SETTING));
    //! 1.12.24 The  Semantic Marker command is sent on PIR, and the Command to send
    SerialTemp.printf("PREFERENCE_ATOM_SOCKET_GLOBAL_ONOFF_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_ATOM_SOCKET_GLOBAL_ONOFF_SETTING));
    
    SerialTemp.printf("PREFERENCE_SENSOR_PLUGS_SETTING (MotorStepperClassType): %s\n", getPreference_mainModule(PREFERENCE_SENSOR_PLUGS_SETTING));
    
    //! 5.14.25
    SerialTemp.printf("PREFERENCE_SENSORS_SETTING: %s\n", getPreference_mainModule(PREFERENCE_SENSORS_SETTING));

    //! 12.21.25
    //! 5.14.25
    SerialTemp.printf("PREFERENCE_CHAIN_SENSORS_SETTING: %s\n", getPreference_mainModule(PREFERENCE_CHAIN_SENSORS_SETTING));

    //!8.14.25 Dead Movie from 10.19.1974 tonight..
    //! issue #394 stepperRPM
    //! stepper RPM
    SerialTemp.printf("PREFERENCE_STEPPER_RPM_SETTING: %s\n", getPreference_mainModule(PREFERENCE_STEPPER_RPM_SETTING));

    //! 9.3.25 back from LA, Horses out. Tyler on lap. Europe next week
    //! sets the 2feed option (go back and forth)
    SerialTemp.printf("PREFERENCE_STEPPER_2FEED_SETTING: %d\n", getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_2FEED_SETTING));

    
#ifdef M5CORE2_MODULE
    SerialTemp.printf("PREFERENCE_M5Core2_SETTING:\n");
#endif
#if (SERIAL_DEBUG_CALL)
    // this is many lines long .. so only show in the CALL settting..
    SerialTemp.printf("PREFERENCE_DEBUG_INFO_SETTING: %s\n", getPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING));
#endif
  
    
    SerialTemp.printf("WIFI_CREDENTIAL: %s\n", main_JSONStringForWIFICredentials());
    //!retrieve a JSON string for the ssid and ssid_password: {'ssid':<ssid>,'ssidPassword':<pass>"}
    
    //! 5.14.25 also print out the sensors
    //! print sensors
    printSensors_mainModule(getSensors_mainModule());
    
    //! 1.24.26 Clear day .. 2 eagles again on walk
    printChainSensors_mainModule(getChainUseStruct_mainModule());
    
    //! 7.31.25
    SerialTemp.println(" *** Example JSON messages you can modify and paste into serial monitor, no DEV required");
    //! TODO: add preference to not support this??
    //! show example JSON  (break up as getPreference re-uses same string...
    SerialTemp.printf("{\"set\":\"sensors\",\"val\":\"%s\"}", (char*)"BuzzerSensorClass,19,22,L9110S_DCStepperClass,21,25");
    SerialTemp.println();
//    SerialTemp.printf("{\"set\":\"sensors\",\"val\":\"%s\"}", (char*)"BuzzerSensorClass,21,25,ULN2003_StepperClass,23,33");
//    SerialTemp.println();
    SerialTemp.printf("{\"set\":\"sensors\",\"val\":\"%s\"}", (char*)"PIRSensorClass,23,33");
    SerialTemp.println();
    SerialTemp.printf("{\"set\":\"sensorPlugs\",\"val\":\"L9110S_DCStepperClass\"}");
    SerialTemp.println();
    SerialTemp.printf("{\"set\":\"M5AtomKind\",\"val\":\"M5HDriver\"}");
    SerialTemp.println();
//    SerialTemp.printf("{\"set\":\"stepperAngle\",\"val\":\"0.25\"} ");
//    SerialTemp.println();
//    SerialTemp.printf("{\"set\":\"stepperRPM\",\"val\":\"15.0\"}");
//    SerialTemp.println();
//    SerialTemp.printf("{\"set\":\"stepperAngle\",\"val\":\"2048.0\"}");
//    SerialTemp.println();
    SerialTemp.printf("{\"set\":\"stepperAngle\",\"val\":\"150\"}  --new Feeder");
    SerialTemp.println();

    
    SerialDebug.println("{\"ssid\":\"Bob\", \"ssidPassword\":\"scott\"}");
    SerialDebug.println("{\"set\":\"2feed\", \"val\":\"1\"}");
    SerialDebug.println("{\"set\":\"BLEUseDeviceName\", \"val\":\"off\"}");

    
    //! 7.31.25 PIN USE
    PinUseStruct* pinUseStruct = getPinUseStruct_mainModule();
    SerialTemp.printf(" *** PIN USE (%d) .. check for duplicated done next .. look for 'ERROR' \n", pinUseStruct->pinUseCount);

    for (int i=0; i< pinUseStruct->pinUseCount; i++)
    {
        SerialTemp.println(pinUseStruct->pinUseArray[i]);
    }
    SerialTemp.printf(" *** PIN Count = %d\n", pinUseStruct->pinUseCount);
    
    //! this needs to have a long pin in the array as well
    //! 8.30.25 check if duplicates
    //! @see https://stackoverflow.com/questions/8199403/how-to-check-if-an-array-has-any-duplicates
    int count = pinUseStruct->pinUseCount;
    boolean duplicatePins = false;
    
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            long pin1 = pinUseStruct->pinNumArray[i];
            long pin2 = pinUseStruct->pinNumArray[j];
            if (pin1 == pin2)
            {
                // do whatever you do in case of a duplicate
                duplicatePins = true;
                SerialDebug.printf("*** ERROR: DUPLICATE PIN: %d, (%s) == (%s)\n",pin1, pinUseStruct->pinUseArray[i], pinUseStruct->pinUseArray[j]);
            }
        }
    }
    
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


//! if the preference was retrieved..
boolean _firstTimeAtomKind = true;
int _ATOM_KIND = ATOM_KIND_M5_SCANNER;
//!returned from mainModule
//#define ATOM_KIND_M5_SCANNER 0
//#define ATOM_KIND_M5_SOCKET 1
//! new 1.4.24 setting ATOM kind (eg. M5AtomSocket, M5AtomScanner)
//! 1.5.24 also set the initial atom storage
//! Then the device reboots.. so setup() and loop() are for the correct ATOM
void savePreferenceATOMKind_MainModule(String value)
{
    SerialDebug.printf("M5AtomKind = %s\n", value.c_str());
    _firstTimeAtomKind = true;
    savePreference_mainModule(PREFERENCE_ATOM_KIND_SETTING,  value);
    
    //! 1.5.24 also set the initial atom storage
    //! 1.4.24 use the _atomKind (which CAN change)
    switch (getM5ATOMKind_MainModule())
    {
        case ATOM_KIND_M5_SCANNER:
            //! 8.1.23 for the ATOM Lite QRCode Reader
            savePreference_mainModule(PREFERENCE_ATOMS_SETTING,"smscanner=on");
            break;
        case ATOM_KIND_M5_SOCKET:
            //! 12.26.23 for the ATOM Socket Power
            savePreference_mainModule(PREFERENCE_ATOMS_SETTING,"socket=off");
            break;
    }

}
//! new 1.4.24 setting ATOM kind (eg. M5AtomSocket, M5AtomScanner)
char* getPreferenceATOMKind_MainModule()
{
    char *atomKind = getPreference_mainModule(PREFERENCE_ATOM_KIND_SETTING);
    return atomKind;
}

//! new 1.4.24 setting ATOM kind (eg. ATOM_KIND_M5_SCANNER, ATOM_KIND_M5_SOCKET)
//! sets global _ATOM_KIND
int getM5ATOMKind_MainModule()
{
    //! optimize to only call this retrieval from EPROM 1 time..  unless changing the ATOMKind
    if (_firstTimeAtomKind)
    {
        char *atomKind = getPreferenceATOMKind_MainModule();
        if (strcasecmp(atomKind,"M5AtomScanner")==0)
        {
            _ATOM_KIND = ATOM_KIND_M5_SCANNER;
        }
        else if (strcasecmp(atomKind,"M5AtomSocket")==0)
        {
            _ATOM_KIND = ATOM_KIND_M5_SOCKET;
        }
        _firstTimeAtomKind = false;
    }
    return _ATOM_KIND;
}

#define NEW_SENSORS_PREFERENCE
//! 5.14.25 Hanging with Tyler,
//! Dead Montana 5.14.74 great stuff
//! add the Sensors Preference .. first the parsing
//! 10.26.25 Power Out, Wind Storm, Tyler handing ..
//! remove the PRINT

//! the memory for the sensorsEPROM
char _sensorsEPROM[500];


//! array of sensorStruct
SensorsStruct *_sensorsStructs_mainModule = NULL;

//! return array of SensorsStruct after parsing string syntax:   {SENSOR,pin1,pin2}
SensorsStruct* parseSensorString_mainModule(char *str);



//! print sensor
void printSensor_mainModule(SensorStruct* sensor)
{
    if (sensor)
        SerialDebug.printf("SENSOR: %s,%d,%d\n", sensor->sensorName, sensor->pin1, sensor->pin2);
    else
        SerialDebug.printf("SENSOR: **** Null sensor ***\n");
}


//! print sensors, passing in a struct
void printSensors_mainModule(SensorsStruct* sensors)
{
    int count = sensors->count;
    SerialDebug.printf("DEFINED_SENSORS SensorClassType (%ld):\n", count);
    SerialDebug.printf(" ******** \n");
    for (int i=0; i< count; i++)
    {
        printSensor_mainModule(&sensors->sensors[i]);
    }
    SerialDebug.printf(" ******** \n");
}

//! return the sensors defined
SensorsStruct* getSensors_mainModule()
{
    return _sensorsStructs_mainModule;
}

//! return the sensor specified or null
SensorStruct* getSensor_mainModule(char *sensorName)
{
    SensorStruct *sensor = NULL;
    if (!_sensorsStructs_mainModule)
    {
        SerialDebug.println(" **** sensorsStructs_mainModule NULL ****");
        return NULL;
    }
    
    int count = _sensorsStructs_mainModule->count;
    for (int i=0; i< count; i++)
    {
        if (strcmp(_sensorsStructs_mainModule->sensors[i].sensorName, sensorName) == 0)
        {
            sensor = &_sensorsStructs_mainModule->sensors[i];
            break;
        }
    }
//    if (!sensor)
//        SerialDebug.printf("*** No sensor: %s\n", sensorName);
    return sensor;
}

//! Only 1 setSensorsString now .. will always append
//! unless a null or blank "" string
//! set a sensor val (array of  sensor,pin,pin,sensor,pin,pin...)
void setSensorsString_mainModule(char *sensorsString)
{
    //! for now .. resetting
    //! 5.17.25
    strcpy(_sensorsEPROM, "");

    SerialDebug.printf("setSensorsString_mainModule(%s)\n", sensorsString);
    //! init EPROM
    if (!sensorsString || strlen(sensorsString)==0)
        strcpy(_sensorsEPROM, "");
    else if (strlen(_sensorsEPROM) > 0)
    {
        // add a ','
        strcat(_sensorsEPROM, ",");
    }
    strcat(_sensorsEPROM, sensorsString);
    
    //! store in EPROM
    savePreference_mainModule(PREFERENCE_SENSORS_SETTING, _sensorsEPROM);

    //! Parse to the global..
    _sensorsStructs_mainModule = parseSensorString_mainModule(_sensorsEPROM);
    SerialDebug.printf("setSensors:_sensorsStructs_mainModule = %d\n", _sensorsStructs_mainModule);
}

//!  init the sensorString from EPROM
//!PREFERENCE_SENSOR_PLUGS_SETTING
void initSensorStringsFromEPROM_mainModule()
{
    SerialDebug.println("**** initSensorStringsFromEPROM_mainModule ****");
    strcpy(_sensorsEPROM, getPreference_mainModule(PREFERENCE_SENSORS_SETTING));
    //! Parse to the global..
    _sensorsStructs_mainModule = parseSensorString_mainModule(_sensorsEPROM);
    SerialDebug.printf("initSensors:_sensorsStructs_mainModule = %d\n", _sensorsStructs_mainModule);

}


//! copy string
char *copyString_mainModule(char *str)
{
    char *copy = strdup(str);
    return copy;
}


//! 3.29.25 Raiiiinier Beeer movie last night
//! 5.13.25 Home with Tyler, Mom in LA
//! Foundation triligy..
//! parseSensorString_mainModule the string
SensorsStruct *parseSensorString_mainModule(char* sensorsString)
{
    //! result
    SensorsStruct *sensors;
    //! default  create memory for the SensorsStruct which holds all the sensors..
    //! the array will be created on the 2nd pass
    sensors = (SensorsStruct*) calloc(1,sizeof(SensorsStruct));
    sensors->count = 0;
    sensors->sensors = NULL;
    
    //! syntax:  sensor,pin1,pin2
    SerialDebug.printf("*** parseSensorString_mainModule: %s\n", sensorsString);
    
    if (!sensorsString || strlen(sensorsString)==0)
    {
        SerialDebug.println("parseSensorString_mainModule: *** No Sensors or string **");
        return sensors;
    }
    
    char *rest = NULL;
    char *token;
    int arrayIndex = 0;
    
    //! needed to copy sensorsString.. as the code below broke the callers' value to "https:" .. SIDE EFFECT
    char str[300];
    char strCopy[300];
    strcpy(str,sensorsString);
    strcpy(strCopy,sensorsString);
    
    //! resulting array
    SensorStruct *sensorItems;
    
    //! number of sensors
    int numSensors;
    
    //! 2 pass
    for (int whichPass = 0; whichPass<2; whichPass++)
    {
#define secondPass (whichPass == 1)
        int max = 3;
        
        if (secondPass)
        {
            numSensors = arrayIndex / max;
            strcpy(str,strCopy);
            SerialDebug.printf(" ** Create sensors %d\n", numSensors);
            sensorItems= (SensorStruct*) calloc(numSensors,sizeof(SensorStruct));
        }
        
        //! reset arrayIndex (which will be increments of 3)
        arrayIndex = 0;
        
        //! 2 pass, first count, 2nd parseSensorString_mainModule
        
        //! look for tokens, comma seperated
        for (char *token= strtok(str,","); token!= NULL; token= strtok(NULL, ","))
        {
            int indexInSensor = arrayIndex / max;
            int indexInArray = arrayIndex % max;
            //SerialDebug.printf("%d Token[%d] = %s\n",indexInArray, indexInSensor,  token);
            
            //! which of the max is this..
            switch (indexInArray)
            {
                case 0:
                {
                    if (secondPass)
                    {
                        //SerialDebug.printf("Sensor = %s\n", token);
                        sensorItems[indexInSensor].sensorName = copyString_mainModule(token);
                        
                        //! empty the class
                        sensorItems[indexInSensor].sensorClassType = NULL;

                    }
                    break;
                }
                case 1:
                {
                    if (secondPass)
                    {
                        int pin = atoi(token);
                        if (pin == 16 || pin == 17)
                        {
                            //!@see https://www.reddit.com/r/arduino/comments/1g89dlo/esp32_crashing_due_to_pinmode_and_fastled/
                            SerialDebug.printf("*** BAD PIN: %d, setting to 22 ***\n", pin);
                            pin = 22;
                        }
                        //SerialDebug.printf("Pin1= %d\n", pin);
                        sensorItems[indexInSensor].pin1= pin;
                    }
                    break;
                }
                case 2:
                {
                    if (secondPass)
                    {
                        int pin = atoi(token);
                        if (pin == 16 || pin == 17)
                        {
                            SerialDebug.printf("*** BAD PIN: %d, setting to 22 ***\n", pin);
                            pin = 22;
                        }
                        //SerialDebug.printf("Pin2= %d\n", pin);
                        sensorItems[indexInSensor].pin2= pin;
                    }
                    break;
                }
            }
            //! only incremnet arrayIndex when
            arrayIndex++;
        }
    }
    
    //! update the result (storage already created)
    sensors->count = numSensors;
    sensors->sensors = sensorItems;
    
    SerialDebug.printf("parseSensorString_mainModule: ** sensors->count = %ld\n", sensors->count);
    return sensors;
}

#pragma mark CHAIN

#pragma mark CHAIN_STRUCT
/**
 #define CHAIN_USE_MAX 15
 typedef struct {
 //! number of chain
 int chainUseCount;
 //! the buton numbers
 int buttonNumberArray[PIN_USE_MAX];
 //! the set strings, val strings, device strings
 char *setStringArray[PIN_USE_MAX];
 //! val strings
 char *valStringArray[PIN_USE_MAX];
 //! the device strings
 char *deviceStringArray[PIN_USE_MAX];
 
 } ChainUseStruct;
 */

//! global for use. This is an object (not a pointer) and has all the storage created statically
ChainUseStruct* _chainUseStruct = NULL;
//! get the pin use array
ChainUseStruct* getChainUseStruct_mainModule()
{
    return _chainUseStruct;
}


//! 1.23.26 storage for the chain sensors EPROM value ..
//! the memory for the chainSensorsEPROM
char _chainSensorsEPROM[700];
//! 1.23.26 Cold, Clear, Mt Nice ... need to use the version above with 5 items ..
//! pointer to the ChainSensors
//! return array of ChainUseStruct after parsing string syntax:   {SENSOR,pin1,pin2}
ChainUseStruct* parseChainSensorString_mainModule(char *chainUseString)
{
    //! syntax:  sensor,pin1,pin2
    SerialDebug.printf("*** parseChainSensorString_mainModule: %s\n", chainUseString);
    
    //! result
    ChainUseStruct *chainUseStruct = (ChainUseStruct*) calloc(1,sizeof(ChainUseStruct));
    //! default
    chainUseStruct->chainUseCount = 0;
    //! create memory.. (the 2nd pass would do this in the future ...)
    chainUseStruct->chainButtonStructArray = (ChainButtonStruct*) calloc(CHAIN_USE_MAX, sizeof(ChainButtonStruct));
    
    //! TEMP .. until parse working ...  will do the same as registerChain does..
    
    return chainUseStruct;
}

//! 1.23.26 Cold, Clear, Mt Nice ... need to use the version above with 5 items ..
//! pointer to the ChainSensors
//! return array of ChainUseStruct after parsing string syntax:   {SENSOR,pin1,pin2}
ChainUseStruct* parseChainSensorString_mainModuleNEW(char *chainUseString)
{
    //! syntax:  sensor,pin1,pin2
    SerialDebug.printf("*** parseChainSensorString_mainModuleNEW:\n%s\n", chainUseString);
    
    //! result
    ChainUseStruct *chainUseStruct = (ChainUseStruct*) calloc(1,sizeof(ChainUseStruct));
    //! default
    chainUseStruct->chainUseCount = 0;
  
    if (!chainUseString || strlen(chainUseString)==0)
    {
        SerialDebug.println("parseSensorString_mainModule: *** No chainUseString or string **");
        return chainUseStruct;
    }
    
    char *rest = NULL;
    char *token;
    int arrayIndex = 0;
    
    //! needed to copy sensorsString.. as the code below broke the callers' value to "https:" .. SIDE EFFECT
#define MAX_CHAIN_STRING 400
    char str[MAX_CHAIN_STRING];
    char strCopy[MAX_CHAIN_STRING];
    if (strlen(chainUseString) > MAX_CHAIN_STRING)
    {
        SerialDebug.printf("ChainUseString length too big %d\n", strlen(chainUseString));
        return NULL;
    }
    strcpy(str,chainUseString);
    strcpy(strCopy,chainUseString);
    
    //! resulting array
    ChainButtonStruct *chainButtonStructs;
    
    //! number of sensors
    int numChainStructs;
    
    //char *str = "BC,1,1,feed,,$localhost,
    //             BC,1,2,feed,,,
    //             BC,2,2,SM_Matrix,6,$dev,
    //             BC,2,3, togglesocket,,M5AtomSocket,
    //! 2 pass
    for (int whichPass = 0; whichPass<2; whichPass++)
    {
#define secondPass (whichPass == 1)
        //! for this we have MAX = 6
        int max = 6;
        
        if (secondPass)
        {
            //! create the storage
            numChainStructs = arrayIndex / max;
            strcpy(str,strCopy);
            SerialLots.printf(" ** Create sensors %d\n", numChainStructs);
            chainButtonStructs= (ChainButtonStruct*) calloc(numChainStructs,sizeof(ChainButtonStruct));
            chainUseStruct->chainButtonStructArray = chainButtonStructs;

        }
        
        //! reset arrayIndex (which will be increments of 3)
        arrayIndex = 0;
        
        //! 2 pass, first count, 2nd parseSensorString_mainModule
        
        //! look for tokens, comma seperated (SEEMS cannot be null or empty .. ?? eg  ,,,,
        for (char *token= strtok(str,","); token!= NULL; token= strtok(NULL, ","))
        {
            int indexInSensor = arrayIndex / max;
            int indexInArray = arrayIndex % max;
            SerialLots.printf("%d Token[%d] = %s\n",indexInArray, indexInSensor,  token);
            
            if (!secondPass)
            {
                //! continut loop .. but increment (since code below wont' increment)
                arrayIndex++;
                continue;
                
            }
            
            //! which of the max is this..
            //! "BC,1,1,feed,,$localhost,   <string> <int> <int> <string> <string> <string>
            switch (indexInArray)
            {
                case 0: // <string>
                {
                    //TODO: strip the spaces of this token
                    chainButtonStructs[indexInSensor].classType = copyString_mainModule(token);
                }
                    break;
                case 1: // <int>
                {
                    int num = atoi(token);
                    chainButtonStructs[indexInSensor].buttonNumber = num;
                }
                    break;
                case 2: // <int>
                {
                    int num = atoi(token);
                    chainButtonStructs[indexInSensor].pressKind = num;
                }
                    break;
                case 3: // <string>   setString
                    chainButtonStructs[indexInSensor].setString = copyString_mainModule(token);
                    break;
                case 4: // <string> valString
                    chainButtonStructs[indexInSensor].valString = copyString_mainModule(token);
                    break;
                case 5: // <string> device
                    chainButtonStructs[indexInSensor].deviceString = copyString_mainModule(token);
                    
                {
                    //! update the chain use string ..
                    char chainUseSample[200];
                    sprintf(chainUseSample,"CHAIN_USE: %s press=%d btn=%2d set= %s, val=%s dev=%s",chainButtonStructs[indexInSensor].classType, chainButtonStructs[indexInSensor].pressKind, chainButtonStructs[indexInSensor].buttonNumber, chainButtonStructs[indexInSensor].setString, chainButtonStructs[indexInSensor].valString, chainButtonStructs[indexInSensor].deviceString);
                    chainButtonStructs[indexInSensor].chainUse = copyString_mainModule(chainUseSample);
                }
                    break;
            }
            
            //! only incremnet arrayIndex when
            arrayIndex++;
        }
    }
    
    //! update the result (storage already created)
    chainUseStruct->chainUseCount = numChainStructs;
    chainUseStruct->chainButtonStructArray = chainButtonStructs;
    
    SerialDebug.printf("parseSensorString_mainModule: ** sensors->count = %ld\n", numChainStructs);
    
    return chainUseStruct;
}

//! 2.9.26 TODO .. use this for good ...
//! just try parsing for now..
void testParse()
{
    
    //! 200 fixed problem..
    char str[400];
    //! needs spaces between "," or won't find them .. FIX THIS ..
    //strcpy(str,"BC,1,1,feed, ,$localhost, BC,1,2,feed, , , BC,2,2,SM_Matrix,6,$dev, BC,2,3, togglesocket, ,M5AtomSocket");
    strcpy(str,(char*)"BC,0,0,feed, ,$localhost,BC,0,2,feed, , ,BC,1,0,feed, ,$localhost,BC,1,2,feed, , ,BC,2,2,SM_MatrixCell,6,$dev,BC,2,1,togglesocket, ,M5AtomSocket,BC,2,0,togglesocket, ,M5AtomSocket,JC,3,0,feed, ,$localhost,JC,3,100,feed, , ,JC,3,101,togglesocket, ,M5AtomSocket,JC,3,102,feed, , ,JC,3,103,$SMART,QHmwUurxC3.1703806697279,");

    //! parse it
    ChainUseStruct *chainUseStruct = parseChainSensorString_mainModuleNEW(str);
    //! print it
    printChainSensors_mainModule(chainUseStruct);

    SerialDebug.println("DONE testParse");
    
    //! bombing after this..
}

#pragma mark CHAIN
//!TODO  add code here
//! 1.23.26 storage for the chain sensors EPROM value ..
//!PREFERENCE_CHAIN_SENSORS_SETTING
//!FIRST do the registration maniually .. LATER .. parseSensros
void initChainSensorStringsFromEPROM_mainModule()
{
    strcpy(_chainSensorsEPROM, getPreference_mainModule(PREFERENCE_CHAIN_SENSORS_SETTING));
    SerialDebug.printf("initSensors:initChainSensorStringsFromEPROM_mainModule = '%s'\n", _chainSensorsEPROM);

    //! Parse to the global..
    //! TODO .. finish this (it should parse the structure like testParse() and do all the registerChain calls..
    _chainUseStruct = parseChainSensorString_mainModule(_chainSensorsEPROM);
    
    //! add this which is Button -0 (not a Chain button).
    //! So if a message comes in for button 0 .. it can find this one..
    registerChain_mainModule("BC", 0, BUTTON_SHORT_PRESS, "feed", "", "$localhost" );
    registerChain_mainModule("BC", 0, BUTTON_DOUBLE_PRESS,"feed", "", "" ); //wifi feed, or BLE if connected as well with the !dev

    //TODO: long press is the AP mode...
    //registerChain_mainModule("BC", 0, BUTTON_LONG_PRESS, clean credentials ... eg.  "x" local .. something..

    //! TODO: the following would use the testParse
    //! has to be after the _chainUseStruct global is set...
    if (true)
    {
        //! idea is you define the button and the press kind
        //! MatrixCell is Column Major  1,4, 7 // 2,5,8 // 3,6,9
        registerChain_mainModule("BC", 1, BUTTON_SHORT_PRESS,  "feed", "", "$localhost" );
        registerChain_mainModule("BC", 1, BUTTON_DOUBLE_PRESS,  "feed", "", "" ); //wifi

       // registerChain_mainModule("BC",   2, BUTTON_SHORT_PRESS,    "SM_MatrixCell", "5", "$dev" );
        registerChain_mainModule("BC",   2, BUTTON_DOUBLE_PRESS,   "SM_MatrixCell", "6", "$dev");
        //{"cmd":"togglesocket","dev":"M5AtomSocket"}
        registerChain_mainModule("BC",   2, BUTTON_LONG_PRESS,     "togglesocket", "", "M5AtomSocket");
        registerChain_mainModule("BC",   2, BUTTON_SHORT_PRESS,    "togglesocket", "", "M5AtomSocket");

        //!joystick buttons..
        //registerChain_mainModule("JC", 3, BUTTON_SHORT_PRESS,  "feed", "", "$localhost" );
        //! 1.24.26 don't know what to do yet ... x,y (or left, top, right, botton ??)
        //registerChain_mainModule("JC", 3, BUTTON_JOYSTICK_LEFT,  "status", "", "" );
        registerChain_mainModule("JC", 3, BUTTON_JOYSTICK_TOP,  "togglesocket", "", "M5AtomSocket");
        registerChain_mainModule("JC", 3, BUTTON_JOYSTICK_RIGHT,  "feed", "", "" );
        //registerChain_mainModule("JC", 3, BUTTON_JOYSTICK_BOTTOM,  "status", "", "" );
        
        //! 2.9.26 try the new smartDefn indirect button
        //! this has a future text with MQTT defined
        registerChain_mainModule("JC", 3, BUTTON_JOYSTICK_BOTTOM,  "$SMART", "QHmwUurxC3.1769990960124", "" );
        //1769990960124&flowCat=MQTT&name=sendMQTT
        
        //!
        //! 2.16.26 runSmart will just send the runSmart, uuid.flownum .. and let someone else run it..
        registerChain_mainModule("JC", 3, BUTTON_SHORT_PRESS,  "runSmart", "QHmwUurxC3.1769990960124", "" );
        //1769990960124&flowCat=MQTT&name=sendMQTT

        //! 2.9.26 try the new smartDefn indirect button
        //! this has a future text with MQTT defined
        registerChain_mainModule("JC", 3, BUTTON_JOYSTICK_LEFT,  "$DOCFOLLOW", "bobsYourUncle", "" );
        //registerChain_mainModule("JC", 3, BUTTON_JOYSTICK_LEFT,  "$DC", "bobsYourUncle", "name" );

        
#define TRY_JC_BUTTON_1
#ifdef  TRY_JC_BUTTON_1
        //! 3.20.26 Pinapple Express Rain, after Deep Snow last weekend,
        //! adding JC to button1 - incase it's the only one there..
        //! This is because the button is known when it's touched .. so it searches numbered button for the Press kind (and joystick is seperate)
        //! TODO: check STORAGE
        registerChain_mainModule("JC", 1, BUTTON_JOYSTICK_TOP,  "togglesocket", "", "M5AtomSocket");
        registerChain_mainModule("JC", 1, BUTTON_JOYSTICK_RIGHT,  "feed", "", "" );
        registerChain_mainModule("JC", 1, BUTTON_JOYSTICK_BOTTOM,  "$SMART", "QHmwUurxC3.1769990960124", "" );
        //! 2.16.26 runSmart will just send the runSmart, uuid.flownum .. and let someone else run it..
        registerChain_mainModule("JC", 1, BUTTON_SHORT_PRESS,  "runSmart", "QHmwUurxC3.1769990960124", "" );
        //! 2.9.26 try the new smartDefn indirect button
        //! this has a future text with MQTT defined
        registerChain_mainModule("JC", 1, BUTTON_JOYSTICK_LEFT,  "$DOCFOLLOW", "bobsYourUncle", "" );
    
#endif
        //! 2.9.26 After Seahawks Superbowl Win
        //! #419 https://github.com/konacurrents/KSQRAvatar/issues/489
        //! decide where the message shows up..
        // <class, buttonNum, pressKind,  $SMART, uuid.flownum, DEV>
        
        //! eg:
        //char *str = "BC,1,1,feed,,$localhost, BC,1,2,feed,,, BC,2,2,SM_Matrix,6,$dev, BC,2,3, togglesocket,,M5AtomSocket, YC, 
    }
    
    //!test...
    testParse();
    
    //! request the chain buttons if SMART
    requestChainSMARTButtons();
    
    SerialDebug.println("done register chain buttons");

}



//! 1.23.26 Clear Day .. puppies, tyler, Clear Snowy Mt Rainier
//!
//! Only 1 setSensorsString now .. will always append
//! unless a null or blank "" string
//! PREFERENCE_CHAIN_SENSORS_SETTING
//! set a sensor val (array of  sensor,pin,pin,sensor,pin,pin...)
void setChainSensorsString_mainModule(char *chainSensorsString)
{
    //! for now .. resetting
    //! 5.17.25
    strcpy(_chainSensorsEPROM, "");
    
    SerialDebug.printf("setChainSensorsString_mainModule(%s)\n", chainSensorsString);
    //! init EPROM
    if (!chainSensorsString || strlen(chainSensorsString)==0)
        strcpy(_chainSensorsEPROM, "");
    else if (strlen(_chainSensorsEPROM) > 0)
    {
        // add a ','
        strcat(_chainSensorsEPROM, ",");
    }
    strcat(_chainSensorsEPROM, chainSensorsString);
    
    //! store in EPROM
    savePreference_mainModule(PREFERENCE_CHAIN_SENSORS_SETTING, _chainSensorsEPROM);
    
    //! Parse to the global..
    _chainUseStruct = parseChainSensorString_mainModule(_chainSensorsEPROM);
    //!printout
    SerialDebug.printf("setSensors:setChainSensorsString_mainModule = %d\n", _chainUseStruct->chainUseCount);
    
}

//! 1.30.26 RAIN, puppies, Tyler looking at me
//! return a non 0 length string " " if nothing or 0 length
char *non0string(char* str)
{
    if (!str)
        return (char*)" ";
    else if (strlen(str) == 0)
        return (char*)" ";
    else
        return str;
}
//! print sensors, passing in a struct
//! also makes a requestChainSMARTButtons() call ..
void printChainSensors_mainModule(ChainUseStruct* chainUseStruct)
{
    if (chainUseStruct)
    {
        //! print it 2 ways, one as the EPROM wouild look, the other are the chain use string
        for (int lap = 0; lap < 2; lap++)
        {
            boolean showAsEPROM = (lap == 0);
            
            //! go through looking for N one (0 based)
            for (int i=0; i< chainUseStruct->chainUseCount; i++)
            {
                //! grab the i'th struct, then look at the button number and pressKind
                ChainButtonStruct* chainButtonStruct = &chainUseStruct->chainButtonStructArray[i];
                if (showAsEPROM)
                {
                    //!   "JC", 3, BUTTON_JOYSTICK_RIGHT,  "feed", "", "" );
                    if (i > 0)
                        SerialDebug.printf(",");
                    SerialDebug.printf("%s,%d,%d,%s,%s,%s (docfollow=%d)", chainButtonStruct->classType, chainButtonStruct->buttonNumber, chainButtonStruct->pressKind, non0string(chainButtonStruct->setString), non0string(chainButtonStruct->valString), non0string(chainButtonStruct->deviceString), chainButtonStruct->isDOCFOLLOW);
                }
                else
                    SerialDebug.printf("%d -> %s\n", i, chainButtonStruct->chainUse?chainButtonStruct->chainUse:(char*)"NULL");
                
#ifdef DO_THIS_LATER
                //! 2.9.26 SuperBowl win, Vonn crash
                //! send the request message again..
                //! This will only be set if the setString wasn't updated ..
                if (strcmp(chainButtonStruct->setString,"$SMART")==0)
                {
                    //! not finalized yes .. so fire off a request again.
                    //! make a request for the UUID ...
                    //! {"set":"getSmartDefn", "val":"uuid.flownum"}
                    sprintf(_mqttMessage_ChainButton, "{'set':'getSmartDefn','val':'%s'}", chainButtonStruct->valString);
                    SerialDebug.println(_mqttMessage_ChainButton);
                    
                    //! send MQTT message requesting a definiition for our SMART button
                    //! send this message
                    sendMessageNoChangeMQTT(_mqttMessage_ChainButton);
                }
#endif
            }
            if (showAsEPROM)
                SerialDebug.println();
        }
        
        //! request the chain buttons if SMART
        requestChainSMARTButtons();
    }
}

#ifdef LATER
//! 2.14.26 WORKS .. dynamic binding .. but now add a cache of last results (would have to reboot to clear cache)
boolean cachedChainDefinition(char *smartAddress)
{
    //! if cached return true
    boolean cached = false;
    //! would be lookup smartAddres to the 'tuple MQTT message'
    //! calll
    /*
    //! //! 2.11.26 set the value for the smartAddress
    //! the val will be parsed..
    void setChainButtonStructSMART(char *valValue);
    
    //! 2.11.26 Lincolns birtyday
    //! will set any buttons with DOCFOLLOW with the smart address specified
    void setChainButtonStructDOCFOLLOW(char *SMARTAddress);
     */
    return cached;
}
#endif

//! 2.9.26 SuperBowl win, Vonn crash
//! send the request message again..
//! This will only be set if the setString wasn't updated ..
//! 2.9.26 Rain. puppies still couple more days
//! call this to ask for SMART Addresses .. when MQTT is connected
//! this looks at all Chain Buttons and if a SMART Address .. requests on MQTT via
//! set: getSmartDefn message
//! TODO: 2.14.26 WORKS .. dynamic binding .. but now add a cache of last results (would have to reboot to clear cache)
void requestChainSMARTButtons()
{
    SerialDebug.println("requestChainSMARTButtons");
    //! go through all chain buttons and see if any are $SMART
    //! if so .. make a MQTT request
    ChainUseStruct* chainUseStruct = getChainUseStruct_mainModule();
    if (chainUseStruct)
    {
        SerialDebug.printf("%d SMARTButtons\n", chainUseStruct->chainUseCount);

        //! go through looking for N one (0 based)
        for (int i=0; i< chainUseStruct->chainUseCount; i++)
        {
            //! grab the i'th struct, then look at the button number and pressKind
            ChainButtonStruct* chainButtonStruct = &chainUseStruct->chainButtonStructArray[i];
            
            //! 2.9.26 SuperBowl win, Vonn crash (5 inch hit gate)
            //! send the request message again..
            //! This will only be set if the setString wasn't updated ..
            if (strcmp(chainButtonStruct->setString,"$SMART")==0)
            {
                char* smartAddress = chainButtonStruct->valString;
                //! 2.14.26 Valentimes day, 46th with Laura Babe
                //! if already cached .. skip the request
                //if (!cachedChainDefinition(smartAddress))
                {
                    //! not finalized yes .. so fire off a request again.
                    //! make a request for the UUID ...
                    //! {"set":"getSmartDefn", "val":"uuid.flownum"}
                    sprintf(_mqttMessage_ChainButton, "{'set':'getSmartDefn','val':'%s'}", smartAddress);
                    SerialDebug.printf("Request(%d, %s) %s\n", i, smartAddress, _mqttMessage_ChainButton);
                    
                    //! send MQTT message requesting a definiition for our SMART button
                    //! send this message
                    sendMessageNoChangeMQTT(_mqttMessage_ChainButton);
                }
            }
        }
    }
}

//! 2.13.26 2 eagles, starting rain, nice ski yesterday
//! add storage for the VALstring so not creating new copy always
#define MAX_SMART_ADDRESS 80
char _buffer_ChainButtonSMARTAddress[MAX_SMART_ADDRESS];
//! return memory that can be used (reused)
char *copyString_SMARTAddress(char* valString)
{
    if (strlen(valString) > MAX_SMART_ADDRESS)
        strcpy(_buffer_ChainButtonSMARTAddress,(char*)"");
    else
        strcpy(_buffer_ChainButtonSMARTAddress, valString);
    return _buffer_ChainButtonSMARTAddress;
}
//! 2.13.26 2 eagles, starting rain, nice ski yesterday
//! add storage for the VALstring so not creating new copy always
#define MAX_USE_SAMPLE 250
char _buffer_ChainButtonUseSample[MAX_USE_SAMPLE];
//! return memory that can be used (reused)
//! namespace future use ...
char *copyString_UseSample(char* valString, char *category)
{
    if (strlen(valString) > MAX_USE_SAMPLE)
        strcpy(_buffer_ChainButtonUseSample,(char*)"");
    else
        strcpy(_buffer_ChainButtonUseSample, valString);
    return _buffer_ChainButtonUseSample;
}

//! 2.12.26 Lincolns birtyday, SKI crystal nice turns
//! will set any buttons with DOCFOLLOW with the smart address specified
//! $SMART, uuid.flow, dev
//! here the smartAddress == UUID.FLOWNUM
//! SMARTAddress is temporary transient storage
void setChainButtonStructDOCFOLLOW(char *SMARTAddress)
{
    SerialDebug.printf("setChainButtonStructDOCFOLLOW: %s\n", SMARTAddress);
    //! go through all chain buttons and see if any are $SMART
    //! if so .. make a MQTT request
    ChainUseStruct* chainUseStruct = getChainUseStruct_mainModule();
    if (chainUseStruct)
    {
        //! go through looking for N one (0 based)
        for (int i=0; i< chainUseStruct->chainUseCount; i++)
        {
            //! grab the i'th struct, then look at the button number and pressKind
            ChainButtonStruct* chainButtonStruct = &chainUseStruct->chainButtonStructArray[i];
            if (chainButtonStruct->isDOCFOLLOW)
            {
                //         registerChain_mainModule("JC", 3, BUTTON_JOYSTICK_BOTTOM,  "$SMART", "QHmwUurxC3.1769990960124", "" );

                SerialDebug.printf("setDOCFOLLOW address=($SMART,%s,)\n",(char*) SMARTAddress);
                //! the SMART Address is when finalized, so the same button can be updated in the future with just the address UUID.FLOW
                chainButtonStruct->SMARTAddress = copyString_SMARTAddress(SMARTAddress);
                //! change the DOCFOLLOW to a SMART item..
                chainButtonStruct->setString = (char*)"$SMART";
                chainButtonStruct->valString = chainButtonStruct->SMARTAddress;
                chainButtonStruct->deviceString = (char*)"";
              
                //! buttonFinalized means we have the MQTT message definition (retrieved from the internet, actually iPhone sends the setSmartDefn message
                //! If finalized then the SMARTAddress is what you want to use, which will
                chainButtonStruct->buttonFinalized = false;
           
                
                //!update the chain use for this one..
                //! eg: "$SMART", "QHmwUurxC3.1769990960124", ""
                char chainUseSample[MAX_USE_SAMPLE];
                sprintf(chainUseSample,(char*)"CHAIN_USE: %s btn=%2d press=%d set= %s, val=%s dev=%s", chainButtonStruct->classType, chainButtonStruct->buttonNumber, chainButtonStruct->pressKind, chainButtonStruct->setString, chainButtonStruct->valString, chainButtonStruct->deviceString);
                //! since DOCFOLLOW all have the same, we can use the same (FOR NOW..
                //! TODO : if the <cat> is used .. then different buffers needed..
                chainButtonStruct->chainUse = copyString_UseSample(chainUseSample, (char*)"");
            }
        }
    }
    //! print it out ...
    printChainSensors_mainModule(getChainUseStruct_mainModule());
    
#ifdef DONE_BY_PRINT
    //! start the process... this sends MQTT message to look for non buttonFinalized smart addresses.
    requestChainSMARTButtons();
#endif
}

//! 1.27.26
//! 7.9.25 reset SENSORS to default
//! "BuzzerSensorClass,23,33,L9110S_DCStepperClass,21,25"
void resetChainSensorToDefault_mainModule()
{
    //UNUSED
}



//!12.19.25 use the Chain
//! 5.3.25 add a central clearing house for defining PIN use
//! central clearing house for all pins used to we can analyze if there are overlaps
//! pin is the actual number, pinName is the local name (eg. IN1_PIN or VIN_PIN).
//! moduleName is the module in the code,
//! isI2C is whether this is a I2C bus (which we aren't using much yet)
//! 1.22.26 add pressKind and classType
void registerChain_mainModule(String classType, int buttonNumber, int pressKind, String setString, String valString, String deviceString)
{
    
    if (_chainUseStruct->chainUseCount >= CHAIN_USE_MAX)
    {
        SerialError.printf("*** ERROR .. too many CHAINS defined ***");
        return;
    }
    
    char chainUseSample[100];
    sprintf(chainUseSample,"CHAIN_USE: %s btn=%2d press=%d set= %s, val=%s dev=%s",classType, buttonNumber,pressKind, setString.c_str(), valString.c_str(), deviceString.c_str());
    SerialDebug.println(chainUseSample);
    
    //! storage is supposed to have been created .. but this line blows ..
    SerialLots.printf("chainButtonStruct = %d, count = %d\n", _chainUseStruct,_chainUseStruct? _chainUseStruct->chainUseCount:-1);
    //! update the n'th version
    //! grab a pointer to the size already created
    //! The array is really contiguous memory .. vs linked list, to the address of the n't array is the pointer to that struct
    //! grab the next entry (chainUseCount)
    ChainButtonStruct *chainbuttonStruct = &_chainUseStruct->chainButtonStructArray[_chainUseStruct->chainUseCount];
    
    //! 1.23.26
    //! 5.3.25 create storage here
    char *chainUse = (char*)calloc(strlen(chainUseSample)+1, sizeof(char));
    strcpy(chainUse, chainUseSample);
    
    //!store globally
    chainbuttonStruct->chainUse = chainUse;
    
    //!store classType
    chainbuttonStruct->classType = createCopy2(classType.c_str());
    
    //! set the buttonNumber for this chain #
    chainbuttonStruct->buttonNumber = buttonNumber;
    
    //! pressKind
    chainbuttonStruct->pressKind = pressKind;
    
    //! set the set for this chain #
    chainbuttonStruct->setString = createCopy2(setString.c_str());
    
    //! set the set for this chain #
    chainbuttonStruct->valString = createCopy2(valString.c_str());
    
    //! set the set for this chain #
    chainbuttonStruct->deviceString = createCopy2(deviceString.c_str());
    
    //! 2.11.26
    chainbuttonStruct->isDOCFOLLOW = false;
    
    //! 2.9.26 superbowl, raining out
    //! fi the set is $SMART then do something ... request the value again .. since otherwise the setString value would be changed
    //! $SMART, UUID.FLOW, <dev>
    if (strcmp(chainbuttonStruct->setString, "$SMART")==0)
    {
        //! set the SMARTAddress
        chainbuttonStruct->SMARTAddress = createCopy(chainbuttonStruct->valString);
        //! not finalized (not BOUND to the MQTT message. the iPhone is the one providing that information ...
        chainbuttonStruct->buttonFinalized = false;
#ifdef DO_THIS_LATER
        //! make a request for the UUID ...
        //! {"set":"getSmartDefn", "val":"uuid.flownum"}
        sprintf(_mqttMessage_ChainButton, "{'set':'getSmartDefn','val':'%s'}", chainbuttonStruct->SMARTAddress);
        SerialDebug.println(_mqttMessage_ChainButton);
        
        //! send MQTT message requesting a definiition for our SMART button
        //! send this message
        sendMessageNoChangeMQTT(_mqttMessage_ChainButton);
#endif
    }
    //! 2.11.26 add the docfollow
    //! $DC,,  <nill args)>
    //! TODO: args:  $DC, <group>, <name> help filter out $DC subsets (context)
    else if (strcmp(chainbuttonStruct->setString, "$DC")==0|| strcmp(chainbuttonStruct->setString, "$DOCFOLLOW")==0)
    {
        //! 2.11.26
        chainbuttonStruct->isDOCFOLLOW = true;
        //! no smart address
        chainbuttonStruct->SMARTAddress = createCopy((char*)"");
        //! but finalized
        chainbuttonStruct->buttonFinalized = false;
    }
    else
    {
        //! no smart address
        chainbuttonStruct->SMARTAddress = createCopy((char*)"");
        //! but finalized
        chainbuttonStruct->buttonFinalized = true;
    }
    
    //! increment chainUseCound
    _chainUseStruct->chainUseCount++;
   
    
}

#ifdef LATER_STANGE
//! NOT USING THIS YET ...
//! 3.15.26 find first button struct for a pressKind (so the joystick can be found)
//!if matched returns defintion otherwises sends a getSmartDefn message over MQTT hoping it shows up here as DOCFOLLOW sends MQTT
ChainButtonStruct* getChainButtonStruct(int pressKind)
{
    SerialDebug.printf("getChainUseStruct(%d)\n", pressKind);
    ChainUseStruct* chainUseStruct = getChainUseStruct_mainModule();
    if (chainUseStruct)
    {
        //! go through looking for N one (0 based)
        for (int i=0; i< chainUseStruct->chainUseCount; i++)
        {
            //! grab the i'th struct, then look at the button number and pressKind
            ChainButtonStruct* chainButtonStruct = &chainUseStruct->chainButtonStructArray[i];
            
            //! i is just the entry .. not the button number
            //! N'th entry
            if (chainButtonStruct->pressKind == pressKind || chainButtonStruct->pressKind == BUTTON_ANY)
            {
                SerialDebug.printf("chainButton press=%d) = %s\n", chainButtonStruct->pressKind, chainButtonStruct->chainUse);
                //! see if the right pressKind
                {
                    SerialDebug.printf("matched button %ld, press=%ld \n", chainButtonStruct->buttonNumber, pressKind);
                    
                    //! 2.9.26 if not finalized .. ask again
                    if (chainButtonStruct->buttonFinalized)
                        //! found one .. go with it
                        return chainButtonStruct;
                    else
                    {
                        //! not finalized yes .. so fire off a request again.
                        //! make a request for the UUID ...
                        //! {"set":"getSmartDefn", "val":"uuid.flownum"}
                        sprintf(_mqttMessage_ChainButton, "{'set':'getSmartDefn','val':'%s'}", chainButtonStruct->valString);
                        SerialDebug.println(_mqttMessage_ChainButton);
                        
                        //! send MQTT message requesting a definiition for our SMART button
                        //! send this message
                        sendMessageNoChangeMQTT(_mqttMessage_ChainButton);
                        
                        //! and no match..
                        return NULL;
                    }
                }
            }
        }
    }
    return NULL;
}
#endif

//! 1.27.26 moved here for generic use .. even by a non chain button
//! 1.22.26 with puppies, cold dry outside
//! get the ChainUseStruct for the buttonNumber (eg 1..9  (no 0)
//! Column Major in the Matrix (but sequential here)
//! return the definition for this 1 based button number (if any)]   //TODO -1
//! pressKind = -1 (matches first one ..)
//! NOTE: pressKind for Joystick is here..
//! 1.28.26 you can ask for BUTTON 0 (which will be the deault for the M5Atom single press (or maybe long press)
//! 2.9.26 if not finlized yet, then request a smart definition.. in case not found yet.
ChainButtonStruct* getChainButtonStruct(int buttonNumber_1based, int pressKind)
{
    SerialDebug.printf("getChainUseStruct(%d, %d)\n", buttonNumber_1based, pressKind);
    ChainUseStruct* chainUseStruct = getChainUseStruct_mainModule();
    if (chainUseStruct)
    {
        //! go through looking for N one (0 based)
        for (int i=0; i< chainUseStruct->chainUseCount; i++)
        {
            //! grab the i'th struct, then look at the button number and pressKind
            ChainButtonStruct* chainButtonStruct = &chainUseStruct->chainButtonStructArray[i];
            
            //! i is just the entry .. not the button number
            //! N'th entry
            if (chainButtonStruct->buttonNumber == buttonNumber_1based)
            {
                SerialDebug.printf("chainButton (%d, press=%d) = %s\n", buttonNumber_1based, chainButtonStruct->pressKind, chainButtonStruct->chainUse);
                //! see if the right pressKind
                if (chainButtonStruct->pressKind == pressKind || chainButtonStruct->pressKind == BUTTON_ANY)
                {
                    SerialDebug.printf("matched button %ld, press=%ld \n", buttonNumber_1based, pressKind);
                    
                    //! 2.9.26 if not finalized .. ask again
                    if (chainButtonStruct->buttonFinalized)
                        //! found one .. go with it
                        return chainButtonStruct;
                    else
                    {
                        //! not finalized yes .. so fire off a request again.
                        //! make a request for the UUID ...
                        //! {"set":"getSmartDefn", "val":"uuid.flownum"}
                        sprintf(_mqttMessage_ChainButton, "{'set':'getSmartDefn','val':'%s'}", chainButtonStruct->valString);
                        SerialDebug.println(_mqttMessage_ChainButton);

                        //! send MQTT message requesting a definiition for our SMART button
                        //! send this message
                        sendMessageNoChangeMQTT(_mqttMessage_ChainButton);
                        
                        //! and no match..
                        return NULL;
                    }
                }
            }
        }
    }
    return NULL;
}

#ifdef NOT_THIS_ONW
//! 2.9.26 Superbowl Win, Vonn Crash
//! get the chain use struct for the SMART address
ChainButtonStruct* getChainButtonStructSMART(char *SMARTAddress)
{
    SerialDebug.printf("getChainUseStructSMART(%s)\n", SMARTAddress);
    ChainUseStruct* chainUseStruct = getChainUseStruct_mainModule();
    if (chainUseStruct)
    {
        //! go through looking for N one (0 based)
        for (int i=0; i< chainUseStruct->chainUseCount; i++)
        {
            //! grab the i'th struct, then look at the button number and pressKind
            ChainButtonStruct* chainButtonStruct = &chainUseStruct->chainButtonStructArray[i];
            
            //! i is just the entry .. not the button number
            //! N'th entry
            if (strcmp(chainButtonStruct->SMARTAddress, SMARTAddress)==0)
            {
                SerialDebug.printf("Found chainButton %s for SMART = %s\n", chainButtonStruct->classType, SMARTAddress);
                //! found one .. go with it
                return chainButtonStruct;
                
                
            }
        }
    }
    return NULL;
}
#endif

//! 2.11.26 set the value for the smartAddress
//! the val will be parsed..
//! valValue is the address used:  UUID.FLOW(set,val,dev)
//! NOTE: valValue is transient storage
void setChainButtonStructSMART(char *valValue)
{
    SerialDebug.printf("setChainButtonStructSMART(%s)\n", valValue);
    ChainUseStruct* chainUseStruct = getChainUseStruct_mainModule();
    if (chainUseStruct)
    {
        //! 2.,10.26 parsing works .. no extra spaces ..
        //! UUID.FLOW(set,val,dev)
        SMStruct sm = parseSMAddress(valValue);
        char *SMARTAddress = sm.smAddress;
        SerialDebug.printf("SM = '%s', '%s', '%s', '%s'\n", sm.smAddress, sm.setString, sm.valString, sm.devString);

        
        //! go through looking for N one (0 based)
        for (int i=0; i< chainUseStruct->chainUseCount; i++)
        {
            //! grab the i'th struct, then look at the button number and pressKind
            ChainButtonStruct* chainButtonStruct = &chainUseStruct->chainButtonStructArray[i];
            
            //! i is just the entry .. not the button number
            //! N'th entry
            if (strcmp(chainButtonStruct->SMARTAddress, SMARTAddress)==0)
            {
                SerialDebug.printf("Found chainButton %s for SMART = %s\n", chainButtonStruct->classType, SMARTAddress);
                //! found one .. go with it
                
                //! 2.9.26 works.. but there might be multiple with same SMART address
                //! OOPS .. the getChainButtonStructSMART .. looks for matches .. this should be returning multiple
                //! BUT for now .. only the first one.
                //! NOTE: this is a chain button with a smart address .. which can be re-bound through this message..
                if (chainButtonStruct)
                {
                    SerialDebug.printf("FOUND chainButtonSMART(%s, %d) =  %s", SMARTAddress, chainButtonStruct->pressKind, chainButtonStruct->chainUse);
                    
                    //! for now .. set to the ToggleSocket
                    chainButtonStruct->setString = sm.setString;
                    chainButtonStruct->valString = sm.valString;
                    chainButtonStruct->deviceString = sm.devString;
                    chainButtonStruct->buttonFinalized = true;
                    //!update the chain use for this one..
                    char chainUseSample[250];
                    sprintf(chainUseSample,(char*)"CHAIN_USE: %s btn=%2d press=%d set= %s, val=%s dev=%s", chainButtonStruct->classType, chainButtonStruct->buttonNumber, chainButtonStruct->pressKind, chainButtonStruct->setString, chainButtonStruct->valString, chainButtonStruct->deviceString);
                    chainButtonStruct->chainUse = copyString_mainModule(chainUseSample);
                    //! printout
                    printChainSensors_mainModule(getChainUseStruct_mainModule());
                    
                }
            }
        }
    }
}
      
        //! 1.22.26 Dead Morning Dew
//! process the given button
//! 1.24.26 NOTE: this doesn't care about button press or joystick .. it's just run that button
void processChainButtonStruct(ChainButtonStruct* chainButtonStruct)
{
    if (chainButtonStruct)
    {
        SerialDebug.printf("processChainUseStruct: %s, %s\n", chainButtonStruct->setString, chainButtonStruct->valString);
        char *deviceName = chainButtonStruct->deviceString;
        boolean isLocalHost = false;
        boolean useCurrentDevice = false;
        boolean nullValString = false;
        if (!chainButtonStruct->valString || strlen(chainButtonStruct->valString) == 0)
            nullValString = true;
        if (deviceName && strlen(deviceName) > 0)
        {
            //! 1.24.26 look at $options
            switch (deviceName[0])
            {
                case '$':
                {
                    //! special
                    if ( strcmp(deviceName,"$localhost")==0)
                        isLocalHost = true;
                    else if (strcmp(deviceName, "$dev")==0)
                        useCurrentDevice = true;
                }
                    break;
            }
            
        }
        else
            deviceName = NULL;


        if (isLocalHost && strcmp(chainButtonStruct->setString,"feed")==0)
        {
            if (isConnectedBLEClient())
            {
                //! 4.3.26 if BLE connected .. just do that..
                
                //!send a BLE feed command as we are connected
                sendFeedCommandBLEClient();
            }
            else
            {
                //! feed ..
                ///feed always  (done after the code below..)
                main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
                //! This will send BLE to our connected device ..
                //!
            }
           
        }
#define TRY_FEED_BLE_WIFI
        //! 4.3.26 Off to the Moon
        //! if "feed" then send via BLE and WIFI
#ifdef TRY_FEED_BLE_WIFI
        else if (strcmp(chainButtonStruct->setString,"feed")==0 &&    //! 8.16.25 BLE CLIENT
                 //!9.30.22  IF SET .. send a feed but to all devices except ours and our pair (if any)
                 //! uses new wildcard syntax either  ! OUR NAME  [ & ! OUR_CONNECTED_NAME
                 getPreferenceBoolean_mainModule(PREFERENCE_SENDWIFI_WITH_BLE) && isConnectedBLEClient())
                 
        {
            SerialDebug.println("*** feed and BLE connected **");
            ///feed always  (done after the code below..)
            main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
        }
#else
#endif

        else
        {
            if (nullValString)
            {
                //! create a set/val message
                //! TODO; device, etc.. topic
                ///eg. registerChain_mainModule("BC", 1, BUTTON_DOUBLE_PRESS,  "feed", "", "" ); //wifi

                if (deviceName)
                    sprintf(_mqttMessage_ChainButton, "{'cmd':'%s','dev':'%s'}", chainButtonStruct->setString, deviceName);
                else
                    sprintf(_mqttMessage_ChainButton, "{'cmd':'%s'}", chainButtonStruct->setString);
                
            }
            else
            {
                //! create a set/val message
                //! TODO; device, etc.. topic
                if (deviceName)
                    sprintf(_mqttMessage_ChainButton, "{'set':'%s','val':'%s','dev':'%s'}", chainButtonStruct->setString, chainButtonStruct->valString, deviceName);
                else
                    sprintf(_mqttMessage_ChainButton, "{'set':'%s','val':'%s'}", chainButtonStruct->setString, chainButtonStruct->valString);
                
            }
            //! send this message
            sendMessageNoChangeMQTT(_mqttMessage_ChainButton);
        }
        //!
        //! idea is you define the button and the press kind
        //! MatrixCell is Column Major  1,4, 7 // 2,5,8 // 3,6,9
        //registerChain_mainModule("ButtonClass",   1, BUTTON_SHORT_PRESS,  "SM_MatrixCell", "15", "$dev" );
        //registerChain_mainModule("ButtonClass",   1, BUTTON_LONG_PRESS,   "SM_MatrixCell", "6", "$dev");
        //registerChain_mainModule("JoystickClass", 2, BUTTON_SHORT_PRESS,  "feed", "", "$localhost" );
        
    }
}
