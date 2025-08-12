//! \link MainModule
#include "../../Defines.h"
//#include "MainModule.h"


#define USE_NEW_M5ATOMCLASS
//! 5.6.25 use the M5Atom ClassType
#ifdef USE_NEW_M5ATOMCLASS
//! @see https://www.cs.fsu.edu/~myers/cop3330/notes/dma.html
#include "../M5AtomClassModule/M5AtomClassType.h"

#include "../M5AtomClassModule/M5Atom_SocketModuleClass.h"
#include "../M5AtomClassModule/M5Atom_QRCodeModuleClass.h"
#include "../M5AtomClassModule/M5Atom_HDriverModuleClass.h"
//! 5.9.25 -- Dead 77
#include "../M5AtomClassModule/M5Atom_Core2ModuleClass.h"
//! 7.17.25 - Adding GPS
#include "../M5AtomClassModule/M5Atom_TinyGPSModuleClass.h"

//! 7.24.25 Hot Day, Ballon last night, Mt Out
//! for the 'C' option of atom color
#ifdef USE_FAST_LED
#include "../ATOM_LED_Module/M5Display.h"
#endif


//! instances of the M5AtomClassType

//1
M5Atom_SocketModuleClass* _M5Atom_SocketModuleClass;
//2
M5Atom_QRCodeModuleClass* _M5Atom_QRCodeModuleClass;
//3
M5Atom_HDriverModuleClass* _M5Atom_HDriverModuleClass;
//4
M5Atom_Core2ModuleClass* _M5Atom_Core2ModuleClass;
//! 7.17.25 - Adding GPS
//! 5
M5Atom_TinyGPSModuleClass* _M5Atom_TinyGPSModuleClass;
//! make sure this is updated.
#define NUM_M5ATOM_CLASS 5
//! 3.31.25 create array of plugs
M5AtomClassType* _M5AtomClassTypes[NUM_M5ATOM_CLASS];

//! use this one...
M5AtomClassType *_whichM5AtomClassType;

#endif //USE_NEW_M5ATOMCLASS

#define USE_SENSOR_CLASS
#ifdef  USE_SENSOR_CLASS
//! 5.14.25 
#include "../SensorClass/SensorClassType.h"
#include "../SensorClass/BuzzerSensorClass.h"
#include "../SensorClass/KeyUnitSensorClass.h"

#define NUM_SENSOR_CLASS 2
BuzzerSensorClass* _BuzzerSensorClass;
KeyUnitSensorClass* _KeyUnitSensorClass;

//! init will look at those SensorClass known above, and look
//! to see which ones are defined by the user in the SensorsStruct
//! A report will show which ones were not found
void initSensorClassTypeArray()
{
    SerialDebug.println(" *** initSensorClassTypeArray ***");
    //! return the sensor specified or null
    SensorsStruct* sensorsStruct = getSensors_mainModule();
    if (sensorsStruct)
    {
        //! all the sensors defined
        //! sensors is an array of sensor
        SensorStruct *sensors = sensorsStruct->sensors;
        int count = sensorsStruct->count;

        //! go through the defined sensorName and instantiate those classes
        for (int i=0; i< count; i++)
        {
            SensorStruct* sensor = &sensors[i];
            if (strcmp(sensor->sensorName, "BuzzerSensorClass")==0)
            {
                //! init this class
                _BuzzerSensorClass = new BuzzerSensorClass((char*)"BuzzerSensorClass");
                //! store the class type instance
                sensor->sensorClassType = _BuzzerSensorClass;
                //! setup and then setPins..
                _BuzzerSensorClass->setPinValues(sensor->pin1, sensor->pin2);
                
                //! call setup
                _BuzzerSensorClass->setup();
            }
        }
        //! find missing sensors
        //! go throug the defined sensorName and instantiate those classes
        for (int i=0; i< count; i++)
        {
            SensorStruct* sensor = &sensors[i];
            if (!sensor->sensorClassType)
            {
                //! missing instance
                SerialDebug.printf("Missing class definition: %s\n", sensor->sensorName);
            }
        }
    }
    SerialDebug.println("Finish initSensorClassTypeArray");
}


#endif

//! 6.7.25 hot air balloons over house..
//! stops the motor
void stopMotor_mainModule()
{
    if (_whichM5AtomClassType)
        _whichM5AtomClassType->stop_M5AtomClassType();
}
//! gets if PTFeeder a surrogate for the M5Atom class
boolean isPTFeeder_mainModule()
{
    if (_whichM5AtomClassType)
        return _whichM5AtomClassType->isPTFeeder_M5AtomClassType();
    else
        //!Defines the name of the service of the server, which for M5 will be PTClicker
    {
        return containsSubstring(MAIN_BLE_SERVER_SERVICE_NAME, "PTFeeder");
    }
    /*
#ifdef ESP_M5
#define MAIN_BLE_SERVER_SERVICE_NAME (char*)"PTClicker"
#else
#define MAIN_BLE_SERVER_SERVICE_NAME (char*)"PTFeeder"
#endif
     */
}

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
#define MESSAGE_STORAGE_MAX 500
char _messageStorage[MESSAGE_STORAGE_MAX];
//!status string (URL query format)
char _fullStatusString[500];
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
    
    main_setScannedDeviceName((char*)"");
    //! 1.7.24
    main_setScannedGroupName((char*)"");

    
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
        
#ifdef M5_ATOM
        //! 5.6.25 use object version
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE,  true); //false);

#else  // not M5_ATOM

        // 8.28.23 .. not doing this anymore..
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE,  false);

        //! not ESP_M5
#endif // M5_ATOM
#else // NOT ESP_M5
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE, true);
#endif
        //! all ..
        savePreferenceBoolean_mainModule(PREFERENCE_FIRST_TIME_FEATURE_SETTING, false);

    }
    
#define NEW_SENSORS
    //! 5.14.25 use the Sensor types
    //! first we have N sensors
    //! then we get which ones the user wants (the SENSORS_SETTING)
    //! then we instantiate, and provide ping, etc.
    initSensorClassTypeArray();
}

#if defined(ESP_M5_CAMERA) || defined(ESP_32)
//!a couinter to slow down the loop doing things..
int _mainLoopCounter = 0;
#endif

//! storage for reading from the serial buffer
//! 6.20.25 (Sam Sprague grad Western party)
char _serialBuffer[300];

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

#pragma mark Serial Input Processor
    //! 6.20.25 works with full code (JSON, or single commands - with '.' as help)
    //! 6.19.25 Mt Peak finally (kept to 124 bpm)
    //! Per ##373 let the serial monitor get some input, and let us set a few features
    //! see if data on the serial input
    //! Re-indexed and now command completion works!!!!!! 3 years later.
    //!@see  https://developer.apple.com/documentation/corespotlight/regenerating-your-app-s-indexes-on-demand?language=objc
    if (Serial.available())
    {
        // read string until meet newline character
        String command = Serial.readStringUntil('\n');
        
        SerialDebug.println(command);
        //! save globally for the callback below..
        strcpy(_serialBuffer, command.c_str());
        
        if (command == "help")
        {
            SerialDebug.println("Enter any Single Char or JSON msg, type '.' for commands");
            SerialDebug.println("r -- reboot");
            SerialDebug.println(". -- shows single char commands");
            SerialDebug.println("Example JSON to change WIFI (copy and modify with your values)");
            SerialDebug.println("   supports single quotes for values");
            SerialDebug.println("{'ssid':'Bob', 'ssidPassword':'scott'}");
            
        }
        else if (command == "r")
        {
            rebootDevice_mainModule();
        }
        else
        {
            //!call the callback specified from the caller (eg. NimBLE_PetTutor_Server .. or others)
            callCallbackMain(CALLBACKS_BLE_SERVER, BLE_SERVER_CALLBACK_ONWRITE, _serialBuffer);
        }
    }
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
#include "../ButtonModule/ButtonProcessing.h"
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
        SerialError.printf("#### Error outside callback range - 1, %d\n", callbackType);
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
        SerialError.printf("#### Error outside callback range - 2, %d\n", callbackType);

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
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void messageSetVal_mainModule(char *setName, char* valValue, boolean deviceNameSpecified)
{
    SerialCall.printf("messageSetVal(%s,%s)\n", setName, valValue);
    // THE IDEA WOULD be a callback is avaialble..
    //FOR now.. just ifdef
#ifdef M5_ATOM
#define USE_NEW_M5ATOMCLASS

    //! 5.6.25 use object version
    if (_whichM5AtomClassType)
        _whichM5AtomClassType->messageSetVal_M5AtomClassType(setName, valValue, deviceNameSpecified);

#endif //M5_ATOM
    
#ifdef M5CORE2_MODULE
    messageSetVal_M5Core2Module(setName, valValue, deviceNameSpecified);
#endif
}
//!TODO: have a callback regist approach

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//!  5.21.25 SEND and CMD will be treated the same and put to "send"
void messageSend_mainModule(char *sendValue, boolean deviceNameSpecified)
{
//! 5.21.25 this will overlap with the "cmd" .. so send == cmd
#ifdef M5_ATOM
#pragma mark USE_NEW_M5ATOMCLASS

    //! 5.6.25 use object version
    if (_whichM5AtomClassType)
        _whichM5AtomClassType->messageSend_M5AtomClassType(sendValue, deviceNameSpecified);
    
#endif //M5_ATOM
    
#ifdef M5CORE2_MODULE
    messageSend_M5Core2Module(sendValue);

#endif
}

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
    refreshDelayButtonTouched_ButtonProcessing();
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
        SerialLots.printf("isValidDeviceAddress(%s,%d)\n", pairedDeviceAddress?pairedDeviceAddress:(char*)"NULL", isValid);
   }
    SerialLots.printf("isValidPairedDevice_mainModule(%s,%d)\n", pairedDevice?pairedDevice:(char*)"NULL", isValid);
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
#ifndef M5STICKCPLUS2

#ifndef ESP_M5_ATOM_S3
    M5.Axp.PowerOff();
#endif
#endif
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
    //! 7.29.25 set the FIRST time ..
    savePreferenceBoolean_mainModule(PREFERENCE_FIRST_TIME_FEATURE_SETTING, true);
    
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


//! 12.14.23
char *_MQTT_Password = (char*)"";
char *_MQTT_Username = (char*)"";
char _scannedDeviceName[100];
char _scannedGroupTopicName[100];

//! return the username and password
//! 12.14.23 to support calling the SMART buttons (smrun) with parameters
char *main_getUsername()
{
    return _MQTT_Username;
}
//! return password
char *main_getPassword()
{
    return _MQTT_Password;
}
//! return devicename
char *main_getScannedDeviceName()
{
    SerialDebug.printf("main_getScannedDeviceName(%s)\n", _scannedDeviceName);

    return _scannedDeviceName;
}
//! set the scanned device name
void main_setScannedDeviceName(char *deviceName)
{
    strcpy(_scannedDeviceName, deviceName);
    SerialDebug.printf("main_setScannedDeviceName(%s)\n", deviceName);
}

//! set the scanned group name
//! 1.7.24
void main_setScannedGroupName(char *groupName)
{
    SerialDebug.printf("main_setScannedGroupName(%s)\n", groupName);
    
    if (groupName && strlen(groupName)>0)
    {
        char *groupTopic = groupTopicFullName(groupName);
        strcpy(_scannedGroupTopicName, groupTopic);
    }
    else
        strcpy(_scannedGroupTopicName, "");
}
//! return groupname -- returns "" or nil if not set,
//! or the FULL group name topic, or nil
char *main_getScannedGroupNameTopic()
{
    if (_scannedGroupTopicName && strlen(_scannedGroupTopicName)>0)
    {
        SerialDebug.printf("main_getScannedGroupNameTopic(%s)\n", _scannedGroupTopicName);
        return _scannedGroupTopicName;

    }
    return NULL;
}


//! sets the WIFI and MQTT user/password. It's up to the code (below, maybe in future a register approach)  to decide who needs to know
void main_updateMQTTInfo(char *ssid, char *ssid_password, char *username, char *password, char *guestPassword, char *deviceName, char * host, char * port, char *locationString)
{
    SerialMin.printf("main_updateMQTTInfo(%s,%s,%s,%s,%s, %s, d=%s)\n", ssid?ssid:"NULL", ssid_password?ssid_password:"NULL", username?username:"NULL", password?password:"NULL", guestPassword?guestPassword:"NULL", locationString?locationString:"NULL", deviceName?deviceName:"NULL");
    
    _MQTT_Password = password;
    _MQTT_Username = username;
    
    //!store the device name
   savePreference_mainModule(PREFERENCE_DEVICE_NAME_SETTING, deviceName);

    //!store the JSON version of these credentials..
    main_saveWIFICredentials(ssid,ssid_password);
    
#ifdef USE_WIFI_AP_MODULE
    WIFI_APModule_updateMQTTInfo(ssid, ssid_password, username, password, guestPassword, deviceName, host, port, locationString);
#endif
}

//! 5.16.25 Fountainhead, Raining cold weekend
//! start SYNC calls starting with the SYNC_CLICK_SOUND
//#define SYNC_CLICK_SOUND 0
//#define SYNC_CALL_MAX 1
//! the main sync command (no parameters yet)
void main_dispatchSyncCommand(int syncCallCommand)
{
    switch (syncCallCommand)
    {
        case SYNC_CLICK_SOUND:
        {
            if (getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE))
            {
                //! 5.15.25 see if the Buzzer class is defined .. if so, then call local message
                SensorStruct* buzzerSensor = getSensor_mainModule((char*)"BuzzerSensorClass");
                
                //!perform the click sound..
                //!this will be the sensors...
                if (buzzerSensor)
                {
                    buzzerSensor->sensorClassType->messageLocal_SensorClassType((char*)"click");
                }
                else
                    SerialDebug.println("NO BuzzerSensorClass defined");
            }
        }
            break;
    }
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
        SerialTemp.printf("1.main_dispatchAsyncCommand: %d\n", asyncCallCommand);
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
        SerialTemp.print("2.main_dispatchAsyncCommandWithString:");
        SerialTemp.print(asyncCallCommand);
        SerialTemp.print(": ");
        SerialTemp.println(_asyncParameter);
        _asyncCallFlagsParameters[asyncCallCommand] = true;
        
        //! 6.15.25 Fathers Day
        //! seems issue when a GROVE or other sensor plugged in the main loop isn't working.
        //! SO for now, if the command is "r" then do an actual reboot..
        if (strlen(_asyncParameter)>1 && _asyncParameter[0] == 'r')
        {
            SerialDebug.println("SYNC_REBOOT");
            rebootDevice_mainModule();
        }
        
    }
}

//!checks if any async commands are in 'dispatch' mode, and if so, invokes them, and sets their flag to false
void invokeAsyncCommands()
{
    SerialCall.printf("invokeAsyncCommands(%d)\n", ASYNC_CALL_PARAMETERS_MAX);

    for (int i = 0; i < ASYNC_CALL_PARAMETERS_MAX; i++)
    {
        boolean asyncCallFlag = _asyncCallFlagsParameters[i];
        if (asyncCallFlag)
        {
            SerialCall.printf("invokeAsyncCommands(%d)\n", i);
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
                                      
                    //! 1.10.24 add ability to send a MQTT Semantic Marker message
                case ASYNC_JSON_MQTT_MESSAGE_PARAMETER:
                {
                    //process the message
                    SerialDebug.print("ASYNC_JSON_MQTT_MESSAGE_PARAMETER: ");
                    SerialDebug.println(_asyncParameter);
                    
                    //! This sends a MQTT message. Currently GROUP not supported
                    //! TODO: add GROUP
                    char *groupTopic = NULL;
                    if (groupTopic)
                        sendMessageStringTopic_mainModule(_asyncParameter, groupTopic);
                    else
                        sendMessageString_mainModule(_asyncParameter);
                }
                    break;
                    
                    //! 3.9.24 REST call 
                case ASYNC_REST_CALL_MESSAGE_PARAMETER:
                {
                    //process the message
                    SerialDebug.print("ASYNC_REST_CALL_MESSAGE_PARAMETER: ");
                    SerialDebug.println(_asyncParameter);
#ifdef USE_REST_MESSAGING
                    sendSecureRESTCall(_asyncParameter);
#endif

                    
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
                    

                    //!this is a sending of the message
                case ASYNC_SEND_MQTT_FEED_MESSAGE:
                    SerialTemp.println("ASYNC_SEND_MQTT_FEED_MESSAGE");
                    //incrementFeedCount_mainModule();
                    SerialCall.println("async_send_feed.1");
                    

#ifdef USE_MQTT_NETWORKING
#ifdef USE_BLE_CLIENT_NETWORKING
                    //!9.30.22  IF SET .. send a feed but to all devices except ours and our pair (if any)
                    //! uses new wildcard syntax either  ! OUR NAME  [ & ! OUR_CONNECTED_NAME
                    if (getPreferenceBoolean_mainModule(PREFERENCE_SENDWIFI_WITH_BLE) && isConnectedBLEClient())
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
                        //! if not paired, then feed everyone except out device...
                    }
                    //! if not BLE connected .. send wifi fee to all below...
#endif //BLE
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
                        //#ifdef USE_MQTT_NETWORKING_NOT_NOW
#ifdef USE_MQTT_NETWORKING
                        
                        //!only if not WIFI with BLE
                        //! 8.20.24 put back to MQTT feed to all .. for now..
                        //! SerialCall.println("async_send_feed.1");
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

                    
                    //!this is a sending of the message
                    //!5.15.25 Sodbuster Rod plowing/disking in minutes with Mark and Bud
                case ASYNC_CLICK_SOUND:
                {
                    SerialTemp.println("ASYNC_CLICK_SOUND");
                    main_dispatchSyncCommand(SYNC_CLICK_SOUND);
                }
                    break;
                    
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
                    SerialDebug.println("ASYNC_REBOOT");
                    rebootDevice_mainModule();
                    
                    break;
                    
                case ASYNC_POWEROFF:
                    SerialDebug.println("ASYNC_POWEROFF");
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
#ifdef ESP_M5_CAMERA
                    char *statusURL = main_currentStatusURL(false);
#else
                    char *statusURL = main_currentStatusURL(true);
#endif
                    SerialDebug.print(" ASYNC_SEND_MQTT_STATUS_URL: ");
                    SerialDebug.println(statusURL);
                    /// NO MORE: sendDocFollowMessageMQTT(statusURL);
                    sendStatusMessageMQTT(statusURL);
                }
#endif
#ifdef M5BUTTON_MODULE
                    //! 1.23.24 call the status which re-evaluates the sensor ALIVE
                    //! this status will be called and let the ALIVE re-evaluate
                    //! Only do this on the STATUS since it might be slow for the LUX
                    statusM5ButtonModule();
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
//! 2.21.25 add a way to change the button color (if any)
void changeButtonColor_MainModule()
{
#ifdef M5BUTTON_MODULE
    SerialDebug.println("changeButtonColor_MainModule");
  //  changeButtonColor_M5ButtonModule()
#endif
}

//!3.17.24  the unqiue chip id
uint32_t _chipID_MainModule = 0;
//! string like: 10311304
char _chipIdString_MainModule[15];


//! 3.17.24 get the chip id
uint32_t getChipId()
{
    if (_chipID_MainModule == 0)
    {
        //get chip ID
        for (int i = 0; i < 17; i = i + 8) {
            _chipID_MainModule |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
        }
        sprintf(_chipIdString_MainModule,"%ld", _chipID_MainModule);
    }
    return _chipID_MainModule;
    
}

//! 3.17.24 get the chip id as a string
char* getChipIdString()
{
    getChipId();
    return _chipIdString_MainModule;
}

//! 1.12.24 add a temporary LUX dark
//! threshholdKind = 0 (LIGHT), 1=(DARK) .. others might be 2=super dark
//#define THRESHOLD_KIND_LIGHT 0
//#define THRESHOLD_KIND_DARK 1
int _thresholdLUXDark = 80;
int _thresholdLUXLight = 0;
//! set the threshold val
void setLUXThreshold_mainModule(int thresholdKind, int luxVal)
{
    switch (thresholdKind)
    {
        case THRESHOLD_KIND_LIGHT:
            _thresholdLUXLight = luxVal;
            break;
        case THRESHOLD_KIND_DARK:
            _thresholdLUXDark = luxVal;
            break;
    }
}
//! get the threshold val
int  getLUXThreshold_mainModule(int thresholdKind)
{
    switch (thresholdKind)
    {
        case THRESHOLD_KIND_LIGHT:
            return _thresholdLUXLight;
            break;
        case THRESHOLD_KIND_DARK:
            return _thresholdLUXDark;
            break;
    }
    return 80;
}

//!storage for the group topic name
//!Note NO "/" in the front of the path only "usersP"
char _groupTopicName[100];
#define GROUP_TOPIC_TO_SEND (char*)"usersP/groups"
//!returns a groupTopic to use as a topic
char *groupTopicFullName(char *groupName)
{
    sprintf(_groupTopicName, "%s/%s",GROUP_TOPIC_TO_SEND, groupName);
    return _groupTopicName;
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
    SerialDebug.println(" ** BUILDS **");

#ifdef ESP_M5
    //![x] ESP_M5
    SerialMin.println("[x] ESP_M5");
#else
    SerialMin.println("[ ] ESP_M5");
#endif
#ifdef ESP_M5_CAMERA
    //![x] ESP_M5
    SerialMin.println("[x] ESP_M5_CAMERA");
#else
    SerialMin.println("[ ] ESP_M5_CAMERA");
#endif
#ifdef ESP_32
    //! [x] ESP_32
    SerialMin.println("[x] ESP_32");
#else
    SerialMin.println("[ ] ESP_32");
#endif
#ifdef M5CORE2_MODULE
    //![x] M5CORE2_MODULE
    SerialMin.println(F("[x] M5CORE2_MODULE"));
#else
    SerialMin.println(F("[ ] M5CORE2_MODULE"));
#endif
    //Set this if the SMART clicker build is used.
#ifdef ESP_M5_SMART_CLICKER_CONFIGURATION
    SerialMin.println(F("[x] ESP_M5_SMART_CLICKER_CONFIGURATION"));
#else
    SerialMin.println(F("[ ] ESP_M5_SMART_CLICKER_CONFIGURATION"));
#endif
#ifdef ESP_M5_ATOM_LITE
    SerialMin.println(F("[x] ESP_M5_ATOM_LITE"));
#else
    SerialMin.println(F("[ ] ESP_M5_ATOM_LITE"));
#endif
    
    SerialDebug.println(F(" ** MQTT BLE NETWORKING **"));

#ifdef USE_MQTT_NETWORKING
    //! [x] USE_MQTT_NETWORKING
    SerialMin.println("[x] USE_MQTT_NETWORKING");
#else
    SerialMin.println("[ ] USE_MQTT_NETWORKING");
#endif
#ifdef USE_BLE_SERVER_NETWORKING
    //! [x] USE_BLE_SERVER_NETWORKING
    SerialMin.println("[x] USE_BLE_SERVER_NETWORKING");
#else
    SerialMin.println("[ ] USE_BLE_SERVER_NETWORKING");
#endif
#ifdef USE_BLE_CLIENT_NETWORKING
    //! [x] USE_BLE_CLIENT_NETWORKING
    SerialMin.println(F("[x] USE_BLE_CLIENT_NETWORKING"));
#else
    SerialMin.println(F("[ ] USE_BLE_CLIENT_NETWORKING"));
    
#endif
#ifdef USE_WIFI_AP_MODULE
    //! [x] USE_WIFI_AP_MODULE
    
    SerialMin.println(F("[x] USE_WIFI_AP_MODULE"));
#else
    SerialMin.println(F("[ ] USE_WIFI_AP_MODULE"));
    
#endif
    
#ifdef USE_BUTTON_MODULE
    // [x] USE_BUTTON_MODULE
    SerialMin.println("[x] USE_BUTTON_MODULE");
#else
    SerialMin.println("[ ] USE_BUTTON_MODULE");
    
#endif
    
#ifdef M5BUTTON_MODULE
    // [x] M5BUTTON_MODULE
    SerialMin.println("[x] M5BUTTON_MODULE");
#else
    SerialMin.println("[ ] M5BUTTON_MODULE");
    
#endif

#ifdef USE_STEPPER_MODULE
    SerialMin.println("[x] USE_STEPPER_MODULE");
#else
    SerialMin.println("[ ] USE_STEPPER_MODULE");
    
#endif
#ifdef USE_UI_MODULE
    SerialMin.println("[x] USE_UI_MODULE");
#else
    SerialMin.println("[ ] USE_UI_MODULE");
    
#endif
#ifdef USE_DISPLAY_MODULE
    SerialMin.println("[x] USE_DISPLAY_MODULE");
#else
    SerialMin.println("[ ] USE_DISPLAY_MODULE");
    
#endif
#ifdef BOARD
    SerialMin.println(F("[x] BOARD"));
#else
    SerialMin.println(F("[ ] BOARD"));
#endif
#ifdef USE_SPIFF_MODULE
    SerialMin.printf("[%d] USE_SPIFF_MODULE/_SETTING\n",  getPreferenceBoolean_mainModule(PREFERENCE_USE_SPIFF_SETTING));
#else
    SerialMin.println("[ ] USE_SPIFF_MODULE");
#endif
#ifdef USE_SPIFF_MQTT_SETTING
    SerialMin.printf("[%d] USE_SPIFF_MQTT_SETTING\n",getPreferenceBoolean_mainModule(PREFERENCE_USE_SPIFF_MQTT_SETTING));
#endif
#ifdef USE_SPIFF_QRATOM_SETTING
    SerialMin.printf("[%d] USE_SPIFF_QRATOM_SETTING\n", getPreferenceBoolean_mainModule(PREFERENCE_USE_SPIFF_QRATOM_SETTING));
#endif

#ifdef USE_FAST_LED
    SerialMin.println(F("[x] USE_FAST_LED"));
#else
    SerialMin.println(F("[ ] USE_FAST_LED"));
#endif
#ifdef KEY_UNIT_SENSOR_CLASS
    SerialMin.println(F("[x] KEY_UNIT_SENSOR_CLASS"));
#else
    SerialMin.println(F("[ ] KEY_UNIT_SENSOR_CLASS"));
#endif
#ifdef USE_LED_BREATH
    SerialMin.println(F("[x] USE_LED_BREATH"));
#else
    SerialMin.println(F("[ ] USE_LED_BREATH"));
#endif
    
#if (SERIAL_DEBUG_ERROR)
    SerialMin.println(F("[x] SERIAL_DEBUG_ERROR"));
#endif
#if (SERIAL_DEBUG_DEBUG)
    SerialMin.println(F("[x] SERIAL_DEBUG_DEBUG"));
#endif
#if (SERIAL_DEBUG_INFO)
    SerialMin.println(F("[x] SERIAL_DEBUG_INFO"));
#endif
#if (SERIAL_DEBUG_MINIMAL)
    SerialMin.println(F("[x] SERIAL_DEBUG_MINIMAL"));
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
    float batVoltage = 1;
    float batPercentage = 100; //plugged in
#ifdef ESP_M5
#ifndef M5STICKCPLUS2

#ifdef M5_ATOM
    //! the M5.Axp.GetBatVoltage() is VERY slow on the M5 (as there isn't one..)
#elif defined(M5CORE2_MODULE)
    //!see https://forum.arduino.cc/t/elseif-not-working/565646/12
    batVoltage = M5.Axp.GetBatVoltage();
    batPercentage = (batVoltage < 3.2) ? 0 : ( batVoltage - 3.2 ) * 100;
#else
    batVoltage = M5.Axp.GetBatVoltage();
    batPercentage = (batVoltage < 3.2) ? 0 : ( batVoltage - 3.2 ) * 100;
    //!#Issue 117
    //!from: https://community.m5stack.com/topic/1361/ischarging-and-getbatterylevel/9
    //!GetVbatdata() is currently depreciated
    uint16_t vbatData = M5.Axp.GetVbatData();
    double vbat = vbatData * 1.1 / 1000;
    batPercentage =  100.0 * ((vbat - 3.0) / (4.07 - 3.0));
#endif
#endif // M5STICKCPLUS2
#endif
 
    if (batPercentage > 100.0)
        batPercentage = 100.0;
    return batPercentage;
}



//!adding a synchronous call to send a message over the network (assuming MQTT but not specified), this tacks on {device} and {t:time}
void sendMessageString_mainModule(char *messageString)
{
    sendMessageStringTopic_mainModule(messageString, TOPIC_TO_SEND);
#ifdef REFACTOR

#ifdef USE_MQTT_NETWORKING
    SerialDebug.printf("sendMessageString_mainModule(%s)\n", messageString);
    if (strlen(messageString) > 0 && messageString[0] == '{')
    {
        // JSON Message
        //! 12.19.23 Amber in air from Iceland. 50 years since Dead 12.19.73
        sendMessageNoChangeMQTT(messageString);
    }
    else if (strlen(messageString) < MESSAGE_STORAGE_MAX)
    {
        //!NOTE: the # has to be there, otherwise the sendMessageMQTT ignores it..
        sprintf(_messageStorage,"#%s {%s} {t:%d}", messageString, getDeviceNameMQTT(), getTimeStamp_mainModule());
        //!send this message over MQTT
        sendMessageMQTT(_messageStorage);
    }
#endif
#endif
}


//!adding a synchronous call to send a message over the network (assuming MQTT but not specified), this tacks on {device} and {t:time}
void sendMessageStringTopic_mainModule(char *messageString, char*topicString)
{
#ifdef USE_MQTT_NETWORKING
    SerialDebug.printf("sendMessageStringTopic_mainModule(%s) - topic=%s\n", messageString, topicString);
    if (strlen(messageString) > 0 && messageString[0] == '{')
    {
        // JSON Message
        //! 12.19.23 Amber in air from Iceland. 50 years since Dead 12.19.73
        sendMessageNoChangeMQTT_Topic(messageString, topicString);
    }
    else if (strlen(messageString) < MESSAGE_STORAGE_MAX)
    {
        //!NOTE: the # has to be there, otherwise the sendMessageMQTT ignores it..
        sprintf(_messageStorage,"#%s {%s} {t:%d}", messageString, getDeviceNameMQTT(), getTimeStamp_mainModule());
        //!send this message over MQTT
        sendMessageMQTT_Topic(_messageStorage, topicString);
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
#ifdef M5_ATOM
#else
#ifndef M5STICKCPLUS2
    M5.IMU.getTempData(&temperature);
#endif
#endif
    
#ifdef USER_THE_MAX_TEMP_FEATURE
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
#endif

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
//!status in JSON format, needs to return something as a ',' is already added before calling this..
char* main_currentStatusJSON()
{
    //returns a JSON string..
//    String b = String(getBatPercentage(),0);
//    return "{'status':[{'BLE':'on'},{'bat':'" + b + "'}]}";
   // return (char*)"";
    
    //! 1.4.24 work on ATOM kinds without IFDEF (except to bring in the code)
#pragma mark USE_NEW_M5ATOMCLASS

    //! 5.6.25 use object version
    if (_whichM5AtomClassType)
        return _whichM5AtomClassType->currentStatusJSON_M5AtomClassType();

    
#ifdef M5CORE2_MODULE
    //!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
    //!starts with "&"*
    return currentStatusJSON_M5Core2Module();
#endif
    
    return (char*)"'tbd':'x'";
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
#pragma mark reused
#ifdef OVERLOADS_TIME_T
    //! 7.20.25 this breaks the T:<time> since there is a dublicate in the URL .. with no error
    addStatusBooleanFlag("T", getPreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE));
#endif
    
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
    
    //! 8.10.25
    //! #393 add stepper angle
    //!show SA for stepper angle float ..
    addStatusStringFlag("sm",getPreferenceString_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING));
    
    
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
    addStatusStringFlag("fdir",  getPreferenceString_mainModule(PREFERENCE_STEPPER_FACTORY_CLOCKWISE_MOTOR_DIRECTION_SETTING));

#endif
}

//!returns a string in in URL so:  status?battery=84'&buzzon='off'  } .. etc
//!if fullStatus, return everything, else just the ATOM stuff
char* main_currentStatusURL(boolean fullStatus)
{
    //!7.20.25 add T too
    int time = getTimeStamp_mainModule();
    if (fullStatus)
    {
#ifdef USE_MQTT_NETWORKING
        char *deviceName = getDeviceNameMQTT();
        //!TODO: make sure no spaces ... unless escaped
      
        sprintf(_fullStatusString,"status?T=%d&v=%s&dev=%s&b=%02.0f&temp=%02.0f&c=%0d&t=%0d",time, VERSION_SHORT, deviceName, getBatPercentage_mainModule(), getTemperature_mainModule(), getFeedCount_mainModule(), getLoopTimer_displayModule());
        
#else
        //!TODO: make sure no spaces ... unless escaped
        sprintf(_fullStatusString,"status?v=%s&b=%02.0f&temp=%02.0f&c=%0d&t=%0d",VERSION_SHORT, getBatPercentage_mainModule(), getTemperature_mainModule(), getFeedCount_mainModule(), getLoopTimer_displayModule());
#endif
    }
    else
    {
        //_fullStatusString[0] = '\0';
#ifdef USE_MQTT_NETWORKING
        char *deviceName = getDeviceNameMQTT();
        //!TODO: make sure no spaces ... unless escaped
        sprintf(_fullStatusString,"status?T=%d&v=%s&dev=%s",time, VERSION_SHORT, deviceName);
        
#else
        //!TODO: make sure no spaces ... unless escaped
        sprintf(_fullStatusString,"status?v=%s",VERSION_SHORT);
#endif
    }
#pragma mark USE_NEW_M5ATOMCLASS
    //! 5.6.25 use object version
    if (_whichM5AtomClassType)
        strcat(_fullStatusString, _whichM5AtomClassType->currentStatusURL_M5AtomClassType());

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

//! 8.18.24 setting this will check for the factory setting..
void setClockwiseMotorDirection_mainModule(boolean isClockwiseFlag)
{
    boolean factoryMotorClockwise = getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_FACTORY_CLOCKWISE_MOTOR_DIRECTION_SETTING);
    SerialDebug.printf("factoryMotorClockwise = %d, isClockwiseFlag = %d\n",factoryMotorClockwise, isClockwiseFlag);

    if (factoryMotorClockwise && isClockwiseFlag)
    {
        SerialDebug.println("setClockwiseMotorDirection -- same so no change");
    }
    else if (factoryMotorClockwise && !isClockwiseFlag)
    {
        //!toggle flag and save as that mode..
        isClockwiseFlag = !isClockwiseFlag;
        //! this says: factory is CW and we want to turn CCW
        //! so we have to go the opposite direction (!CW)
    }
    else if (!factoryMotorClockwise && isClockwiseFlag)
    {
        //! toggle flag as well,
        isClockwiseFlag = !isClockwiseFlag;
        //! this says: factory is CCW and we want to turn CW
        //! so we have to go the opposite direction (!CW)
    }
    else if (!factoryMotorClockwise && !isClockwiseFlag)
    {
        SerialDebug.println("setClockwiseMotorDirection -- same so no change");
    }
    //! set to what it thinks it is...
    savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING, isClockwiseFlag);

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
    //!7.20.25 6 months T
    //!Using switch to guarentee unique case of single characters
    switch (cmd)
    {
        case 0x00:
        case 's':
        case 'c':
        {
            sendToStepperModule = true;
        } break;
        case 'a':
        {
            
            SerialDebug.println("cmd=a startTimer");
            //!start timer..
            startStopTimer_mainModule(true);
            
        } break;
        case 'A':
        {
            
            SerialDebug.println("cmd=A stopTimer");
            
            //!start timer..
            startStopTimer_mainModule(false);
            
        } break;
        case 'j':
        {
            sendToStepperModule = true;
            SerialLots.println("Setting FeedState = JACKPOT_FEED");
            savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_JACKPOT_FEED_VALUE, true);
            
        } break;
        case 'u':
        {
            SerialLots.println("Setting feederType = UNO");
            savePreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE, STEPPER_IS_UNO);
            
            //!Issue #332 8.17.2024
            savePreferenceInt_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING, 45);
            //! set autoRotoate as well..
            savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_AUTO_MOTOR_DIRECTION_SETTING, false);
            //! default to  clockwise == 0
            //! 8.18.24 setting this will check for the factory setting..
            setClockwiseMotorDirection_mainModule(true);
        } break;
        case 'm':
        {
            SerialLots.println("Setting feederType = MINI");
            //save preference
            savePreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE, STEPPER_IS_MINI);
            // turn clockwise..
            //! 8.18.24 setting this will check for the factory setting..
            setClockwiseMotorDirection_mainModule(true);
        } break;
        case 'L':
        {
            SerialLots.println("Setting feederType = Tumbler");
            //save preference
            savePreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE, STEPPER_IS_TUMBLER);
            //!Issue #332 8.17.2024
            savePreferenceInt_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING, 200);
            //! set autoRotoate as well..
            //! 8.18.24 setting this will check for the factory setting..
            setClockwiseMotorDirection_mainModule(true);
        } break;
        case 'B':
        {
            sendToStepperModule = true;
            SerialDebug.println("Setting buzzStatus = BUZZON");
            //save pref
            savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE, true);
        } break;
        case 'b':
        {
            sendToStepperModule = true;
            
            SerialDebug.println("Setting buzzStatus = BUZZOFF");
            savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE, false);
        } break;
        case 'T':
        {
            SerialDebug.println("*** Setting tilt = ON");
            savePreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE, true);
        } break;
        case 't':
        {
            SerialDebug.println("Setting tilt = OFF");
            savePreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE, false);
        } break;
        case 'R':
        {
            SerialLots.println("Clean Credentials");
            //! dispatches a call to the command specified. This is run on the next loop()
            main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_CREDENTIALS);
        } break;
            //!6.20.25
#ifdef NOT_SUPPORTED_RIGHT_NOW
        case 'O':
        {
            SerialLots.println("OTA Update.. ");
            //! dispatches a call to the command specified. This is run on the next loop()
            main_dispatchAsyncCommand(ASYNC_CALL_OTA_UPDATE);
        } break;
#endif
        case 'X':
        {
            SerialDebug.println("Clean EPROM.. ");
            //! dispatches a call to the command specified. This is run on the next loop()
            main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_EPROM);
        } break;
        case 'x':
        {
            SerialDebug.println("Clean SSID from EPROM.. ");
            char cleanWIFI[100];
            strcpy(cleanWIFI,"{'ssid':'','ssidPassword':''}");
            SerialDebug.println(cleanWIFI);
#ifdef USE_MQTT_NETWORKING
            //! send to ourself
            sendMessageNoChangeMQTT((char*)cleanWIFI);
#endif
                        
        } break;
            
            //!NOTE: the gateway is auto selected for now. A future version might manually set it in other situations (eg. my iPhone app should have a flag to not be a gateway at time)
        case 'G':
        {
            SerialDebug.println("Setting Gateway = ON");
            main_dispatchAsyncCommand(ASYNC_SET_GATEWAY_ON);
        } break;
        case 'g':
        {
            SerialDebug.println("Setting Gateway = OFF");
            main_dispatchAsyncCommand(ASYNC_SET_GATEWAY_OFF);
        } break;
        case '_':
        {
            //This is from the handshake like "_BLEClient_ESP_M5"
            SerialLots.println("unused cmd '_'");
        } break;
#ifdef OLD_FOR_M5_DISPLAY
        case 'Z':
        {
            sendToStepperModule = false;
            
            SerialDebug.println("Setting SM Zoom = zoomed");
            savePreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE, true);
        } break;
        case 'z':
        {
            sendToStepperModule = false;
            
            SerialDebug.println("Setting SM Zoom = full SM");
            savePreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE, false);
        } break;
#else
            //! 7.25.25 chilly morning, Mt starting to come out.
            //! SPIFF
        case 'Z':
        {
            //! clean SPIFF file"
            
            SerialDebug.println("clean SPIFF file");
            deleteFiles_SPIFFModule();
        } break;
        case 'z':
        {
            //! upload SPIFF to web
            
            SerialDebug.println("upload SPIFF to web");
            //! number of lines to send
            //! TODO: configure thie number of lines to save..
            sendStrings_SPIFFModule(1500);
        } break;
#endif
        case 'P':
        {
            //printout the spiff.. to the serial debug monitor
            //!Restarts (or attempts) a restart of the WIFI using the existing credentials -- vs the 'n' command
            printFile_SPIFFModule();
        } break;
            //! 7.24.25 use SPIFF
        case 'S':
        {
            //! toggle spiff on/off
            boolean spiffFlag = getPreferenceBoolean_mainModule(PREFERENCE_USE_SPIFF_SETTING);
            spiffFlag = !spiffFlag;
            
            SerialDebug.printf("Changing current USE_SPIFF = %s\n", spiffFlag?"NOYES":"NO");
            savePreferenceInt_mainModule(PREFERENCE_USE_SPIFF_SETTING, spiffFlag);
            
            // reboot
            rebootDevice_mainModule();
            
        }
            //! WIFI credential changes...
        case 'N':
        {
            //NOTE: this might be where we toggle credentials?? TODO
            //found other one..
            char *credentials = main_JSONStringForWIFICredentials();
            
            //!These are the ASYNC_CALL_PARAMETERS_MAX
            main_dispatchAsyncCommandWithString(ASYNC_CALL_BLE_CLIENT_PARAMETER, credentials);
        } break;
        case 'n':
        {
            //!NOTE: this is almost the same as 'w' except there might be more WIFI than 2 (so swap is different).
            main_dispatchAsyncCommand(ASYNC_NEXT_WIFI);
        } break;
        case 'W':
        {
            main_dispatchAsyncCommand(ASYNC_RESTART_WIFI_MQTT);
        } break;
        case 'w':
        {
            SerialDebug.println("w for swapWifi");
            
            main_dispatchAsyncCommand(ASYNC_SWAP_WIFI);
        } break;
      
            //! 8.16.24 per #332
        case 'H':
        {
            SerialDebug.printf("PREFERENCE_STEPPER_AUTO_MOTOR_DIRECTION_SETTING true");
            
            // autoMotorDirection ON
            savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_AUTO_MOTOR_DIRECTION_SETTING, true);
            
        } break;
            //! 8.16.24 per #332
            
        case 'h':
        {
            SerialDebug.printf("PREFERENCE_STEPPER_AUTO_MOTOR_DIRECTION_SETTING false");
            
            // autoMotorDirection off
            savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_AUTO_MOTOR_DIRECTION_SETTING, false);
        } break;
        case 'D':
        {
            SerialDebug.printf("PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING counter clockwise");
            
            //!NOTE: no current mode to specify CCW or CW dynamically (non factory reset)
            // motor direction = counterclockwise
            savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_FACTORY_CLOCKWISE_MOTOR_DIRECTION_SETTING,false);
        } break;
        case 'd':
        {
            SerialDebug.printf("PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING clockwise");
            
            // motor direction ==  (clockwise)
            savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_FACTORY_CLOCKWISE_MOTOR_DIRECTION_SETTING,true);
        } break;
            //! 9.30.23 reverse direction
        case 'Q':
        {
            SerialDebug.printf("PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING reverse direction");
            
            //! note: reboot not needed as the next time a feed happens, it reads this value
            // motor direction ==  (reverse)
            boolean  currentDirection = getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING);
            currentDirection = !currentDirection;
            savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING,currentDirection);
            
        } break;
        case 'E':
        {
            SerialDebug.printf("PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING false");
            
            //!if set, the BLE Server (like PTFeeder) will tack on the device name (or none if not defined).
            savePreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING,false);
            // reboot
            rebootDevice_mainModule();
        } break;
        case 'e':
        {
            SerialDebug.printf("PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING true");
            
            //!if set, the BLE Server (like PTFeeder) will tack on the device name (or none if not defined).
            savePreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING,true);
            // reboot
            rebootDevice_mainModule();
        } break;
        case 'r':
        {
            // reboot
            SerialDebug.println("REBOOT ...");
            rebootDevice_mainModule();
        } break;
            
        case 'p':
        {
            //! poweroff
            SerialDebug.printf("(%c) POWEROFF ...", cmd);
            poweroff_mainModule();
        } break;
            //! 7.12.25
            //!  0 == Clear SENSOR definitions
        case '0':
        {
            //! poweroff
            SerialDebug.println("Clearing Sensors");
            setSensorsString_mainModule((char*)"");
            
            //! reboot .. so the sensors are set..
            rebootDevice_mainModule();
        } break;
            //! 7.9.25
            //!  1 == Init SENSOR definitions
        case '1':
        {
            //! poweroff
            SerialDebug.println("Default Sensors");
            //resetSensorToDefault_mainModule();
            //! 7.30.25 changing to the HDriver's board
            setSensorsString_mainModule((char*)"BuzzerSensorClass,19,22,L9110S_DCStepperClass,21,25");
            //! 7.31.25 if Scanner or QR then pin 22 used .. so make M5HDriver (basically don't have a sensor)
            savePreference_mainModule(PREFERENCE_ATOM_KIND_SETTING, "M5HDriver");
             //! also specify the sensor plug
            savePreference_mainModule(PREFERENCE_SENSOR_PLUGS_SETTING, "L9110S_DCStepperClass");
            //! 1 second motor (overloads "angle" field)
            savePreferenceFloat_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING, 1.0);
            //! reboot .. so the sensors are set..
            rebootDevice_mainModule();
        } break;
            //! 7.12.25
            //!  2 == default for SMART Button
        case '2':
        {
            //! poweroff
            SerialDebug.println("Default for SMART Button");
            setSensorsString_mainModule((char*)"");
            
            //! reboot .. so the sensors are set..
            rebootDevice_mainModule();
        } break;
            //! 7.19.25 add Clear Sensors
        case '3':
        {
            //! poweroff
            SerialDebug.println("No Sensors");
            setSensorsString_mainModule((char*)"");
            
            //! reboot .. so the sensors are set..
            rebootDevice_mainModule();
        } break;
            //! 7.19.25 BLE Client ON
        case '4':
        {
            boolean val = getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE);
            val = !val;
            //! poweroff
            SerialDebug.printf("BLE Client %s\n", val?"ON":"OFF");
            savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE, val);
            
            //! reboot .. so the sensors are set..
            rebootDevice_mainModule();
        } break;
            //! 7.9.25 grabbed from BOOTSTRAP let someone update the atom to the recent OTA
        case '5':
        {
            SerialDebug.println(" *** performing m5atom OTA Update");
#ifdef USE_MQTT_NETWORKING
            //!retrieves from constant location
            performOTAUpdate((char*)"http://KnowledgeShark.org", (char*)"OTA/TEST/M5Atom/ESP_IOT.ino.m5stick_c_plus.bin");
#endif
        } break;
            //! 7.9.25 grabbed from BOOTSTRAP let someone update the atom to the recent OTA
        case '6':
        {
            SerialDebug.println(" *** performing m5atom OTA Update - DAILY");
#ifdef USE_MQTT_NETWORKING
            //!retrieves from constant location
            performOTAUpdate((char*)"http://KnowledgeShark.org", (char*)"OTA/TEST/M5Atom/daily/ESP_IOT.ino.m5stick_c_plus.bin");
#endif
        } break;
        case '7':
        {
            SerialDebug.println(" *** performing m5atom OTA Update - BOOTSTRAP");
#ifdef USE_MQTT_NETWORKING
            //!retrieves from constant location
            performOTAUpdate((char*)"http://KnowledgeShark.org", (char*)"OTA/Bootstrap/ESP_M5_BOOTSTRAP.ino.m5stack_stickc_plus.bin");
#endif
        } break;
            
        case 'C':
        {
            SerialDebug.println("'C' change color touched");
#ifdef USE_FAST_LED
            CRGB randomColor = getRandomColor();
            fillpix(randomColor);
            delay(50);
#endif
        }
            break;
        case '.':
        {
            
            //! returns if the BLEClient is turned on.. note, if connected to a BLE device, then disconnect
            
            boolean val = getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE);
            //! toggle spiff on/off
            boolean spiffFlag = getPreferenceBoolean_mainModule(PREFERENCE_USE_SPIFF_SETTING);
            
            SerialMin.println("Valid Commands: ");
            SerialMin.println("         . = help, this message");
            SerialMin.println("         p = poweroff if can");
            SerialMin.println(" 0x0, s, c == Single Feed ");
            SerialMin.println("         a == AutoFeed On");
            SerialMin.println("         A == AutoFeed Off");
            SerialMin.println("         u == UNO ");
            SerialMin.println("         m == MINI ");
            SerialMin.println("         L == tumbler");
            SerialMin.println("         H == autoMotorDirection on");
            SerialMin.println("         h == autoMotorDirection off");
            SerialMin.println("         D == FACTORY counter clockwise motor direction");
            SerialMin.println("         d == FACTORY clockwise motor direction");
            SerialMin.println("         Q == change motor direction opposite of current");
            
            SerialMin.println("         B == Buzzer On");
            SerialMin.println("         b == Buzzer Off");
            SerialMin.println("         G == Gateway On");
            SerialMin.println("         g == Gateway Off");
            SerialMin.println("         R == clean credentials");
            SerialMin.println("         X == clean EPROM");
            SerialMin.println("         x == clean SSID from EPROM");
            SerialMin.println("         r == reboot ");
            //!6.20.25
            
            SerialMin.println("         T == tiltOn");
            SerialMin.println("         t == tiltOff");
            SerialMin.println("         N == send WIFI Credential to BLEServer");
            SerialMin.println("         n == next WIFI Credential");
            SerialMin.println("         W == retry WIFI");
            SerialMin.println("         w == swap WIFI");
            
            SerialMin.println("    SPIFF internal memory ");
#ifndef OLD_FOR_M5_DISPLAY
            SerialMin.println("         Z == clean SPIFF file");
            SerialMin.println("         z == upload SPIFF to web");
#endif
            SerialMin.println("         P == print SPIFF");
            SerialMin.printf ("         S == toggle SPIFF %s\n", spiffFlag?"OFF":"ON");
            
            SerialMin.println("    BLE Naming ");

            SerialMin.println("         E == use only PTFeeder naming");
            SerialMin.println("         e == use naming PTFeeder:name");
#ifdef OLD_FOR_M5_DISPLAY
            SerialMin.println("         Z == Setting SM Zoom = zoomed");
            SerialMin.println("         z == Setting SM Zoom = full SM");
#endif
            SerialMin.println("         0 == no sensors");
            SerialMin.println("         1 == Default SENSOR for new feeder");
            SerialMin.println("         2 == Default SENSOR for SMART Button");
            SerialMin.println("         3 == Clear Sensors");
            SerialMin.printf ("         4 == Turn BLE Client %s (will look for PTFeeder)\n", val?"OFF":"ON");
            
#ifdef NOT_SUPPORTED_RIGHT_NOW
            SerialMin.println("         O == OTA update");
#else
            SerialMin.println("         5 == m5atom DEV OTA update");
            SerialMin.println("         6 == m5atom DAILY TEST DEV OTA update");
            SerialMin.println("         7 == go back to m5atom BOOTSTRAP");
            SerialMin.println("         C == change m5 atom to random color");

#endif
            SerialMin.println();
            SerialMin.println("Full API at: https://github.com/konacurrents/SemanticMarkerAPI");
            SerialMin.println();
            
            //!print out stuff
            main_printModuleConfiguration();
            
        } break;
            break;
        default:
            
        {
            SerialMin.printf("*****invalid command '%c' from client (use '.' for help)  *****\n", cmd);
        }
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
    //! 7.27.25
    //! miliseconds .. convert to seconds
    //now = now * 1000;
    SerialMin.printf("Unix Time: %d\n", now);
    return now;
}

//! ************** SM Mode Processing ***************


//!returns an index from 0..max of SM matching cmd, or -1 if none
int whichSMMode_mainModule(char *cmd)
{
#ifdef ESP_M5
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
#endif //ESP_M5
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

//! 6.6.25 get the current M5AtomClassType
M5AtomClassType* whichM5AtomClassType()
{
    return _whichM5AtomClassType;
}

//! 1.22.24 add setup and loop at main so it can call appropriate plugs
void loop_Sensors_mainModule()
{
#ifdef USE_BUTTON_MODULE
    //! the model part of the MVC for buttons
    loop_ButtonProcessing();
#endif
    
    //! these are the plugin modules .. and only 1 active at a time except for ATOM
#ifdef M5CORE2_MODULE
    loop_M5Core2Module();
#elif defined(M5_ATOM)
    
#pragma mark USE_NEW_M5ATOMCLASS
    //! 5.6.25 use object version
    if (_whichM5AtomClassType)
    {
        SerialCall.printf("*** _whichM5AtomClassType %s\n",_whichM5AtomClassType->classIdentity() );
        _whichM5AtomClassType->loop_M5AtomClassType();
    }
    else
    {
        SerialDebug.println("*** _whichM5AtomClassType NULL ***");

    }

    // end atom
    
#elif defined(USE_CAMERA_MODULE)
    loop_CameraModule();
#elif defined(M5BUTTON_MODULE)
    loop_M5ButtonModule();
#endif
}

//! 1.22.24 setup of buttons
void setup_Sensors_mainModule()
{
    SerialDebug.println("setup_Sensors_mainModule");
#ifdef USE_BUTTON_MODULE
    //! the model part of the MVC for buttons
    setup_ButtonProcessing();
#endif
    //! these are the plugin modules .. and only 1 active at a time except for ATOM
#ifdef M5_ATOM
    SerialDebug.println(" ***** M5_ATOM define ****");
#endif
#ifdef M5CORE2_MODULE
    setup_M5Core2Module();
#elif defined(M5_ATOM)
  
    
    //! 5.6.25 use the M5Atom ClassType
#ifdef USE_NEW_M5ATOMCLASS
    //! @see https://www.cs.fsu.edu/~myers/cop3330/notes/dma.html
    _M5Atom_SocketModuleClass = new M5Atom_SocketModuleClass((char*)"M5AtomSocket");
    _M5Atom_QRCodeModuleClass = new M5Atom_QRCodeModuleClass((char*)"M5AtomScanner");
    _M5Atom_HDriverModuleClass = new M5Atom_HDriverModuleClass((char*)"M5HDriver");
    //! 7.17.25
    _M5Atom_TinyGPSModuleClass = new M5Atom_TinyGPSModuleClass((char*)"M5AtomTinyGPS");

    int whichM5AtomIndex = 0;
    SerialDebug.println("setup_M5Atoms");
    
    //! 3.31.25 create array of plugs
    _M5AtomClassTypes[whichM5AtomIndex++] = _M5Atom_SocketModuleClass;
    _M5AtomClassTypes[whichM5AtomIndex++] = _M5Atom_QRCodeModuleClass;
    _M5AtomClassTypes[whichM5AtomIndex++] = _M5Atom_HDriverModuleClass;
    //! 7.17.25
    _M5AtomClassTypes[whichM5AtomIndex++] = _M5Atom_TinyGPSModuleClass;

    //! add check..
    if (whichM5AtomIndex > NUM_M5ATOM_CLASS)
    {
        SerialDebug.printf("**** sensors are more than max .. FIX CODE");
    }
    SerialDebug.printf("Created %d M5AtomClassTypes\n", whichM5AtomIndex);
    
    
    //! use this one...
    _whichM5AtomClassType = NULL;
    //! find the current atomKind. which is a string
    char *atomKind = getPreferenceATOMKind_MainModule();
    
    //! find which one..
    for (int i=0; i<NUM_M5ATOM_CLASS; i++)
    {
        if (!_M5AtomClassTypes[i])
        {
            SerialDebug.println("NULL M5AtomClassType");
            continue;
        }
        //! check against the identity.. (or make this part of that method?)
        if (strcmp(_M5AtomClassTypes[i]->classIdentity(), atomKind) == 0)
        {
            //! Matched..
            _whichM5AtomClassType = _M5AtomClassTypes[i];
            break;
        }
    }
    if (_whichM5AtomClassType)
    {
        SerialDebug.printf("** Found M5AtomClass = %s\n", atomKind);
    }
    else
    {
        SerialDebug.printf("****^^^ Cannot find M5AtomClass = %s\n", atomKind);

        SerialDebug.println("Defaulting to _M5Atom_HDriverModuleClass");
        _whichM5AtomClassType = _M5Atom_HDriverModuleClass;
    }
#else
    SerialDebug.printf("M5AtomKind = %d\n", getM5ATOMKind_MainModule());
#endif
    
    //! now the setup() call
#pragma mark USE_NEW_M5ATOMCLASS
    //! 5.6.25 use object version
    if (_whichM5AtomClassType)
        _whichM5AtomClassType->setup_M5AtomClassType();
    
    // end atom
#elif defined(USE_CAMERA_MODULE)
    //! let ESP_IOT.ino call this .. since it needs the WIFI running..
    //setup_CameraModule();
#elif defined(M5BUTTON_MODULE)
    setup_M5ButtonModule();
#endif
}
//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void buttonA_ShortPress_mainModule()
{
#ifdef USE_BUTTON_MODULE
    buttonA_ShortPress();
#endif
#ifdef M5CORE2_MODULE
    buttonA_ShortPress_M5Core2Module();
#elif defined(M5_ATOM)
#pragma mark USE_NEW_M5ATOMCLASS
    //! 5.6.25 use object version
    if (_whichM5AtomClassType)
        _whichM5AtomClassType->buttonA_ShortPress_M5AtomClassType();

// end atom
#elif defined(USE_CAMERA_MODULE)
    buttonA_ShortPress_CameraModule();
#elif defined(M5BUTTON_MODULE)
    buttonA_ShortPress_M5ButtonModule();
#endif

}

//!long press on buttonA (top button)
void buttonA_LongPress_mainModule()
{
#ifdef USE_BUTTON_MODULE
    buttonA_LongPress();
#endif
#ifdef M5CORE2_MODULE
    buttonA_ShortPress_M5Core2Module();
#elif defined(M5_ATOM)
    
#pragma mark USE_NEW_M5ATOMCLASS
    //! 5.6.25 use object version
    if (_whichM5AtomClassType)
        _whichM5AtomClassType->buttonA_LongPress_M5AtomClassType();
    
#elif defined(USE_CAMERA_MODULE)
    buttonA_LongPress_CameraModule();
#elif defined(M5BUTTON_MODULE)
    buttonA_LongPress_M5ButtonModule();
#endif
}


//!the long press of the side button
void buttonB_LongPress_mainModule()
{
#ifdef USE_BUTTON_MODULE
    buttonB_LongPress();
#endif
#ifdef M5BUTTON_MODULE
    //!the long press of the side button
     buttonB_LongPress_M5ButtonModule();
#endif
}
//!the short press of the side button
void buttonB_ShortPress_mainModule()
{
#ifdef USE_BUTTON_MODULE
    buttonB_ShortPress();
#endif

#ifdef M5BUTTON_MODULE
    //!the long press of the side button
    buttonB_ShortPress_M5ButtonModule();
#endif
}

//!restarts all the menu states to the first one .. useful for getting a clean start. This doesn't care if the menu is being shown
void restartAllMenuStates_mainModule()
{
    //!restarts all the menu states to the first one .. useful for getting a clean start. This doesn't care if the menu is being shown
    restartAllMenuStates_ModelController();
}

//! 1.1.24 send status of this device after events..
void sendStatusMQTT_mainModule()
{
    //On demand #STATUS send the statusURL as well (if an M5)
    //this queues the sending of the StatusURL over MQTT.
    // This is async (next loop) since sending 2 MQTT messages can be hard to do in a row ..
    main_dispatchAsyncCommand(ASYNC_SEND_MQTT_STATUS_URL_MESSAGE);
}


//! 3.23.25 rainy weekend
//! create a JSON string from the SemanticMarker
//! https://semanticmarker.org/bot/setdevice/scott@konacurrents.com/PASS/M5AtomSocket/socket/on}
//! Create a JSON knowing the "bot" syntax, eg.  setdevice/USER/PASS/device/<set>/<flag>
//! defined in TokenParser.h
char *semanticMarkerToJSON_mainModule(char* semanticMarker)
{
    return semanticMarkerToJSON_TokenParser(semanticMarker);
}

#pragma mark PIN USE to see if overlaps.
//! 7.31.25 add this for a status, saw that QRCode was using 22 also  .. so buzer didn't work.
/**
#define PIN_USE_MAX 10
struct pinUseStruct {
    int pineUseCount;
    char *pinUseArray[PIN_USE_MAX];
} _pinUseStruct;
*/

//! global for use
PinUseStruct _pinUseStruct;
//! get the pin use array
PinUseStruct getPinUseStruct_mainModule()
{
    return _pinUseStruct;
}

//! 7.31.25 store this information.. for STATUS
//! 5.3.25 add a central clearing house for defining PIN use
//! @see issue #365
//! central clearing house for all pins used to we can analyze if there are overlaps
//! pin is the actual number, pinName is the local name (eg. IN1_PIN or VIN_PIN).
//! moduleName is the module in the code,
//! isI2C is whether this is a I2C bus (which we aren't using much yet)
void registerPinUse_mainModule(long pin, String pinName, String moduleName, boolean isI2C)
{
    char pinUseSample[100];
    sprintf(pinUseSample,"PIN_USE: %2d = %s, %s %s", pin, pinName.c_str(), moduleName.c_str(), isI2C?"(I2C)":"");
    //! 5.3.25 create storage here
    char *pinUse = (char*)calloc(strlen(pinUseSample)+1, sizeof(char));
    strcpy(pinUse, pinUseSample);
    //!store globally
    _pinUseStruct.pinUseArray[_pinUseStruct.pineUseCount] = pinUse;
    //! increment
    _pinUseStruct.pineUseCount++;
    
    SerialDebug.printf("** PIN_USE: %s = %d, module=%s %s\n", pinName.c_str(), pin, moduleName.c_str(), isI2C?"(I2C)":"");
}
