#ifndef WIFI_AP_MODULE_h
#define WIFI_AP_MODULE_h
/*
*******************************************************************************
  Copyright (c) 2021 by M5Stack
                 Equipped with M5StickC-Plus sample source code
                         配套  M5StickC-Plus 示例源代码
  Visit the website for more information：https://docs.m5stack.com/en/core/m5stickc_plus
  获取更多资料请访问：https://docs.m5stack.com/zh_CN/core/m5stickc_plus

  describe：WiFi_APModule  wifi连接
  date：2021/9/18

  Scott Version: 1.1.2022
*******************************************************************************
*/
#include "../../Defines.h"
//! 8.16.25 WIFI AP
//! 4.8.22. Called to do any state variables.
//! a presetup() approach  ..
//! THIS will make the doneWIFI call valid.. and has to be done before _done is looked at..
void preSetup_WIFI_APModule();
void setup_WIFI_APModule();
void loop_WIFI_APModule();

//!cleans the eprom info
void cleanEPROM_WIFI_APModule();

//! The rest are custom methods ..
//!
//!called to see if the WIFIModule has finished bootstrapping..
boolean doneWIFI_APModule_Credentials();

//!called to set the done flag (this could be from outside like if BLE updated credentials..)
void setDoneWIFI_APModuleFlag(boolean flag);

//!resets the credentials (maybe only the SSID .. keep the other)
void WIFI_APModule_ResetCredentials();

//! retrieves the JSON string
String WIFI_APModule_JsonOutputString();

//!returns true if the SSID is set, false otherwise. If set, then just exit..
boolean isSet_SSID_WIFICredentials();

//! cleans credentials
void clean_SSID_WIFICredentials();


//! sets the MQTT user/password. It's up to the code to decide who needs to know
void WIFI_APModule_updateMQTTInfo(char *ssid, char *ssid_password, char *username, char *password, char *guestPassword, char *deviceName, char *host, char *port, char *locationString);

#endif

