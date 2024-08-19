#pragma once 

#include "../../Defines.h"

#ifdef ESP_M5
// not used by M5, but there is a pin 2 that buzzes
#define BUZZ       23

#elif defined(BOARD)
/***************** LED definitions *******************************/
#define BLUE_LED   32
#define GREEN_LED  33
#define YELLOW_LED 25

/***************** Touch Pad definitons **************************/
#define TOUCH_1     4
#define TOUCH_2    13
#define TOUCH_3    15

/***************** Other definintions ****************************/
#define BUZZ       23
#define POWER_PIN  18


#else //older without the board .. but with a buzzer
#define BUZZ       26

#endif //ESP_M5

/***************** End definitons ** Start assignments **********/
void setup_UIModule();
void loop_UIModule();

//!UI specific actions
void setBuzzerLight_UIModule(boolean onFlag);

//!turns on/off a solid light
void solidLightOnOff_UIModule(boolean onOff);

//!blink the LED
void blinkLED_UIModule();
