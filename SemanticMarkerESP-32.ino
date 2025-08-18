/*
  Name:		NimBLE_PetTutor_Server.ino
  Created:	8/25/2021 7:49:11 PM
  Author:	wes and scottl
*/

//!defines.h defines the #ifdef USE_MODULE statements, as they are included elsewhere.. Don't define them here.
#include "Defines.h"

#ifdef ESP_M5
//NEED to fix the port to use...   /dev/cu.usbserial-61CEEBBFAD
//todo..
//#define HIGH 1
#define LED 2
//#include "src/ButtonModule/ButtonProcessing.h"
//NOTE: the Board has to be modified to M5Stick-C-Plus

//called from BLEServerNetworking, MainModule and StepperModule(Dispense.cpp)
void stepperModule_ProcessClientCmd(char cmd)
{
  SerialError.printf("UNDEFINED: stepperModule_processClientCmd: %c\n", cmd);
}
#else

#endif  //ESP_M5


//NOTE: there might be an issue that the REGISTERED methods aren't done yet .. so cannot call them..
boolean _finishSetupCalledAlready = false;;

//forward definition
void finishSetup();

#include "nvs_flash.h"
// ******************************************
//!note: setup only called one time...
//! so, the first time the doneWIFIModuleCredentials is set.. call setup()
//!main setup
void setup()
{
    
#ifdef ERASE_ALL
    //https://github.com/espressif/arduino-esp32/issues/2144
    ESP_ERROR_CHECK(nvs_flash_erase());
    nvs_flash_init();
    
#endif
    //#define TRY_NEW_SERIAL_WAIT
    //! https://forum.arduino.cc/t/do-i-really-need-while-serial/1183292/16
    //! trying to get the debug to printout always, not just on REBOOT
  Serial.begin(115200);

#ifdef TRY_NEW_SERIAL_WAIT
    while (!Serial){};
#endif
    Serial.println("ESP_IOT Startup");
    
    //! added 8.29.23 .. figure if this is right!
    //! 9.25.23 .. took out .. might be cause of crash..
    pinMode(LED, OUTPUT);
    //! 5.3.25 register our PIN use
    registerPinUse_mainModule(LED, "LED_OUTPUT", "ESP_IOT", false);
    
    
    //This must be first .. as it inializes the display and Serial Ports..
    //! called for the loop() of this plugin
    setup_displayModule();
    
    SerialInfo.println("******************");
    SerialMin.println(VERSION);
    
    //Timestamp the compile
    SerialDebug.print("Compiled ");
    SerialDebug.print(__DATE__);
    SerialDebug.print(" @ ");
    SerialDebug.println(__TIME__);
    
    //not good time .. since no WIFI to get global time..
    //  printInt_SPIFFModule(getTimeStamp_mainModule());
    //  println_SPIFFModule((char*)" time");
    
    //called in setup() to init the async calls
    initAsyncCallFlags();
    
    //!4.17.22
    //! MainModule does all the preferences, etc..
    setup_mainModule();
    
    main_printModuleConfiguration();
    SerialInfo.println("******************");
    
    //! 8.17.25 register when SSID completed
    //registerCallbackMain(CALLBACKS_MQTT, )
    
#ifdef USE_SPIFF_MODULE
    //!SPIFF must be after main_setup() since it reads preferences .. is first so debug can be saved
    setup_SPIFFModule();
    //println_SPIFFModule((char*)VERSION);
    println_SPIFFModule_JSON((char*)"version",(char*)VERSION);
    
#endif
    
    //! 2.4.24 (2.4.68 lisa birthday). Mine 2.5.58 tomorrow
    //! The setup_Buttons_mainModule() is really a setup_Sensors so
    //! take away the #ifdef
    //calls the setup() for ButtonModule
    setup_Sensors_mainModule();
    
#ifdef USE_BUTTON_MODULE
    //register the single click
    registerCallbackMain(CALLBACKS_BUTTON_MODULE, SINGLE_CLICK_BM, &singleClickTouched);
#endif
    
    
    //!12.8.22 add audio
#ifdef USE_AUDIO_MODULE
    setup_AudioModule();
#endif
    
#ifdef USE_STEPPER_MODULE
    //reads credentials, etc..
    setup_StepperModule();
    
#ifdef FEED_ON_STARTUP
    //! 6.6.25 this is because the M5Atom on power-on will send current to the feeder
    //! so when we get here .. we try to break the "feed" by actually feeding for a fraction of a second 0.0
    // setup_StepperModule();
    {
        SerialDebug.println("stopMotor ..");
        stopMotor_mainModule();
    }
#endif //FEED ON STARTUP
    
#endif //USE_STEPPER_MODULE
    
#ifdef USE_UI_MODULE
    //setup the stepper for the feeder
    setup_UIModule();
#endif
    
    SerialDebug.println("Starting PetTutor_Server");
    SerialDebug.printf("M5STACK VERSION = %s\n", M5STACK_VERSION);
    
    //APril 8, refactoring. Moved up to here..
    //! 8.16.25 MQTT
    
    //pass the callbacks, (1) the message callback, and the (blinkLed) implementation
    //setMessageCallbacks(&feedMessageCallback, &blinkMessageCallback);
    //register the 2 callbacks for now..
    registerCallbackMain(CALLBACKS_MQTT, MQTT_CALLBACK_FEED, &feedMessageCallback);
    registerCallbackMain(CALLBACKS_MQTT, MQTT_CALLBACK_BLINK, &blinkMessageCallback);
    registerCallbackMain(CALLBACKS_MQTT, MQTT_CALLBACK_SOLID_LIGHT, &solidLightMessageCallback); //use the string "ON", "OFF"
    
    //3.25.22 -- register the cleanSSID_EPROM
    registerCallbackMain(CALLBACKS_MQTT, MQTT_CLEAN_SSID_EPROM, &cleanSSID_EPROM_MessageCallback);
    
    //NOTE: 6.3.22 .. this is breaking the AP mode, preSetup_WIF_APModule must be called first..
    //try call from loop():
    //  setup_MQTTNetworking();
    
    //the tilt  5.2.22  (no one calls this ...)
    registerCallbackMain(CALLBACKS_MQTT, MQTT_CALLBACK_TILT_MOTION_DETECTED, &feedMessageCallback);
    
    //! 8.16.25 WIFI AP
    //called to validate the doneWIFI() call...
    preSetup_WIFI_APModule();
    
    //init the BLE here.
    //! 8.16.25 BLE SERVER
    //yes
    //!if the BLE_SERVER is turned on..
    boolean useBLEServer = true;
#ifdef ESP_M5
    useBLEServer = getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE);
#endif
    if (useBLEServer)
    {
        //Initialize the BLE Server, using the service name PTFeeder
        //https://stackoverflow.com/questions/20944784/why-is-conversion-from-string-constant-to-char-valid-in-c-but-invalid-in-c
        //! 8.16.25 MQTT
        
        //*** The callback for "onWrite" of the bluetooth "onWrite'
        registerCallbackMain(CALLBACKS_BLE_SERVER, BLE_SERVER_CALLBACK_ONWRITE, &onWriteBLEServerCallback);
        //*** The callback for "onWrite" of the bluetooth "onWrite'
        registerCallbackMain(CALLBACKS_BLE_SERVER, BLE_SERVER_CALLBACK_STATUS_MESSAGE, &onStatusMessageBLEServerCallback);
        registerCallbackMain(CALLBACKS_BLE_SERVER, BLE_SERVER_CALLBACK_STATUS_MESSAGE, &onStatusMessageBLEServerCallback);
        
        //strdup() get away from the
        setup_BLEServerNetworking(MAIN_BLE_SERVER_SERVICE_NAME, getDeviceNameMQTT(), strdup(PT_SERVICE_UUID), strdup(PT_CHARACTERISTIC_UUID));
        
        SerialDebug.println("done setupBLEServerNetworking");
    }
    
    //BLE Client --
    //! 8.16.25 BLE CLIENT
    setup_BLEClientNetworking(MAIN_BLE_CLIENT_SERVICE, (char*) PT_SERVICE_UUID, (char*) PT_CHARACTERISTIC_UUID);
    
    //MAJOR ISSUE:  if the port/server are wrong .. can never reset unless we can get a BUTTON..
    //If BOOTSTRAP (a compile time ifdef in Defines.h is set, then bypass the AP mode, and use the constants in MQTTNetworking.cpp
#ifdef BOOTSTRAP
#else
    if (!doneWIFI_APModule_Credentials())
    {
        //Turn on light until out of the AP mode..
        solidLightOnOff(true);
        //test
        WIFI_APModule_ResetCredentials();
        
        //set flag ..
        _finishSetupCalledAlready = false;
        
        setup_WIFI_APModule();
    }
#endif //BOOTSTRAP

}

// **************** finishSetup()
//! called to finsh the setup.. this is because the MQTT and WIFI aren't run if in AP mode..
void finishSetup()
{
    //! 8.16.25 WIFI AP
    //basically after the AP module finished, the function WIFI_APModule_JsonOutputString() stores the credentials (versus them calling here ..)
    if (doneWIFI_APModule_Credentials())
    {
        //Turn on light until out of the AP mode..
        solidLightOnOff(false);
        
        String jsonOutputString = WIFI_APModule_JsonOutputString();
        // now this could be passed onto the MQTT processor..
        SerialDebug.print("From WIFI -> JSON = ");
        SerialDebug.println(jsonOutputString);
        
        //! 8.16.25 MQTT
        //moved here 6.3.22
        setup_MQTTNetworking();
        
        //! 8.16.25 WIFI AP
        //pass the json string onto the MQTT code..
        // 3.26.22
        SerialDebug.println("calling .. processJSONMessage");
        
        //NOTE: if done was because credentials .. then don't call this..
        //basically if MQTT running .. don't call this..
        processJSONMessageMQTT(&jsonOutputString[0], NULL);
        
        //NOTE: we are still in the BLE callback .. and then we call setup_MQTTNetworking..
        //LETS try setting state in MQTT that is waiting for WIFI..
        
        //perform the setup() in the MQTTNetworking
        //NOTE: this is called 2 times if processJSONMessage() sets the eprom data .. since the network was already setup
        //there is a state flag internally that won't setup 2 times (unless MQTT was kicked out via AP mode, or cleaned credentials)
        //try call from loop():
        //setup_MQTTNetworking();
        
        // sets the _chipName (from MQTT Module)
        char *deviceName = getDeviceNameMQTT();
        
        //THIS should output the device name, user name, etc...
        //    SerialDebug.print("pFoodCharacteristic->setValue: ");
        //    SerialDebug.println(_fullJSONString);
        SerialDebug.print("DeviceName: ");
        SerialDebug.println(deviceName);
        
        //! 8.16.25 WIFI AP
    } //doneWIFI_APModule_Credentials()
    
    //! try a call..  NOTE, if the WIFI isn't working, this won't work either..
#ifdef USE_REST_MESSAGING
    setupSecureRESTCall();
#endif
    
    //! 1.20.24 setup the camera after the WIFI is working ..
    //! 8.16.25 do this now with the OO module in main..
#ifdef USE_CAMERA_MODULE_x
    setup_CameraModule();
#endif
    
}

// ****************************************
//! main loop() of the Arduino runtime
// ****************************************
void loop() {
    //#define USE_STOP_ALL
    
    //! try a call..
#ifdef USE_REST_MESSAGING
    // sendSecureRESTCall("/help");
#endif
    
#ifdef USE_STOP_ALL
    if (stopAllProcesses_mainModule())
    {
        delay(100);
        return;
    }
#endif
    
    //!4.17.22
    loop_mainModule();
    
    //! 8.16.25
    //! TODO: change these loops to the M5Sensor plugin model..
#ifdef USE_AUDIO_MODULE
    //!12.8.22 add audio
    loop_AudioModule();
#endif

        //! 8.16.25 do this now with the OO module in main..
#ifdef USE_CAMERA_MODULE_x
    loop_CameraModule();
#endif

    //! 8.16.25 WIFI AP
    if (doneWIFI_APModule_Credentials())
    {
        //since the MQTT and BLE were not started, the first time the WIFI credentials are finished (after submit)
        // finish the setup (which are the MQTT and WIFI.
        //NOTE: this needs to get the credentials in JSON format and store ..
        if (!_finishSetupCalledAlready)
        {
            _finishSetupCalledAlready = true;
            SerialDebug.println("doneWIFI_APModuleCredentials,  and now calling finishSetup()");
            finishSetup();
        }
    }
    
    //! called to check if any Async Commands are in dispatch mode
    invokeAsyncCommands();
    
    //! note: displayModule() musts be called before buttonModule (as the "model" is updated (initiated) in the display module
    //! called for the loop() of this plugin (this is almost the "frames" of the graphic, as fast as the runtime can make this loop! FAST.. 24 fps ??
    loop_displayModule();
    
    //! loop on the buttons
    loop_Sensors_mainModule();
    
    /** Do your thing here, this just spams notifications to all connected clients */
#ifdef USE_STEPPER_MODULE
    loop_StepperModule(); //was FeederStateMachine
#endif //USE_STEPPER_MODULE
    
#ifdef USE_UI_MODULE
    loop_UIModule();
#endif
    
    //BLE Client --
    //! 8.16.25 BLE CLIENT
    loop_BLEClientNetworking();
    
    
    //** NOTE: the WIFI_AP_MODULE is tricky. The doneWIFI_APModule_Credentials() is set when done, but also when the MQTT networking is running (meaning the credentials were specified via BLE, or from EPROM)
    //! 8.16.25 WIFI AP
    //if the APmodule credentials are finished .. then continue normal loop
    // Otherwise just BLE and AP (for now)
    if (doneWIFI_APModule_Credentials())
    {
        /*******************************MQTT*************************************/
        //! 8.16.25 MQTT
        loop_MQTTNetworking();
        
        
        //! 8.16.25 BLE SERVER
        //!if the BLE_SERVER is turned on..
        boolean useBLEServer = true;
#ifdef ESP_M5
        useBLEServer = getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE);
#endif
        if (useBLEServer)
            //! old ESP32 didn't have the "server" turned on..
            loop_BLEServerNetworking();
        
        
        //! 8.16.25 WIFI AP
    } //!doneWIFI_APModuleCredentials
    
    //! 8.16.25 WIFI AP
    if (!doneWIFI_APModule_Credentials())
    {
        loop_WIFI_APModule();
    }
    
    //APril 8, 2022
    //loop always, we just don't have WIFI working yet...?
    //NOTE: the loopBLE will call the register for a message (and call processJSONMessage on BLE commands)
    //NEED: to set the doneWIFI_APModuleCredentials  iff a processJSONMessage resulted in credential updates,
    // not all JSON are credentials. Others are setting options, like OTA.  -- and kill the AP module..
    //UNFORTUNATELY .. the WIFI and MQTT might fail .. and slow things down. NEED a better way to
    // run the MQTT loop (so others can get some time, like BLE).
    
#ifdef ESP_32
    delay(100);  //this delays the feed trigger response
#else
    delay(50); // 1.26.24 try a smaller loop delay
#endif
} //loop()
