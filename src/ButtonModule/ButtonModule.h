/*
*******************************************************************************
  Scott Version: 3.24.2022
 BM = ButtonModule
 USE_BUTTON_MODULE
*******************************************************************************
*/

/*******************************ButtonModule *************************************/
//TODO: where there are multiple buttons, allow for register(button1,button2, ...)
//Also a way to define buttons ...  eg button1 = feed, button1,quickPress = Single Feed, button1,longPress=AutoFeed .. etc.//

#ifndef ButtonModule_h
#define ButtonModule_h

#include "../../Defines.h"
#ifdef USE_BUTTON_MODULE

//#include "ButtonProcessing.h"

void setup_ButtonModule();
void loop_ButtonModule();

#define SINGLE_CLICK_BM 0
#define MAX_CALLBACKS_BM 1



//! called by the feed operation to say the device is still running.. and count it as a button click.
void refreshDelayButtonTouched_ButtonModule();

//!This is only thing exposed to others.. (Kinda which only 1 button module)
//!short press on buttonA (top button)
void buttonA_ShortPress_ButtonModule();
//!long press on buttonA (top button)
void buttonA_LongPress_ButtonModule();
//!the long press of the side button
void buttonB_LongPress_ButtonModule();
//!the short press of the side button
void buttonB_ShortPress_ButtonModule();

#endif

#endif //ButtonModule_h
