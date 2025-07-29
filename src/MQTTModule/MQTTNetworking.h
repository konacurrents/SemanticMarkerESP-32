/** The MQTT + WIFI part
 
 
 Created: on Jan 1, 2022, iDogWatch.com
 Author: Scott Moody
 
 */
//#include <string>


#ifndef MQTTNetworking_h
#define MQTTNetworking_h

#include "../../Defines.h"
#ifdef USE_MQTT_NETWORKING

#include "OTAImageUpdate.h"
//does this make those .h available?

/*******************************MQTT*************************************/

//!callback with the message if required (like sending the FEED message)
//!!function pointers: https://www.cprogramming.com/tutorial/function-pointers.html
//!define as: void callback(char* message)
//!  call processMessage(message, &callback);
//! This must be called first before the setup or loop
//! eg:   void myCallback(char* message) { }
//!    setMessageCallback(&myCallback)
//void setMessageCallbacks(void (*callback)(char*), void (*blinkTheLED)());

#define MQTT_CALLBACK_FEED 0
#define MQTT_CALLBACK_BLINK 1
#define MQTT_CALLBACK_TEMP 2
//SOLID_LIGHT uses string parameter, "ON", "OFF"
#define MQTT_CALLBACK_SOLID_LIGHT 3
#define MQTT_CLEAN_SSID_EPROM 4
//NEW 4.1.22  (what to do if no MQTT??)
#define MQTT_CALLBACK_TILT_MOTION_DETECTED 5
#define MQTT_CALLBACK_PIR_DETECTED 6

//! MAX must be updated to 1 more than last item..
#define MQTT_MAX_CALLBACKS 7

//!THIS IS the setup() and loop() but using the "component" name, eg MQTTNetworking()
//! called from the setup()
void setup_MQTTNetworking();

//! called for the loop() of this plugin
void loop_MQTTNetworking();

//! NOTE: The following are only called from MainModule or ESP_IOT.ino (basically main)

//!called for things like the advertisement
char *getDeviceNameMQTT();

//!restart the WIFI and then MQTT connection
void restartWIFI_MQTTState();

//!process the JSON message, which can be configuration information. This is called from outside on things like a Bluetooth message..
//!return true if valid JSON, and false otherwise. This looks for '{'  as the starting character (after possible spaces in front). A topic can be sent, or nil
boolean processJSONMessageMQTT(char *ascii, char* topic);

//!process an MQTT message looking for keywords (barklet language) NOT exposed yet...
//! just send a message (let the internals to figure out topics, etc..
//!so the BLE can send something on the MQTT
//! for now only send if it start message starts with "#"
 void sendMessageMQTT(char *message);

//! for now only send if it start message starts with "#"
void sendMessageMQTT_Topic(char *message, char *topic);

//! just send a message but without any extras
void sendMessageNoChangeMQTT(char *message);

//! just send a message but without any extras, using a topic specified
void sendMessageNoChangeMQTT_Topic(char *message, char *topic);

//! Wrapper of the mqttclient publish
void publishMQTTMessage(char *topic, char *message);

//! Wrapper of the mqttclient publish
void publishMQTTMessageDefaultTopic(char *message);

//! publish a binary file..
//! fileExtension is .jpg, .json, .txt etc
void publishBinaryFile(char *topic, uint8_t * buf, size_t len, String fileExtension);


//! send semantic /smrun
//! 3.25.24 this is an HTTP not https
void publishSMRunMessage(char* smrunMessage);

//! sends the semantic marker as a doc follow message #remoteMe (vs STATUS, as that triggers a status reply.. )
void sendStatusMessageMQTT(const char *semanticMarker);

//! sends the semantic marker as a doc follow message #DOCFOLLOW
void sendDocFollowMessageMQTT(const char *semanticMarker);

//! retrieves the last DocFollow SemanticMarker (from the message #DOCFOLLOW | #followMe {AVM=<SM>}
//! Or the JSON:  {'set':'semanticMarker','val','<URL>}
char *getLastDocFollowSemanticMarker_MQTTNetworking();

//! sets the last DocFollow SemanticMarker 
void setLastDocFollowSemanticMarker_MQTTNetworking(char *semanticMarker);

//!cleans the eprom info
void cleanEPROM_MQTTNetworking();

//!value of WIFI connected
boolean isConnectedWIFI_MQTTState();
//!value of MQTT connected
boolean isConnectedMQTT_MQTTState();
//!get the chip info
void getChipInfo();

//!timed events 
//!starts the delay for WIFI  checking, called at startup, and each time the timer finished..
void restartDelayCheckWIFI_MQTTNetworking();
//!checks delay for the WIFI connectivity
void checkDelaySinceWIFICheck_MQTTNetworking();
//!checks if the WIFI is off (or not reachable) and tries consecting again (the 'W' command)
void checkAndReconnectWIFI_MQTTNetworking();

//! 9.16.23 some debug information
//! retrieve the Configuration JSON string in JSON format..
String getJSONConfigString();

//! retrieve the WIFIInfoString
String get_WIFIInfoString();

//! 9.18.23 LA (after Eagle Rock bike ride, Van Morrison tomorrow)
void cleanMQTTpasswordsUpdateInEPROM();

//!Decode the URL (exposed 12.17.23 for the scanner
String MQTT_urlDecode(String input);

//! 3.22.24 get the WIFI SSID for the status
String get_WIFI_SSID();

#ifdef USE_SPIFF_MODULE
//! publish a binary file..
//! fileExtension is .jpg, .json, .txt etc
void publishSPIFFFile_MQTT(char *topic, char *path, int len);
#endif


#else //PLACEHOLDERS

//!process the JSON message, which can be configuration information. This is called from outside on things like a Bluetooth message..
//!return true if valid JSON, and false otherwise. This looks for '{'  as the starting character (after possible spaces in front). A topic can be sent, or nil
boolean processJSONMessageMQTT(char *ascii, char* topic);

//! send semantic /smrun
//! 3.25.24 this is an HTTP not https
void publishSMRunMessage(char* smrunMessage);


//!Decode the URL (exposed 12.17.23 for the scanner
String MQTT_urlDecode(String input);

#endif // USE_MQTT_NETWORKING

#endif // MQTTNetworking_h

