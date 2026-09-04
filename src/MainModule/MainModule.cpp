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
//! 8.16.25 bring in the CameraModule from 2022
#include "../M5AtomClassModule/M5Atom_CameraModuleClass.h"
//! 10.26.25 Power Out .. Wind Storm. Generator Running
//! add a NoModule class
#include "../M5AtomClassModule/M5Atom_NoModuleClass.h"

//! 7.24.25 Hot Day, Ballon last night, Mt Out
//! for the 'C' option of atom color
#ifdef USE_FAST_LED
#include "../ATOM_LED_Module/M5Display.h"
#endif

#pragma mark BUTTON Processing of M5
//! 10.16.25 add buttons
boolean _shortPress_MainModule = false;
boolean _longPress_MainModule = false;
boolean _longLongPress_MainModule = false;
boolean _doublePress_MainModule = false;

//! 8.11.26 have B (the top of the display button on M5AtomS3
//! 10.16.25 add buttons
boolean _shortPressB_MainModule = false;
boolean _longPressB_MainModule = false;
boolean _longLongPressB_MainModule = false;
boolean _doublePressB_MainModule = false;

//!big button on front of M5StickC Plus
void checkButtonA_MainModule();
//! buttonB will set the _shortPressB .. fields
void checkButtonB_MainModule();

//! instances of the M5AtomClassType
#pragma mark M5AtomClassType

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
//! 8.16.25 bring in the CameraModule from 2022
//! 6
M5Atom_CameraModuleClass* _M5Atom_CameraModuleClass;
//! 10.26.25 Power Out .. Wind Storm. Generator Running
//! add a NoModule class
M5Atom_NoModuleClass* _M5Atom_NoModuleClass;

//! make sure this is updated.
//! 8.16.25 == 6
//! 10.26.25 == 7
#define NUM_M5ATOM_CLASS 7

//! 3.31.25 create array of plugs
M5AtomClassType* _M5AtomClassTypes[NUM_M5ATOM_CLASS];

//! use this one...
M5AtomClassType *_whichM5AtomClassType;

#endif //USE_NEW_M5ATOMCLASS

#pragma mark SensorClassType

#ifdef ESP_M5_ATOM_S3
//! 7.12.26 Ashton's 12th birthday  in Hawaii
//! M5AtomS3 working .. now add more ...
#define USE_SENSOR_CLASS
// blowing up at ChainButtonClass::setup(19.22) ...  PINS are now:
// blowing up at ChainButtonClass::setup(6,5) BLOWING STILL ...  PINS are now:

#else
#define USE_SENSOR_CLASS
#endif
#ifdef  USE_SENSOR_CLASS
//! 5.14.25 
#include "../SensorClass/SensorClassType.h"
#include "../SensorClass/BuzzerSensorClass.h"
#include "../SensorClass/KeyUnitSensorClass.h"
//! 11.27.25 add PIRSensor
#include "../SensorClass/PIRSensorClass.h"
//! 12.19.25 48 hrs no power, high wind storm, flooding in lowlands - bad
//! this will be the ChainButtonClass
#include "../SensorClass/ChainButtonClassType.h"

#define NUM_SENSOR_CLASS_TYPES 4
//!
BuzzerSensorClass* _BuzzerSensorClass;
//2
KeyUnitSensorClass* _KeyUnitSensorClass; //not used.. here
//3
PIRSensorClass* _PIRSensorClass;
//4
ChainButtonClass* _ChainButtonClass;

//! 11.28.25 Clear day, M5 Out
//! create array of instances POSSIBLE (the Sensors, pin1,pin2 -- will instantiate them)
SensorClassType* _SensorClassTypes[NUM_SENSOR_CLASS_TYPES];

//! 11.27.25 Turkey Day, loop
void loopSensorClassTypeArray()
{
    //return; //! 11.28.25 inifinite loop when using a  sensor is defined, but no instance created, eg StepperMotor ..

    //! return the sensor specified or null
    SensorsStruct* sensorsStruct = getSensors_mainModule();
    //SerialDebug.printf(" *** loopSensorClassTypeArray (%ld) ***\n",  sensorsStruct?sensorsStruct->count:-1);
#define SERIAL_LOTS_
#ifdef SERIAL_LOTS
    //! try a debug print..
    printSensors_mainModule(sensorsStruct);
#endif

    if (sensorsStruct)
    {
        //printSensors_mainModule(sensorsStruct);
        
        //! all the sensors defined
        //! sensors is an array of sensor
        SensorStruct *sensors = sensorsStruct->sensors;
        int count = sensorsStruct->count;
        
        //! go through the defined sensorName and call loop on those classes
        for (int i=0; i< count; i++)
        {
            //! this only works if there is an instance there of value .. the NULL doesn't work
            //! as this is a memory location somethwere.
            //! so the instantiator will now create a DUMMY SensorClass object .. so this works.
            //! The issue is there can be a sensor that doesn't exist so that slot is empty .. but the &sensors[i] BLOWS up
            SensorStruct* sensor = &sensors[i];
            //SerialTemp.printf("sensor = %d\n", sensor);

            //! 11.28.25 KLUDGE .. for now only this sensor has a loop
         //   if (strcmp(sensor->sensorName, (char*)"PIRSensorClass")==0)
                //! check sensor exists ..
                if (sensor)
                {
                    //check this???  PIR HAT only one so far that needs this ...
                    sensor->sensorClassType->loop();
                }
        }

    }
}

//! 1.24.26 send the setval of the MQTT message ..TODO
//! 11.27.25 Turkey Day, loop
void messageSetVal_SensorClassTypeArray(char *setName, char* valValue, boolean deviceNameSpecified)
{
    //return; //! 11.28.25 inifinite loop when using a  sensor is defined, but no instance created, eg StepperMotor ..
    
    //! return the sensor specified or null
    SensorsStruct* sensorsStruct = getSensors_mainModule();
    if (sensorsStruct)
    {
        //! all the sensors defined
        //! sensors is an array of sensor
        SensorStruct *sensors = sensorsStruct->sensors;
        int count = sensorsStruct->count;
        
        //! go through the defined sensorName and call loop on those classes
        for (int i=0; i< count; i++)
        {
            //! this only works if there is an instance there of value .. the NULL doesn't work
            //! as this is a memory location somethwere.
            //! so the instantiator will now create a DUMMY SensorClass object .. so this works.
            //! The issue is there can be a sensor that doesn't exist so that slot is empty .. but the &sensors[i] BLOWS up
            SensorStruct* sensor = &sensors[i];
            
            //! check sensor exists ..
            if (sensor)
            {
                SensorClassType *sensorClassType = sensor->sensorClassType;
                if (sensorClassType)
                    //! forward the message
                    sensorClassType->messageSetVal_SensorClassType(setName, valValue, deviceNameSpecified);
            }
        }
    }
}
    
//! init will look at those SensorClass known above, and look
//! to see which ones are defined by the user in the SensorsStruct
//! A report will show which ones were not found
void initSensorClassTypeArray()
{
    SerialDebug.println(" *** initSensorClassTypeArray ***");
    
    //! create possible Sensor class instan ces
    //! 11.28.25
    //! use the class name as identity()
    _BuzzerSensorClass = new BuzzerSensorClass((char*)"BuzzerSensorClass");
    _KeyUnitSensorClass = new KeyUnitSensorClass((char*)"KeyUnitSensorClass");
    _PIRSensorClass = new PIRSensorClass((char*)"PIRSensorClass");
    //! 12.19.25 add Class type for Chain
    _ChainButtonClass = new ChainButtonClass((char*)"ChainButtonClass");
    
    //! 11.28.25 Clear day, M5 Out
    //! create array of instances POSSIBLE (the Sensors, pin1,pin2 -- will instantiate them)
    SensorClassType* _SensorClassTypes[NUM_SENSOR_CLASS_TYPES];
    
    int whichSensorClassIndex = 0;
    SerialDebug.println("setup_SensorClassIypes");
    
    //! 11.28.25 create array of Sensors
    _SensorClassTypes[whichSensorClassIndex++] = _BuzzerSensorClass;
    _SensorClassTypes[whichSensorClassIndex++] = _KeyUnitSensorClass;
    _SensorClassTypes[whichSensorClassIndex++] = _PIRSensorClass;
    //! 12.19.25 add ChainButtonClass
    _SensorClassTypes[whichSensorClassIndex++] = _ChainButtonClass;
    
    //! add check..
    if (whichSensorClassIndex > NUM_SENSOR_CLASS_TYPES)
    {
        SerialDebug.printf("**** sensors are more than max .. FIX CODE");
    }
    SerialDebug.printf("Created %d SensorClassTypes\n", whichSensorClassIndex);
    
    
    //! return the sensor specified or null
    SensorsStruct* sensorsStruct = getSensors_mainModule();
    if (sensorsStruct)
    {
        //! all the sensors defined
        //! sensors is an array of sensor
        SensorStruct *sensors = sensorsStruct->sensors;
        int count = sensorsStruct->count;

        //! 11.28.25 use the SensorClassType (like the M5AtomClassType pattern)
        //! go through the defined sensorName and instantiate those classes
        for (int i=0; i< count; i++)
        {
            SensorStruct* sensor = &sensors[i];
            SerialDebug.printf("Looking for sensor: %s, %ld, %ld\n", sensor->sensorName, sensor->pin1, sensor->pin2);
            //! find matching sensor instantiated
            SensorClassType *whichSensorClassType = NULL;
            boolean foundMatchingClassType = false;
            for (int whichSensor=0; whichSensor<NUM_SENSOR_CLASS_TYPES; whichSensor++)
            {
                //! look for ths sensor mentioned..
                whichSensorClassType = _SensorClassTypes[whichSensor];
                char *sensorIdentity = whichSensorClassType->classIdentity();
                if (strcmp(sensor->sensorName, sensorIdentity)==0)
                {
                    //! set the running sensor type for this instnce (currently only 1 per sensor type)
                    sensor->sensorClassType = whichSensorClassType;
                    
                    //! call this setup and pin sets
                    //! setup and then setPins..
                    whichSensorClassType->setPinValues(sensor->pin1, sensor->pin2);
                    //! 1.22.26 the Chain will read the button definitions at setup time and loop time
                 
                    SerialTemp.printf("sensor.setup(%s) = %d\n", whichSensorClassType->classIdentity(), whichSensorClassType);

                    //! call setup
                    whichSensorClassType->setup();
                    //! exit loop
                    foundMatchingClassType = true;
                    break;
                }
            }

            if (!foundMatchingClassType)
            {
                SerialDebug.printf("Missing SensorClass definition: %s\n", sensor->sensorName);
                sensor->sensorClassType = NULL;
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
                sensor->sensorClassType = _BuzzerSensorClass;
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

//! 10.10.25 #405 #406
//! see if the device is a PTClicker if the M5Atom class is one..
//! return the service name:  PTClicker or PTFeeder
char *getServerServiceName_mainModule()
{
    if (isPTFeeder_mainModule())
        return MAIN_BLE_SERVER_SERVICE_NAME_PTFeeder;
    else
        return MAIN_BLE_SERVER_SERVICE_NAME_PTClicker;
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
    
    //! change LED
    setLEDColor(BLUE_LED);
    
#ifdef DONE_IN_INO
    //! 7.9.26 add this. 20 mile bike ride .. felt good
#ifdef ESP_M5_ATOM_S3
    espBegin_mainModule(0); //7.26.26 //M5.Begin();
#else
    // Init Atom-Matrix(Initialize serial port, LED).
    //M5.begin(true, false, true);
    espBegin_mainModule(0); //7.26.26 //M5.Begin();
#endif
#endif
    
#ifdef BOOTSTRAP_CLEAN
    //!cleans the EPROM .. if things are really not working..
    cleanEPROM_mainModule();
#endif
#define TEMP_FIX_LOOP_NO
#ifdef TEMP_FIX_LOOP
    setSensorsString_mainModule((char*)"BuzzerSensorClass,21,25");
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
#ifdef USE_SENSOR_CLASS
    //! 5.14.25 use the Sensor types
    //! first we have N sensors
    //! then we get which ones the user wants (the SENSORS_SETTING)
    //! then we instantiate, and provide ping, etc.
    initSensorClassTypeArray();
#endif
    
#define TRY_DUPLICATES_works //uncomment to try again..
#ifdef  TRY_DUPLICATES
    {
        //! 8.32.25 added a test .. the '.' needs to run.. to see the error
        boolean pinValid = registerPinUse_mainModule(22, "TestPin22", "MainModule", false);
        pinValid = pinValid && registerPinUse_mainModule(27, "TestPin27", "MainModule", false);
        if (!pinValid)
        {
            SerialError.println("*** invalidPins testing..");
        }
    }
#endif
    
    SerialDebug.println("END setup_mainModule");
}

//! main BEGIN and UPDATE .. everyone calls this .. and we can see if dont too many times
//! 7.26.26
//! the Beging aggregate
void espBegin_mainModule(int num)
{
    SerialDebug.print(" **** ESP BEGIN: ");
    SerialDebug.println(num);
    
#ifdef ESP_ATOM_S3
    Atom.begin();
#else
    M5.begin();
#endif
    SerialDebug.println(" *** DONE M5.begin");
}
//! the Update aggrgate
void espUpdate_mainModule(int num)
{
    SerialLots.printf("***ESP UPDATE(%d)\n",num);
#ifdef ESP_ATOM_S3
    Atom.update();
#else
    M5.update();
#endif
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
    
    //! 7.9.26 add this. 20 mile bike ride .. felt good
#ifdef ESP_M5_ATOM_S3
    espUpdate_mainModule(0); //AtomS3.update();
#else
    espUpdate_mainModule(0); //M5.update();
#endif

    
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
        
        SerialDebug.printf("** SerialCommand: '%s'\n", command);
        //! save globally for the callback below..
        strcpy(_serialBuffer, command.c_str());
        
        if (command == "help")
        {
            SerialTemp.println(VERSION);

            SerialDebug.println("Enter any Single Char or JSON msg, type '.' for commands");
            SerialDebug.println("r -- reboot");
            SerialDebug.println(". -- shows single char commands");
            SerialDebug.println("Example JSON to change WIFI (copy and modify with your values)");
            SerialDebug.println("   supports single quotes for values");
            SerialDebug.println("    Use to set your WIFI");
            SerialDebug.println("{'ssid':'SunnyWhiteriver', 'ssidPassword':'sunny2021'}");
            SerialDebug.println("{'ssid':'wes', 'ssidPassword':'wes'}");
            SerialDebug.println();

                        
            //! 7.23.26 this callback only gets called if the BLE_Server is on.. so let's call it anyway without the callback.
            //processClientCommandChar_mainModule('.');
        }
        else if (command == "r")
        {
            rebootDevice_mainModule();
        }
        //! 8.10.26 Nice boat trip to Fox Island and Tanglewood
        //! work on the TEST ideas
        else if (command == "test1")
        {
            // do an ota of test1 ... ???
            SerialDebug.println(" **** TODO: OTA for test1");
        }
        //! 8.11.26 reate test user
        else if (command == "test")
        {
            SerialDebug.println("create TEST user");
            SerialDebug.println("{'ssid':'SunnyWhiteriver', 'ssidPassword':'sunny2021'}");
            SerialDebug.println("{'ssid':'wes', 'ssidPassword':'wes'}");
            /*
            {
                "ssid" : "SunnyWhiteriver",
                "ssidPassword" : "sunny2021",
                "mqtt_topic": "usersP/bark/test",
                "mqtt_user" : "test",
                "deviceName" : "M5AtomTest",
                "mqtt_password" : "test",
                "mqtt_guestPassword" : "test",
                "uuid" : "test",
                "mqtt_port" : "1883",
                "mqtt_server" : "idogwatch.com",
                "mqtt_status" : "Success",
                "location": "seattle, wa"
            }
             */
            char credentials[400];
            sprintf(credentials,
                    "{'ssid':'SSID','ssidPassword':'PASS','mqtt_topic':'usersP/bar/test','mqtt_user':'test','deviceName':'M5Atomtest','mqtt_password':'test','mqtt_guestPassword':'test','uuid':'test','mqtt_port':'1883','mqtt_server':'idogwatch.com','location':'Seattle,Wa'}");
            
            
            //!These are the ASYNC_CALL_PARAMETERS_MAX
            //!NO: just change our credentials ...
            //send to ourself.., recursively...
            int val = processJSONMessageMQTT(credentials, TOPIC_TO_SEND);
        }
        else
        {
            //! 8.7.26 w laura babe,
            //! Tyler,  if BLEServer is on .. send the serial buffer ..
            //! if not serial server .. then accept single commands .. and go right there .. basically to turn on the server
            if (getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE))
            {
                //!call the callback specified from the caller (eg. NimBLE_PetTutor_Server .. or others)
                callCallbackMain(CALLBACKS_BLE_SERVER, BLE_SERVER_CALLBACK_ONWRITE, _serialBuffer);
            }
            else
            {
                SerialDebug.println(" *** only single character supported since BLE_Server off, type '#' to turn on");
                //! 7.23.26 this callback only gets called if the BLE_Server is on.. so let's call it anyway without the callback.
                processClientCommandChar_mainModule(_serialBuffer[0]);
            }

        }
    }
    
#pragma mark button processing
#ifdef USE_BUTTON_MODULE
    //! duplicate here for now...
    checkButtonA_MainModule();

#else
    //! 10.16.25 add the M5.update here...
    //! 7.14.26 THIS no-work, cannot do update 2 times.. the butotns won't work.
  //  M5.update();
    
    //! 10.16.25 Mt out .. looks like storm comming
    //! implement the buttonA here..
    //!big button on front of M5StickC Plus
    checkButtonA_MainModule();
    //! 10.16.25 seems the big button on M5ATOM is Button B ..
    //! the LED push button...  (WE HAVE TO SUPPORT A,B,C buttons .. )
    checkButtonB_MainModule();
    //! now process single button .. merged
    if (_shortPress_MainModule)
    {
        buttonA_ShortPress_mainModule();
    }
    else if (_longPress_MainModule)
    {
        buttonA_LongPress_mainModule();
    }
    else if (_longLongPress_MainModule)
    {
        //! 11.25.25 Cold 30's
        //! call the long long
        buttonA_LongLongPress_mainModule();
    }
#endif
    
#ifdef USE_SENSOR_CLASS
    //! 11.27.25 Turkey Day, loop
    loopSensorClassTypeArray();
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
        //! 8.16.25 MQTT
        //note needs # or won't send..
        //!NOTE: don't send "feed" as it might trigger a FEED ...
        sendMessageMQTT((char*)"#count reached .. resetting");

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

//! 8.16.25 MQTT
#include "../MQTTModule/MQTTNetworking.h"

//! 8.16.25 BLE SERVER
#include "../BLEServerModule/BLEServerNetworking.h"

//! 8.16.25 BLE CLIENT
#include "../BLEClientModule/BLEClientNetworking.h"
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
        //! 8.16.25 MQTT
        _callbacksFunctionsMAXS[CALLBACKS_MQTT] = CALLBACKS_MAX_MQTT;
        //! 8.16.25 BLE SERVER
        _callbacksFunctionsMAXS[CALLBACKS_BLE_SERVER] = CALLBACKS_MAX_BLE_SERVER;
        //! 8.16.25 BLE CLIENT
        _callbacksFunctionsMAXS[CALLBACKS_BLE_CLIENT] = CALLBACKS_MAX_BLE_CLIENT;
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
    SerialLots.printf("registerCallbackMain %d, %d\n", callbacksModuleId, callbackType);
    
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
    SerialTemp.printf("messageSetVal(%s,%s)\n", setName, valValue);
    // THE IDEA WOULD be a callback is avaialble..
    //FOR now.. just ifdef
#ifdef M5_ATOM
    //! 5.6.25 use object version
    if (_whichM5AtomClassType)
        _whichM5AtomClassType->messageSetVal_M5AtomClassType(setName, valValue, deviceNameSpecified);

#ifdef USE_SENSOR_CLASS
    //! 1.24.26 call the same on the SensorClassType
    messageSetVal_SensorClassTypeArray(setName, valValue, deviceNameSpecified);
#endif
    
#endif //M5_ATOM
    
#ifdef M5CORE2_MODULE
    //! TODO fix this
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
    //! TODO FIX THIS
    messageSend_M5Core2Module(sendValue);
#endif
}

//! 8.16.25 MQTT
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
        //! 8.16.25 BLE CLIENT

        SerialTemp.print("Paired Device: ");
        SerialTemp.println(getPreference_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING));
        
        SerialTemp.printf("Gateway: Feeding over the BLE CLIENT network .. if connected: %d\n",isConnectedBLEClient());
        if (isConnectedBLEClient())
        {
            //!send the feed over the BLE client (if connected)
            sendFeedCommandBLEClient();
        }
        SerialLots.println("after send feed via ble");
    }
    
#ifdef TOO_MUCH_7_10_26
    //! 11.26.25 try the #STATUS on every feed
    SerialDebug.println("FEED now send STATUS");
    sendMessageMQTT((char*)"#STATUS");
#endif
}

//!moved here 4.25.22 (entirely from ESP_IOT.ino)

//! April 8, 2022
//! set that the credentials are set. Called from the MQTT after processJSONMessage() found things were good..
void main_credentialsUpdated()
{
    // set the done in the WI
    SerialDebug.println(" *** main_credentialsUpdated .. setDoneWIFI_APModuleFlag **");
    //! 8.16.25 WIFI AP
    setDoneWIFI_APModuleFlag(true);
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
    
    //! 8.16.25 MQTT
    //This should be a 'register' command..
    
    //!the MQTTNetwork.processJSONMessage()
    //!will return true if the message was JSON and was processes, false otherwise.
    if (processJSONMessageMQTT(message, NULL))
    {
        //!processed by the MQTTNetworking code..
        //! 8.16.25 BLE SERVER
        sendBLEMessageACKMessage();
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
        
        //! 8.16.25 BLE SERVER
        sendBLEMessageACKMessage();
        
    }
    SerialDebug.println("*********");
    
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
    
#ifdef M5UNIFIED
    //! 6.5.26
#else
    M5.Axp.PowerOff();
#endif
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
    
    //! 8.16.25 MQTT
    cleanEPROM_MQTTNetworking();
    
    //! 8.16.25 WIFI AP
    cleanEPROM_WIFI_APModule();
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
    sprintf(_JSONStringForWIFICredentials, "{'ssid':'%s','ssidPassword':'%s'}", ssid?ssid:"NULL", ssid_password?ssid_password:"");
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
    SerialMin.printf("main_updateMQTTInfo(%s,%s,%s,%s,%s, %s, d=%s)\n", ssid?ssid:"NULL", ssid_password?ssid_password:"", username?username:"NULL", password?password:"NULL", guestPassword?guestPassword:"NULL", locationString?locationString:"NULL", deviceName?deviceName:"NULL");
    
    _MQTT_Password = password;
    _MQTT_Username = username;
    
    //!store the device name
   savePreference_mainModule(PREFERENCE_DEVICE_NAME_SETTING, deviceName);

    //!store the JSON version of these credentials..
    main_saveWIFICredentials(ssid,ssid_password);
    
    //! 8.16.25 WIFI AP
    WIFI_APModule_updateMQTTInfo(ssid, ssid_password, username, password, guestPassword, deviceName, host, port, locationString);
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
                {
                    SerialDebug.println("NO BuzzerSensorClass defined ..turn off");
                    savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE, false);
                }
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
    SerialTemp.printf("initAsyncCallFlags(%d,%d)\n", ASYNC_CALL_MAX, ASYNC_CALL_PARAMETERS_MAX);
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
            SerialTemp.printf("invokeAsyncCommands(%d)\n", i);
            _asyncCallFlagsParameters[i] = false;
            switch (i)
            {
                case ASYNC_CALL_BLE_CLIENT_PARAMETER:
                    SerialTemp.print("ASYNC_CALL_BLE_CLIENT_PARAMETER: ");
                    SerialTemp.println(_asyncParameter);
                    //! 8.16.25 BLE CLIENT
                    if (strlen(_asyncParameter)<=13)
                        sendCommandBLEClient_13orLess(_asyncParameter);
                    else
                        sendCommandBLEClient(_asyncParameter);
                    
                    break;
                case ASYNC_CALL_OTA_FILE_UPDATE_PARAMETER:
                    SerialTemp.print("ASYNC_CALL_OTA_FILE_UPDATE_PARAMETER: ");
                    SerialTemp.println(_asyncParameter);
                    //! 8.16.25 MQTT
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
                    //! 8.16.25 MQTT
                    performOTAUpdateSimple();
                    break;
                case ASYNC_CALL_CLEAN_CREDENTIALS:
                    SerialTemp.println("ASYNC_CALL_CLEAN_CREDENTIALS");
                    //! 8.16.25 WIFI AP
                    clean_SSID_WIFICredentials();
                    //this reboots ..
                    break;
                case ASYNC_CALL_CLEAN_EPROM:
                    SerialTemp.println("ASYNC_CALL_CLEAN_EPROM");
                    cleanEpromPreferences();
                    break;
                case ASYNC_CALL_FEED_COMMAND:
                    SerialLots.println("ASYNC_CALL_FEED_COMMAND");
                    incrementFeedCount_mainModule();
                    //! 8.16.25 MQTT
                    //!register the 2 callbacks for now..
                    callCallbackMain(CALLBACKS_MQTT, MQTT_CALLBACK_FEED, (char *)"");
                    break;
                    //!this is a sending of the message
                case ASYNC_SEND_MQTT_FEED_MESSAGE:
                    SerialTemp.println("ASYNC_SEND_MQTT_FEED_MESSAGE");
                    //incrementFeedCount_mainModule();
                    SerialCall.println("async_send_feed.1");

                    //! 8.16.25 MQTT
                    //! 8.16.25 BLE CLIENT
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
                    //SerialTemp.println("async_send_feed.3");
                    //TODO: 7.26.22 .. decide if this logic makes sense:
                    //NO BLECLient, and a message arrives for this device to #FEED. So previously it would sned that onto the network .. but I don't think that's right. (And we weren't in that mode yet).
                    //sendMessageMQTT((char *)"#FEED");
                    //! 8.16.25 BLE CLIENT
                    //! returns whether connected over BLE as a client to a server(like a ESP feeder)
                    if (!isConnectedBLEClient())
                    {
                        //! If not connected over BLE -- then send the MQTT feed message.. ??? WHY??
                        //! 8.16.25 MQTT

                        //!only if not WIFI with BLE
                        //! 8.20.24 put back to MQTT feed to all .. for now..
                        //! SerialCall.println("async_send_feed.1");
                        sendMessageMQTT((char *)"#FEED");
                        SerialTemp.println(" *** NO BLE connected so send wifi ASYNC_SEND_MQTT_FEED_MESSAGE");
                        
                    }
                    else
                    {
                        SerialCall.println("async_send_feed.2");
                        incrementFeedCount_mainModule();
                        
                        //!send a BLE feed command as we are connected
                        sendFeedCommandBLEClient();
                        
                        //!perform ACK too
                        //! 8.16.25 MQTT
                        //ack is sent by the caller of this message..??
                        //! 11.26.25 send Status instead
                        //sendMessageMQTT((char *)"#ackMe");
                        sendMessageMQTT((char*)"#STATUS");
                    }
#ifdef NOT_NOW
                    //! 8.24.22 (This will redraw the screen - and turn off the blank screen (but not let anyone know)
                    //! SO: now only the buttons will unlock the blank screen..
                    
                    //!redraws the Semantic Marker image..
                    redrawSemanticMarker_displayModule(KEEP_SAME);
#endif //not_now
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
                    {
                        cmdToSend = "{'cmd':'buzzon'}";
                        //! click call synchronously ...
                        main_dispatchSyncCommand(SYNC_CLICK_SOUND);
                    }
                    else
                        cmdToSend = "{'cmd':'buzzoff'}";
 
                    //!OK: issue.
                    //!  if we are an ESP feeder, the STEPPER is on .. so we're good,
                    //! if an M5 - then either send over BLE or MQTT
#ifdef ESP_M5
                    //!send via BLE or MQTT ..
                    //! 8.16.25 BLE CLIENT
                    //! returns whether connected over BLE as a client to a server(like a ESP feeder)
                    if (!isConnectedBLEClient())
                    {
                        SerialLots.println("async_call_buzz_on/off -- not BLE connected.. send MQTT");
                        //! If not connected over BLE -- then send the MQTT buzzon message..
                        //! 8.16.25 MQTT
                        sendMessageMQTT((char*)cmdToSend.c_str());
                    }
                    else
                    {
                        //!send over BLE...
                        sendCommandBLEClient(cmdToSend);
                    }
#else  //not M5
                    
                    //! 8.16.25 BLE CLIENT
                    //! returns whether connected over BLE as a client to a server(like a ESP feeder)
                    if (!isConnectedBLEClient())
                    {
                        SerialLots.println("async_call_buzz_on -- not BLE connected.. send MQTT");
                        //! If not connected over BLE -- then send the MQTT buzzon message..
                        //! 8.16.25 MQTT
                        sendMessageMQTT((char *)cmdToSend.c_str());
                    }
                    else
                    {
                        SerialLots.println("async_call_buzz_on -- YES BLE connected.. send BLE");
                        processClientCommandChar_mainModule(isBuzzOn?'B':'b');
                        
                    }

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
                    //! 8.16.25 MQTT
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
                    //! 8.16.25 MQTT

                    //!These are the ASYNC_CALL_PARAMETERS_MAX
                    //!NO: just change our credentials ...
                    //send to
                    //main_dispatchAsyncCommandWithString(ASYNC_CALL_BLE_CLIENT_PARAMETER, credentials);
                    processJSONMessageMQTT(credentials, TOPIC_TO_SEND);
                }
                    break;
                case ASYNC_RESTART_WIFI_MQTT:
                    //! 8.16.25 MQTT
                    //!Restarts (or attempts) a restart of the WIFI using the existing credentials -- vs the 'n' command
                    restartWIFI_MQTTState();
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
    //! 12.19.25 Power back on, Snowing in mountains
    //! support the blink but using the plugin class
    //! 5.15.25 try the async CLICK
    //! click call  5.26.25 SYNC version
    main_dispatchSyncCommand(SYNC_CLICK_SOUND);
    
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
    //! 8.16.25 WIFI AP
    //!clean_SSID_WIFICredentials();
    //!now register an async call..
    main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_CREDENTIALS);
    
}

//!called on single click
//!NOTE: with BLE_CLIENT_NETWORKING, the right button and top button send a BLE command for feeding..
void singleClickTouched(char *whichButton)
{
    //! 8.16.25 WIFI AP
    //!for now, only send the FEED command via BLE_CLIENT if turned on. No reboot to AP mode yet..
    //! 8.16.25 BLE CLIENT
    //!send the async FEED
    
    //! dispatches a call to the command specified. This is run on the next loop()
    main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
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
    SerialDebug.println(PARTITION_SCHEME);
    SerialDebug.printf("M5Stack Version = %s\n", (char*)M5STACK_VERSION);
    
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
    //! 7.9.26 Nice summer.. up and dow, 20 mile bike today..
#ifdef ESP_M5_ATOM_S3
    SerialMin.println(F("[x] ESP_M5_ATOM_S3"));
#endif
#ifdef ESP_M5_ATOM_LITE
    SerialMin.println(F("[x] ESP_M5_ATOM_LITE"));
#else
    SerialMin.println(F("[ ] ESP_M5_ATOM_LITE"));
#endif
    
    SerialDebug.println(F(" ** MQTT BLE NETWORKING **"));
    
    //! 8.16.25 MQTT
    //! [x] USE_MQTT_NETWORKING
    SerialMin.println("[x] USE_MQTT_NETWORKING");
    
    //! 8.16.25 BLE SERVER
    //! [x] USE_BLE_SERVER_NETWORKING
    SerialMin.println("[x] USE_BLE_SERVER_NETWORKING");
    
    //! 8.16.25 BLE CLIENT
    //! [x] USE_BLE_CLIENT_NETWORKING
    SerialMin.println(F("[x] USE_BLE_CLIENT_NETWORKING"));
    
    //! 8.16.25 WIFI AP
    //! [x] USE_WIFI_AP_MODULE
    
    SerialMin.println(F("[x] USE_WIFI_AP_MODULE"));

    
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
    SerialMin.printf("[x] USE_SPIFF_MODULE/_SETTING on=%d\n",  getPreferenceBoolean_mainModule(PREFERENCE_USE_SPIFF_SETTING));
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
#ifdef USE_LIB_WEBSERVER
    SerialMin.println("[x] USE_LIB_WEBSERVER");
#else
    SerialMin.println("[ ] USE_LIB_WEBSERVER");
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

//!If nil it create one with just the null, so strlen = 0
//!NOTE: the strdup() might be used later..
char* createCopy2(const char * stringA)
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
}


//!adding a synchronous call to send a message over the network (assuming MQTT but not specified), this tacks on {device} and {t:time}
void sendMessageStringTopic_mainModule(char *messageString, char*topicString)
{
    //! 8.16.25 MQTT
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
        //! 8.16.25 MQTT
        sprintf(_messageStorage,"#Temp %f too high > %d - powering off {%s}", temperature, maxtemp, getDeviceNameMQTT());
        //!send this message over MQTT
        sendMessageMQTT(_messageStorage);
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
    //! 8.16.25 MQTT
    addStatusBooleanFlag("W",isConnectedWIFI_MQTTState());
    addStatusBooleanFlag("M",isConnectedMQTT_MQTTState());
    //! 8.16.25 BLE CLIENT
    //useBLECLient == it's linked in and running (vs not running)
    addStatusBooleanFlag("B",useBLEClient());
    //! connected == we are connected to another BLEServer
    addStatusBooleanFlag("C",isConnectedBLEClient());
    
    //! 8.16.25 WIFI AP
    //!not done is what we look for ..
    addStatusBooleanFlag("A",!doneWIFI_APModule_Credentials());
    
#pragma mark reused
#ifdef OVERLOADS_TIME_T
    //! 7.20.25 this breaks the T:<time> since there is a dublicate in the URL .. with no error
    addStatusBooleanFlag("T", getPreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE));
#endif
    
    //! 8.16.25 BLE SERVER
    addStatusBooleanFlag("S",getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE));
    {
        //! add a bleS=PTFeeder:name
        char *serverBLEName = getServiceName_BLEServerNetworking();
        addStatusStringFlag((char*)"bleS",serverBLEName);
    }
    
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
        //! 8.16.25 MQTT
        char *deviceName = getDeviceNameMQTT();
        //!TODO: make sure no spaces ... unless escaped
      
        sprintf(_fullStatusString,"status?T=%d&v=%s&dev=%s&b=%02.0f&temp=%02.0f&c=%0d&t=%0d",time, VERSION_SHORT, deviceName, getBatPercentage_mainModule(), getTemperature_mainModule(), getFeedCount_mainModule(), getLoopTimer_displayModule());
        
    }
    else
    {
        //_fullStatusString[0] = '\0';
        //! 8.16.25 MQTT
        char *deviceName = getDeviceNameMQTT();
        //!TODO: make sure no spaces ... unless escaped
        sprintf(_fullStatusString,"status?T=%d&v=%s&dev=%s",time, VERSION_SHORT, deviceName);
        
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
    //! 8.16.25 MQTT
    sendDocFollowMessageMQTT(SMDocFollowAddress);
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
            SerialDebug.println("Setting feederType = UNO");
            savePreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE, STEPPER_IS_UNO);
            
            //!Issue #332 8.17.2024
            savePreferenceInt_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING, 22.5);
            //! set autoRotoate as well..
            savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_AUTO_MOTOR_DIRECTION_SETTING, false);
            //! default to  clockwise == 0
            //! 8.18.24 setting this will check for the factory setting..
            setClockwiseMotorDirection_mainModule(true);
        } break;
        case 'm':
        {
            SerialDebug.println("Setting feederType = MINI");
            //save preference
            savePreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE, STEPPER_IS_MINI);
            //!Issue #332 8.17.2024
            savePreferenceInt_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING, 45.0);
            // turn clockwise..
            //! 8.18.24 setting this will check for the factory setting..
            setClockwiseMotorDirection_mainModule(true);
        } break;
        case 'L':
        {
            SerialDebug.println("Setting feederType = Tumbler");
            //save preference
            savePreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE, STEPPER_IS_TUMBLER);
            //!Issue #332 8.17.2024
            savePreferenceInt_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING, 200.0);
            //! set autoRotoate as well..
            savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_AUTO_MOTOR_DIRECTION_SETTING, true);
            //! set autoRotoate as well..
            //! 8.18.24 setting this will check for the factory setting..
            //setClockwiseMotorDirection_mainModule(true);
        } break;
            //! 11.25.25 toggle the buzzer, THIS is local already, but the toggle:buzzer will be ONLY be to a BLE device
            //! that way it sends 'F'
        case 'F':
        {
            SerialDebug.println("Toggle Buzzer via BLE: 'F'");
            //! then toggle value
            togglePreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE);
            
            //! will be turned on so make sound..
            //! the main sync command (no parameters yet)
            main_dispatchSyncCommand(SYNC_CLICK_SOUND);
#ifdef LOCAL_NOT_REMOTE
            if (buzzerOn)
                main_dispatchAsyncCommand(ASYNC_CALL_BUZZ_OFF);
            else
                main_dispatchAsyncCommand(ASYNC_CALL_BUZZ_ON);
#endif
        } break;
            //! this is a call to the connected device
        case 'f':
        {
            //! this will be a BLE message to the connectd device sending a 'F"
            SerialDebug.println("Toggle remove Buzzer via BLE: 'f'");
            //! then toggle value
            //! see if connected to a BLE device..
            char *connectedBLEDeviceName = connectedBLEDeviceName_mainModule()?connectedBLEDeviceName_mainModule():NULL;
            SerialDebug.printf("connectedBLEDeviceName = %s\n", connectedBLEDeviceName);
            
            if (connectedBLEDeviceName)
            {
                SerialDebug.printf("Call Toggle Buzzer via BLE: 'F' - dev=%s\n", connectedBLEDeviceName);

                //! send via BLE
                //!send a string of 13 or less characters.
                sendCommandBLEClient_13orLess("F");
            }
        } break;
        case 'B':
        {
            sendToStepperModule = true;
            SerialDebug.println("Setting buzzStatus = BUZZON");
            //save pref
            savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE, true);
            
            //! 11.27.25 Turkey Day, Nice day Mt Out.
            //! Also make the Buzzer sound
            
            //! will be turned on so make sound..
            //! the main sync command (no parameters yet)
            main_dispatchSyncCommand(SYNC_CLICK_SOUND);
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
   
            //!6.20.25
#ifdef NOT_SUPPORTED_RIGHT_NOW
        case 'O':
        {
            SerialDebug.println("OTA Update.. ");
            //! dispatches a call to the command specified. This is run on the next loop()
            main_dispatchAsyncCommand(ASYNC_CALL_OTA_UPDATE);
        } break;
#endif
#define SUPPORT_MINI_CLEAN
#ifdef SUPPORT_MINI_CLEAN
            //! 7.16.26 SpaceX 1 sec stoppage
            //! only suport Clean Credentials
        case 'X':
        {
            SerialDebug.println("Clean EPROM.. so it's first time again");
            //! dispatches a call to the command specified. This is run on the next loop()
            main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_EPROM);
        } break;
        case 'x':
        {
            SerialDebug.println("Clean SSID from EPROM.. ");
            char cleanWIFI[100];
            strcpy(cleanWIFI,"{'ssid':'','ssidPassword':''}");
            SerialDebug.println(cleanWIFI);
            //! 8.16.25 MQTT
            //! send to ourself
            sendMessageNoChangeMQTT((char*)cleanWIFI);
                        
        } break;
#else
        case 'X':
        case 'x':
#endif
        case 'R':
        {
            SerialDebug.println("Clean Credentials");
            //! dispatches a call to the command specified. This is run on the next loop()
            main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_CREDENTIALS);
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
            SerialDebug.println("unused cmd '_'");
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
        case 'k':
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
            setConfiguration_mainModule((char*)"L9110S_DCStepperClass");
        } break;
#ifdef NOT_ANYMORE
            //! 7.12.25
            //!  2 == default for SMART Button
        case '2':
        {
            setConfiguration_mainModule((char*)"ULN2003_StepperClass");
        } break;
#endif
            //! 12.21.25 Winter Solstice, Eagles,
            //! support the new Chain Buttons
        case '2':
        {
            setConfiguration_mainModule((char*)"ChainButtonClass");
        } break;
            //! 7.19.25 add Clear Sensors
        case '3':
        {
            setConfiguration_mainModule((char*)"PTStepper");
        } break;
            //! 8.18.26 use this for the mode of a released PTStepper
            case '=':
        {
            SerialDebug.println("Setting modes for (a) PTStepper with (b) E mode 8.18.26");
            //! E mode first . don't use the DEVICE_NAME
            savePreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING,false);

           //! do the PTStepper but first set the E mode
            setConfiguration_mainModule((char*)"PTStepper");
            
            //! cannot do anything else, as the line above reoots the computer
        }
            break;
        case '4':
        {
            setConfiguration_mainModule((char*)"M5AtomCamera");
        } break;
            //! 7.9.25 grabbed from BOOTSTRAP let someone update the atom to the recent OTA
        case '5':
        {
            SerialDebug.println(" *** performing m5atom OTA Update");
            //! 8.16.25 MQTT
            //!retrieves from constant location
            performOTAUpdate((char*)"http://KnowledgeShark.org", (char*)"OTA/TEST/M5Atom/ESP_IOT.ino.m5stick_c_plus.bin");
        } break;
            //! 7.9.25 grabbed from BOOTSTRAP let someone update the atom to the recent OTA
        case '6':
        {
            SerialDebug.println(" *** performing m5atom OTA Update - DAILY");
            //! 8.16.25 MQTT
            //!retrieves from constant location
            performOTAUpdate((char*)"http://KnowledgeShark.org", (char*)"OTA/TEST/M5Atom/daily/ESP_IOT.ino.m5stick_c_plus.bin");
        } break;
        case '7':
        {
            SerialDebug.println(" *** performing m5atom OTA Update - BOOTSTRAP");
            //! 8.16.25 MQTT
            //!retrieves from constant location
            performOTAUpdate((char*)"http://KnowledgeShark.org", (char*)"OTA/Bootstrap/ESP_M5_BOOTSTRAP.ino.m5stack_stickc_plus.bin");
        } break;
#pragma mark M5AtomS3 OTA
            //! 7.10.26 M5AtomS3 (without display)
            //        SerialDebug.println("   y -  _m5atomS3");
            //        SerialDebug.println("   Y -  _mm5atomS3Daily");
            //! must be first..
        case 'y':
        {
            SerialDebug.println(" *** performing m5atomS3 OTA Update");
            
            //!retrieves from constant location
            performOTAUpdate((char*)"http://KnowledgeShark.org", (char*)"OTA/TEST/M5AtomS3/ESP_IOT.ino.m5stick_c_plus.bin");
        }break;
        case 'Y':
        {
            SerialDebug.println(" *** performing m5atomS3 OTA Update - DAILY");
            
            //!retrieves from constant location
            performOTAUpdate((char*)"http://KnowledgeShark.org", (char*)"OTA/TEST/M5AtomS3/daily/ESP_IOT.ino.m5stick_c_plus.bin");
        }break;
            //! 8.11.26 added the platform.io bin (which Wes creates)
        case '+':
        {
            SerialDebug.println(" *** performing m5atomS3_platform.io OTA Update - DAILY");
            
            //!retrieves from constant location
            performOTAUpdate((char*)"http://KnowledgeShark.org", (char*)"OTA/TEST/M5AtomS3/platform.io/daily/ESP_IOT.ino.m5stick_c_plus.bin");
        }break;
            //! 8.15.26 add - for the released version
        case '-':
        {
            SerialDebug.println(" *** performing m5atomS3_platform.io OTA Update - RELEASED");
            
            //!retrieves from constant location
            performOTAUpdate((char*)"http://KnowledgeShark.org", (char*)"OTA/TEST/M5AtomS3/platform.io/ESP_IOT.ino.m5stick_c_plus.bin");
        }break;
            //! 8.18.25
        case '8':
        {
            setConfiguration_mainModule((char*)"M5NoModule");
        } break;
        case '9':
        {
            setConfiguration_mainModule((char*)"M5TinyGPS");
        } break;
        case '0': //m5atom_qrode
        {
            setConfiguration_mainModule((char*)"M5AtomScanner");
        } break;
#ifdef USE_6
        case '9':
        {
            //! 8.18.25 M5Clicker
            SerialDebug.println(" *** performing m5atom OTA Update - M5Clicker");
            //! 8.16.25 MQTT
            //!retrieves from constant location
            performOTAUpdate((char*)"http://KnowledgeShark.org", (char*)"OTA/Bootstrap/ESP_M5_BOOTSTRAP.ino.m5stack_stickc_plus.bin");
        } break;
#endif
        case 'C':
        {
            SerialDebug.println("'C' change color touched");
#ifdef USE_FAST_LED
            CRGB randomColor = getRandomColor();
            fillpix(randomColor);
            delay(50);
#else
            //! 8.2.26 random LED color
            setLEDColor(getRandomColor());
#endif
        }
            break;
        case 'i':
        {
            char sendCapture[100];
            strcpy(sendCapture,"{'send':'capture'}");
            SerialDebug.println(sendCapture);
            //! 8.16.25 MQTT
            //! send to ourself
            sendMessageNoChangeMQTT((char*)sendCapture);
            
        }
#pragma mark BLE_SETTINGS
            //! 8.11.26 BLE Server and Client on/off
        case '!':
        {
            SerialDebug.println("       ! -  BLE_Client on");
            savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE, true);
            //! reboot .. so the sensors are set..
            rebootDevice_mainModule();
        }
            break;
        case '@':
        {
            SerialDebug.println("       @ -  BLE_Client off");
            savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE, false);
            //! reboot .. so the sensors are set..
            rebootDevice_mainModule();
        }
            break;
        case '#':
        {
            SerialDebug.println("       # -  BLE_Server on");
            savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE, true);
            //! reboot .. so the sensors are set..
            rebootDevice_mainModule();
        }
            break;
        case '$':
        {
            SerialDebug.println("       $ -  BLE_Server off");
            savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE, false);
            //! reboot .. so the sensors are set..
            rebootDevice_mainModule();
        }
            break;
            
#pragma mark HELP
        case '.':
        {
            
            //! returns if the BLEClient is turned on.. note, if connected to a BLE device, then disconnect
            
            boolean val = getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE);
            //! toggle spiff on/off
            boolean spiffFlag = getPreferenceBoolean_mainModule(PREFERENCE_USE_SPIFF_SETTING);
            SerialTemp.println(VERSION);
            SerialMin.println("Valid Commands: ");
            SerialMin.println("         . = help, this message");
            SerialMin.println("         p = poweroff if can");
            SerialMin.println(" 0x0, s, c == Single Feed - via BLE to connected device");
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
            
            //! this is local buzzer on/off calling the current Atom Class
            SerialMin.println("         F == Toggle Buzzer on/off");
            //! this will be a BLE message to the connectd device sending a 'F"
            SerialMin.println("         f == Toggle Buzzer on/off on BLE connected device");

            SerialMin.println("         B == Buzzer On");
            SerialMin.println("         b == Buzzer Off");
            SerialMin.println("         G == Gateway On");
            SerialMin.println("         g == Gateway Off");
            SerialMin.println("         R == clean credentials");
            SerialMin.println("         X == clean EPROM <<<< FIRST TIME DO THIS <<<< 7.26.26");
            SerialMin.println("         x == clean SSID from EPROM");
            SerialMin.println("         r == reboot ");
            //!6.20.25
            
            SerialMin.println("         T == tiltOn");
            SerialMin.println("         t == tiltOff");
            SerialMin.println("         N == send WIFI Credential to BLEServer");
            SerialMin.println("         n == next WIFI Credential");
            SerialMin.println("         W == retry WIFI");
            SerialMin.println("         w == swap WIFI");
            SerialMin.println("         C == change m5 atom to random color");
            SerialMin.println("         i == take pic, {send:capture}");

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
            SerialMin.println("    Configurations of M5Atom  ");

            SerialMin.println("         k == no sensors");
            SerialMin.println("         1 == L9100S_DCStepper (current main)");
            //SerialMin.println("         2 == ULN2002 Stepper (new test 8.13.25)");
            SerialMin.println("         2 == ChainSensors (12.21.25))");
            SerialMin.println("         3 == *** PTStepperClass (original code - with M5");
            SerialMin.printf ("         4 == M5AtomCamera\n");
            SerialMin.println("         8 == SMART Button, Clicker");
            SerialMin.println("         9 == M5TinyGPS, Clicker");
            SerialMin.println("         0 == M5AtomScanner, Clicker");
            SerialMin.println("         = == E mode, then PTStepper (3)");


#ifdef NOT_SUPPORTED_RIGHT_NOW
            SerialMin.println("         O == OTA update");
#else
            SerialMin.println("    OTA Updates of M5AtomLite -board M5StickCPlus ");
            SerialMin.println("         5 == m5atom DEV OTA update");
            SerialMin.println("         6 == m5atom DAILY TEST DEV OTA update");
            SerialMin.println("         7 == go back to m5atom BOOTSTRAP");

            //! 7.10.26 M5AtomS3 (without display)
            SerialDebug.println("   OTA updates for M5AtomS3 - board M5AtomS3");
            SerialDebug.println("       y -  _m5atomS3");
            SerialDebug.println("       Y -  _mm5atomS3Daily");
            SerialDebug.println();
            //! 8.11.26 M5AtomS3_platformIO
            SerialDebug.println("   OTA updates for M5AtomS3 - board M5AtomS3 - platform.io");
            SerialDebug.println("       +   _mm5atomS3 Daily_platform.io");
            SerialDebug.println("       -   _mm5atomS3 Released _platform.io");

            SerialDebug.println();
#endif
            //! 8.11.26 BLE Server and Client on/off
            SerialDebug.println("   Turn On/Off BLE Server and Client");
            SerialDebug.println("       ! -  BLE_Client on");
            SerialDebug.println("       @ -  BLE_Client off");
            SerialDebug.println("       # -  BLE_Server on");
            SerialDebug.println("       $ -  BLE_Server off");
            SerialDebug.println("       test -- create test user ");

            SerialDebug.println();
            
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

//! 8.20.25 Horses grazing if field first time for Mark and Bud
//! add set/config
//! PTStepper
//! M5AtomCamera
void setConfiguration_mainModule(char* configurationName)
{
    SerialDebug.printf("setConfiguration: %s\n", configurationName);
    
    if (strcmp(configurationName, "PTStepper")==0)
    {
        //! poweroff
        SerialDebug.println("Default PTStepperClass");
        //resetSensorToDefault_mainModule();
        //! 7.30.25 changing to the HDriver's board
        //!     savePreference_mainModule(PREFERENCE_SENSORS_SETTING, _sensorsEPROM);
#ifdef DONT_ADD_TO_SENSORS
        setSensorsString_mainModule((char*)"BuzzerSensorClass,21,25,PTStepperClass,23,33");
#else
#ifdef ESP_M5_ATOM_S3
        //! 7.12.26 Ashton's 12th birtyday, Bike ride 20
        //! seems the AtomS3 blows UP with 19,22
        //! now they are 39, 38
        //! now -1, 39
        setSensorsString_mainModule((char*)"BuzzerSensorClass,-1,39");

#else
     //   setSensorsString_mainModule((char*)"BuzzerSensorClass,21,25");
        //! try -1 and 21 ...
        //! 7.14.26 warm day, fluffing up hay in field
        
         setSensorsString_mainModule((char*)"BuzzerSensorClass,-1,21");

#endif

#endif
        //! 7.31.25 if Scanner or QR then pin 22 used .. so make M5HDriver (basically don't have a sensor)
        savePreference_mainModule(PREFERENCE_ATOM_KIND_SETTING, "M5HDriver");
        //! also specify the sensor plug
        savePreference_mainModule(PREFERENCE_SENSOR_PLUGS_SETTING, "PTStepperClass");
        //! for Tumbler .. use 200
        savePreferenceFloat_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING, 250.0);
        //! @see #390 this is the RPM of the stepper
        //savePreferenceFloat_mainModule(PREFERENCE_STEPPER_RPM_SETTING, 15.0);
        //! set autoRotoate as well..
        savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_AUTO_MOTOR_DIRECTION_SETTING, true);
        //! tumbler
        savePreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE, STEPPER_IS_TUMBLER);
        
        //! 8.18.26 default buzzer OFF
        savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE, false);

        //! 9.5.25 set the 2step OFF for now
        savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_2FEED_SETTING, false);

        //! 10.24.25 also set BLE client and serve on,
        //! RAIN .. atosmopheric river
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE, true);
        
        //!if set, the BLE Server (like PTFeeder) will tack on the device name (or none if not defined).
        //!8.18.26 don't set it either way, use E if want other..
        //savePreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING,true);

        //!if set, the BLE client OFF 8.18.26
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE,false);

        //! reboot .. so the sensors are set..
        rebootDevice_mainModule();
    }
#pragma mark CHAIN BUTTON
    //! 12.21.25 Winter Solstice, Eagles,
    //! Chain Button
    else if (strcmp(configurationName, "ChainButtonClass")==0)
    {
        //! poweroff
        SerialDebug.println("Default ChainButtonClass  **** hardcoded button messages ****");
        //resetSensorToDefault_mainModule();
#ifdef ESP_M5_ATOM_S3
        //! 7.12.26 Ashton's 12th birtyday, Bike ride 20
        //! seems the AtomS3 blows UP with 19,22
        //! now they are G6 and G5
        //setSensorsString_mainModule((char*)"ChainButtonClass,6,5");
        //! 8.7.26 w Laura, swap pins .. RXD, TXD
        //! 8.8.26 Row Jimmy, Bike warm nice. less smoke, put back the 6,5 pins..
        setSensorsString_mainModule((char*)"ChainButtonClass,6,5");

#else
        //! 7.30.25 changing to the HDriver's board
        setSensorsString_mainModule((char*)"ChainButtonClass,19,22");
#endif
        
        //! 7.31.25 if Scanner or QR then pin 22 used .. so make M5HDriver (basically don't have a sensor)
        savePreference_mainModule(PREFERENCE_ATOM_KIND_SETTING, "M5AtomNoModule");
        //! also specify the sensor plug (non for now..
        savePreference_mainModule(PREFERENCE_SENSOR_PLUGS_SETTING, "");
       
        //! 4.3.26 Off to the Moon
        //! This will send WIFI along with BLE (if double click). or WIFI if not BLE connected
        savePreferenceBoolean_mainModule(PREFERENCE_SENDWIFI_WITH_BLE, true);
        
        
        //!TODO.. Chain
        //!presskind = 0, short, 1, long, 2, longlong
        //!ButtonKind, number_1based, pressKind, setString, valString, device
        //!if localhost then internal message, otherwise external MQTT message
        savePreference_mainModule(PREFERENCE_CHAIN_SENSORS_SETTING, "ChainButtonClass,1,0,SM_MatrixCell,8,$all,ButtonClass,1,1,feed,,$pair,JoystickClass,2,0,feed,,$localhost");
        
        //! iniitlize from the string ..
        initChainSensorStringsFromEPROM_mainModule();
        
        //! idea is you define the button and the press kind
        //! MatrixCell is Column Major  1,4, 7 // 2,5,8 // 3,6,9
        registerChain_mainModule("ButtonClass",   1, BUTTON_SHORT_PRESS,  "SM_MatrixCell", "15", "$dev" );
        registerChain_mainModule("ButtonClass",   1, BUTTON_LONG_PRESS,   "SM_MatrixCell", "6", "$dev");
        registerChain_mainModule("JoystickClass", 2, BUTTON_SHORT_PRESS,  "feed", "", "localhost" );


        //! 10.24.25 also set BLE client and serve on,
        //! RAIN .. atosmopheric river
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE, true);
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE, true);
        
        //!if set, the BLE Server (like PTFeeder) will tack on the device name (or none if not defined).
        savePreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING,true);
        
        //! this initializes as well as prints (I think) 8.8.26
        processClientCommandChar_mainModule('.');

        //! reboot .. so the sensors are set..
        // FOR NOW until parser workign .. don't reboot ...
        rebootDevice_mainModule();
    }
    else if (strcmp(configurationName, "ULN2003_StepperClass")==0)
    {
        /*
         #define IN1 22
         #define IN2 19
         #define IN3 23
         #define IN4 33
         */
        //! poweroff
        SerialDebug.println("Default ULN2003_StepperClass");
        //resetSensorToDefault_mainModule();
        //! 7.30.25 changing to the HDriver's board
        setSensorsString_mainModule((char*)"BuzzerSensorClass,21,25");
        //! 7.31.25 if Scanner or QR then pin 22 used .. so make M5HDriver (basically don't have a sensor)
        savePreference_mainModule(PREFERENCE_ATOM_KIND_SETTING, "M5HDriver");
        //! also specify the sensor plug
        savePreference_mainModule(PREFERENCE_SENSOR_PLUGS_SETTING, "ULN2003_StepperClass");
        //! 1 second motor (overloads "angle" field)
        //! This is the RPM speed
        savePreferenceFloat_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING, 2048.0);
        //! @see #390 this is the RPM of the stepper
        savePreferenceFloat_mainModule(PREFERENCE_STEPPER_RPM_SETTING, 15.0);
        //! set autoRotoate as well..
        savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_AUTO_MOTOR_DIRECTION_SETTING, true);
        //! tumbler
        savePreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE, STEPPER_IS_TUMBLER);
        
        //!if set, the BLE Server (like PTFeeder) will tack on the device name (or none if not defined).
        savePreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING,true);
        
        //! reboot .. so the sensors are set..
        rebootDevice_mainModule();
    }
    else if (strcmp(configurationName, "M5AtomCamera")==0)
    {
        //! poweroff
        SerialDebug.println("Default M5AtomCamera");
        //resetSensorToDefault_mainModule();
        //! 7.30.25 changing to the HDriver's board
        setSensorsString_mainModule((char*)"");
        //! 7.31.25 if Scanner or QR then pin 22 used .. so make M5HDriver (basically don't have a sensor)
        savePreference_mainModule(PREFERENCE_ATOM_KIND_SETTING, "M5AtomCamera");
        //! also specify the sensor plug
        savePreference_mainModule(PREFERENCE_SENSOR_PLUGS_SETTING, "");
        
        //! 10.24.25 also set BLE client and serve on,
        //! RAIN .. atosmopheric river
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE, true);
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE, true);
        
        //!if set, the BLE Server (like PTFeeder) will tack on the device name (or none if not defined).
        savePreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING,true);
        
        //! reboot .. so the sensors are set..
        rebootDevice_mainModule();
    }
    else if (strcmp(configurationName, "M5TinyGPS")==0)
    {
        //!
        SerialDebug.println("Default M5AtomTinyGPS");
        //resetSensorToDefault_mainModule();
        //! 7.30.25 changing to the HDriver's board
        setSensorsString_mainModule((char*)"");
        //! 7.31.25 if Scanner or QR then pin 22 used .. so make M5HDriver (basically don't have a sensor)
        savePreference_mainModule(PREFERENCE_ATOM_KIND_SETTING, "M5AtomTinyGPS");
        //! also specify the sensor plug (non for now..
        savePreference_mainModule(PREFERENCE_SENSOR_PLUGS_SETTING, "");
        
        //! 10.24.25 also set BLE client and serve on,
        //! RAIN .. atosmopheric river
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE, true);
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE, true);
        
        //!if set, the BLE Server (like PTFeeder) will tack on the device name (or none if not defined).
        savePreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING,true);
        
        //! reboot .. so the sensors are set..
        rebootDevice_mainModule();
    }
    else if (strcmp(configurationName, "M5AtomScanner")==0)
    {
        //!
        SerialDebug.println("Default M5AtomScanner");
        //resetSensorToDefault_mainModule();
        //! 7.30.25 changing to the HDriver's board
        setSensorsString_mainModule((char*)"");
        //! 7.31.25 if Scanner or QR then pin 22 used .. so make M5HDriver (basically don't have a sensor)
        savePreference_mainModule(PREFERENCE_ATOM_KIND_SETTING, "M5AtomScanner");
        //! also specify the sensor plug (non for now..
        savePreference_mainModule(PREFERENCE_SENSOR_PLUGS_SETTING, "");
        
        //! 10.24.25 also set BLE client and serve on,
        //! RAIN .. atosmopheric river
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE, true);
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE, true);
        
        //!if set, the BLE Server (like PTFeeder) will tack on the device name (or none if not defined).
        savePreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING,true);
        
        //! reboot .. so the sensors are set..
        rebootDevice_mainModule();
    }
    else if (strcmp(configurationName, "M5NoModule")==0)
    {
        //!
        SerialDebug.println("Default M5AtomNoModule");
        //resetSensorToDefault_mainModule();
        //! 7.30.25 changing to the HDriver's board
        setSensorsString_mainModule((char*)"");
        //! 7.31.25 if Scanner or QR then pin 22 used .. so make M5HDriver (basically don't have a sensor)
        savePreference_mainModule(PREFERENCE_ATOM_KIND_SETTING, "M5AtomNoModule");
        //! also specify the sensor plug (non for now..
        savePreference_mainModule(PREFERENCE_SENSOR_PLUGS_SETTING, "");
        
        //! 10.24.25 also set BLE client and serve on,
        //! RAIN .. atosmopheric river
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE, true);
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE, true);
        
        //!if set, the BLE Server (like PTFeeder) will tack on the device name (or none if not defined).
        savePreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING,true);
        
        //! reboot .. so the sensors are set..
        rebootDevice_mainModule();
    }
    else
    {
        SerialDebug.printf(" **** Unknown Configuration: %s\n", configurationName);
    }
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
    SerialLots.printf("Unix Time: %d\n", now);
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
    //! 8.16.25 BLE CLIENT
    if (isConnectedBLEClient())
        return _fullBLEDeviceName;
    else
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
   
    //! 8.16.25 BLE CLIENT

    //! if connected, return the connected name, otherwise return empty string
    if (isConnectedBLEClient())
    {
        return nameToUse;
    }
    else
    {
        return (char*)"";
    }
}

//!returns address part of name.
char *connectedBLEDeviceNameAddress_mainModule()
{
    //! 8.16.25 BLE CLIENT

    //! if connected, return the connected name, otherwise return empty string
    if (isConnectedBLEClient())
    {
        return _connectedBLEDeviceAddress;
    }
    else
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
#endif
    
#ifdef M5_ATOM
  
#pragma mark M5AtomClassType

    //! 5.6.25 use the M5Atom ClassType
#ifdef USE_NEW_M5ATOMCLASS
    //! @see https://www.cs.fsu.edu/~myers/cop3330/notes/dma.html
    _M5Atom_SocketModuleClass = new M5Atom_SocketModuleClass((char*)"M5AtomSocket");
    _M5Atom_QRCodeModuleClass = new M5Atom_QRCodeModuleClass((char*)"M5AtomScanner");
    _M5Atom_HDriverModuleClass = new M5Atom_HDriverModuleClass((char*)"M5HDriver");
    //! 7.17.25
    _M5Atom_TinyGPSModuleClass = new M5Atom_TinyGPSModuleClass((char*)"M5AtomTinyGPS");
    //! 8.16.25 bring in the Camera
    _M5Atom_CameraModuleClass = new M5Atom_CameraModuleClass((char*)"M5AtomCamera");
    //! 10.26.25 Power Out .. Wind Storm. Generator Running
    //! add a NoModule class
    _M5Atom_NoModuleClass = new M5Atom_NoModuleClass((char*)"M5AtomNoModule");

    int whichM5AtomIndex = 0;
    SerialDebug.println("setup_M5Atoms");
    
    //! 3.31.25 create array of plugs
    _M5AtomClassTypes[whichM5AtomIndex++] = _M5Atom_SocketModuleClass;
    _M5AtomClassTypes[whichM5AtomIndex++] = _M5Atom_QRCodeModuleClass;
    _M5AtomClassTypes[whichM5AtomIndex++] = _M5Atom_HDriverModuleClass;
    //! 7.17.25
    _M5AtomClassTypes[whichM5AtomIndex++] = _M5Atom_TinyGPSModuleClass;
    //! 8.16.25 bring in the Camera
    _M5AtomClassTypes[whichM5AtomIndex++] = _M5Atom_CameraModuleClass;
    //! 10.26.25 Power Out .. Wind Storm. Generator Running
    //! add a NoModule class
    _M5AtomClassTypes[whichM5AtomIndex++] = _M5Atom_NoModuleClass;
    
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
    
#pragma mark DEBUG_7_9_26
    {
        if (!atomKind || strlen(atomKind) ==0)
        {
            SerialDebug.println(" **** no atomKind set .. use M5AtomNoModule");
            atomKind = (char*)"M5AtomNoModule";
        }
        SerialDebug.printf("**** setting M5AtomNoModule %s\n", atomKind);
        // still don't get it.. but for motor lets change this...
//        atomKind = "M5AtomNoModule";
//        processClientCommandChar_mainModule('3');
    }
//    SerialDebug.println(" **** no atomKind set .. use M5AtomNoModule");
//    atomKind = "M5AtomNoModule";
//    SerialDebug.println(" **** no atomKind set .. use M5AtomNoModule");
#pragma mark endDebug
    
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
#pragma mark DEBUG_7_9_26
    //SerialDebug.println("** DEBUG: xxx");
    
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
    {
        SerialDebug.println("buttonA_ShortPress_mainModule");
        _whichM5AtomClassType->buttonA_ShortPress_M5AtomClassType();
    }

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
    {
        SerialDebug.println("buttonA_LongPress_mainModule");
        _whichM5AtomClassType->buttonA_LongPress_M5AtomClassType();
    }

#elif defined(M5BUTTON_MODULE)
    buttonA_LongPress_M5ButtonModule();
#endif
}

//! 11.25.25 add long long
//!long press on buttonA (top button)
void buttonA_LongLongPress_mainModule()
{
#ifdef USE_BUTTON_MODULE
    //buttonA_LongLongPress();
    //! 7.11.26 nedw M5AtomS3a  NOT: let's turn off USE_BUTTON for now
    //buttonA_LongLongPress_mainModule();

#endif
#ifdef M5CORE2_MODULE
    buttonA_LongLongPress_M5Core2Module();
#elif defined(M5_ATOM)
    
#pragma mark USE_NEW_M5ATOMCLASS
    //! 5.6.25 use object version
    if (_whichM5AtomClassType)
    {
        SerialDebug.println("buttonA_LongLongPress_mainModule");
        _whichM5AtomClassType->buttonA_LongLongPress_M5AtomClassType();
    }
    else
    {
        SerialDebug.println(" *** buttonA_LongLongPress undefined ****");
    }
    
#elif defined(M5BUTTON_MODULE)
    buttonA_LongLongPress_M5ButtonModule();
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
#pragma mark BUTTON Processing of M5

//!big button on front of M5StickC Plus
void checkButtonA_MainModule()
{
    _shortPress_MainModule = false;
    _longPress_MainModule = false;
    _longLongPress_MainModule = false;
#ifdef M5UNIFIED
    //!NOTE: ths issue is the timer is interruped by the scanner.. so make long-long very long..
    //was 1000  (from 500)
    if (M5.BtnA.wasPressed())
    {
        //        buttonA_longPress_MainModule();
        SerialDebug.println("MainModule M5.BtnA **** wasPressed ***");
        _shortPress_MainModule = true;
    }
    else if (M5.BtnA.wasReleasefor(4500))
    {
        //        buttonA_longPress_MainModule();
        SerialDebug.println("MainModule M5.BtnA **** LONG LONG PRESS ***");
        _longLongPress_MainModule = true;
    }
    else if (M5.BtnA.wasReleasefor(1000))
    {
        //        buttonA_longPress_MainModule();
        SerialDebug.println("MainModule M5.BtnA **** LONG PRESS ***");
        _longPress_MainModule = true;
    }
    else if (M5.BtnA.wasReleased())
    {
        //        buttonA_shortPress_MainModule();
        SerialDebug.println("MainModule M5.BtnA **** wasReleased ***");
        _shortPress_MainModule = true;
    }
#elif defined(ESP_M5_ATOM_S3)
    //!NOTE: ths issue is the timer is interruped by the scanner.. so make long-long very long..
    //was 1000  (from 500)
    if (AtomS3.BtnA.wasPressed())
    {
        //        buttonA_longPress_MainModule();
        SerialDebug.println("MainModule M5.BtnA **** wasPressed ***");
        _shortPress_MainModule = true;
    }
    else if (AtomS3.BtnA.wasReleasefor(4500))
    {
        //        buttonA_longPress_MainModule();
        SerialDebug.println("MainModule M5.BtnA **** LONG LONG PRESS ***");
        _longLongPress_MainModule = true;
    }
    else if (AtomS3.BtnA.wasReleasefor(1000))
    {
        //        buttonA_longPress_MainModule();
        SerialDebug.println("MainModule M5.BtnA **** LONG PRESS ***");
        _longPress_MainModule = true;
    }
    else if (AtomS3.BtnA.wasReleased())
    {
        //        buttonA_shortPress_MainModule();
        SerialDebug.println("MainModule M5.BtnA **** wasReleased ***");
        _shortPress_MainModule = true;
    }
#else
    //!NOTE: ths issue is the timer is interruped by the scanner.. so make long-long very long..
    //was 1000  (from 500)
    if (M5.BtnA.wasPressed())
    {
        //        buttonA_longPress_MainModule();
        SerialDebug.println("MainModule M5.BtnA **** wasPressed ***");
        _shortPress_MainModule = true;
    }
    else if (M5.BtnA.wasReleasefor(4500))
    {
        //        buttonA_longPress_MainModule();
        SerialDebug.println("MainModule M5.BtnA **** LONG LONG PRESS ***");
        _longLongPress_MainModule = true;
    }
    else if (M5.BtnA.wasReleasefor(1000))
    {
        //        buttonA_longPress_MainModule();
        SerialDebug.println("MainModule M5.BtnA **** LONG PRESS ***");
        _longPress_MainModule = true;
    }
    else if (M5.BtnA.wasReleased())
    {
        //        buttonA_shortPress_MainModule();
        SerialDebug.println("MainModule M5.BtnA **** wasReleased ***");
        _shortPress_MainModule = true;
    }
#endif
}
//!big button on front of M5StickC Plus
void checkButtonB_MainModule()
{
    //! OOPS:: these override the A_versions....
    //! just ADD to the state..
    if (true)
    {
        _shortPressB_MainModule = false;
        _longPressB_MainModule = false;
        _longLongPressB_MainModule = false;
        _doublePressB_MainModule = false;

    }
#ifdef ESP_M5
    
    //!NOTE: ths issue is the timer is interruped by the scanner.. so make long-long very long..
    //was 1000  (from 500)
    if (M5.BtnB.wasReleasefor(4500))
    {
        //        buttonA_longPress_MainModule();
        SerialDebug.println("MainModule M5.BtnB **** LONG LONG PRESS ***");
        _longLongPress_MainModule = true;
    }
    else if (M5.BtnB.wasReleasefor(1000))
    {
        //        buttonA_longPress_MainModule();
        SerialDebug.println("MainModule M5.BtnB **** LONG PRESS ***");
        _longPress_MainModule = true;
    }
    else if (M5.BtnB.wasReleased())
    {
        //        buttonA_shortPress_MainModule();
        SerialDebug.println("MainModule M5.BtnB **** SHORT PRESS ***");
        _shortPressB_MainModule = true;
    }
    /// Returns true when some time has passed since the button was double clicked.
    //!@see bool wasDoubleClicked(void) const { return _currentState == state_decide_click_count && _clickCount == 2; }

    else if (M5.BtnB.wasDoubleClicked())
    {
        //! one idea:  double press is like single press. then long preess is A's long press? But need B to more through menues of display
        SerialDebug.println("MainModule M5.BtnB **** DOUBLE PRESS ***");
        _doublePressB_MainModule = true;
    }
#endif
}

#pragma mark Models
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

#pragma mark PIN_USE_STRUCT
#pragma mark PIN USE to see if overlaps.
//! 7.31.25 add this for a status, saw that QRCode was using 22 also  .. so buzer didn't work.
/**
#define PIN_USE_MAX 10
struct pinUseStruct {
    int pinUseCount;
     //! string describing the module, etc
     char *pinUseArray[PIN_USE_MAX];
     //! each pin
     long pinNumArray[PIN_USE_MAX];
} _pinUseStruct;
*/

//! global for use. This is an object (not a pointer) and has all the storage created statically
PinUseStruct _pinUseStruct;
//! get the pin use array
PinUseStruct* getPinUseStruct_mainModule()
{
    return &_pinUseStruct;
}


//! 7.31.25 store this information.. for STATUS
//! 5.3.25 add a central clearing house for defining PIN use
//! @see issue #365
//! central clearing house for all pins used to we can analyze if there are overlaps
//! pin is the actual number, pinName is the local name (eg. IN1_PIN or VIN_PIN).
//! moduleName is the module in the code,
//! isI2C is whether this is a I2C bus (which we aren't using much yet)
//! //! 7.13.26 Ballon morning .. field haying
//! return Boolean saying the pin was Valid of Invalie
boolean registerPinUse_mainModule(long pin, String pinName, String moduleName, boolean isI2C)
{
    char pinUseSample[100];
    sprintf(pinUseSample,"PIN_USE: %2d = %s, %s %s", pin, pinName.c_str(), moduleName.c_str(), isI2C?"(I2C)":"");
    
    //! check if pins are valid ..
    boolean validPins = true;
#ifdef ESP_M5_ATOM_S3
    switch (pin)
    {
        case -1: // support this as a power
        case 0: //main button hod 2s
        case 1: //Port.A
        case 2: //Port.A
        case 4: //IR
        case 5:
        case 6:
        case 7:
        case 8:
        case 39:
        case 38:
        case 41: //BTN
        case 34: //LCD-RST
        case 21: //MOSI
        case 17: //SCK
        case 15: //CS
        case 33: //RS
        case 16: //BL
        case 35: //RGB ??
            validPins = true;
            break;
        default:
            validPins = false;
            break;
    }
#else
    switch (pin)
    {
        case -1: // support this as a power
        case 0: //Main button hod 2s ???
        case 26: //Port.A
        case 32: //Port.A
        case 12: //IR
        case 22:
        case 19:
        case 23:
        case 25:
        case 39: //BTN
            // don't know these ...
        case 34: //LCD-RST ??
        case 21: //MOSI
        case 17: //SCK
        case 15: //CS
        case 33: //RS
        case 16: //BL
        case 27: //LED ??
      //  case 2: //?? added 7.25.26
            validPins = true;
            break;
        default:
            validPins = false;
    }
#endif
    if (!validPins)
    {
        SerialError.printf("**** Invalid pin: %d, %s\n", pin, pinUseSample);
        return false;
    }
    //! 5.3.25 create storage here
    char *pinUse = (char*)calloc(strlen(pinUseSample)+1, sizeof(char));
    strcpy(pinUse, pinUseSample);
    //!store globally
    _pinUseStruct.pinUseArray[_pinUseStruct.pinUseCount] = pinUse;
    //! 8.30.25 add the pin
    _pinUseStruct.pinNumArray[_pinUseStruct.pinUseCount] = pin;

    //! increment
    _pinUseStruct.pinUseCount++;
    
    if (_pinUseStruct.pinUseCount >= PIN_USE_MAX)
    {
        SerialError.printf("*** ERROR .. too many PINS defined ***");
        _pinUseStruct.pinUseCount=0;

    }
    
    SerialDebug.printf("** PIN_USE: %s = %d, module=%s %s\n", pinName.c_str(), pin, moduleName.c_str(), isI2C?"(I2C)":"");
    
    return true;
}

#pragma mark LED Buttons
#ifdef M5UNIFIED

//! these are the constanst for the colors
// TFT_WHITE, TFT_CYAN, TFT_RED, TFT_YELLOW, TFT_BLUE, TFT_GREEN


long _randomColorIndex = 0;
#define MAX_COLORS 6
//! 7.24.25 return a (semi) random color
int getRandomColorUnified() {
    _randomColorIndex = (++_randomColorIndex) % MAX_COLORS;
    int color = TFT_BLUE;
    switch (_randomColorIndex) {
        case 0:
            color = TFT_WHITE;
            break;
        case 1:
            color = TFT_CYAN;
            break;
        case 2:
            color = TFT_RED;
            break;
        case 3:
            color = TFT_YELLOW;
            break;
        case 4:
            color = TFT_BLUE;
            break;
            
        default:
        case 5:
            color = TFT_GREEN;
            break;
    }
    return color;
}
//! returns a string for the color int
String getColorStringUnified(int color)
{
    String colorString = "TFT_RED";
    switch (color) {
        case TFT_WHITE: colorString = "TFT_WHITE"; break;
        case TFT_CYAN: colorString = "TFT_CYAN"; break;
        case TFT_RED: colorString = "TFT_RED"; break;
        case TFT_YELLOW: colorString = "TFT_YELLOW"; break;
        case TFT_BLUE: colorString = "TFT_BLUE"; break;
            
        default:
        case TFT_GREEN: colorString = "TFT_GREEN"; break;
    }
    return colorString;
}
//!this is the M5UNIFIED version ...
//! uses the color numbers TFT_GREEN, etc
void setLEDColorUnified(int color)
{
    SerialLots.printf("setLEDColorUnified%s)\n", getColorStringUnified(color));
    M5.Led.setAllColor(color);
}
#endif

//! 7.24.25 return a (semi) random color
ColorsUnified getRandomColor()
{
    _randomColorIndex = (++_randomColorIndex) % MAX_COLORS;
    ColorsUnified color = RED_LED;
    switch (_randomColorIndex) {
        case 0:
            color = WHITE_LED;
            break;
        case 1:
            color = CYAN_LED;
            break;
        case 2:
            color = RED_LED;
            break;
        case 3:
            color = YELLOW_LED;
            break;
        case 4:
            color = BLUE_LED;
            break;
        default:
        case 5:
            color = GREEN_LED;
            break;
    }
    return color;
}
//! returns a string for the color int
String getColorString(ColorsUnified color)
{
    String colorString = "RED_LED";
    switch (color) {
        case RED_LED: colorString = "RED_LED"; break;
        case BLUE_LED: colorString = "BLUE_LED"; break;
        case GREEN_LED: colorString = "GREEN_LED"; break;
        case YELLOW_LED: colorString = "YELLOW_LED"; break;
        case CYAN_LED: colorString = "CYAN_LED"; break;
            
        default:
        case WHITE_LED: colorString = "WHITE_LED"; break;
    }
    return colorString;
}

//! uses colors.. _GREEN to be typed values
void setLEDColor(ColorsUnified color)
{
    SerialDebug.printf("setLEDColor(%s)\n", getColorString(color));
#ifdef M5UNIFIED
    // M5.Led.setAllColor(color);
    setLEDColorUnified((int)color);
#else
    //! Anyone?
    SerialDebug.printf("setLEDColor(%s) NOT IMPLEMENTED\n", getColorString(color));
#endif
    
}

