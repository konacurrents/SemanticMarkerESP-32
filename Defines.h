//
//  Defines.h
//
//  Created by Scott Moody on 3/8/22.
/*
  //emoteMe {M5QRScanner} {#connectedMe} {I,F}  {'T':'1739574713','dev':'M5QRScanner','user':'scott@konacurrents.com','location':'buckley,wa','ble':'PTClicker:M5QRScanner','v':'Version-(3.11)-4.6.24-ES','chipid':'8593020','ssid':'SunnyWhiteriver', 'smscanner':'on'}
  Publish message(usersP/groups/atlasDogs): #remoteMe {M5QRScanner} {#connectedMe} {I,F}  {'T':'1739574713','dev':'M5QRScanner','user':'scott@konacurrents.com','location':'buckley,wa','ble':'PTClicker:M5QRScanner','v':'Version-(3.11)-4.6.24-ES','chipid':'8593020','ssid':'SunnyWhiteriver', 'smscanner':'on'}
  ASYNC_SEND_MQTT_STATUS_URL: status?usev=v6&dev=M5QRScanner&b=100&temp=00&c=0&t=0&smscanner=on&W=on&M=on&B=off&C=off&A=off&T=off&S=on&bleS=PTClicker:M5QRScanner&Z=on&G=off
  Publish message(usersP/bark/scott@konacurrents.com): #remoteMe {M5QRScan
*/
//! 11.18.24
//! 2.14.25 Valentines day  (M5Atom was:
//! 3.22.25 adding ScannedSemanticMarker message - to replay on button click

#ifndef Defines_h
#define Defines_h

//! *******************************
//! *** 7 main builds numbered below ****
//! 1. M5 Smart Clicker (Board:
//#define ESP_M5_SMART_CLICKER_CONFIGURATION
//
//! 2. ESP32 Feeder(orig old),
//#define ESP_32_FEEDER
//
//! 3. ESP32Feeder Board (Board: ESP-32 dev module, partition: default 4mb withspiffs(1.2MB app/1.5MB SPIFFS)
//! NOTE: we should change this to Minimal spiffs (1.9MB App with OTA.190KB SPIFFS) -- but all deployed feeders couldn't OTA
//#define ESP_32_FEEDER_WITH_BOARD
//#define ESP_32_FEEDER_BLE_GEN3
//
//! 4. M5 Atom  This includes QR and Socket
#define ESP_M5_ATOM_LITE

//
//! 5. M5 Camera
//#define ESP_M5_CAMERA
//
//! 6. M5Core2
//#define M5CORE2_MODULE
//!
//! 7 ESP_M5_ATOM_S3  (for the M5AtomS3 with a display)
//!       Board: M5AtomS3 library Minimal spiffs (1.9MB App with OTA.190KB SPIFFS)
//#define ESP_M5_ATOM_S3
#ifdef ESP_M5_ATOM_S3
#define ESP_M5_ATOM_LITE
//#define ESP_M5_MINIMAL_SENSORS
#endif
//
//! *******************************
//! *** CURRENTLY: 4
//! ****************************************

//turn on or off which configuration is desired..
//BOARD = M5Stick-C-Plus,  type : make uploadFlashM5TEST
// *** 1
//#define ESP_M5_SMART_CLICKER_CONFIGURATION
//! comment out the following to use the SENSORS (like KeyUnitSensor)
//#define ESP_M5_MINIMAL_SENSORS

//original feeder: (and any without PetTutor board. Otherwise Blue light doesn't flash)
//BOARD = ESP32 Dev Modulef   type: make uploadFlashOrigTEST
// *** 2
//#define ESP_32_FEEDER

//When testing the new board, define _WITH_BOARD  -- make uploadFlashBoardTEST
// *** 3
//#define ESP_32_FEEDER_WITH_BOARD

//original feeder: (or any) that also pairs with a GEN3 only, and gateways messages.
// *** 3b
//#define ESP_32_FEEDER_BLE_GEN3

//turn on or off which configuration is desired..
// 12.26.23 added for the power socket ATOM
//BOARD = M5Stick-C-Plus,  type : make uploadFlashM5AtomLiteTEST
// *** 4
//#define ESP_M5_ATOM_LITE


// an ESP M5 with a camera 8.11.22 (same as M5 without a display or buttons, but with a camera)
// Use the M5Stack-Timer-CAM library
// Use the Minimal SPIFF (1.9MB app, with OTA 190KB SPIFF), uses 57% so far (now: 61%)
// *** 5
//#define ESP_M5_CAMERA

//! 1.6.24 M5Core2Module
//! library M5Core2
//! minimal SPIFFS (1.9MB APP, with OTA/190KB SPIFFS)
//! https://docs.m5stack.com/en/api/core2/lcd_api
// *** 6
//#define M5CORE2_MODULE

//TRY this for close to minimal BLEClient (no WIFI, no BLEServerr)
//#define ESP_M5_SMART_CLICKER_CONFIGURATION_MINIMAL

//if the ESP_32 were almost all functions of clicker (without UI)
//#define ESP_32_SMART_CLICKER_CONFIGURATION  (NOTE: NOT compiling right now..)


//!global to all releases (for now) used in Status for M5 (only 2 characters)
#define VERSION_SHORT "v8"

//!used to force using the BLE Device name in SERVER( eg. PTFEEDER:ScoobyDoo)
//NOT NOW 9.14.22 (no)
//NOW: make default for v12.a on...  9.20.22
//OFF again on 1.6.23 - managed by preference:
//#define FORCE_USE_BLE_SERVER_DEVICE_NAMEa

//! 9.29.22 support wildcard naming via TokenParser.h
#define WILDCARD_DEVICE_NAME_SUPPORT

//! 7.1.23 try process SMARTButton JSON
//! 9.27.23  NOT WORKING .. parsing the JSON isn't working..
//! This was for a stored procedure.
//! NOTE: going to try a call to node-red via https (USE_REST_MESSAGING) and have it run the stored
//! procedure with the arguments we send
//#define PROCESS_SMART_BUTTON_JSON


// *** Start of Configurations ***
#define M5STACK_VERSION "2.0.4"
//#define DHAT not working.. with 2.1.0
//#define M5STACK_VERSION "2.0.7"  //9.24.23  (didn't seem to have heap space to start BLE server)
// 2.1 is too BIG .. 2.22.24   (ALSO: issue OTA of too big an app...)
//#define M5STACK_VERSION "2.1.0"

//! 4.20.24  using library M5GFX 0.1.15 (was 0.0.20) for the M5StickCPlus2

/* *************************** CONFIGURATIONS **********************************/
//  ******************************* M5 ****************************************** 

//! ****** MAIN:  M5 ****
//M5stack 2.0.5-1.0 (Nov 2022)hula2127
//This is an easy way to turn on the smart clicker configuration...
#ifdef ESP_M5_SMART_CLICKER_CONFIGURATION
//#define VERSION "V(2.17b)-11.29.2023-ESP_M5_SMART_CLICKER_BLE_GROUPS_SM_RAND"
//! 2.5.24 (scotty birthday)
#define VERSION "V(3.7)-4.2.24-ESP_M5_SMART_CLICKER_KEY_SONIC_CLASS_30K_CHIPID_AUDIO"
//#define VERSION "Version-(3.6a)-2.22.24-BOOTSTRAP"

#define ESP_M5
#define USE_MQTT_NETWORKING
#define USE_BUTTON_MODULE
#define USE_WIFI_AP_MODULE
#define USE_BLE_CLIENT_NETWORKING
#define USE_BLE_SERVER_NETWORKING
#define USE_DISPLAY_MODULE
#define M5BUTTON_MODULE

//! 4.20.24 Beau Birthday, 4.20, etc
//! trying the M5StickCPlus2 (which uses M5.unified) YELLOW version (not red)
#define M5STICKCPLUS2
#ifdef  M5STICKCPLUS2
#define ESP_M5_MINIMAL_SENSORS
#else
#define USE_SPIFF_MODULE
#endif
#define USE_SPIFF_MQTT_SETTING
#define USE_SPIFF_QRATOM_SETTING
//!@see https://github.com/konacurrents/ESP_IOT/issues/322
//#define USE_AUDIO_MODULE
//TODO: #define USE_FAST_LED

//! if not minimal sensors .. then include whatever is desired
#ifndef ESP_M5_MINIMAL_SENSORS

//! Sensors:  2.8.24 (30K above pacific)
#define KEY_UNIT_SENSOR_CLASS
#define USE_LED_BREATH

//! Ultrasonic-I2C
//! 2.23.24 after nice ski day, Da King beautiful weather (Laura along - not king)
//! NOTE: won't work at same time as KEY_UNIT (overlapping). So how to fix that?
//#define ULTRASONIC_I2C_SENSOR_CLASS //NOTE: this is taking time away from M5 button presses...

#endif //not ESP_M5_MINIMAL_SENSORS


//The M5_CAPTURE_SCREEN is an attempt to get the image of the M5. not working yet.. 8.25.22
// This issue is the backing store of the bitmap isn't there, so grabbing the pixels are blank
//#define M5_CAPTURE_SCREEN
#endif //SMART_CLICKER_CONFIGURATION_M5


//! This is an ESP_M5 camera (such as the M5 Timer X. 8.11.22
//! Select  M5Stack-Timer-CAM, Minimal SPIFFS (1.9mB APP with OTA/190KB Spiffs) 240MH WiFi/BT
#ifdef ESP_M5_CAMERA
//#define VERSION "Version-(2.13)-11.21.2022-ESP_M5_CAMERA_WEBPAGE_Timer"
#define VERSION "V(2.2e)-1.20.2024-ESP_M5_CAMERA_WEBPAGE_Timer_Groups_HTTPS_RTSP"
//! still getting xQueueGenericSend error on "status" and "feed" message
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
#define VERSION "V(v1.3)-5.9.2022-ESP_M5_SMART_CLICKER-minimal"
#define ESP_M5
#define USE_BUTTON_MODULE // for generic button processing (with a display)
//! 1.22.24 added M5ButtonModule by itself..
#define M5BUTTON_MODULE

#define USE_BLE_CLIENT_NETWORKING
#define USE_DISPLAY_MODULE


#endif //ESP_M5_SMART_CLICKER_CONFIGURATION_MINIMAL

#ifdef ESP_32_FEEDER_WITH_BOARD
#define ESP_32_FEEDER // inherit that below
#define BOARD
#endif

//This is an easy way to turn on the esp-32 feeder configuration...
#ifdef ESP_32_FEEDER

//! 3.28.24 -- feed when booted up (so HUBSpace can turn on/off feeder via TDSnap via Eye Gaze)
//! 5.3.24 but on
#if defined (BOARD)
#else
//! original feeder (like PumpkinUno) will feed on startup..
//! SEEMS this is only compiler flag for now.. Default OFF anyway
#define FEED_ON_STARTUP
#endif
//  ******************************* ESP32 ****************************************** 

// *************************** case 3 ESP32 with BOARD *********************************
#if defined(BOARD)
#ifdef  ESP_32_FEEDER_BLE_GEN3
#define VERSION "V(2.15b)-7.16.2023-ESP_32_FEEDER_GROUPS2"
#else
//!normal BOARD - or the main feeders !! MODIFY THIS ONE...  **********************
#define VERSION "2025_v3.27_V(2.19)-ESP_32_groups_StepperMotorClass"

//!  3.16.25 new CLASS for the two types so far..
//#define USE_DCMOTOR_STEPPER_CLASS
//#define USE_PT_STEPPER_CLASS
// use sensor mode: PTStepperClass or DCMotorStepperClass
// defaults to PTStepperCLass

#endif

#else // orig
#ifdef  ESP_32_FEEDER_BLE_GEN3
#define VERSION "V(v2.12g)-10.23.2022-ESP_32_FEEDER-BLE_GEN3"
#else
//THIS IS WHERE VERSION FOR ORIGINAL ESP feeder (without board)  12.31.22
//#define VERSION "Version-(2.16a)-6.14.2024-ESP_32_FEEDER_GROUPS3_WIFI_AP_STARTUP_FEED"
#define VERSION "V(2.16g)-8.9.2024-ESP_32_FEEDER_GROUPS3_WIFI_AP_STARTUP_FEED_JerryGarcia"


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
#define VERSION "V2.6-4.23.2022-ESP_32_SMART_CLICKER"
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

//  ******************************* M5ATOM ****************************************** 

//!NOTE: M5_ATOM is define for the subset of ESP_M5 that is an ATOM
//! This turns on features like the LED display

//! m5Atom kind supported


//! 1.5.24 NOW the M5AtomLite is all the ATOM's but configured to 1 at a time
//This is an easy way to turn on the smart clicker configuration...
//! https://docs.m5stack.com/en/atom/atomic_qr
//! library:  not using M5Stack-ATOM (actually using M5Stick-C-Plus)
//! 12.26.23 day after xmas
//! https://docs.m5stack.com/en/atom/atom_socket
//! ALSO: use the Minimal SPIFF, 1.9m app, OTA and 190KB spiff
//! 2.25.23 Sketch uses 1497093 bytes (76%) of program storage space. Maximum is 1966080 bytes.
//! 8.22.24 (seems to generate m5stack_stickc_plus.bin)   .. and size now 65%
#ifdef ESP_M5_ATOM_LITE
#ifdef ESP_M5_ATOM_S3
#define VERSION "2025d_4.4V(1.0a)-8.30.24-M5_ATOM_S3"
#else
//! ***** THIS IS MAIN "ATOM" ***** 3.29.25
#define VERSION "2025d_4.14-25(4.6c)-ATOM_MOTOR_SERVo_QR_SCAN_DOCFOLLOW_SOCKET_SCANNED_SM_BUTTON"
#endif //ESP_M5_ATOM_S3

#define ESP_M5
#define M5_ATOM
#define USE_MQTT_NETWORKING
//#define USE_BUTTON_MODULE  ... ATOMQButtons.cpp
#define USE_WIFI_AP_MODULE
#define USE_BLE_SERVER_NETWORKING
#define USE_BLE_CLIENT_NETWORKING //try to be a smart clicker too..
//#define USE_DISPLAY_MODULE  .. no display

#define USE_STEPPER_MODULE
#define USE_UI_MODULE

//! which ATOM plugs MODULEs are included in build:
#define ATOM_QRCODE_MODULE
//! including ATOM_SOCKET_MODULE now ...
#define ATOM_SOCKET_MODULE
//#else

//! the ATOM uses FAST_LED (the light on top button, but not M5AtomS3)
#define USE_FAST_LED

//! Sensors:  2.8.24 (30K above pacific)
#define KEY_UNIT_SENSOR_CLASS
#define USE_LED_BREATH
//! 11.14.23 try "https" secure web call (to SemanticMarker.org/bot/...)
//!  See https://GitHub.com/konacurrents/SemanticMarkerAPI for more info
//crashing .. 3.22.24 (just send DOCFOLLOW for now)
//#define USE_REST_MESSAGING
//! 3.25.24 try again ... (not working)


//! 4.4.24 (after glacier skiing at Crystal ice, but snowing) -- year ago now 4.4.25 (yesterday good. Today would have been amazing views)
#define USE_SPIFF_MODULE
#define USE_SPIFF_MQTT_SETTING
#define USE_SPIFF_QRATOM_SETTING


//! 4.10.24 GPS sensor
//#define USE_GPS_SENSOR_CLASS

//#endif // not M5AtomS3

#endif //ESP_M5_ATOM_LITE

//  ******************************* M5CORE2 ****************************************** 
//! 1.22.24 M5CORE2 uses M5Core2 Library (not M5StackC-Plus)
#ifdef M5CORE2_MODULE
#define VERSION "2025v4.1.25-V(3.8a)-4.5.24-ESP_M5CORE2_OurM5Dsp_Touch_30KPacific_KeyUnit_ClickAudio_SPIFF"
//#define VERSION "Version-(3.6)-3.17.24-ESP_M5CORE2_OurM5Dsp_Touch_30KPacific_KeyUnit_ChipID"
#define ESP_M5
#define USE_MQTT_NETWORKING
#define USE_BUTTON_MODULE  ///... ATOMQButtons.cpp
#define USE_WIFI_AP_MODULE
#define USE_BLE_SERVER_NETWORKING
#define USE_BLE_CLIENT_NETWORKING //try to be a smart clicker too..
#define USE_DISPLAY_MODULE

//! 11.14.23 try "https" secure web call (to SemanticMarker.org/bot/...)
//!  See https://GitHub.com/konacurrents/SemanticMarkerAPI for more info
#define USE_REST_MESSAGING

//! from the ATOM code .. look at getting this working.
#define USE_FAST_LED

//! Sensors:  2.8.24 (30K above pacific)
#define KEY_UNIT_SENSOR_CLASS
#define USE_LED_BREATH

//! 4.2.24 try the audio .. but for the double click
//#define USE_AUDIO_MODULE

//! 4.4.24 (after glacier skiing at Crystal ice, but snowing)
#define USE_SPIFF_MODULE
#define USE_SPIFF_MQTT_SETTING
#define USE_SPIFF_QRATOM_SETTING

#endif //M5CORE2_MODULE



#if !defined(ESP_M5) && !defined(ESP_32) && !defined(M5_ATOM) && !defined(M5CORE2_MODULE)
Error either ESP_M5 or ESP_32 or M5_ATOM or M5CORE2_MODULE must be defined
#endif
#if defined(ESP_M5) && defined(ESP_32) && defined(M5_ATOM) && defined(M5CORE2_MODULE)
Error only one of ESP_M5 or ESP_32 or M5_ATOM or M5CORE2_MODULE must be defined
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
#ifdef M5CORE2_MODULE
#include <M5Core2.h>
#else
//fix this... 4.25.24
#ifdef  M5STICKCPLUS2
#include <M5StickCPlus2.h>
#else
//! 8.30.24 add M5AtomS3
#ifdef ESP_M5_ATOM_S3
#include <M5AtomS3.h>
#else
#include <M5StickCPlus.h>
#endif
#endif

//! see https://github.com/m5stack/M5Stack/issues/97
#undef min
#endif //M5Core2
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
#include "src/ButtonModule/ButtonProcessing.h"
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

//! 12.26.23
#ifdef ATOM_SOCKET_MODULE
#include "src/ATOM_SocketModule/ATOM_SocketModule.h"
#endif


//! 1.6.24
#ifdef M5CORE2_MODULE
#include "src/M5Core2Module/M5Core2Module.h"
#endif


//! 1.22.24 refactored it out to just M5
#ifdef M5BUTTON_MODULE
#include "src/M5Module/M5ButtonModule.h"
#endif

//! try a call..
#ifdef USE_REST_MESSAGING
#include "src/MQTTModule/RESTMessaging.h"
#endif

//! The following are ALWAYS included as the "Main" functionality

//! add the MainModule.h so the defines for preferences are known
#include "src/MainModule/MainModule.h"

//! 1.22.24 refactored ButtonModule always avaialble, and it calls plugin modules
// MAYBE NOT ... #include "src/ButtonModule/ButtonProcessing.h"

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
