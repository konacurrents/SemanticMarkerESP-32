/**
* \link MQTTModule \endlink
*/

/** The MQTT + WIFI part
 
 
 Created: on Jan 1, 2022
 Author: Scott Moody
 test..
 */

//#define VERSION "Version 1.6a -  3.8.22"  NOW defined in Defines.h

#include "../../Defines.h"

#ifdef USE_CAMERA_MODULE
#include <HTTPClient.h>
#endif //USE_CAMERA_MODULE

#define DECODE_BASE64
#ifdef DECODE_BASE64

//! 9.27.23 to decode a base64 string (a Semantic Marker)
#include <libb64/cdecode.h>
#endif


//!@see https://www.carletonsheets.com/assets/shared/usr/share/doc/doxygen-1.8.5/html/commands.html#cmdlink

//!Ambers 22nd birthday.. 2.20.22

//! 7.1.23 Dad's 92nd birthday
#include "SMARTButton.h"

/**
 Testing:
 1. the bootstrap of MQTT
 1.a  use a bad password and bad user
 1.b  see if it stops trying after a MAX tries
 2. send the credentials over BLE
 2.a   use a bad password and bad user
 2.b  see if it stops trying after MAX tries
 3. have a bad WIFI (so it won't connect)
 3.a see if it stops trying after a bit
 4. has valid WIFI and MQTT
 4.1  see it connects
 5. Using BOOTSTRAP
 5.a see if it connects
 5.b turn off BOOTSTRAP - re-upload script
 5.c  see if it reads the EPROM correctly
 TODO: send status information back over BLE since iPhone has a UI to troubleshoot..
 */

#include "MQTTNetworking.h"

#define ESP_EPROM_NAME "ESP32"


#ifdef USE_MQTT_NETWORKING
#include "OTAImageUpdate.h"

/*******************************MQTT*************************************/
//!added 1.1.2022 by iDogWatch.com
//!CURRENTLY, this code below only has constant:  usersP/bark as a super subscription
//!This could be passed in via JSON later..

//https://www.arduino.cc/en/Reference/BLECharacteristicConstructor

//! 3.3.22 Using the new JSON library which is supposed to catch syntax errors without blowing up
//https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
#include <ArduinoJson.h>

//the EPROM is in preferences.h
#include <Preferences.h>

#ifdef USE_REST_MESSAGING
#include <WiFiClientSecure.h>
#else
#include <WiFi.h>
#endif
#include <PubSubClient.h>

// wifi config store.  wifi配置存储的类
Preferences _preferencesMQTTNetworking;

//Can't seem to reuse this as a global .. it gets appended to, etc.
//DynamicJsonDocument myObject(1024);

//!just update the EPROM, and send that to the WIFI_AP module as well
void updatePreferencesInEPROM();

//!setup the WIFI using ssid and password
void setupWIFI(char * arg_ssid, char * arg_password);

//reset on success
int _globalMQTTAttempts = 0;
//this is the number of times to retry again, after trying WIFI again..
#define MAXglobalMQTTAttempts  10 //2
// the number of times to retry MQTT before trying WIFI again (then only MAXglobalMQTTAttemtt
#define MAX_MQTT_ATTEMPTS 10 //4

//max times to try the WIFI before attempting again..
#define MAX_WIFI_CONNECT_ATTEMPTS 30

//forward declarations
//!process an MQTT message looking for keywords (barklet language)
void processBarkletMessage(String message, String topic);

//!setup the MQTT server
void setupMQTT(char* mqttServerString, char *mqttPortString, char *mqttPasswordString, char *mqttUserString, char *deviceNameString, char *uuidString); //forward

//!process the JSON message (looking for FEED, etc). Note: topic can be nil, or if not, it's an MQTT topic (so send replies if you want)
boolean processJSONMessageMQTT(char *ascii, char *topic);

//check for MQTT messages???
void checkMQTTMessages_loop();


//!blinks the blue light
void blinkBlueLightMQTT();

//called to setup the MQTT (which is really the _mqttClient setup). Done on it's own thread..
void callPreSetupMQTT();

//!setup the WIFI
//void setup_WIFI(char *ssidString, char *ssidPasswordString);

//!read any values from EPROM
void MQTTModule_readPreferences();

//!uptime since last reboot.
int _startTimestamp = 0;

//Methods:  setupMQTT();
//          checkMQTTMessages_loop()

#define OTA "#OTA"

#define STATUS "#STATUS"
//NOTE: the "Me" names are known and keep them..
#define REMOTEME "#remoteMe"
#define REMOTE2 "#REMOTE"
#define FEED "#FEED"
#define FEED_2 "#feedme"
#define ACK_FEED  "#actMe"
#define ACK_FEED2 "#ackMe"
#define CONNECTED "#connectedMe"
#define NOT_CONNECTED "#noconnectedMe"
#define NO_ACK_FEED "#noactMe"
#define PLAY_ME "#playMe"
#define DOCFOLLOW "#docFollow"
#define DOCFOLLOW2 "#DOCFOLLOW"
#define DOCSYNC "#docSync"
#define NO_CAN "#NO_CAN"
//!DOCFOLLOW syntax 8.11.22
//! syntax: #followMe {AVM=<avm address>}  .. no quotes
#define FOLLOW_ME "#followMe"

//!The WIFI client
WiFiClient _espClient;

//!The PubSub MQTT Client
PubSubClient _mqttClient(_espClient);

int _counterLoop;
int _maxCounterLoop;

//!Decode the URL (copied from WIFI_APModule. Easier than breaking modules)
String MQTT_urlDecode(String input);

// *********************** METHODS invoked from BLE (JSON) and MQTT messages ***************
//!perform the OTA update. This calls the OTAImageUpdate methods (via preformOTAUpdateSimple())
void performOTAUpdateMethod();
//!calls the method for cleaning the SSID eprom. This calls the WIFI_APModule callback
void cleanSSID_EPROM_Method();
//!calls the FEED message via the callback (which calls the BLE code)
void performFeedMethod(char* topic);

//  *********************** END SPECIFICATION AND GLOBAL VARIABLES ******

//!returns seconds since first booted
int getUptime()
{
    int uptime = getTimeStamp_mainModule() - _startTimestamp;
    return uptime;
}

//!define here as well.. NOTE: this could be passed is as well... TODO


// NTP server to request epoch time
const char* _ntpServer = "pool.ntp.org";

// flag that the MQTT is running..
boolean _MQTTRunning = false;

#ifdef PROCESS_SMART_BUTTON_JSON
//!define this storage once, and use everwhere..
#define MAX_MESSAGE 2024
#else
#define MAX_MESSAGE 1024

#endif
//!message received on subscription
char _fullMessageIn[MAX_MESSAGE];
//! message to send out
char _fullMessageOut[MAX_MESSAGE];

char _semanticMarkerString[MAX_MESSAGE];

//!saves the group topic .. to write back on ..
char _lastGroupTopic[100];

//!Points to strings read from JSON (limited to 15 char key name)
#define _preferencesJSONName "JSONPrefs"
char* _ssidString;
char* _ssidPasswordString;
char* _mqttServerString;
char* _mqttPortString;
char* _mqttPasswordString;
char* _mqttGuestPasswordString;
char* _mqttUserString;
char* _mqttTopicString;
char* _deviceNameString;
char* _uuidString;
char* _jsonVersionString;
char* _jsonHeaderString;
char* _jsonLocationString;

//!this is sent from the backend as a message {'guest':'guest password'}  .. but lets' add to the credentials..
//char* _mqttGuestPasswordString = NULL;


//! whether message should be skipped for display and debug printouts
//! uses _fullMessageIn global
boolean skipMessageProcessing()
{
    boolean skip = false;
    if (containsSubstring(_fullMessageIn, ACK_FEED)
        || containsSubstring(_fullMessageIn, ACK_FEED2)
        || containsSubstring(_fullMessageIn, REMOTE2)
        || containsSubstring(_fullMessageIn, REMOTEME))
        skip = true;
    return skip;
}

//!the publishMQTTMessage is placed here as a placeholder for making the mqtt publish. If needed, this could be moved
//!to another thread (or the next loop)
#define TRY_MORE_ASYNC_PROCESSING
#ifdef TRY_MORE_ASYNC_PROCESSING
//! Wrapper of the mqttclient publish. NOTE: this might need to be in the loop as well, as the BLE could be the way the message arrived, and we are sending out over MQTT (while in the BLE thread). Don't know??
void publishMQTTMessage(char *topic, char *message)
{
    

    SerialMin.printf("Publish message(%s): %s\n",topic, message);
    if (!isConnectedMQTT_MQTTState())
    {
        SerialMin.println(" *** MQTT not connected .. message will queue until connected (hopefully)" );
    }
    //!publish on the mqttClient object
    _mqttClient.publish(topic, message);
    
    //!see if this pushs the publish out.. (otherwise a reply might occure .. an break our _fullMessage)
    //_mqttClient.loop();

}
#endif


//! Wrapper of the mqttclient publish
void publishMQTTMessageDefaultTopic(char *message)
{
    publishMQTTMessage(_mqttTopicString, message);
}

#ifdef USE_CAMERA_MODULE

//! Users/scott/Library/Arduino15/packages/m5stack/hardware/esp32/2.0.3/tools/sdk
#include "esp_camera.h"
#include "FS.h"                // SD Card ESP32

#endif
//! publish a binary file..
void publishBinaryFile(char *topic, uint8_t * buf, size_t len)
{
    SerialMin.printf("Publish binary file(%s), len=%d\n", topic, len);
    //   _mqttClient.publish(_mqttTopicString, buf, len);
    //!https://randomnerdtutorials.com/esp32-http-get-post-arduino/
    //!https://randomnerdtutorials.com/esp32-cam-post-image-photo-server/
    //!https://raw.githubusercontent.com/RuiSantosdotme/ESP32-CAM-Arduino-IDE/master/ESP32-CAM-HTTP-POST-Image/ESP32-CAM-HTTP-POST-Image.ino
    //! Lets do a POSt to my whats-this site..
#ifdef USE_CAMERA_MODULE
    
#define NEW_WAY
#ifdef NEW_WAY
    //!create a WIFI client that talks to just our upload servlet
    WiFiClient postClient;

    String serverName = "knowledgeshark.me";   // REPLACE WITH YOUR Raspberry Pi IP ADDRESS
                                           //String serverName = "example.com";   // OR REPLACE WITH YOUR DOMAIN NAME
    String serverPath = "/examples/servlets/UploadServlet";
    //!tomcat server.. 8080
    int serverPort = 8080;
    SerialDebug.println("2. Connecting to server: " + serverName);

    if (postClient.connect(serverName.c_str(), serverPort))
    {
        SerialDebug.printf("Connection successful! len = %d\n", len);
        String filename = "esp32-cam-" + String(getDeviceNameMQTT()) + "-" + String(random(0xffff), HEX) + ".jpg";
        String head = "--KonaCurrents\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"" +
                         filename + "\"\r\nContent-Type: image/jpeg\r\n\r\n";
        String tail = "\r\n--KonaCurrents--\r\n";
        
        uint32_t imageLen = len;
        uint32_t extraLen = head.length() + tail.length();
        uint32_t totalLen = imageLen + extraLen;
        
        postClient.println("POST " + serverPath + " HTTP/1.1");
        postClient.println("Host: " + serverName);
        postClient.println("Content-Length: " + String(totalLen));
        postClient.println("Content-Type: multipart/form-data; boundary=KonaCurrents");
        postClient.println();
        postClient.print(head);
        
        uint8_t *fbBuf = buf;
        size_t fbLen = len;
        for (size_t n=0; n<fbLen; n=n+1024)
        {
            if (n+1024 < fbLen)
            {
                postClient.write(fbBuf, 1024);
                fbBuf += 1024;
            }
            else if (fbLen%1024>0)
            {
                size_t remainder = fbLen%1024;
                postClient.write(fbBuf, remainder);
            }
        }
        postClient.print(tail);
        //!stop this client (it's recreated each publish)
        postClient.stop();
        
        //! WORKS FIRST TIME FROM M5 Camera, to tomcat on KnowledgeShark: 9.17.22
        //!publish location of this file.
        //String fileURL = "http://" + serverName + ":" + String(serverPort) + "/examples/uploads/" + filename;
       //!send this out as a DOCFOLLOW message (but different syntax)
       
        sprintf(_semanticMarkerString,"#url {%s} {http://%s:%d/examples/uploads/%s}", getDeviceNameMQTT(), &serverName[0], serverPort, &filename[0]);
        //sendSemanticMarkerDocFollow_mainModule(&fileURL[0]);
        //! for now only send if it start message starts with "#"
        publishMQTTMessageDefaultTopic(_semanticMarkerString);
        //seems to be sent 2 times ...
    }
    else
    {
        SerialDebug.printf("Connection NOT successful! ");
    }

    
#endif  //newway
    
#endif
}

//! These are set by the MQTT callback..
//! flag to let the processor know there are new messages
boolean _newMQTTMessageArrived = false;
//! the topic the new message came in on..
String _topic;

//!storage for the full JSON message string to send around..
String _fullJSONString;

//! retrieve the Configuration JSON string in JSON format..
String getJSONConfigString()
{
#define TRY_READING_BACK
#ifdef  TRY_READING_BACK
    //!NEW: 2.21.22
    //!TRY: reading back..
    _preferencesMQTTNetworking.begin(ESP_EPROM_NAME, false);  //false=read/write..
    _fullJSONString = _preferencesMQTTNetworking.getString(_preferencesJSONName);
    SerialDebug.print("Reading.12 EPROM JSON = ");
    SerialDebug.println(_fullJSONString? _fullJSONString:"NULL");
    
    //!check ... _fullMessageOut
    //! Close the Preferences
    _preferencesMQTTNetworking.end();
#endif
    return _fullJSONString;
}

//!callback with the message if required (like sending the FEED message)
//!!function pointers: https://www.cprogramming.com/tutorial/function-pointers.html
//!define as: void callback(char* message)
//!  call processMessage(message, &callback);
//void setMessageCallbacks(void (*callbackFunction)(char*), void (*blinkTheLED)())


//!called for things like the advertisement
char *getDeviceNameMQTT()
{
    if (!_deviceNameString)
        _deviceNameString = NOTSET_STRING;
    return _deviceNameString;
    // return _chipName;
}


uint32_t _chipId = 0;
char _chipName[100];

//!create a unique ID (but it needs to be stored.. otherwise it's unique each time??
void getChipInfo()
{
    
    //get chip ID
    for (int i = 0; i < 17; i = i + 8) {
        _chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    
    if (_deviceNameString)
        sprintf(_chipName, "%s-%d", _deviceNameString, _chipId);
    else
        sprintf(_chipName, "esp.%d", _chipId);
    
    //chipName = "esp." + chipId;
    SerialInfo.println(_chipName);
    
}

//!This uses the String (*getStatusFunc)(void))  to re-create this..
//!used by the displayModule to call this for each new status
const char* getDynamicStatusFunc()
{
    //Make URL for the status..
    char *statusString = main_currentStatusURL(true);
    
    //    sprintf(_semanticMarkerString,"%s/%s/%s/%s", "https://SemanticMarker.org/bot/sensor", _mqttUserString, _mqttGuestPasswordString, statusString);
    //shorten..
    sprintf(_semanticMarkerString,"%s/%s", "https://SemanticMarker.org/bot", statusString);
    
    SerialLots.print("getDynamicStatusFunc: ");
    SerialLots.println(_semanticMarkerString);
    
    return _semanticMarkerString;
}
//!examples
//!https://SemanticMarker.org/bot/status?v=v5&dev=M5WRR&b=71&temp=59&c=0&t=8&W=on&M=on&B=on&C=off&A=off&T=off&S=on&bleS=PTClicker:M5WRR&Z=off&G=on&P=DukeGEN3&t=8
//!https://SemanticMarker.org/bot/status?v=v5&dev=M55&b=94&temp=54&c=1&t=2&W=on&M=on&B=on&C=on&A=off&T=off&S=on&bleS=PTClicker:M55&Z=off&G=off&t=2

//!storage for last doc follow semantic marker
char _lastDocFollowSemanticMarker[200];
//! retrieves the last DocFollow SemanticMarker (from the message #DOCFOLLOW | #followMe {AVM=<SM>}
//! Or the JSON:  {'set':'semanticMarker','val','<URL>}
char *getLastDocFollowSemanticMarker_MQTTNetworking()
{
    return _lastDocFollowSemanticMarker;
}

//! sets the last DocFollow SemanticMarker
void setLastDocFollowSemanticMarker_MQTTNetworking(char *semanticMarker)
{
    strcpy(_lastDocFollowSemanticMarker, semanticMarker);
}

//! a counter to erase the last message if not changed in N calls..
int _countSinceLastChangedMessageStatus = 0;
//!storage of the last message status
char _lastMessageStatusURL[100];

//! Put all the storage initialization here..
void initAllArrayStorage()
{
    strcpy(_lastMessageStatusURL,"startup");
    strcpy(_lastDocFollowSemanticMarker,"https://SemanticMarker.org");
}

//! retrieves a token string.. without spaces.
//! Currently this will be things like
void setLastMessageStatus(char *token)
{
    SerialLots.print("setLastMessageStatus: ");
    SerialLots.println(token);
    _countSinceLastChangedMessageStatus = 0;

    char *deviceName = getDeviceNameMQTT();
    //! add just the version and device name to start, but add the msg=
    sprintf(_lastMessageStatusURL,"status?v=%s&dev=%s&msg=",VERSION_SHORT, deviceName);
    
    //! Make up a shorter version of the message
    if (strcasecmp(token,"FEED")==0)
        strcat(_lastMessageStatusURL,"FEED");
    else if (strcasecmp(token,"STATUS")==0)
        strcat(_lastMessageStatusURL,"STATUS");
    else
        strcat(_lastMessageStatusURL, "none");
    //!TODO: make sure no spaces ... unless escaped
    
    SerialTemp.println(_lastMessageStatusURL);
}

//!empty the status message
void emptyLastMessageStatus()
{
    _countSinceLastChangedMessageStatus = 0;
    setLastMessageStatus((char*)"none");
}

//!returns a string in in URL so:  status?battery=84'&buzzon='off'  } .. etc
char* currentMessageStatusURL()
{
    //!increment the count
    _countSinceLastChangedMessageStatus++;
    //5 (counts) seconds since a change..
    if (_countSinceLastChangedMessageStatus > 5)
    {
        emptyLastMessageStatus();
    }
    return _lastMessageStatusURL;
}

//!used by the displayModule to call this for each new status
const char* getDynamicMessageFunc()
{
    //Make URL for the status..
    char *statusString = currentMessageStatusURL();
    
    //    sprintf(_semanticMarkerString,"%s/%s/%s/%s", "https://SemanticMarker.org/bot/sensor", _mqttUserString, _mqttGuestPasswordString, statusString);
    //shorten..
    sprintf(_semanticMarkerString,"%s/%s", "https://SemanticMarker.org/bot", statusString);
    
    SerialTemp.print("getDynamicMessageFunc: ");
    SerialTemp.println(_semanticMarkerString);
    
    return _semanticMarkerString;
}

//! 4.26.22  50 year anniverssery of Grateful Dead in Frankfurt 1972
#define WIFI_MQTT_STATES
#ifdef WIFI_MQTT_STATES

//!the loop part of WIFI
void setupWIFI_loop();
//!end of WIFI loop..
void finishWIFI_Setup();
//!reconnects and re-subscribes
//!NOTE: we need the host info...
void reconnectMQTT_loop();

//!state variables
enum WIFI_MQTTStates
{
    // presetup WIFI
    preSetupWIFI,
    //in a wait for WIFI mode
    waitingForWIFI,
    //called to start the mqttClient (out of this thread)
    preSetupMQTT,
    //in a wait for MQTT mode
    waitingForMQTT,
    // all connected WIFI
    connectedWIFI,
    // connectged MQTT
    connectedMQTT,
    //all disconnected WIFI
    disconnectedWIFI,
    //all disconnected MQTT
    disconnectedMQTT
};
//!the state we are in..
WIFI_MQTTStates _WIFI_MQTTState = preSetupWIFI;
//!the delay in seconds for each state
float _WIFI_MQTTStateDelays[] =
{
    [preSetupWIFI] = 0.1,
    [waitingForWIFI]=0.3,
    [preSetupMQTT]=0.1,
    [waitingForMQTT]=0.2,
    [connectedWIFI]=0,
    [connectedMQTT]=0,
    [disconnectedWIFI]=0,
    [disconnectedMQTT]=0

};


//https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html
//! the time the delay started
unsigned long _delayStart_WIFI_MQTTState;
//! true if still waiting for delay to finish
boolean _delayRunning_WIFI_MQTTState = false;
//! length of delay
float _delaySeconds_WIFI_MQTTState;
//!init the delay, this uses the array of delays so we can change easier..
void startDelay_WIFI_MQTTState()
{
    float seconds = _WIFI_MQTTStateDelays[_WIFI_MQTTState];
    SerialLots.printf("startDelay_WIFI_MQTTState(%f)\n", seconds);
    _delayStart_WIFI_MQTTState = millis();   // start delay
    _delayRunning_WIFI_MQTTState = true; // not finished yet
    _delaySeconds_WIFI_MQTTState = seconds;
    
}
//!if finished..
boolean delayFinished_WIFI_MQTTState()
{
    if (_delayRunning_WIFI_MQTTState && ((millis() - _delayStart_WIFI_MQTTState) >= (_delaySeconds_WIFI_MQTTState * 1000)))
    {
        _delayRunning_WIFI_MQTTState = false;
        return true;
    }
    return false;
}

//!stop the delay
void stopDelay_WIFI_MQTTState()
{
    _delayRunning_WIFI_MQTTState = false;
}

#endif  //MQTT STATES

// *********************** END METHODS invoked from BLE (JSON) and MQTT messages ***************

//!try a flag so setupMQTTnetworking only called 1 times..
boolean _setupMQTTNetworkingAlready = false;

//!setup the MQTT part of networking
void setup_MQTTNetworking()
{
    SerialDebug.println("setup_MQTTNetworking");
    
    //make this non reentrant (only 1 time in a boot of ESP)
    if (_setupMQTTNetworkingAlready)
    {
        SerialDebug.println("setupMQTTNetworking already setup..");
        return;
    }
    //!init variables..
    initAllArrayStorage();
    
   // initLastMessageStatusURL();
    
    SerialDebug.println(" .. continue setup_MQTTNetworking");
    _setupMQTTNetworkingAlready = true;
    
    // sets the _chipName
    getChipInfo();
    
    //THIS should output the device name, user name, etc...
    SerialInfo.println(_chipName);
    
    //read what was stored, if any..
    MQTTModule_readPreferences();
    
    SerialTemp.print("setupMQTTNetworking ssid="); SerialInfo.print(_ssidString?_ssidString:"NULL");
    SerialTemp.print(", password = "); SerialInfo.print(_ssidPasswordString?_ssidPasswordString:"NULL");
    SerialTemp.println();
    
    _MQTTRunning = false;

    //set the state, then the 'loop' will call setupWIFI(...)
    _WIFI_MQTTState = preSetupWIFI;
    startDelay_WIFI_MQTTState();

    //!starts the delay for WIFI  checking, called at startup, and each time the timer finished..
    restartDelayCheckWIFI_MQTTNetworking();
}

//!value of WIFI connected
boolean isConnectedWIFI_MQTTState()
{
    
    SerialLots.printf("isConnectedWIFI_MQTTState: %s\n", _mqttClient.connected()?"connected":"not connected");
    if (!_mqttClient.connected())
    {
        _MQTTRunning = false;
        
        SerialTemp.printf("isConnectedWIFI_MQTTState: %s\n", _mqttClient.connected()?"connected":"not connected");
        return false;
    }
    return  _WIFI_MQTTState == waitingForMQTT || isConnectedMQTT_MQTTState();
}
//!value of MQTT connected
boolean isConnectedMQTT_MQTTState()
{
    return _MQTTRunning;
}
/**
 State:
 0 .
 1. waitingForWIFI  (delaying when
 */
//!Nice writeup: https://microcontrollerslab.com/esp32-mqtt-publish-multiple-sensor-readings-node-red/
//!
//! called for the loop() of this plugin
void loop_MQTTNetworking()
{
//#define TRY_AGAIN1  //9.19.23
#ifdef  TRY_AGAIN1
     if (!_ssidString || (_ssidString && strlen(_ssidString)==0))
     {
         SerialDebug.println("loop_MQTTNetworking .. null ssid");
        // setDoneWIFI_APModuleFlag(false); // this would turn off BLE server .. not good
         _WIFI_MQTTState = preSetupWIFI;
         //stopDelayCheckWIFI_MQTTNetworking();
         
     }
#endif
    //only check messages if MQTT is running (or want's to run.. )
    if (_MQTTRunning)
    {
        checkMQTTMessages_loop();
    }

    //!check if should try to reconnect to WIF
    checkDelaySinceWIFICheck_MQTTNetworking();

    //!check if a delay was running.. for the STATE..
    if (delayFinished_WIFI_MQTTState())
    {
        //SerialTemp.printf("delayFinished_WIFI_MQTTState: %d\n", _WIFI_MQTTState);
        //what state were we in for the delay.. continue doing that
        //NOTE: THese could be function pointers and just call the state we are in loop..
        // This is the "delay" loop
        switch (_WIFI_MQTTState)
        {
            case preSetupWIFI:
                //setup with WIFI
                setupWIFI(_ssidString, _ssidPasswordString);
                break;
                //in a wait for WIFI mode
            case waitingForWIFI:
                setupWIFI_loop();
                break;
                //called to init the _mqttClient
            case preSetupMQTT:
                callPreSetupMQTT();
                break;
                //in a wait for MQTT mode
            case waitingForMQTT:
                reconnectMQTT_loop();
                break;
                // all connected WIFI
            default:
                break;
        }
    }
}

// ******************************MQTT + WIFI ***********************************

//!show the status in string form (from Library/Adruino... WiFiType.h)
char *wifiStatus_MQTT()
{
    switch (WiFi.status())
    {
        case WL_NO_SHIELD:return (char*)"WL_NO_SHIELD";
        case WL_IDLE_STATUS: return (char*)"WL_IDLE_STATUS";
        case WL_NO_SSID_AVAIL: return (char*)"WL_NO_SSID_AVAIL";
        case WL_SCAN_COMPLETED: return (char*)"WL_SCAN_COMPLETED";
        case WL_CONNECTED :return (char*)"WL_CONNECTED";
        case WL_CONNECT_FAILED: return (char*)"WL_CONNECT_FAILED";
        case WL_CONNECTION_LOST: return (char*)"WL_CONNECTION_LOST";
        default:
        case WL_DISCONNECTED: return (char*)"WL_DISCONNECTED";
    }
}

//! a call to see if the WIFI is connected
//**** Delay Methods*******
#define SINGLE_DELAY
#ifdef SINGLE_DELAY
//https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html
//! the time the delay started
unsigned long _delayCheckWIFIStart_MQTTNetworking;
//! true if still waiting for delayCheckWIFI to finish
boolean _delayCheckWIFIRunning_MQTTNetworking = false;
//! length of delay
int _delayCheckWIFISeconds_MQTTNetworking;
//!init the delay
void startDelayCheckWIFI_MQTTNetworking(int seconds)
{
    SerialCall.printf("startdelayCheckWIFI_MQTTNetworking: %d\n", seconds);
    
    _delayCheckWIFIStart_MQTTNetworking = millis();   // start delay
    _delayCheckWIFIRunning_MQTTNetworking = true; // not finished yet
    _delayCheckWIFISeconds_MQTTNetworking = seconds;
    
}

//!get the delay values
int getDelayCheckWIFISeconds_MQTTNetworking()
{
    //return 10;  // 30 seconds ..   changed back to 10 9.19.23 in testing
    return 30;  // 30 seconds ..
}

//!starts the delay for WIFI  checking, called at startup, and each time the timer finished..
void restartDelayCheckWIFI_MQTTNetworking()
{
    startDelayCheckWIFI_MQTTNetworking(getDelayCheckWIFISeconds_MQTTNetworking());
}

//!if finished..
boolean delayCheckWIFIFinished_MQTTNetworking()
{
    if (_delayCheckWIFIRunning_MQTTNetworking && ((millis() - _delayCheckWIFIStart_MQTTNetworking) >= (_delayCheckWIFISeconds_MQTTNetworking * 1000)))
    {
        _delayCheckWIFIRunning_MQTTNetworking = false;
        SerialCall.println("delayCheckWIFIFinished_MQTTNetworking..");
        
        return true;
    }
    return false;
}

//!stop the delay (not called)
void stopDelayCheckWIFI_MQTTNetworking()
{
    SerialCall.println("stopDelayCheckWIFI_MQTTNetworking _delayRunning=false");
    
    _delayCheckWIFIRunning_MQTTNetworking = false;
}

//!checks delay for the WIFI connectivity
void checkDelaySinceWIFICheck_MQTTNetworking()
{
    if (delayCheckWIFIFinished_MQTTNetworking())
    {
        //!check and reconnect to the WIFI is not connected
        checkAndReconnectWIFI_MQTTNetworking();
        //!restart the timer
        restartDelayCheckWIFI_MQTTNetworking();
    }
}

//!The setup() will call restartDelay_MQTTNetworking
//!Each loop will call checkDelaySinceButtonTouched_MQTTNetworking
#endif //SINGLE_DELAY

//!print a SPIFF timestamp..

//!checks if the WIFI is off (or not reachable) and tries consecting again (the 'W' command)
void checkAndReconnectWIFI_MQTTNetworking()
{
    SerialMin.printf("checkAndReconnectWIFI_MQTTState: %s\n",wifiStatus_MQTT());
    boolean tryReconnect = true;
    switch (WiFi.status())
    {
        case WL_NO_SHIELD:
            break;
        case WL_IDLE_STATUS:
            break;
        case WL_NO_SSID_AVAIL:
            //tryReconnect = false;
            break;
        case WL_SCAN_COMPLETED:
            break;;
        case WL_CONNECTED:
            //!start outputing SPIFF info
#ifdef TOO_MUCH
            printTimestamp_SPIFFModule();
            println_SPIFFModule((char*)"WIFI WL_CONNECTED");
#endif
            SerialLots.printf("isConnectedWIFI = %s\n",isConnectedWIFI_MQTTState()?"connected":"not connected");
            //!it seems the WIFI can reconnect -- but all the MQTT isn't restarted.. So if our internal state things WIFI is off, reconnect anyway..
            if (!isConnectedWIFI_MQTTState())
                tryReconnect = true;
            else
                tryReconnect = false;
            break ;
        case WL_CONNECT_FAILED:
            printTimestamp_SPIFFModule();
            println_SPIFFModule((char*)"WIFI WL_CONNECT_FAILED");
            break;
        case WL_CONNECTION_LOST:
            printTimestamp_SPIFFModule();
            println_SPIFFModule((char*)"WIFI WL_CONNECTION_LOST");
            break;
        case WL_DISCONNECTED:
            printTimestamp_SPIFFModule();
            println_SPIFFModule((char*)"WIFI WL_DISCONNECTED");
            break;
        default:
            break;

    }
    
    //!try reconnecting if not connected (and ssid is available)
    if (tryReconnect)
    {
        //!start outputing SPIFF info
        printTimestamp_SPIFFModule();
        print_SPIFFModule((char*)"WIFI Reconnect attempt: ");
        println_SPIFFModule(wifiStatus_MQTT());
        SerialMin.println("reconnectAttempt");
        //!restart the WIFI and then MQTT connection
        restartWIFI_MQTTState();
    }
}
// ******************************HELPER METHODS, ***********************************

//!https://www.arduino.cc/en/Tutorial/Foundations/DigitalPins
void blinkBlueLightMQTT()
{
    //call method passed in..
    //  if (_blinkTheLED)
    //      (*_blinkTheLED)();
//    callCallbackMain(CALLBACKS_MQTT, MQTT_CALLBACK_BLINK, strdup("blink"));
    callCallbackMain(CALLBACKS_MQTT, MQTT_CALLBACK_BLINK, (char*)"blink");

}


//!setup the WIFI using ssid and password (called from setup_MQTTNetworking() .. the main setup for this module)
void setupWIFI(char * arg_ssid, char * arg_password)
{
    //state preSetupWIFI
    //SerialTemp.printf("setupWIFI(%d)\n", _WIFI_MQTTState);

    // We start by connecting to a WiFi network
    SerialDebug.printf("1. Connecting to '%s' password = '%s'\n", arg_ssid?arg_ssid:"NULL", arg_password?arg_password:"NULL");

    //!save some reason we are in the AP mode
    appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, " *** WIFI attempt:");
    appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, arg_ssid?arg_ssid:"No SSID");
    storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, arg_password);

    printTimestamp_SPIFFModule();
    print_SPIFFModule((char*)"setupWIFI: ");
    println_SPIFFModule(arg_ssid?arg_ssid:(char*)"empty");
#ifdef NOT_HELPING
    //! 9.19.23 if null . set max loop
    if (!arg_ssid || (arg_ssid && strlen(arg_ssid)==0))
    {
        SerialDebug.println(" NULL SSID in setupWIFI .. so leaving");
      //  _maxCounterLoop = MAX_WIFI_CONNECT_ATTEMPTS + 1;
#define TRY_EXIT3
#ifdef TRY_EXIT3
        //! 9.19.23 before Van Morrison ..
        {
            //putting here .. time might have gone too fast..
            _WIFI_MQTTState = disconnectedWIFI;
            //stopDelay_WIFI_MQTTState();
         //   return;
        }
#endif
        stopDelay_WIFI_MQTTState();
      //  _WIFI_MQTTState = disconnectedWIFI;
//        return;
        
    }
#endif
    //!start the WIFI mode and begin
    WiFi.mode(WIFI_STA);
    WiFi.begin(arg_ssid, arg_password);
    
    SerialDebug.println("WIF_STA mode..");
    
    //!set the counters..
    _counterLoop = 0;
    _maxCounterLoop = 0;
    
    //!reset the global attempts .. since we are trying to reconnect
    _globalMQTTAttempts = 0;
    
    //set the state..
    _WIFI_MQTTState = waitingForWIFI;
    startDelay_WIFI_MQTTState();
}


//!the loop part of WIFI. Call this each time the timer is up (the delay() )
//! and only go to the next state if state changes to waitingForMQTT
void setupWIFI_loop()
{
    //SerialTemp.printf("setupWIFI_loop(%d)\n", _WIFI_MQTTState);

    if (WiFi.status() == WL_CONNECTED)
    {
        SerialDebug.println("WiFi.status() == WL_CONNECTED()");
#ifdef STORE_DEBUG_INFO
        //!debug info
        storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "WiFi.status() == WL_CONNECTED()");
#endif
        //stop the timer..
        stopDelay_WIFI_MQTTState();
    
        //try the autoReconnect (seems default was true .. so no help)
        WiFi.setAutoReconnect(true);
        
        //finish up.. let that step change the "state"
        finishWIFI_Setup();
    }
    else if (_maxCounterLoop < MAX_WIFI_CONNECT_ATTEMPTS)
    {
#ifdef STORE_DEBUG_INFO

        //!debug info
        storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "WiFi.status() try again");
#endif
        //should still be waitingForMQTT
        // the is part of the loop..
        
        _counterLoop++;
        //SerialInfo.print(".");
        if (_counterLoop > 10)
        {
            _counterLoop = 0;
            //SerialInfo.println();
        }
        _maxCounterLoop++;
        
        //the delay is set by what state we are in, and this is called back
        // if the timer is finished...
        startDelay_WIFI_MQTTState();
    }
    else //_maxCounterLoop >= MAX_WIFI_CONNECT_ATTEMPTS
    {
        SerialDebug.println("WIFI **** Cannot connect .. try bluetooth update ... ");
        
#ifdef STORE_DEBUG_INFO

        SerialTemp.println("Before storePref");

        //!save some reason we are in the AP mode
//        appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, get_WIFIInfoString());
        SerialTemp.println("after 1. storePref");

        storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "MAX_WIFI_CONNECT_ATTEMPTS .. going to AP mode");
        SerialTemp.println("After storePref");
#endif
        
        //blink the LED
        // blinkBlueLightMQTT();
        callCallbackMain(CALLBACKS_MQTT, MQTT_CALLBACK_SOLID_LIGHT, (char*)"solidLight");
        
        //stop the timer..
        stopDelay_WIFI_MQTTState();
#ifdef GO_BACK_TO_ORIGINAL
#ifdef TRY_CALLBACK
#define TRY_CALLBACK
        //NOTE: THis is taken out.. so the credentials are the same.. next time you reboot it should work if
        // the wifi ssid is the same.
        //NOTE: let's delete the 'ssid' so it goes back into AP mode. via cleadSSID_EPROM
        //! call the callback for cleaning the SSID eprom..
        callCallbackMain(CALLBACKS_MQTT, MQTT_CLEAN_SSID_EPROM, (char*)"cleanSSID_EPROM");
        
        //showText_displayModule("Retry WIFI");
#else
        //!9.17.23 8.29.23  Lets just go into AP Mode
        //!Keep ProcessClientCmd short to let the callback run. instead change the feeder state flag
        //! processes a message that might save in the EPROM.. the cmd is still passed onto other (like the stepper module)
        processClientCommandChar_mainModule('R');
        showText_displayModule("AP Mode");
#endif
#endif // original
    }
}  //
//#define TRY_GET_WIFI
#ifdef TRY_GET_WIFI
//! for getting the Debug of the WIFI info
String _WIFIInfoString;
//! retrieve the WIFIInfoString
String get_WIFIInfoString()
{
    long rssi = WiFi.RSSI();

    _WIFIInfoString = "IP Address: " + WiFi.localIP();
    _WIFIInfoString += "\n WIFI SSID" + String(WiFi.SSID());
    _WIFIInfoString += "\n RSSI" + rssi;
   // _WIFIInfoString += "\n WIFI Status = " + String(wifiStatus_MQTT());

    SerialDebug.println(_WIFIInfoString.c_str());
    return _WIFIInfoString;
}
#else
String get_WIFIInfoString()
{
    char buf[30];
    sprintf(buf,"%s",WiFi.SSID());
    return "\n WIFI SSID: " + String(WiFi.SSID());
}
#endif

//! print the WIFI info
void printWIFIInfo()
{
    SerialMin.println("WiFi connected");
    SerialMin.print("IP Address: ");
    SerialMin.println(WiFi.localIP());
    SerialMin.print("WiFi SSID:");
    SerialMin.println(WiFi.SSID());
    long rssi = WiFi.RSSI();
    SerialMin.print("signal strength (RSSI):");
    SerialMin.print(rssi);
    SerialMin.println(" dBm");
}

//!end of WIFI loop..
void finishWIFI_Setup()
{
    SerialMin.println("finishWIFI_Setup()");

    //random ??  for the WIFI address?
    randomSeed(micros());
    
    //This creates a DHCP address
    printWIFIInfo();
    
    addToTextMessages_displayModule("IP ADDRESS");

    //NOTE: this is a 192.168.0.130 kind of address. But when the outside MQTT world see it,
    // is like : 72.106.50.236:49205  (The address of my entry to my subdomain point ..)
    
    //see if this works: took out to stop confusion..
    //  getExternalIP();
    
    //configure the time server
    configTime(0, 0, _ntpServer);
    
    //set the time on startup
    _startTimestamp = getTimeStamp_mainModule();
    
    //blink the LED
    blinkBlueLightMQTT();
    
    SerialDebug.println("setupMQTTNetworking_WIFI done..");
    
    _WIFI_MQTTState = preSetupMQTT;
    
    //lets kick off a delay.. this could be 0 or 1 ?? the first time
    startDelay_WIFI_MQTTState();
  
    
}

//!called to setup the MQTT (which is really the _mqttClient setup). Done on it's own thread..
void callPreSetupMQTT()
{
    SerialDebug.printf("callPreSetupMQTT(%d, %s)\n", _WIFI_MQTTState, _deviceNameString?_deviceNameString:"NULL");

 //
    //this would use the values .. and then we save afterwards..
    setupMQTT(_mqttServerString, _mqttPortString, _mqttPasswordString, _mqttUserString, _deviceNameString, _uuidString);
    
    //NEW -
    //head to the next .. state == waitingForMQTT
    _WIFI_MQTTState = waitingForMQTT;
    
    //lets kick off a delay.. this could be 0 or 1 ?? the first time
    startDelay_WIFI_MQTTState();
    
}

//! break up the MQTT Handler 8.12.22 as per "My guess is that you have your data collection (from some I2C device) and data delivery intermingled. Separate them so that you have the data in hand before you make the network connection. That will reduce the possibility of timeouts and race conditions. It also makes it easier to add new collection and delivery processes. The more asynchronous you can make these steps, the more robust your application will be overall."


//! add globals for knowing the type of message.
//! call the check message processing
//!state variables
enum MQTTMessageTopicType
{
    // usersP/bark/<user>
    userTopic,
    // usersP/dawgpack
    dawgpackTopic,
    // usersP/bark
    superTopic,
    // usersP/bark/groups/#
    groupTopic
    
} _MQTTMessageTopicType = userTopic;

//!helper to know it's a dawgpack topic (and not process in most cases). Only support DOCFOLLOW for now..
boolean isDawgpackTopic()
{
    return _MQTTMessageTopicType == dawgpackTopic;
}
//!helper to know it's a superuser topic (and not process in most cases).
boolean isSuperTopic()
{
    return _MQTTMessageTopicType == superTopic;
}
//!helper to know it's a superuser topic (and not process in most cases).
boolean isGroupTopic()
{
    return _MQTTMessageTopicType == groupTopic;
}
//!classify a topic
void classifyTopic(char *topic)
{
    //!set the topic type
    if (strcmp(topic,"usersP/dawgpack")==0)
        _MQTTMessageTopicType = dawgpackTopic;
    else if (strcmp(topic,"usersP/bark") == 0)
        _MQTTMessageTopicType = superTopic;
    else if (containsSubstring(topic,"usersP/groups/"))
    {
        _MQTTMessageTopicType = groupTopic;
        strcpy(_lastGroupTopic, topic);
        SerialDebug.printf("Group Topic: %s\n", topic);
    }
    else
        _MQTTMessageTopicType = userTopic;
}

//!prints the topic on debug
void printTopicType()
{
    SerialDebug.print("Topic = ");
    switch (_MQTTMessageTopicType)
    {
        case userTopic:
            SerialDebug.println("userTopic");
            break;
            // usersP/dawgpack
        case dawgpackTopic:
            SerialDebug.println("dawgPackTopic");
            break;
            // usersP/bark
        case superTopic:
            SerialDebug.println("superTopic");
            break;
            // usersP/bark/groups
        case groupTopic:
            SerialDebug.println("groupTopic");
            break;
    }
}

//if (_MQTTMessageTopicType == superTopic)
//if (_MQTTMessageTopicType == dawgpackTopic)



//!called when data on the MQTT socket
void callbackMQTTHandler(char* topic, byte* payload, unsigned int length)
{
    SerialLots.printf("callbackMQTTHandler topic: %s\n", topic);
    int i = 0;
    for (i = 0; i < length && i < MAX_MESSAGE; i++) {
        _fullMessageIn[i] = (char)payload[i];
    }
    _fullMessageIn[i] = (char)NULL;
    
    //note there is a strange issue where a "null" is showing up in my MQTT/Websocket path
    //eg: Message arrived [idogwatch/bark] Guest35: null
    //sow for now.. not processing if the message has a "null"
    if (containsSubstring(_fullMessageIn, "null"))
    {
        //don't process..
        return;
    }
    
    //! too many printouts which actully slows things down.. start with actMe   (or collect a count of #actme and report that??)
    if (!skipMessageProcessing())
       SerialDebug.printf("MessageArrived: '%s', onTopic=%s\n", _fullMessageIn,  topic);
    
    //!classify the topic type
    classifyTopic(topic);
    
    //! 7.26.23 don't process if a group message and FLAG not set
    if (_MQTTMessageTopicType == groupTopic)
    {
        // if the EPROM says not to process groups .. then skip this message..
        //! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
        if ( !getPreferenceBoolean_mainModule(PREFERENCE_SUPPORT_GROUPS_SETTING))
        {
            SerialDebug.printf("NOT PRocessing as PREFERENCE_SUPPORT_GROUPS_SETTING not set");
            return;
        }
    }


    //!NOTE: This assumes the callbackMQTTHandler is only called once per message processed, as the next time in the loop(), it processes this _fullMessage since the _newMQTTMessageArrived == true
    
#ifdef TRY_MORE_ASYNC_PROCESSING
    _topic = topic;
    _newMQTTMessageArrived = true;
    SerialLots.printf("MessageArrived: '%s', onTopic=%s\n", _fullMessageIn,  topic);

#else
    //!save some part of this message for later display by SemanticMarker 8.4.22
   // setLastMessageStatus(_fullMessage);

    //process this message (We don't recognize just a JSON config yet..) That arrives on bluetooth
    processBarkletMessage(_fullMessageIn, topic);
    
    //send to the Display .. but truncate
    //! 11.7.22  if it's an #actMe .. don't show
   if (!skipMessageProcessing())
        addToTextMessages_displayModule(_fullMessageIn);
    
#endif
    SerialLots.println(" -- DONE processsBarkletMessage ----");
}



//!reconnects and re-subscribes
//!NOTE: we need the host info...
void reconnectMQTT_loop()
{
    //SerialTemp.println("reconnectMQTT_loop()");
//#define NOTNOW
#ifdef NOTNOW
    if (isEmptyString(_ssidPasswordString) || isEmptyString(_mqttPasswordString))
    {
        SerialDebug.println(" *** No ssid or password");
        //! MQTT is not connecting .. so go to AP mode
        //!9.17.23, 8.29.23  Lets just go into AP Mode
        //!Keep ProcessClientCmd short to let the callback run. instead change the feeder state flag
        //! processes a message that might save in the EPROM.. the cmd is still passed onto other (like the stepper module)
        processClientCommandChar_mainModule('R');
        return;
        
    }
#endif
    if (_mqttClient.connected())
    {
        SerialDebug.println("reconnectMQTT_loop: _mqttClient.connected()");
#ifdef STORE_DEBUG_INFO
        //!save some reason we are in the AP mode
        storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "_mqttClient.connected()");
#endif
        
        stopDelay_WIFI_MQTTState();
        _WIFI_MQTTState = connectedMQTT;
    }
    else if (_globalMQTTAttempts > MAXglobalMQTTAttempts)
    {
#ifdef STORE_DEBUG_INFO

        //!save some reason we are in the AP mode
        appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "MQTT_SERVER");
        appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, _mqttServerString?_mqttServerString:"NULL");
        appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "MQTT_USER");
        appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, _mqttUserString?_mqttUserString:"NULL");
        appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "MQTT_PASSWORD");
        appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, _mqttPasswordString?_mqttPasswordString:"NULL");
        storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "STOPPING MQTT connection attempt > max");
#endif
        SerialDebug.printf("STOPPING MQTT connection attempts: '%s' count=%d\n", _mqttServerString?_mqttServerString:"NULL", _globalMQTTAttempts);
        
        _WIFI_MQTTState = disconnectedMQTT;
        
        //! 9.17.23 .. I think the cleaning 
        //! call the callback for cleaning the SSID eprom..
        //callCallbackMain(CALLBACKS_MQTT, MQTT_CLEAN_SSID_EPROM, (char*)"cleanSSID_EPROM");
#ifdef NOT_IN_ORIGINAL
        //! MQTT is not connecting .. so go to AP mode
        //!9.17.23, 8.29.23  Lets just go into AP Mode
        //!Keep ProcessClientCmd short to let the callback run. instead change the feeder state flag
        //! processes a message that might save in the EPROM.. the cmd is still passed onto other (like the stepper module)
        processClientCommandChar_mainModule('R');
        
        // TODO.. why not going into AP mode???
#endif
    }
    else if (_counterLoop > MAX_MQTT_ATTEMPTS)
    {
        SerialInfo.println("FAILED MQTT .. so lets try connecting to WIFI again..");
        SerialInfo.println("Setting WIFI from JSON parameters");
        //try to setup the WIFI again, seems to help.
#ifdef STORE_DEBUG_INFO
        //!save some reason we are in the AP mode
        storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "FAILED MQTT .. so lets try connecting to WIFI again");
#endif
        //set the state back to starting WIFI..
        _WIFI_MQTTState = preSetupWIFI;
        setupWIFI(_ssidString, _ssidPasswordString);
    }
    else
    {
        //Lets try to connect...
        //reset on connection, or new BLE config info...
        _globalMQTTAttempts++;
       // _WIFI_MQTTState = waitingForMQTT;
        _WIFI_MQTTState = preSetupMQTT;

        SerialDebug.printf("Attempting MQTT connection: '%s' '%s' '%s' count=%d\n", _mqttServerString?_mqttServerString:"NULL", _mqttUserString?_mqttUserString:"NULL", _mqttPasswordString?_mqttPasswordString:"NULL", _globalMQTTAttempts);
#ifdef STORE_DEBUG_INFO

        //!save some reason we are in the AP mode
        appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "Attempting MQTT connection:");
        appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, _mqttServerString?_mqttServerString:"NULL");
        storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, _mqttPasswordString?_mqttPasswordString:"NULL");

#endif
        // Create a random client ID
#ifdef ESP_M5
        String clientId = "espM5-";
#endif
#ifdef ESP_32
        String clientId = "esp32-";
#endif
        clientId += String(random(0xffff), HEX);
        
        SerialDebug.printf("attempt _mqttClient.connect(%s)\n", clientId);
 
        // Attempt to connect   NOTE: this takes time...
        //TODO: use the argments...
        if (_mqttClient.connect(clientId.c_str(), _mqttUserString, _mqttPasswordString))
        {
            
            
            
            //!try making a bigger packet..  10.23.22  (seems to help)
            //!Looking at the PubSubClient.cpp,
            //!   if (this->bufferSize < MQTT_MAX_HEADER_SIZE + 2+strnlen(topic, this->bufferSize) + plength) {
            _mqttClient.setBufferSize(MAX_MESSAGE + MQTT_MAX_HEADER_SIZE);
            
            addToTextMessages_displayModule(_mqttUserString);
#ifdef STORE_DEBUG_INFO
            //!save for debug
            storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "MQTT CONNECTED");
#endif
            
            SerialInfo.println("MQTT CONNECTED");
            blinkBlueLightMQTT();
            
            //_mqttTopicString = strdup("usersP/bark/scott@konacurrents.com");
            //NOTE: if topic not supported ... no good error message ... it disconnects
            //NOTE: no wildcards allowed on statusfeed.
            // Once connected, publish an announcement...
            //NOTE: _jsonLocationString is null... sometimes.
            sprintf(_fullMessageOut, "%s {%s}{'mqttUser':'%s','location':'%s','uptime':'%d',%s,'v':'%s'}", REMOTEME, _deviceNameString?_deviceNameString:"NULL", _mqttUserString?_mqttUserString:"NULL", _jsonLocationString?_jsonLocationString:"somewhere", getUptime(), main_currentStatusJSON(), VERSION);
            
            SerialInfo.println(_fullMessageOut);
            
#ifdef TRY_MORE_ASYNC_PROCESSING
            //publish back on topic
            publishMQTTMessage(_mqttTopicString, _fullMessageOut);
#else
            _mqttClient.publish(_mqttTopicString, _fullMessageOut);
#endif
            
#ifdef NOT_NOW_ONLY_DOCFOLLOW
#ifdef TRY_MORE_ASYNC_PROCESSING
            //! NOTE publish on the dawgpack as well so a single user can monitor the events..

            publishMQTTMessage((char*)"usersP/dawgpack", _fullMessageOut);
#else
            _mqttClient.publish("usersP/dawgpack", _fullMessageOut);
#endif
#endif
            
#define TRY_GROUP
#ifdef TRY_GROUP
            //!@see https://pubs.opengroup.org/onlinepubs/000095399/functions/index.html
            //! if the EPROM says not to process groups .. then skip this message..
            //! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
            if (getPreferenceBoolean_mainModule(PREFERENCE_SUPPORT_GROUPS_SETTING))
            {
                //!get the group names
                char *groupNames = getPreference_mainModule(PREFERENCE_GROUP_NAMES_SETTING);
                SerialDebug.printf("Subscribing to groups as PREFERENCE_SUPPORT_GROUPS_SETTING was set: %s\n", groupNames);

                if (strlen(groupNames)==0 || strcmp(groupNames, "#")== 0)
                {
                    //! 7.15.23  have it's own root "groups" so those that subscribe to 'bark' won't get it unless published 2 times (which it is)
                    //! the wildcard WORKS !!!
                    _mqttClient.subscribe((char*)"usersP/groups/#");
                    SerialTemp.printf("Subscribe usersP/groups/#\n");
                }
                else
                {
                    //!@see https://www.educative.io/answers/splitting-a-string-using-strtok-in-c
                    //!@see https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
                    // parse the group names .. TODO..
                    /*
                     The strtok_r() function is a reentrant version strtok(). The saveptr argument is a pointer to a char * variable that is used internally by strtok_r() in order to maintain context between successive calls that parse the same string.
                     
                     On the first call to strtok_r(), str should point to the string to be parsed, and the value of saveptr is ignored. In subsequent calls, str should be NULL, and saveptr should be unchanged since the previous call.
                     
                     
                     char *strtok_r(char *str, const char *delim, char **saveptr);
                     
                     https://linux.die.net/man/3/strtok_r
                     */
                    char groupSub[100];
                    char *str = groupNames;
                    char *rest = NULL;
                    char *token;
                    for (token = strtok_r(str,",",&rest); token != NULL; token = strtok_r(NULL, ",", &rest))
                    {
                        sprintf(groupSub,"usersP/groups/%s", token);
                        _mqttClient.subscribe(groupSub);
                        SerialTemp.printf("Subscribe %s\n", groupSub);
                    }
                    
                }
            }
#endif
            // ... and resubscribe to same topic  8.3.22
            _mqttClient.subscribe(_mqttTopicString);
            
            //add another topic ... (should be ok with  usersP/bark/# )
            
            //2.2.22  The root level usersP/bark for messages from the super-user
            // _mqttTopicString = strdup("usersP/bark");
            // ... and resubscribe to same topic
            _mqttClient.subscribe((char*)"usersP/bark");
            
            //! Only subscribe if turned on.. 8.17.22
            if (getPreferenceBoolean_mainModule(PREFERENCE_SUB_DAWGPACK_SETTING))
            {
                //! 8.15.22  Also subscribe to the dawgpack .. but restrict what it can effect.
                //! For example, start with STATUS and DOCFOLLOW
                _mqttClient.subscribe((char*)"usersP/dawgpack");
            }
            _MQTTRunning = true;
            
            //!reset the global attempts .. since we connected
            _globalMQTTAttempts = 0;
            
            _WIFI_MQTTState = connectedMQTT;
            stopDelay_WIFI_MQTTState();
        }
        else
        {
#ifdef STORE_DEBUG_INFO

            //!save for debug
            appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "FAILED MQTT connect:");
            storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING,  String(_mqttClient.state()));
#endif
            
            //receiving state = -2  ???
            //https://forum.arduino.cc/t/mqtt-esp32-nodemcu-failed-with-state-2-connecting-to-mqtt/939270
            SerialTemp.printf("FAILED, rc=%d, trying again in 0.2 seconds\n", _mqttClient.state());
            //or reset something...
            // Wait .2 seconds before retrying
            //try..
            //NOTE: state hasn't changed but should be waitingMQTT..
            startDelay_WIFI_MQTTState();
        }
    } //while not connected
}

//!setup the MQTT (called after the WIFI connected)
void setupMQTT(char* mqttServerString, char *mqttPortString, char *mqttPasswordString, char *mqttUserString, char *deviceNameString, char *uuidString)
{
    SerialTemp.println("**** setupMQTT *****");
#ifdef STORE_DEBUG_INFO

    appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "setupMQTT ");
    appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, mqttServerString?String(mqttServerString):"NULL");
    appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, mqttPortString?String(mqttPortString):"NULL");
    appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, mqttPasswordString?String(mqttPasswordString):"NULL");
    appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, mqttUserString?String(mqttUserString):"NULL");
#endif
    // make sure the password etc are valid
    if (mqttServerString && mqttPortString && mqttPasswordString && mqttUserString)
    {
        //connect to server:port
        int port = atoi(mqttPortString);
        _mqttClient.setServer(mqttServerString, port);
        _mqttClient.setCallback(callbackMQTTHandler);
        _MQTTRunning = true;
        
        //try to connect.. so _MQTTRUnning is not completed, just the next phase..
        
        //! print the WIFI info AGAIN..
        printWIFIInfo();
        
        //!debug
//        storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, get_WIFIInfoString());
#ifdef BOMBS
        //debug
        appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "IP Address: " + WiFi.localIP());
       // appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "WIFI SSID" + WiFi.SSID);

        appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, WiFi.SSID());

        storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "WIFI Status = " + String(wifiStatus_MQTT() ));
#endif
    }
    else
    {
        
#ifdef STORE_DEBUG_INFO
        //!debug

        storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING,  "*** No MQTT Server/Port Specified ** abort");
#endif
        SerialInfo.println(" *** No MQTT Server/Port Specified ** abort");
        _MQTTRunning = false;
    }
    
#ifdef STORE_DEBUG_INFO

    SerialTemp.println("done setupMQTT");
    storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "done setupMQTT");
#endif

}

//!check for MQTT messages, called from the main loop()
void checkMQTTMessages_loop()
{
    //!don't do the loop at same time as check, do it at another time..
    if (_newMQTTMessageArrived)
    {
#ifdef TRY_MORE_ASYNC_PROCESSING
        //!save some part of this message for later display by SemanticMarker 8.4.22
        // setLastMessageStatus(_fullMessageIn);
        if (!skipMessageProcessing())
        {
            //process this message (We don't recognize just a JSON config yet..) That arrives on bluetooth
            processBarkletMessage(_fullMessageIn, _topic);
            
            //send to the Display .. but truncate
            //! 11.7.22  if it's an #actMe .. don't show
            addToTextMessages_displayModule(_fullMessageIn);
        }
#endif
        _newMQTTMessageArrived = false;
    }
    else
    {
        _newMQTTMessageArrived = false;
        //!call the MQTT infrastructure loop which does it's MQTT messaging
        _mqttClient.loop();
    }
}

//!check if the string matches
bool stringMatch(String message, String substring)
{
    return strcmp(&message[0], &substring[0]) == 0;
}

//!! should be a definition that the bluetooth is ONLINE
boolean bluetoothOnline()
{
    return true;
}

//!cleans the eprom info
void cleanEPROM_MQTTNetworking()
{
    SerialDebug.println("cleanEPROM_MQTTNetworking");
    _preferencesMQTTNetworking.begin(ESP_EPROM_NAME, false);  //readwrite..
    _preferencesMQTTNetworking.clear();
    _preferencesMQTTNetworking.end();
}

//! just send a message (let the internals to figure out topics, etc..
//!so the BLE can send something on the MQTT
//! for now only send if it start message starts with "#"
void sendMessageMQTT(char *message)
{
    if   (_MQTTRunning)
    {
        //Basically if we send {'cmd':'buzzon'} -- it comes back to us.. and infinite loop
        // for now only send if it start message starts with "#"
        if (containsSubstring(message, "#"))
        {
            sprintf(_fullMessageOut,"%s {%s}", message, _deviceNameString);

            //publish this message..
//            _mqttClient.publish(_mqttTopicString, _fullMessageOut);
//            SerialTemp.printf("Sending message:%s %s\n",_mqttTopicString,  _fullMessageOut);
#ifdef TRY_MORE_ASYNC_PROCESSING
            //publish back on topic
            publishMQTTMessage(_mqttTopicString, _fullMessageOut);
#else
            _mqttClient.publish(_mqttTopicString, _fullMessageOut);
#endif
        }
    }
}

//! just send a message but without any extras
void sendMessageNoChangeMQTT(char *message)
{
    if   (_MQTTRunning)
    {
        //Basically if we send {'cmd':'buzzon'} -- it comes back to us.. and infinite loop
        // for now only send if it start message starts with "#"
        
        sprintf(_fullMessageOut,"%s", message);
        
        //publish this message..
//        _mqttClient.publish(_mqttTopicString, _fullMessageOut);
//        SerialTemp.printf("Sending message:%s %s\n",_mqttTopicString,  _fullMessageOut);
#ifdef TRY_MORE_ASYNC_PROCESSING
        //publish back on topic
        publishMQTTMessage(_mqttTopicString, _fullMessageOut);
#else
        _mqttClient.publish(_mqttTopicString, _fullMessageOut);
#endif
    }
}

//! sends the semantic marker as a doc follow message #remoteMe (vs STATUS, as that triggers a status reply.. )
void sendStatusMessageMQTT_deviceName(char *deviceName, const char *semanticMarker)
{
    SerialCall.println("sendStatusMessageMQTT..");
    //! don't call main_currentStatusURL .. since it was already called
    sprintf(_fullMessageOut, "#remoteMe {%s} {AVM=%s}", deviceName, semanticMarker);
    //sprintf(_fullMessageOut, "#remoteMe {%s} {AVM=%s%s}", deviceName, semanticMarker, main_currentStatusURL(false));
    if   (_MQTTRunning)
    {
        //        _mqttClient.publish(_mqttTopicString, _fullMessageOut);
        //        SerialMin.printf("Sending message: %s\n", _fullMessageOut);
#ifdef TRY_MORE_ASYNC_PROCESSING
        //publish back on topic
        publishMQTTMessage(_mqttTopicString, _fullMessageOut);
#else
        _mqttClient.publish(_mqttTopicString, _fullMessageOut);
#endif
    }
}

//! sends the semantic marker as a doc follow message #remoteMe (vs STATUS, as that triggers a status reply.. )
void sendStatusMessageMQTT(const char *semanticMarker)
{
    sendStatusMessageMQTT_deviceName(_deviceNameString, semanticMarker);
}
//! sends the semantic marker as a doc follow message
void sendDocFollowMessageMQTT(const char *semanticMarker)
{
    SerialCall.println("sendDocFollowMessageMQTT..");
    if (!containsSubstring(semanticMarker,"https"))
        sprintf(_fullMessageOut, "#DOCFOLLOW {%s} {AVM=https://SemanticMarker.org/bot/%s}", _deviceNameString, semanticMarker);
    else
#ifdef ESP_M5_ATOM_LITE
        //! using the followme syntax for now..
        sprintf(_fullMessageOut, "#followMe {AVM=%s}", semanticMarker);

#else
        sprintf(_fullMessageOut, "#DOCFOLLOW {%s} {AVM=%s}", _deviceNameString, semanticMarker);
#endif
    if   (_MQTTRunning)
    {
//        _mqttClient.publish(_mqttTopicString, _fullMessageOut);
//        SerialMin.printf("Sending message: %s\n", _fullMessageOut);
#ifdef TRY_MORE_ASYNC_PROCESSING
        //publish back on topic
        publishMQTTMessage(_mqttTopicString, _fullMessageOut);
#else
        _mqttClient.publish(_mqttTopicString, _fullMessageOut);
#endif
    }
}

//!process an MQTT message looking for keywords (this version uses the Barklet Language Grammer @c 2014)
//!NOTE: The processJSONMessage() is part of this (called if straight JSON).
//!TODO: merge these two methods..
void processBarkletMessage(String message, String topic)
{
    //!https://stackoverflow.com/questions/7352099/stdstring-to-char
    int time = getTimeStamp_mainModule();
    
    //!convert String to char *
    char *messageString = &message[0];
    //!flag to send the message back on MQTT
    bool messageValidToSendBack = false;
    SerialCall.print("processBarkletMessage: ");
    SerialCall.print(message);
    SerialCall.print("   topic=");
    SerialCall.println(topic);
    
    if (!topic)
        _MQTTMessageTopicType = userTopic;
    
    //!debug printout..
    //printTopicType();

    //!new 4.12.22  if this is straight JSON .. then sent to the processJSONmessage
    if (processJSONMessageMQTT(messageString, topic?&topic[0]:NULL))
    {
        //This was processed by the JSON processor
        //SerialDebug.println("** not an older Barklet message syntax, but straight JSON ***");
        return;
    }
    
    //!If the dawgpack, only process the DOCFOLLOW message
    //!
    //!note: these messages are sent to MQTT. But the messages comming down originated on WebSocket barklets language
    //! so the 'remoteMe ..." gets up there, but not back to the rest. It's rewritten by nodered.
    if (containsSubstring(message, STATUS) && !isDawgpackTopic())
    {
        float temp = getTemperature_mainModule();
        
        //!save some part of this message for later display by SemanticMarker 8.4.22
        //!set the status
        setLastMessageStatus((char*)"status");
        
        char pairedDevice[100];
        if (isValidPairedDevice_mainModule())
        {
            strcpy(pairedDevice,getPairedDevice_mainModule());
//            strcat(pairedDevice,(char*)":");
//            strcat(pairedDevice,getPairedDeviceAddress_mainModule());
        }
        else
            strcpy(pairedDevice,"none");
#ifdef USE_BLE_CLIENT_NETWORKING
        boolean isConnectedBLE = isConnectedBLEClient();
#else
        boolean isConnectedBLE = false;
#endif
        boolean isGateway = getPreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE);
        //! process the pair if match
        if (isConnectedBLE && isGateway)
        {
            //! FOR NOW , copy the code and create a _fullMessageOut that is for the Paired device...
            
            sprintf(_fullMessageOut, "%s {%s} {%s} {I,F}  {'T':'%d','dev':'%s','user':'%s','location':'%s','v':'%s','ble':'%s,%s}",
                    REMOTEME,
                    pairedDevice,
                    bluetoothOnline() ? CONNECTED : NOT_CONNECTED,
                    time,
                    pairedDevice,
                    _mqttUserString?_mqttUserString:"NULL",
                    _jsonLocationString?_jsonLocationString:"somewhere",
                    VERSION,
#ifdef USE_BLE_SERVER_NETWORKING
                    //! retrieve the service name (PTFEEDER, PTFeeder:Name, PTClicker:Name, etc)
                    getServiceName_BLEServerNetworking()
#else
                    "none"
#endif
                    // if calling this.. add "%s" to sprintf above..
                    , main_currentStatusJSON()
                    );
            
            //publish this message..
            SerialTemp.printf("GEN3: Sending message: %s\n", _fullMessageOut);

            //_mqttClient.publish(_mqttTopicString, _fullMessageOut);
#ifdef TRY_MORE_ASYNC_PROCESSING
            //publish back on topic
            publishMQTTMessage(_mqttTopicString, _fullMessageOut);
#else
            _mqttClient.publish(_mqttTopicString, _fullMessageOut);
#endif
            //! 5.21.22 WORKS!!
            //!topic is the topic we can in on.. so could be super user..
            //if (strcmp(&topic[0],"usersP/bark")==0)
            if (isSuperTopic())
            {
                SerialTemp.println("Sending on DawgPack too..");
                //publish back on topic
                //_mqttClient.publish("usersP/dawgpack", _fullMessageOut);
#ifdef TRY_MORE_ASYNC_PROCESSING
                //publish back on topic
                publishMQTTMessage((char*)"usersP/dawgpack", _fullMessageOut);
#else
                _mqttClient.publish("usersP/dawgpack", _fullMessageOut);
#endif
            }
        }

        //sprintf(_fullMessageOut, "%s {%s} {%s} {I,F} {T=now}", REMOTEME, _deviceNameString, bluetoothOnline() ? CONNECTED : NOT_CONNECTED);
        // Once connected, publish an announcement...
        // sprintf(message, "#STATUS {%s} {%s}", _deviceNameString, chipName);
        sprintf(_fullMessageOut, "%s {%s} {%s} {I,F}  {'T':'%d','dev':'%s','user':'%s','location':'%s','ble':'%s','v':'%s',%s}",
                REMOTEME,
                _deviceNameString?_deviceNameString:"NULL",
                bluetoothOnline() ? CONNECTED : NOT_CONNECTED,
                time,
                _deviceNameString?_deviceNameString:"NULL",
                _mqttUserString?_mqttUserString:"NULL",
                _jsonLocationString?_jsonLocationString:"somewhere",
#ifdef USE_BLE_SERVER_NETWORKING
                //! retrieve the service name (PTFEEDER, PTFeeder:Name, PTClicker:Name, etc)
                getServiceName_BLEServerNetworking()
#else
                "none"
#endif
                , VERSION
                , main_currentStatusJSON()
                );
        
        messageValidToSendBack = true;
        
        addToTextMessages_displayModule(_fullMessageOut);

        //On demand #STATUS send the statusURL as well (if an M5)
        //this queues the sending of the StatusURL over MQTT.
        // This is async (next loop) since sending 2 MQTT messages can be hard to do in a row ..
        main_dispatchAsyncCommand(ASYNC_SEND_MQTT_STATUS_URL_MESSAGE);
    }
    // else if (containsSubstring(message, "#FEED") || containsSubstring(message, "feedme"))
    //only use #FEED (the feedme will turn into #FEED)
    else if (containsSubstring(message, "#FEED")  && !isDawgpackTopic())
    {
      //! flag for whether feed will occur. it won't if a device is specified and it's not our device (unless super topic)
        boolean performFeed = true;
        //!check against the super feeder. If super feeder, then feed all devices, otherwise logic below
        if (!isSuperTopic())
        //if (!stringMatch(topic, "/usersP/bark") && !stringMatch(topic, "/usersP/dawgpack"))
        {
            //only check this if not the super feed topic "/usersP/bark" ..
            //TODO: not make this hardwired to /usersP/bark
            //TODO: 2.2.22
            //try 2.19.22 (but simple version..)
            //TODO: 7.23.22 .. look if the paired device  too..
            if (containsSubstring(message, "deviceName"))
            {
                //since deviceName specified, then only feed if our device is specified..
                if (containsSubstring(message, _deviceNameString))
                {
                    // this could mean a "deviceName" was found, and our _deviceNameString was there.,
                    // versus actually parsing for "deviceName":ourName
                    performFeed = true;
                }
                else if (getPreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE))
                {
                    //broke up the if since the get paired device could overright the boolean (or visa versa). Up to the compiler to know the order these are called.. unreliable
                    if (containsSubstring(message, getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING)))
                    {
                        SerialTemp.println(" *** Feeding via our gateway ***");
                        performFeed = true;
                    }
                    else
                    {
                        SerialDebug.println(" ** Not feeding as not our paired device either ***");
                        performFeed = false;
                    }

                }
                else
                {
                    //perform feed... (knowing the message device name requires pqrsing .. so not for now since containsSubstring() suffices..
                    SerialDebug.printf("**NOT Perform FEED as deviceName doesn't ours: %s\n", _deviceNameString );
                    
                    performFeed = false;
                }
            }
        }
        
        //if performFeed set, then continue..
        if (performFeed)
        {
            //!perform the feed
            performFeedMethod(&topic[0]);
            
            //!message already sent ...
            messageValidToSendBack = false;
        }
    }
#ifdef ESP_M5
    //!DOCFOLLOW .. support Dawgpack
    else if (containsSubstring(message, "#followMe") || containsSubstring(message,"#DOCFOLLOW"))
    {
        //! retrieves the last DocFollow SemanticMarker (from the message #DOCFOLLOW | #followMe {AVM=<SM>}
        //! need to parse to the AVM= grab the <SM> up to the "}"
        //!
        if (containsSubstring(message,"AVM="))
        {
            char *indexOfEqual = index(&message[0],'=');
            if (strlen(indexOfEqual)>2)
                //move past the =
                indexOfEqual++;
            strcpy(_lastDocFollowSemanticMarker,"");
            // loop until the } is found
            while (*indexOfEqual && *indexOfEqual !='}')
            {
                //copy a character at a time until the } (or nill)
                strncat(_lastDocFollowSemanticMarker, indexOfEqual,1);
                indexOfEqual++;
            }
            strcat(_lastDocFollowSemanticMarker, "\0");

            SerialDebug.printf("SemanticMarker: %s\n", _lastDocFollowSemanticMarker);
            //setLastDocFollowSemanticMarker(_lastDocFollowSemanticMarker);
            
            //!parse the #followMe {AVM=<url>}
            sprintf(_fullMessageOut,"#ACK {%s} {doc_follow=%s}", _deviceNameString, _lastDocFollowSemanticMarker);
        }
        else
        {
            sprintf(_fullMessageOut,"#ACK {%s} {bad_doc_follow syntax}", _deviceNameString);

        }
        messageValidToSendBack = true;
    }
    //!note: this might be candidate for wider use
    else if (containsSubstring(message, "#CAPTURE")  && !isDawgpackTopic())
    {
#ifdef ESP_M5_CAMERA
       // sprintf(_fullMessageOut, "#TAKING_PIC {%s} {real soon to be implemented 8.11.22}", _deviceNameString);
        takePicture_MainModule();

#else
#ifdef M5_CAPTURE_SCREEN
        saveScreen_SPIFFModule();
        //sprintf(_fullMessageOut, "#M5_SCREEN {%s} {capturing screen as bmp}", _deviceNameString);
#else
        //sprintf(_fullMessageOut, "#NO_CAN_CAMERA_CAPTURE {%s} {I am just a chip without a camera}", _deviceNameString);
#endif // ESP_M5
#endif //ESP_M5_CAMERA
        messageValidToSendBack = false;
    }
#endif // M5
    else if (containsSubstring(message, "#TEMP") && !isDawgpackTopic())
    {
#ifdef ESP_M5
        float temp = getTemperature_mainModule();
        //SYNTAX should evolve .. backward compatable ..
        sprintf(_fullMessageOut,"#ACK {%s} {TEMP} %2.0f F {'temp':'%2.0f'}", _deviceNameString, temp, temp);
#else
        sprintf(_fullMessageOut, "#NO_CAN_GET_TEMP {%s} {I am just a chip without a temp sensor}", _deviceNameString);
#endif
        //call the callback specified from the caller (eg. NimBLE_PetTutor_Server .. or others)
        callCallbackMain(CALLBACKS_MQTT, MQTT_CALLBACK_TEMP, messageString);

        messageValidToSendBack = true;
    }
    //!3.25.22 -- trying the CLEAN the ePROM SSID
    else if (containsSubstring(message, "#CLEAN_SSID_EPROM") && !isDawgpackTopic())
    {
        //! call the callback for cleaning the SSID eprom..
        callCallbackMain(CALLBACKS_MQTT, MQTT_CLEAN_SSID_EPROM, (char*)"cleanSSID_EPROM");
        
    }
    //!3.8.22 -- trying the OTA. IT WORKS!!!
    //!NOW: 2 versions, 3.28.22, try to parse {v:'version starts with.." ) -- or just contains substring. eg. #OTA  Version-1.6a ... and check against our "VERSION"
    else if (containsSubstring(message, "#OTA") && !isDawgpackTopic() && !isGroupTopic())
    {
        boolean performOTAUpdate = true;
        
        //syntax:  #OTA {v:VERSION}, or {k:ESP_32 or ESP_M5
        if (containsSubstring(message, "{v:"))
        {
            //does the installed "VERSION" == the string passed in eg. {v:OUR_VERSION} VERSION=OUR_VERSION
           // performOTAUpdate = containsSubstring(message, VERSION);
           performOTAUpdate = containsSubstring(message, VERSION);

            SerialDebug.printf("#OTA version correct: %d\n", performOTAUpdate);
        }
        else if (containsSubstring(message, "{k:") && !isDawgpackTopic())
        {
            //does the installed "KIND" == the string passed in eg. {v:ESP_32} we are one or the other..
#ifdef ESP_M5
            performOTAUpdate = containsSubstring(message, "ESP_M5");
            SerialDebug.printf("#OTA match ESP_M5: %d\n", performOTAUpdate);

#else
            performOTAUpdate = containsSubstring(message, "ESP_32");
            SerialDebug.printf("#OTA match ESP_32: %d\n", performOTAUpdate);

#endif

        }
       
        
        //parse out the {kind, host, binfile}
        //SOON .. this might be a triple click?? or keep the messaging?
        
        
        if (performOTAUpdate)
        {
            //NOTE: cannot put #OTA in message or it infinite loops..
            sprintf(_fullMessageOut, "over the air binary image update from version: %s", VERSION);
            //let clients know what's happening..
            _mqttClient.publish(_mqttTopicString, _fullMessageOut);
            SerialLots.printf("Sending message: %s\n", _fullMessageOut);
            //blink the light
            blinkBlueLightMQTT();

            //! dispatches a call to the command specified. This is run on the next loop()
            main_dispatchAsyncCommand(ASYNC_CALL_OTA_UPDATE);
        }
        else
        {
            sprintf(_fullMessageOut, "over the air NOT updating as not matching string: %s",
#ifdef ESP_M5
           "ESP_M5"
#else
           "ESP_32"
#endif
            );
            SerialLots.printf("Sending message: %s\n", _fullMessageOut);
            _mqttClient.publish(_mqttTopicString, _fullMessageOut);
            
        }

    } //#OTA
    else if (isDawgpackTopic())
    {
        SerialDebug.println("DAWGPACK unsupported message");
    }
    
    if (messageValidToSendBack)
    {
        //publish this message..
//        _mqttClient.publish(_mqttTopicString, _fullMessageOut);
        SerialLots.printf("1.Sending message: %s\n", _fullMessageOut);
#ifdef TRY_MORE_ASYNC_PROCESSING
        //publish back on topic
        publishMQTTMessage(_mqttTopicString, _fullMessageOut);
#else
        _mqttClient.publish(_mqttTopicString, _fullMessageOut);
#endif
        
        //! 5.21.22 WORKS!!
        //!topic is the topic we can in on.. so could be super user..
        //if (strcmp(&topic[0],"usersP/bark")==0)
        if (isSuperTopic() || isDawgpackTopic())
        {
            SerialTemp.println("2.Sending on DawgPack too..");
            //publish back on topic
           // _mqttClient.publish("usersP/dawgpack", _fullMessageOut);
#ifdef TRY_MORE_ASYNC_PROCESSING
            //publish back on topic
            publishMQTTMessage((char*)"usersP/dawgpack", _fullMessageOut);
#else
            _mqttClient.publish("usersP/dawgpack", _fullMessageOut);
#endif
        }
        
        //! 7.15.23 anniversery of 1799 finding Rosetta Stone in Egypt
        if (isGroupTopic())
        {
            SerialTemp.printf("3.Sending on %s too..\n", _lastGroupTopic);
#ifdef TRY_MORE_ASYNC_PROCESSING
            //publish back on topic
            publishMQTTMessage((char*)_lastGroupTopic, _fullMessageOut);
#else
            _mqttClient.publish(_lastGroupTopic, _fullMessageOut);
#endif
        }
    }
}


//! *********************** METHODS invoked from BLE (JSON) and MQTT messages ***************

//!perform the OTA update. This calls the OTAImageUpdate methods (via preformOTAUpdateSimple())
void performOTAUpdateMethod()
{
    //NOTE: cannot put #OTA in message or it infinite loops..
    sprintf(_fullMessageOut, "over the air binary image update, replacing our version: %s", VERSION);
    //let clients know what's happening..
    //   _mqttClient.publish(_mqttTopicString, _fullMessageOut);
    SerialDebug.printf("%s\n", _fullMessageOut);
    //blink the light
    blinkBlueLightMQTT();
    //#define REALLY_DO_IT_BOMBS
#ifdef REALLY_DO_IT_BOMBS
    //NOTE: this sync call doesn't work..
    //printf(" *** Bad syntax, no { } \n");
    performOTAUpdateSimple();
    //this reboots .. so the code below never runs anyway.. will work on it..
#else
    // setAsyncCallOTAUpdate(true);
    //! dispatches a call to the command specified. This is run on the next loop()
    main_dispatchAsyncCommand(ASYNC_CALL_OTA_UPDATE);
#endif
}

//!calls the method for cleaning the SSID eprom. This calls the WIFI_APModule callback
void cleanSSID_EPROM_Method()
{
    //! call the callback for cleaning the SSID eprom..
    callCallbackMain(CALLBACKS_MQTT, MQTT_CLEAN_SSID_EPROM, (char*)"cleanSSID_EPROM");
}

//! //!calls the FEED message via the callback (which calls the BLE code)
//!NOTE: this will send a BLE command if connected via the GATEWAY to a GEN3 (or other gateway in the future)
//!NOTE: This sends the _full message on the topic ..
void performFeedMethod(char *topic)
{
    //!get the temperature
    float temp = getTemperature_mainModule();
    
    //!get the connected status
    //!save some part of this message for later display by SemanticMarker 8.4.22
    //!set the feed
    setLastMessageStatus((char*)"feed");
    
    //perform feed...
    SerialDebug.println("Perform FEED internally, calling callbackFunction.. 2");
    //call the callback specified from the caller (eg. NimBLE_PetTutor_Server .. or others)
    // (*_callbackFunction)(rxValue);
    callCallbackMain(CALLBACKS_MQTT, MQTT_CALLBACK_FEED, (char*)"feed");
    
    //ASYNC_SEND_MQTT_FEED_MESSAGE
    //On demand #STATUS send the statusURL as well (if an M5)
    //this queues the sending of the StatusURL over MQTT.
    // This is async (next loop) since sending 2 MQTT messages can be hard to do in a row ..
    //main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
    
    //SerialTemp.println(" ** returned from ASYNC_SEND_MQTT_FEED_MESSAGE ***");
#define ACK_FOR_PAIR_TOO
    char pairedDevice[100];
    if (isValidPairedDevice_mainModule())
        strcpy(pairedDevice,getPairedDevice_mainModule());
    else
        strcpy(pairedDevice,"none");
    
#ifdef USE_BLE_CLIENT_NETWORKING
    boolean isConnectedBLE = isConnectedBLEClient();
#else
    boolean isConnectedBLE = false;
#endif
    boolean isGateway = getPreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE);
    
    if (isConnectedBLE && isGateway)
    {
        //! FOR NOW , copy the code and create a _fullMessageOut that is for the Paired device...
        SerialTemp.print("PairedDevice: ");
        SerialTemp.println(pairedDevice);
        sprintf(_fullMessageOut, "%s {%s} {'T':'%d','temp':'%2.0f','topic':'%s','user':'%s','v':'%s','location':'%s'}", ACK_FEED, pairedDevice, time, temp, &topic[0],_mqttUserString, VERSION_SHORT, _jsonLocationString?_jsonLocationString:"somewhere");
        
        //publish this message..
//        _mqttClient.publish(_mqttTopicString, _fullMessageOut);
//        SerialTemp.printf("ACK: Sending message: %s\n", _fullMessageOut);
#ifdef TRY_MORE_ASYNC_PROCESSING
        //publish back on topic
        publishMQTTMessage(_mqttTopicString, _fullMessageOut);
#else
        _mqttClient.publish(_mqttTopicString, _fullMessageOut);
#endif
        //! 5.21.22 WORKS!!
        //!topic is the topic we can in on.. so could be super user..
       // if (strcmp(&topic[0],"usersP/bark")==0)
        if (isSuperTopic() || isDawgpackTopic())
        {
//            SerialLots.println("Sending on DawgPack too..");
//            //publish back on topic
//            _mqttClient.publish("usersP/dawgpack", _fullMessageOut);
#ifdef TRY_MORE_ASYNC_PROCESSING
            //publish back on topic
            publishMQTTMessage((char*)"usersP/dawgpack", _fullMessageOut);
#else
            _mqttClient.publish("usersP/dawgpack", _fullMessageOut);
#endif
        }
        
        //! 7.15.23 anniversery of 1799 finding Rosetta Stone in Egypt
        if (isGroupTopic())
        {
            SerialTemp.printf("3.Sending on %s too..\n", _lastGroupTopic);
#ifdef TRY_MORE_ASYNC_PROCESSING
            //publish back on topic
            publishMQTTMessage((char*)_lastGroupTopic, _fullMessageOut);
#else
            _mqttClient.publish(_lastGroupTopic, _fullMessageOut);
#endif
        }
        
    }
    //! output the main #actMe message .. but it get's nothing from the plugins - like ATOM status
    sprintf(_fullMessageOut, "%s {%s} {'T':'%d','temp':'%2.0f','topic':'%s','user':'%s','v':'%s','location':'%s','paired':'%s', 'ble':'%s','connected':'%s','gateway':'%s'", ACK_FEED, _deviceNameString, time, temp, &topic[0]?&topic[0]:"NULL",_mqttUserString, VERSION_SHORT, _jsonLocationString?_jsonLocationString:"somewhere",pairedDevice, isConnectedBLE?"c":"x", connectedBLEDeviceName_mainModule()?connectedBLEDeviceName_mainModule():"none", isGateway?"on":"off");
    
    // send the FEED to the display (if any)
    addToTextMessages_displayModule("FEED");
    
#define TRY_MORE_URL
#ifdef  TRY_MORE_URL
    char *moreStatus = main_currentStatusJSON();
    if (moreStatus && strlen(moreStatus) > 0)
    {
        strcat(_fullMessageOut, (char*)",");
        
        //Make URL for the status..
        strcat(_fullMessageOut, moreStatus);
    }
    
#endif
    //! close the JSON message
    strcat(_fullMessageOut, (char*)"}");

    //if (messageValidToSendBack)
    if (true)
    {
        //publish this message..
//        _mqttClient.publish(_mqttTopicString, _fullMessageOut);
//        SerialDebug.printf("Sending message: %s\n", _fullMessageOut);
#ifdef TRY_MORE_ASYNC_PROCESSING
        //publish back on topic
        publishMQTTMessage(_mqttTopicString, _fullMessageOut);
#else
        _mqttClient.publish(_mqttTopicString, _fullMessageOut);
#endif
        //! 5.21.22 WORKS!!
        //!topic is the topic we can in on.. so could be super user..
       // if (strcmp(&topic[0],"usersP/bark")==0)
        if (isSuperTopic() || isDawgpackTopic())

        {
//            SerialTemp.println("Sending on DawgPack too..");
//            //publish back on topic
//            _mqttClient.publish("usersP/dawgpack", _fullMessageOut);
#ifdef TRY_MORE_ASYNC_PROCESSING
            //publish back on topic
            publishMQTTMessage((char*)"usersP/dawgpack", _fullMessageOut);
#else
            _mqttClient.publish("usersP/dawgpack", _fullMessageOut);
#endif
        }
        
        //! 7.15.23 anniversery of 1799 finding Rosetta Stone in Egypt
        if (isGroupTopic())
        {
            SerialTemp.printf("3.Sending on %s too..\n", _lastGroupTopic);
#ifdef TRY_MORE_ASYNC_PROCESSING
            //publish back on topic
            publishMQTTMessage((char*)_lastGroupTopic, _fullMessageOut);
#else
            _mqttClient.publish(_lastGroupTopic, _fullMessageOut);
#endif
        }
    }
}


//! *********************** END METHODS invoked from BLE (JSON) and MQTT messages ***************


//!This is in case corruption when changing what's written.. defining BOOTSTRAP will clean up the EPROM

//!read the eprom..
void MQTTModule_readPreferences()
{
    SerialDebug.println("MQTT.readPreferences");
     //#define BOOTSTRAP
#ifdef BOOTSTRAP
    //note: this could be a 'rebootstrap' message via MQTT .. in the future..
    {
        SerialDebug.println("BOOTSTRAP device with our own WIFI and MQTT");
        
        char* BOOT_mqtt_server = (char*)"iDogWatch.com";
        
        //example with "test" as the user name. Change, ssid, user, pass, device name and topic
        char* BOOT_mqtt_port = (char*)"1883";
        char* BOOT_ssid = (char*)"SunnyWhiteriver";
        char* BOOT_ssid_password = (char*)"sunny2021";
        char *BOOT_mqtt_user = (char*)"test";
        char *BOOT_mqtt_password = (char*)"test";
        char *BOOT_mqtt_guestPassword = (char*)"test";

        //new 2.2.22 (last time this century..)
        //change over to new MQTT Namespace: usersP/bark
        char *BOOT_mqtt_topic = (char*)"usersP/bark/test";
        
        char *BOOT_deviceName = (char*)"name-of-feeder";
        char *BOOT_uuidString = (char*)"unused";
        char *BOOT_jsonHeaderString = (char*)"WIFI+MQTT";
        char *BOOT_jsonVersionString = (char*)"BOOTSTRAP 1.3";
        char *BOOT_jsonLocationString = (char*)"PetLand";  //enter something is you like (Seattle, WA)
        
        ///note: these createCopy are to get between String and char* .. probably a better way like  &BOOT[0] or something..
        _ssidString = createCopy(BOOT_ssid);

        _ssidPasswordString = createCopy(BOOT_ssid_password);
    
        _mqttServerString = createCopy(BOOT_mqtt_server);
        _mqttPortString = createCopy(BOOT_mqtt_port);
        _mqttPasswordString = createCopy(BOOT_mqtt_password);
        _mqttGuestPasswordString = createCopy(BOOT_mqtt_guestPassword);
        _mqttUserString = createCopy(BOOT_mqtt_user);
        _mqttTopicString = createCopy(BOOT_mqtt_topic);
        _deviceNameString = createCopy(BOOT_deviceName);
        _uuidString = createCopy(BOOT_uuidString);
        _jsonHeaderString = createCopy(BOOT_jsonHeaderString);
        _jsonVersionString = createCopy(BOOT_jsonVersionString);
        _jsonLocationString = createCopy(BOOT_jsonLocationString);
        
        DynamicJsonDocument myObject(1024);
        
        myObject["ssid"] = BOOT_ssid;
        myObject["ssidPassword"] = BOOT_ssid_password;
        myObject["mqtt_server"] = BOOT_mqtt_server;
        myObject["mqtt_port"] = BOOT_mqtt_port;
        myObject["mqtt_password"] = BOOT_mqtt_password;
        myObject["mqtt_guestPassword"] = BOOT_mqtt_guestPassword;

        myObject["mqtt_user"] = BOOT_mqtt_user;
        myObject["mqtt_topic"] = BOOT_mqtt_topic;
        myObject["deviceName"] = BOOT_deviceName;
        myObject["uuid"] = BOOT_uuidString;
        myObject["jsonHeader"] = BOOT_jsonHeaderString;
        myObject["jsonVersion"] = BOOT_jsonVersionString;
        myObject["location"] = BOOT_jsonLocationString;
        
        //open the preferences
        
        _preferencesMQTTNetworking.begin(ESP_EPROM_NAME, false);  //readwrite..
        _preferencesMQTTNetworking.clear();
        //output our object.. myObject has a string version..
        SerialDebug.print("Writing EPROM JSON = ");
        //JSON
        String output1;
        serializeJson(myObject, output1);
        SerialDebug.println(output1);
        _preferencesMQTTNetworking.putString(_preferencesJSONName, output1);
        
        // Close the Preferences
        _preferencesMQTTNetworking.end();
        
        //new 2.21.22  (On bootstrap, it's nil..?? .. maybe the myObject isn't a string??
        //TRY: reading back..
        _preferencesMQTTNetworking.begin(ESP_EPROM_NAME, false);  //false=read/write..
        _fullJSONString = _preferencesMQTTNetworking.getString(_preferencesJSONName);
        SerialDebug.print("Reading.3 EPROM JSON = ");
        SerialDebug.println(_fullJSONString);
        
        //check ... _fullMessageOut
        // Close the Preferences
        _preferencesMQTTNetworking.end();
        
        //end new
    }
    return;
#endif //BOOTSTRAP
    
#ifdef BOOTSTRAP_AP_MODE_STARTUP
    SerialDebug.println("*** STARTUP in AP MODE ***");
    _ssidString = NULL;
    _ssidPasswordString = NULL;
    return;
#endif
    //https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/
    //https://github.com/espressif/arduino-esp32/blob/master/libraries/Preferences/src/Preferences.cpp
    // Open Preferences with my-app namespace. Each application module, library, etc
    // has to use a namespace name to prevent key name collisions. We will open storage in
    // RW-mode (second parameter has to be false).
    // Note: Namespace name is limited to 15 chars.
    _preferencesMQTTNetworking.begin(ESP_EPROM_NAME, false);  //false=read/write..
    _fullJSONString = _preferencesMQTTNetworking.getString(_preferencesJSONName);
    SerialDebug.print("Reading.1 EPROM JSON = ");
    SerialDebug.println(_fullJSONString);
    
    //check ... _fullMessageOut
    // Close the Preferences
    _preferencesMQTTNetworking.end();
    
    //3.29.22:  ISSUE. the eprom wasn't written, but we can in from the CREDENTIALS...
    
    //first time there won't be any eprom info.. THIS is a bug without this code update
    if (!_fullJSONString || _fullJSONString.length() == 0)
    {
        SerialDebug.println("*** no JSON in preferences, probably first time. use Bootstrap, or BLE update ***");
        _ssidString = NULL;
        _ssidPasswordString = NULL;
        return;
    }
    
    DynamicJsonDocument myObject(1024);
    //StaticJsonDocument myObject(1024);
    deserializeJson(myObject, _fullJSONString);
    SerialDebug.print("JSON parsed.1 = ");
    String output1;
    serializeJson(myObject, output1);
    SerialDebug.println(output1);
    
#ifdef NOT_ORIGINAL
    //defaults:  9.19.23  is already set don't override..
    if (!_deviceNameString || (_deviceNameString && strlen(_deviceNameString)==0))
        _deviceNameString = (char*)"Unnamed";
#else
    //defaults:
    _deviceNameString = (char*)"Unnamed";
#endif
    //parse
    const char* a1 = myObject["ssid"];
#ifdef NOT_ORIGINAL
    if (a1 && strlen(a1)>0)   /// CHANGED 9.19.23
#else
    if (a1)
#endif
    {
        _ssidString = const_cast<char*>(a1);
        _ssidString = createCopy(_ssidString);
        SerialDebug.println(_ssidString);
        
    }
    else
    {
        _ssidString = NULL;
        SerialDebug.println("ssid == NULL");
    }
    if (!_ssidString)
    {
        
        SerialDebug.println("No SSID set, try BLE update again.. ");
        _ssidString = NULL;
        _ssidPasswordString = NULL;
        _mqttServerString = NULL;
        _mqttPortString = NULL;
        _mqttPasswordString = NULL;
        _mqttGuestPasswordString = NULL;
        _mqttUserString = NULL;
        _mqttTopicString = NULL;
#ifdef NOT_ORIGINAL
      //  _deviceNameString = (char*)"Unnamed";    /// CHANGED 9.19.23
#else
        _deviceNameString = (char*)"Unnamed";
#endif
        _uuidString = NULL;
        _jsonHeaderString = NULL;
        _jsonVersionString = NULL;
        _jsonLocationString = NULL;
        
        //call the callback specified from the caller (eg. NimBLE_PetTutor_Server .. or others)
        // (*_callbackFunction)(rxValue);
        callCallbackMain(CALLBACKS_MQTT, MQTT_CALLBACK_SOLID_LIGHT, (char*)"solidLight");
        
        return;
        
    }
    //!seems the JSON object only returns these const char*, and not easy to just create a char *, so they are created in their own memory..
    {
        const char* a2 = myObject["ssidPassword"];
        if (a2)
        {
            _ssidPasswordString = const_cast<char*>(a2);
            _ssidPasswordString = createCopy(_ssidPasswordString);
            SerialDebug.println(_ssidPasswordString);
        }
        else
            _ssidPasswordString = NULL;
    }
    
    {
        //!the MQTT host/port/user/password  (topic is created in this code...)
        const char* a3 = myObject["mqtt_server"];
        if (a3)
        {
            _mqttServerString = const_cast<char*>(a3);
            _mqttServerString = createCopy(_mqttServerString);
            SerialDebug.println(_mqttServerString);
        }
        else
            _mqttServerString = NULL;
    }
    
    {
        const char* a4 = myObject["mqtt_port"];
        if (a4)
        {
            _mqttPortString = const_cast<char*>(a4);
            _mqttPortString = createCopy(_mqttPortString);
        }
        else
            _mqttPortString = NULL;
    }
    
    {
        const char* a5 = myObject["mqtt_password"];
        if (a5)
        {
            _mqttPasswordString = const_cast<char*>(a5);
            _mqttPasswordString = createCopy(_mqttPasswordString);
            SerialDebug.println(_mqttPasswordString);
        }
        else
            _mqttPasswordString = NULL;
    }
    
    {
        const char* a6 = myObject["mqtt_user"];
        if (a6)
        {
            _mqttUserString = const_cast<char*>(a6);
            _mqttUserString = createCopy(_mqttUserString);
        }
        else
            _mqttUserString = NULL;
    }
    
    {
        const char* a7 = myObject["deviceName"];
        if (a7)
        {
            _deviceNameString = const_cast<char*>(a7);
            _deviceNameString = createCopy(_deviceNameString);
        }
        else
            _deviceNameString = NULL;
    }
    
    //update the chip name with the deviceName
    getChipInfo();
    
    SerialDebug.println(_deviceNameString);
    
    {
        const char* a8 = myObject["uuid"];
        if (a8)
        {
            _uuidString = const_cast<char*>(a8);
            _uuidString = createCopy(_uuidString);
            SerialDebug.print("UUID: ");
            SerialDebug.println(_uuidString);

        }
        else
            _uuidString = NULL;
    }
    
    {
        const char* a9 = myObject["mqtt_topic"];
        if (a9)
        { _mqttTopicString = const_cast<char*>(a9);
            _mqttTopicString = createCopy(_mqttTopicString);
            SerialDebug.println(_mqttTopicString);
        }
        else
            _mqttTopicString = NULL;
    }
    
    {
        const char* a10 = myObject["jsonHeader"];
        if (a10)
        { _jsonHeaderString = const_cast<char*>(a10);
            _jsonHeaderString = createCopy(_jsonHeaderString);
            SerialDebug.println(_jsonHeaderString);
        }
        else
            _jsonHeaderString = NULL;
    }
    
    {
        //!Note: This is where the code could look for backward compatability, etc..
        const char* a11 = myObject["jsonVersion"];
        if (a11)
        { _jsonVersionString = const_cast<char*>(a11);
            _jsonVersionString = createCopy(_jsonVersionString);
            SerialDebug.println(_jsonVersionString);
        }
        else
            _jsonVersionString = NULL;
    }
    
    {
        const char* a12 = myObject["location"];
        if (a12)
        { _jsonLocationString = const_cast<char*>(a12);
            _jsonLocationString = createCopy(_jsonLocationString);
            SerialDebug.println(_jsonLocationString);
        }
        else
            _jsonLocationString = NULL;
    }
    
    {
        const char* a13 = myObject["mqtt_guestPassword"];
        if (a13)
        {
            _mqttGuestPasswordString = const_cast<char*>(a13);
            _mqttGuestPasswordString = createCopy(_mqttGuestPasswordString);
            SerialDebug.println(_mqttGuestPasswordString);
        }
        else
            _mqttGuestPasswordString = NULL;
    }
    
    //! sets the MQTT user/password. It's up to the code to decide who needs to know (currently saves in the WIFI_APModule
    main_updateMQTTInfo(_ssidString, _ssidPasswordString, _mqttUserString, _mqttPasswordString, _mqttGuestPasswordString, _deviceNameString, _mqttServerString, _mqttPortString, _jsonLocationString);
}



//!whether the string is TRUE, ON, 1
boolean isTrueString(String valCmdString)
{
    return  valCmdString.equalsIgnoreCase("on") ||
    valCmdString.equalsIgnoreCase("1") ||
    valCmdString.equalsIgnoreCase("true");
}
#ifdef UNUSED
//!whether the string is FALSE, OFF, 0
function isFalseString(String valCmdString)
{
    return  valCmdString.equalsIgnoreCase("off") ||
            valCmdString.equalsIgnoreCase("0") ||
            valCmdString.equalsIgnoreCase("false");
}
#endif

//!send message to ourself to change to current specifed SM Mode
void invokeCurrentSMModePage(char *topic)
{
    //!send message to ourself to process the current mode..
    sprintf(_fullMessageOut,"{'cmd':'%s'}",charSMMode_mainModule(getCurrentSMMode_mainModule()));
    processJSONMessageMQTT(_fullMessageOut, topic);
}

//!process the JSON message (looking for FEED, etc). Note: topic can be nil, or if not, it's an MQTT topic (so send replies if you want)
boolean processJSONMessageMQTT(char *ascii, char *topic)
{
    SerialLots.println(" *** processJSONMessageMQTT ***");
    //! use the default user topic if not specified...
    if (!topic)
    {
        if (!_mqttTopicString)
            _mqttTopicString = (char*)"usersP/bark/test";
        topic = _mqttTopicString;
    }
    classifyTopic(topic);
    
    if (!ascii)
        return false;
    
    //! 7.26.23 don't process if a group message and FLAG not set
    if (_MQTTMessageTopicType == groupTopic)
    {
        // if the EPROM says not to process groups .. then skip this message..
        //! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
        if (! getPreferenceBoolean_mainModule(PREFERENCE_SUPPORT_GROUPS_SETTING))
        {
            SerialDebug.printf("NOT Processing as PREFERENCE_SUPPORT_GROUPS_SETTING not set");
            return false;
        }
    }
    
    //!empty the status for the last message. Then various places the feed or status, etc are set
    //emptyLastMessageStatus();
    //cant empty here .. as the ACK gets sent .. how about after the ACK!
    
    //!Basically processing as a JSON if the "{" is somewhere.. could still be invalid code
    if (!startsWithChar(ascii,'{'))
    {
        SerialLots.printf("processJSONMessageMQTT(%s) -> return false, not JSON\n", ascii);
        
        return false;
    }
    
    SerialDebug.printf("processJSONMessageMQTT: '%s'\n", ascii);

    // Deserialize the JSON document, then store the ascii in the EPROM (if it parses..)
    
    SerialLots.printf("Ascii before deserializeJson: %s\n", ascii);
    
#ifdef PROCESS_SMART_BUTTON_JSON
    DynamicJsonDocument myObject(2024);
#else
    DynamicJsonDocument myObject(1024);
#endif

    deserializeJson(myObject, ascii);
    serializeJsonPretty(myObject, Serial);

    //NOTE: the ascii is now corrupted...
    SerialDebug.print("JSON parsed = ");
    // String output;
    String output1;
    serializeJson(myObject, output1);
    SerialDebug.println(output1);
    SerialLots.printf("Ascii after deserializeJson: %s\n", ascii);
    
    //NEW: 3.28.22 {'cmd':COMMANDS}
    // {'sm':<sm>}
    // {'guest':<passws>
    //{ 'set':<object>,'val':<value>}
    const char* cmd = myObject["cmd"];
    const char* semanticMarkerCmd = myObject["sm"];
    const char* guestCmd = myObject["guest"];

    //! 9.18.23 add this .. so it doesnt' fall through
    const char *set64Cmd = myObject["set64"];

    const char *setCmd = myObject["set"];
    const char *valCmd = myObject["val"];
    //new:  'send':<request> eg. status|temp
    const char *sendCmd = myObject["send"];
    //! devName is if a dev=<NAME> was specified, then dissregard if not our device
    const char *devName = myObject["dev"];
    //if processCommands, then do NOT process the credentials..
    boolean processCommands = false;
    if (cmd || semanticMarkerCmd || guestCmd || setCmd || sendCmd || set64Cmd)
        processCommands = true;
    
#ifdef PROCESS_SMART_BUTTON_JSON
    //! 7,1.23 Dads 92'n birthday
//!NOT DOING THESE RIGHT NOW.. THE REST https is being done by ATOM
    //const char *SMARTButton = myObject["SMARTButton"];
    boolean processSMARTButton = containsSubstring(output1,"SMARTButton"); //myObject["SMARTButton"] != NULL;
    SerialTemp.printf("SMARTButton = %d\n", processSMARTButton);
    //! set processCommands just so it process the 'dev' argument..
    if (processSMARTButton)
        processCommands = true;
#endif
    
    //try 5.12.22 {'set':'item'},{'val':'value'}
    //   eg. set:hightemp, val:80
    
    //Find the Guest Password, and the user name - or defaults if notset
    String guestPassword = "pettutor";
    if (_mqttGuestPasswordString)
    {
        //in case empty (but not null) - not checking for spaces only... too lazy
        if (strlen(_mqttGuestPasswordString)>0)
            guestPassword = _mqttGuestPasswordString;
    }
    // set vals to NOTSET if not set
    char* baseString;
    String title = "";
    if (!_mqttUserString)
    {
        _mqttUserString = NOTSET_STRING;
    }
    if (!_mqttPasswordString)
    {
        _mqttPasswordString = NOTSET_STRING;
    }
 
    //!this is to ensure that the credentials are not processed..
    //! there is a return 'true' after processing commands
    if (processCommands)
    {
        //! for dawgPack, only support the DOCFOLLOW message for now 8.19.22
        if (isDawgpackTopic())
        {
            char* setCmdString  = const_cast<char*>(setCmd);
            char* valCmdString = const_cast<char*>(valCmd);

            if (setCmd && strcasecmp(setCmd,"semanticMarker")==0)
            {
                SerialDebug.printf("DAWGPACK supported message: %s\n", setCmd);
            }
            else
            {
                SerialDebug.println("DAWGPACK unsupported message");
                return true;
            }
            //fall through for supported messaages..
        }
        
        //TODO: For an M5 (with a name) .. and it's paired with a GEN3 and GATEWAY, then the feed of the GEN3 name (instead of M5 name) should be supported...
        //! a couple commands, like bleserveron require a device name to be specified (so everyone listening doesn't perform operation)
        boolean deviceNameSpecified = devName != NULL;
        //!if a dev:<dev> is specified, then only process if the device name is that same
        boolean processMessageOrGateway = true;
        if (deviceNameSpecified)
        {
            //default we only feed if our device or our gateway..
            processMessageOrGateway = false;
#ifdef WILDCARD_DEVICE_NAME_SUPPORT
            //!parses a line of text, The caller then uses queryMatchesName() to see if their name matches
            parseQueryLine_mainModule((char*)devName);
            if (queryMatchesName_mainModule(_deviceNameString))
            {
                SerialTemp.printf("Query: %s *** Matches our dev name: %s\n", devName, _deviceNameString);
                processMessageOrGateway = true;
            }
#else
            //!If the dev name is specified, and our device is that name .. then good
            if (devName && strcmp(devName,_deviceNameString) == 0)
            {
                SerialTemp.println(" .. our own device ..");
                processMessageOrGateway = true;
            }
#endif

            //!we are in gateway mode, and the paired device isn't ours..
            if (devName
                && getPreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE))
            {
#ifdef WILDCARD_DEVICE_NAME_SUPPORT
                if (queryMatchesName_mainModule(getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING)))
                {
                    SerialTemp.printf("Query: %s *** Matches our paired dev name: %s\n", devName, getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING));
                    processMessageOrGateway = true;
                }
#else
                if (strcmp(devName,getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING)) == 0)
                {
                    SerialTemp.println(" .. our paired device ..");
                    processMessageOrGateway = true;
                }
#endif
            }
            
        }
     
        //! basically, processMessageOrGateway is set to TRUE if the device isn't mentioned, OR
        //! the dev is mentioned, and the wildcard works (or is paired)
        //! In either case, the deviceNameSpecified will be true if "dev" was specified (even with wildcard)
        //! So code below that only work if "dev" specified will work and know it's their device
        //! For the SemanticMarker, if the onlyDevSM==true, then look for deviceNameSpecified
        
#ifdef PROCESS_SMART_BUTTON_JSON  //no 9.28.23
        //! early attempt to process a SMART buttons JSON as a stored procedure.
        //! But currently the JSON parser is bombing on the JSON provided..
        //! 7.1.23 Dads 92nd birthday
        if (processMessageOrGateway && processSMARTButton)
        {
            StaticJsonDocument<2024>  smartButtonObject;

            deserializeJson(smartButtonObject, myObject["SMARTButton"]);

            return processJSONSMARTButton(smartButtonObject);
        }
#endif
        //! after this, if true, then ifDeviceNameSpecified .. then it's a good name..
        if (!processMessageOrGateway)
        {
            //Only gets here if dev set .. so the parser was run (if WILDCARD)
            SerialTemp.print("Disregard as Device Specified: ");
            SerialTemp.print(devName);
            
            //! as per issue #122, if a device is in gateway mode, and paired with a device name specified, then the message can be sent
            SerialTemp.print(" not ours: ");
            SerialTemp.println(_deviceNameString);
            if (getPreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE))
            {
#ifdef WILDCARD_DEVICE_NAME_SUPPORT
                if (!queryMatchesName_mainModule(getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING)))
#else
                if (strcmp(devName,getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING)) != 0)
#endif
                {
                    SerialTemp.print(" .. And not paired device:");
                    SerialTemp.println(getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING));
                }
            }
        }
        else if (cmd)
        {
            
            //SerialDebug.printf("BLE CMD = '%s'\n", cmd);
            //note: we could have a mode for "testing" .. so the OTA for example does't fire.
            //smN
            //NOTE: THIS REQUIRES == 0 or no work..
            //NOTE: the number here has to be updated in the ButtonProcessing code too..
            int whichSMMode = whichSMMode_mainModule((char*)cmd);
            SerialTemp.printf("MQTT or BLE CMD = '%s'\n", cmd);

            // -1 if none..
            if (whichSMMode >= 0)
            {
                //! per #206 .. only change the page when not in doc_follow
                //! 11.9.22
                int currentSMMode = getCurrentSMMode_mainModule();
                SerialDebug.printf("currentSMMode = %d whichSMMode = %d\n", currentSMMode, whichSMMode);
                if (currentSMMode == SM_doc_follow)
                {
                    //! Issue: #222 for #206, this sets the current mode to SM_doc_follow, but
                    //! when at that page in the the current mode (which is now SM_doc_follow) won't let
                    //! the page go somewhere else (except in this case we are the same page). Only but a
                    //! physical button click.
                    //! I THINK THE ANSWER: if current and next are the same an SM_doc_follow, then do the page change..
                    if (currentSMMode != whichSMMode)
                    {
                        SerialDebug.println(" *** Not changing page as in DOCFOLLOW mode ***");
                        return true;
                    }
                    else
                        SerialDebug.println(" *** SM_doc_follow and changing to the same page ***");
                }

                //set the global SMMode.. NOTE: if greater than the MAX change mode to NON MIN
                setCurrentSMMode_mainModule(whichSMMode);

                boolean markerAlreadyShown = false;
                switch (whichSMMode)
                {
                        //since the TITLE is used by the display (it doesn't use the SMMode)
                    case SM_home_simple: //TILT
                    {
                        //new 7.25.22
                        baseString = (char*)"https://iDogWatch.com/bot/guestpage2";
                        title = "MINI CLICKER";
                        sprintf(_semanticMarkerString,"%s/%s/%s", baseString, _mqttUserString?_mqttUserString:"NULL", guestPassword?guestPassword:"NULL");
                    }
                        break;
                    case SM_home_simple_1: // BUZZER
                    {
                        //NOTE: THIS title is a binding/linking to the DisplayModule (as it looks for the title)
                        //new 7.25.22
                        baseString = (char*)"https://iDogWatch.com/bot/guestpage2";
                        title = "MINI-1";
                        sprintf(_semanticMarkerString,"%s/%s/%s", baseString, _mqttUserString?_mqttUserString:"NULL", guestPassword?guestPassword:"NULL");
                    }
                        break;
                    case SM_home_simple_2:  //FEED
                    {
                        //new 7.25.22
                        baseString = (char*)"https://iDogWatch.com/bot/guestpage2";
                        title = "MINI-2";
                        sprintf(_semanticMarkerString,"%s/%s/%s", baseString, _mqttUserString?_mqttUserString:"NULL", guestPassword?guestPassword:"NULL");
                    }
                        break;
                    case SM_home_simple_3:  //EXPERT
                    {
                        //new 7.25.22
                        baseString = (char*)"https://iDogWatch.com/bot/guestpage2";
                        title = "MINI-3";
                        sprintf(_semanticMarkerString,"%s/%s/%s", baseString, _mqttUserString?_mqttUserString:"NULL", guestPassword?guestPassword:"NULL");
                    }
                        break;
                        //! the 4th page, start of smart clicker
                    case SM_smart_clicker_homepage:
                    {
                        //Make URL for the status..
                        char *statusString = currentMessageStatusURL();
                        
                        //!create the SemanticMarker address
                        sprintf(_semanticMarkerString,"%s/%s/%s/%s", "https://SemanticMarker.org/bot/sensor", _mqttUserString?_mqttUserString:"NULL", guestPassword?guestPassword:"NULL", statusString?statusString:"NULL");
                        
                        title = "WIFI FEED";

                        //TODO.. use the String (*getStatusFunc)(void))  to re-create this..
                        //TODO: get the guest info.. or they send us the guest password in a message.. for next time..
                        //SerialDebug.print("SemanticMarker: ");
                        SerialLots.println(_semanticMarkerString);
                        //MAYBE save the user web page.. somewhere EPROM
                        
                        //!call the displayModuleFunc passing our dynamic status fund
                        showSemanticMarkerFunc_displayModule(_semanticMarkerString, title, &getDynamicMessageFunc);
                        markerAlreadyShown = true;
                    }
                        
                        break;
                    case SM_status:
                    {
                        //Make URL for the status..
                        char *statusString = main_currentStatusURL(true);
                        
                        //!create the SemanticMarker address
                        sprintf(_semanticMarkerString,"%s/%s/%s/%s", "https://SemanticMarker.org/bot/sensor", _mqttUserString?_mqttUserString:"NULL", guestPassword?guestPassword:"NULL", statusString?statusString:"NULL");

                        //!tack on the device name..
                        sprintf(_fullMessageOut,"Status %s", getDeviceNameMQTT());
                        //title = _fullMessageOut;
                        
                        //TODO.. use the String (*getStatusFunc)(void))  to re-create this..
                        //TODO: get the guest info.. or they send us the guest password in a message.. for next time..
                        //SerialDebug.print("SemanticMarker: ");
                        SerialLots.println(_semanticMarkerString);
                        //MAYBE save the user web page.. somewhere EPROM
                        
                        //!call the displayModuleFunc passing our dynamic status fund
                        showSemanticMarkerFunc_displayModule(_semanticMarkerString, _fullMessageOut, &getDynamicStatusFunc);
                        markerAlreadyShown = true;
                    }
                        break;
                        
                    case SM_guest_page:
                        
                    {
                        baseString = (char*)"https://iDogWatch.com/bot/guestpage";
                        title = "Guest Page";
                        sprintf(_semanticMarkerString,"%s/%s/%s", baseString, _mqttUserString?_mqttUserString:"NULL", guestPassword?guestPassword:"NULL");
                    }
                        break;
                    case SM_guest_feed:
                    {
                        baseString = (char*)"https://iDogWatch.com/bot/feedguest";
                        title = "Feed Guest";
                        sprintf(_semanticMarkerString,"%s/%s/%s", baseString, _mqttUserString?_mqttUserString:"NULL", guestPassword?guestPassword:"NULL");
                    }
                        break;
                    case SM_pair_dev:
                    {
                        baseString = (char*)"https://iDogWatch.com/bot/feedguestdevice";
                        char *pairDev = getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING);
                        //!NOTE this could be "NONE" the "P:" is so the display knows this is a paired device command
                        title = "P:";
                        sprintf(_semanticMarkerString,"%s/%s/%s/%s", baseString, _mqttUserString?_mqttUserString:"NULL", guestPassword?guestPassword:"NULL", pairDev);
                    }
                        break;
                    case SM_WIFI_ssid:
                    {
                        title = "WIFI";

                        //!#issue 136 create a SM for the WIFI syntax
                        //!WIFI:S:<SSID>;T:<WEP|WPA|blank>;P:<PASSWORD>;H:<true|false|blank>;
                        sprintf(_semanticMarkerString,"WIFI:S:%s;T:;P:%s;H:;", _ssidString?_ssidString:"NONE", _ssidPasswordString?_ssidPasswordString:"");
                    }
                        break;
                        
                        // different SM
                    case SM_ap_mode:
                    {
                        //AP mode..
                        sprintf(_semanticMarkerString,"%s", "http://192.168.4.1");
                        title = "AP Mode";
                    }
                        break;
                        
                    case SM_help:
                    {
                        //HELP..
                        //Make URL for the status..
                        sprintf(_semanticMarkerString,"%s/%s/%s", "https://SemanticMarker.org/bot/help", _mqttUserString?_mqttUserString:"NULL", guestPassword?guestPassword:"NULL");
                        title = "Help Info";
                    }
                        break;
                    case SM_doc_follow:  //sm12
                    {
                        //This is where a dynamic DOCFollow would show up..
                        //Make URL for the status..
                        String SM = getLastDocFollowSemanticMarker_MQTTNetworking();
                        title = "DOC FOLLOW";
                        //call the displayModule
                        if (SM.length()>0)
                            sprintf(_semanticMarkerString,"%s", SM);
                        else
                            SM = "https://SemanticMarker.org";
                    }
                        break;
                        //NOTE: each added sm, needs the ButtonProcessing.cpp to update it's list..
                    case SM_reboot:
                    {
                        //REboot the device
                        sprintf(_semanticMarkerString,"%s/%s/%s", "https://SemanticMarker.org/bot/reboot", _mqttUserString?_mqttUserString:"NULL", guestPassword?guestPassword:"NULL");
                        title = "Reboot";
                    }
                        break;
                    case SM_timer:
                    {
                        //timer the device
                        sprintf(_semanticMarkerString,"%s/%s/%s", "https://SemanticMarker.org/bot/timer", _mqttUserString?_mqttUserString:"NULL", guestPassword?guestPassword:"NULL");
                        title = "Timer";
                    }
                        break;

                }
                //TODO: get the guest info.. or they send us the guest password in a message.. for next time..
                //SerialDebug.print("SemanticMarker: ");
                SerialLots.println(_semanticMarkerString);
                //MAYBE save the user web page.. somewhere EPROM
                
                //!turn this OFF if it came in via a SemanticMarker command ...  ???
                if (!markerAlreadyShown)
                {
                    //only if not already shown. Status uses a dynamic function..
                    showSemanticMarker_displayModule(_semanticMarkerString, title);
                }
            } //end smMode 0..n
            else if (strcasecmp(cmd,"ota")==0)
            {
                SerialDebug.println("OTA via BLE");
                //!calls the OTA update method (this doesn't return as device is rebooted...)
                performOTAUpdateMethod();
            }
            else if (strcasecmp(cmd,"clean")==0)
            {
                SerialDebug.println("CLEAN via BLE");
                //!calls the method for cleaning the SSID eprom. This calls the WIFI_APModule callback
                cleanSSID_EPROM_Method();
            }
            else if (strcasecmp(cmd,"feed")==0)
            {
                SerialDebug.printf("FEED via BLE (%s)\n",topic?topic:"NULL TOPIC!!");
                performFeedMethod(topic);
            }
            else if (strcasecmp(cmd,"resettimer")==0)
            {
                resetLoopTimer_displayModule();
            }
            else if (strcasecmp(cmd,"status")==0)
            {
                SerialDebug.println("STATUS via BLE");
                //!print status of the WIFI and MQTT
                SerialMin.printf("WIFI_MQTTState= %d\n",_WIFI_MQTTState);
                SerialMin.printf("DeviceName= %s\n",getDeviceNameMQTT());
                SerialMin.printf("DynamcState= %s\n",getDynamicStatusFunc());
                SerialMin.printf("WIFI connected = %d, %s\n", isConnectedWIFI_MQTTState(), wifiStatus_MQTT());
                SerialMin.printf("MQTT connected = %d, %s\n", isConnectedMQTT_MQTTState(),_mqttClient.connected()?"connected":"not connected");
                
                if (getPreferenceBoolean_mainModule(PREFERENCE_USE_SPIFF_SETTING))
                {
                    printTimestamp_SPIFFModule();
                    printInt_SPIFFModule(_WIFI_MQTTState);
                    println_SPIFFModule((char*)"=WIFI_MQTTState");

                    print_SPIFFModule((char*)getDynamicStatusFunc());
                    println_SPIFFModule((char*)"=DynamcState");

                    printInt_SPIFFModule(isConnectedWIFI_MQTTState());
                    println_SPIFFModule((char*)"=WIFIConnected");

                    print_SPIFFModule((char*)wifiStatus_MQTT());
                    println_SPIFFModule((char*)"=WIFI Status");

                    printInt_SPIFFModule(isConnectedMQTT_MQTTState());
                    println_SPIFFModule((char*)"=MQTTConnected");

                    printInt_SPIFFModule(_mqttClient.connected());
                    println_SPIFFModule((char*)"=MQTTConnected");

                }


                //WL_NO_SSID_AVAIL .. to WL_DISCONNECTED
                //but never reconnects ... 
                SerialLots.println("cmd == status");
                //! request a STATUS be sent.
                processBarkletMessage("#STATUS", topic);
            }
            else if (strcasecmp(cmd,"erase")==0)
            {
                SerialDebug.println("ERASE via BLE");
                main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_EPROM);
            }
            
            //!TODO: duplicate and depreciate these and replace with set:buzz,val:on
            else if (strcasecmp(cmd,"buzzon")==0)
            {
                SerialDebug.println("BUZZON via BLE");
                main_dispatchAsyncCommand(ASYNC_CALL_BUZZ_ON);
            }
            else if (strcasecmp(cmd,"buzzoff")==0)
            {
                SerialDebug.println("BUZZOFF via BLE");
                main_dispatchAsyncCommand(ASYNC_CALL_BUZZ_OFF);
            }
            //Gateway (which I think is obsolete if we can determin this from knowing which feeder we have)
            //keeping for now..
            else if (strcasecmp(cmd,"gatewayOn")==0)
            {
                if (deviceNameSpecified)
                {
                    SerialDebug.println("ASYNC_SET_GATEWAY_ON via BLE");
                    main_dispatchAsyncCommand(ASYNC_SET_GATEWAY_ON);
                }
            }
            else if (strcasecmp(cmd,"gatewayOff")==0)
            {
                if (deviceNameSpecified)
                {
                    SerialDebug.println("ASYNC_SET_GATEWAY_OFF via BLE");
                    main_dispatchAsyncCommand(ASYNC_SET_GATEWAY_OFF);
                }
            }
            //!resetFirstTime
            else if (strcasecmp(cmd,"resetfirsttime")==0)
            {
                if (deviceNameSpecified)
                {
                    SerialDebug.println("PREFERENCE_FIRST_TIME_FEATURE_SETTING ON via BLE");
                    savePreferenceBoolean_mainModule(PREFERENCE_FIRST_TIME_FEATURE_SETTING, true);
                    //!for now just reboot which will use this perference
                    rebootDevice_mainModule();
                }
            }
            //BLECLient
            else if (strcasecmp(cmd,"bleclientOn")==0)
            {
                if (deviceNameSpecified)
                {
                    SerialDebug.println("PREFERENCE_MAIN_BLE_CLIENT_VALUE ON via BLE");
                    savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE, true);
                    //!for now just reboot which will use this perference
                    rebootDevice_mainModule();
                }
            }
            else if (strcasecmp(cmd,"bleclientOff")==0)
            {
                if (deviceNameSpecified)
                {
                    SerialDebug.println("PREFERENCE_MAIN_BLE_CLIENT_VALUE OFF via BLE");
                    savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE, false);
                    //!for now just reboot which will use this perference
                    rebootDevice_mainModule();
                }
            }
            else if (strcasecmp(cmd,"bleserverOn")==0)
            {
                if (deviceNameSpecified)
                {
                    SerialDebug.println("PREFERENCE_MAIN_BLE_SERVER_VALUE ON via BLE");
                    savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE, true);
                    //!for now just reboot which will use this perference
                    rebootDevice_mainModule();
                }
            }
            else if (strcasecmp(cmd,"bleserverOff")==0)
            {
                if (deviceNameSpecified)
                {
                    SerialDebug.println("PREFERENCE_MAIN_BLE_SERVER_VALUE OFF via BLE");
                    savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE, false);
                    //!for now just reboot which will use this perference
                    rebootDevice_mainModule();
                }
            }
            else if (strcasecmp(cmd,"reboot")==0)
            {
                if (deviceNameSpecified)
                {
                    SerialDebug.println("REBOOT  via BLE");
                    //!for now just reboot which will use this perference
                    rebootDevice_mainModule();
                }
            }
            else if (strcasecmp(cmd,"tiltOn")==0)
            {
                SerialDebug.println("PREFERENCE_SENSOR_TILT_VALUE ON via BLE");
                savePreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE, true);
            }
            else if (strcasecmp(cmd,"tiltOff")==0)
            {
                SerialDebug.println("PREFERENCE_SENSOR_TILT_VALUE OFF via BLE");
                savePreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE, false);
            }
            //!zoom == the NON semantic marker version.. so min menu is true
            else if (strcasecmp(cmd,"zoomSMOn")==0)
            {
                
                //hide semantic marker.. (but only if in the max menus)
                //NOTE: this only hides the Semantic Marker - if on a page that has one..
                SerialDebug.println("PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE ON via BLE");
                savePreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE, true);
                
                //!zoom only if in the max menu set..
                if (getCurrentSMMode_mainModule() >= minMenuModesMax_mainModule())
                {
                    //stay on this page, but change the marker..
                    redrawSemanticMarker_displayModule(START_NEW);
                }
                
            }
            else if (strcasecmp(cmd,"zoomSMOff")==0)
            {
                //show semantic marker..
                //NOTE: this only shows the Semantic Marker - if on a page that has one..
                SerialDebug.println("PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE OFF via BLE");
                savePreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE, false);
                savePreferenceBoolean_mainModule(PREFERENCE_IS_MINIMAL_MENU_SETTING, false);
                
                //!zoom only if in the max menu set..
                if (getCurrentSMMode_mainModule() < minMenuModesMax_mainModule())
                {
                    //change to the status..
                    setCurrentSMMode_mainModule(SM_status);
                    //!send message to ourself to process the current mode..
                    invokeCurrentSMModePage(topic);
                }
                else
                {
                    //stay on this page, but change the zoom..
                    redrawSemanticMarker_displayModule(START_NEW);
                }
            }
            else if (strcasecmp(cmd,"poweroff")==0)
            {
                SerialDebug.println("ASYNC_POWEROFF OFF via BLE");
                main_dispatchAsyncCommand(ASYNC_POWEROFF);
                
            }
            else if (strcasecmp(cmd,"wifi")==0)
            {
                SerialDebug.println("cmd=wifi via BLE");
                restartWIFI_MQTTState();
            }
            else if (strcasecmp(cmd,"swapwifi")==0)
            {
                SerialDebug.println("cmd=swapwifi via BLE");
                
                //NOTE: this might be where we toggle credentials?? TODO
                //found other one..
                char *credentials = main_nextJSONWIFICredential();
                
                //!These are the ASYNC_CALL_PARAMETERS_MAX
                //!NO: just change our credentials ...
                //send to ourself.., recursively...
                int val = processJSONMessageMQTT(credentials, topic);
            }
            
            //new 7.29.22 SPIFF
            else if (strcasecmp(cmd,"readspiff")==0)
            {
                SerialDebug.println("readspiff...");
 
                printFile_SPIFFModule();
            }
            else if (strcasecmp(cmd,"sendspiff")==0)
            {
                sendStrings_SPIFFModule(10);
            }
            else if (strcasecmp(cmd,"deletespiff")==0)
            {
                deleteFiles_SPIFFModule();
            }
            else if (strcasecmp(cmd,"capture")==0)
            {
                //! request a CAPTURE be sent.
                processBarkletMessage("#CAPTURE", topic);
#ifdef ESP_M5
                saveScreen_SPIFFModule();
#endif
            }
            //end new
            else if (strcasecmp(cmd,"help")==0)
            {
                //!and print any preferences to show
                printPreferenceValues_mainModule();
                
                sprintf(_fullMessageOut, "Syntax  {\'cmd': \'[ota|clean|feed|erase|status|buzzon|buzzoff| MORE..help]\'} ");
                //!publich back on topic
                //_mqttClient.publish(_mqttTopicString, _fullMessageOut);
#ifdef TRY_MORE_ASYNC_PROCESSING
                //publish back on topic
                publishMQTTMessage(_mqttTopicString, _fullMessageOut);
#else
                _mqttClient.publish(_mqttTopicString, _fullMessageOut);
#endif
            }
            
            //! 12.27.23 pass this onto those registered (which mainModule is handling..)
            if (deviceNameSpecified)
            {
                char* sendCmdString  = const_cast<char*>(cmd);

                //! 12.28.23, 8.28.23  Tell Main about the set,val and if others are registered .. then get informed
                messageSend_mainModule(sendCmdString);
            }
        }
        //! {'guest':'guest password'}
        else if (guestCmd)   //depreciated..
        {
            _mqttGuestPasswordString = const_cast<char*>(guestCmd);
            SerialDebug.printf("guestCmd = '%s'\n", _mqttGuestPasswordString);
        }
        // {'sm':'name/cat/uuid'}
        else if (semanticMarkerCmd)
        {
            //char semanticMarkerString[200];
            char* baseString = (char*)"https://SemanticMarker.org/bot/";
            sprintf(_semanticMarkerString,"%s/%s", baseString, semanticMarkerCmd);
            
            //! 9.28.29 devOnlySM if set, then
            boolean showSM = true;
            if (getPreferenceBoolean_mainModule(PREFERENCE_DEV_ONLY_SM_SETTING))
                showSM = deviceNameSpecified;
            
            if (showSM)
                //! use the name/cat/uuid ..
                showSemanticMarker_displayModule(_semanticMarkerString, "Semantic Marker");
            else
                SerialDebug.println("Not showing SemanticMarker ");
        }
        //!5.12.22
        else if (setCmd && valCmd)
        {
            //! options:  hightemp, feedcount, timeout
            char* setCmdString  = const_cast<char*>(setCmd);
            char* valCmdString = const_cast<char*>(valCmd);
            SerialTemp.print("Set: ");
            SerialTemp.print(setCmdString);
            SerialTemp.print(", Val: ");
            SerialTemp.println(valCmdString);
            
            //! 12.27.23 pass this onto those registered (which mainModule is handling..)
            if (deviceNameSpecified)
            {
                //! 8.28.23  Tell Main about the set,val and if others are registered .. then get informed
                messageSetVal_mainModule(setCmdString, valCmdString);
            }

            //!set flag (if a boolean command)
            boolean flag = isTrueString(valCmdString);
       
            //!try 5.12.22 {'set':'item'},{'val':'value'}
            //!   eg. set:hightemp, val:80)
            //!   TODO: confirm valid integer values...
            if (strcasecmp(setCmdString,"hightemp")==0)
            {
                //!set the high temp value..
                savePreferenceIntFromString_mainModule(PREFERENCE_HIGH_TEMP_POWEROFF_VALUE, valCmdString);
            }
            //! 9.29.22 duplicating a couple of 'set':'cmd', 'val':'feed", since the QUERY for a device is sent that way sometimes..
            else if (strcasecmp(setCmdString,"cmd")==0)
            {
                if (strcasecmp(valCmdString,"feed")==0)
                {
                    SerialCall.printf("feed via set,cmd (%s)\n",topic?topic:"NULL TOPIC!!");
                    performFeedMethod(topic);
                }
                else if (strcasecmp(valCmdString,"status")==0)
                {
                    SerialCall.println("status via set,cmd");
                    //! request a STATUS be sent.
                    processBarkletMessage("#STATUS", topic);
                }
                else if (strcasecmp(valCmdString,"resettimer")==0)
                {
                    resetLoopTimer_displayModule();
                }
                else
                {
                    SerialTemp.printf("Unknown cmd: %s\n", valCmdString);
                }
            }
            //! 9.28.23 #272 devOnlySM only show a SM if sent to this device
            else if (strcasecmp(setCmdString,"devOnlySM")==0)
            {
                if (deviceNameSpecified)
                {
                    //set ble+wifi transient state..
                    savePreferenceBoolean_mainModule(PREFERENCE_DEV_ONLY_SM_SETTING, flag);
                }
            }

            else if (strcasecmp(setCmdString,"ble+wifi")==0)
            {
                if (deviceNameSpecified)
                {
                    //set ble+wifi transient state..
                    savePreferenceBoolean_mainModule(PREFERENCE_SENDWIFI_WITH_BLE, flag);
                }
            }
            else if (strcasecmp(setCmdString,"factoryreset")==0)
            {
                if (deviceNameSpecified)
                {
                    // factory reset .. eventually
                    resetAllPreferences_mainModule();
                }
            }
            //! 11.9.22
            else if (strcasecmp(setCmdString,"restartmodels")==0)
            {
                //!restarts all the menu states to the first one .. useful for getting a clean start. This doesn't care if the menu is being shown
                if (flag)
                    restartAllMenuStates_mainModule();
            }
            else if (strcasecmp(setCmdString,"screentimeout")==0)
            {
                //set the screen timeout
                savePreferenceIntFromString_mainModule(PREFERENCE_DISPLAY_SCREEN_TIMEOUT_VALUE, valCmdString);
            }
            else if (strcasecmp(setCmdString,"stepperangle")==0)
            {
                //!set the stepperangle.
                savePreference_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING, valCmdString);
            }
            else if (strcasecmp(setCmdString,"noclick")==0)
            {
                //!set the timeout from no click to poweroff
                savePreferenceIntFromString_mainModule(PREFERENCE_NO_BUTTON_CLICK_POWEROFF_SETTING, valCmdString);
            }
            else if (strcasecmp(setCmdString,"gateway")==0)
            {
                if (deviceNameSpecified)
                {
                    //! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
                    savePreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE, flag);
                }
            }
            //! 10.4.22
            else if (strcasecmp(setCmdString,"DiscoverM5PTClicker")==0)
            {
                //! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
                setDiscoverM5PTClicker(flag);
            }
            else if (strcasecmp(setCmdString,"usespiff")==0)
            {
                //! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
                savePreferenceBoolean_mainModule(PREFERENCE_USE_SPIFF_SETTING, flag);
                if (flag)
                {
                    //! the setup for this module
                    setup_SPIFFModule();

                }
            }
        
            //!MQTT:  set: timerdelay, val:seconds
            else if  (strcasecmp(setCmdString,"timerdelay")==0)
            {
                if (deviceNameSpecified)
                {
                    int timerdelay = atoi(valCmdString);
                    //!set the timer delay (0 == stop).
                    setTimerDelaySeconds_mainModule(timerdelay);
                    //!start or stop the timer..
                    SerialDebug.printf("timerdelay: %d\n", timerdelay);
                }
            }
            //!MQTT:  set: timerdelay, val:seconds
            else if  (strcasecmp(setCmdString,"timerdelayMax")==0)
            {
                if (deviceNameSpecified)
                {
                    int timerdelay = atoi(valCmdString);
                    //!set the timer delay (0 == stop).
                    setTimerDelaySecondsMax_mainModule(timerdelay);
                    //!start or stop the timer..
                    SerialDebug.printf("timerdelayMax: %d\n", timerdelay);
                }
            }
            //! MQTT:  set: starttimer, val: true/false  (true == start timer, false = stop timer)
            else if  (strcasecmp(setCmdString,"starttimer")==0)
            {
                if (deviceNameSpecified)
                {
                    //!start or stop the timer..
                    startStopTimer_mainModule(flag);
                    //!start or stop the timer..
                    SerialDebug.printf("startTimer: %d\n", flag);
                }
            }
            
            //!add stepper type
            else if (strcasecmp(setCmdString,"stepper")==0)
            {
                int feederType = STEPPER_IS_UNO;
                if (strcasecmp(valCmdString,"mini")==0)
                    feederType = STEPPER_IS_MINI;
                else if (strcasecmp(valCmdString,"tumbler")==0)
                    feederType = STEPPER_IS_TUMBLER;
                //! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
                savePreferenceInt_mainModule(PREFERENCE_STEPPER_KIND_VALUE, feederType);
            }
            else if (strcasecmp(setCmdString,"clockwiseMotor")==0)
            {
                SerialCall.println(" *** Setting clockwise motor");
                //!note since clockwise == 0 we set the opposite of the value..
                savePreferenceBoolean_mainModule(PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING, flag);
            }
            //!add stepper type
            else if (strcasecmp(setCmdString,"otafile")==0)
            {
                //perform the OTA via a file specified .. be careful..
                main_dispatchAsyncCommandWithString(ASYNC_CALL_OTA_FILE_UPDATE_PARAMETER, valCmdString);
            }
            //!set the location
            else if (strcasecmp(setCmdString,"location")==0)
            {
                //perform the OTA via a file specified .. be careful..
                _jsonLocationString = createCopy(valCmdString);
                updatePreferencesInEPROM();
            }
            //! rename device
            else if (strcasecmp(setCmdString,"device")==0)
            {
                //define the device
                _deviceNameString = createCopy(valCmdString);
                updatePreferencesInEPROM();
                
                //!since renaming, lets set a STATUS out..
                //! request a STATUS be sent.
                processBarkletMessage("#STATUS", topic);
            }
            //! pairnow is for invoking the pair when there isn't a user interface. Basically
            //! once an ESP32 gets connected, especially to a GEN3, the pairnow will make it paired
            //! for future. 10.24.22
            else if (strcasecmp(setCmdString,"pairnow")==0)
            {
                if (deviceNameSpecified)
                {
                    //! TRUE will pair, FALSE will unpair
                    if (flag)
                        //!performs the pairing..  to whatever is currently connected, this means a message could make that happen
                        //!for a device (ESP-32) with no user interface.
                        invokePair_ModelController();
                    else
                        //! just unpair .. don't skip
                        //!performs the unpairing
                        invokeUnpairNoName_ModelController();
                }
            }
            //! paireddev the paired device (used with BLEUsePairedDeviceName and gen3Only
            else if (strcasecmp(setCmdString,"pairdev")==0)
            {
                if (deviceNameSpecified)
                {
                    char *previousName = getPreference_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING);
                    if (strcasecmp(valCmdString,previousName)!=0)
                    {
                        //different than current..
                        
                        //!saves the pair device name  TODO: the feed device should use the pair as well.. (DONE..)
                        savePreference_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING, valCmdString);
                        
                        //! paired address is null until found..
                        //! Keep whatever is set ...???
#ifdef USE_BLE_CLIENT_NETWORKING
                        //!if BLE connected, then we keep the address if any and GEN3
                        if (isConnectedBLEClient() && connectedBLEDeviceIsGEN3_mainModule())
                        {
                            //keep the Address (but change the use supplied device name)
                        }
                        else
                        {
                            //! erase the ADDRESS (as well as new name) .. and disconnect if connected..
                            savePreference_mainModule(PREFERENCE_PAIRED_DEVICE_ADDRESS_SETTING, (char*)"");
                            if (isConnectedBLEClient())
                                //! try to disconnect..
                                disconnect_BLEClientNetworking();
                        }
#endif
                        
#ifdef NO_MORE_PREFERENCE_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING
                        if (strlen(valCmdString)==0)
                        {
                            //!turn off pairing
                            savePreferenceBoolean_mainModule(PREFERENCE_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING, false);
                            
                        }
#endif
                    }
                }
            }
            
            //! sets the PREFERENCE_SUPPORT_GROUPS_SETTING
            else if (strcasecmp(setCmdString,"usegroups")==0)
            {
                //! sets the PREFERENCE_SUPPORT_GROUPS_SETTING flag
                savePreferenceBoolean_mainModule(PREFERENCE_SUPPORT_GROUPS_SETTING, flag);
                
                //! reboot the device to set subscribe or not for groups
                rebootDevice_mainModule();

            }
            //! sets the PREFERENCE_GROUP_NAMES_SETTING
            else if (strcasecmp(setCmdString,"groups")==0)
            {
                //! sets the PREFERENCE_GROUP_NAMES_SETTING val (eg. atlasDogs, houndDogs) or (#) or ""
                savePreference_mainModule(PREFERENCE_GROUP_NAMES_SETTING, valCmdString);
                
                //! reboot the device to set subscribe or not for groups
                rebootDevice_mainModule();
                
            }

            else if (strcasecmp(setCmdString,"screencolor")==0)
            {
                //!set the screen color 0..n
                int screenColor = atoi(valCmdString);
                setScreenColor_displayModule(screenColor);
                
                //stay on this page, but change the marker..
                redrawSemanticMarker_displayModule(START_NEW);
            }
            //! sets the gen3only flag (only look for BLEServers that are GEN3)
            else if (strcasecmp(setCmdString,"gen3only")==0)
            {
                //! sets the gen3only flag
                savePreferenceBoolean_mainModule(PREFERENCE_ONLY_GEN3_CONNECT_SETTING, flag);
                //!for now just reboot which will use this perference
               // rebootDevice_mainModule();
                //TODO... maybe just disconnect .. or don't worry about it unless connected
            }
            //! BLEUsePairedDeviceName (Says to only look for BLEServers with the paired name..
            else if (strcasecmp(setCmdString,"BLEUsePairedDeviceName")==0)
            {
                if (deviceNameSpecified)
                {
#ifdef NO_MORE_PREFERENCE_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING
                    //! sets the bleusepaireddevicename flag
                    savePreferenceBoolean_mainModule(PREFERENCE_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING, flag);
#endif
                }
            }
            //! sets the BLEUseDeviceName  flag == the BLEServer will add the name, eg PTFeeder:ScoobyDoo
            else if (strcasecmp(setCmdString,"BLEUseDeviceName")==0)
            {
                if (deviceNameSpecified)
                {
                    //! sets the bleusedevicename flag
                    savePreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING, flag);
                    
                    //!for now just reboot which will use this perference and re-create the service name..
                    rebootDevice_mainModule();
                    
#ifdef USE_BLE_CLIENT_NETWORKING
                    //! try to disconnect..
                    // disconnect_BLEClientNetworking();
#endif
                }
            }
            else if (strcasecmp(setCmdString,"minMenu")==0)
            {
                SerialDebug.println("PREFERENCE_IS_MINIMAL_MENU_SETTING  via BLE");
                if (flag)
                {
                    setCurrentSMMode_mainModule(0);
                    savePreferenceBoolean_mainModule(PREFERENCE_IS_MINIMAL_MENU_SETTING, true);
                }
                else
                {
                    int max = minMenuModesMax_mainModule();
                    //set to start of after min..
                    setCurrentSMMode_mainModule(max);
                }
                //!send message to ourself to process the current mode..
                invokeCurrentSMModePage(topic);
            }
            else if (strcasecmp(setCmdString,"addwifi")==0)
            {
                if (deviceNameSpecified)
                {
                    //has to support "Cisco:"
                    //parse the valCmdString:  ssid:password
                    char str[100];
                    strcpy(str,valCmdString);
                    char *token;
                    char *rest = str;
                    char* ssid = strtok_r(rest,":", &rest);
                    char* password = strtok_r(rest,":", &rest);
                    SerialDebug.printf("addwifi %s, %s\n", ssid?ssid:"null", password?password:"");
                    //now save as a credential
                    //   main_addWIFICredentials(addSSID, addPassword);
                    //!send message to ourself to process the current mode..
                    //   invokeCurrentSMModePage(topic);
                    
                    //NOTE: there can be empty passwords..
                    char credentials[100];
                    //!store the JSON version of these credentials..
                    sprintf(credentials, "{'ssid':'%s','ssidPassword':'%s'}", ssid?ssid:"NULL", password?password:"");
                    // This works by just sending the credentials to ourself .. and process correctly.
                    SerialMin.println(credentials);
                    //!per #224 this will also set WIFI_CREDENTIAL_2  (even if it's also setting #1)
                    //!NOTE: this saving has to be done before calling processJSON (since the string is goofed upand == 'ssid' not the full string
                    savePreference_mainModule(PREFERENCE_WIFI_CREDENTIAL_2_SETTING, credentials);
                    
                    //!now process the credentials, which will set CREDENTIAL_1
                    processJSONMessageMQTT(credentials, TOPIC_TO_SEND);
                    
                    //!print the preferences to SerialDebug
                    printPreferenceValues_mainModule();
                    
                    //The problem with invoking the current SMModePage is if we are on the swap WIFI page, then it will swap .. which might not be desired..
                }
            }
            else if (strcasecmp(setCmdString,"usedocfollow")==0)
            {
                SerialDebug.printf("PREFERENCE_USE_DOC_FOLLOW_SETTING %s\n", valCmdString);
                savePreferenceBoolean_mainModule(PREFERENCE_USE_DOC_FOLLOW_SETTING, flag);
        
            }
            else if (strcasecmp(setCmdString,"semanticMarker")==0)
            {
                SerialDebug.printf("SemanticMarker: %s\n", valCmdString);
                setLastDocFollowSemanticMarker_MQTTNetworking(valCmdString);
                
                //! 9.28.29 devOnlySM if set, then
                boolean showSM = true;
                if (getPreferenceBoolean_mainModule(PREFERENCE_DEV_ONLY_SM_SETTING))
                    showSM = deviceNameSpecified;
                if (showSM)
                    // 9.27.23 also show this ..
                    //! use the name/cat/uuid ..
                    showSemanticMarker_displayModule(valCmdString, "Semantic Marker");
                else
                    SerialDebug.println("Not showing SemanticMarker ");
            }
            //blankscreen on/off
            else if (strcasecmp(setCmdString,"blankscreen")==0)
            {
                //!if flag then blankscreen, otherwise wake the screen..
                if (flag)
                    //!blanks the screen
                    blankScreen_displayModule();
                else
                    //!wakes up the screen
                    wakeupScreen_displayModule();
            }
            
            //!8.17.22 SubDawgpack
            else if (strcasecmp(setCmdString,"SubDawgpack")==0)
            {
                if (deviceNameSpecified)
                {
                    SerialDebug.println("PREFERENCE_SUB_DAWGPACK_SETTING via BLE");
                    savePreferenceBoolean_mainModule(PREFERENCE_SUB_DAWGPACK_SETTING, flag);
                    //!for now just reboot which will use this perference
                    //rebootDevice_mainModule();
                    if (flag)
                    {
                        //! start a dawgpack subscription
                        //! 8.15.22  Also subscribe to the dawgpack .. but restrict what it can effect.
                        //! For example, start with STATUS and DOCFOLLOW
                        _mqttClient.subscribe((char*)"usersP/dawgpack");
                    }
                    else
                    {
                        // unsubscribe  (tested and it works)
                        _mqttClient.unsubscribe((char*)"usersP/dawgpack");
                    }
                }
            }

            //!TODO: duplicate and depreciate these and replace with set:buzz,val:on
            else if (strcasecmp(setCmdString,"buzz")==0)
            {
                //! this uses the ASYNC since it involves a BLE command, and has to be done outside
                //! of this WIFI (MQTT) operation..
                if (flag)
                {
                    SerialDebug.println("BUZZ:ON via BLE");
                    main_dispatchAsyncCommand(ASYNC_CALL_BUZZ_ON);
                }
                else
                {
                    SerialDebug.println("BUZZ:OFF via BLE");
                    main_dispatchAsyncCommand(ASYNC_CALL_BUZZ_OFF);
                }
            }
            
            
            //BLECLient
            else if (strcasecmp(setCmdString,"bleclient")==0)
            {
                if (deviceNameSpecified)
                {
                    SerialDebug.println("PREFERENCE_MAIN_BLE_CLIENT_VALUE via BLE");
                    savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE
                                                     , flag);
                    //!for now just reboot which will use this perference
                    rebootDevice_mainModule();
                }
            }
            // bleserver
            else if (strcasecmp(setCmdString,"bleserver")==0)
            {
                if (deviceNameSpecified)
                {
                    SerialDebug.println("PREFERENCE_MAIN_BLE_SERVER_VALUE via BLE");
                    savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE, flag);
                    //!for now just reboot which will use this perference
                    rebootDevice_mainModule();
                }
            }
            else if (strcasecmp(setCmdString,"tilt")==0)
            {
                if (deviceNameSpecified)
                {
                    SerialDebug.println("PREFERENCE_SENSOR_TILT_VALUE  via BLE");
                    savePreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE, flag);
                }
            }
            else if (strcasecmp(setCmdString,"zoomSm")==0)
            {
                //!zoom == the NON semantic marker version.. so min menu is true
                if (flag)
                {
                    
                    //hide semantic marker.. (but only if in the max menus)
                    //NOTE: this only hides the Semantic Marker - if on a page that has one..
                    SerialDebug.println("PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE ON via BLE");
                    savePreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE, true);
                    
                    //!zoom only if in the max menu set..
                    if (getCurrentSMMode_mainModule() >= minMenuModesMax_mainModule())
                    {
                        //stay on this page, but change the marker..
                        redrawSemanticMarker_displayModule(START_NEW);
                    }
                    
                }
                else
                {
                    //show semantic marker..
                    //NOTE: this only shows the Semantic Marker - if on a page that has one..
                    SerialDebug.println("PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE OFF via BLE");
                    savePreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE, false);
                    savePreferenceBoolean_mainModule(PREFERENCE_IS_MINIMAL_MENU_SETTING, false);
                    
                    //!zoom only if in the max menu set..
                    if (getCurrentSMMode_mainModule() < minMenuModesMax_mainModule())
                    {
                        //change to the status..
                        setCurrentSMMode_mainModule(SM_status);
                        //!send message to ourself to process the current mode..
                        invokeCurrentSMModePage(topic);
                    }
                    else
                    {
                        //stay on this page, but change the zoom..
                        redrawSemanticMarker_displayModule(START_NEW);
                    }
                }
            } // zoomSM
            //! 9.22.22  added button press from messages..
            else if (strcasecmp(setCmdString,"buttonA")==0)
            {
                if (strcasecmp(valCmdString,"longpress")==0)
                    buttonA_LongPress_mainModule();
                else if (strcasecmp(valCmdString,"shortpress")==0)
                    buttonA_ShortPress_mainModule();
            }
            else if (strcasecmp(setCmdString,"buttonB")==0)
            {
                if (strcasecmp(valCmdString,"longpress")==0)
                    buttonB_LongPress_mainModule();
                else if (strcasecmp(valCmdString,"shortpress")==0)
                    buttonB_ShortPress_mainModule();
            }
            else if (strcasecmp(setCmdString,"M5AtomKind")==0)
            {
                //! new 1.4.24 setting ATOM kind (eg. M5AtomSocket, M5AtomScanner). MQTT message "set":"M5AtomKind", val=
                savePreferenceATOMKind_MainModule(valCmdString);
            
                //!for now just reboot which will use this perference
                rebootDevice_mainModule();
            }
            
            
            else
            {
                SerialMin.printf("Unknown cmd: %s\n", setCmdString);
            }

        }
        //!5.24.22  send:<request>  .. Note these are for cmd without an argument..
        else if (sendCmd)
        {
            //!NOTE: This will be calling ourself

            char* sendCmdString  = const_cast<char*>(sendCmd);
            if (strcasecmp(sendCmdString,"temp")==0)
            {
                SerialCall.println("sendCmd == temp");
                //! request a TEMP be sent.
                processBarkletMessage("#TEMP", topic);
            }
            else if (strcasecmp(sendCmdString,"status")==0)
            {
                SerialCall.println("sendCmd == status");
                //! request a STATUS be sent.
                processBarkletMessage("#STATUS", topic);
            }
            else if (strcasecmp(sendCmdString,"capture")==0)
            {
                SerialCall.println("sendCmd == capture");
                //! request a CAPTURE be sent.
                processBarkletMessage("#CAPTURE", topic);
            }
            else if (strcasecmp(sendCmdString,"volume")==0)
            {
                SerialCall.println("sendCmd == volume (not implemented)");
                //! request a VOLUME be sent.
                processBarkletMessage("#VOLUME", topic);
            }
            else
            {
                SerialTemp.print("Unknown send request: ");
                SerialTemp.println(sendCmdString);
            }

        }
        //! 9.18.23 set64 with a val
        else if (set64Cmd)
        {
#ifdef ESP_M5
            //! currently 9.28.23 no reason for feeder to decode base64 messages. That could change if a stored procedure..
            char* setCmdString  = const_cast<char*>(set64Cmd);
            char* valCmdString = const_cast<char*>(valCmd);
#ifdef DECODE_BASE64
            char plaintext_out[200];
            int len = strlen(valCmdString);
            if (len > 200)
            {
                SerialError.println("Length too long to base64 decode: 200");
            }
            else
            {
                int status = base64_decode_chars(valCmdString, len, plaintext_out);
                String decoded = String(plaintext_out);
                decoded = MQTT_urlDecode(decoded);
                SerialDebug.println(decoded);
                // not case sensitive
                if (strcasecmp(setCmdString,"semanticMarker")==0)
                {
                    //! 9.28.29 devOnlySM if set, then set to whether the deviceNameSpecified (eg. dev=...)
                    boolean showSM = true;
                    if (getPreferenceBoolean_mainModule(PREFERENCE_DEV_ONLY_SM_SETTING))
                        showSM = deviceNameSpecified;
                    
                    if (showSM)
                        //!displays the Semantic Marker (a super QR code) on the M5 screen (title = to display)
                        showSemanticMarker_displayModule(decoded, "SemanticMarker");
                    else
                        SerialDebug.println("Not showing SemanticMarker ");
                }
            }
            
#endif //DECODE_BASE64
#endif //M5

        }
        
        //!NOTE: if teh command isn't recognized .. then it slips through.. and is treaded like
        //!this return is important!!!
        return true;
    } //!end process commands..
    
    //! ISSUE: when new JSON shows up that isn't process above, it's thought to be setting the credentials.
    //! Thus the {set64} continues below..
    
    
    //end NEW
    //!TRY without 1.30.22 (RAMS win) and it works..
    //!9.17.23 .. RAMS loose to 49rs in LA (we are in LA)
    //!if can talk BLE, then reboot..  2.2.22
    
    //NOTE: here is where different reasons for the info could be provided in the data>
    // eg. dataKind (wifi, mqtt, etc...)boot
    /**
     {
     "ssid" : "SunnyWhiteriver",
     "ssidPassword" : "sunny2021",
     "mqtt_topic": "usersP/bark/test",
     "mqtt_user" : "test",
     "deviceName" : "HowieFeeder",
     "mqtt_password" : "password..",
     "mqtt_guestPassword" : "password",
     "uuid" : "scott",
     "mqtt_port" : "1883",
     "mqtt_server" : "idogwatch.com",
     "mqtt_status" : "Success",
     "location": "whatever to reply with, GPS, state, city, etc.."
     }
     
     //todo pass the guest password too
     */
    //TODO: parse the string...
    //!! Store wifi config.  存储wifi配置信息
    //! @see https://arduinojson.org
    
//! THE CHALLENGE:  if send use {ssid, ssidPassword} .. it will assume others
//! SO .. only null out the value if (1) there is an attributed {ssid} and empty string..
//#define ONLY_NULL_IF_THERE
#ifdef ONLY_NULL_IF_THERE
    SerialDebug.println(" *** processMQTT .. treat like credentials");
    {
        const char* a1 = myObject["ssid"];
        if (a1)
        {
            _ssidString = const_cast<char*>(a1);
            _ssidString = createCopy(_ssidString);
        }
        
        SerialTemp.printf("myObject[ssid] = '%s'\n", a1?a1:"EMPTY");
        //SerialTemp.println(a1);
        SerialTemp.println(_ssidString);
    }
    
    {
        const char* a2 = myObject["ssidPassword"];
        if (a2)
        {
            _ssidPasswordString = const_cast<char*>(a2);
            _ssidPasswordString = createCopy(_ssidPasswordString);
        }
        
    }
#else
    SerialDebug.println(" *** processMQTT .. treat like credentials");
    {
        const char* a1 = myObject["ssid"];
        if (a1 && strlen(a1)>0)
        {
            _ssidString = const_cast<char*>(a1);
            _ssidString = createCopy(_ssidString);
        }
        else
            _ssidString = NULL;
        SerialTemp.printf("myObject[ssid] = '%s'\n", a1?a1:"EMPTY");
        //SerialTemp.println(a1);
        SerialTemp.println(_ssidString);
    }
    
    {
        const char* a2 = myObject["ssidPassword"];
        if (a2 && strlen(a2)>0)
        {
            _ssidPasswordString = const_cast<char*>(a2);
            _ssidPasswordString = createCopy(_ssidPasswordString);
        }
        else
            _ssidPasswordString = NULL;
    }
#endif
    {
        //!5.25.22 (50 years since Grateful Dead London Show
        
        //! To support just setting the ssid and password, a JSON
        //! of {ssid:s,ssidPassword:p} is supported, so don't null out if mqtt aren't provided..
        //! this should work (since the SSID is what's checked to go to the AP mode)
        
        //!the MQTT host/port/user/password  (topic is created in this code...)
        const char* a3 = myObject["mqtt_server"];
        if (a3)
        {
            _mqttServerString = const_cast<char*>(a3);
            _mqttServerString = createCopy(_mqttServerString);
        }
        //    else
        //        _mqttServerString = NULL;
    }
    {
        const char* a4 = myObject["mqtt_port"];
        if (a4)
        {
            _mqttPortString = const_cast<char*>(a4);
            _mqttPortString = createCopy(_mqttPortString);
        }
        //    else
        //        _mqttPortString = NULL;
        //
    }
    {
        const char* a5 = myObject["mqtt_password"];
        if (a5)
        {
            _mqttPasswordString = const_cast<char*>(a5);
            _mqttPasswordString = createCopy(_mqttPasswordString);
        }
        //    else
        //        _mqttPasswordString = NULL;
    }
    
    {
        const char* a6 = myObject["mqtt_user"];
        if (a6)
        {
            _mqttUserString = const_cast<char*>(a6);
            _mqttUserString = createCopy(_mqttUserString);
        }
        //    else
        //        _mqttUserString = NULL;
    }
    {
        const char* a7 = myObject["deviceName"];
        if (a7 && strlen(a7)>0)
        {
            _deviceNameString = const_cast<char*>(a7);
            _deviceNameString = createCopy(_deviceNameString);
        }
        //    else
        //        _deviceNameString = NULL;
    }
    {
        const char* a8 = myObject["uuid"];
#ifdef NOT_ORIGINAL
        if (a8 && strlen(a8)>0)
#else
        if (a8)
#endif
        {
            _uuidString = const_cast<char*>(a8);
            _uuidString = createCopy(_uuidString);
        }
        //    else
        //        _uuidString = NULL;
    }
    {
        const char* a9 = myObject["mqtt_topic"];
#ifdef NOT_ORIGINAL
        if (a9 && strlen(a9)>0)
#else
        if (a9)
#endif
        {
            _mqttTopicString = const_cast<char*>(a9);
            _mqttTopicString = createCopy(_mqttTopicString);
        }
        //    else
        //        _mqttTopicString = NULL;
        
    }
    {
        const char* a10 = myObject["mqtt_guestPassword"];
#ifdef NOT_ORIGINAL
        if (a10 && strlen(a10)>0)
#else
        if (a10)
#endif
        {
            _mqttGuestPasswordString = const_cast<char*>(a10);
            _mqttGuestPasswordString = createCopy(_mqttGuestPasswordString);
        }
        //    else
        //        _mqttGuestPasswordString = NULL;
    }
    
    {
        const char* a11 = myObject["location"];
        if (a11 && strlen(a11)>0)
        {
            _jsonLocationString = const_cast<char*>(a11);
            _jsonLocationString = createCopy(_jsonLocationString);
        }
        //    else
        //        _mqttGuestPasswordString = NULL;
    }
    
    //!reset the MQTT attempts
    _globalMQTTAttempts = 0;
    
    boolean saveJSONPreferences = true;
    //!setup the WIFI if the ssid string (at least) is specified
    if (!isEmptyString(_ssidString ))
    {
        SerialDebug.println("Setting WIFI from JSON parameters");
        //setupWIFI(_ssidString, _ssidPasswordString);
        
        //!new: go out and let the process work...
        //!set the state, then the 'loop' will call setupWIF(...)
        _WIFI_MQTTState = preSetupWIFI;
        startDelay_WIFI_MQTTState();

    }
    else
    {
        SerialDebug.println(" ***** ERROR .. no ssidString *** ");
        //!call the callback specified from the caller (eg. NimBLE_PetTutor_Server .. or others)
        callCallbackMain(CALLBACKS_MQTT, MQTT_CALLBACK_SOLID_LIGHT, (char*)"solidLight");

        saveJSONPreferences = false;
    }

    
    //!don't save the preferences, since it didn't have enough information..
    if (!saveJSONPreferences)
    {
        SerialDebug.println("**** Not saving JSON in preferences ***");
        return true;
    }
    
    //!NOTE: this writes over entire values, since it's a string vs an JSON object
    updatePreferencesInEPROM();
        
    //!new 4.8.22 .. trying to kick out of AP mode if the credentials are good..
    main_credentialsUpdated();
    
    //!putting here .. time might have gone too fast..
    _WIFI_MQTTState = preSetupWIFI;
    startDelay_WIFI_MQTTState();

    
    return true;
}

//!restart the WIFI and then MQTT connection
void restartWIFI_MQTTState()
{
    //let it know MQTT isn't running either
    _MQTTRunning = false;
    
    SerialTemp.printf("restartWIFI_MQTTState (%s, %s)\n", _ssidString?_ssidString:"NULL", _ssidPasswordString?_ssidPasswordString:"NULL");
//#define TRY_EXIT2
#ifdef TRY_EXIT2
    //! 9.19.23 before Van Morrison ..
    if (!_ssidString || (_ssidString && strlen(_ssidString)==0))
    {
        SerialDebug.println("set disconnectedWIFI and stopDelay");
        //putting here .. time might have gone too fast..
        _WIFI_MQTTState = disconnectedWIFI;
        stopDelay_WIFI_MQTTState();
        return;
    }
#endif
    //putting here .. time might have gone too fast..
    _WIFI_MQTTState = preSetupWIFI;
    startDelay_WIFI_MQTTState();
}

//! 9.18.23 LA (after Eagle Rock bike ride, Van Morrison tomorrow)
void cleanMQTTpasswordsUpdateInEPROM()
{
    SerialDebug.println("cleanMQTTpasswordsUpdateInEPROM");
    _ssidString = NULL;
    _mqttPasswordString = NULL;
    _mqttGuestPasswordString = NULL;

    //!now update the eprom with these null values
    updatePreferencesInEPROM();
}

//!just update the EPROM, and send that to the WIFI_AP module as well
void updatePreferencesInEPROM()
{
    SerialDebug.printf("updatePreferencesInEPROM (mqtt-pass=%s)\n",_mqttPasswordString?_mqttPasswordString:"NULL");
    
    DynamicJsonDocument myObject(1024);

    //!basically if only the ssid/pass are sent, that is all that's written to EPROM
    //!even if the other information is available.. So recreate the JSON instead..
    //!seems c++ you cannot re-use output as it just appends to it.. unreal
    String output2;
#ifdef NOT_ORIGINAL
    myObject["ssid"] = _ssidString?_ssidString:"";
    myObject["ssidPassword"] = _ssidPasswordString?_ssidPasswordString:"";
    myObject["mqtt_server"] = _mqttServerString;
    myObject["mqtt_port"] = _mqttPortString;
    myObject["mqtt_password"] = _mqttPasswordString?_mqttPasswordString:"";
    myObject["mqtt_guestPassword"] = _mqttGuestPasswordString?_mqttGuestPasswordString:"";
#else
    myObject["ssid"] = _ssidString;
    myObject["ssidPassword"] = _ssidPasswordString;
    myObject["mqtt_server"] = _mqttServerString;
    myObject["mqtt_port"] = _mqttPortString;
    myObject["mqtt_password"] = _mqttPasswordString;
    myObject["mqtt_guestPassword"] = _mqttGuestPasswordString;
#endif
    myObject["mqtt_user"] = _mqttUserString;
    {
        char buf[100];
        sprintf(buf,"usersP/bark/%s", _mqttUserString?_mqttUserString:"NOTSET");
        _mqttTopicString = createCopy(buf);
    }
    myObject["mqtt_topic"] = _mqttTopicString;
    myObject["deviceName"] = _deviceNameString;
    myObject["uuid"] = _uuidString;
    myObject["jsonHeader"] = _jsonHeaderString;
    myObject["jsonVersion"] = _jsonVersionString;
    myObject["location"] = _jsonLocationString;
    
    
    //!JSON
    serializeJson(myObject, output2);
    
    //!open the preferences
    _preferencesMQTTNetworking.begin(ESP_EPROM_NAME, false);
    SerialDebug.print("MQTTNetworking.Writing EPROM JSON = '");
    SerialDebug.print(output2);
    SerialDebug.println("'");
    
    //!save in EPROM
    _preferencesMQTTNetworking.putString(_preferencesJSONName, output2);
    //! Close the Preferences
    _preferencesMQTTNetworking.end();
    
    //! sets the MQTT user/password. It's up to the code to decide who needs to know (currently saves in the WIFI_APModule
    main_updateMQTTInfo(_ssidString, _ssidPasswordString, _mqttUserString, _mqttPasswordString, _mqttGuestPasswordString, _deviceNameString, _mqttServerString, _mqttPortString, _jsonLocationString);
#define NO_NEED_AND_GRU_CRASH
#ifdef NO_NEED_AND_GRU_CRASH
    //!NEW: 2.21.22
    //!TRY: reading back..
    _preferencesMQTTNetworking.begin(ESP_EPROM_NAME, false);  //false=read/write..
    _fullJSONString = _preferencesMQTTNetworking.getString(_preferencesJSONName);
    SerialDebug.print("Reading.2 EPROM JSON = ");
    SerialDebug.println(_fullJSONString? _fullJSONString:"NULL");
    
    //!check ... _fullMessageOut
    //! Close the Preferences
    _preferencesMQTTNetworking.end();
#endif
}



//!Decode the URL
String MQTT_urlDecode(String input) {
    String s = input;
    s.replace("%20", " ");
    s.replace("+", " ");
    s.replace("%21", "!");
    s.replace("%22", "\"");
    s.replace("%23", "#");
    s.replace("%24", "$");
    s.replace("%25", "%");
    s.replace("%26", "&");
    s.replace("%27", "\'");
    s.replace("%28", "(");
    s.replace("%29", ")");
    s.replace("%30", "*");
    s.replace("%31", "+");
    s.replace("%2C", ",");
    s.replace("%2E", ".");
    s.replace("%2F", "/");
    s.replace("%2C", ",");
    s.replace("%3A", ":");
    s.replace("%3A", ";");
    s.replace("%3C", "<");
    s.replace("%3D", "=");
    s.replace("%3E", ">");
    s.replace("%3F", "?");
    s.replace("%40", "@");
    s.replace("%5B", "[");
    s.replace("%5C", "\\");
    s.replace("%5D", "]");
    s.replace("%5E", "^");
    s.replace("%5F", "-");
    s.replace("%60", "`");
    return s;
}
#endif //USE_MQTT_NETWORKING
