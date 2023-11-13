//! \link MainModule
#include "../../Defines.h"
//#include "MainModule.h"


//! reads the preferences. Save is everytime the savePreference is called
void readPreferences_mainModule();
//!testing..
boolean _stopAllProcessing = false;
//!stop all loops... while OTA working..
void stopProcessesForOTAUpdate_mainModule()
{
    _stopAllProcessing = true;
}
//!restart all loops... while OTA working..
void restartProcessesForOTAUpdate_mainModule()
{
    _stopAllProcessing = false;
}
//! if stopped
boolean stopAllProcesses_mainModule()
{
    //! try to disconnect..
    //disconnect_BLEClientNetworking();
   // return false; //
    return _stopAllProcessing;
}

//****** GLOBALS (shows how much memory is allocated.. )
//!the parameter being sent to those commands passing an argument
char _asyncParameter[500];
//!global to store credentials when ever they are stored..
char _JSONStringForWIFICredentials[200];
//!global for others to use..
#define MESSAGE_STORAGE_MAX 400
char _messageStorage[MESSAGE_STORAGE_MAX];
//!status string (URL query format)
char _fullStatusString[300];
//! current smMode
char _smMode_MainModule[10];
//! saved deviceName storage..
char _deviceNameSave[50];

//!store a big message  #MSG_START .. #MSG_END
boolean _waitingForBigMessageEnd = false;
char _bigMessage[500];
//!saved BLE connected name 8.26.22
char _connectedBLEDeviceName[50];
//!full: ""Name: PTFeeder:HowieFeeder, Address: 7c:9e:bd:48:af:92, serviceUUID: 0xdead"
char _fullBLEDeviceName[100];
//!full: ""Address: 7c:9e:bd:48:af:92, serviceUUID: 0xdead"
char _connectedBLEDeviceAddress[50];

//!init globals strings
void initGlobals_mainModule()
{
    //!NOTE: this is definitely needed, as grabbing strings willy nilly can bomb or corrupt stuff..
    strcpy(_connectedBLEDeviceName,(char*)"");
    strcpy(_bigMessage,(char*)"");
    strcpy(_deviceNameSave,(char*)"");
    strcpy(_smMode_MainModule,(char*)"");
    strcpy(_fullStatusString,(char*)"");
    strcpy(_messageStorage,(char*)"");
    strcpy(_JSONStringForWIFICredentials,(char*)"");
    strcpy(_asyncParameter,(char*)"");
    strcpy(_fullBLEDeviceName,(char*)"");
    
}

//THIS IS the setup() and loop() but using the "component" name, eg MQTTNetworking()
//! called from the setup()
void setup_mainModule()
{
    SerialMin.println("setup_mainModule");
#ifdef BOOTSTRAP_CLEAN
    //!cleans the EPROM .. if things are really not working..
    cleanEPROM_mainModule();
#endif
    
    //test...
    //! setup for the token parser (really just for testing)  -- an
    setup_tokenParser_mainModule();
    
    //!init globals like strings (but nothing that needs preferences)
    initGlobals_mainModule();
    
    //!read the preferences from EPROM
    readPreferences_mainModule();
    
    //!must initialize the structs NOTE: this has to ber AFTER the preferences are read in..
    initModelStructs_ModelController();
    
    //!a first time feature .. to get EPROM changed to a different default
    if (getPreferenceBoolean_mainModule(PREFERENCE_FIRST_TIME_FEATURE_SETTING))
    {
        
        //!7.12.22
        //!  {'dev':'m5",'cmd':'bleserveron'}  or bleserveroff  will work later..
        //!#issue 116 .. turn off the BLE_Server for the M5
        SerialMin.println("FIRST TIME TURNING off the BLE_SERVER for the M5");
#ifdef ESP_M5
#ifdef ATOM_QRCODE_MODULE
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE,  true); //false);
        // 8.28.23 .. not doing this anymore..
#else
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE,  false);

#endif
#else
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE, true);
#endif
        savePreferenceBoolean_mainModule(PREFERENCE_FIRST_TIME_FEATURE_SETTING, false);

    }
    
}

#if defined(ESP_M5_CAMERA) || defined(ESP_32)
//!a couinter to slow down the loop doing things..
int _mainLoopCounter = 0;
#endif

//! called for the loop() of this plugin
void loop_mainModule()
{
    //no op..
    
#if defined(ESP_M5_CAMERA) || defined(ESP_32)
    //!only do this menu update if there is no DisplayModule running the MVC loop
    
    if (_mainLoopCounter++ > 30)
    {
        //! 9.2.22 new .. update the timer. This is on blank screen as well .. so don't check _semanticMarkerShown
        updateMenuState(timerModel);
        
        _mainLoopCounter = 0;
    }
    
#endif

}


//! 5.3.22 added a feed count approach..  (NOTE: there is a _feedCount in Dispence.cpp ... and no linker error!!! )
int _feedCount_mainModule = 0;
//!returns the max for this feeder
int feedCountMax_mainModule()
{
    int max = 16;
    switch (getFeederType_mainModule())
    {
        case STEPPER_IS_UNO:
            max = 16;
            break;
        case STEPPER_IS_MINI:
            max = 55;
            break;
        case STEPPER_IS_TUMBLER:
            max = 50;
            break;
    }
    return max;
}
//!feedcount info..
int getFeedCount_mainModule()
{
    return _feedCount_mainModule;
}

//!get the feeder type (Sepper 1,2,3 ...)
int getFeederType_mainModule()
{
    int kind = getPreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE);
    return kind;
}

//!increments .. and if MAX goes to 0 -- and sends a message on MQTT
void incrementFeedCount_mainModule()
{
    SerialTemp.println("incrementFeedCount_mainModule");
    
    _feedCount_mainModule++;
    if (_feedCount_mainModule >= feedCountMax_mainModule())
    {
        // send a message.
#ifdef USE_MQTT_NETWORKING
        //note needs # or won't send..
        //!NOTE: don't send "feed" as it might trigger a FEED ...
        sendMessageMQTT((char*)"#count reached .. resetting");
#endif
        _feedCount_mainModule = 0;
    }
    
    //! called by the feed operation to say the device is still running.. and count it as a button click.
    //! Issue #145  8.8.22
    refreshDelayButtonTouched_MainModule();
    
}
//! sets the feed count max
void resetFeedCount_mainModule()
{
    _feedCount_mainModule = 0;
}

//!check if the string contains the other string. This is a poor man's grammer checker
bool containsSubstring(String message, String substring)
{
    if (substring.length() == 0 )
        return false;
    bool found = strstr(&message[0], &substring[0]);
    SerialLots.printf("containsSubstring-%d - %s in: %s\n", found, &substring[0],  &message[0]);
    return found;
}

#ifdef USE_MQTT_NETWORKING
#include "../MQTTModule/MQTTNetworking.h"
#endif
#ifdef USE_BLE_SERVER_NETWORKING
#include "../BLEServerModule/BLEServerNetworking.h"
#endif
#ifdef USE_BLE_CLIENT_NETWORKING
#include "../BLEClientModule/BLEClientNetworking.h"
#endif
#ifdef USE_BUTTON_MODULE
#include "../ButtonModule/ButtonModule.h"
#endif

//! New RegisterCallback that works across a number of callback modules
//!
//! callbacksModuleId
#define CALLBACKS_MQTT          0
#define CALLBACKS_BUTTON_MODULE 1
#define CALLBACKS_BLE_CLIENT    2
#define CALLBACKS_BLE_SERVER    3


//!register the callback based on the callbackType. use the callbacksModuleId for which one..
void registerCallbackMain(int callbacksModuleId, int callbackType, void (*callback)(char*));
//!performs the indirect callback based on the callbackType
void callCallbackMain(int callbacksModuleId, int callbackType, char *message);


//!make sure these are the number of callbacks.. 0..n
//! These values are from the respective .h of the modules
#define CALLBACKS_MAX_MQTT          MQTT_MAX_CALLBACKS
#define CALLBACKS_MAX_BUTTON_MODULE MAX_CALLBACKS_BM
#define CALLBACKS_MAX_BLE_CLIENT    BLE_CLIENT_MAX_CALLBACKS
#define CALLBACKS_MAX_BLE_SERVER    BLE_SERVER_MAX_CALLBACKS


//!example callback
void dummyCallbackMain(char *message)
{
    SerialLots.printf("No callback defined .. %s\n", message);
}

//tricky getting array of arrays (or pointers to arrays), without dynamic memory.. we are going to use calloc()
//https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/
//https://www.tutorialspoint.com/cprogramming/c_pointer_to_pointer.htm
//https://stackoverflow.com/questions/5573302/pointer-to-an-array-of-function-pointers
//!the array of callback functions
//* this means there are arrays of N pointers to functions that take 1 parameter (char*).


//! the max of each module .. hard coded (or 0 if module not there)
int _callbacksFunctionsMAXS[CALLBACKS_MODULE_MAX];

//!flag for initializing if not yes
boolean _callbacksInitializedMain = false;

//a pointer to a callback function that takes (char*) and returns void
typedef void (*callbackSignature)(char *);
//callbackSignature fpar[2] = {&dummyCallbackMain, &dummyCallbackMain};
      
//!return dyamically created array of max
callbackSignature * createMemory(int max)
{
    SerialLots.printf("creatingCallbacks %d size: %d\n", max, sizeof(callbackSignature *));
    callbackSignature *callbackArray = (callbackSignature *)calloc(max,sizeof(callbackSignature *));
    for (int i=0; i< max; i++)
    {
        callbackArray[i] = &dummyCallbackMain;
    }
    return callbackArray;
}
//!array of known size (CALLBACKS_MODULE_MAX) of callbackSignatures
callbackSignature *_allCallbacks[CALLBACKS_MODULE_MAX];

//!init the callbacks to dummy callbacks
void initCallbacksMain()
{
    if (!_callbacksInitializedMain)
    {
        _callbacksInitializedMain = true;
        
        // default to 0 callbacks.. in case module not there..
        for (int i = 0; i < CALLBACKS_MODULE_MAX; i++)
        {
            _callbacksFunctionsMAXS[i] = 0;
        }
        //! only place for #ifdef  (NOTE some can be 0 based on the #ifdef module not being included..
#ifdef USE_MQTT_NETWORKING
        _callbacksFunctionsMAXS[CALLBACKS_MQTT] = CALLBACKS_MAX_MQTT;
#endif
#ifdef USE_BLE_SERVER_NETWORKING
        _callbacksFunctionsMAXS[CALLBACKS_BLE_SERVER] = CALLBACKS_MAX_BLE_SERVER;
#endif
#ifdef USE_BLE_CLIENT_NETWORKING
        _callbacksFunctionsMAXS[CALLBACKS_BLE_CLIENT] = CALLBACKS_MAX_BLE_CLIENT;
#endif
#ifdef USE_BUTTON_MODULE
        _callbacksFunctionsMAXS[CALLBACKS_BUTTON_MODULE] = CALLBACKS_MAX_BUTTON_MODULE;
#endif
        // initialize the dummy callbacks
        for (int i = 0; i < CALLBACKS_MODULE_MAX; i++)
        {
            //first determine how many are in the array..
            int max = _callbacksFunctionsMAXS[i];
            //then create the space which are (void *) pointers
            _allCallbacks[i] = createMemory(max);
        }
    }
}

//!register the callback based on the callbackType. use the callbacksModuleId for which one..
void registerCallbackMain(int callbacksModuleId, int callbackType, void (*callback)(char*))
{
    //init if not already..
    initCallbacksMain();
    int max = _callbacksFunctionsMAXS[callbacksModuleId];

    if (callbackType < 0 || callbackType >= max)
    {
        SerialError.println("#### Error outside callback range");
    }
    else
    {
        // array of arrays (or array of pointer to another array)
        _allCallbacks[callbacksModuleId][callbackType] = callback;
    }
}
//!performs the indirect callback based on the callbackType
void callCallbackMain(int callbacksModuleId, int callbackType, char *message)
{
    //init if not already..
    initCallbacksMain();
    int max = _callbacksFunctionsMAXS[callbacksModuleId];
    
    if (callbackType < 0 || callbackType >= max)
    {
        SerialError.println("#### Error outside callback range");
    }
    else {
        void (*callbackFunction)(char *) = _allCallbacks[callbacksModuleId][callbackType];
        (*callbackFunction)(message);
    }
}

//!whether the string is TRUE, ON, 1
boolean isTrueString_mainModule(String valCmdString)
{
    return  valCmdString.equalsIgnoreCase("on") ||
    valCmdString.equalsIgnoreCase("1") ||
    valCmdString.equalsIgnoreCase("true");
};

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
void messageSetVal_mainModule(char *setName, char* valValue)
{
    SerialCall.printf("messageSetVal(%s,%s)\n", setName, valValue);
    // THE IDEA WOULD be a callback is avaialble..
    //FOR now.. just ifdef
#ifdef ATOM_QRCODE_MODULE
    messageSetVal_ATOMQRCodeModule(setName, valValue);
#endif
}
//!TODO: have a callback regist approach

#ifdef USE_MQTT_NETWORKING
//!example callback: but the scope would have the pCharacteristic defined, etc..
//!This is passed just before the setupMQTTNetworking() is called..
void feedMessageCallback(char *message)
{
    SerialTemp.print("feedMessageCallback_main: ");
    SerialTemp.println(message);
    char * rxValue = (char*)"c";  //feed  (s and 0 work too)  MIGHT NOT BE WORKING IF MESSAGE ARRIVES VIA MQTT (and we want to use BLE)
    //!sends this single character to the StepperModule
    processClientCommandChar_mainModule(rxValue[0]);
    
    //!increment the feed count .. TODO .. see if this is good place..
//    incrementFeedCount_mainModule();

    //!the FEED was send over MQTT, now what to do?
    //!  WIth the StepperModule -- it just performs the feeding internalls
    //! if using the BLE_CLIENT then we have the potential to send to over BLE
    //! to the server (another feeder, GEN3 or ESP32).
    //! The issue is that ESP32 feeders are receiving the same MQTT message (usually unless MQTT turned off
    //! So the gatewayMode can be set (or in the EPROM) using {'cmd':'gatewayOn'}
    if (getPreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE))
    {
#ifdef USE_BLE_CLIENT_NETWORKING

        SerialTemp.print("Paired Device: ");
        SerialTemp.println(getPreference_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING));
        
        SerialTemp.printf("Gateway: Feeding over the BLE CLIENT network .. if connected: %d\n",isConnectedBLEClient());
        if (isConnectedBLEClient())
        {
            //!send the feed over the BLE client (if connected)
            sendFeedCommandBLEClient();
        }
        SerialLots.println("after send feed via ble");
#endif
    }
}
#endif //USE_MQTT_NETWORKING

//!moved here 4.25.22 (entirely from ESP_IOT.ino)

//! April 8, 2022
//! set that the credentials are set. Called from the MQTT after processJSONMessage() found things were good..
void main_credentialsUpdated()
{
    // set the done in the WI
    SerialDebug.println(" *** main_credentialsUpdated .. setDoneWIFI_APModuleFlag **");
#ifdef USE_WIFI_AP_MODULE
    setDoneWIFI_APModuleFlag(true);
#endif
}
//NOTE: #else not there so this won't link.. on purpose

/*******************************BLE Server*************************************/
//!The callback for "onWrite" of the bluetooth "onWrite'
void onBLEServerCallback(char *message)
{
    SerialCall.print("onWriteBLEServerCallback: ");
    SerialCall.println(message);
    
}
//!  The callback for "status messages" of the bluetooth
void onStatusMessageBLEServerCallback(char *message)
{
    SerialCall.print("onStatusMessageBLEServerCallback");
    SerialCall.print(message);
    //! 2.2.22  send this over bluetooth..  TODO.
    //
}

//! The FINISH of  callback for "onWrite" of the bluetooth "onWrite'
void onWriteBLEServerCallbackFinish(char *message)
{
 
    SerialDebug.print("onWriteBLEServerCallback_main: ");
    SerialDebug.println(message);
    
    //!client never sends these wrapped messages of length 1, so process normally..
    //!gets us out of potential infinite wait
    if (strlen(message) == 1)
        _waitingForBigMessageEnd = false;
    
    if (strcmp(message, "#MSG_START")==0)
    {
        _waitingForBigMessageEnd = true;
        strcpy(_bigMessage,"");
        return;
    }
    else if (_waitingForBigMessageEnd)
    {
        if (strcmp(message, "#MSG_END")==0)
        {
            _waitingForBigMessageEnd = false;
            //! we are done.. send big message to ourself..
            onWriteBLEServerCallback(_bigMessage);
        }
        else
        {
            strcat(_bigMessage,message);
            return;
        }
    }
    _waitingForBigMessageEnd = false;
    
    //!Issue: the message is arriving via BLE, but if we call the MQTT process, it might call us back to send
    //!the message over BLE. This hangs things up!
    //!So short optimization for 'feed' will be to look for cmd and feed in message {cmd:feed}
    //!8.19.22  issue #162
    if (containsSubstring(message,"cmd"))
    {
        if (containsSubstring(message,"feed"))
        {
            message = (char*)"s";
        }
        else if (containsSubstring(message,"buzzOn"))
        {
            message = (char*)"B";
        }
        else if (containsSubstring(message,"buzzOff"))
        {
            message = (char*)"b";
        }

    }
    
#ifdef USE_MQTT_NETWORKING
    //This should be a 'register' command..
    
    //!the MQTTNetwork.processJSONMessage()
    //!will return true if the message was JSON and was processes, false otherwise.
    if (processJSONMessageMQTT(message, NULL))
    {
        //!processed by the MQTTNetworking code..
#ifdef USE_BLE_SERVER_NETWORKING
        sendBLEMessageACKMessage();
#endif
        //pCharacteristic->setValue(0x01);  //??  This is the acknowlege(ACK) back to client.  Later this should be contigent on a feed completed
    }
    else
    {
        //!perform feed...
        SerialDebug.printf("Perform BLE Command '%s'\n", message);
        
        char cmd = message[0];
        
        //OOOPS.. we need the ProcessClientCmd .. for other than just step..
        // for sensors..
        
        //!look for ** sensor commands..
        processClientCommandChar_mainModule(cmd);
        
#ifdef USE_BLE_SERVER_NETWORKING
        sendBLEMessageACKMessage();
#endif
        
    }
    SerialDebug.println("*********");
    
#endif
} //onWriteBLEServerCallback

//! The callback for "onWrite" of the bluetooth "onWrite'
void onWriteBLEServerCallback(char *message)
{
    //TEST:
#define TRY_ASYNC_BLE_PROCESSING
#ifdef TRY_ASYNC_BLE_PROCESSING
    
    //!send an async call with a string parameter. This will set store the value and then async call the command (passing the parameter)
    //!These are the ASYNC_CALL_PARAMETERS_MAX
    main_dispatchAsyncCommandWithString(ASYNC_JSON_MESSAGE_PARAMETER, message);
#else
    
    //! The callback for "onWrite" of the bluetooth "onWrite'
    onWriteBLEServerCallbackFinish(char *message);
#endif
}

//!take a picture (calls the camera module).. what to do with picture??? TODO
void takePicture_MainModule()
{
#ifdef USE_CAMERA_MODULE
    takePicture_CameraModule();
#endif
}

//! called by the feed operation to say the device is still running.. and count it as a button click.
void refreshDelayButtonTouched_MainModule()
{
#ifdef USE_BUTTON_MODULE
    //!calls the button processing control
    refreshDelayButtonTouched_ButtonModule();
#endif
}
//! 3.28.22 .. implemented in ESP_IOT.ino
boolean _asyncCallOTAUpdateFlag = false;
//!performs an async OTA update
boolean asyncCallOTAUpdate()
{
    return _asyncCallOTAUpdateFlag;
}
//!sets the async OTA flag (for next loop)
void setAsyncCallOTAUpdate(bool flag)
{
    _asyncCallOTAUpdateFlag = flag;
}

//!returns if the paired device is not NONE
char * getPairedDevice_mainModule()
{
    return  getPreference_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING);
}

//!returns if the paired device is not NONE
char * getPairedDeviceAddress_mainModule()
{
    return  getPreference_mainModule(PREFERENCE_PAIRED_DEVICE_ADDRESS_SETTING);
    
}
//!returns if the paired device is not NONE. Note, the paired Name might be an address now (see below)
boolean isValidPairedDevice_mainModule()
{
    char *pairedDevice = getPairedDevice_mainModule();
    //new: now if non ""
    //return strcmp(pairedDevice,"NONE")!= 0;
    boolean isValid = pairedDevice && strlen(pairedDevice)>0 && strcmp(pairedDevice,"NONE")!= 0;
    if (!isValid)
    {
        char *pairedDeviceAddress = getPairedDeviceAddress_mainModule();
        isValid = pairedDeviceAddress && strlen(pairedDeviceAddress)>0 && strcmp(pairedDeviceAddress,"NONE")!= 0;
        SerialDebug.printf("isValidDeviceAddress(%s,%d)\n", pairedDeviceAddress, isValid);
   }
    SerialDebug.printf("isValidPairedDevice_mainModule(%s,%d)\n", pairedDevice, isValid);
    return isValid;
}

//**** dang.. the getPreference reuses the same storage!! ****

//!returns if the paired device is not NONE .. returns address or device
char * getPairedDeviceOrAddress_mainModule()
{
    char *pairedDevice = getPairedDevice_mainModule();
    if (pairedDevice && strlen(pairedDevice)>0 && strcmp(pairedDevice,"NONE")!= 0)
    {
        SerialLots.printf("getPairedDeviceOrAddress_mainModule(%s)\n", pairedDevice);
        return pairedDevice;
    }
    char *pairedDeviceAddress = getPairedDeviceAddress_mainModule();
    if (pairedDeviceAddress && strlen(pairedDeviceAddress)>0 && strcmp(pairedDeviceAddress,"NONE")!= 0)
    {
        SerialLots.printf("getPairedDeviceOrAddress_mainModule(%s)\n", pairedDeviceAddress);
        return pairedDeviceAddress;
    }
    else
        return (char*)"";
}

//!gets the device name
char *deviceName_mainModule()
{
    char *deviceName = getPreference_mainModule(PREFERENCE_DEVICE_NAME_SETTING);
    strcpy(_deviceNameSave,deviceName);
    return _deviceNameSave;
}

//!power of devices
//!reboot
void rebootDevice_mainModule()
{
    //reboot
    ESP.restart();
}

//!power off
void poweroff_mainModule()
{
#ifdef ESP_M5
    //SerialTemp.println(" **** THIS WOULD BE A POWEROFF ***");
    SerialTemp.println(" **** POWEROFF ***");

#ifndef ESP_M5_CAMERA
    M5.Axp.PowerOff();
#endif

#endif
}
// ******************* Preference Cleaning ************

//!cleans the EPROM
void cleanEpromPreferences()
{
    SerialDebug.println("CLEANING EPROM PREFERENCES");
    
    //4.17.22
    cleanEPROM_mainModule();
    
#ifdef USE_MQTT_NETWORKING
    cleanEPROM_MQTTNetworking();
#endif
    
#ifdef USE_WIFI_AP_MODULE
    cleanEPROM_WIFI_APModule();
#endif
    
    
    SerialDebug.println("REBOOTING...");
    
    //!reboot
    rebootDevice_mainModule();

}


//!retrieve a JSON string for the ssid and ssid_password: {'ssid':<ssid>,'ssidPassword':<pass>"}
char *main_JSONStringForWIFICredentials()
{
    return _JSONStringForWIFICredentials;
}

//!! cycle through the next WIFI saved credential
char* main_nextJSONWIFICredential()
{
    
    SerialTemp.printf("WIFI_CREDENTIAL_1: %s\n", getPreference_mainModule(PREFERENCE_WIFI_CREDENTIAL_1_SETTING));
    SerialTemp.printf("WIFI_CREDENTIAL_2: %s\n", getPreference_mainModule(PREFERENCE_WIFI_CREDENTIAL_2_SETTING));
    
   //! WIFI_CREDENTIALS_MAX
    for (int i = PREFERENCE_WIFI_CREDENTIAL_1_SETTING; i< PREFERENCE_WIFI_CREDENTIAL_1_SETTING+WIFI_CREDENTIALS_MAX; i++)
    {
        char* credential = getPreference_mainModule(i);
        //!jump out of loop if no match.. so index is valid
        if (strcmp(credential,"NONE")==0)
            continue;
        if (strcmp(credential,_JSONStringForWIFICredentials)!= 0)
        {
            SerialTemp.print("main_nextJSONWIFICredential:");
            SerialTemp.println(credential);
            return credential;
 
        }
    }
    SerialTemp.print("main_nextJSONWIFICredential:");
    SerialTemp.println(main_JSONStringForWIFICredentials());
    return main_JSONStringForWIFICredentials();
}

//!save the WIFI credential
void main_saveWIFICredentials(char *ssid, char *ssid_password)
{
    //!store the JSON version of these credentials..
    sprintf(_JSONStringForWIFICredentials, "{'ssid':'%s','ssidPassword':'%s'}", ssid?ssid:"NULL", ssid_password?ssid_password:"NULL");
    SerialMin.print("main_saveWIFICredentials");
    SerialMin.println(_JSONStringForWIFICredentials);

    //!look for an non matching slot.. if none, then use the first
    int index = PREFERENCE_WIFI_CREDENTIAL_1_SETTING;
    for (; index< PREFERENCE_WIFI_CREDENTIAL_1_SETTING+WIFI_CREDENTIALS_MAX; index++)
    {
        char* credential = getPreference_mainModule(index);
        //!jump out of loop if no match.. so index is valid, or NONE (which is the placeholder)
        if (strcmp(credential,"NONE")==0)
            break;;
        if (strcmp(credential,_JSONStringForWIFICredentials)== 0)
        {
            break;
        }
    }

    if (index >= PREFERENCE_WIFI_CREDENTIAL_1_SETTING+WIFI_CREDENTIALS_MAX)
        index = PREFERENCE_WIFI_CREDENTIAL_1_SETTING;

    SerialDebug.println(_JSONStringForWIFICredentials);
    //!save in chosen index
    savePreference_mainModule(index, _JSONStringForWIFICredentials);
}

//!clean the saved WIFI credential, otherwise the AP mode doesn't work (6.3.22)
void main_cleanSavedWIFICredentials()
{
    SerialMin.println("main_cleanSavedWIFICredentials");
    
    //!go through the saved WIFI credentials and empty them with "" (vs null)
    int index = PREFERENCE_WIFI_CREDENTIAL_1_SETTING;
    for (; index< PREFERENCE_WIFI_CREDENTIAL_1_SETTING+WIFI_CREDENTIALS_MAX; index++)
    {
        //!cannot null but can make stringlen=0
        savePreference_mainModule(index,"");
    }
}


//! sets the WIFI and MQTT user/password. It's up to the code (below, maybe in future a register approach)  to decide who needs to know
void main_updateMQTTInfo(char *ssid, char *ssid_password, char *username, char *password, char *guestPassword, char *deviceName, char * host, char * port, char *locationString)
{
    SerialMin.printf("main_updateMQTTInfo(%s,%s,%s,%s,%s, %s, d=%s)\n", ssid?ssid:"NULL", ssid_password?ssid_password:"NULL", username?username:"NULL", password?password:"NULL", guestPassword?guestPassword:"NULL", locationString?locationString:"NULL", deviceName?deviceName:"NULL");
    
    //!store the device name
   savePreference_mainModule(PREFERENCE_DEVICE_NAME_SETTING, deviceName);

    //!store the JSON version of these credentials..
    main_saveWIFICredentials(ssid,ssid_password);
    
#ifdef USE_WIFI_AP_MODULE
    WIFI_APModule_updateMQTTInfo(ssid, ssid_password, username, password, guestPassword, deviceName, host, port, locationString);
#endif
}

//! 3.21.22 these are to setup for the next time the main loop() runs to call these commands..
//! The implementation is hard coded in the ESP_IO.ino
//! TODO: make this a registeration approach
//#define ASYNC_CALL_OTA_UPDATE 0
//#define ASYNC_CALL_CLEAN_CREDENTIALS 1
//#define ASYNC_CALL_MAX 2

//!storage for asyncCallCommands
boolean _asyncCallFlags[ASYNC_CALL_MAX];
//!array of async flags for the different ASYNC_CALl values
boolean _asyncCallFlagsParameters[ASYNC_CALL_PARAMETERS_MAX];


//!initialize the async call flags (with and without parameters)
void initAsyncCallFlags()
{
    for (int i = 0; i < ASYNC_CALL_MAX; i++)
    {
        _asyncCallFlags[i] = false;
    }
    for (int i = 0; i < ASYNC_CALL_PARAMETERS_MAX; i++)
    {
        _asyncCallFlagsParameters[i] = false;
    }
}

// ******************* Async Dispatch Processing  ************



//!checks if any async commands are in 'dispatch' mode, and if so, invokes them, and sets their flag to false

//! dispatches a call to the command specified. This is run on the next loop()
void main_dispatchAsyncCommand(int asyncCallCommand)
{
    //! this just sets the flag, it's the next loop that makes the call
#ifdef ERROR_CHECKS
    if (asyncCallCommand < 0 || asyncCallCommand >= ASYNC_CALL_MAX)
    {
        SerialError.printf("ERROR: async call out of range: %d\n", asyncCallCommand);
    }
    else
#endif
    {
        SerialLots.printf("main_dispatchAsyncCommand: %d\n", asyncCallCommand);
        _asyncCallFlags[asyncCallCommand] = true;
    }
}

//!send an async call with a string parameter. This will set store the value and then async call the command (passing the parameter)
//!These are the ASYNC_CALL_PARAMETERS_MAX
void main_dispatchAsyncCommandWithString(int asyncCallCommand, char *parameter)
{
    //!store in the parameter mailbox
    strcpy(_asyncParameter,parameter);
    //! this just sets the flag, it's the next loop that makes the call
#ifdef ERROR_CHECKS
    if (asyncCallCommand < 0 || asyncCallCommand >= ASYNC_CALL_PARAMETERS_MAX)
    {
        SerialError.printf("ERROR: async call out of range: %d\n", asyncCallCommand);
    }
    else
#endif
    {
        SerialTemp.print("main_dispatchAsyncCommandWithString:");
        SerialTemp.print(asyncCallCommand);
        SerialTemp.print(": ");
        SerialTemp.println(_asyncParameter);
        _asyncCallFlagsParameters[asyncCallCommand] = true;
    }
}

//!checks if any async commands are in 'dispatch' mode, and if so, invokes them, and sets their flag to false
void invokeAsyncCommands()
{
    for (int i = 0; i < ASYNC_CALL_PARAMETERS_MAX; i++)
    {
        boolean asyncCallFlag = _asyncCallFlagsParameters[i];
        if (asyncCallFlag)
        {
            _asyncCallFlagsParameters[i] = false;
            switch (i)
            {
                case ASYNC_CALL_BLE_CLIENT_PARAMETER:
                    SerialTemp.print("ASYNC_CALL_BLE_CLIENT_PARAMETER: ");
                    SerialTemp.println(_asyncParameter);
#ifdef USE_BLE_CLIENT_NETWORKING
                    if (strlen(_asyncParameter)<=13)
                        sendCommandBLEClient_13orLess(_asyncParameter);
                    else
                        sendCommandBLEClient(_asyncParameter);
#endif
                    
                    break;
                case ASYNC_CALL_OTA_FILE_UPDATE_PARAMETER:
                    SerialTemp.print("ASYNC_CALL_OTA_FILE_UPDATE_PARAMETER: ");
                    SerialTemp.println(_asyncParameter);
#ifdef USE_MQTT_NETWORKING
                {
                    //look for the host:  http://<name>/
                    // httpAddress is everything after the /
                    //hostname is without http://
                    char hostname[100];
                    char httpAddress[100];
                    int thirdSlash = 0;
                    for (int i=0; i< strlen(_asyncParameter);i++)
                    {
                        if (_asyncParameter[i] == '/')
                        {
                            thirdSlash++;
                        }
                        if (thirdSlash == 3)
                        {
                            // i == point to 3rd slash
                            strncpy(hostname,_asyncParameter,i);
                            //add null
                            hostname[i] = (char)0;
                            char *a = &_asyncParameter[i+1];
                            strcpy(httpAddress,a);
                            break;
                        }
                    }
                    //todo..
                    SerialTemp.print("will performOTAUpdate: ");
                    SerialTemp.print(hostname);
                    SerialTemp.print("  with httpAddress: ");
                    SerialTemp.println(httpAddress);
                    performOTAUpdate(hostname, httpAddress);
                    
                }
#endif
                    break;
                case ASYNC_JSON_MESSAGE_PARAMETER:
                {
                  //process the message
                    SerialDebug.print("ASYNC_JSON_MESSAGE_PARAMETER: ");
                    SerialDebug.println(_asyncParameter);
                    
                    //! The FINISH of  callback for "onWrite" of the bluetooth "onWrite'
                    onWriteBLEServerCallbackFinish(_asyncParameter);
                }
                    break;
                    
            }
            
        }
    }
     
    //!now process those that don't have a string parameter
    for (int i = 0; i < ASYNC_CALL_MAX; i++)
    {
        boolean asyncCallFlag = _asyncCallFlags[i];
        if (asyncCallFlag)
        {
            _asyncCallFlags[i] = false;
            switch (i)
            {
                case ASYNC_CALL_OTA_UPDATE:
                    SerialLots.println("ASYNC_CALL_OTA_UPDATE");
#ifdef USE_MQTT_NETWORKING
                    performOTAUpdateSimple();
#endif
                    break;
                case ASYNC_CALL_CLEAN_CREDENTIALS:
                    SerialTemp.println("ASYNC_CALL_CLEAN_CREDENTIALS");
#ifdef USE_WIFI_AP_MODULE
                    clean_SSID_WIFICredentials();
                    //this reboots ..
#endif
                    break;
                case ASYNC_CALL_CLEAN_EPROM:
                    SerialTemp.println("ASYNC_CALL_CLEAN_EPROM");
                    cleanEpromPreferences();
                    break;
                case ASYNC_CALL_FEED_COMMAND:
                    SerialLots.println("ASYNC_CALL_FEED_COMMAND");
                    incrementFeedCount_mainModule();
#ifdef USE_MQTT_NETWORKING
                    //!register the 2 callbacks for now..
                    callCallbackMain(CALLBACKS_MQTT, MQTT_CALLBACK_FEED, (char *)"");
                    
#endif //USE_MQTT_NETWORKING
                    break;
                    
#define TRY_WIFI_FIRST
#ifdef  TRY_WIFI_FIRST
                    //!this is a sending of the message
                case ASYNC_SEND_MQTT_FEED_MESSAGE:
                    SerialTemp.println("ASYNC_SEND_MQTT_FEED_MESSAGE");
                    //incrementFeedCount_mainModule();
                    SerialCall.println("async_send_feed.0");
                    

#ifdef USE_MQTT_NETWORKING
                    
                    //!9.30.22  IF SET .. send a feed but to all devices except ours and our pair (if any)
                    //! uses new wildcard syntax either  ! OUR NAME  [ & ! OUR_CONNECTED_NAME
                    if (getPreferenceBoolean_mainModule(PREFERENCE_SENDWIFI_WITH_BLE))
                    {
                        char pubString[100];
                        char *paired = getPreference_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING);
                        boolean isPaired = paired && strlen(paired)>0;
                        // cannot save it.. so refresh below.. (Preferences are reused)
                        SerialTemp.printf("Paired = %s\n", paired?paired:(char*)"none");
                        SerialTemp.printf("Connected = %s\n", connectedBLEDeviceName_mainModule()?connectedBLEDeviceName_mainModule():(char*)"none");
                        
                        if (connectedBLEDeviceName_mainModule() && strlen(connectedBLEDeviceName_mainModule()) > 0)
                            sprintf(pubString,"{'dev':'!%s & !%s'",deviceName_mainModule(),connectedBLEDeviceName_mainModule());
                        else if (isPaired)
                            sprintf(pubString,"{'dev':'!%s & !%s'",deviceName_mainModule(),getPreference_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING));
                        else
                            sprintf(pubString,"{'dev':'!%s'",deviceName_mainModule());
                        strcat(pubString,",'cmd':'feed'}");
                        publishMQTTMessageDefaultTopic(pubString);
                    }
                    
                    
                    //SerialTemp.println("async_send_feed.3");
                    //TODO: 7.26.22 .. decide if this logic makes sense:
                    //NO BLECLient, and a message arrives for this device to #FEED. So previously it would sned that onto the network .. but I don't think that's right. (And we weren't in that mode yet).
                    //sendMessageMQTT((char *)"#FEED");
#endif // USE_MQTT_NETWORKING
#ifdef USE_BLE_CLIENT_NETWORKING
                    //! returns whether connected over BLE as a client to a server(like a ESP feeder)
                    if (!isConnectedBLEClient())
                    {
                        //! If not connected over BLE -- then send the MQTT feed message.. ??? WHY??
#ifdef USE_MQTT_NETWORKING_NOT_NOW
                        SerialCall.println("async_send_feed.1");
                        sendMessageMQTT((char *)"#FEED");
                        SerialTemp.println(" *** NO BLE connected so send wifi ASYNC_SEND_MQTT_FEED_MESSAGE");
#endif //use_MQTT_not_now
                    }
                    else
                    {
                        SerialCall.println("async_send_feed.2");
                        incrementFeedCount_mainModule();
                        
                        //!send a BLE feed command as we are connected
                        sendFeedCommandBLEClient();
                        
                        //!perform ACK too
#ifdef USE_MQTT_NETWORKING
                        //ack is sent by the caller of this message..??
                        sendMessageMQTT((char *)"#ackMe");
                        
#endif  //useMQTT
                    }
#ifdef NOT_NOW
                    //! 8.24.22 (This will redraw the screen - and turn off the blank screen (but not let anyone know)
                    //! SO: now only the buttons will unlock the blank screen..
                    
                    //!redraws the Semantic Marker image..
                    redrawSemanticMarker_displayModule(KEEP_SAME);
#endif //not_now
#endif  //using BLE_CLIENT
                    break;
#else //BLE_FIRST
                    //!this is a sending of the message
                case ASYNC_SEND_MQTT_FEED_MESSAGE:
                    SerialTemp.println("ASYNC_SEND_MQTT_FEED_MESSAGE");
                    //incrementFeedCount_mainModule();
                    SerialCall.println("async_send_feed.0");

#ifdef USE_BLE_CLIENT_NETWORKING
                    //! returns whether connected over BLE as a client to a server(like a ESP feeder)
                    if (!isConnectedBLEClient())
                    {
                        //! If not connected over BLE -- then send the MQTT feed message.. ??? WHY??
#ifdef USE_MQTT_NETWORKING_NOT_NOW
                        SerialCall.println("async_send_feed.1");
                        sendMessageMQTT((char *)"#FEED");
                        SerialTemp.println(" *** NO BLE connected so send wifi ASYNC_SEND_MQTT_FEED_MESSAGE");
#endif //use_MQTT_not_now
                    }
                    else
                    {
                        SerialCall.println("async_send_feed.2");
                        incrementFeedCount_mainModule();

                        //!send a BLE feed command as we are connected
                        sendFeedCommandBLEClient();
                        
                        //!perform ACK too
#ifdef USE_MQTT_NETWORKING
                        //ack is sent by the caller of this message..?? 
                        sendMessageMQTT((char *)"#ackMe");

#endif  //useMQTT
                    }
#ifdef NOT_NOW
                    //! 8.24.22 (This will redraw the screen - and turn off the blank screen (but not let anyone know)
                    //! SO: now only the buttons will unlock the blank screen..
            
                    //!redraws the Semantic Marker image..
                    redrawSemanticMarker_displayModule(KEEP_SAME);
#endif //not_now
#endif  //using BLE_CLIENT
#ifdef USE_MQTT_NETWORKING
                    
                    //!9.30.22  IF SET .. send a feed but to all devices except ours and our pair (if any)
                    //! uses new wildcard syntax either  ! OUR NAME  [ & ! OUR_CONNECTED_NAME
                    if (getPreferenceBoolean_mainModule(PREFERENCE_SENDWIFI_WITH_BLE))
                    {
                        char pubString[100];
                        char *paired = getPreference_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING);
                        boolean isPaired = paired && strlen(paired)>0;
                        // cannot save it.. so refresh below.. (Preferences are reused)
                        SerialTemp.printf("Paired = %s\n", paired?paired:(char*)"none");
                        SerialTemp.printf("Connected = %s\n", connectedBLEDeviceName_mainModule()?connectedBLEDeviceName_mainModule():(char*)"none");
                        
                        if (connectedBLEDeviceName_mainModule() && strlen(connectedBLEDeviceName_mainModule()) > 0)
                            sprintf(pubString,"{'dev':'!%s & !%s'",deviceName_mainModule(),connectedBLEDeviceName_mainModule());
                        else if (isPaired)
                            sprintf(pubString,"{'dev':'!%s & !%s'",deviceName_mainModule(),getPreference_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING));
                        else
                            sprintf(pubString,"{'dev':'!%s'",deviceName_mainModule());
                        strcat(pubString,",'cmd':'feed'}");
                        publishMQTTMessageDefaultTopic(pubString);
                    }
                    
                    
                    //SerialTemp.println("async_send_feed.3");
//TODO: 7.26.22 .. decide if this logic makes sense:
                    //NO BLECLient, and a message arrives for this device to #FEED. So previously it would sned that onto the network .. but I don't think that's right. (And we weren't in that mode yet).
                    //sendMessageMQTT((char *)"#FEED");
#endif // USE_MQTT_NETWORKING
                    break;
#endif

                case ASYNC_CALL_BUZZ_ON:
                case ASYNC_CALL_BUZZ_OFF:
                {
                    boolean isBuzzOn = (i == ASYNC_CALL_BUZZ_ON);
                    //! set the persistence (Note, this is locally saved. If sending elsewhere .. then they have to set their value)
                    savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE, isBuzzOn);
                    
                    SerialDebug.printf("ASYNC_CALL_BUZZ %d\n",isBuzzOn);
                    String cmdToSend;
                    if (isBuzzOn)
                        cmdToSend = "{'cmd':'buzzon'}";
                    else
                        cmdToSend = "{'cmd':'buzzoff'}";
 
                    //!OK: issue.
                    //!  if we are an ESP feeder, the STEPPER is on .. so we're good,
                    //! if an M5 - then either send over BLE or MQTT
#ifdef ESP_M5
                    //!send via BLE or MQTT ..
#ifdef USE_BLE_CLIENT_NETWORKING
                    //! returns whether connected over BLE as a client to a server(like a ESP feeder)
                    if (!isConnectedBLEClient())
                    {
                        SerialLots.println("async_call_buzz_on/off -- not BLE connected.. send MQTT");
                        //! If not connected over BLE -- then send the MQTT buzzon message..
#ifdef USE_MQTT_NETWORKING
                            sendMessageMQTT((char*)cmdToSend.c_str());
#endif
                    }
                    else
                    {
                        //!send over BLE...
                            sendCommandBLEClient(cmdToSend);
                    }
#endif //USE_BLE_CLIENT
#else  //not M5
                    
#ifdef USE_BLE_CLIENT_NETWORKING
                    //! returns whether connected over BLE as a client to a server(like a ESP feeder)
                    if (!isConnectedBLEClient())
                    {
                        SerialLots.println("async_call_buzz_on -- not BLE connected.. send MQTT");
                        //! If not connected over BLE -- then send the MQTT buzzon message..
#ifdef USE_MQTT_NETWORKING
                        sendMessageMQTT((char *)cmdToSend.c_str());
#endif
                    }
                    else
                    {
                        SerialLots.println("async_call_buzz_on -- YES BLE connected.. send BLE");
                        processClientCommandChar_mainModule(isBuzzOn?'B':'b');
                        
                    }
#else //(so we are a feeder.. or another bridge .. doit)
                    //! we are not BLE_CLIENT .. so local
                    SerialDebug.println("Local, doit");
                    processClientCommandChar_mainModule(isBuzzOn?'B':'b');

#endif //USE_BLE_CLIENT_NETWORKING
#endif // ESP_M5
                }
                    break;
                    
                    //!setGatewayOn/Off called from the processJSON message in MQTT
                    //! or via the EPROM setting (TODO)
                case ASYNC_SET_GATEWAY_ON:
                    SerialLots.println("ASYNC_SET_GATEWAY_ON");
                    savePreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE, true);
                    
                    break;
                    
                case ASYNC_SET_GATEWAY_OFF:
                    SerialLots.println("ASYNC_SET_GATEWAY_OFF");
                    savePreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE, false);
                    
                    break;
                    
                case ASYNC_REBOOT:
                    SerialLots.println("ASYNC_REBOOT");
                    rebootDevice_mainModule();
                    
                    break;
                    
                case ASYNC_POWEROFF:
                    SerialLots.println("ASYNC_POWEROFF");
                    poweroff_mainModule();
                    
                    break;
                case ASYNC_BLANKSCREEN:
                    SerialDebug.println("ASYNC_BLANKSCREEN");
                    blankScreen_displayModule();
                    break;
                case ASYNC_SEND_MQTT_STATUS_URL_MESSAGE:
                    //!sends the status from the main module URL
#ifdef USE_MQTT_NETWORKING
                {
                    char *statusURL = main_currentStatusURL();
                    SerialDebug.print(" ASYNC_SEND_MQTT_STATUS_URL: ");
                    SerialDebug.println(statusURL);
                    /// NO MORE: sendDocFollowMessageMQTT(statusURL);
                    sendStatusMessageMQTT(statusURL);
                }
#endif
                    break;
                    
                case ASYNC_SWAP_WIFI:
                case ASYNC_NEXT_WIFI:
                {
                    //NOTE: this might be where we toggle credentials?? TODO
                    //found other one..
                    char *credentials = main_nextJSONWIFICredential();
#ifdef USE_MQTT_NETWORKING
                    
                    //!These are the ASYNC_CALL_PARAMETERS_MAX
                    //!NO: just change our credentials ...
                    //send to
                    //main_dispatchAsyncCommandWithString(ASYNC_CALL_BLE_CLIENT_PARAMETER, credentials);
                    processJSONMessageMQTT(credentials, TOPIC_TO_SEND);
#endif
                }
                    break;
                case ASYNC_RESTART_WIFI_MQTT:
#ifdef USE_MQTT_NETWORKING
                    //!Restarts (or attempts) a restart of the WIFI using the existing credentials -- vs the 'n' command
                    restartWIFI_MQTTState();
#endif
                    break;
                default:
                    SerialLots.printf("NO COMMAND: %s", i);
            }
        }
    }
}


//define a couple lights .. maybe move somewhere else..
char* _ON_LIGHT = (char*)"ON";
char* _OFF_LIGHT = (char*)"OFF";

//!callback for blinking led
void blinkMessageCallback(char *message)
{
    
#ifdef ESP_M5
    
    //call the already defined blink led
    // devined in Dispense.cpp
//    digitalWrite(LED, HIGH);
//    delay(150);
//    digitalWrite(LED, LOW);
        
#else //not M5
      //call the already defined blink led
      // defined in Dispense.cpp
   // stepperModule_BlinkLED();
    blinkLED_UIModule();
#endif  //ESP_M5
}

//!callback for SOLID blinking led
void solidLightMessageCallback(char *message)
{
    SerialLots.println("solidLightMessageCallback..");
#ifdef ESP_M5
#else
    //!call the already defined solid led
    //! defined in Dispense.cpp
    boolean lightOn = true;
    lightOn = strstr(&message[0], _ON_LIGHT);
    solidLightOnOff_UIModule(lightOn);
#endif
}

//!clean the SSID eprom (MQTT_CLEAN_SSID_EPROM)
void cleanSSID_EPROM_MessageCallback(char *message)
{
    //!call the already defined solid led
    //! defined in Dispense.cpp
#ifdef USE_WIFI_AP_MODULE
    //!clean_SSID_WIFICredentials();
    //!now register an async call..
    main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_CREDENTIALS);
#endif //USE_WIFI_AP_MODULE
    
}

//!called on single click
//!NOTE: with BLE_CLIENT_NETWORKING, the right button and top button send a BLE command for feeding..
void singleClickTouched(char *whichButton)
{
#ifdef USE_WIFI_AP_MODULE
    //!for now, only send the FEED command via BLE_CLIENT if turned on. No reboot to AP mode yet..
#ifdef USE_BLE_CLIENT_NETWORKING
    //!send the async FEED
    
    //! dispatches a call to the command specified. This is run on the next loop()
    main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
    
#else
    //TODO.. if M5 do this differently..
    clean_SSID_WIFICredentials();
#endif
#else  //not USE_WIFI_AP_MODULE // never sure of the #elsif syntax..
#ifdef USE_BLE_CLIENT_NETWORKING
    //send the async FEED
    //! dispatches a call to the command specified. This is run on the next loop()
    main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
#endif
#endif //USE_WIFI_AP_MODULE
    
}

//! shows a FEED (or whatever) then blanks the screen after N seconds
//! NOTE: This will be a scrolling text as sometime ..
void showText_mainModule(String text)
{
    //showText_displayModule(text);
    addToTextMessages_displayModule(text);

}

//!callback for SOLID blinking led
void solidLightOnOff(boolean onOff)
{
#ifdef ESP_M5
#else
    SerialLots.println("solidLight..");
    //!call the already defined solid led
    //! defined in Dispense.cpp
    boolean lightOn = onOff;
    solidLightOnOff_UIModule(lightOn);
#endif
}



//!prints the module configuration by looking at #defines
//! Eventually this might be an object returned letting the code
//!know a capability is included for runtime (vs compile time) decisions
void main_printModuleConfiguration()
{
    //!Module Configuration
    SerialDebug.println(" ** #define Module Configuration **");
#ifdef ESP_M5
    //![x] ESP_M5
    SerialMin.println("[x] ESP_M5");
#else
    SerialInfo.println("[ ] ESP_M5");
#endif
#ifdef ESP_M5_CAMERA
    //![x] ESP_M5
    SerialMin.println("[x] ESP_M5_CAMERA");
#endif
#ifdef ESP_32
    //! [x] ESP_32
    SerialMin.println("[x] ESP_32");
#else
    SerialInfo.println("[ ] ESP_32");
#endif
#ifdef USE_MQTT_NETWORKING
    //! [x] USE_MQTT_NETWORKING
    SerialMin.println("[x] USE_MQTT_NETWORKING");
#else
    SerialInfo.println("[ ] USE_MQTT_NETWORKING");
#endif
#ifdef USE_BLE_SERVER_NETWORKING
    //! [x] USE_BLE_SERVER_NETWORKING
    SerialMin.println("[x] USE_BLE_SERVER_NETWORKING");
#else
    SerialInfo.println("[ ] USE_BLE_SERVER_NETWORKING");
#endif
#ifdef USE_BLE_CLIENT_NETWORKING
    //! [x] USE_BLE_CLIENT_NETWORKING
    SerialMin.println("[x] USE_BLE_CLIENT_NETWORKING");
#else
    SerialInfo.println("[ ] USE_BLE_CLIENT_NETWORKING");
    
#endif
#ifdef USE_BUTTON_MODULE
    // [x] USE_BUTTON_MODULE
    SerialMin.println("[x] USE_BUTTON_MODULE");
#else
    SerialInfo.println("[ ] USE_BUTTON_MODULE");
    
#endif
#ifdef USE_WIFI_AP_MODULE
    //! [x] USE_WIFI_AP_MODULE

    SerialMin.println("[x] USE_WIFI_AP_MODULE");
#else
    SerialInfo.println("[ ] USE_WIFI_AP_MODULE");
    
#endif
#ifdef USE_STEPPER_MODULE
    SerialMin.println("[x] USE_STEPPER_MODULE");
#else
    SerialInfo.println("[ ] USE_STEPPER_MODULE");
    
#endif
#ifdef USE_UI_MODULE
    SerialMin.println("[x] USE_UI_MODULE");
#else
    SerialInfo.println("[ ] USE_UI_MODULE");
    
#endif
#ifdef USE_DISPLAY_MODULE
    SerialMin.println("[x] USE_DISPLAY_MODULE");
#else
    SerialInfo.println("[ ] USE_DISPLAY_MODULE");
    
#endif
#ifdef BOARD
    SerialMin.println("[x] BOARD");
#else
    SerialInfo.println("[ ] BOARD");
#endif
#ifdef USE_SPIFF_MODULE
    SerialMin.println("[x] USE_SPIFF_MODULE");
#else
    SerialInfo.println("[ ] USE_SPIFF_MODULE");
#endif
//#ifdef PROTO
//    SerialMin.println("[x] PROTO");
//#else
//    SerialInfo.println("[ ] PROTO");
//#endif

    //Set this if the SMART clicker build is used.
#ifdef USE_SMART_CLICKER
    SerialMin.println("[x] USE_SMART_CLICKER");
#else
    SerialInfo.println("[ ] USE_SMART_CLICKER");
#endif
    
#if (SERIAL_DEBUG_ERROR)
    SerialMin.println("[x] SERIAL_DEBUG_ERROR");
#endif
#if (SERIAL_DEBUG_DEBUG)
    SerialMin.println("[x] SERIAL_DEBUG_DEBUG");
#endif
#if (SERIAL_DEBUG_INFO)
    SerialMin.println("[x] SERIAL_DEBUG_INFO");
#endif
#if (SERIAL_DEBUG_MINIMAL)
    SerialMin.println("[x] SERIAL_DEBUG_MINIMAL");
#endif
    
    //!and print any preferences to show
    printPreferenceValues_mainModule();
}


//!If nil it create one with just the null, so strlen = 0
//!NOTE: the strdup() might be used later..
char* createCopy(char * stringA)
{
    if (stringA)
        return strdup(stringA);
    else
        return strdup("");
}

//!informs if null or empty string
boolean isEmptyString(char *stringA)
{
    if (!stringA)
    {
        return true;
    }
    else if (strlen(stringA) == 0)
    {
        return true;
    }
    else
        return false;
}

//!a char* version of startsWith (after skipping spaces)
boolean startsWithChar(char *str, char c)
{
    if (!str) return false;
    //!find first non space character, and if not '{' then return false
    int i = 0;
    while (i < strlen(str))
    {
        //!jump out if not a space, and 'i' will be the thing to look for
        if (str[i] != ' ')
            break;
        i++;
    }
    
    if (str[i] == c)
        return true;
    
    return false;
}

//!start of the sensor updates ... TODO: tie these to the MQTT messaging as well..
float getBatPercentage_mainModule()
{
#ifdef ESP_M5
    float batVoltage = M5.Axp.GetBatVoltage();
    float batPercentage = (batVoltage < 3.2) ? 0 : ( batVoltage - 3.2 ) * 100;
#else
    float batPercentage = 87.0;
#endif
    
#ifdef ESP_M5
    //!#Issue 117
    //!from: https://community.m5stack.com/topic/1361/ischarging-and-getbatterylevel/9
    //!GetVbatdata() is currently depreciated
    uint16_t vbatData = M5.Axp.GetVbatData();
    double vbat = vbatData * 1.1 / 1000;
    batPercentage =  100.0 * ((vbat - 3.0) / (4.07 - 3.0));
#endif
    if (batPercentage > 100.0)
        batPercentage = 100.0;
    return batPercentage;
}



//!adding a synchronous call to send a message over the network (assuming MQTT but not specified), this tacks on {device} and {t:time}
void sendMessageString_mainModule(char *messageString)
{
#ifdef USE_MQTT_NETWORKING
    if (strlen(messageString) < MESSAGE_STORAGE_MAX)
    {
        //!NOTE: the # has to be there, otherwise the sendMessageMQTT ignores it..
        sprintf(_messageStorage,"#%s {%s} {t:%d}", messageString, getDeviceNameMQTT(), getTimeStamp_mainModule());
        //!send this message over MQTT
        sendMessageMQTT(_messageStorage);
    }
#endif
}

//!retrieves the temperature .
float getTemperature_mainModule()
{
#ifdef ESP_M5
    //!return celcius
    //!float temperature = M5.Axp.GetTempInAXP192();
    float temperature;
    M5.IMU.getTempData(&temperature);

    int maxtemp =  getPreferenceInt_mainModule(PREFERENCE_HIGH_TEMP_POWEROFF_VALUE);
    if (temperature > maxtemp)
    {
        //! turn off the device after sending a message..
        //! let others know we are powering off..
        //! then poweroff
#ifdef USE_MQTT_NETWORKING
        sprintf(_messageStorage,"#Temp %f too high > %d - powering off {%s}", temperature, maxtemp, getDeviceNameMQTT());
        //!send this message over MQTT
        sendMessageMQTT(_messageStorage);
#endif
        //! now poweroff
        SerialTemp.println(" *** This would poweroff .. but you can send message");
        //!poweroff_mainModule();

    }

#else
    float temperature = 10.0;
#endif
    return temperature ;//* 9/5 + 32;
}
//!NEW: 4.30.22
//!returns a string in JSON format, such that {'battery':'84'}, {'buzzon':'off'} .. etc
//!as a URL:   SemanticMarker.org/bot/sample/scott@name/password/status?BLE:on&numfeeds=8&WIFI:on&AP:off
/** Sample format:
{"status": [
    {
    "BLE": "on"
    },
    {
    "numfeeds": "8",
    "maxfeeds": "16"
    },
    {
    "battery": "87"
    },
    {
    "buzzon": "on"
    },
    {
    "MQTT": "on"
    },
    {
    "WIFI": "on"
    },
    {
    "AP": "off"
    },
    {
    "tilt": "55"
    }
]
}
*/
char* main_currentStatusJSON()
{
    //returns a JSON string..
//    String b = String(getBatPercentage(),0);
//    return "{'status':[{'BLE':'on'},{'bat':'" + b + "'}]}";
    return NULL;
}

//! TODO: fix syntax. If just sensor sensor status: ....
//!https://SemanticMarker.org/bot/sensor/scott@konacurrents.com/doggy/status?v=v2&dev=M5Ski&b=68&temp=66&c=0&t=8&W=on&M=on&B=on&C=on&A=off&T=on&S=on&Z=off&t=8
//!not working just added Z=on/off  length = 157
//! going to remove the username/password to shorten..


//!adds a query string "&key=value"
void addStatusStringFlag(const char *key, char * val)
{
    char buf[50];
    sprintf(buf,"&%s=%s", key, val);
    strcat(_fullStatusString, buf);
}
//!adds to _fullStatusString a query string "&key=value"
void addStatusBooleanFlag(const char *key, boolean flag)
{
    addStatusStringFlag(key, flag?(char*)"on":(char*)"off");
    
}
//!sets the "Module Status"
//!in queryString == name=val&name=val ...
void addMoreStatusQueryString()
{
    //!value of WIFI connected
#ifdef USE_MQTT_NETWORKING
    addStatusBooleanFlag("W",isConnectedWIFI_MQTTState());
    addStatusBooleanFlag("M",isConnectedMQTT_MQTTState());
#endif
#ifdef USE_BLE_CLIENT_NETWORKING
    //useBLECLient == it's linked in and running (vs not running)
    addStatusBooleanFlag("B",useBLEClient());
    //! connected == we are connected to another BLEServer
    addStatusBooleanFlag("C",isConnectedBLEClient());
    
#endif
#ifdef USE_WIFI_AP_MODULE
    //!not done is what we look for ..
    addStatusBooleanFlag("A",!doneWIFI_APModule_Credentials());
#endif
    addStatusBooleanFlag("T", getPreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE));
    
#ifdef USE_BLE_SERVER_NETWORKING
    addStatusBooleanFlag("S",getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE));
    {
        //! add a bleS=PTFeeder:name
        char *serverBLEName = getServiceName_BLEServerNetworking();
        addStatusStringFlag((char*)"bleS",serverBLEName);
    }
#endif
    
    //!show Z for buZZ
    addStatusBooleanFlag("Z",getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE));
    
    //!show G for gateway
    addStatusBooleanFlag("G",getPreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE));
    
    //!P = paired
    char *pairedDeviceName = getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING);
    if (isValidPairedDevice_mainModule())
#ifdef NO_MORE_PREFERENCE_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING

    if (getPreferenceBoolean_mainModule(PREFERENCE_ONLY_GEN3_CONNECT_SETTING) ||
        getPreferenceBoolean_mainModule(PREFERENCE_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING))
#endif
    {
        addStatusStringFlag("P", pairedDeviceName);
    }
    
    //! add the canister and stepper angle per #278
#ifdef ESP_32
    addStatusStringFlag("can",  getPreferenceString_mainModule(PREFERENCE_STEPPER_KIND_VALUE));
    addStatusStringFlag("stp",  getPreferenceString_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING));
    addStatusStringFlag("dir",  getPreferenceString_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING));

#endif
}

//!returns a string in in URL so:  status?battery=84'&buzzon='off'  } .. etc
char* main_currentStatusURL()
{

#ifdef USE_MQTT_NETWORKING
    char *deviceName = getDeviceNameMQTT();
    //!TODO: make sure no spaces ... unless escaped
    sprintf(_fullStatusString,"status?v=%s&dev=%s&b=%2.0f&temp=%2.0f&c=%0d&t=%0d",VERSION_SHORT, deviceName, getBatPercentage_mainModule(), getTemperature_mainModule(), getFeedCount_mainModule(), getLoopTimer_displayModule());

#else
        //!TODO: make sure no spaces ... unless escaped
    sprintf(_fullStatusString,"status?v=%s&b=%2.0f&temp=%2.0f&c=%0d&t=%0d",VERSION_SHORT, getBatPercentage_mainModule(), getTemperature_mainModule(), getFeedCount_mainModule(), getLoopTimer_displayModule());
#endif

    //add to _fullStatusString
    addMoreStatusQueryString();
    
//    //NOTE: any new query from somewhere starts with & (they put it in)

    SerialCall.println( _fullStatusString );
    
    //!TODO: put these somewhere somehow (expandable easily..architecturally. eg. setStatus("battery","87") key,val
    //!the JSON DB supports db["battery"]=87 like syntax..
    
    //return "status?buzzon=off&AP=off&MQTT=on&WIFI=on&BLE=on&battery=" + String(getBatPercentage(),0);
    return _fullStatusString;
}

#ifdef UNDEFINED
//!sets status parts cia a a string in JSON format, such that {'battery':'84'}, {'buzzon':'off'} .. etc
void main_setStatusJSON(const char* JSONString)
{
    //would set the JSON string
}
//!sets status parts cia a a string in JSON format, such that {'battery':'84'}, {'buzzon':'off'} .. etc
void main_setStatusKeyValue(const char* key, const char* value)
{
    //todo..
}
#endif

//!SemanticMarker events
//!This would see a DOCFollow message, and set the value.. then SM10 can display it..
char* _lastSemanticMarkerDocFollow = NULL;
//!sed the address to follow
void setSemanticMarkerDocFollow_mainModule(char* SMDocFollowAddress)
{
    _lastSemanticMarkerDocFollow = SMDocFollowAddress;
    if (!_lastSemanticMarkerDocFollow)
        _lastSemanticMarkerDocFollow = NULL;
}
//!gets the semanticAddress SemanticMarker&trade;
char* getSemanticMarkerDocFollow_mainModule()
{
    return _lastSemanticMarkerDocFollow;
}


//!sends the SM on the DOCFOLLOW channel (publish it..)
void sendSemanticMarkerDocFollow_mainModule(const char* SMDocFollowAddress)
{
#ifdef USE_MQTT_NETWORKING
    sendDocFollowMessageMQTT(SMDocFollowAddress);
#endif

}

//!Keep ProcessClientCmd short to let the callback run. instead change the feeder state flag
//! processes a message that might save in the EPROM.. the cmd is still passed onto other (like the stepper module)
void processClientCommandChar_mainModule(char cmd)
{
    //!the sentToStepper is only needed if there are 'actions' on the command, versus just
    //! setting persistent EPROM data. Like feed, buzzer, etc..
    //!Thus we don't need to know much of their implementation.. or we just pass anyway!
    boolean sendToStepperModule = false;
    SerialDebug.printf("***** processClientCommand_mainModule(%c) from client*****\n", cmd);
    
    //char cmd = message[0];
    //!only process things that are stored persistently..
    if ((cmd == 0x00) || (cmd == 's') || (cmd == 'c'))
    {
        sendToStepperModule = true;
    }
    else if (cmd == 'a')
    {

        SerialDebug.println("cmd=a startTimer");
        //!start timer..
        startStopTimer_mainModule(true);

    }
    else if (cmd == 'A')
    {

        SerialDebug.println("cmd=A stopTimer");

        //!start timer..
        startStopTimer_mainModule(false);
        
    }
    else if (cmd == 'j')
    {
        sendToStepperModule = true;
        SerialLots.println("Setting FeedState = JACKPOT_FEED");
        savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_JACKPOT_FEED_VALUE, true);
    
    }
    else if (cmd == 'u')
    {
        SerialLots.println("Setting feederType = UNO");
        savePreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE, STEPPER_IS_UNO);
    }
    else if (cmd == 'm')
    {
        SerialLots.println("Setting feederType = MINI");
        //save preference
        savePreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE, STEPPER_IS_MINI);
    }
    else if (cmd == 'L')
    {
        SerialLots.println("Setting feederType = Tumbler");
        //save preference
        savePreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE, STEPPER_IS_TUMBLER);
    }
    else if (cmd == 'B')
    {
        sendToStepperModule = true;
        SerialDebug.println("Setting buzzStatus = BUZZON");
        //save pref
        savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE, true);
    }
    else if (cmd == 'b')
    {
        sendToStepperModule = true;

        SerialDebug.println("Setting buzzStatus = BUZZOFF");
        savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE, false);
    }
    else if (cmd == 'T')
    {
        SerialDebug.println("*** Setting tilt = ON");
        savePreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE, true);
    }
    else if (cmd == 't')
    {
        SerialDebug.println("Setting tilt = OFF");
        savePreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE, false);
    }
    else if (cmd == 'R')
    {
        SerialLots.println("Clean Credentials");
        //! dispatches a call to the command specified. This is run on the next loop()
        main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_CREDENTIALS);
    }
    else if (cmd == 'O')
    {
        SerialLots.println("OTA Update.. ");
        //! dispatches a call to the command specified. This is run on the next loop()
        main_dispatchAsyncCommand(ASYNC_CALL_OTA_UPDATE);
    }
    else if (cmd == 'X')
    {
        SerialLots.println("Clean EPROM.. ");
        //! dispatches a call to the command specified. This is run on the next loop()
        main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_EPROM);
    }
    
    //!NOTE: the gateway is auto selected for now. A future version might manually set it in other situations (eg. my iPhone app should have a flag to not be a gateway at time)
    else if (cmd == 'G')
    {
        SerialLots.println("Setting Gateway = ON");
        main_dispatchAsyncCommand(ASYNC_SET_GATEWAY_ON);
    }
    else if (cmd == 'g')
    {
        SerialLots.println("Setting Gateway = OFF");
        main_dispatchAsyncCommand(ASYNC_SET_GATEWAY_OFF);
    }
    else if (cmd == '_')
    {
        //This is from the handshake like "_BLEClient_ESP_M5"
        SerialLots.println("unused cmd '_'");
    }
    else if (cmd == 'Z')
    {
        sendToStepperModule = false;
        
        SerialLots.println("Setting SM Zoom = zoomed");
        savePreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE, true);
    }
    else if (cmd == 'z')
    {
        sendToStepperModule = false;
        
        SerialLots.println("Setting SM Zoom = full SM");
        savePreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE, false);
    }
    else if (cmd == 'N')
    {
        //NOTE: this might be where we toggle credentials?? TODO
        //found other one..
        char *credentials = main_JSONStringForWIFICredentials();

        //!These are the ASYNC_CALL_PARAMETERS_MAX
        main_dispatchAsyncCommandWithString(ASYNC_CALL_BLE_CLIENT_PARAMETER, credentials);
    }
    else if (cmd == 'n')
    {
        //!NOTE: this is almost the same as 'w' except there might be more WIFI than 2 (so swap is different).
        main_dispatchAsyncCommand(ASYNC_NEXT_WIFI);
    }
    else if (cmd == 'W')
    {
        main_dispatchAsyncCommand(ASYNC_RESTART_WIFI_MQTT);
    }
    else if (cmd == 'w')
    {
        SerialTemp.println("w for swapWifi");

        main_dispatchAsyncCommand(ASYNC_SWAP_WIFI);
    }
    else if (cmd == 'P')
    {
        //printout the spiff.. to the serial debug monitor
        //!Restarts (or attempts) a restart of the WIFI using the existing credentials -- vs the 'n' command
        printFile_SPIFFModule();
    }
    else if (cmd == 'D')
    {
        // motor direction = counterclockwise
        savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING,false);
    }
    else if (cmd == 'd')
    {
        // motor direction ==  (clockwise)
        savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING,true);
    }
    //! 9.30.23 reverse direction
    else if (cmd == 'Q')
    {
        //! note: reboot not needed as the next time a feed happens, it reads this value
        // motor direction ==  (reverse)
        boolean  currentDirection = getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING);
        currentDirection = !currentDirection;
        savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING,currentDirection);

    }
    else if (cmd == 'E')
    {
        //!if set, the BLE Server (like PTFeeder) will tack on the device name (or none if not defined).
        savePreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING,false);
        // reboot
        rebootDevice_mainModule();
    }
    else if (cmd == 'e')
    {
        //!if set, the BLE Server (like PTFeeder) will tack on the device name (or none if not defined).
        savePreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING,true);
        // reboot
        rebootDevice_mainModule();
    }
    else if (cmd == 'r')
    {
        // reboot
        rebootDevice_mainModule();
    }
    else if (cmd == 'H')
    {
        
        SerialLots.println("Valid Commands: ");
        SerialLots.println("         H == help, this message");
        SerialLots.println(" 0x0, s, c == Single Feed ");
        SerialLots.println("         a == AutoFeed On");
        SerialLots.println("         A == AutoFeed Off");
        SerialLots.println("         u == UNO ");
        SerialLots.println("         m == MINI ");
        SerialLots.println("         L == tumbler");
        SerialLots.println("         D == counterclockwise motor direction");
        SerialLots.println("         d == clockwise motor direction");
        SerialLots.println("         Q == change motor direction opposite of current");

        SerialLots.println("         B == Buzzer On");
        SerialLots.println("         b == Buzzer Off");
        SerialLots.println("         G == Gateway On");
        SerialLots.println("         g == Gateway Off");
        SerialLots.println("         R == clean credentials");
        SerialLots.println("         X == clean EPROM");
        SerialLots.println("         r == reboot ");
        SerialLots.println("         O == OTA update");
        SerialLots.println("         T == tiltOn");
        SerialLots.println("         t == tiltOff");
        SerialLots.println("         N == send WIFI Credential to BLEServer");
        SerialLots.println("         n == next WIFI Credential");
        SerialLots.println("         W == retry WIFI");
        SerialLots.println("         w == swap WIFI");
        SerialLots.println("         P == print SPIFF");
        SerialLots.println("         E == use only PTFeeder naming");
        SerialLots.println("         e == use naming PTFeeder:name");
        SerialLots.println("         Z == Setting SM Zoom = zoomed");
        SerialLots.println("         z == Setting SM Zoom = full SM");

        //!print out stuff
        main_printModuleConfiguration();
   
    }
    else
    {
        SerialLots.printf("*****invalid command '%c' from client*****\n", cmd);
    }
    
#ifdef USE_STEPPER_MODULE
    if (sendToStepperModule)
    {
        stepperModule_ProcessClientCmdFinal(cmd);
    }
#endif
}


//!On the esp32, sec is all we can handle.  We can return as a double if milisecond resolution is needed.
//!This is the time since app started..
//!https://randomnerdtutorials.com/epoch-unix-time-esp32-arduino/
int getTimeStamp_mainModule()
{
    
    time_t now;
    struct tm timeinfo;
    time(&now);
    SerialLots.printf("Unix Time: %d\n", now);
    return now;
}

//! ************** SM Mode Processing ***************


//!returns an index from 0..max of SM matching cmd, or -1 if none
int whichSMMode_mainModule(char *cmd)
{
    //https://www.cplusplus.com/reference/cstring
    if (strncmp(cmd,"sm",2) == 0)
    {
        //point to the 'm' in 'sm'
        char *p = strchr(cmd,'m');
        //then increment
        p++;
        //convert to a number
        int num = atoi(p);
        //convert to integer..
        SerialLots.print("whichSMMode: ");
        SerialLots.print(cmd);
        SerialLots.print(" => " );
        SerialLots.println(num);
        return num;
    }
    else
    {
        return -1;
    }
}
//!returns if a match the mode. whichSMMode is 0..12 and == sm0 .. smn
boolean matchesSMMode_mainModule(char *cmd, int whichSMMode)
{
    char *mode = charSMMode_mainModule(whichSMMode);
    return (strcasecmp(cmd,mode)==0);
}
//!returns string form whichSMMode, sg "sm0", sm1 ...
//!This can (and is) called by multiple places (like ButtonProcessing and MainModule
char* charSMMode_mainModule(int whichSMMode)
{
    SerialLots.printf("charSMMode_mainModule: %d\n", whichSMMode);
    
    sprintf(_smMode_MainModule,"sm%0d",whichSMMode);
    SerialCall.println(_smMode_MainModule);
    return _smMode_MainModule;
}

//!returns which mode in (min or expanded)
boolean isMinimalMenuMode_mainModule()
{
    return getPreferenceBoolean_mainModule(PREFERENCE_IS_MINIMAL_MENU_SETTING);
}

//!toggles the menu mode
void toggleMinimalMenuMode_mainModule()
{
    togglePreferenceBoolean_mainModule(PREFERENCE_IS_MINIMAL_MENU_SETTING);
}

//! returns the current max of the MIN menu modes (using the setting of min or expanded) to determine
int minMenuModesMax_mainModule()
{
    return MAX_SM_MIN_MODES;
}

//! returns the current max of the menu modes (using the setting of min or expanded) to determine
int maxMenuModes_mainModule()
{
    SerialLots.printf("maxMenuModes_mainModule: %s, %d, %d\n", isMinimalMenuMode_mainModule?"1":"0", MAX_SM_MIN_MODES, MAX_SM_EXPANDED_MODES);
    if (isMinimalMenuMode_mainModule())
        return MAX_SM_MIN_MODES;
    else
        return MAX_SM_EXPANDED_MODES;
    
}

//!the saved SMMode
int _saveWhichSMMode = 0;
//!sets the current screen mode .. which can be used by Button and Display processing
void setCurrentSMMode_mainModule(int whichSMMode)
{
    SerialLots.print("setCurrentSMMode_mainModule:");
    SerialLots.println(whichSMMode);
    
    // This version sets the MIN OR MAX
    if (whichSMMode >= MAX_SM_MIN_MODES)
    {
       // change to EXPANDED MODES
       savePreferenceBoolean_mainModule(PREFERENCE_IS_MINIMAL_MENU_SETTING, false);
    }
    _saveWhichSMMode = whichSMMode;
}
//!returns the current SM Mode
int getCurrentSMMode_mainModule()
{
    return _saveWhichSMMode;
}

//!increment the currentSMMode, wrapping and using the max menu
void incrementSMMode_mainModule()
{
    _saveWhichSMMode++;
    int max = maxMenuModes_mainModule();
    if (_saveWhichSMMode >= max)
    {
        _saveWhichSMMode = 0;
    }
    SerialTemp.print("incrementSMMode_mainModule:");
    SerialTemp.println(_saveWhichSMMode);
}
//!increment the currentSMMode, wrapping and using the max menu
void decrementSMMode_mainModule()
{
    if (_saveWhichSMMode > 0)
    {
        _saveWhichSMMode--;
    }
}


//!full: ""Name: PTFeeder:HowieFeeder, Address: 7c:9e:bd:48:af:92, serviceUUID: 0xdead"
char *getFullBLEDeviceName_mainModule()
{
#ifdef USE_BLE_CLIENT_NETWORKING

    if (isConnectedBLEClient())
        return _fullBLEDeviceName;
    else
#endif
    {
        return (char*)"";
    }
}

//!whether connected GEN3
boolean _connecteBLEisGEN3 = false;
//!whether the connected is a GEN3 (so the name isn't valid)
boolean connectedBLEDeviceIsGEN3_mainModule()
{
    return _connecteBLEisGEN3;
}

//! BLE Discovery Methods
//! Connected to a BLE device with the advertised name. The syntax can include (PTFeeder:NAME) or just PTFeeder
//! Being disconnected is already a flag isConnectedBLE ...
void setConnectedBLEDevice_mainModule(char *deviceName, boolean isGEN3)
{
    //!set the isGEN3 flag
    _connecteBLEisGEN3 = isGEN3;
    //!now set the gateway based on if GEN3
    SerialTemp.printf("auto-settingGateway(%d)\n", isGEN3);
    savePreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE, isGEN3);

    strcpy(_fullBLEDeviceName, deviceName);
    
    //!seems device name = "Name: PTFeeder:HowieFeeder, Address: 7c:9e:bd:48:af:92, serviceUUID: 0xdead
    //!Note: the Address, eg "7c:9e:bd:48:af:92" is unique somehow.. lets use that to skip one..
    SerialCall.printf("setConnectedBLEDevice_mainModule: %s\n", deviceName);
    strcpy(_connectedBLEDeviceName,"");
    
    if (containsSubstring(deviceName,"PTFeeder:") || containsSubstring(deviceName,"PTClicker:"))
    {
        //!parse out the 2nd half of name
        char *colon = index(deviceName,':');
        colon++;
        colon = index(colon,':');
        //! go past the :
        colon++;

        while (*colon && *colon != ',')
        {
            int len = strlen(_connectedBLEDeviceName);
            _connectedBLEDeviceName[len] = colon[0];
            _connectedBLEDeviceName[len+1] = '\0';
            colon++;
        }
    }
    else
    {
        //! empty name (just PTFeeder)
        strcpy(_connectedBLEDeviceName,(char*)"");
    }
    SerialTemp.print("connectedBLEName= ");
    SerialTemp.println(_connectedBLEDeviceName);

    //! 8.29.23 send a message saying we connected..
#ifdef LATER
    seems to be crashing..
    {
        char connectMessage[300];
        sprintf(connectMessage,"#connectedBLE {%s to %s}", getDeviceNameMQTT(), _connectedBLEDeviceName);
        SerialTemp.println(connectMessage);
        //sendSemanticMarkerDocFollow_mainModule(&fileURL[0]);
        //! for now only send if it start message starts with "#"
        publishMQTTMessageDefaultTopic(connectMessage);
    }
#endif
    //!parse for the address too..
    //! strcpy(_fullBLEDeviceName, deviceName);
    
    //!seems device name = "Name: PTFeeder:HowieFeeder, Address: 7c:9e:bd:48:af:92, serviceUUID: 0xdead
    //!Note: the Address, eg "7c:9e:bd:48:af:92" is unique somehow.. lets use that to skip one..
    strcpy(_connectedBLEDeviceAddress,"");
    if (containsSubstring(_fullBLEDeviceName,"Address:"))
    {
        //!parse out the 2nd half of name
        char *colon = strstr(_fullBLEDeviceName,"Address:");
        colon += strlen("Address:");
        //strip spaces
        while (*colon && *colon == ' ')
        {
            colon++;
        }
        //now until the , is the address
        while (*colon && *colon != ',')
        {
            int len = strlen(_connectedBLEDeviceAddress);
            _connectedBLEDeviceAddress[len] = colon[0];
            _connectedBLEDeviceAddress[len+1] = '\0';
            colon++;
        }
    }
    else
    {
        //! empty name (just PTFeeder)
        strcpy(_connectedBLEDeviceAddress,(char*)"");
    }
    SerialTemp.print("_connectedBLEDeviceAddress= ");
    SerialTemp.println(_connectedBLEDeviceAddress);
}

//!ISSUE: if BLE, can only return the address.. it's up to the caller to know it might not match the Paired Name (eg DukeGEN3)
//! returns the connected BLE Device name (the :NAME of advertisment, Address: 01:39:3f:33 part of name
char* connectedBLEDeviceName_mainModule()
{
    char *nameToUse;
    if (strlen(_connectedBLEDeviceName)!= 0)
        nameToUse = _connectedBLEDeviceName;
    else if (strlen(_connectedBLEDeviceAddress)!= 0)
        nameToUse =_connectedBLEDeviceAddress;
    else
        nameToUse = (char*)"";
   
#ifdef USE_BLE_CLIENT_NETWORKING

    //! if connected, return the connected name, otherwise return empty string
    if (isConnectedBLEClient())
    {
        return nameToUse;
    }
    else
#endif
    {
        return (char*)"";
    }
}

//!returns address part of name.
char *connectedBLEDeviceNameAddress_mainModule()
{
#ifdef USE_BLE_CLIENT_NETWORKING

    //! if connected, return the connected name, otherwise return empty string
    if (isConnectedBLEClient())
    {
        return _connectedBLEDeviceAddress;
    }
    else
#endif
    {
        return (char*)"";
    }
}


//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void buttonA_ShortPress_mainModule()
{
#ifdef USE_BUTTON_MODULE
    buttonA_ShortPress_ButtonModule();
#endif
}
//!long press on buttonA (top button)
void buttonA_LongPress_mainModule()
{
#ifdef USE_BUTTON_MODULE
    buttonA_LongPress_ButtonModule();
#endif
}
//!the long press of the side button
void buttonB_LongPress_mainModule()
{
#ifdef USE_BUTTON_MODULE
    buttonB_LongPress_ButtonModule();
#endif
}
//!the short press of the side button
void buttonB_ShortPress_mainModule()
{
#ifdef USE_BUTTON_MODULE
    buttonB_ShortPress_ButtonModule();
#endif
}

//!restarts all the menu states to the first one .. useful for getting a clean start. This doesn't care if the menu is being shown
void restartAllMenuStates_mainModule()
{
    //!restarts all the menu states to the first one .. useful for getting a clean start. This doesn't care if the menu is being shown
    restartAllMenuStates_ModelController();
}
