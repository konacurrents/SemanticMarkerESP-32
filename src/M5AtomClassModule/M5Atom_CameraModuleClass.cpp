//
//  M5Atom_CameraModuleClass.cpp
//  ESP_IOT
//
//  Created by Scott Moody on 8/16/25.
//

#include "M5Atom_CameraModuleClass.h"


#ifdef USE_CAMERA_MODULE
#include "../CameraModule/CameraModule.h"
#endif

M5Atom_CameraModuleClass::M5Atom_CameraModuleClass(char *config) : M5AtomClassType(config)
{
    SerialDebug.printf("M5Atom_CameraModuleClass init %s\n", config);
    
}


//! Data members of class
//! Generic config info .. specific the the sensor type

//!starts the PTStepper
void M5Atom_CameraModuleClass::start_M5AtomClassType()
{
    
}

//!setup the PTStepper
void M5Atom_CameraModuleClass::setup_M5AtomClassType()
{
#ifdef USE_CAMERA_MODULE
    //! call setup from CameraModule
    setup_CameraModule();
#endif
    
    //! 8.16.25 register our PIN use
    registerPinUse_mainModule(4,  "CameraPinOut", "M5Atom_CameraModuleClass", false);
    
    SerialDebug.println("M5Atom_CameraModuleClass.setup");
}

//! stops motor
void M5Atom_CameraModuleClass::stop_M5AtomClassType()
{
}

//!setup the PTStepper
void M5Atom_CameraModuleClass::loop_M5AtomClassType()
{
#ifdef USE_CAMERA_MODULE
    loop_CameraModule();
#endif
}

//! ATOM specific

//! these are from the ATOM

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 12.27.23 support setName == "QRCode"
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void M5Atom_CameraModuleClass::messageSetVal_M5AtomClassType(char *setName, char* valValue, boolean deviceNameSpecified)
{
    //messageSetVal_M5CameraModule(setName, valValue, deviceNameSpecified);
    SerialDebug.printf("camera.set %s val: %s\n", setName, valValue);

}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the  send -
void M5Atom_CameraModuleClass::messageSend_M5AtomClassType(char *sendValue, boolean deviceNameSpecified)
{
    SerialDebug.printf("camera.send: %s nameSpecified=%d\n", sendValue, deviceNameSpecified);
    if (containsSubstring(sendValue, "capture") || containsSubstring(sendValue, "CAPTURE"))
    {
        SerialDebug.println("takePicture_CameraModule");
#ifdef USE_CAMERA_MODULE
        takePicture_CameraModule();
#endif
    }
}

//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void M5Atom_CameraModuleClass::buttonA_ShortPress_M5AtomClassType()
{
    //buttonA_ShortPress_M5CameraModule();
}
//!long press on buttonA (top button)
void M5Atom_CameraModuleClass::buttonA_LongPress_M5AtomClassType()
{
    //buttonA_LongPress_M5CameraModule();
}

//! for status
char _cameraStatusBuffer[100];

//!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * M5Atom_CameraModuleClass::currentStatusURL_M5AtomClassType()
{
    //! 8.4.25 add k=GPS
    sprintf(_cameraStatusBuffer,"camera=on");
    return (char*)_cameraStatusBuffer;
}

//!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * M5Atom_CameraModuleClass::currentStatusJSON_M5AtomClassType()
{
    sprintf(_cameraStatusBuffer,"'camera':'on'");
    return (char*)_cameraStatusBuffer;
}

