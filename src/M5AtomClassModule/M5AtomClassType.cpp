//
//  M5AtomClassType.cpp
//  
//
//  Created by Scott Moody on 5/6/25.
//

#include "M5AtomClassType.h"

M5AtomClassType::M5AtomClassType(char *config)
{
    SerialLots.printf("M5AtomClassType init %s\n", config);
    //! 5.3.25 create storage here
    _identityString = (char*)calloc(strlen(config)+1, sizeof(char));
    strcpy(_identityString, config);
}

M5AtomClassType::~M5AtomClassType()
{
    SerialDebug.printf("M5AtomClassType destroy\n");
}

//! returns the identity .. actaully the config
//! This can be used to see if this class instance matches
//! a known name (like DCMotorStepper, etc)
char *M5AtomClassType::classIdentity()
{
    SerialDebug.printf("M5AtomClassType::classIdentity = %s\n", _identityString);
    return _identityString;
}

//!6.6.24 D-Day 81 years
//!return true or false if this should be a PTFeeder or PTClicker
//!default 'false' to PTClicker.   The Bluetooth will use this
boolean M5AtomClassType::isPTFeeder_M5AtomClassType()
{
    return false;
}

//! 11.25.25 Add LONG LONG press for the AP mode
//! default unless overridded
void M5AtomClassType::buttonA_LongLongPress_M5AtomClassType()
{
    SerialDebug.printf("base class: buttonA_LongLongPress_M5AtomClassType\n");

    //! dispatches a call to the command specified. This is run on the next loop()
    main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_CREDENTIALS);
}

