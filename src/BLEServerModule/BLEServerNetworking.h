//
//  BLENetworking.hpp
//  M5Stick
//
//  Created by Scott Moody on 1/19/22.
//

#ifndef BLEServerNetworking_h
#define BLEServerNetworking_h

#include "../../Defines.h"

//!defines the operations on BLE Server Networking
//!BLEServerNetworking is the "server" side of the BLE interface

//!the 'setup' for this module BLEServerNetworking. Here the service name is added (and potentially more later)
void setup_BLEServerNetworking(char *serviceName, char * deviceName, char *serviceUUID, char *characteristicUUID);

//#define SERVICE_UUID    PT_SERVICE_UUID  //Pet Tutor feeder service for feed
//#define CHARACTERISTIC_UUID PT_CHARACTERISTIC_UUID //Pet Tutor feeder characteristic

//!the 'setup' for this module BLEServerNetworking. Here the service name is added (and potentially more later)
void loop_BLEServerNetworking();

#define BLE_SERVER_CALLBACK_ONREAD 0
#define BLE_SERVER_CALLBACK_ONWRITE 1
//!used to send a string message back (which might be sent to MQTT for example)
#define BLE_SERVER_CALLBACK_STATUS_MESSAGE 2
#define BLE_SERVER_MAX_CALLBACKS 3

#ifdef NOT_HERE_USE_MAIN
//! 10.13.25 Nice dayl . New snow in hills. Mt looking nice
//! don't store service name here .. 
//!sets the device name
void setBLEServerDeviceName(char *deviceName);
#endif //! NOT HERE

//! retrieve the service name (PTFEEDER, PTFeeder:Name, PTClicker:Name, etc)
char *getServiceName_BLEServerNetworking();
       //!
//!send something over bluetooth, this right now is 0x01
void sendBLEMessageACKMessage();

#endif /* BLEServerNetworking_hpp */
