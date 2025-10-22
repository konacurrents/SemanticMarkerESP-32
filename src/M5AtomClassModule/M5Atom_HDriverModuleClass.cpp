//
//  M5Atom_HDriverModuleClass.cpp
//  
//
//  Created by Scott Moody on 5/6/25.
//

#include "M5Atom_HDriverModuleClass.h"

//! 8.30.25 LA Warm, Zuma Beach - end scene of Planet of Apes
#ifdef USE_FAST_LED
#include "../ATOM_LED_Module/M5Display.h"
#endif

//! Currently the HDriver only processes the BUZZER .. so the setup works..
M5Atom_HDriverModuleClass::M5Atom_HDriverModuleClass(char *config) : M5AtomClassType(config)
{
    SerialDebug.printf("M5Atom_HDriverModuleClass init %s\n", config);
    
}

//!6.6.24 D-Day 81 years
//!return true or false if this should be a PTFeeder or PTClicker
//!default 'false' to PTClicker.   The Bluetooth will use this
boolean M5Atom_HDriverModuleClass::isPTFeeder_M5AtomClassType()
{
    SerialDebug.printf("M5Atom_HDriverModuleClass isPTFeeder = TRUE\n");

    return true;
}

//! Data members of class
//! Generic config info .. specific the the sensor type

//!starts the PTStepper
void M5Atom_HDriverModuleClass::start_M5AtomClassType()
{
}

//!setup the PTStepper
void M5Atom_HDriverModuleClass::setup_M5AtomClassType()
{
    SerialDebug.printf("M5Atom_HDriverModuleClass::setup_M5AtomClassType\n");
#ifdef USE_FAST_LED
        //!NOTE: this could probably be done by ESP_IOT.ino .. but for now keep here (and in the other ATOM code..)
    setup_M5Display();
    //  fillpix(L_GREEN);
    fillpix(L_BLUE);
    
#endif

}

//! stops motor
void M5Atom_HDriverModuleClass::stop_M5AtomClassType()
{
    
}

//!setup the PTStepper
void M5Atom_HDriverModuleClass::loop_M5AtomClassType()
{
}

//! ATOM specific

//! these are from the ATOM

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 12.27.23 support setName == "HDriver"
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void M5Atom_HDriverModuleClass::messageSetVal_M5AtomClassType(char *setName, char* valValue, boolean deviceNameSpecified)
{
    
}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the  send -
void M5Atom_HDriverModuleClass::messageSend_M5AtomClassType(char *sendValue, boolean deviceNameSpecified)
{
    SerialDebug.printf("M5Atom_HDriverModuleClass::messageSend_M5AtomClassType(%s)\n", sendValue);

}

//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void M5Atom_HDriverModuleClass::buttonA_ShortPress_M5AtomClassType()
{
    SerialDebug.println("M5Atom::buttonA_ShortPress_M5AtomClassType");
    //! feed
    SerialDebug.printf("Feed BLE\n");
    // send the _lastSemanticMarker again ...
    //!send this as a DOCFOLLOW message
    //sendSemanticMarkerDocFollow_mainModule(_lastSemanticMarker);
    //        strcpy(_lastSemanticMarker,"https://iDogWatch.com/bot/feed/test/test");
    
    ///feed always  (done after the code below..)
    main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
}
//!long press on buttonA (top button)
void M5Atom_HDriverModuleClass::buttonA_LongPress_M5AtomClassType()
{
    
    SerialDebug.println("M5Atom_HDriverModuleClass::buttonA_LongPress_M5AtomClassType");
    //
    SerialDebug.printf("CLEAN CREDENTIALS and reboot to AP mode\n");
    
    //! dispatches a call to the command specified. This is run on the next loop()
    main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_CREDENTIALS);
}

//!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * M5Atom_HDriverModuleClass::currentStatusURL_M5AtomClassType()
{
    //! 8.4.25 add k=HD  (HDriver)
    return (char*)"&k=HD";
}

//!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
//!   (SINGLE QUOTES)
char * M5Atom_HDriverModuleClass::currentStatusJSON_M5AtomClassType()
{
    return (char*)"'kind':'HD'";
}
