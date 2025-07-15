
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

//! parse the query args .. ?<arg>=<val>{&<arg>=<val>}
//! 1.1.24 if useAnyArgs then just create JSON from all the arg-val without remapping
void parseQueryArgs(char *credentialsStringInput, boolean useAnyArgs);


#define MAX_SM 500
char _copyLastSemanticMarker[MAX_SM];
char _lastScannedGroupName[40];

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
    //! https://SemanticMarker.org/bot/scannedDevice/{deviceName}{?queryParms}
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
        //! per issue #286, syntax has been extended to include arguments
        //! /scannedDevice/DeviceName?kind=AtomSocket&status=socket&valKind=onoff
        //! HERE we just need to search for the "?" and that will be the device name.
        char *questionMark = strstr(smartIndex, "?");
        char scannedDeviceBuffer[50];
        scannedDeviceBuffer[0] = '\0';
        int *scannedDeviceLen;
        //! use the _copyLastSemanticMarker as a placeholder..
        //! @see https://stackoverflow.com/questions/1453876/why-does-strncpy-not-null-terminate
        if (questionMark)
        {
            int len = questionMark - smartIndex;
            strncpy(scannedDeviceBuffer, smartIndex, len);
            scannedDeviceBuffer[len] = '\0';
        }
        else
            strcpy(scannedDeviceBuffer,smartIndex);
        char *scannedDevice = scannedDeviceBuffer;
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
    //! 1.7.24 (snow in mountains and down low .. ski monday)
    else if (containsSubstring(semanticMarker,"/scannedGroup/"))
    {
        char *smartIndex = strstr(semanticMarker,"/scannedGroup/");
        smartIndex += strlen("/scannedGroup/");
        //! per issue #286, syntax has been extended to include arguments
        //! /scannedGroup/GroupName?kind=AtomSocket&status=socket&valKind=onoff
        //! HERE we just need to search for the "?" and that will be the Group name.
        char *questionMark = strstr(smartIndex, "?");
        char scannedGroupBuffer[50];
        scannedGroupBuffer[0] = '\0';
        int *scannedGroupLen;
        //! see if any arguments (which we aren't using yet ..)
        if (questionMark)
        {
            int len = questionMark - smartIndex;
            strncpy(scannedGroupBuffer, smartIndex, len);
            scannedGroupBuffer[len] = '\0';
        }
        else
            strcpy(scannedGroupBuffer,smartIndex);
        char *scannedGroup = scannedGroupBuffer;
        //! save the group name (withoiut topic path) for use in the smrun below
        strcpy(_lastScannedGroupName, scannedGroup);
        
        SerialDebug.printf("scannedGroup = %s\n", scannedGroup);
        if (strcmp(scannedGroup,"_none")==0)
            main_setScannedGroupName((char*)"");
        else
            main_setScannedGroupName(scannedGroup);
        return false;
    }
    else if (containsSubstring(semanticMarker, "WIFI:"))
    {
        //! TODO.. parse this, create a JSON  { "ssid"  then call processJSONMessageMQTT ..
        char credentialsJSON[200];
        char credentialsString[200];

#ifdef ENCODED_URL
        //! SEEMS the syntax is not to encode vaues (although it's not perfectly clear)
        //!decode the URL (removing %20 etc)
        String decoded = String(semanticMarker);
        decoded = MQTT_urlDecode(decoded);
        SerialDebug.println(decoded);
        strcpy(credentialsString, decoded.c_str());
#else
        strcpy(credentialsString, semanticMarker);
#endif
        
        //!NOTE: the idea of sending this around is kinda catch 22. How can I tell you the SSID
        //!if you aren't connected to the internet..
        //!1.1.24 NOTE: these can be in any order.. so start with semanticMarker each time..
        /*
         WIFI:S:<SSID>;T:<WEP|WPA|blank>;P:<PASSWORD>;H:<true|false|blank>;
         */
        char *ssidIndex = strstr(credentialsString,"S:");
        ssidIndex += 2;
        char *semicolon = strstr(ssidIndex, ";");
        char ssidStr[50];
        int len = semicolon - ssidIndex;
        ssidStr[0] = '\0';
        strncpy(ssidStr, ssidIndex, len);
        ssidStr[len] = '\0';

        //!start over 1.1.24
        strcpy(credentialsString, semanticMarker);
        char *ssidPassIndex = strstr(credentialsString,"P:");
        ssidPassIndex+= 2;
        semicolon = strstr(ssidPassIndex, ";");
        len = semicolon - ssidPassIndex;
        char ssidPassStr[50];
        ssidPassStr[0] = '\0';
        strncpy(ssidPassStr, ssidPassIndex, len);
        ssidPassStr[len] = '\0';

        ///!put together into JSON. case sensitive..
        sprintf(credentialsJSON, "{'ssid':'%s','ssidPassword':'%s'}", ssidStr, ssidPassStr);
        SerialDebug.println(credentialsJSON);

        processJSONMessageMQTT(credentialsJSON, TOPIC_TO_SEND);
        
        /*
         6.16.25 A successful scan look like:
         ***validScannedSM = 'WIFI:S:SunnyWhiteriver;T:WPA/WPA2;P:sunny2021;H:;'
         ATOM_processSemanticMarker(WIFI:S:SunnyWhiteriver;T:WPA/WPA2;P:sunny2021;H:;, https://SemanticMarker.org/bot/smart?uuid=QHmwUurxC3&flow=1735086442396&flowCat=Scotty&name=SMART_DeviceM5AtomSocketOutside)
              {'ssid':'SunnyWhiteriver','ssidPassword':'sunny2021'}
                 processJSONMessageMQTT: '{'ssid':'SunnyWhiteriver','ssidPassword':'sunny2021'}'
                  {
                    "ssid": "SunnyWhiteriver",
                      "ssidPassword": "sunny2021"
                  }
         */
        //! Set the credentials for the WIFI using this format
        return false;
    }
    
    else if (containsSubstring(semanticMarker, "/credentials"))
    {
        //!   ./bot/credentials?ssid=X&ssidpass=Y....
#ifdef ESP_M5
        //! currently 9.28.23 no reason for feeder to decode base64 messages. That could change if a stored procedure..
        //! 12.30.23 .. changing to query parameters
        
        char *smartIndex = strstr(semanticMarker,"/credentials");
        smartIndex += strlen("/credentials");
        char *credentialsString = smartIndex;
        SerialDebug.printf("credentials = %s\n", credentialsString);
        
        //! parse .. creating JSON and send it onto the parseJSON
        parseQueryArgs(credentialsString, false);
        
        
        // syntax:  /bot/credentials/base64JSON   or
        //          WIFI convention.. not base64
//#define DECODE_BASE64_HERE
#ifdef DECODE_BASE64_HERE
        char plaintext_out[400];
        int len = strlen(credentialsString);
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
    else if (containsSubstring(semanticMarker, "/setJSON"))
    {
        //! 1.1.24 support setJSON as well..
        //!   ./bot/setJSON?ssid=X&ssidpass=Y....
        //! currently 9.28.23 no reason for feeder to decode base64 messages. That could change if a stored procedure..
        //! 12.30.23 .. changing to query parameters
        
        char *smartIndex = strstr(semanticMarker,"/setJSON");
        smartIndex += strlen("/setJSON");
        char *credentialsString = smartIndex;
        SerialDebug.printf("setJSON = %s\n", credentialsString);
        
        //! parse .. creating JSON and send it onto the parseJSON
        parseQueryArgs(credentialsString, true);
        
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
            
            SerialDebug.printf("1. *** WIFI FEEDING since inside SM (device=%s)\n", device);
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
        return true;
    }
    else if (containsSubstring(semanticMarker,"feeddevice"))
    {
        char *device = rindex(semanticMarker,'/');
        if (device) device++; // skip over the "/"
        SerialDebug.printf("2. *** WIFI FEEDING since inside SM (device=%s)\n", device);
        
        ///feed always  (done after the code below..)
        main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
        return true;
    }
    else if (containsSubstring(semanticMarker,"feedguest"))
    {
        SerialDebug.println("3. *** WIFI FEEDING since inside SM");
        
        ///feed always  (done after the code below..)
        main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
        return true;
    }
    else if (containsSubstring(semanticMarker,"feed"))
    {
        SerialDebug.println("4. *** WIFI FEEDING since inside SM");
        
        ///feed always  (done after the code below..)
        main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
        return true;
    }
    //! 1.7.24 if non null then valid group topic eg.  /usersP/group/GROUP_NAME
    char *groupTopic = main_getScannedGroupNameTopic();
    
    char getCommand[MAX_SM];
    char *username = main_getUsername();
    char *password = main_getPassword();
    char *scannedDeviceName = main_getScannedDeviceName();
    SerialDebug.println(username);
    SerialDebug.println(password);
    SerialDebug.println(scannedDeviceName);
    
    //! TODO: syntax for an /optimize?uuid=x&flow=y&cmd=feed
    //! The MQTT is much faster.. Lets only use MQTT for the 'device' specified (to see this difference)
    //    strcpy(_lastSemanticMarker,"https://SemanticMarker.org/bot/smart?uuid=QHmwUurxC3&flow=1674517131429");
    boolean scannedDeviceValid = false;
    //! this will be a cache of commands to try..
    //! cmd:feed,  cmddevice,feed,device
    if (scannedDeviceName && strlen(scannedDeviceName) > 0)
    {
        //! dev: <dev>, cmd: feed
        //! cmd: feed
        scannedDeviceValid = true;
    }
    
    //! 1.7.24  For now only SMART buttons will use the GROUP feature (and only if local) not the smrun
    // SMART buttons..
    if (containsSubstring(semanticMarker,"/smart"))
    {
        //! try a call..
        //! call 'smrun' with URL query parameters, like username/password/device
        //! /bot/smrun?uuid=x&flownum=y&username=X&password=y&device=z
        //! HERE .. we add the Username, Password and Device name.. parameters
 
        
#define TRY_OPTIMIZE
#ifdef  TRY_OPTIMIZE

        //! this is the cached (in code) SMART buton for feed.
        //! @See https://SemanticMarker.org/bot/smart?uuid=QHmwUurxC3&flow=1674517131429
        if (containsSubstring(semanticMarker, "QHmwUurxC3") && containsSubstring(semanticMarker,"1674517131429"))
        {
            //! this will be a cache of commands to try..
            //! cmd:feed,  cmddevice,feed,device
            if (scannedDeviceValid)
            {
                //! dev: <dev>, cmd: feed
                //! cmd: feed
                sprintf(getCommand, "{'dev':'%s', 'cmd':'feed'}", scannedDeviceName);
                if (groupTopic)
                    sendMessageStringTopic_mainModule(getCommand, groupTopic);
                else
                    sendMessageString_mainModule(getCommand);
                return true;
                
            }
            else
            {
                //   sprintf(getCommand, "{'cmd':'feed'}");
                return false;
            }
        }
        //! this is the cached (in code) SMART buton for toggle socket power.
        //! @See https://SemanticMarker.org/bot/smart?uuid=QHmwUurxC3&flow=1703806697279
        else if (containsSubstring(semanticMarker, "QHmwUurxC3") && containsSubstring(semanticMarker,"1703806697279"))
            //! SMART_ToggleSocket_Device
        {
            //! this is the togglesocket command (versus turn on/off)
            //! https://iDogWatch.com/bot/cmddevice/USERNAME/PASSWORD/DEVICE_1/togglesocket
            //! this will be a cache of commands to try..
            //! cmd:feed,  cmddevice,feed,device
            if (scannedDeviceValid)
            {
                //! dev: <dev>, cmd: feed
                //! cmd: feed
                sprintf(getCommand, "{'dev':'%s', 'cmd':'togglesocket'}", scannedDeviceName);
                if (groupTopic)
                    sendMessageStringTopic_mainModule(getCommand, groupTopic);
                else
                    sendMessageString_mainModule(getCommand);
                return true;
                
            }
            else
            {
                //   sprintf(getCommand, "{'cmd':'feed'}");
                return false;
            }
        }
        
#endif //optimize
    }
    //! only here if not optimized
    if (containsSubstring(semanticMarker,"/smart") || containsSubstring(semanticMarker,"/smflowinfo"))
    {
#ifdef USE_REST_MESSAGING
        //! call the /smrun which an HTTPS call to the SemanticMarker.org/bot site, and then decoded
        //! .. etc  Slower
        char *smartIndex;
        if (containsSubstring(semanticMarker,"/smflowinfo"))
        {
            smartIndex = strstr(semanticMarker,"/smflowinfo");
            //! move past /smart
            smartIndex += strlen("/smflowinfo");
        }
        else
        {
            smartIndex = strstr(semanticMarker,"/smart");
            //! move past /smart
            smartIndex += strlen("/smart");
        }
        strcpy(getCommand, "/bot/smrun");
        strcat(getCommand, smartIndex);
#else
        // replace the smart or smflowinfo with smrun
        strcpy(getCommand, semanticMarker);
#endif
        //! NOTE: this assumes smartIndex has parameters like: " /smart?uuid=x&flownum=y"
        //! WITHOUT the smart/  it's smart?uuid...
        //TODO: don't add these if already there (in the scanned semantic marker)
        strcat(getCommand, "&username=");
        strcat(getCommand, username);
        strcat(getCommand, "&password=");
        strcat(getCommand, password);
        if (scannedDeviceName && strlen(scannedDeviceName) > 0)
        {
            strcat(getCommand, "&device=");
            strcat(getCommand, scannedDeviceName);
        }
        //!tack on the raw group name, not the groupTopic which is full path: usersP/group/NAME
        if (groupTopic)
        {
            strcat(getCommand, "&group=");
            strcat(getCommand, _lastScannedGroupName);
        }

        //!REST is not working, so now sending to TOMCAT on KnowledgeShark.me:8080 (as HTTP not HTTPS)
#ifdef USE_REST_MESSAGING
        //! if REST worked, the message will be "/bot/smrun/..."
        //! Otherwise, the semantic marker is not changed so it stays as "smart"...
        //! 3.17.24 no async
#define TRY_ASYNC
#ifdef  TRY_ASYNC
        //!send an async call with a string parameter. This will set store the value and then async call the command (passing the parameter)
        //!These are the ASYNC_CALL_PARAMETERS_MAX
        main_dispatchAsyncCommandWithString(ASYNC_REST_CALL_MESSAGE_PARAMETER, getCommand);
        
#else
        sendSecureRESTCall(getCommand);
#endif //async
        
#else //no REST
        
        //! right now it shows smart of smflowinfo .. 
        SerialDebug.println("send as DOCFOLLOW");
        SerialDebug.println(getCommand);
#ifdef NOT_NEEDED
        char buffer[MAX_SM];
        
        // send as doc follow..  "/bot/smrun...."
        sprintf(buffer, "https://SemanticMarker.org%s",getCommand);
        SerialDebug.println(buffer);
    //    sendSemanticMarkerDocFollow_mainModule(buffer);
    //
        //! 3.25.24 try the http to tomcat, then it sends https to node-red
        publishSMRunMessage(buffer);
#else
        //! 3.25.24 try the http to tomcat, then it sends https to node-red
        publishSMRunMessage(getCommand);
#endif
        
#endif //use REST
        return true;
    }
    //! already an /smrun so call is..
    else if (containsSubstring(semanticMarker,"/smrun"))
    {
        SerialDebug.println(" ****** SHOULDN't have a smrun ...");
        
        //! 3.25.24 try the http to tomcat, then it sends https to node-red
//        publishSMRunMessage(semanticMarker);
        return true;
    }
    return true;
}

//! parse the query args .. ?<arg>=<val>{&<arg>=<val>}
//! 1.1.24 if useAnyArgs then just create JSON from all the arg-val without remapping
void parseQueryArgs(char *credentialsStringInput, boolean useAnyArgs)
{
    char credentialsJSON[300];
    //! init the JSON
    strcpy(credentialsJSON,"{");
    int countArgs = 0;
    int bufferLen = 100;

    //! parse the query args .. ?<arg>=<val>{&<arg>=<val>}
    
    
    char *argNames[]       = {(char*)"ssid",(char*)"ssidpass",(char*)"username",(char*)"password", (char*)"location", (char*)"deviceName"};
    //! mapping to the JSON needed. NOTE: Case sensitive  ssidPassword
    char *argOutputNames[] = {(char*)"ssid",(char*)"ssidPassword",(char*)"mqtt_user",(char*)"mqtt_password", (char*)"location", (char*)"deviceName"};
    int argNamesLen = sizeof(argNames)/sizeof(char*);
   
    //!decode the URL (removing %20 etc)
    String decoded = String(credentialsStringInput);
    decoded = MQTT_urlDecode(decoded);
    SerialDebug.println(decoded);
    char credentialsStringStorage[MAX_SM];
    strcpy(credentialsStringStorage, decoded.c_str());
    char *credentialsString = credentialsStringStorage;
    
    // eat the & or ?
    if (credentialsString[0] == '?')
        credentialsString++;
    while (credentialsString)
    {
        //! keep going while &<arg>
        
        // look for the next '='
        char *equalIndex = strstr(credentialsString, "=");
        if (!equalIndex)
        {
            SerialDebug.println("** Bad syntax of URL");
            // maybe bad syntax.. exit loop
            exit;
        }
        int argLen = equalIndex - credentialsString;
        char argString[bufferLen];
        for (int i=0;i<bufferLen;i++) argString[i] = '\0';
        strncpy(argString,credentialsString,argLen);
        argString[argLen] = '\0';
        // step over '='
        if (equalIndex)
            equalIndex++;
        
        // move pointer
        credentialsString = equalIndex;

        //! look for value (to end of line, or next '&')
        char *nextAmper = strstr(credentialsString,"&");
        char valString[bufferLen];
        for (int i=0;i<bufferLen;i++) valString[i] = '\0';
        //strcpy(valString,"");
        if (nextAmper)
        {
            // more
            int valLen = nextAmper - credentialsString;
            strncpy(valString,credentialsString,valLen);
            valString[valLen] = '\0';

            //! eat the '&'
            credentialsString += valLen + 1;
        }
        else
        {
            strcpy(valString,credentialsString);
            //! done.. so null
            credentialsString = NULL;
        }
        
        //! if useAnyArgs then
        if (useAnyArgs)
        {
            // if 2nd on put a ',' there
            if (countArgs > 0)
                strcat(credentialsJSON, ",");
            
            char oneArgVal[100];
            // argOutpuNames might be different (like username => mqtt_user)
            sprintf(oneArgVal,"'%s':'%s'", argString, valString);
            // add this oneline
            strcat(credentialsJSON, oneArgVal);

            countArgs++;
        }
        else
        {
            // look for matching arg (in any order)
            for (int i=0; i< argNamesLen; i++)
            {
                if (strcmp(argNames[i], argString)==0)
                {
                    char oneArgVal[100];
                    SerialDebug.printf(" ** Matched %s == %s\n", argNames[i], valString);
                    // argOutpuNames might be different (like username => mqtt_user)
                    sprintf(oneArgVal,"'%s':'%s'", argOutputNames[i], valString);
                    
                    // if 2nd on put a ',' there
                    if (countArgs > 0)
                        strcat(credentialsJSON, ",");
                    // add this oneLine
                    strcat(credentialsJSON, oneArgVal);
                    
                    countArgs++;
                    
                }
            }
        }
    }
    if (!useAnyArgs)
    {
        //! tack on mqtt_server and mqtt_port
        strcat(credentialsJSON, ",'mqtt_server':'idogwatch.com'");
        strcat(credentialsJSON, ",'mqtt_port':'1883'");

    }
    strcat(credentialsJSON, "}");
    SerialDebug.printf("JSON = %s\n", credentialsJSON);
    
    //! send whatever credentials were created
    processJSONMessageMQTT(credentialsJSON, TOPIC_TO_SEND);

}

#endif //ATOM_QRCODE_MODULE
