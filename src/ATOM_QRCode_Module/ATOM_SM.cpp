
//
//  ATOM_SM.cpp
//  M5ATOM_Testing
//
//  Created by Scott Moody on 12/18/23.
//

#include "ATOM_SM.h"
#ifdef ATOM_QRCODE_MODULE

#define DECODE_BASE64
#ifdef DECODE_BASE64

//! 9.27.23 to decode a base64 string (a Semantic Marker)
#include <libb64/cdecode.h>
#endif

#define MAX_SM 500
char _copyLastSemanticMarker[MAX_SM];
//! process the semantic marker (maybe from a click or a scan)
//! If a SMART button, it will tack on username, password and optionally scannedDevice
//! 12.15.23
//! return TRUE if this is a new SemanticMarker to save in _lastSemanticMarker
//! return FALSE if this is a transient SM so don't override _lastSemanticMarker (eg. /scannedDevice, WIFI, credentials)
//! Added passing lastSemanticMarker .. this is for the /scannedDevice to recursively call itself
//! (so parms are substituded)
boolean ATOM_processSemanticMarker(char *semanticMarker, char *lastSemanticMarker)
{
    SerialDebug.printf("ATOM_processSemanticMarker(%s, %s)\n", semanticMarker, lastSemanticMarker);
    //!NOTE: The _lastSemanticMarker will be replaced with this semanticMarker .. unless
    //!   * scannedDevice - since ethen the long press will re-run the last scanned semantic marker but with the new Device
    
    //! scannedDevice is a special that is only for the scaner. Syntax:
    //! https://SemanticMarker.org/bot/scannedDevice/{deviceName}
    //! if {deviceName} == "_none"  then it empties it..
    //! NOTE: 12.16.23 The scannedDevice is a temporal feature. It will not replace
    //! the last scanned SemanticMarker, instead it will be used to set the ScannedDevice (or none)
    //! That way the longPress can still be used to re-invoke the lastSemanticMarker (such as the feed)
    //! without having to re-scan it.
    //! BUT: we will send this on the MQTT network just so others can hear it and decide what to do
    //! Such as  the remote Abilities monitor sees if the user scanned the device first etc..
    //! FUTURE: decide on a remote ScannedDevice feature.. maybe called that: remoteScannedDevice
    //! which will be an MQTT message (so a remote handler can set the device..)
    if (containsSubstring(semanticMarker,"/scannedDevice/"))
    {
        char *smartIndex = strstr(semanticMarker,"/scannedDevice/");
        smartIndex += strlen("/scannedDevice/");
        char *scannedDevice = smartIndex;
        SerialDebug.printf("scannedDevice = %s\n", scannedDevice);
        if (strcmp(scannedDevice,"_none")==0)
            main_setScannedDeviceName((char*)"");
        else
            main_setScannedDeviceName(scannedDevice);
        
        //! TODO: send MQTT message is it {"scannedDevice",device}
        //! TODO: implement the remoteScannedDevice MQTT message (or is {set:remoteScannedDevice, val:device}
        //! or {device:D, set:remoteScannedDevice, val:device} -- only to specified device (in the multicast world)
        
        strcpy(_copyLastSemanticMarker, lastSemanticMarker);
        //! this is 2 in one. So call ourself but using the lastSemanticMarker
        ATOM_processSemanticMarker(_copyLastSemanticMarker, lastSemanticMarker);
        return false;
    }
    else if (containsSubstring(semanticMarker, "WIFI:"))
    {
        //! TODO.. parse this, create a JSON  { "ssid"  then call processJSONMessageMQTT ..
        char credentailsJSON[100];
        char *ssid = (char*)"SunnyWhiteriver";
        char *ssidPassword = (char*)"sunny2021";
        sprintf(credentailsJSON, "{'ssid':'%s','ssidpassword':'%s'}", ssid, ssidPassword);
        /*
         WIFI:S:<SSID>;T:<WEP|WPA|blank>;P:<PASSWORD>;H:<true|false|blank>;
         */
        processJSONMessageMQTT(credentailsJSON, TOPIC_TO_SEND);
        
        //! Set the credentials for the WIFI using this format
        return false;
    }
    
    else if (containsSubstring(semanticMarker, "/credentials/"))
    {
        //!   ./bot/credentials/base64JSON of credentials ..
#ifdef ESP_M5
        //! currently 9.28.23 no reason for feeder to decode base64 messages. That could change if a stored procedure..
        
        char *smartIndex = strstr(semanticMarker,"/credentials/");
        smartIndex += strlen("/credentials/");
        char *scannedDevice = smartIndex;
        SerialDebug.printf("credentials = %s\n", scannedDevice);
        
        // syntax:  /bot/credentials/base64JSON   or
        //          WIFI convention.. not base64
#define DECODE_BASE64
#ifdef DECODE_BASE64
        char plaintext_out[400];
        int len = strlen(scannedDevice);
        if (len > 400)
        {
            SerialError.println("Length too long to base64 decode: 400");
        }
        else
        {
            int status = base64_decode_chars(scannedDevice, len, plaintext_out);
            String decoded = String(plaintext_out);
            decoded = MQTT_urlDecode(decoded);
            char *decodedCredentials = (char*)decoded.c_str();
            SerialDebug.println(decodedCredentials);
            
            //! ascii and topic
            processJSONMessageMQTT(decodedCredentials, TOPIC_TO_SEND);
            
            //! TODO..
            //! now pass this onto the MQTT processor as JSON (we hope ...)
            return false;
        }
        
#endif //DECODE_BASE64
#endif //M5
        return false;
    }
    
    //! see if connected to a BLE device..
    char *connectedBLEDeviceName = connectedBLEDeviceName_mainModule()?connectedBLEDeviceName_mainModule():NULL;
    SerialDebug.printf("connectedBLEDeviceName = %s\n", connectedBLEDeviceName);
    
    if (containsSubstring(semanticMarker,"feedguestdevice"))
    {
        char *device = rindex(semanticMarker,'/');
        if (device) device++; // skip over the "/"
        
        if (device && connectedBLEDeviceName && strcmp(device,connectedBLEDeviceName)==0)
        {
            // only feed if the device is ours (over BLE)
            SerialDebug.printf(" *** BLE FEEDING since inside SM (device=%s)\n", device);
#ifdef USE_BLE_CLIENT_NETWORKING
            
            //!send a BLE feed command as we are connected
            sendFeedCommandBLEClient();
#endif
            //!perform ACK too
#ifdef USE_MQTT_NETWORKING
            //ack is sent by the caller of this message..??
            sendMessageMQTT((char *)"#ackMe");
            
#endif  //useMQTT
        }
        else
        {
            //! send WIFI feed to only the device decoded..
            
            SerialDebug.printf(" *** WIFI FEEDING since inside SM (device=%s)\n", device);
            if (device)
            {
                char pubString[100];
                //!this should work ..
                sprintf(pubString,"{'dev':'%s'",device);
                strcat(pubString,",'cmd':'feed'}");
#ifdef USE_MQTT_NETWORKING
                publishMQTTMessageDefaultTopic(pubString);
#endif
            }
            else
            {
                ///feed always  (done after the code below..)
                main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
                
            }
        }
    }
    else if (containsSubstring(semanticMarker,"feeddevice"))
    {
        char *device = rindex(semanticMarker,'/');
        if (device) device++; // skip over the "/"
        SerialDebug.printf(" *** WIFI FEEDING since inside SM (device=%s)\n", device);
        
        ///feed always  (done after the code below..)
        main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
    }
    else if (containsSubstring(semanticMarker,"feedguest"))
    {
        SerialDebug.println(" *** WIFI FEEDING since inside SM");
        
        ///feed always  (done after the code below..)
        main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
    }
    else if (containsSubstring(semanticMarker,"feed"))
    {
        SerialDebug.println(" *** WIFI FEEDING since inside SM");
        
        ///feed always  (done after the code below..)
        main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
    }
    
    
    // SMART buttons..
    else if (containsSubstring(semanticMarker,"/smart"))
    {
        //! try a call..
        //! call 'smrun' with URL query parameters, like username/password/device
        //! /bot/smrun?uuid=x&flownum=y&username=X&password=y&device=z
        //! HERE .. we add the Username, Password and Device name.. parameters
#ifdef USE_REST_MESSAGING
        char getCommand[MAX_SM];
        char *username = main_getUsername();
        char *password = main_getPassword();
        char *scannedDeviceName = main_getScannedDeviceName();
        SerialDebug.println(username);
        SerialDebug.println(password);
        SerialDebug.println(scannedDeviceName);
        
#define TRY_OPTIMIZE
#ifdef  TRY_OPTIMIZE
        //! TODO: syntax for an /optimize?uuid=x&flow=y&cmd=feed
        //! The MQTT is much faster.. Lets only use MQTT for the 'device' specified (to see this difference)
        //    strcpy(_lastSemanticMarker,"https://SemanticMarker.org/bot/smart?uuid=QHmwUurxC3&flow=1674517131429");
        if (containsSubstring(semanticMarker, "QHmwUurxC3") && containsSubstring(semanticMarker,"1674517131429"))
        {
            //! this will be a cache of commands to try..
            //! cmd:feed,  cmddevice,feed,device
            if (scannedDeviceName && strlen(scannedDeviceName) > 0)
            {
                //! dev: <dev>, cmd: feed
                //! cmd: feed
                sprintf(getCommand, "{'dev':'%s', 'cmd':'feed'}", scannedDeviceName);
                sendMessageString_mainModule(getCommand);
                return true;

            }
            else
            {
             //   sprintf(getCommand, "{'cmd':'feed'}");
            }
        }
#endif
        //! call the /smrun which an HTTPS call to the SemanticMarker.org/bot site, and then decoded
        //! .. etc  Slower
        char *smartIndex = strstr(semanticMarker,"/smart");
        //! move past /smart
        smartIndex += strlen("/smart");
        strcpy(getCommand, "/bot/smrun");
        strcat(getCommand, smartIndex);
        //! NOTE: this assumes smartIndex has parameters like: " /smart?uuid=x&flownum=y"
        //! WITHOUT the smart/  it's smart?uuid...
        strcat(getCommand, "&username=");
        strcat(getCommand, username);
        strcat(getCommand, "&password=");
        strcat(getCommand, password);
        if (scannedDeviceName && strlen(scannedDeviceName) > 0)
        {
            strcat(getCommand, "&device=");
            strcat(getCommand, scannedDeviceName);
        }
#ifdef FIRST_TAKE
        //!for this version .. we are just going to send username/password/device to the smart button .. it might not even use them but
        //!it might.. It will definitely use the username and password
        //!NOTE: Some SMART Buttons might have additional parameters ..
        if (scannedDeviceName && strlen(scannedDeviceName) > 0)
            sprintf(getCommand, "/bot/smrun?username=%s&password=%s&device=%s",username, password, scannedDeviceName);
        else
            sprintf(getCommand, "/bot/smrun?username=%s&password=%s", username?username:"NULL", password?password:"NULL");
#endif
        
        sendSecureRESTCall(getCommand);
#endif
    }
    return true;
}

#endif //ATOM_QRCODE_MODULE
