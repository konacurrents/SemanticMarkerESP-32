//
//  M5Atom_NoModuleClass.cpp
//  ESP_IOT
//
//  Created by Scott Moody on 10/26/25.
//

#include "M5Atom_NoModuleClass.h"


#ifdef USE_FAST_LED
#include "../ATOM_LED_Module/M5Display.h"
#endif

//! TODO.. have only 1 of these included in build, but change the callback
//! That requires that ATOM be a class, OR there is a single Sensor but different callback..
//#define KEY_UNIT_SENSOR_CLASS in defines.h
#ifdef  KEY_UNIT_SENSOR_CLASS

#include "../SensorClass/SensorClassType.h"
#include "../SensorClass/KeyUnitSensorClass.h"


//#include "TimerDelayClass.h"
//! create instance of the timer class
TimerDelayClass *_timerDelayClass_M5NoModuleClass;

//! default .. this could be set via Preferences (TODO)
float _delaySeconds_M5NoModuleClass_setting = 1.5;

//! keyUnitSensorClass object for the BUTTON
KeyUnitSensorClass *_KeyUnitSensorClass_M5AtomNoModule;

//!a pointer to a callback function that takes (char*) and returns void
void M5AtomCallback_M5AtomNoModule(char *parameter, boolean flag)
{
    SerialDebug.printf("M5AtomCallback_M5AtomNoModule.sensorCallbackSignature(%s,%d)\n", parameter, flag);
    
    //   sendMessageString_mainModule((char*)"M5Atom.KEY Pressed ");
        
    
    ///feed always  (done after the code below..)
    main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
    
    //!send status after power on/off change..
    //sendStatusMQTT_mainModule();
    
}
#endif //KEY_UNIT_SENSOR_CLASS

M5Atom_NoModuleClass::M5Atom_NoModuleClass(char *config) : M5AtomClassType(config)
{
    SerialDebug.printf("M5Atom_NoModuleClass init %s\n", config);
    
    _timerDelayClass_M5NoModuleClass = new TimerDelayClass(_delaySeconds_M5NoModuleClass_setting);
}


//! Data members of class
//! Generic config info .. specific the the sensor type

//!starts the PTStepper
void M5Atom_NoModuleClass::start_M5AtomClassType()
{
}

//!setup the PTStepper
void M5Atom_NoModuleClass::setup_M5AtomClassType()
{
    SerialDebug.println("M5Atom_NoModuleClass::setup_M5AtomClassType");
    
#ifdef USE_FAST_LED
    SerialDebug.println("M5Atom_NoModuleClass.M5.begin");
    
    //  M5.begin(true,false,true);
    //!NOTE: this could probably be done by ESP_IOT.ino .. but for now keep here (and in the other ATOM code..)
    setup_M5Display();
    //  fillpix(L_GREEN);
    fillpix(L_BLUE);
    M5.update();

#endif
    
    
#ifdef KEY_UNIT_SENSOR_CLASS
    _KeyUnitSensorClass_M5AtomNoModule = new KeyUnitSensorClass((char*)"KeyUnitSensorClass_M5AtomNoModule");
    //! specify the callback
    _KeyUnitSensorClass_M5AtomNoModule->registerCallback(&M5AtomCallback_M5AtomNoModule);
    //! call the setup
    _KeyUnitSensorClass_M5AtomNoModule->setup();
    SerialDebug.printf("_Key = %d\n", _KeyUnitSensorClass_M5AtomNoModule);
#endif
    
}



//! stops motor
void M5Atom_NoModuleClass::stop_M5AtomClassType()
{
    
    //!user timer class instance
    _timerDelayClass_M5NoModuleClass->stopDelay();
}

//!setup the PTStepper
void M5Atom_NoModuleClass::loop_M5AtomClassType()
{
    M5.update();

#ifdef KEY_UNIT_SENSOR_CLASS
    //SerialDebug.printf("_Key = %d\n", _KeyUnitSensorClass_M5AtomNoModule);
    
    _KeyUnitSensorClass_M5AtomNoModule->loop();
#endif
    
    //! see if timer done .. change color back
    //!user timer class instance
    if (_timerDelayClass_M5NoModuleClass->delayFinished())
    {
#ifdef USE_FAST_LED
        fillpix(L_BLUE);
        M5.update();
#endif
    }
    
}

//! ATOM specific

//! these are from the ATOM

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 12.27.23 support setName == "socket"
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void M5Atom_NoModuleClass::messageSetVal_M5AtomClassType(char *setName, char* valValue, boolean deviceNameSpecified)
{
    
}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the  send -
void M5Atom_NoModuleClass::messageSend_M5AtomClassType(char *sendValue, boolean deviceNameSpecified)
{
  
}

//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void M5Atom_NoModuleClass::buttonA_ShortPress_M5AtomClassType()
{
    SerialDebug.println("M5Atom::buttonA_ShortPress_M5Atom_NoModuleClass");
    //! feed
    SerialDebug.printf("Feed BLE\n");
    // send the _lastSemanticMarker again ...
    //!send this as a DOCFOLLOW message
    //sendSemanticMarkerDocFollow_mainModule(_lastSemanticMarker);
    //        strcpy(_lastSemanticMarker,"https://iDogWatch.com/bot/feed/test/test");
    
#ifdef USE_FAST_LED
    fillpix(L_RED);
    M5.update();

#endif
    
    ///feed always  (done after the code below..)
    main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);

    //! start the delay..  (which at .5 might not be needed...)
    //! 10.26.25 when done .. change to BLUE again
    _timerDelayClass_M5NoModuleClass->startDelay(_delaySeconds_M5NoModuleClass_setting);
}
//!long press on buttonA (top button)
void M5Atom_NoModuleClass::buttonA_LongPress_M5AtomClassType()
{
    
    SerialDebug.println("M5Atom_HDriverModuleClass::buttonA_LongPress_M5Atom_NoModuleClass");
    //
    SerialDebug.printf("CLEAN CREDENTIALS and reboot to AP mode\n");
    
    //! dispatches a call to the command specified. This is run on the next loop()
    main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_CREDENTIALS);
}

//!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * M5Atom_NoModuleClass::currentStatusURL_M5AtomClassType()
{
    //! 8.4.25 add k=GPS
    //! 8.4.25 add k=HD  (HDriver)
    return (char*)"&k=NO";
}

//!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * M5Atom_NoModuleClass::currentStatusJSON_M5AtomClassType()
{
    return (char*)"'kind':'NO'";
}


//!6.6.24 D-Day 81 years
//!return true or false if this should be a PTFeeder or PTClicker
//!default 'false' to PTClicker.   The Bluetooth will use this
boolean M5Atom_NoModuleClass::isPTFeeder_M5AtomClassType()
{
    SerialDebug.printf("M5Atom_NoModuleClass isPTFeeder = false\n");
    
    return false;
}
