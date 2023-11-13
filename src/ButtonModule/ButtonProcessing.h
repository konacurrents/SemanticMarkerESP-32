//
//  ButtonProcessing.h
//  M5Stick
//
//  Created by Scott Moody on 3/9/22.
//


#include "../../Defines.h"
#ifdef USE_BUTTON_MODULE

//continuation of ButtonModule

//!the loop for buttonProcessing (extension of ButtonModule)
void loop_ButtonProcessing();
//!the setup for buttonProcessing (extension of ButtonModule)
//!  在 M5StickC Plus 启动或者复位后，即会开始执行setup()函数中的程序，该部分只会执行一次。
void setup_ButtonProcessing();


//! called by the feed operation to say the device is still running.. and count it as a button click.
void refreshDelayButtonTouched_ButtonProcessing();


//! These button presses are exposed so a MQTT message can invoke them..
//! {'set':'buttonA','val':'longpress'}
//! {'set':'buttonA','val':'shortpress'}
//! {'set':'buttonB','val':'longpress'}
//! {'set':'buttonB','val':'shortpress'}

//!short press on buttonA (top button)
void buttonA_ShortPress();
//!long press on buttonA (top button)
void buttonA_LongPress();
//!the long press of the side button
void buttonB_LongPress();
//!the short press of the side button
void buttonB_ShortPress();
#endif //USE_BUTTON_MODULE
