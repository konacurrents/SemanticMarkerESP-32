#include "../../defines.h"

#ifdef M5BUTTON_MODULE

//#define TRY_SENSOR_CLASS
//! Defined in defines.h
#ifdef  KEY_UNIT_SENSOR_CLASS

#include "../SensorClass/SensorClassType.h"
#include "../SensorClass/KeyUnitSensorClass.h"

//! instance of the class
KeyUnitSensorClass *_KeyUnitSensorClass;


//a pointer to a callback function that takes (char*) and returns void
void M5Callback(char *parameter, boolean flag)
{
    SerialDebug.printf("M5.sensorCallbackSignature(%s,%d)\n", parameter, flag);
    
    sendMessageString_mainModule((char*)"M5.KEY Pressed -- call feed");
    
    //This (in mainModule) will look if connected so sends over BLE, otherwise MQTT (if connected)
    main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
}
#endif


//! Ultrasonic-I2C
//! 2.23.24 after nice ski day, Da King beautiful weather (Laura along)
#ifdef ULTRASONIC_I2C_SENSOR_CLASS
#include "../SensorClass/SensorClassType.h"
#include "../SensorClass/Sonic_I2CSensorClass.h"
//! instance of the class
Sonic_I2CSensorClass *_Sonic_I2CSensorClass;

//a pointer to a callback function that takes (char*) and returns void
void M5CallbackValue(char *parameter, int value)
{
    SerialDebug.printf("M5.sensorCallbackValueSignature(%s,%d)\n", parameter, value);
    
    //sendMessageString_mainModule((char*)"M5.KEY Pressed -- call feed");
    
    //This (in mainModule) will look if connected so sends over BLE, otherwise MQTT (if connected)
    //main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
}

#endif

/**
 *  \link ButtonModule
 */
/*
 *******************************************************************************
 // https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
 
 Enable (EN)
 Enable (EN) is the 3.3V regulatorâs enable pin. Itâs pulled up, so connect to ground to disable the 3.3V regulator. This means that you can use this pin connected to a pushbutton to restart your ESP32, for example.
 
 Scott Version: 1.1.2022
 *******************************************************************************
 */

//@see https://github.com/m5stack/M5StickC-Plus
//@see https://github.com/m5stack/M5StickC-Plus/blob/master/src/utility/Button.h

//! 3.3.22 Using the new JSON library which is supposed to catch syntax errors without blowing up
//https://arduinojson.org/?utm_source=meta&utm_medium=library.properties

/**
 class Button {
 public:
 Button(uint8_t pin, uint8_t invert, uint32_t dbTime);
 uint8_t read();
 */
/*----------------------------------------------------------------------*
 * isPressed() and isReleased() check the button state when it was last *
 * read, and return false (0) or true (!=0) accordingly.                *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
/*
 uint8_t isPressed();
 uint8_t isReleased();
 */
/*----------------------------------------------------------------------*
 * wasPressed() and wasReleased() check the button state to see if it   *
 * changed between the last two reads and return false (0) or           *
 * true (!=0) accordingly.                                              *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
/*
 uint8_t wasPressed();
 uint8_t wasReleased();
 */
/*----------------------------------------------------------------------*
 * pressedFor(ms) and releasedFor(ms) check to see if the button is     *
 * pressed (or released), and has been in that state for the specified  *
 * time in milliseconds. Returns false (0) or true (1) accordingly.     *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/

/*
 uint8_t pressedFor(uint32_t ms);
 uint8_t releasedFor(uint32_t ms);
 uint8_t wasReleasefor(uint32_t ms);
 */
/*----------------------------------------------------------------------*
 * lastChange() returns the time the button last changed state,         *
 * in milliseconds.                                                     *
 *----------------------------------------------------------------------*/
/*
 uint32_t lastChange();
 */
/*
 class Button {
 public:
 Button(uint8_t pin, uint8_t invert, uint32_t dbTime);
 uint8_t read();
 uint8_t isPressed();
 uint8_t isReleased();
 uint8_t wasPressed();
 uint8_t wasReleased();
 uint8_t pressedFor(uint32_t ms);
 uint8_t releasedFor(uint32_t ms);
 uint8_t wasReleasefor(uint32_t ms);
 uint32_t lastChange();
 */
#ifdef ESP_M5
#ifdef M5STICKCPLUS2
//#include <M5StickCPlus.h>
#endif
#endif


//extension of ButtonModule - includes Sensors

//! 1.11.24 work on the DM5_DLight sensor
//! @Links [Unit DLight](https://docs.m5stack.com/en/unit/dlight)
//! @Links [HAT DLight](https://docs.m5stack.com/en/hat/hat_dlight)
//! This will use the PIR Semantic Marker logic (although it needs an OFF or the opposite of the ON)
#include <M5_DLight.h>
//! the sensor object for the DLight
M5_DLight _M5DLightSensor;

//! this will be set in the setup()
boolean _M5DlightSensorALIVE = false;

//! ALIVE set on setup() or status()
boolean _PIRSensorALIVE = true;
//! check sensor alive
void checkPIRSensorSensorStatus()
{
    //! this one is so fast it doesn't matter
    _PIRSensorALIVE = true;
}

//! classify the status
void classifyM5DlightSensorValue(int lux)
{
    //! if > = 65535 then it's not plugged in..
    if (lux > 65000)
        _M5DlightSensorALIVE = false;
    else
        _M5DlightSensorALIVE = true;
}

//! check sensor alive
void checkM5DlightSensorStatus()
{
    //! get the sensor value
    uint16_t lux = _M5DLightSensor.getLUX();
    
    //!classify which sets the _M5DLightSensorAlive flag
    classifyM5DlightSensorValue(lux);
    SerialDebug.printf("checkM5DlightSensorStatus alive=%d\n",_M5DlightSensorALIVE );

}

//! this status will be called and let the ALIVE re-evaluate
void statusM5ButtonModule()
{
    checkM5DlightSensorStatus();
    checkPIRSensorSensorStatus();
}



//#include <M5Display.h>

#include "M5ButtonModule.h"

//! called by the feed operation to say the device is still running.. and count it as a button click.
void refreshDelayButtonTouched_M5ButtonModule()
{
    refreshDelayButtonTouched_ButtonProcessing();
}

// See https://forum.arduino.cc/t/arduino-push-button-double-click-function/409353
//!moved to the KeyUnitSensorClass ....
#ifdef NOT_HERE_MOVED_TO_UNIT_CLASS

#define USE_LED
#ifdef USE_LED
#include <FastLED.h>
uint8_t _ledColor = 0;
#define DATA_PIN 32  // Define LED pin.  定义LED引脚.
CRGB _leds[1];        // Define the array of leds.  定义LED阵列.
void keyUnitLED(void *parameter);
void changeLedColor();
#endif

#define KEY_UNIT_GROVE
#ifdef  KEY_UNIT_GROVE
#define KEY_PIN 33 //Define Key Pin.  定义Key引脚
//!setup the KeyUnit
void setupKeyUnit()
{
    pinMode(KEY_PIN, INPUT_PULLUP);  // Init Key pin.  初始化Key引脚.
    
#ifdef USE_LED
    FastLED.addLeds<SK6812, DATA_PIN, GRB>(_leds,
                                           1);  // Init FastLED.  初始化FastLED.
    
    xTaskCreate(
                keyUnitLED, "led", 1000, NULL, 0,
                NULL);  // Create a thread for breathing LED. 创建一个线程用于LED呼吸灯.
#endif
}

//! 2.21.25 add a way to change the button color (if any)
void changeButtonColor_M5ButtonModule()
{
#ifdef USE_LED
    SerialDebug.println("changeButtonColor_M5ButtonModule");
    changeLedColor();  // Change LED color.  更换LED呼吸灯颜色.
#endif
}

//! loop the key unit (after other called M5.updfate)
void loopKeyUnit()
{
    if (!digitalRead(KEY_PIN)) 
    {
        // If Key was pressed.  如果按键按下.
        SerialDebug.println("M5ButtonModule.Key Pressed");
#ifdef USE_LED
        changeLedColor();  // Change LED color.  更换LED呼吸灯颜色.
#endif

        while (!digitalRead(KEY_PIN))
            // Hold until the key released.  在松开按键前保持状态.
            ;
        SerialDebug.println("Button.Key Released");
        
        sendMessageString_mainModule((char*)"KEY Pressed -- call feed");
        
        //This (in mainModule) will look if connected so sends over BLE, otherwise MQTT (if connected)
        main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
    }
}
#ifdef USE_LED
void keyUnitLED(void *parameter) {
    _leds[0] = CRGB::Red;
    for (;;) {
        for (int i = 0; i < 255;
             i++) {  // Set LED brightness from 0 to 255.  设置LED亮度从0到255.
            FastLED.setBrightness(i);
            FastLED.show();
            delay(5);
        }
        for (int i = 255; i > 0;
             i--) {  // Set LED brightness from 255 to 0.  设置LED亮度从255到0.
            FastLED.setBrightness(i);
            FastLED.show();
            delay(5);
        }
    }
    vTaskDelete(NULL);
}

void changeLedColor() {
    _ledColor++;
    if (_ledColor > 2) _ledColor = 0;
    switch (_ledColor)
    {  // Change LED colors between R,G,B.  在红绿蓝中切换LED颜色.
        case 0:
            _leds[0] = CRGB::Red;
            break;
        case 1:
            _leds[0] = CRGB::Green;
            break;
        case 2:
            _leds[0] = CRGB::Blue;
            break;
        default:
            break;
    }
}
#endif
#endif

#endif // TRY_SENSOR_CLASS


//! set a previous LUX so can see if changed from light to dark
#define LUX_DARK -1
#define LUX_SAME 0
#define LUX_LIGHT 1
int _previousLUXKind = LUX_SAME;

#define SENSORS_MOTION_PIR
#ifdef SENSORS_MOTION_PIR
#define PIR_PIN 36 //passive IR Hat
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


//!Return "true" on PIR (over sensitivity) and false otherwise
//!This is just a sensor (if plugged in) - so any timing is on the caller of this
bool checkPIR_ButtonProcessing()
{
    //    if (!getPreferenceBoolean_mainModule(PREFERENCE_SENSOR_PIR_VALUE))
    //        return false;
    
    boolean triggeredPIR = digitalRead(PIR_PIN);
    if (triggeredPIR)
    {
        if ((millis()-_PrevTriggerTime)>Elapsed4secs)
        {
            triggeredPIR = true;
            _PrevTriggerTime = millis();
            SerialLots.println("triggeredPIR = true");
        }
        else
            triggeredPIR = false;
    }
    return triggeredPIR;
}

//! only check the LUX every MAX loop times..
int _luxButtonCount = 0;
#define LUXBUTTOMAX 5
//! 1.23.24 Seems the getLUX() is a VERY SLOW operation and breaks
//! button timing, etc.
//!1.11.24 work on the DHat light sensor
//!Return "true" on PIR (over sensitivity) and false otherwise
//!This is just a sensor (if plugged in) - so any timing is on the caller of this
//!returns the return 0 == no change, -1 = dark, and 1 == light
//!@see https://github.com/konacurrents/ESP_IOT/issues/287
int checkDLight_ButtonProcessing()
{
#ifdef NONO
    _luxButtonCount++;
    if (_luxButtonCount < LUXBUTTOMAX)
    {
        return LUX_SAME;
    }
    else
    {
        _luxButtonCount = 0;
    }
#endif
    //! determing what the LUX values mean..
    int LUXKind = LUX_LIGHT;
    //SerialTemp.println("start: checkDLight_ButtonProcessing");
#ifdef ESP_M5
    
    //! get the sensor value
    uint16_t lux = _M5DLightSensor.getLUX();
    
    //! if > = 65535 then it's not plugged in..
    SerialLots.printf("mid:   checkDLight_ButtonProcessing(lux=%d)\n", lux);
    classifyM5DlightSensorValue(lux);

    //! grab the temporary threshold
    int  luxThresholdDark = getLUXThreshold_mainModule(THRESHOLD_KIND_DARK);
    
    //!TODO: set this threshold via a EPROM value
    if (lux < luxThresholdDark)
    {
        //! dark
        LUXKind = LUX_DARK;
        
    }
    else
    {
        //! low light different (Note: it might be nice to have variations ...)
        LUXKind = LUX_LIGHT;
    }
    
    boolean changedLight = true;
    //! has this transitioned from drak to light??
    if (LUXKind == _previousLUXKind)
    {
        changedLight = false;
        LUXKind = LUX_SAME;
    }
    if (changedLight)
    {
        _previousLUXKind = LUXKind;
        SerialLots.printf("DLight LUX changed = lux=%d == kind=%d\n", lux, LUXKind);
    }
#endif
    SerialLots.printf("end:   checkDLight_ButtonProcessing kind=%d\n", LUXKind);

    return LUXKind;
}
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
        // M5.IMU.getGyroData(&_IMU.gyroX,&_IMU.gyroY,&_IMU.gyroZ);
#ifndef M5STICKCPLUS2

        M5.IMU.getAccelData(&_IMU.accX,&_IMU.accY,&_IMU.accZ);
        //  M5.IMU.getAhrsData(&_IMU.pitch,&_IMU.roll,&_IMU.yaw);
        
        //debug print
        //SerialTemp.printf("%5.2f  %5.2f  %5.2f   \n\r", _IMU.accX, _IMU.accY, _IMU.accZ);
        //        SerialTemp.printf("%5.2f  %5.2f  %5.2f   \n\r", _IMU.gyroX, _IMU.gyroY, _IMU.gyroZ);
        //        SerialTemp.printf("%5.2f  %5.2f  %5.2f   \n\r", _IMU.pitch, _IMU.roll, _IMU.yaw);
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
        SerialLots.printf("diff:  %.2f  \r\n", _IMU.diffXYZ);
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


//!the setup for buttonProcessing (extension of ButtonModule)
//!  在 M5StickC Plus 启动或者复位后，即会开始执行setup()函数中的程序，该部分只会执行一次。
void setup_M5ButtonModule()
{
    setCurrentSMMode_mainModule(0);
    
#ifdef  KEY_UNIT_GROVE
    //!setup the KeyUnit
    setupKeyUnit();
#endif //KEY_UNIT_GROVE
#ifdef KEY_UNIT_SENSOR_CLASS
    _KeyUnitSensorClass = new KeyUnitSensorClass((char*)"KeyUnitInstanceM5Button");
    //! specify the callback
    _KeyUnitSensorClass->registerCallback(&M5Callback);
    //! call the setup
    _KeyUnitSensorClass->setup();
#endif
    
    //! Ultrasonic-I2C
    //! 2.23.24 after nice ski day, Da King beautiful weather (Laura along)
#ifdef ULTRASONIC_I2C_SENSOR_CLASS
     _Sonic_I2CSensorClass = new Sonic_I2CSensorClass((char*)"Sonic_I2CSensorClass");
    //! specify the callback
    _Sonic_I2CSensorClass->registerCallbackValue(&M5CallbackValue);
    //! call the setup
    _Sonic_I2CSensorClass->setup();
#endif
    
#ifdef ESP_M5
    //!setup the plugin PIR (if there..)
    pinMode(PIR_PIN,INPUT_PULLUP);
    
    //M5.begin();  already called..
    //Init IMU.  初始化IMU
    
#ifndef M5STICKCPLUS2

    M5.Imu.Init();
    M5.Axp.begin();
#endif
    _InactivityTimeOut = Elapsed3mins;
    
    //!https://docs.m5stack.com/en/api/stickc/pwm
    //!
    //    ledcSetup(_channelBuzzer, _freq, _resolution_bits);
    //    ledcAttachPin(_buzzerPin, _channelBuzzer);
    //  M5.Beep.begin();
    
    //! M5DHAT hangs with 2.1.0 M5 board..
//#if defined(M5STACK_VERSION) && ("2.0.4" == "2.0.4")
#ifdef M5STICKCPLUS2
#else
#define M5DHAT
#endif
#ifdef  M5DHAT
    SerialDebug.println("_M5DLightSensor sensor begin.....");
    Wire.begin(0,26);
    _M5DLightSensor.begin();  // HAT DLight
    
    // CONTINUOUSLY_H_RESOLUTION_MODE
    // CONTINUOUSLY_H_RESOLUTION_MODE2
    // CONTINUOUSLY_L_RESOLUTION_MODE
    // ONE_TIME_H_RESOLUTION_MODE
    // ONE_TIME_H_RESOLUTION_MODE2
    // ONE_TIME_L_RESOLUTION_MODE
    _M5DLightSensor.setMode(CONTINUOUSLY_H_RESOLUTION_MODE);
    
    //! 1.23.24 check the sensor status
    checkM5DlightSensorStatus();
#endif
    
#endif
}
//!the loop for buttonProcessing (extension of ButtonModule)
void loopCode_M5ButtonProcessing();

#ifdef ESP_32
//state of the button..
boolean _buttonPressed;
#endif

//!small button on right side of M5StickC Plus
void checkButtonB_M5ButtonProcessing()
{
  //  SerialTemp.println("checkButtonB_ButtonProcessing");
    boolean buttonTouched = true;
#ifdef ESP_M5
    
#ifndef M5STICKCPLUS2
    //was 1000
    if (M5.BtnB.wasReleasefor(500))
    {
        SerialTemp.println("buttonB_LongPress");
        
        buttonB_LongPress();
    }
    //side button.. cycles through choices..
    else
#endif // not Cplus2
        if (M5.BtnB.wasReleased())
    {
        SerialTemp.println("buttonB_ShortPress");
        
        buttonB_ShortPress();
    }
    else
    {
        
        buttonTouched = false;
    }
    //if a button was touched, update the delay since no touch..
    if (buttonTouched)
    {
        refreshDelayButtonTouched_M5ButtonModule();
    }
#endif // ESP_M5
    
}

//!big button on front of M5StickC Plus
void checkButtonA_M5ButtonProcessing()
{
    boolean buttonTouched = true;
#ifdef ESP_M5
#ifndef M5STICKCPLUS2
    //was 1000
    if (M5.BtnA.wasReleasefor(500))
    {
        SerialDebug.println("buttonA LONG touched");
        buttonA_LongPress();
    }
    else 
#endif
        if (M5.BtnA.wasReleased())
    {
        SerialDebug.println("buttonA SHORT touched");

        buttonA_ShortPress();
    }
    else
    {
        buttonTouched = false;
    }
    //if a button was touched, update the delay since no touch..
    if (buttonTouched)
    {
        refreshDelayButtonTouched_M5ButtonModule();
    }
    
    
#endif //ESP_M5
}


//if _buttonPressed (the LOW state == 0)
// then wait for the HIGH state == 1
// and that is a button click..

void loop_M5ButtonModule()
{
    //TODO.. check if this is needed for the old ESP32 devices...
#ifdef ESP_32
    // put your main code here, to run repeatedly:
    int _buttonState;

    _buttonState = digitalRead(_buttonPin);
    switch (_buttonState)
    {
        case LOW: //0
            _buttonPressed = true;
            break;
        default:
            
        case HIGH: // 1  (nothing touched)
            if (_buttonPressed)
            {
                // then the button was unpressed..
                SerialLots.println("BUTTON PRESSED and RELEASED");
                _buttonPressed = false;
                
                callCallbackMain(CALLBACKS_BUTTON_MODULE, SINGLE_CLICK_BM, (char*)"B1");
                
            }
            break;
    }
    //SerialLots.printf("_buttonState = %d\n", _buttonState);
#endif
    //!calls the extension of ButtonModule
    loopCode_M5ButtonProcessing();
}

//!the loop for buttonProcessing (extension of ButtonModule)
void loopCode_M5ButtonProcessing()
{
    
    M5.update(); //Read the press state of the key. ONLY call once per loop or the status of B button is lost
#ifdef  KEY_UNIT_GROVE
    //!setup the KeyUnit
    loopKeyUnit();
#endif //KEY_UNIT_GROVE
#ifdef KEY_UNIT_SENSOR_CLASS
    _KeyUnitSensorClass->loop();
#endif
  
    //! Ultrasonic-I2C
    //! 2.23.24 after nice ski day, Da King beautiful weather (Laura along)
#ifdef ULTRASONIC_I2C_SENSOR_CLASS
    _Sonic_I2CSensorClass->loop();
#endif
    
    //! ALIVE set on setup() or status()
    if (_PIRSensorALIVE)
    {
        // check for tilt etc..
        if (checkMotion_ButtonProcessing())
        {
            SerialTemp.println("motion detected");
            sendMessageString_mainModule((char*)"M5.TiltDetected");
            
            //This (in mainModule) will look if connected so sends over BLE, otherwise MQTT (if connected)
            main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
        }
        
        // check for PIR etc..
        if (checkPIR_ButtonProcessing())
        {
            SerialTemp.println("PIR detected");
            sendMessageString_mainModule((char*)"PIRDetected");
            
            //! 1.10.24 support optionally sending a Sematic Marker command (MQTT instead)
            //! if on then call MQTT insteal.
            if ( getPreferenceBoolean_mainModule(PREFERENCE_SM_ON_PIR_SETTING))
            {
                //! 1.10.24 Flag on whether a Semantic Marker command is sent on PIR, and the Command to send
                //! 1.10.24 The  Semantic Marker command is sent on PIR, and the Command to send
                char *smCommand = getPreference_mainModule(PREFERENCE_SM_COMMAND_PIR_SETTING);
                
                SerialDebug.printf("SM_ON_PIR command = %s\n", smCommand);
                
                //This (in mainModule) will look if connected so sends over BLE, otherwise MQTT (if connected)
                //!send an async call with a string parameter. This will set store the value and then async call the command (passing the parameter)
                //!These are the ASYNC_CALL_PARAMETERS_MAX
                main_dispatchAsyncCommandWithString(ASYNC_JSON_MQTT_MESSAGE_PARAMETER, smCommand);
            }
            else
            {
                //This (in mainModule) will look if connected so sends over BLE, otherwise MQTT (if connected)
                main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
            }
        }
    }
#define TRY_WITHOUT
#ifdef TRY_WITHOUT

    //! ALIVE set on setup() or status()
    if (_M5DlightSensorALIVE)
    {
        
        //! 1.11.24 check for Dlight LUX DARK, LIGHT or same
        int DLightKind = checkDLight_ButtonProcessing();
        
        //! the sensor is evaluated again .. so if unplugged then ALIVE is false
        if (_M5DlightSensorALIVE)
        {
            //! now if it changed ..
            if (DLightKind != LUX_SAME)
            {
                SerialTemp.printf("DLight detected: %d\n", DLightKind);
                switch (DLightKind)
                {
                        //! in this sensor, the "OFF" is when the LIGHT is ON
                    case LUX_LIGHT:
                        sendMessageString_mainModule((char*)"DLight LUX Light");
                        
                        //! 1.10.24 support optionally sending a Sematic Marker command (MQTT instead)
                        //! if on then call MQTT insteal.
                        if ( getPreferenceBoolean_mainModule(PREFERENCE_SM_ON_PIR_SETTING))
                        {
                            //! 1.10.24 Flag on whether a Semantic Marker command is sent on PIR, and the Command to send
                            //! 1.10.24 The  Semantic Marker command is sent on PIR, and the Command to send
                            //char *smCommand = (char*) "{'set':'socket','val':'off'}"; //getPreference_mainModule(PREFERENCE_SM_COMMAND_PIR_SETTING);
                            char *smCommand = getPreference_mainModule(PREFERENCE_SM_COMMAND_PIR_OFF_SETTING);
                            
                            SerialDebug.printf("SM_ON_PIR_OFF command = %s\n", smCommand);
                            
                            //This (in mainModule) will look if connected so sends over BLE, otherwise MQTT (if connected)
                            //!send an async call with a string parameter. This will set store the value and then async call the command (passing the parameter)
                            //!These are the ASYNC_CALL_PARAMETERS_MAX
                            main_dispatchAsyncCommandWithString(ASYNC_JSON_MQTT_MESSAGE_PARAMETER, smCommand);
                        }
                        break;
                    case LUX_DARK:
                        //! use the LIGHT for the "ON"
                        sendMessageString_mainModule((char*)"DLight LUX Dark");
                        
                        //! 1.10.24 support optionally sending a Sematic Marker command (MQTT instead)
                        //! if on then call MQTT insteal.
                        if ( getPreferenceBoolean_mainModule(PREFERENCE_SM_ON_PIR_SETTING))
                        {
                            //! 1.10.24 Flag on whether a Semantic Marker command is sent on PIR, and the Command to send
                            //! 1.10.24 The  Semantic Marker command is sent on PIR, and the Command to send
                            char *smCommand = getPreference_mainModule(PREFERENCE_SM_COMMAND_PIR_SETTING);
                            
                            SerialDebug.printf("SM_ON_PIR command = %s\n", smCommand);
                            
                            //This (in mainModule) will look if connected so sends over BLE, otherwise MQTT (if connected)
                            //!send an async call with a string parameter. This will set store the value and then async call the command (passing the parameter)
                            //!These are the ASYNC_CALL_PARAMETERS_MAX
                            main_dispatchAsyncCommandWithString(ASYNC_JSON_MQTT_MESSAGE_PARAMETER, smCommand);
                        }
                        break;
                }
                
            }
        }
    }
    
#endif
    
    //checkBothPressed_ButtonProcessing();
    checkButtonA_M5ButtonProcessing();
    checkButtonB_M5ButtonProcessing();
}

//!This is only thing exposed to others.. (Kinda which only 1 button module)
//!short press on buttonA (top button)
void buttonA_ShortPress_M5ButtonModule()
{
    
}
//!long press on buttonA (top button)
void buttonA_LongPress_M5ButtonModule()
{
    
}
//!the long press of the side button
void buttonB_LongPress_M5ButtonModule()
{
    
}
//!the short press of the side button
void buttonB_ShortPress_M5ButtonModule()
{
    
}

#endif //USE_BUTTON_MODULE
