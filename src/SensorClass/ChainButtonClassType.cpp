//
//  ChainButtonClassType.cpp
//  ESP_IOT
//
//  Created by Scott Moody on 12/19/25.
//

#include "ChainButtonClassType.h"


#include "../../Defines.h"

//! library code M5Chain.h
#include "M5Chain.h"

//! 2.11.26 token parser
#include "../MainModule/TokenParser.h"

ChainButtonClass::ChainButtonClass(char *config) : SensorClassType(config)
{
    SerialDebug.printf("ChainButtonClass init %s\n", config);
}

//! from example:
//! @see https://github.com/m5stack/M5Chain
//#define TXD_PIN GPIO_NUM_6  // Tx
//#define RXD_PIN GPIO_NUM_5  // Rx
#define TXD_PIN 22
#define RXD_PIN 19
//! global M5Chain structure used by the physical chain's and their library
Chain _M5Chain;
//! the device list WE have defined (eg button, button, joystick)
device_list_t *_devicesList = NULL;
//! number of physical CHAIN buttons plugged in right now
uint16_t _deviceNums        = 0;
//uint8_t _operationalStatus    = 0;
//chain_status_t _chainStatus = CHAIN_OK;

uint8_t _rgbTest[5][3] =
{
    {0xFF, 0x00, 0x00}, {0x00, 0xFF, 0x00}, {0x00, 0x00, 0xFF}, {0xFF, 0xFF, 0xFF}, {0x00, 0x00, 0x00},
};
void printType(long num)
{
    switch (num)
    {
        case CHAIN_UNKNOWN_TYPE_CODE:
            SerialDebug.println("type = CHAIN_UNKNOWN_TYPE_CODE");
            break;
        case CHAIN_ENCODER_TYPE_CODE:
            SerialDebug.println("type = CHAIN_ENCODER_TYPE_CODE");
            break;
        case CHAIN_KEY_TYPE_CODE:
            SerialDebug.println("type = CHAIN_KEY_TYPE_CODE");
            break;
        case CHAIN_ANGLE_TYPE_CODE:
            SerialDebug.println("type = CHAIN_ANGLE_TYPE_CODE");
            break;
        case CHAIN_JOYSTICK_TYPE_CODE:
            SerialDebug.println("type = CHAIN_JOYSTICK_TYPE_CODE");
            break;
        case CHAIN_TOF_TYPE_CODE:
            SerialDebug.println("type = CHAIN_TOF_TYPE_CODE");
            break;
    }
}

//! print the physical devices connected
void printDeviceList(device_list_t *devices)
{
    if (devices == NULL)
    {
        SerialDebug.println("devices is NULL");
        return;
    }
    
    SerialDebug.printf("Physical CHAIN devices count: %d\n", devices->count);
    
    for (uint8_t i = 0; i < devices->count; i++)
    {
        SerialDebug.print("devices ID: ");
        SerialDebug.println(devices->devices[i].id);
        SerialDebug.print("devices type: ");
        SerialDebug.println(devices->devices[i].device_type);
        printType(devices->devices[i].device_type);
        
    }
}

#define DEFN_
#ifdef DEFN

//! 1.22.26 wrap the button
//! define ChainbuttonStruct
typedef struct {
    //! string describing the module, etc
    char *classType;
    //! each in chainUseCount references the 4 arrays index
    //! the buton numbers
    int buttonNumber;
    //! the set strings, val strings, device strings
    char *setString;
    //! val strings
    char *valString;
    //! the device strings
    //! Note, if "localhost" then this is an INTERNAL message (like feed)
    //! so use bluetooth or whatever network works
    char *deviceString;
} ChainButtonStruct;


//! 12.19.25 ChainStruct, duplicated from above
//! NOTE: the parser will be updated...
//! TODO: update the parser
typedef struct {
    //! number of chain
    int chainUseCount;
    
    //! 1.22.26 define a Chainbutton
    ChainButtonStruct* chainButtonStructArray[CHAIN_USE_MAX];
} ChainUseStruct;

//! get the chain struct
ChainUseStruct getChainUseStruct_mainModule();
#endif


//! 1.26.26 Joystick quadrants
//! returns a button .. which will match same as SINGLE PRESS, etc
int getJoystickQuadrantButton(int x, int y)
{
    /*
     //! map x,y to quadrants
     #define BUTTON_JOYSTICK_LEFT 100
     #define BUTTON_JOYSTICK_TOP 101
     #define BUTTON_JOYSTICK_RIGHT 102
     #define BUTTON_JOYSTICK_BOTTOM 103
     */
    
    if (x <= 0 && y < 120 && y >= 0)
        return BUTTON_JOYSTICK_LEFT;
    else if (x > 120 && y >= 0)
        return BUTTON_JOYSTICK_TOP;
    else if (x <= 0 && y <= 0)
        return BUTTON_JOYSTICK_RIGHT;
    else if (x <= 0 && y >= 0)
        return BUTTON_JOYSTICK_BOTTOM;
    else return BUTTON_JOYSTICK_TOP;
}


//! 2.7.26 (after my 68th bday w Laura Babe). Weekend now, olympics, moon flight, superbowl with Seahawks
//! keep the last time for differencing.
long _lastChainButtonTime = 0;
long _lastChainButtonTimes[4] = {0,0,0,0};
//! 2.13.26 cold rain
//! use thenew diff per quadrant
//! use last chain button time but for the 4 quadrants 100,101,102,103
void setChainButtonTime(long time, int pressKind)
{
    int normalized = BUTTON_JOYSTICK_LEFT - pressKind;
    //! normalize pressKind 100 == 0
    _lastChainButtonTimes[normalized] = time;
}
//! use last chain button time but for the 4 quadrants 100,101,102,103
long diffChainButtonTime(long timeNow, int pressKind)
{
    int normalized = BUTTON_JOYSTICK_LEFT - pressKind;

    //! normalize pressKind 100 == 0
    long diff = timeNow - _lastChainButtonTimes[normalized];
    //! update
    setChainButtonTime(timeNow, pressKind);
    //! return diff
    return diff;
}

//!@see https://www.w3schools.com/cpp/cpp_class_methods.asp
//! 1.22.26
//! the buttons are numbered 0..n Long press and Short Press need defining for each button
//! then what the button should do, eg. send MQTT message: eg.  MatrixCell, 5
//! Basically we have a full programming language needed. This is exactly why the SMART Buttons are
//! created, but calling them from here isn't easy. Instead we need MQTT messages (or internal messages, like feed the dog)
void ChainButtonClass::loop()
{
    //! stores status
    chain_status_t chainStatus;

    //! 3.15.26 Stormy Weekend - 410 100's trees downed ..
    //! The challenge: If there is only 1 button (let's say the Joystick) physically used
    //! but in our demo - it's the 3rd button (physically).  So the code here and elsewhere wouldn't
    //! know that. So what does "setup"
    //!  and whad does "loop" do when the button definitions aren't correct.
    //!  CASE:   there is only 1 physical button, and it's a JoyStick. It would be nice to know that .. and I don't know how to know it's plugged
    //!  in except if the CHAIN_OK state below .. let us know.
    //!  THen there is a CHAIN_LINKER that says the button and button types ..
    if (_devicesList)
    {
        //! basically goes through the PHYSICAL list .. and figures out what button was pressed..
        for (uint8_t i = 0; i < _devicesList->count; i++)
        {
            if (_devicesList->devices[i].device_type == CHAIN_KEY_TYPE_CODE)
            {
                uint8_t button_status = 0;
                button_double_click_time_t button_double_click_time;
                button_long_press_time_t button_long_press_time;
                chain_button_press_type_t button_press_type;
                chain_button_mode_t button_mode;
                chainStatus = _M5Chain.getKeyButtonStatus(_devicesList->devices[i].id, &button_status);
                if (chainStatus == CHAIN_OK)
                {
                    SerialLots.printf("KEY ID[%d] button status:%d \r\n", _devicesList->devices[i].id, button_status);
                }
                else
                {
                    SerialLots.printf("KEY ID[%d] get button status failed, chainStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus);
                }
                chainStatus = _M5Chain.getKeyButtonTriggerInterval(_devicesList->devices[i].id,
                                                                   &button_double_click_time, &button_long_press_time);
                if (chainStatus == CHAIN_OK)
                {
                    SerialLots.printf("KEY ID[%d] button double click time:%d, button long press time:%d \r\n",
                                      _devicesList->devices[i].id, button_double_click_time, button_long_press_time);
                }
                else
                {
                    //! 1.29.26 Rain finally
                    //! this happens when it's unplugged without rebooting ...
                    SerialDebug.printf("KEY ID[%d] get button trigger interval failed, chainStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus);
                }
                chainStatus = _M5Chain.getKeyButtonMode(_devicesList->devices[i].id, &button_mode);
                if (chainStatus == CHAIN_OK)
                {
                    SerialLots.printf("KEY ID[%d] button mode:%d \r\n", _devicesList->devices[i].id, button_mode);
                }
                else
                {
                    SerialLots.printf("KEY ID[%d] get button mode failed, chainStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus);
                }
                
                //! here is where the CHAIN LINKER links the chainUseStruct (our definitions) with the button found.
                while (_M5Chain.getKeyButtonPressStatus(_devicesList->devices[i].id, &button_press_type))
                {
                    int button1_based = i + 1;
                    ChainButtonStruct* chainUseStruct = NULL;
                    switch (button_press_type)
                    {
                        case CHAIN_BUTTON_PRESS_SINGLE:
                            //! grab the correct chain use struct..
                            chainUseStruct = getChainButtonStruct(button1_based, BUTTON_SHORT_PRESS);
                            SerialLots.printf("KEY ID[%d] button press type: single \r\n", _devicesList->devices[i].id);
                            break;
                        case CHAIN_BUTTON_PRESS_DOUBLE:
                            chainUseStruct = getChainButtonStruct(button1_based, BUTTON_DOUBLE_PRESS);
                            SerialLots.printf("KEY ID[%d] button press type: double \r\n", _devicesList->devices[i].id);
                            break;
                        case CHAIN_BUTTON_PRESS_LONG:
                            chainUseStruct = getChainButtonStruct(button1_based, BUTTON_LONG_PRESS);
                            SerialLots.printf("KEY ID[%d] button press type: long \r\n", _devicesList->devices[i].id);
                            break;
                    }
                    //! process this button . checks for NULL in function
                    processChainButtonStruct(chainUseStruct);
                }
            }
            //! TODO .. figure out how to pass these to the Joystick class..
            else if (_devicesList->devices[i].device_type == CHAIN_JOYSTICK_TYPE_CODE)
            {
                uint16_t xAdcValue = 0;
                uint16_t yAdcValue = 0;
                uint8_t xAdcValue8 = 0;
                uint8_t yAdcValue8 = 0;
                uint16_t mapRange[8];
                int16_t xMapAdcValue  = 0;
                int16_t yMapAdcValue  = 0;
                int8_t xMapAdcValue8  = 0;
                int8_t yMapAdcValue8  = 0;
                uint8_t button_status = 0;
                button_double_click_time_t button_double_click_time;
                button_long_press_time_t button_long_press_time;
                chain_button_press_type_t button_press_type;
                chain_button_mode_t button_mode;
                //! get joystick values
                chain_status_t chainStatus = _M5Chain.getJoystick16Adc(_devicesList->devices[i].id, &xAdcValue, &yAdcValue);
                if (chainStatus == CHAIN_OK) {
                    SerialLots.printf("JOYSTICK ID[%d] xAdcValue:%d, yAdcValue:%d \r\n", _devicesList->devices[i].id,
                                      xAdcValue, yAdcValue);
                } else {
                    SerialLots.printf("JOYSTICK ID[%d] get 16 adc value failed, chainStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus);
                }
                chainStatus = _M5Chain.getJoystick8Adc(_devicesList->devices[i].id, &xAdcValue8, &yAdcValue8);
                if (chainStatus == CHAIN_OK) {
                    SerialLots.printf("JOYSTICK ID[%d] xAdcValue8:%d, yAdcValue8:%d \r\n", _devicesList->devices[i].id,
                                      xAdcValue8, yAdcValue8);
                } else {
                    SerialLots.printf("JOYSTICK ID[%d] get 8 adc value failed, chainStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus);
                }
                
                chainStatus = _M5Chain.getJoystickMappedRange(_devicesList->devices[i].id, mapRange, JOYSTICK_MAP_SIZE);
                if (chainStatus == CHAIN_OK) {
                    SerialLots.printf("JOYSTICK ID[%d] mapRange:%d %d %d %d %d %d %d %d \r\n", _devicesList->devices[i].id,
                                      mapRange[0], mapRange[1], mapRange[2], mapRange[3], mapRange[4], mapRange[5],
                                      mapRange[6], mapRange[7]);
                } else {
                    SerialLots.printf("JOYSTICK ID[%d] get 16 adc map range failed, chainStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus);
                }
                chainStatus = _M5Chain.getJoystickMappedInt16Value(_devicesList->devices[i].id, &xMapAdcValue, &yMapAdcValue);
                if (chainStatus == CHAIN_OK) {
                    SerialLots.printf("JOYSTICK ID[%d] xMapAdcValue:%d, yMapAdcValue:%d \r\n", _devicesList->devices[i].id,
                                      xMapAdcValue, yMapAdcValue);
                } else {
                    SerialLots.printf("JOYSTICK ID[%d] get 16 adc map value failed, chainStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus);
                }
                chainStatus = _M5Chain.getJoystickMappedInt8Value(_devicesList->devices[i].id, &xMapAdcValue8, &yMapAdcValue8);
                if (chainStatus == CHAIN_OK) {
                    if (xMapAdcValue8 != 0 || yMapAdcValue8 != 0)
                    {
                        //! get the quadrant of the x,y eg, left, top, right, bottom
                        int pressKind = getJoystickQuadrantButton(xMapAdcValue8, yMapAdcValue8);
                        
                        //! 2.7.26 (after my 68th bday w Laura Babe, 2.5.2026).
                        //! see if the message is too fast ??
                        long chainButtonTime = getTimeStamp_mainModule();
                        long diffTime;
#ifdef OLDWAY
                        if (_lastChainButtonTime == 0)
                            //! first time
                            diffTime = 0;
                        diffTime = chainButtonTime - _lastChainButtonTime;
                        _lastChainButtonTime = chainButtonTime;
                        
#else
                        //! 2.13.26 cold rain
                        //! use thenew diff per quadrant
                        diffTime = diffChainButtonTime(chainButtonTime, pressKind);
#endif
                        //! 2.7.26 (after my 68th bday w Laura Babe).
                        //! print out
                        SerialLots.printf("M5Chain TIME: %d,  diff = %d\n", chainButtonTime, diffTime);
                        //! try a threshold .. 2 for now..
                        if (diffTime > 2)
                        {
                            //! 1.24.26 process the button version .. not the x,y version
                            SerialDebug.printf("JOYSTICK ID[%d=%d] xMapAdcValue8:%d, yMapAdcValue8:%d \r\n",
                                               _devicesList->devices[i].id, pressKind, xMapAdcValue8, yMapAdcValue8);
                            int button1_based = i + 1;
                            ChainButtonStruct* chainUseStruct = getChainButtonStruct(button1_based, pressKind);
                            
                            //! process this button . checks for NULL in function
                            processChainButtonStruct(chainUseStruct);
                            
                            //! 3.15.26 Winter out there.. snow in hills
                            //! NOTE: the though is if there is only 1 button and it's a joystick, then this code will run
                            //! but the BUTTON won't be let's say '3' instead it will be '1'
                            //! SO: the lookup for button1_based will fail if we say '1'. (actually it will find a BUTTON not a JOYSTICK)
                            //! which would do something.. but not the JOYSTICK thing
                            //! TODO: if only 1 physical button _devicenum == 1
                            //! THEN .. find the first "Joystick" button  .. the buttonStruct doesn't have that knowledge ..
                            //! BRIDGE TOO FAR .. OR could search for a pressKind that matches .. first match goes ..
                        }
                    }
                } else {
                    SerialLots.printf("JOYSTICK ID[%d] get 8 adc map value failed, chainStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus);
                }
                
                chainStatus = _M5Chain.getJoystickButtonStatus(_devicesList->devices[i].id, &button_status);
                if (chainStatus == CHAIN_OK) {
                    SerialLots.printf("JOYSTICK ID[%d] button status:%d \r\n", _devicesList->devices[i].id, button_status);
                } else {
                    SerialLots.printf("JOYSTICK ID[%d] get button status failed, chainStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus);
                }
                chainStatus = _M5Chain.getJoystickButtonTriggerInterval(
                                                                        _devicesList->devices[i].id, &button_double_click_time, &button_long_press_time);
                if (chainStatus == CHAIN_OK) {
                    SerialLots.printf("JOYSTICK ID[%d] button double click time:%d, button long press time:%d \r\n",
                                      _devicesList->devices[i].id, button_double_click_time, button_long_press_time);
                } else {
                    //! 1.29.26 Rain finally
                    //! this happens when it's unplugged without rebooting ...
                    SerialDebug.printf("JOYSTICK ID[%d] get button trigger interval failed, chainStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus);
                }
                chainStatus = _M5Chain.getJoystickButtonMode(_devicesList->devices[i].id, &button_mode);
                if (chainStatus == CHAIN_OK) {
                    SerialLots.printf("JOYSTICK ID[%d] button mode:%d \r\n", _devicesList->devices[i].id, button_mode);
                } else {
                    SerialLots.printf("JOYSTICK ID[%d] get button mode failed, chainStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus);
                }
                //! 1.24.26 process the button version .. not the x,y version
                while (_M5Chain.getJoystickButtonPressStatus(_devicesList->devices[i].id, &button_press_type))
                {
                 
                    int button1_based = i + 1;
                    ChainButtonStruct* chainUseStruct = NULL;
                    switch (button_press_type)
                    {
                        case CHAIN_BUTTON_PRESS_SINGLE:
                            //! grab the correct chain use struct..
                            chainUseStruct = getChainButtonStruct(button1_based, BUTTON_SHORT_PRESS);
                            SerialLots.printf("JOYSTICK ID[%d] button press type: single \r\n", _devicesList->devices[i].id);
                            break;
                        case CHAIN_BUTTON_PRESS_DOUBLE:
                            chainUseStruct = getChainButtonStruct(button1_based, BUTTON_DOUBLE_PRESS);
                            SerialLots.printf("JOYSTICK ID[%d] button press type: double \r\n", _devicesList->devices[i].id);
                            break;
                        case CHAIN_BUTTON_PRESS_LONG:
                            chainUseStruct = getChainButtonStruct(button1_based, BUTTON_LONG_PRESS);
                            SerialLots.printf("JOYSTICK ID[%d] button press type: long \r\n", _devicesList->devices[i].id);
                            break;
                    }
                    //! process this button . checks for NULL in function
                    processChainButtonStruct(chainUseStruct);
                }
            }
        }
    }
}


//!setup the buttons
void ChainButtonClass::setup()
{
    //! flag returned
    uint8_t operationalStatus;
    //! chain status
    chain_status_t chainStatus;

    SerialDebug.printf("ChainButtonClass::setup(%d,%d)\n",_pin1, _pin2);
 
    SerialLots.println("1._M5Chain Key Test");
    _M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);
    
    //! 2.7.26 (after my 68th bday w Laura Babe, 2.5.2026).
    //! see if the message is too fast ??
    long chainButtonTime = getTimeStamp_mainModule();
    _lastChainButtonTime = chainButtonTime;
    
    //! register the pin used
    registerPinUse_mainModule(RXD_PIN, "RXD_PIN", "ChainButtonClass", false);
    registerPinUse_mainModule(TXD_PIN, "TXD_PIN", "ChainButtonClass", false);
    
    //! if the chains are ready ...
    //! This first loop counts the number of devices, and then creates a devices list of that number..
    //! It would also know the type of the device ..
    if (_M5Chain.isDeviceConnected())
    {
        SerialDebug.println("CHAIN devices is connected");
        //!
        chainStatus = _M5Chain.getDeviceNum(&_deviceNums);
        SerialDebug.printf("Number of connected CHAIN Devices: %d\n", _deviceNums);
        if (chainStatus == CHAIN_OK)
        {
            _devicesList          = (device_list_t *)malloc(sizeof(device_list_t));
            _devicesList->count   = _deviceNums;
            //! create storage for the NUM devices (acutal devices)
            _devicesList->devices = (device_info_t *)malloc(sizeof(device_info_t) * _deviceNums);
            //! this fills in the devicesList structures .. including the device_type (CHAIN_KEY_TYPE or CHAIN_JOYSTICK_TYPE_CODE)
            if (_M5Chain.getDeviceList(_devicesList))
            {
                SerialLots.println("get devices list success");
                printDeviceList(_devicesList);
            }
            else
            {
                SerialLots.println("get devices list failed");
            }
        }
        else
        {
            SerialLots.printf("error status:%d \r\n", chainStatus);
            SerialLots.printf("devices num get failed.\r\n");
        }
    }
    else
    {
        SerialLots.println("devices is not connected.");
    }
    
    //! this is our list but we don't know the button types yet
    //! the devicesList is global of the PHYSICAL buttons plugged in..
    if (_devicesList)
    {
        for (uint8_t i = 0; i < _devicesList->count; i++)
        {
            if (_devicesList->devices[i].device_type == CHAIN_KEY_TYPE_CODE)
            {
                chainStatus = _M5Chain.setRGBLight(_devicesList->devices[i].id, 100, &operationalStatus);
                if (chainStatus == CHAIN_OK && operationalStatus)
                {
                    SerialLots.printf("ID[%d] set rgb light success\r\n", _devicesList->devices[i].id);
                }
                else
                {
                    SerialLots.printf("ID[%d] set rgb light failed, chainStatus:%d  operationalStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus, operationalStatus);
                }
                //! draw on the RGB colors
                for (uint8_t j = 0; j < 5; j++)
                {
                    uint8_t rgb[3] = {0};
                    chainStatus = _M5Chain.setRGBValue(_devicesList->devices[i].id, 0, 1, _rgbTest[j], 3, &operationalStatus);
                    if (chainStatus == CHAIN_OK && operationalStatus)
                    {
                        SerialLots.printf("ID[%d] set rgb %d %d %d success\r\n", _devicesList->devices[i].id,
                                          _rgbTest[j][0], _rgbTest[j][1], _rgbTest[j][2]);
                    }
                    else
                    {
                        SerialLots.printf("ID[%d] set rgb %d %d %d failed, chainStatus:%d  operationalStatus:%d \r\n",
                                      _devicesList->devices[i].id, _rgbTest[j][0], _rgbTest[j][1], _rgbTest[j][2],
                                      chainStatus, operationalStatus);
                    }
                    chainStatus = _M5Chain.getRGBValue(_devicesList->devices[i].id, 0, 1, rgb, 3, &operationalStatus);
                    if (chainStatus == CHAIN_OK && operationalStatus)
                    {
                        SerialLots.printf("ID[%d] get rgb %d %d %d success \r\n", _devicesList->devices[i].id, rgb[0],
                                          rgb[1], rgb[2]);
                    }
                    else
                    {
                        SerialLots.printf("ID[%d] get rgb %d %d %d failed, chainStatus:%d  operationalStatus:%d \r\n",
                                      _devicesList->devices[i].id, rgb[0], rgb[1], rgb[2], chainStatus,
                                      operationalStatus);
                    }
                    delay(50);
                }
                //! setup the trigger interval .. their API
                chainStatus = _M5Chain.setKeyButtonTriggerInterval(_devicesList->devices[i].id, BUTTON_DOUBLE_CLICK_TIME_200MS,BUTTON_LONG_PRESS_TIME_3S, &operationalStatus);
                if (chainStatus == CHAIN_OK && operationalStatus)
                {
                    SerialLots.printf("KEY ID[%d] set key button trigger interval success\r\n",
                                  _devicesList->devices[i].id);
                }
                else
                {
                    SerialLots.printf(
                                  "KEY ID[%d] set key button trigger interval failed, chainStatus:%d  operationalStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus, operationalStatus);
                }
                //! setup the CHAIN REPORT MODE . their API
                chainStatus = _M5Chain.setKeyButtonMode(_devicesList->devices[i].id, CHAIN_BUTTON_REPORT_MODE, &operationalStatus);
                if (chainStatus == CHAIN_OK && operationalStatus)
                {
                    SerialLots.printf("KEY ID[%d] set key button mode success\r\n", _devicesList->devices[i].id);
                }
                else
                {
                    SerialLots.printf("KEY ID[%d] set key button mode failed, chainStatus:%d  operationalStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus, operationalStatus);
                }
            }
            else if (_devicesList->devices[i].device_type == CHAIN_JOYSTICK_TYPE_CODE)
            {
                chainStatus = _M5Chain.setRGBLight(_devicesList->devices[i].id, 100, &operationalStatus);
                if (chainStatus == CHAIN_OK && operationalStatus) {
                    SerialLots.printf("ID[%d] set rgb light success\r\n", _devicesList->devices[i].id);
                } else {
                    SerialLots.printf("ID[%d] set rgb light failed, chainStatus:%d  operationalStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus, operationalStatus);
                }
                //! draw colors
                for (uint8_t j = 0; j < 5; j++) {
                    uint8_t rgb[3] = {0};
                    chainStatus = _M5Chain.setRGBValue(_devicesList->devices[i].id, 0, 1, _rgbTest[j], 3, &operationalStatus);
                    if (chainStatus == CHAIN_OK && operationalStatus) {
                        SerialLots.printf("ID[%d] set rgb %d %d %d success\r\n", _devicesList->devices[i].id,
                                          _rgbTest[j][0], _rgbTest[j][1], _rgbTest[j][2]);
                    } else {
                        SerialLots.printf("ID[%d] set rgb %d %d %d failed, chainStatus:%d  operationalStatus:%d \r\n",
                                      _devicesList->devices[i].id, _rgbTest[j][0], _rgbTest[j][1], _rgbTest[j][2],
                                      chainStatus, operationalStatus);
                    }
                    chainStatus = _M5Chain.getRGBValue(_devicesList->devices[i].id, 0, 1, rgb, 3, &operationalStatus);
                    if (chainStatus == CHAIN_OK && operationalStatus) {
                        SerialLots.printf("ID[%d] get rgb %d %d %d success \r\n", _devicesList->devices[i].id, rgb[0],
                                      rgb[1], rgb[2]);
                    } else {
                        SerialLots.printf("ID[%d] get rgb %d %d %d failed, chainStatus:%d  operationalStatus:%d \r\n",
                                      _devicesList->devices[i].id, rgb[0], rgb[1], rgb[2], chainStatus,
                                      operationalStatus);
                    }
                    delay(50);
                }
                //! configure the double click time
                chainStatus = _M5Chain.setKeyButtonTriggerInterval(_devicesList->devices[i].id, BUTTON_DOUBLE_CLICK_TIME_200MS,
                                                    BUTTON_LONG_PRESS_TIME_3S, &operationalStatus);
                if (chainStatus == CHAIN_OK && operationalStatus) {
                    SerialLots.printf("JOYSTICK ID[%d] set key button trigger interval success\r\n",
                                  _devicesList->devices[i].id);
                } else {
                    SerialLots.printf(
                                  "JOYSTICK ID[%d] set key button trigger interval failed, chainStatus:%d  operationalStatus:%d "
                                  "\r\n",
                                  _devicesList->devices[i].id, chainStatus, operationalStatus);
                }
                //! setup the report mode
                chainStatus = _M5Chain.setKeyButtonMode(_devicesList->devices[i].id, CHAIN_BUTTON_REPORT_MODE, &operationalStatus);
                if (chainStatus == CHAIN_OK && operationalStatus) {
                    SerialLots.printf("JOYSTICK ID[%d] set key button mode success\r\n", _devicesList->devices[i].id);
                } else {
                    SerialLots.printf(
                                  "JOYSTICK ID[%d] set key button mode failed, chainStatus:%d  operationalStatus:%d \r\n",
                                  _devicesList->devices[i].id, chainStatus, operationalStatus);
                }
            }
        }
    }
    else
    {
        SerialLots.println("devices list is NULL");
    }
}


//! 5.15.25 try a special command local to this class
void ChainButtonClass::messageLocal_SensorClassType(char *message)
{
    SerialDebug.printf("ChainButtonClass::messageLocal_SensorClassType(%s)\n", message);
    if (strcmp(message,"click")==0)
    {
        SerialDebug.println("ChainButtonClass.click");
    }
}

#ifdef REFACTORED_TO_TOKEN_PARSER
//! 2.10.26 NICE day out ..
//! struct for returning a parsed SM address
//! syntax:   uuid.flow(set,val,dev)
typedef struct smStruct
{
    char *smAddress;
    char *setString;
    char *valString;
    char *devString;
} SMStruct;


//! 2.10.26 Sunny Clear, Mountain,
//! parse the address
//! syntax:   uuid.flow(set,val,dev)
//! NOTE: THis creates storage for the 3 components
SMStruct parseSMAddress(char *str)
{
    //! return value
    SMStruct sm;
    SerialDebug.printf("ParseSM: %s\n", str);
    
    //! syntax:   uuid.flow(set,val,dev)
    
    // search for '(' first
    char* paren = index(str, '(');
    {
        char b[100];
        strcpy(b, str);
        b[paren-str] = '\0';
        //strncpy(b, str, paren-str);
        sm.smAddress = createCopy(b);
        SerialLots.printf("sm. %s\n", b);
    }
    
    //! now find the pieces starting at 1 past paren
    // (toggleSocket,,M5AtomSocket)
    paren++;
    char *comma = index(paren,',');
    {
        char b[50];
        strcpy(b, paren);
        b[comma-paren] = '\0';
        //strncpy(b, paren, comma-paren);
        SerialLots.printf("str. %s\n", b);
        sm.setString = createCopy(b);
    }
    
    //! now the valString
    paren = ++comma;
    comma = index(paren,',');
    {
        char b[50];
        strcpy(b, paren);
        b[comma-paren] = '\0';
        //strncpy(b, paren, comma-paren);
        SerialLots.printf("val. %s\n", b);
        sm.valString = createCopy(b);
        //! get rid of the " " version
        if (strlen(sm.valString) == 1)
        {
            sm.valString = createCopy((char*)"");
        }
    }

    //! now the dev follow = )
    paren = ++comma;
    char *rparen = index(paren,')');
    {
        char b[50];
        strcpy(b,paren);
        b[rparen-paren] = '\0';
        //strncpy(b, paren, rparen - paren);
        SerialLots.printf("dev. %s\n", b);
        sm.devString = createCopy(b);
    }
    
    return sm;
}

//! storage for sm parsin
char _smBuffer_chain[100];
//! 2.11.26 Lincoln's birthday, Puppies going home to Auburn, Nice Day out..
//! return the uuid.flownum (if any)
//! search for 'uuid=<uuid>&"  and "flow=<flow> [& or }"
//! return NULL if not matching
//! NOTE: no storage is created but the buffer returned is REUSED
char *parseDOCFOLLOW(char *str)
{

    //!  strstr() function locates the first occurrence of the null-terminated string needle in the null-terminated string
    char *uuidIndex = strstr(str, "uuid=");
    if (!uuidIndex)
        return NULL;
    
    //! find uuid
    uuidIndex += strlen("uuid=");
    int i = 0;
    while (uuidIndex[i] != '&')
    {
        _smBuffer_chain[i] = uuidIndex[i];
        i++;
    }
    _smBuffer_chain[i++] = '.';
    _smBuffer_chain[i] = '\0';
    
    //! now find flow
    char *flowIndex = strstr(str, "flow=");
    if (!flowIndex)
        return NULL;
    flowIndex += strlen("flow=");
    
    //! follow = } or & (if more args)
    int j=0;
    while (flowIndex[j] != '&' && flowIndex[j] != '}' && flowIndex[j] != '\0')
    {
        _smBuffer_chain[i] = flowIndex[j];
        i++;
        j++;
    }
    _smBuffer_chain[i] = '\0';
    
    return _smBuffer_chain;
}

#endif

//! adding the messages as well
//! 5.14.25 (Laura/Paul flying). 5.14.74 great Dead
//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 12.27.23 support setName == "socket"
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void ChainButtonClass::messageSetVal_SensorClassType(char *setName, char* valValue, boolean deviceNameSpecified)
{
    SerialDebug.printf("ChainButtonClass.messageSetVal_SensorClassType: %s, %s, devSpecified=%d\n", setName, valValue, deviceNameSpecified);
    //! todo .. look for SM_MatrixcCell and hit the button (single press) the matching 1..n
    //! 1.24.26 see if SM_MatrixCell, num .. then run the SINGLE_PRESS of that cell 1..n
    //boolean isMatrixCell = strcmp(setName,"SM_MatrixCell")==0;
    //! 1.25.26 Football day .. Seahawks vs Rams for superbowl ..
    //! Not supporting SM_MatrixCall (since we aren't a matrix YET ...)
    if (strcmp(setName,"SM_ChainButton")==0)
    {
        //! the MatrixCell is actuall row.col .. but we will piggy back on that..
        //! NFL Games, Seahawks-Rams, Patriots-Broncos
        //! need tu spport <name> l[row [.col]]
        
        //! 1.25.26 Cold Clear Mt, Puppies in garage eating my toes,
        //! support new SM_ChainButton, button, pressKind syntax
        //! set: SM_ChainButton, val: button.presskind  (or just button [.presskind])
        
        //! which loop, button, then pressKind
        int i = 0;
        int cellNum = 1;
        int pressKind = BUTTON_SHORT_PRESS;
        char str[10];
        strcpy(str, valValue);
        char *ch = strtok(str, ".");
        while (ch != NULL)
        {
            int num = atoi(ch);
            switch (i)
            {
                case 0: // first loop
                    cellNum = num;
                    break;
                case 1:  //2nd loop the pressKind 0..n
                    pressKind = num;
                    break;
            }
            ch = strtok(NULL, ".");
            i++;
        }
        ChainButtonStruct* chainUseStruct = getChainButtonStruct(cellNum, pressKind);
        //! process this button (for testing will be button 2 .. turn on/off the lights)
        processChainButtonStruct(chainUseStruct);
    }
    //! 2.9.26 after Superbowl win, and L Vonn epic crash
    //! support the new message for processing a SMARTDefn of a set/val
    //! {"set":"smartDefn","val":"uuid.flownum(set, val, dev)"}
    else if (strcmp(setName, "smartDefn")==0)
    {
#ifdef MOVED_TO_OTHER
        //! 2.,10.26 parsing works .. no extra spaces ..
        SMStruct sm = parseSMAddress(valValue);
        char *smartAddress = sm.smAddress;
        SerialDebug.printf("SM = '%s', '%s', '%s', '%s'\n", sm.smAddress, sm.setString, sm.valString, sm.devString);

#ifdef DEBUG_VERSION
        //! new message
        //! break apart the definition .. parsing sucks
        //! look for the '('
        char smartAddress[50];
        char setString[20];
        char valString[20];
        char devString[20];
        SerialDebug.printf("TODO: smartDefn; %s\n", valValue);
        //strcpy(smartAddress, "QHmwUurxC3.1703806697279");
        strcpy(smartAddress, "QHmwUurxC3.1769990960124"); //! TODO THIS IS HARD WIRED FOR NOW.... unitl parser works..
        strcpy(setString, "toggleSocket");
        strcpy(valString, "");
        strcpy(devString, "M5AtomSocket");
         //test:      {'cmd':'togglesocket','dev':'M5AtomSocket'}
#endif
        //! 2.9.26 works.. but there might be multiple with same SMART address
        //! OOPS .. the getChainButtonStructSMART .. looks for matches .. this should be returning multiple
        //! BUT for now .. only the first one.
        //! NOTE: this is a chain button with a smart address .. which can be re-bound through this message..
        ChainButtonStruct* chainButtonStruct = getChainButtonStructSMART(smartAddress);
        if (chainButtonStruct)
        {
            SerialDebug.printf("FOUND chainButtonSMART(%s, %d) =  %s", smartAddress, chainButtonStruct->pressKind, chainButtonStruct->chainUse);
            
            //! for now .. set to the ToggleSocket
            chainButtonStruct->setString = sm.setString;
            chainButtonStruct->valString = sm.valString;
            chainButtonStruct->deviceString = sm.devString;
            chainButtonStruct->buttonFinalized = true;
            //!update the chain use for this one..
            sprintf(chainButtonStruct->chainUse,createCopy("CHAIN_USE: %s btn=%2d press=%d set= %s, val=%s dev=%s", chainButtonStruct->classType, chainButtonStruct->buttonNumber, chainButtonStruct->pressKind, chainButtonStruct->setString, chainButtonStruct->valString, chainButtonStruct->deviceString));

            //! printout
            printChainSensors_mainModule(getChainUseStruct_mainModule());

        }
#else
        //! 2.9.26 works.. but there might be multiple with same SMART address
        //! OOPS .. the getChainButtonStructSMART .. looks for matches .. this should be returning multiple
        //! BUT for now .. only the first one.
        //! NOTE: this is a chain button with a smart address .. which can be re-bound through this message..
        setChainButtonStructSMART(valValue);
#endif
    }
    //! 2.11.26 nice day. Mt Looking great
    //! the valValue could be the http version, or alerady a semantic marker UUID.flow
    else if (strcmp(setName, "SemanticMarker")==0)
    {
        SerialDebug.printf("Process **** SemanticMarker: %s\n", valValue);
        char *sm;
        char *uuidIndex = strstr(valValue, "http");
        if (!uuidIndex)
        {
            sm = valValue;
        }
        else
        {
            //! parse full value ..
            sm = parseDOCFOLLOW(valValue);
        }
        SerialDebug.printf("SM = %s\n", sm?sm:(char*)"NULL");
        if (sm)
        {
            //! inform the CHAIN buttons that DOCFOLLOW is set to the value specified ...
            //! TODO: think about the Group as a parameter to have kinds of "DOCFOLLOW" not all the same
            //! sm is still transient storage
            setChainButtonStructDOCFOLLOW(sm);
        }
    }
}

#ifdef NOT_DEFINED

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the  send -
void BuzzerSensorClass::messageSend_SensorClassType(char *sendValue)
{
    
}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the cmd
void BuzzerSensorClass::messageCmd_SensorClassType(char *cmdValue)
{
    
}
#endif //NOT_DEFINED
