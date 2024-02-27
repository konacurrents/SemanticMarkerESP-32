\page DisplayModuleREADME

# SemanticMarker&trade;

![SemanticMarkeri2](SMStatus1.PNG)
![SemanticMarker](SMStatus2.PNG)
![SemanticMarker3](IMG_4616.PNG)

NOT WORKING
setupESP_M5
M5Core2 initializing...axp: vbus limit off
axp: gpio1 init
axp: gpio2 init
axp: rtc battery charging enabled
axp: esp32 power voltage was set to 3.35v
axp: lcd backlight voltage was set to 2.80v
axp: lcd logic and sdcard voltage preset to 3.3v
axp: vibrator voltage preset to 2v

WORKING
setupESP_M5
M5Core2 initializing...axp: vbus limit off
axp: gpio1 init
axp: gpio2 init
axp: rtc battery charging enabled
axp: esp32 power voltage was set to 3.35v
axp: lcd backlight voltage was set to 2.80v
axp: lcd logic and sdcard voltage preset to 3.3v
axp: vibrator voltage preset to 2v
touch: FT6336 ready (fw id 0x10 rel 1, lib 0x300E)
E (1626) I2S: i2s_driver_uninstall(1957): I2S port 0 has not installed
OK
M5CORE2_MODULE
Version-(4.2)-2.8.24-ESP_M5CORE2_OurM5Dsp_Touch_30KPacific_KeyUnit
Compiled Feb  8 2024 @ 21:29:09
setup_mainModule
main_setScannedDeviceName()
main_setScannedGroupName()
readPreferences_mainModule(MainPrefs)
 **** setting PREFERENCE_MAIN_BLE_SERVER_VALUE[1]: 1
setOnBootPreferences_mainModule
 ** #define Module Configuration **


--------

setupESP_M5
M5Core2 initializing...touch: FT6336 ready (fw id 0x05 rel 1, lib 0x3008)
E (2292) I2S: i2s_driver_uninstall(1957): I2S port 0 has not installed
OK
M5CORE2_MODULE
Version-(4.2)-2.8.24-ESP_M5CORE2_OurM5Dsp_Touch_30KPacific_KeyUnit
Compiled Feb  9 2024 @ 14:17:34
setup_mainModule
main_setScannedDeviceName()
main_setScannedGroupName()
readPreferences_mainModule(MainPrefs)
 **** setting PREFERENCE_MAIN_BLE_SERVER_VALUE[1]: 1
setOnBootPreferences_mainModule
 ** #define Module Configuration **
[x] ESP_M5
[ ] ESP_M5_CAMERA
[ ] ESP_32
[x] M5CORE2_MODULE
[x] USE_MQTT_NETWORKING
[x] USE_BLE_SERVER_NETWORKING
[x] USE_BLE_CLIENT_NETWORKING
[x] USE_BUTTON_MODULE
[ ] M5BUTTON_MODULE
[x] USE_WIFI_AP_MODULE
[ ] USE_STEPPER_MODULE
[ ] USE_UI_MODULE
[x] USE_DISPLAY_MODULE
[ ] BOARD
[ ] USE_SPIFF_MODULE
[ ] USE_SMART_CLICKER
[x] SERIAL_DEBUG_ERROR
[x] SERIAL_DEBUG_DEBUG
[x] SERIAL_DEBUG_MINIMAL
******************
Version-(4.2)-2.8.24-ESP_M5CORE2_OurM5Dsp_Touch_30KPacific_KeyUnit
STEPPER_KIND: 3  1=UNO,2=MINI,3=TUMBLER
PREFERENCE_TIMER_INT_SETTING: 5
STEPPER_AUTO_FEED: 0
STEPPER_JACKPOT_FEED: 0
PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING: 45.000000
STEPPER_FEEDS_PER_JACKPOT: 3
DISPLAY_SCREEN_TIMEOUT: 480
PREFERENCE_HIGH_TEMP_POWEROFF_VALUE: 80
PREFERENCE_IS_MINIMAL_MENU_SETTING: 1
PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE: 1
PREFERENCE_NO_BUTTON_CLICK_POWEROFF_SETTING: 10000
PREFERENCE_MAIN_GATEWAY_VALUE: 0
PREFERENCE_SENSOR_TILT_VALUE: 1
WIFI_CREDENTIAL_1: {'ssid':'SunnyWhiteriver','ssidPassword':'sunny2021'}
WIFI_CREDENTIAL_2: NONE
PREFERENCE_PAIRED_DEVICE_SETTING: NONE
PREFERENCE_PAIRED_DEVICE_ADDRESS_SETTING: 
PREFERENCE_DEVICE_NAME_SETTING: M5Core3
PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING: 1
PREFERENCE_MAIN_BLE_CLIENT_VALUE: 1
PREFERENCE_MAIN_BLE_SERVER_VALUE: 0
PREFERENCE_FIRST_TIME_FEATURE_SETTING: 0
PREFERENCE_SCREEN_COLOR_SETTING: 0
PREFERENCE_SUB_DAWGPACK_SETTING: 1
PREFERENCE_STEPPER_CLOCKWISE_MOTOR_DIRECTION_SETTING: 0
PREFERENCE_SENDWIFI_WITH_BLE: 0
PREFERENCE_ONLY_GEN3_CONNECT_SETTING: 0
PREFERENCE_SUPPORT_GROUPS_SETTING: 0
PREFERENCE_GROUP_NAMES_SETTING: #
PREFERENCE_DEV_ONLY_SM_SETTING: 0
PREFERENCE_ATOMS_SETTING: socket=off
PREFERENCE_ATOM_KIND_SETTING: M5AtomScanner
PREFERENCE_SM_ON_PIR_SETTING: 0
PREFERENCE_SM_COMMAND_PIR_SETTING: {'set':'socket','val':'on'}
PREFERENCE_SM_COMMAND_PIR_OFF_SETTING: {'set':'socket','val':'off'}
PREFERENCE_ATOM_SOCKET_GLOBAL_ONOFF_SETTING: 1
PREFERENCE_M5Core2_SETTING:
WIFI_CREDENTIAL: 
setup_Sensors_mainModule
setup_M5Core2Module
KeyUnitSensorClass init _KeyUnitSensorClass_M5Core2Module
KeyUnitSensorClass::setup()
 setupKeyUnit == 0x3ffbd670
M5STACK VERSION = 2.0.4
WIFI ssid = SunnyWhiteriver
preSetup_WIFI_APModule: isSet(set)
WIFI ssid = SunnyWhiteriver
Starting NimBLE BLEClientNetworking
doneWIFI_APModuleCredentials,  and now calling finishSetup()
From WIFI -> JSON = 
setup_MQTTNetworking
 .. continue setup_MQTTNetworking
MQTT.readPreferences
Reading.1 EPROM JSON = {"ssid":"SunnyWhiteriver","ssidPassword":"sunny2021","mqtt_server":"idogwatch.com","mqtt_port":"1883","mqtt_password":"id0gwatch","mqtt_guestPassword":"doggy","mqtt_user":"scott@konacurrents.com","mqtt_topic":"usersP/bark/scott@konacurrents.com","deviceName":"M5Core3","uuid":null,"jsonHeader":null,"jsonVersion":null,"location":"Mineaplos, Minnesota"}
JSON parsed.1 = {"ssid":"SunnyWhiteriver","ssidPassword":"sunny2021","mqtt_server":"idogwatch.com","mqtt_port":"1883","mqtt_password":"id0gwatch","mqtt_guestPassword":"doggy","mqtt_user":"scott@konacurrents.com","mqtt_topic":"usersP/bark/scott@konacurrents.com","deviceName":"M5Core3","uuid":null,"jsonHeader":null,"jsonVersion":null,"location":"Mineaplos, Minnesota"}
SunnyWhiteriver
sunny2021
idogwatch.com
id0gwatch

