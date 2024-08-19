#ifndef DisplayModule_h
#define DisplayModule_h

//!for types: String, boolean ..
#include "../../Defines.h"

/*! M5 Display with SemanticMarker&trade;
* \image html SMStatus2.PNG "SemanticMarker&trade;"
*/


//!THIS IS the setup() and loop() but using the "component" name, eg MQTTNetworking()
//!This will perform preference initializtion as well
//! called from the setup()
void setup_displayModule();

//! called for the loop() of this plugin
void loop_displayModule();

//! shows a FEED (or whatever) then blanks the screen after N seconds
//! NOTE: This will be a scrolling text as sometime ..
void showText_displayModule(String text);

//! adds messages that aren't shown unless in message window mode.
//! NOTE: This will be a scrolling text as sometime ..
void addToTextMessages_displayModule(String text);

//!blanks the screen
void blankScreen_displayModule();

//!wakes up the screen
void wakeupScreen_displayModule();

//!whether screen is blank (so the button can be a wake, vs action..)
boolean isBlankScreen_displayModule();

//!clears the screen
void clearScreen_displayModule();

//!displays the Semantic Marker (a super QR code) on the M5 screen (title = to display)
void showSemanticMarker_displayModule(String semanticMarkerAddress, String title);

//!returns the current semantic marker (eg. guest page)
const char* currentSemanticMarkerAddress_displayModule();

//!the function to call to get the 'status', return char*
//char* (*getStatusFunc)(void)
//typedef String (*getStatusFunc)(void);

//!displays the Semantic Marker (a super QR code) on the M5 screen (title = to display)
void showSemanticMarkerFunc_displayModule(String semanticMarkerAddressBase, String title, const char* (*getStatusFunc)(void));

#define START_NEW true
#define KEEP_SAME false
//!redraws the Semantic Marker image..
void redrawSemanticMarker_displayModule(boolean startNew);

//!returns the loop timer (just a timer..)
int getLoopTimer_displayModule();

//!reset the loop timer .. useful for testing,
void resetLoopTimer_displayModule();

//!Increment the screen color 0..n cache for getting the screen color 0..max (max provided by sender)
//!This is implemented by incrementScreenColor_mainModule() since it knows the MAX value of colors
void incrementScreenColor_displayModule();

//!cache for getting the screen color 0..n. Will reset the cache as well
void setScreenColor_displayModule(int screenColor);

//!show an OTA message..
void showOTAUpdatingMessage();

#ifdef M5CORE2_MODULE

/**
 First attempt at using the M5Core2 display to show something other than the M5 display
 1. scrolling text
 */
//! 1.24.24 Adding scrolling text (if that window on the M5Core2 is shown)
void addToScrollingText_displayModule(String textString);
//! 1.24.24 Goto the scrolling text mode ..
void toggleShowingScrollingTextMode_displayModule();
//! 1.24.24 Goto the scrolling text mode .. if flag  -- saves value
void setShowingScrollingTextMode_displayModule(boolean flag);
//! 2.27.24 make the button seem to be touched
//void showButtonTouched_displayModule();
//! 2.29.24 Leap Year day
//! Returns whether the Scrolling Text window is shown
//! 1.24.24 as alternate display windows are introducted,
//! let others know and don't overright stuff..
boolean inAlternateDisplay_displayModule();

//! add a loop for the alternate display
void loop_Alternate_displayModule();

//! scrolls the displayed text
void scrollText_displayModule();

//! 4.3.24 show text in big letters
//!@see https://github.com/konacurrents/ESP_IOT/issues/323
//!show full screen message..
void displayFullscreenMessage(String text, boolean turnOn);

#endif

#endif // DisplayModule
