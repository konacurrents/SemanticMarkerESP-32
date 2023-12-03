//
//  Defines.h
//
//  Created by Scott Moody on 3/8/22.
//


#ifndef Defines_h
#define Defines_h

//turn on or off which configuration is desired..  
//BOARD = M5Stick-C-Plus,  type : make uploadFlashM5TEST
#define ESP_M5_SMART_CLICKER_CONFIGURATION

//original feeder: (and any without PetTutor board. Otherwise Blue light doesn't flash)
//BOARD = ESP32 Dev Modulef   type: make uploadFlashOrigTEST
//#define ESP_32_FEEDER

//When testing the new board, define _WITH_BOARD  -- make uploadFlashBoardTEST
//#define ESP_32_FEEDER_WITH_BOARD

//original feeder: (or any) that also pairs with a GEN3 only, and gateways messages.
//#define ESP_32_FEEDER_BLE_GEN3

//turn on or off which configuration is desired..  
//BOARD = M5Stick-C-Plus,  type : make uploadFlashM5TEST
//#define ESP_M5_ATOM_LITE_QR_SCANNER_CONFIGURATION

// an ESP M5 with a camera 8.11.22 (same as M5 without a display or buttons, but with a camera)
// Use the M5Stack-Timer-CAM library
// Use the Minimal SPIFF (1.9MB app, with OTA 190KB SPIFF), uses 57% so far (now: 61%)
//#define ESP_M5_CAMERA

//TRY this for close to minimal BLEClient (no WIFI, no BLEServerr)
//#define ESP_M5_SMART_CLICKER_CONFIGURATION_MINIMAL

//if the ESP_32 were almost all functions of clicker (without UI)
//#define ESP_32_SMART_CLICKER_CONFIGURATION  (NOTE: NOT compiling right now..)


//!global to all releases (for now) used in Status for M5 (only 2 characters)
#define VERSION_SHORT "v5"

//!used to force using the BLE Device name in SERVER( eg. PTFEEDER:ScoobyDoo)
//NOT NOW 9.14.22 (no)
//NOW: make default for v12.a on...  9.20.22
//OFF again on 1.6.23 - managed by preference: 
//#define FORCE_USE_BLE_SERVER_DEVICE_NAME

//! 9.29.22 support wildcard naming via TokenParser.h
#define WILDCARD_DEVICE_NAME_SUPPORT

//! 7.1.23 try process SMARTButton JSON
//! 9.27.23  NOT WORKING .. parsing the JSON isn't working..
//! This was for a stored procedure.
//#define PROCESS_SMART_BUTTON_JSON

// *** Start of Configurations ***
#define M5STACK_VERSION "2.0.4"
//#define M5STACK_VERSION "2.0.7"  //9.24.23  (didn't seem to have heap space to start BLE server)

//M5stack 2.0.5-1.0 (Nov 2022)s
//This is an easy way to turn on the smart clicker configuration...
#ifdef ESP_M5_SMART_CLICKER_CONFIGURATION
#define VERSION "Version-(2.16v)-10.13.2023-ESP_M5_SMART_CLICKER_BLE_GROUPS_SET64_DEVONLY_SM"
#define ESP_M5
#define USE_MQTT_NETWORKING
#define USE_BUTTON_MODULE
#define USE_WIFI_AP_MODULE
#define USE_BLE_CLIENT_NETWORKING
#define USE_BLE_SERVER_NETWORKING
#define USE_DISPLAY_MODULE

//#define USE_SPIFF_MODULE
//#define USE_AUDIO_MODULE

//The M5_CAPTURE_SCREEN is an attempt to get the image of the M5. not working yet.. 8.25.22
// This issue is the backing store of the bitmap isn't there, so grabbing the pixels are blank
//#define M5_CAPTURE_SCREEN
#endif //SMART_CLICKER_CONFIGURATION_M5


//! This is an ESP_M5 camera (such as the M5 Timer X. 8.11.22
//! Select  M5Stack-Timer-CAM, Minimal SPIFFS (1.9mB APP with OTA/190KB Spiffs) 240MH WiFi/BT
#ifdef ESP_M5_CAMERA
#define VERSION "Version-(2.13)-11.21.2022-ESP_M5_CAMERA_WEBPAGE_Timer"
#define ESP_M5
#define USE_MQTT_NETWORKING
#define USE_WIFI_AP_MODULE
#define USE_BLE_SERVER_NETWORKING
//add client too .. but turn off.. can act like gateway..
#define USE_BLE_CLIENT_NETWORKING
#define USE_SPIFF_MODULE
#define USE_CAMERA_MODULE
#endif

//This is an easy way to turn on the smart clicker configuration...
#ifdef ESP_M5_SMART_CLICKER_CONFIGURATION_MINIMAL
#define VERSION "Version-(v1.3)-5.9.2022-ESP_M5_SMART_CLICKER-minimal"
#define ESP_M5
#define USE_BUTTON_MODULE
#define USE_BLE_CLIENT_NETWORKING
#define USE_DISPLAY_MODULE
#endif //ESP_M5_SMART_CLICKER_CONFIGURATION_MINIMAL

#ifdef ESP_32_FEEDER_WITH_BOARD
#define ESP_32_FEEDER // inherit that below
#define BOARD
#endif

//This is an easy way to turn on the esp-32 feeder configuration...
#ifdef ESP_32_FEEDER
#if defined(BOARD)
#ifdef  ESP_32_FEEDER_BLE_GEN3
#define VERSION "Version-(2.15b)-7.16.2023-ESP_32_FEEDER_GROUPS2"
#else
//!normal BOARD - or the main feeders !! MODIFY THIS ONE...
#define VERSION "Version-(2.16v)-10.13.2023-ESP_32_FEEDER_GROUPS2_WIFI_AP"
#endif

#else // orig
#ifdef  ESP_32_FEEDER_BLE_GEN3
#define VERSION "Version-(v2.12g)-10.23.2022-ESP_32_FEEDER-BLE_GEN3"
#else
//THIS IS WHERE VERSION FOR ORIGINAL ESP feeder (without board)  12.31.22 
#define VERSION "Version-(2.16v)-10.13.2023-ESP_32_FEEDER_GROUPS2_WIFI_AP"


#endif
#endif //BOARD
#define ESP_32

#define USE_MQTT_NETWORKING
#define USE_BLE_SERVER_NETWORKING
#define USE_BUTTON_MODULE
#define USE_WIFI_AP_MODULE
#define USE_STEPPER_MODULE
#define USE_UI_MODULE
//#define USE_SPIFF_MODULE

#ifdef ESP_32_FEEDER_BLE_GEN3
//! This is a feeder that gateways to a GEN3
// NOT NOW.. #define USE_BLE_CLIENT_NETWORKING
#endif

#endif //ESP_32_FEEDER

//This is an easy way to turn on the smart clicker configuration...
//This is a version that is a smart clicker, and the right button of ESP32 is the "button"
#ifdef ESP_32_SMART_CLICKER_CONFIGURATION
#define VERSION "Version-2.6-4.23.2022-ESP_32_SMART_CLICKER"
#define ESP_32
#define USE_MQTT_NETWORKING
#define USE_BUTTON_MODULE
#define USE_WIFI_AP_MODULE
#define USE_BLE_CLIENT_NETWORKING
#define USE_BLE_SERVER_NETWORKING
#endif //ESP_32_SMART_CLICKER_CONFIGURATION

//This is an easy way to turn on the smart clicker configuration...
#ifdef ESP_M5_SERVER
#define VERSION "Version-2.5-4.23.2022-ESP_M5_SERVER"
#define ESP_M5
#define USE_MQTT_NETWORKING
#define USE_BLE_SERVER_NETWORKING
#define USE_BUTTON_MODULE
#define USE_WIFI_AP_MODULE
#endif //ESP_M5_SERVER


//This is an easy way to turn on the smart clicker configuration...
//! https://docs.m5stack.com/en/atom/atomic_qr
//! library:  M5Stack-ATOM (actually using M5Stack-C-Plus)
#ifdef ESP_M5_ATOM_LITE_QR_SCANNER_CONFIGURATION
#define VERSION "Version-1.9-9.27.23.2023-ESP_M5_ATOM_LITE_QR_SCANNER_AP_OTA_SM"
#define ESP_M5
#define USE_MQTT_NETWORKING
//#define USE_BUTTON_MODULE  ... ATOMQButtons.cpp
#define USE_WIFI_AP_MODULE
#define USE_BLE_SERVER_NETWORKING
#define USE_BLE_CLIENT_NETWORKING //try to be a smart clicker too..
#define ATOM_QRCODE_MODULE
//#define USE_DISPLAY_MODULE  .. no display

#endif //ESP_M5_ATOM_LITE_QR_SCANNER_CONFIGURATION

#if !defined(ESP_M5) && !defined(ESP_32)
Error either ESP_M5 or ESP_32 must be defined
#endif
#if defined(ESP_M5) && defined(ESP_32)
Error either ESP_M5 or ESP_32 must be defined but not both
#endif

//https://forum.arduino.cc/t/single-line-define-to-disable-code/636044/4
// Turn on/off Serial printing being included in the executable
//NOTE: if multiple lines of code say are creating a string to print, wrap that
//code with #if (SERIAL_DEBUG_INFO) - or whatever level of printing
//Use:  SerialError.print...
#define SERIAL_DEBUG_ERROR true
#define SerialError if (SERIAL_DEBUG_ERROR) Serial
#define SERIAL_DEBUG_LOTS false
#define SerialLots  if (SERIAL_DEBUG_LOTS) Serial
#define SERIAL_DEBUG_DEBUG true
#define SerialDebug  if (SERIAL_DEBUG_DEBUG) Serial
#define SERIAL_DEBUG_INFO false
#define SerialInfo  if (SERIAL_DEBUG_INFO) Serial
#define SERIAL_DEBUG_MINIMAL true
#define SerialMin  if (SERIAL_DEBUG_MINIMAL) Serial
// a temporary debug.. without having to set the above..
#define SERIAL_DEBUG_TEMP true
#define SerialTemp  if (SERIAL_DEBUG_TEMP) Serial
//turn on CALLS to see the methods called in order, etc
#define SERIAL_DEBUG_CALL false
#define SerialCall  if (SERIAL_DEBUG_CALL) Serial
// *** REST ARE NORMAL INCLUDES based on these #defines ****

//!name of service to call (which will always be PTFeeder)
#define MAIN_BLE_CLIENT_SERVICE (char*)"PTFeeder"
//!name of service to call (which will always be PTClicker)
#define M5_BLE_CLIENT_SERVICE (char*)"PTClicker"

//!Defines the name of the service of the server, which for M5 will be PTClicker
#ifdef ESP_M5
#define MAIN_BLE_SERVER_SERVICE_NAME (char*)"PTClicker"
#else
#define MAIN_BLE_SERVER_SERVICE_NAME (char*)"PTFeeder"
#endif
#define NOTSET_STRING (char*)"notset"

//!This is to debug the BLE ACK happening or not happening. CURRENT is the correct one, but the OLD can be tested
//! since OLD discovers the GEN3 (or actually ESP32) feeders from the _ESP_32 string being returned..
//! Code is in BLEServer.cpp
#define CURRENT_BLE_ACK_APPROACH
//#define OLD_BLE_NO_ACK_APPROACH

#ifdef ESP_M5
#include <M5StickCPlus.h>
#else
#include <Arduino.h>
#endif


//Turn this on 1st time.. and testing..  AND if something drastic causes it to reboot always
//#define BOOTSTRAP
//#define BOOTSTRAP_AP_MODE_STARTUP
//! set bootstrap_clean one time, then turn it off.. Sometimes get's it out of error funk.
//#define BOOTSTRAP_CLEAN

#ifdef USE_MQTT_NETWORKING
#include "src/MQTTModule/MQTTNetworking.h"
#endif
#ifdef USE_BLE_SERVER_NETWORKING
#include "src/BLEServerModule/BLEServerNetworking.h"
#endif
#ifdef USE_BLE_CLIENT_NETWORKING
#include "src/BLEClientModule/BLEClientNetworking.h"
#endif
#ifdef USE_BUTTON_MODULE
#include "src/ButtonModule/ButtonModule.h"
#endif
#ifdef USE_STEPPER_MODULE
#include "src/StepperModule/StepperModule.h"
#endif
#ifdef USE_UI_MODULE
#include "src/UIModule/UI.h"
#endif
#ifdef USE_WIFI_AP_MODULE
#include "src/WIFI_APModule/WIFI_APModule.h"
#endif
#ifdef USE_CAMERA_MODULE
#include "src/CameraModule/CameraModule.h"
#endif
#ifdef USE_AUDIO_MODULE
#include "src/AudioModule/AudioModule.h"
#endif

#ifdef ATOM_QRCODE_MODULE
#include "src/ATOM_QRCode_Module/ATOMQRCodeModule.h"
#endif
//! The following are ALWAYS included as the "Main" functionality

//! add the MainModule.h so the defines for preferences are known
#include "src/MainModule/MainModule.h"

//! DisplayModule is always included (it has dummy stubs for non Display ESP's)
#include "src/DisplayModule/DisplayModule.h"

//! SPIFFModule is always included
#include "src/MainModule/SPIFFModule.h"
/*
 Code naming conventions, some to help not have linker duplicate symbols. Note: some of these issues of duplicate symbols is we are "C" without any encapsulation like an Object Oriented language would provide. 
   1. functions (methods) start with lower case,
      to help find the function, prefix or postfix with the module name
      eg. 
   2. local variables start with lower case
   3. global variables start with _  (eg. int _globalVariable)
    //note to prevent linker clashes, common names (like _preferences) should be
     augmented with the module prefix: eg.  _preferencesMQTTNetworking;
 
   4. constants should be all upper case (eg. #define BLINK 3)
   5. No global variables in ".h" - and if needed, wrap that in a function() call
         eg.  getFeederType()
 
 Modules (eg. UI_MODULE, WIFI_APModule ..)
   1. enclose in #ifdef (#ifdef UI_MODULE) and "defines.h" sets them for a build (and maybe not for M5)
   2. Each "Module", hould have a single ".h" that gets included elsewhere.
         eg.  WIFI_APModule.h
   3. Plugin methods:
          loop_<module>()
          setup_<module_()
          clearEPROM_<module>()
          //optionally:
          preSetup_<module>()
           // and some more complicated module specific calls.

 Defines:
   wrap with the #ifndef -- which is the same as #pragma once , etc

 Note: https://www.tweaking4all.com/forum/arduino/arduino-ide-change-the-color-of-the-error-messages/
 /Applications/Arduino.app/Contents/Java/lib/theme

# GUI - CONSOLE
console.font = Monospaced,plain,11
console.font.macosx = Monaco,plain,10
console.color = #000000
console.output.color = #eeeeee
console.error.color = #E34C00

 #ifndef ButtonModule_h
 #define ButtonModule_h
 ....
 #endif //ButtonModule_h
 */

 #endif // Defines_h
