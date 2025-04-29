
#include "KeyUnitSensorClass.h"

#include "../../defines.h"
#if defined(ESP_M5) && !defined(ESP_M5_MINIMAL_SENSORS)

//#include <iostream>
#include <string.h>
#include <stdio.h>

#ifdef ESP_M5
#ifdef M5CORE2_MODULE
#include <M5Display.h>
#else
#ifdef M5STICKCPLUS2
#include <M5StickCPlus.h>
#endif

#endif
#endif

KeyUnitSensorClass::KeyUnitSensorClass(char *config)  : SensorClassType(config)
{
    
    printf("KeyUnitSensorClass init %s\n", config);
    _ledColor = 0;
    _leds = new CRGB[1] ;       // Define the array of leds.  定义LED阵列.

}

KeyUnitSensorClass::~KeyUnitSensorClass()
{
   printf("KeyUnitSensorClass destroy\n");
}


#define USE_LED
#ifdef USE_LED
#ifdef USE_FAST_LED
#include <FastLED.h>
#endif

//! 8.20.24
//! The PIN map stuff is tricky, and I don't know how to do this except via configuration messages
//! (or Compile defines).
//! When using the M5Atom, the PIN Port (next to the power is: G19, G21,G22,G23,G25, G33)
//! But if using the the PORT B or the IC-2, the pins are DATA_PIN 23, KEY_PIN 33
//! But the port out the back in DATA_PIN 26, KEY_PIN 32
//! HOW TO KNOW THIS ???   With the QRCode scanner, it's the port out the back
//! If the
//! @see https://shop.m5stack.com/products/mechanical-key-button-unit
//! 8.21.24 .. quick test, will use factoryClockwise to change these for a test
//! if factoryClockwise then it's plugged into the M5 (DATA_PIN 26, KEY_PIN 32)
//! if not factoryClockwise then it's the extension ..  (DATA_PIN 23, KEY_PIN 33)

#ifdef ATOM_QRCODE_MODULE
//! port b
#define DATA_PIN 26
int getDATA_PIN()
{
    int pin;
    if (getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_FACTORY_CLOCKWISE_MOTOR_DIRECTION_SETTING))
        pin= 26;
    else
        pin= 23;
   // SerialDebug.printf("getDATA_PIN = %d\n", pin);
    return pin;
}
#else
#define DATA_PIN 32  // Define LED pin.  定义LED引脚.
int getDATA_PIN() { return DATA_PIN; };

#endif
#endif

//#define USE_LED_BREATH  //not working, (working for M5button and Core2)
//! for ATOM (maybe change the priority and see what happens)


//!wrapper static method
void KeyUnitSensorClass::startTaskImpl(void* _this)
{
    SerialDebug.printf(" startTaskImpl == %p\n", _this);
    
#ifdef USE_LED_BREATH
     ((KeyUnitSensorClass*)_this)->keyUnitLED();
#endif
}
#define KEY_UNIT_GROVE
#ifdef  KEY_UNIT_GROVE
#ifdef ATOM_QRCODE_MODULE
//! port b
#define KEY_PIN 32
int getKEY_PIN()
{
    int pin;
    if (getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_FACTORY_CLOCKWISE_MOTOR_DIRECTION_SETTING))
        pin = 32;
    else
        pin = 33;
   // SerialDebug.printf("getKEY_PIN = %d\n", pin);
    return pin;

}
#else
#define KEY_PIN 33 //Define Key Pin.  定义Key引脚
int getKEY_PIN() { return KEY_PIN; };
#endif //ATOM_QRCODE_MODULE
                   //!setup the KeyUnit
void KeyUnitSensorClass::setupKeyUnit()
{
    SerialDebug.printf(" setupKeyUnit == %p\n", this);

    pinMode(getKEY_PIN(), INPUT_PULLUP);  // Init Key pin.  初始化Key引脚.
    
#ifdef USE_LED
    getDATA_PIN();
    getKEY_PIN();
    if (getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_FACTORY_CLOCKWISE_MOTOR_DIRECTION_SETTING))
        FastLED.addLeds<SK6812, 26, GRB>(this->_leds,
                                                    1);  // Init FastLED.  初始化FastLED.
    else
        FastLED.addLeds<SK6812, 23, GRB>(this->_leds,
                                                    1);  // Init FastLED.  初始化FastLED.
#endif
#ifdef USE_LED_BREATH  //not working,
    //!@see https://stackoverflow.com/questions/45831114/c-freertos-task-invalid-use-of-non-static-member-function
    //!@see https://www.freertos.org/FreeRTOS_Support_Forum_Archive/July_2010/freertos_Is_it_possible_create_freertos_task_in_c_3778071.html
    //!@see https://stackoverflow.com/questions/77931188/c-freertos-task-this-instance-being-passed-as-null?noredirect=1#comment137393059_77931188
    //!4th parameter cannot be NULL but instead is 'this'
    //!NOTE: 2024 stack size needed .. otherwise stack overflow 2.5.24 (my birthday)
    xTaskCreate(
                &this->startTaskImpl, "led", 2024, this, 0,
                NULL);  // Create a thread for breathing LED. 创建一个线程用于LED呼吸灯.
#endif
}


//! loop the key unit (after other called M5.updfate)
void KeyUnitSensorClass::loopKeyUnit()
{
    if (!digitalRead(getKEY_PIN()))
    {
        // If Key was pressed.  如果按键按下.
        SerialDebug.println("Key Pressed");
#ifdef USE_LED
        changeLedColor();  // Change LED color.  更换LED呼吸灯颜色.
#endif
        //! THERE should be a time limit on the button .. say 10 or so..
        while (!digitalRead(getKEY_PIN()))
            // Hold until the key released.  在松开按键前保持状态.
            ;
        SerialDebug.println("Key Released");
        
        //!call the callback
        callCallback((char*)"keyPressed",true);
        
    }
}
#ifdef USE_LED_BREATH
void KeyUnitSensorClass::keyUnitLED() {

    SerialDebug.printf(" setupKeyUnit %p\n", this);
    //! default to RED
    _leds[0] = CRGB::Red;
    for (;;) {
        for (int i = 0; i < 255; i++)
        {
            //! Set LED brightness from 0 to 255.  设置LED亮度从0到255.
            FastLED.setBrightness(i);
            FastLED.show();
            delay(5);
        }
        for (int i = 255; i > 0;  i--)
        {
            //! Set LED brightness from 255 to 0.  设置LED亮度从255到0.
            FastLED.setBrightness(i);
            FastLED.show();
            delay(5);
        }
    }
    vTaskDelete(NULL);
}
#endif

void KeyUnitSensorClass::changeLedColor() {
    SerialDebug.println("changeLedColor");
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
#endif //key unit grove



//! error: https://stackoverflow.com/questions/3065154/undefined-reference-to-vtable
//!@see https://www.w3schools.com/cpp/cpp_class_methods.asp
void KeyUnitSensorClass::loop()
{
    //!setup the KeyUnit
    loopKeyUnit();
}

void KeyUnitSensorClass::setup()
{
    printf("KeyUnitSensorClass::setup()\n");
    
    setupKeyUnit();
}

#endif //ESP_M5
