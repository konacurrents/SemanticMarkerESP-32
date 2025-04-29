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

#ifndef M5ButtonModule_h
#define M5ButtonModule_h

#include "../../Defines.h"
#ifdef M5BUTTON_MODULE

//#include "ButtonProcessing.h"

void setup_M5ButtonModule();
void loop_M5ButtonModule();

#define SINGLE_CLICK_BM 0
#define MAX_CALLBACKS_BM 1

//! this status will be called and let the ALIVE re-evaluate
void statusM5ButtonModule();

//! called by the feed operation to say the device is still running.. and count it as a button click.
//void refreshDelayButtonTouched_ButtonModule();

//!This is only thing exposed to others.. (Kinda which only 1 button module)
//!short press on buttonA (top button)
void buttonA_ShortPress_M5ButtonModule();
//!long press on buttonA (top button)
void buttonA_LongPress_M5ButtonModule();
//!the long press of the side button
void buttonB_LongPress_M5ButtonModule();
//!the short press of the side button
void buttonB_ShortPress_M5ButtonModule();

//! 2.21.25 add a way to change the button color (if any)
void changeButtonColor_M5ButtonModule();

#endif

#endif //ButtonModule_h
