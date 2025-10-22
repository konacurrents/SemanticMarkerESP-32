//! \link WIFI_APModule
/*
 *******************************************************************************
 Copyright (c) 2021 by M5Stack
 Equipped with M5StickC-Plus sample source code
 配套  M5StickC-Plus 示例源代码
 Visit the website for more information：https://docs.m5stack.com/en/core/m5stickc_plus
 获取更多资料请访问：https://docs.m5stack.com/zh_CN/core/m5stickc_plus
 
 describe：WiFi connect.  wifi连接
 date：2021/9/18
 
 Scott Version: 1.1.2022
 *******************************************************************************
 */
//#include "../../Defines.h"
#include "WIFI_APModule.h"
//! 8.16.25 WIFI AP

#define TRY_FEED_FROM_AP

//NOTE: one solution is to create a JSON object, and stringify then pass to the MQTT code to set the credentials..
//! Will spif up the web page with some small images..
//! https://randomnerdtutorials.com/display-images-esp32-esp8266-web-server/
#include <ArduinoJson.h>

//!NOTE: 12.15.23  The WiFiClientSecure does NOT work with the access point shown below.
//!So we will go back to the WiFi.h
#ifdef X_USE_REST_MESSAGING
#include <WiFiClientSecure.h>
#else
#include <WiFi.h>
#endif
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include "WebServer.h"
#include <Preferences.h>

//!returns string for the header image
String headerImageString();

void WIFI_startWebServer();
void WIFI_setupMode();
String WIFI_makePage(String title, String contents);
String WIFI_urlDecode(String input);

//used when we server as a WAP (Wireless Access Point == PetTutorSetup_192_168_4_1)
const char *_WIFIWirelessAP = "192.168.4.1";

//web page afterwards, using the wifi chosen.. NOT USED..
//const char* _WIFIWirelessPage = "192.168.0.57";

//Define the address of the wireless AP.  定义无线AP的地址
const IPAddress _WIFIapIP(192, 168, 4, 1);

//Define the name of the created hotspot.  定义创建热点的名称
const char* _apSSID = "PetTutorSetup_192_168_4_1";

boolean _WIFI_SettingMode;
String _WIFISSIDList;
//Store the name of the wireless network.  存储无线网络的名称
String _WIFI_ssid;
//Store the password of the wireless network.  存储无线网络的密码
String _WIFI_password;
//last ssid saved
String _WIFI_ssid_last;

//!EPROM INFO  .. might be a length issue of the attribute name
#define AP_EPROM_NAME "ap_wifi"
#define WIFI_SSID     "1ws"
#define WIFI_PASSWORD "2wp"

//the last time it was saved - only used for the PULL DOWN web list
#define WIFI_SSID_LAST     "3sl"

#define MQTT
#ifdef MQTT
#define MQTT_PORT        "4pt"
#define MQTT_SERVER      "5sv"
#define MQTT_USER        "6mu"
#define MQTT_PASSWORD    "7mp"
#define MQTT_GUEST_PASSWORD   "8mg"

#define MQTT_TOPIC       "9mt"
#define MQTT_DEVICE_NAME "10md"
#define MQTT_LOCATION_NAME "11ml"

String _WIFI_mqttPort;
String _WIFI_mqttServer;
String _WIFI_mqttUser;
String _WIFI_mqttPassword;
String _WIFI_mqttGuestPassword;

String _WIFI_deviceName;
//char _WIFI_mqttTopic[100]; // or it's created from the 'user', etc user/bark
String _WIFI_mqttTopic;
String _WIFI_locationString;


#endif

//!rediscover (and prints)  the WIFIlist .. scanning each time.. so a page refresh will look again...
void rediscoverWIFIList();

//!create the javascript header to convert to lowercase, etc
String getJavascriptString();

//!remove spaces in a string
//https://stackoverflow.com/questions/16329358/remove-spaces-from-a-string-in-c
String removeSpaces(String input)
{
    String result = "";
    for (int i=0; i< input.length(); i++)
    {
        if (input[i] != ' ')
        {
            result += input[i];
        }
    }
    return result;
}

//! DNSServer dnsServer;.  webServer的类, port 80
WebServer WIFI_webServer(80);
//!client of the web server??
WiFiClient _WIFIClient;

//! wifi config store.  wifi配置存储的类
Preferences _WIFIPreferences;

boolean _doneWIFI_APModuleFlag = false;
//!called to set the done flag
void setDoneWIFI_APModuleFlag(boolean flag)
{
    _doneWIFI_APModuleFlag = flag;
}

//!called to see if the WIFIModule has finished bootstrapping..
boolean doneWIFI_APModule_Credentials()
{
    return _doneWIFI_APModuleFlag;
}

//!resets the credentials
void WIFI_APModule_ResetCredentials()
{
    _WIFI_SettingMode = true;
}

//! the json output string that contain the credentials
String _jsonOutputString;

//! retrieves the JSON string
String WIFI_APModule_JsonOutputString()
{
    return _jsonOutputString;
}


//!returns true if the SSID is set, false otherwise. If set, then just exit..
boolean isSet_SSID_WIFICredentials()
{
    _WIFIPreferences.begin(AP_EPROM_NAME);
    
    _WIFI_ssid = _WIFIPreferences.getString(WIFI_SSID);
    _WIFI_password = _WIFIPreferences.getString(WIFI_PASSWORD);
    
    //added for last, only used in the web
    _WIFI_ssid_last = _WIFIPreferences.getString(WIFI_SSID_LAST);
    _WIFIPreferences.end();
    
    boolean ssidSet = _WIFI_ssid && (_WIFI_ssid.length() > 0);
    SerialDebug.print("WIFI ssid = ");
    SerialDebug.println(_WIFI_ssid?_WIFI_ssid:"NULL");
    return ssidSet;
}

//!returns true if the SSID is set, false otherwise. If set, then just exit..
void clean_SSID_WIFICredentials()
{
    SerialDebug.println("clean_SSID_WIFICredentials");
    
    _WIFIPreferences.begin(AP_EPROM_NAME);
    
    //this is emptied .. as the only way we know to get into this mode is an empty SSID ..
    // THUS .. it doesn't matter if we had a value before.. UNLESS we save it somewhere else!
    _WIFI_ssid = "";
    _WIFIPreferences.putString(WIFI_SSID, _WIFI_ssid);
    _WIFIPreferences.end();
    //! 8.16.25 MQTT
    SerialDebug.println("call cleanMQTTpasswordsUpdateInEPROM");
    //! 9.18.23 LA (after Eagle Rock bike ride, Van Morrison tomorrow)
    cleanMQTTpasswordsUpdateInEPROM();

    SerialDebug.println("cleaned EPROM WIFI ssid and MQTT passwords ** REBOOTING");
    //Restart MPU.  重启MPU
    ESP.restart();
    
}

//!cleans the eprom info
void cleanEPROM_WIFI_APModule()
{
    SerialDebug.println("cleanEPROM_WIFI_APModule");
    _WIFIPreferences.begin(AP_EPROM_NAME, false);  //readwrite..
    _WIFIPreferences.clear();
    _WIFIPreferences.end();
}

//! sets the MQTT user/password. It's up to the code to decide who needs to know
void WIFI_APModule_updateMQTTInfo(char *ssid, char *ssid_password, char *username, char *password, char *guestPassword, char *deviceName, char *host, char *port, char *locationString)
{
    SerialDebug.printf("wifi_apModule.updateMQTTInfo(%s, %s, %s,%s, %s, %s, %s)\n", ssid?ssid:"NULL", ssid_password?ssid_password:"NULL", username?username:"NULL", password?password:"NULL", guestPassword?guestPassword:"NULL", locationString?locationString:"NULL", deviceName?deviceName:"NULL");
    _WIFIPreferences.begin(AP_EPROM_NAME, false);  //readwrite..
    _WIFIPreferences.putString(WIFI_SSID, ssid);
    _WIFIPreferences.putString(WIFI_PASSWORD, ssid_password?ssid_password:"");

    _WIFIPreferences.putString(MQTT_PORT, port);
    _WIFIPreferences.putString(MQTT_SERVER, host);

    _WIFIPreferences.putString(MQTT_USER, username);
    _WIFIPreferences.putString(MQTT_PASSWORD, password?password:"");
    _WIFIPreferences.putString(MQTT_GUEST_PASSWORD, guestPassword?guestPassword:"");

    _WIFIPreferences.putString(MQTT_DEVICE_NAME, deviceName);
    _WIFIPreferences.putString(MQTT_LOCATION_NAME, locationString);

    _WIFIPreferences.end();
    
    SerialDebug.println("done wifi_apModule.updateMQTTInfo");
}

//! 4.8.22. Called to do any state variables.
//! a presetup() approach  ..
void preSetup_WIFI_APModule()
{
//#define ADDED_TO_INIT_WILL_TAIKE_OUT_FOR_NOW

#ifdef ADDED_TO_INIT_WILL_TAIKE_OUT_FOR_NOW
    _jsonOutputString = "";
    _doneWIFI_APModuleFlag = false;
#endif
    SerialTemp.printf("preSetup_WIFI_APModule: isSet(%s)\n", isSet_SSID_WIFICredentials()?"set":"notSet");
    
    if (isSet_SSID_WIFICredentials())
    {
        //!Exit setup().  退出setup()
        setDoneWIFI_APModuleFlag(true);
    }
    
}

//!sets the config in the EPPROM called wifi-config. NOTE: the _done isn't valid until after this code...
void setup_WIFI_APModule() {
    
    SerialTemp.printf("setup_WIFI_APModule: isSet(%s)\n", isSet_SSID_WIFICredentials()?"set":"notSet");
    
    if (isSet_SSID_WIFICredentials())
    {
        //!Exit setup().  退出setup()
        setDoneWIFI_APModuleFlag(true);
        return;
    }
    
    //NOTE: here is where we would look for SSID set (if empty .. then continue). Not worrying about MQTT
    _WIFIPreferences.begin(AP_EPROM_NAME);
    delay(10);
    
    //If there is no stored wifi configuration information, turn on the setting mode.  若没有已存储的wifi配置信息,则开启设置模式
    _WIFI_SettingMode = true;
    WIFI_setupMode();
}

//!main loop
void loop_WIFI_APModule() {
    if (_WIFI_SettingMode)
    {
    }
    //Check that there is no facility to send requests to the M5StickC Web server over the network.
    WIFI_webServer.handleClient();
}

//!starts the web server
//!Open the web service.  打开Web服务
void WIFI_startWebServer() 
{
    SerialDebug.println("wifi_apModule.WIFI_startWebServer");
    
    //If the setting mode is on.  如果设置模式处于开启状态
    if (_WIFI_SettingMode)
    {
        _WIFI_mqttServer = _WIFIPreferences.getString(MQTT_SERVER);
        _WIFI_mqttPort = _WIFIPreferences.getString(MQTT_PORT);
        _WIFI_mqttUser = _WIFIPreferences.getString(MQTT_USER);
        _WIFI_mqttPassword = _WIFIPreferences.getString(MQTT_PASSWORD);
        _WIFI_mqttGuestPassword = _WIFIPreferences.getString(MQTT_GUEST_PASSWORD);

        _WIFI_deviceName = _WIFIPreferences.getString(MQTT_DEVICE_NAME);
        _WIFI_locationString = _WIFIPreferences.getString(MQTT_LOCATION_NAME);

        //CREATE TOPIC HERE
#ifdef SPRINTF_WORKS   //NOTE: it should work as it's used all over the place..
        sprintf(_WIFI_mqttTopic, "usersP/bark/%s", _WIFI_mqttUser);
#else
        _WIFI_mqttTopic = "usersP/bark/" + _WIFI_mqttUser;
        
        //        strcpy(_WIFI_mqttTopic,"usersP/bark/");
        //        strcat(_WIFI_mqttTopic, &_WIFI_mqttUser[0]);
#endif
        SerialDebug.print("Visit Web Server: \n");
        
        //Output AP address (you can change the address you want through _WIFIapIP at the beginning).  输出AP地址(可通过开头的_WIFIapIP更改自己想要的地址)
        SerialDebug.print(WiFi.softAPIP());
        
        /********* ROOT == notFound *****************/
        WIFI_webServer.onNotFound([]() {
#ifdef DONT_USE_BASE_ADDRESS
            // String s = "<meta HTTP-EQUIV=\"REFRESH\" content=\"0; url=http://192.168.4.1/settings\">";
            
            String s = "<h1>ESP is now in Access Point mode</h1><p><a href=\"/settings\">Click to update WiFi Credentials</a></p>";
            //            s += "<img src=\"https://idogwatch.com/pettutor/iDogWatch.png\"  width=\"200\">";
            //            s += "<img src=\"https://idogwatch.com/pettutor/PetTutor.jpeg\"  width=\"500\">";
            

            
            WIFI_webServer.send(200, "text/html", WIFI_makePage("AP mode", s));
        });
        
        /******* /settings *******************/
        //AP web interface settings.  AP网页界面设置
        //NOTE: the <input name=X> has to have X known later to get the value back..
        WIFI_webServer.on("/settings", []() {
#endif

            addToTextMessages_displayModule("AP Credentials");

            SerialDebug.println("settings touched, ssid_last = ");
            String s = headerImageString();
            
            //!add the javascript
            s += getJavascriptString();
            
            //!this re-calculates the WIFI list
            rediscoverWIFIList();

            SerialDebug.println(_WIFI_ssid_last);


            
#ifdef ESP_M5
            s += "<h1>Wi-Fi Settings for ESP-M5 Device</h1>";
#endif
#ifdef ESP_32
            s += "<h1>Wi-Fi Settings for ESP-32 Device</h1>";
#endif

            s += "<p>Enter your password after selecting the visible SSID names";
//! ##47 https://github.com/konacurrents/ESP_IOT/issues/47
//!so referting back to this..
#define TRY_DATA_LIST
#ifdef TRY_DATA_LIST
s += " (or type missing WIFI). <i>*older web browsers may not show list, and only 2.4gHz WIFI supported</i></p>";
            s += "<button onClick=\"window.location.reload();\">Refresh to re-discover WIFI list</button> </p>";

#else
s += "<button onClick=\"window.location.reload();\">Refresh to re-discover WIFI list</button></p>";
#endif

            s += "<form method=\"get\" action=\"setap\" onsubmit=\"return validateForm()\">\n";
            s += "<label>SSID: </label>\n";

#ifdef TRY_DATA_LIST
            s += "<input list=\"ssid\"  class=\"form-control\" name=\"ssid\" id=\"ssid_val\" style=\"width:300px;\">";

            s += "<datalist id=\"ssid\" >";
            s += _WIFISSIDList;  //a list that is discovered..
            s += "</datalist>";
#else
            s += "<select name=\"ssid\" id=\"ssid_val\" length=\"64\" \">";
            s += _WIFISSIDList;  //a list that is discovered..
            s += "</select>";
#endif
            s += "<br>SSID Last: " + _WIFI_ssid_last;
            s += "<br>Password: <input name=\"pass\" id=\"pass\" length=64 type=\"text\" value=\"" + _WIFI_password + "\">";
            
            //hard wire for now... TODO remove this..
            _WIFI_mqttServer = "idogwatch.com";
            _WIFI_mqttPort = "1883";
            
#ifdef MQTT
            s += "<h2>MQTT User Settings</h2>";
             s += "MQTT user   : <input name=\"mqtt_user\" id=\"mqtt_user\" length=\"64\"  size=\"64\" type=\"text\" style=\"text-transform: lowercase\" value=\"" + _WIFI_mqttUser + "\">";
            s += "<br>MQTT pass    : <input name=\"mqtt_password\" id=\"mqtt_password\" length=\"64\" size=\"64\" type=\"text\" style=\"text-transform: lowercase\" value=\"" + _WIFI_mqttPassword + "\">";
            s += "<br>MQTT guestpass: <input name=\"mqtt_guestPassword\" id=\"mqtt_guestPassword\" length=\"64\" size=\"64\" type=\"text\" style=\"text-transform: lowercase\" value=\"" + _WIFI_mqttGuestPassword + "\">";
#ifdef SPRINTF_WORKS
            sprintf(_WIFI_mqttTopic, "usersP/bark/%s", _WIFI_mqttUser);
            String topic(_WIFI_mqttTopic);
#else
            _WIFI_mqttTopic = "usersP/bark/" + _WIFI_mqttUser;
            
            //            strcpy(_WIFI_mqttTopic,"usersP/bark/");
            //            strcat(_WIFI_mqttTopic, &_WIFI_mqttUser[0]);
#endif
            
            s += "<br>DeviceName : <input name=\"deviceName\" id=\"deviceName\" length=\"64\"  size=\"64\" type=\"text\" value=\"" + _WIFI_deviceName + "\">";
            s += "<br>Location in World (city,state): <input name=\"locationString\" id=\"locationString\" length=\"64\"  size=\"64\" type=\"text\" value=\"" + _WIFI_locationString + "\">";
            s += "<h2>MQTT Server Settings</h2>";
            s += "<br>MQTT Topic : " + _WIFI_mqttTopic;
            s += "<br>MQTT Server: <input name=\"mqtt_server\"  id=\"mqtt_server\" length=\"64\"  size=\"64\" type=\"text\" value=\"" + _WIFI_mqttServer + "\">";
            s += "<br>MQTT Port    : <input name=\"mqtt_port\" id=\"mqtt_port\" length=\"10\" size=\"10\" type=\"text\" value=\"" + _WIFI_mqttPort + "\">";
#ifdef ESP_M5
            {
                //! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
                boolean isBLEServer = getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE);
                String isBLEServerStr = isBLEServer?"1":"0";
                s += "<br><br>is BLE Server (type 1 or 0): <input name=\"isBLEServer\" id=\"isBLEServer\" length=\"1\" size=\"1\" type=\"text\" value=\"" + isBLEServerStr + "\">";
            }
#endif
#endif
            // s += "<br><input type=\"reset\">";
            s += "<br><br><input type=\"submit\"></form>";
            //submit will call /setap
            //	s += "<br><a href=\"http://192.168.4.1\">Restart</a>";
       
#ifdef M5_CAPTURE_SCREEN
            //!try to grab the screen as a BMP and post it ...
            s += "<a href=\"capture\">capture</a>";
#endif
            
#ifdef TRY_FEED_FROM_AP
            //! invokes the web server with "feed" which will be the "/feed" below
            s += "<br><h2><a href=\"feed\">Feed your dog right now</a></h2>";
#endif
            
//#define AP_DEBUG_MODE  (defined in Perferences..)
#ifdef  AP_DEBUG_MODE
            /********  APDebug ******************/
            
            SerialDebug.println("apDebug touched");
            
            s += "<br><br><h1>Diagnostic Information<br>";
            s += "<textarea rows='30' cols='100'>";
            
            //! show some information...
            s += "\nVERSON = " + String(VERSION);
          //  s += "WIFI_CREDENTIAL_1:" +  getPreference_mainModule(PREFERENCE_WIFI_CREDENTIAL_1_SETTING)  + "\n";
            
            //! 8.16.25 MQTT
            //! 9.16.23 some debug information
            //! retrieve the Configuration JSON string in JSON format..
            s += "\nConfig (minus password) = \n" + getJSONConfigString();
            //! retrieve the WIFIInfoString
            s += "\nLast AP reason = " + String(getPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING));

            s += "\n</textarea>";
            
            //! empty it now.
            resetPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING);
               
#endif

            
            WIFI_webServer.send(200, "text/html", WIFI_makePage("Wi-Fi & MQTT Settings", s));
        });
        
        /******** /setap ******************/
        //call this when the 'submit' touched..s
        WIFI_webServer.on("/setap", []() {
            SerialDebug.println("setAP touched");
            
            //String ssid = removeSpaces(WIFI_urlDecode(WIFI_webServer.arg("ssid")));
            //! as per #248 support SSID syntax (first will remove the spaces at start and end)
            String ssid = WIFI_urlDecode(WIFI_webServer.arg("ssid"));

            addToTextMessages_displayModule(ssid);

            SerialDebug.print("SSID: ");
            SerialDebug.println(ssid);
         
            //! Issue #305 don't remove spaces...
            //String pass = removeSpaces(WIFI_urlDecode(WIFI_webServer.arg("pass")));
            String pass = WIFI_urlDecode(WIFI_webServer.arg("pass"));

#ifdef MQTT
            _WIFI_mqttServer = removeSpaces(WIFI_urlDecode(WIFI_webServer.arg("mqtt_server")));
            _WIFI_mqttPort = removeSpaces(WIFI_urlDecode(WIFI_webServer.arg("mqtt_port")));
            _WIFI_mqttUser = removeSpaces(WIFI_urlDecode(WIFI_webServer.arg("mqtt_user")));
            _WIFI_mqttPassword = removeSpaces(WIFI_urlDecode(WIFI_webServer.arg("mqtt_password")));
            _WIFI_mqttGuestPassword = removeSpaces(WIFI_urlDecode(WIFI_webServer.arg("mqtt_guestPassword")));

            _WIFI_deviceName = removeSpaces(WIFI_urlDecode(WIFI_webServer.arg("deviceName")));
            //strip spaces
            _WIFI_deviceName = removeSpaces(_WIFI_deviceName);
            _WIFI_locationString = WIFI_urlDecode(WIFI_webServer.arg("locationString"));
            //CREATE TOPIC HERE
#ifdef SPRINTF_WORKS
            sprintf(_WIFI_mqttTopic, "usersP/bark/%s", _WIFI_mqttUser);
#else
            _WIFI_mqttTopic = "usersP/bark/" + _WIFI_mqttUser;
            
#endif
#ifdef ESP_M5
            {
                String isBLEServerString = removeSpaces(WIFI_urlDecode(WIFI_webServer.arg("isBLEServer")));
                boolean isBLEServerFlag = isBLEServerString.compareTo("1")==0;
                SerialDebug.printf("Setting isBLEServerFlag = %d\n", isBLEServerFlag);
                savePreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE, isBLEServerFlag);

            }

#endif
            
            SerialDebug.print("mqtt_server = "); SerialDebug.print(_WIFI_mqttServer?_WIFI_mqttServer:"NULL");
            SerialDebug.print(":");SerialDebug.println(_WIFI_mqttPort);
            SerialDebug.print("mqtt_user = "); SerialDebug.println(_WIFI_mqttUser?_WIFI_mqttUser:"NULL");
            SerialDebug.print("mqtt_password = "); SerialDebug.println(_WIFI_mqttPassword?_WIFI_mqttPassword:"NULL");
            SerialDebug.print("mqtt_guestPassword = "); SerialDebug.println(_WIFI_mqttGuestPassword?_WIFI_mqttGuestPassword:"NULL");

            SerialDebug.print("deviceName = "); SerialDebug.println(_WIFI_deviceName?_WIFI_deviceName:"NULL");
            SerialDebug.print("mqtt_topic = "); SerialDebug.println(_WIFI_mqttTopic?_WIFI_mqttTopic:"NULL");
            SerialDebug.print("location = "); SerialDebug.println(_WIFI_locationString?_WIFI_locationString:"NULL");

#endif
            
            SerialDebug.println(" **** Writing out SSID .. ");
            _WIFIPreferences.putString(WIFI_SSID, ssid?ssid:"");
            _WIFIPreferences.putString(WIFI_PASSWORD, pass?pass:"");
            
            //last ssid as well
            _WIFIPreferences.putString(WIFI_SSID_LAST, ssid?ssid:"");

#ifdef MQTT
            _WIFIPreferences.putString(MQTT_SERVER, _WIFI_mqttServer);
            _WIFIPreferences.putString(MQTT_PORT, _WIFI_mqttPort);
            _WIFIPreferences.putString(MQTT_USER, _WIFI_mqttUser);
            _WIFIPreferences.putString(MQTT_PASSWORD, _WIFI_mqttPassword);
            _WIFIPreferences.putString(MQTT_GUEST_PASSWORD, _WIFI_mqttGuestPassword);

            _WIFIPreferences.putString(MQTT_TOPIC, _WIFI_mqttTopic);
            _WIFIPreferences.putString(MQTT_DEVICE_NAME, _WIFI_deviceName);
            _WIFIPreferences.putString(MQTT_LOCATION_NAME, _WIFI_locationString);

#endif
            DynamicJsonDocument myObject(1024);
            myObject["ssid"] = ssid;
            myObject["ssidPassword"] = pass;
            myObject["mqtt_server"] = _WIFI_mqttServer;
            myObject["mqtt_port"] = _WIFI_mqttPort;;
            myObject["mqtt_user"] = _WIFI_mqttUser;
            myObject["mqtt_password"] = _WIFI_mqttPassword;
            myObject["mqtt_guestPassword"] = _WIFI_mqttGuestPassword;

            myObject["mqtt_topic"] = _WIFI_mqttTopic;
            myObject["deviceName"] = _WIFI_deviceName;
            myObject["location"] = _WIFI_locationString;

            serializeJson(myObject, _jsonOutputString);
            
            SerialDebug.print("JSON = ");
            SerialDebug.println(_jsonOutputString);
            // Store wifi config.  存储wifi配置信息
            SerialDebug.println("Write to WIFI_AP_Module.eprom done!");
            
            //now let the other code work..
            setDoneWIFI_APModuleFlag(true);
            
            String s = headerImageString();
            s += "<h1>Credential Setup Complete.</h1><ol><li>Reconnect to your favorite WIFI\n<li>Then to access device commands, click: ";
            s += "<a href=\"https://idogwatch.com/bot/userpage2/"+ _WIFI_mqttUser + "/" + _WIFI_mqttPassword + "\">Feeding User Page</a></ol>";
            s += "<br><a href=\"http://192.168.4.1\">Restart Credentials (after connecting to PetTutorSetup_192_168_4_1)</a>";
            WIFI_webServer.send(200, "text/html", WIFI_makePage("iDogWatch Page", s));
            //delay(2000);
            delay(200); //??
            
        });
#ifdef M5_CAPTURE_SCREEN
        /******** /capture ******************/
        WIFI_webServer.on("/capture", []() {
            
            SerialDebug.println("capture touched");
            WiFiClient _WIFIClient = WIFI_webServer.client();

            //!The WIFI client

            _WIFIClient.println("HTTP/1.1 200 OK");
            _WIFIClient.println("Content-type:image/bmp");
            _WIFIClient.println();
            M5Screen2bmp(_WIFIClient);
            _WIFIClient.println();
           // _WIFIClient.stop();

        });
#endif
    

#ifdef TRY_FEED_FROM_AP
        /******** /feed ******************/
        WIFI_webServer.on("/feed", []() {
            
            SerialDebug.println("feed touched");
            //!send a feed command 's'
            processClientCommandChar_mainModule('s');

            //shows that we clicked the button... (vs waiting for another to show it..)
            showText_mainModule("FEED..");
            
            String s = headerImageString();
            s += "<h1>Your dog was just feed via local WIFI without bluetooth";
            s += "<br><br><a href=\"/feed\">Feed Again</a>";
            s += "<br><br><a href=\"/setup\">Return to Setup Credentials</a>";

            WIFI_webServer.send(200, "text/html", WIFI_makePage("iDogWatch Page", s));
            //delay(2000);
            delay(200); //??
            
        });
#endif
    }
    
    //Start web service.  开启web服务
    WIFI_webServer.begin();
}

//!print the WIFIlist .. scanning each time..
void rediscoverWIFIList()
{
    int n = WiFi.scanNetworks();
    delay(100);
    SerialTemp.println(" *** WIFI LIST: ");
#ifdef  AP_DEBUG_MODE
    //!try debug
    appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, " *** WIFI DISCOVERED");

#endif
    _WIFISSIDList = "";
    //Save each wifi name scanned to ssidList.  将扫描到的每个wifi名称保存到ssidList中
    for (int i = 0; i < n; i++) {
        SerialTemp.print("SSID: ");
        SerialTemp.print(WiFi.SSID(i));
#ifdef  AP_DEBUG_MODE
        //!try debug
        appendPreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, WiFi.SSID(i));
#endif
        
        SerialTemp.print("  Signal: ");
        SerialTemp.print(WiFi.RSSI(i));
        SerialTemp.print(" dBm, channel: ");
        SerialTemp.println(WiFi.channel(i));

        _WIFISSIDList += "<option value=\"";
        _WIFISSIDList += WiFi.SSID(i);
        _WIFISSIDList += "\"";
#ifndef TRY_DATA_LIST
        if(_WIFI_ssid_last.compareTo(WiFi.SSID(i)) == 0)
        {
            _WIFISSIDList += " selected";
        }
#endif
        _WIFISSIDList += ">";
        _WIFISSIDList += WiFi.SSID(i);
        _WIFISSIDList += "</option>";
    }
#ifdef  AP_DEBUG_MODE
    //!try debug\
    storePreference_mainModule(PREFERENCE_DEBUG_INFO_SETTING, "");
#endif
}

//!Setup the WIFI access point
void WIFI_setupMode() {
    SerialDebug.println("wifi_apModule.WIFI_setupMode");
    
    //Set Wi-Fi mode to WIFI_MODE_STA.  设置Wi-Fi模式为WIFI_MODE_STA
    WiFi.mode(WIFI_MODE_STA);
    
    //Disconnect wifi connection.  断开wifi连接
    WiFi.disconnect();
    //delay(100);
    
    
    //create an access point
    WiFi.softAPConfig(_WIFIapIP, _WIFIapIP, IPAddress(255, 255, 255, 0));
    
    //Turn on Ap mode.  开启Ap模式
    WiFi.softAP(_apSSID);
    
    //Set WiFi to soft-AP mode.  设置WiFi为soft-AP模式
    WiFi.mode(WIFI_MODE_AP);
    
    //Open the web service.  打开Web服务
    WIFI_startWebServer();

    addToTextMessages_displayModule(_apSSID);

    SerialDebug.printf("\nStarting Access Point at \n\"%s\"\n", _apSSID);
    
}

//!makes a webpage with the title, and contents, already in HTML
String WIFI_makePage(String title, String contents) {
    
    SerialDebug.print("WIFI_makePage:");
    SerialDebug.println(title);
    
    String s = "<!DOCTYPE html><html><head>";
    s += "<meta name=\"viewport\" content=\"width=device-width,user-scalable=0\">";
    s += "<title>";
    s += title;
    s += "</title></head><body>";
    s += contents;
    s += "</body></html>";
    return s;
}

//!Decode the URL
String WIFI_urlDecode(String input) {
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

//!returns string for the header image. This was about the smallest image without incuring much bits in the size of the BIN file
String headerImageString()
{
    //#include "idogwatch.base64"

    String imageString = "<img width=\"200\" src=\"data:image/png;base64,";
    imageString +=
    "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";

    imageString += "\">";
    
   
    imageString += "<br>";

    return imageString;
    
//data:image/png;base64,iVBORWOKGgOAAAANSUhEUgAAAMgAAADICAYAAACtWK6EAA
}

//!create the javascript header to convert to lowercase, etc
String getJavascriptString()
{
    String s = "";

    s += "<script type=\"text/javascript\">\n";

    s += "var _validatedForm  = true;\n";

    s += "//!convert to lower case and strip spaces, using the valId, and setting valId if valid. if toLower==-1 then fix SSID\n";
    s += "function checkAndConvertText(valId, msg, toLower)\n";
    s += "{\n";
    s += "var val = document.getElementById(valId).value;\n";
    s += "if (!val)\n";
    s += "{\n";
    s += "alert(msg  + \" required\");\n";
    s += "    _validatedForm = false;\n";
    s += "    return;\n";
    s += "}\n\n";
#define TRY_SSID_AMPER
#ifdef TRY_SSID_AMPER
    s += "if (toLower == -1) {\n";
    s += "   var ssidStr = val.replace(/&/g,'%26');\n";
    s += "   document.getElementById(valId).value = ssidStr;\n";
    s += "   return true; \n";
    s += "}\n";
#endif
    s += "//!strip all spaces\n";
    //NOTE: the \ in front of \s
    s += "var str = val.replace(/\\s+/g, '');\n";
    
    s += "if (toLower)\n";
    s += "{\n";
    s += "//!convert to lower case\n";
    s += "str = str.toLowerCase();\n";
    s += "}\n\n";

    s += "document.getElementById(valId).value = str;\n";
    s += "return true;\n";
    s += "}\n\n";

    s += "//!checks a string to make sure a number..\n";
    s += "function checkNumber(valId, msg)\n";
    s += "{\n";
    s += "var num = document.getElementById(valId).value;\n";
    s += "if (isNaN(num) || num < 1)\n";
        s += "{\n";
    s += "alert(msg + \": \" + num + \" must be valid number\");\n";
    s += "_validatedForm = false;\n";
        s += "}\n\n";
    s += "}\n\n";

    
    
   
   
    
    s += "//!v8 confirms the values the user entered\n";
    s += "//! set _validatedForm = false if not confirmed\n";
    s += "function confirmValues()\n";
    s += "{\n";
    s += "   var val;\n";
    s += "   var msg = \"Please confirm values\\n\";\n";
    s += "   val = document.getElementById(\"ssid_val\").value;\n";
    s += "   msg += \"WIFI SSID  = '\" + val + \"'\\n\";\n";
    s += "   val = document.getElementById(\"pass\").value;\n";
    s += "   msg += \"WIFI PASS  = '\" + val + \"'\\n\";\n";
    s += "   val = document.getElementById(\"mqtt_user\").value;\n";
    s += "   msg += \"MQTT USER = '\" + val + \"'\\n\";\n";
    s += "   val = document.getElementById(\"mqtt_password\").value;\n";
    s += "   msg += \"MQTT PASS = '\" + val + \"'\\n\";\n";
    s += "   val = document.getElementById(\"mqtt_guestPassword\").value;\n";
    s += "   msg += \"MQTT Guest PASS = '\" + val + \"'\\n\";\n";
    s += "   val = document.getElementById(\"deviceName\").value;\n";
    s += "   msg += \"Device Name = '\" + val + \"'\\n\";\n";
    s += "   val = document.getElementById(\"locationString\").value;\n";
    s += "   msg += \"Location in world = '\" + val + \"'\\n\";\n";
    s += "   _validatedForm = confirm(msg);\n";
    s += "}\n\n";




    s += "//! checks a few fields, and makes sure they are not null, and convert to lowerCase and strip spaces\n";
    s += "function validateForm()\n";
    s += "{\n";
    s += "//! set default that alls good\n";
    s += "_validatedForm = true;\n";
    s += "var toLower = true;\n";
    s += "var keepCase = false;\n";
    
    s += "// These will set _validatedFrom to false if not valid\n";
    //!issue #248 support full SSID name syntax (or at least let the user type anything.)
  //  s += "checkAndConvertText(\"ssid_val\",\"WIFI SSID\", keepCase);\n";
#ifdef TRY_SSID_AMPER
    s += "checkAndConvertText(\"ssid_val\",\"WIFI SSID\", -1);\n";

#endif
    s += "checkAndConvertText(\"mqtt_user\",\"MQTT User Name\", toLower);\n";
    s += "checkAndConvertText(\"mqtt_password\",\"MQTT Password\", toLower);\n";
    s += "checkAndConvertText(\"mqtt_guestPassword\",\"MQTT Guest Password\", toLower);\n";
    s += "checkAndConvertText(\"deviceName\",\"Device Name\", keepCase);\n";
    
    s += "checkAndConvertText(\"mqtt_server\",\"MQTT Server Name\", toLower);\n";
    s += "checkNumber(\"mqtt_port\",\"MQTT Port\");\n";
    s += "\n";
    s += "if (_validatedForm) confirmValues();\n";
    s += "return _validatedForm;\n";
    
    s += "}\n\n";

    s += "</script>\n";
 
    return s;
}

