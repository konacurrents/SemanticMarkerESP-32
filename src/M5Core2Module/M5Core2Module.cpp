
#include "../../Defines.h"
#ifdef M5CORE2_MODULE
#include "M5Core2Module.h"


#include <M5Display.h>

//#include <M5Atom.h>

//! ability to turn on/off the M5Core2Module's scanning
boolean _isOn_M5Core2Module = true;

//! set if any A/B/C button touched ..
boolean _buttonTouched_M5Core2Module = false;

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
void messageSend_M5Core2Module(char* sendValue)
{
}

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void messageSetVal_M5Core2Module(char *setName, char* valValue, boolean deviceNameSpecified)
{
    
}

//! this adds the SENSOR class object
//! 2.8.24 30,000 feet above bumpy pacific on way back to Seattle (nice Moody visit to Oahu)
//#define KEY_UNIT_SENSOR_CLASS
#ifdef  KEY_UNIT_SENSOR_CLASS

#include "../SensorClass/SensorClassType.h"
#include "../SensorClass/KeyUnitSensorClass.h"

KeyUnitSensorClass *_KeyUnitSensorClass_M5Core2Module;


//a pointer to a callback function that takes (char*) and returns void
void M5Core2Callback(char *parameter, boolean flag)
{
    SerialDebug.printf("M5Core2Callback.sensorCallbackSignature(%s,%d)\n", parameter, flag);
    
    sendMessageString_mainModule((char*)"M5Core2Callback.KEY Pressed ");
       
    SerialDebug.printf("PREFERENCE_SENDWIFI_WITH_BLE = %d\n", getPreferenceBoolean_mainModule(PREFERENCE_SENDWIFI_WITH_BLE));
    
    //!returns the current SM Mode
    int mode = getCurrentSMMode_mainModule();
    switch (mode)
    {
        case SM_doc_follow:
            //! lets do whatever the DOC FOLLOW shows to do..
#ifdef USE_MQTT_NETWORKING
            //send feed over MQTT
            sendMessageMQTT((char *)"#FEED");
            
#endif  //useMQTT
            break;
        default:
            //This (in mainModule) will look if connected so sends over BLE, otherwise MQTT (if connected)
            main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
            break;
    }
    
}
#endif //KEY_UNIT_SENSOR_CLASS

#define SENSORS_MOTION_PIR
#ifdef SENSORS_MOTION_PIR
const uint32_t SLEEP_DURATION = 1 * 1000000;

unsigned long _PrevSampleTime    = 0;
unsigned long _PrevTriggerTime    = 0;
unsigned long _InactivityTimeOut = 0;

#define Elapsed3secs  3000
#define Elapsed4secs  4000

#define Elapsed3mins  180000 // 3 minutes in milliseconds
struct IMUVariables
{
    float accX = 0.0F;
    float accY = 0.0F;
    float accZ = 0.0F;
    float diffX   = 0.0F;
    float diffY   = 0.0F;
    float diffZ   = 0.0F;
    float diffXYZ = 0.0F;
    float prevX = 0.0F;
    float prevY = 0.0F;
    float prevZ = 0.0F;
#ifdef GYRO
    float gyroX = 0.0F;
    float gyroY = 0.0F;
    float gyroZ = 0.0F;
#endif
#ifdef PITCH_ROLL
    float pitch = 0.0F;
    float roll  = 0.0F;
    float yaw   = 0.0F;
#endif
    float TILT_SENSITIVITY = 0.5;
} _IMU;


//!looks at M5.IMU sensor to see if changed since last time..
//!Return "true" on motion (over sensitivity) and false otherwise
bool checkMotion_ButtonProcessing()
{
    //! if not an M5 return
#ifndef ESP_M5
    return false;
#endif
    //! if the TILT isn't an option.. then never return true ..
    if (!getPreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE))
    {
        return false;
    }
    else
    {
        //        main_printModuleConfiguration();
        //        SerialTemp.println("**** SENSOR TILT IS ON");
        //        main_printModuleConfiguration();
    }
    
    // 100ms sample interval
    if ( (millis() - _PrevSampleTime) >100)
    {
        //update for next sample point
        _PrevSampleTime = millis();
#ifdef ESP_M5
#ifdef GYRO
        M5.IMU.getGyroData(&_IMU.gyroX,&_IMU.gyroY,&_IMU.gyroZ);
#endif
        
        // juse use this..
        M5.IMU.getAccelData(&_IMU.accX,&_IMU.accY,&_IMU.accZ);
        
#ifdef PITCH_ROLL
        M5.IMU.getAhrsData(&_IMU.pitch,&_IMU.roll,&_IMU.yaw);
#endif
        
        //debug print
        SerialLots.printf("%5.2f  %5.2f  %5.2f   \n\r", _IMU.accX, _IMU.accY, _IMU.accZ);
#ifdef GYRO
        SerialLots.printf("%5.2f  %5.2f  %5.2f   \n\r", _IMU.gyroX, _IMU.gyroY, _IMU.gyroZ);
#endif
#ifdef PITCH_ROLL
        SerialLots.printf("%5.2f  %5.2f  %5.2f   \n\r", _IMU.pitch, _IMU.roll, _IMU.yaw);
#endif
#endif
        _IMU.diffX =   abs(_IMU.prevX - _IMU.accX);
        _IMU.diffY =   abs(_IMU.prevY - _IMU.accY);
        _IMU.diffZ =   abs(_IMU.prevZ - _IMU.accZ);
        _IMU.diffXYZ = _IMU.diffX + _IMU.diffY + _IMU.diffZ;
        
        //save x,y,z from this cycle for next cycle
        
        _IMU.prevX = _IMU.accX;
        _IMU.prevY = _IMU.accY;
        _IMU.prevZ = _IMU.accZ;
    }
    //if the movement is above threshold sensitivity then broadcast and start IGNORE time
    if ( (_IMU.diffXYZ > _IMU.TILT_SENSITIVITY) && ((millis()-_PrevTriggerTime)>Elapsed4secs) )
    {
        _PrevTriggerTime = millis();
        SerialDebug.printf("diff:  %.2f  \r\n", _IMU.diffXYZ);
        // TILT has exceed threshold
        return true;
    }
    else
    {
        //movement does not exceed threshold
        return false;
    }
}  // end of CheckMotion
#endif // SENSORS_MOTION_PIR
//!forward reference .. since if button press called externally..
void loopCode_M5Core2Module();


//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void buttonA_ShortPress_M5Core2Module()
{
    loopCode_M5Core2Module();
}
//!long press on buttonA (top button)
void buttonA_LongPress_M5Core2Module()
{
    loopCode_M5Core2Module();
}
//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void buttonB_ShortPress_M5Core2Module()
{
    loopCode_M5Core2Module();
}
//!long press on buttonA (top button)
void buttonB_LongPress_M5Core2Module()
{
    loopCode_M5Core2Module();
}

#define SHORT_VIBRATE 0
#define LONG_VIBRATE 1
#define LONG_LONG_VIBRATE 2
//! turn on/off the vibration
//! length can be 0, 1, 2
//! for short, mid, long
void vibrate_M5Core2Module(int relativeLength)
{
    
    int length;
    switch (relativeLength)
    {
        case SHORT_VIBRATE:
            length = 200;
            break;
        case LONG_VIBRATE:
            length = 500;
            break;
        case LONG_LONG_VIBRATE:
            length = 1000;
            break;
    }
#ifdef OLDER_CORE2
    M5.Axp.SetLDOEnable(3, true);  //Open the vibration.   开启震动马达
    delay(length);
    M5.Axp.SetLDOEnable(3, false);  //Open the vibration.   关闭震动马达
 //   delay(length);
#else
    M5.Axp.SetVibration(true);  // Open the vibration.   开启震动马达
    delay(length);
    M5.Axp.SetVibration(false);  // Open the vibration.   关闭震动马达
#endif
    
}

//!big button on front of M5StickC Plus
void checkButtonC_M5Core2Module()
{
    
    //! THIS is code for one of the button .. the screen
#ifdef ESP_M5
#ifdef M5CORE2_MODULE
    
    //!NOTE: ths issue is the timer is interruped by the scanner.. so make long-long very long..
    //was 1000  (from 500)
    if (M5.BtnC.wasReleasefor(3500))
    {
        // since button touched .. to quick vibrate
        vibrate_M5Core2Module(LONG_VIBRATE);
        //        buttonA_longPress_M5Core2Module();
        SerialDebug.println("C. **** LONG LONG PRESS ***");
                
        //! set A/B/C button touched ..
        _buttonTouched_M5Core2Module = true;
    }
    else if (M5.BtnC.wasReleasefor(1000))
    {
        // since button touched .. to quick vibrate
        vibrate_M5Core2Module(LONG_VIBRATE);
        //        buttonA_longPress_M5Core2Module();
        SerialDebug.println(" **** C. LONG PRESS (status)***");
        //! update the model (ButtonProcessing)
       // buttonB_LongPress();
        
#ifdef USE_MQTT_NETWORKING
        //!also send a #STATUS
        sendMessageMQTT((char*)"#STATUS");
#endif
        //! set A/B/C button touched ..
        _buttonTouched_M5Core2Module = true;
    }
    else if (M5.BtnC.wasReleased())
    {
        // since button touched .. to quick vibrate
        vibrate_M5Core2Module(SHORT_VIBRATE);
        SerialDebug.println(" **** C. SHORT PRESS ***");
        //! update the model (ButtonProcessing)
        toggleShowingScrollingTextMode_displayModule();
#ifdef USE_MQTT_NETWORKING
        //!also send a #STATUS
        sendMessageMQTT((char*)"#STATUS");
#endif
        //! set A/B/C button touched ..
        _buttonTouched_M5Core2Module = true;
    }
    
#endif // M5Core2
#endif //ESP_M5
}
//!big button on front of M5StickC Plus
void checkButtonB_M5Core2Module()
{
    
    //! THIS is code for one of the button .. the screen
#ifdef ESP_M5
#ifdef M5CORE2_MODULE
    
    //!NOTE: ths issue is the timer is interruped by the scanner.. so make long-long very long..
    //was 1000  (from 500)
    if (M5.BtnB.wasReleasefor(3500))
    {
        //        buttonA_longPress_M5Core2Module();
        SerialDebug.println(" **** LONG LONG PRESS ***");
        //! set A/B/C button touched ..
        _buttonTouched_M5Core2Module = true;
        // since button touched .. to quick vibrate
        vibrate_M5Core2Module(LONG_VIBRATE);
    }
    else if (M5.BtnB.wasReleasefor(1000))
    {
        // since blong utton touched .. do long vibrate
        vibrate_M5Core2Module(LONG_VIBRATE);
        //! 1.24.24 Goto the scrolling text mode .. if flag
        setShowingScrollingTextMode_displayModule(false);
        
        //        buttonA_longPress_M5Core2Module();
        SerialDebug.println(" **** B. LONG PRESS ***");
        //! update the model (ButtonProcessing)
        buttonB_LongPress();
        //! set A/B/C button touched ..
        _buttonTouched_M5Core2Module = true;
    }
    else if (M5.BtnB.wasReleased())
    {
        // since button touched .. to quick vibrate
        vibrate_M5Core2Module(SHORT_VIBRATE);
        //! 1.24.24 Goto the scrolling text mode .. if flag
        setShowingScrollingTextMode_displayModule(false);
        
        //        buttonA_shortPress_M5Core2Module();
        SerialDebug.println(" **** B. SHORT PRESS ***");
        //! update the model (ButtonProcessing)
        buttonB_ShortPress();
        //! set A/B/C button touched ..
        _buttonTouched_M5Core2Module = true;
    }
    
#endif // M5Core2
#endif //ESP_M5
}

//!big button on front of M5StickC Plus
void checkButtonA_M5Core2Module()
{
    
    //! THIS is code for one of the button .. the screen
#ifdef ESP_M5
#ifdef M5CORE2_MODULE

    //!NOTE: ths issue is the timer is interruped by the scanner.. so make long-long very long..
    //was 1000  (from 500)
    if (M5.BtnA.wasReleasefor(3500))
    {
        // since long button touched .. do long  vibrate
        vibrate_M5Core2Module(LONG_VIBRATE);
        //        buttonA_longPress_M5Core2Module();
        SerialDebug.println(" **** LONG LONG PRESS ***");
        //! set A/B/C button touched ..
        _buttonTouched_M5Core2Module = true;
    }
    else if (M5.BtnA.wasReleasefor(1000))
    {
        // since button touched .. do long vibrate
        vibrate_M5Core2Module(LONG_VIBRATE);
        //! 1.24.24 Goto the scrolling text mode .. if flag
        setShowingScrollingTextMode_displayModule(false);
        
        //        buttonA_longPress_M5Core2Module();
        SerialDebug.println(" **** LONG PRESS ***");
        
        //! update the model (ButtonProcessing)
        buttonA_LongPress();
        //! set A/B/C button touched ..
        _buttonTouched_M5Core2Module = true;
    }
    else if (M5.BtnA.wasReleased())
    {
        // since button touched .. do quick vibrate
        vibrate_M5Core2Module(SHORT_VIBRATE);
        //! 1.24.24 Goto the scrolling text mode .. if flag
        setShowingScrollingTextMode_displayModule(false);
        
        //        buttonA_shortPress_M5Core2Module();
        SerialDebug.println(" **** SHORT PRESS ***");
        buttonA_ShortPress();
        //! set A/B/C button touched ..
        _buttonTouched_M5Core2Module = true;
        
    }
    
#endif // M5Core2
#endif //ESP_M5
}

//! init any globals
//! 1.5.24
void initGlobals_M5Core2Module()
{
    //! ability to turn on/off the M5Core2Module's scanning
    _isOn_M5Core2Module = true;
    
    //! set if any A/B/C button touched ..
    _buttonTouched_M5Core2Module = false;
}

//!  the setup() for this ATOM
void setup_M5Core2Module()
{
    SerialDebug.println("setup_M5Core2Module");
    initGlobals_M5Core2Module();
    
    //! call M5.begin () only here since this Module is only one in this build
  //  M5.begin();
   // M5.begin(true, true, true, false);

    
    //!Init IMU.  初始化IMU
    //! initi the motion sensor IMU/AXP
    //! NOTE: on the M5Core2 its' M5.IMU (not M5.Imu)
    M5.IMU.Init();
    /*
    M5.Axp.begin(); // blanks the screen ...
Then shows these error messages:
setup_M5Core2Module
axp: vbus limit off
axp: gpio1 init
axp: gpio2 init
axp: rtc battery charging enabled
axp: esp32 power voltage was set to 3.35v
axp: lcd backlight voltage was set to 2.80v
axp: lcd logic and sdcard voltage preset to 3.3v
axp: vibrator voltage preset to 2v
    */
    _InactivityTimeOut = Elapsed3mins;
    
#ifdef KEY_UNIT_SENSOR_CLASS
    _KeyUnitSensorClass_M5Core2Module = new KeyUnitSensorClass((char*)"_KeyUnitSensorClass_M5Core2Module");
    //! specify the callback
    _KeyUnitSensorClass_M5Core2Module->registerCallback(&M5Core2Callback);
    //! call the setup
    _KeyUnitSensorClass_M5Core2Module->setup();
    //SerialDebug.printf("_Key = %p\n", _KeyUnitSensorClass_M5Core2Module);
#endif
}



//! the loop
void loop_M5Core2Module()
{
    //!Read the press state of the key.  读取按键 A, B, C 的状态
    M5.update();
    
    _buttonTouched_M5Core2Module = false;
    checkButtonA_M5Core2Module();
    checkButtonB_M5Core2Module();
    checkButtonC_M5Core2Module();
    
    if (!_buttonTouched_M5Core2Module)
    {
        TouchPoint_t pos = M5.Touch.getPressPoint();
        //  if (pos.y != -1 && pos.y < 100)
        if (pos.y != -1 && pos.x != -1)
        {
            SerialDebug.printf("TOUCH = %d,%d\n", pos.x, pos.y);
            //! it seems the 3 A/B/C buttons are in the touch screen too...
            if (pos.y < 220)
            {
                //! VALID ... touch
                //  scrollText_displayModule();
                SerialTemp.printf("M5Core2 Screen Touched(%d,%d)\n", pos.x, pos.y);
                sendMessageString_mainModule((char*)"M5Core2 Screen Touched .. sending feed");
                
                //This (in mainModule) will look if connected so sends over BLE, otherwise MQTT (if connected)
                main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
                
                //!vibrate the display
                vibrate_M5Core2Module(LONG_LONG_VIBRATE);
                
                //! 2.27.24 make the button seem to be touched
                showButtonTouched_displayModule();
            }
        }
    }
    else
    {
     
    }

    //! do loop code
    loopCode_M5Core2Module();
}

//! LOOP code.. refactored so it's also called when a buttonPress message arrives
//! the button global has been set.. in the checkButton()
//!   HERE we only have button B (for now)
void loopCode_M5Core2Module()
{
#ifdef KEY_UNIT_SENSOR_CLASS
    //SerialDebug.printf("_Key = %p\n", _KeyUnitSensorClass_M5Core2Module);
    
    _KeyUnitSensorClass_M5Core2Module->loop();
#endif
    
    // check for tilt etc..
    if (checkMotion_ButtonProcessing())
    {
        SerialTemp.println("motion detected");
        sendMessageString_mainModule((char*)"M5Core.TiltDetected");
        
        //This (in mainModule) will look if connected so sends over BLE, otherwise MQTT (if connected)
        main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
        
        //!vibrate when tilted..
        vibrate_M5Core2Module(SHORT_VIBRATE);

    }

}


//!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
char * currentStatusURL_M5Core2Module()
{
    return (char*)"&m5core2=on";
}

//!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * currentStatusJSON_M5Core2Module()
{
    return (char*)"'m5core2':'on'";
}

#endif
