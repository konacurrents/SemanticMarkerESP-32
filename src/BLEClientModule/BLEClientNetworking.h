//
//  BLEClientNetworking.hpp
//  M5Stick
//
//  Created by Scott Moody on 1/20/22.
//

#ifndef BLEClientNetworking_h
#define BLEClientNetworking_h
#include "../../Defines.h"

//! https://esp32.com/viewtopic.php?t=2291
/*!< Scan interval. This is defined as the time interval from
 when the Controller started its last LE scan until it begins the subsequent LE scan.
 Range: 0x0004 to 0x4000 Default: 0x0010 (10 ms)
 Time = N * 0.625 msec
 Time Range: 2.5 msec to 10.24 seconds*/
/** Set scan interval (how often) and window (how long) in milliseconds */

/*!< Scan window. The duration of the LE scan. LE_Scan_Window
 shall be less than or equal to LE_Scan_Interval
 Range: 0x0004 to 0x4000 Default: 0x0010 (10 ms)
 Time = N * 0.625 msec
 Time Range: 2.5 msec to 10240 msec */
#define PSCAN_INTERVAL 45
#define PSCAN_WINDOW   15
#define PSCAN_TIME   0  /* scan forever*/

//from example.. 
//#define PSCAN_INTERVAL 1349
//#define PSCAN_WINDOW   449


//#include <stdio.h>

//!These are the legacy PetTutor known addresses..
//#define SERVICE_UUID      PT_SERVICE_UUID   //Pet Tutor feeder service for feed
//#define CHARACTERISTIC_UUID PT_CHARACTERISTIC_UUID   //Pet Tutor feeder characteristic


#define BLE_CLIENT_CALLBACK_ONREAD 0
#define BLE_CLIENT_CALLBACK_ONWRITE 1
//!the BLE code wants to blink the light
#define BLE_CLIENT_CALLBACK_BLINK_LIGHT 2
//!used to send a string message back (which might be sent to MQTT for example)
#define BLE_CLIENT_CALLBACK_STATUS_MESSAGE 3
#define BLE_CLIENT_MAX_CALLBACKS 4


//!the 'setup' for this module BLEClientNetworking. Here the service name is added (and potentially more later)
void setup_BLEClientNetworking(char *serviceName, char *serviceUUID, char *characteristicUUID);

//!the loop()
void loop_BLEClientNetworking();

//!FOR NOW THIS IS HERE.. but it should be more generic. eg:  sendBluetoothCommand() ..
//!send the feed command
void sendFeedCommandBLEClient();

//!FOR NOW THIS IS HERE.. but it should be more generic. eg:  sendBluetoothCommand() ..
//!send the feed command
void sendCommandBLEClient(String cmdString);

//!send a string of 13 or less characters.
void sendCommandBLEClient_13orLess(String cmdString);

//! returns whether connected over BLE as a client to a server(like a ESP feeder)
boolean isConnectedBLEClient();

//! returns if the BLEClient is turned on.. note, if connected to a BLE device, then disconnect
boolean useBLEClient();

//! try to disconnect..
void disconnect_BLEClientNetworking();

//! an address or name to skip (for at least 1 time)
void skipNameOrAddress_BLEClientNetworking(char *nameOrAddress);

#endif /* BLEClientNetworking_h */
