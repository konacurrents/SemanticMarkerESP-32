//! \link UIModule

// 
// 
// 
// 

#include "UI.h"

#ifdef ESP_M5
#else

#ifdef BOARD
//!initialize the board layoud, which is different than default

void setup_UIModule()
{
	SerialDebug.println("XXXXXXXXXXXXXXXX BOARD Defined  XXXXXXXXXXXXXXXXXXX");
	boolean buzzStatus = getBuzzStatus_StepperModule();

	pinMode(BLUE_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);
	pinMode(BUZZ, OUTPUT);
	
	digitalWrite(BLUE_LED, LOW);
	digitalWrite(GREEN_LED, HIGH);  //turn the power light on  
	
	if (buzzStatus)
	{
		digitalWrite(YELLOW_LED, HIGH);
	}
	else
	{
		digitalWrite(YELLOW_LED, LOW);
	}

	pinMode(TOUCH_1, INPUT);
	pinMode(TOUCH_2, INPUT);
	pinMode(TOUCH_3, INPUT);

	pinMode(POWER_PIN, OUTPUT);
	digitalWrite(POWER_PIN, LOW); //nessesary to enable 6.3V circuit
	SerialDebug.println("POWER_PIN set LOW");
}
//! the main loop()
void loop_UIModule()
{
    //nothing yet
}
#else


void setup_UIModule()
{
	SerialDebug.println("OOOOOOOOOOOOOOOO No BOARD Defined OOOOOOOOOOOOOOOOOOO");
	pinMode(BUZZ, OUTPUT);
	
	digitalWrite(BUZZ, LOW);
}
	
//! the main loop()
void loop_UIModule()
{
    //nothing yet
}
#endif  //BOARD

//!UI specific actions
void setBuzzerLight_UIModule(boolean onFlag)
{
    SerialDebug.printf("setBuzzerLight %s\n",onFlag?"ON":"OFF");

#ifdef BOARD
    //set the buzzer light on
    digitalWrite(YELLOW_LED, onFlag?HIGH:LOW);
#endif

}

//!callback for SOLID blinking led
//void solidLight(char *message)
void solidLightOnOff_UIModule(boolean onOff)
{
#ifdef ESP_32
    SerialDebug.println("solidLight..");
    digitalWrite(LED, onOff ? HIGH : LOW);
#endif
}

//!blink the LED
void blinkLED_UIModule() {
#ifdef ESP_32
    // grab the buzzStatus right now
    boolean buzzStatus = getBuzzStatus_StepperModule();
	int stepperAngle =  getPreferenceFloat_mainModule(PREFERENCE_STEPPER_ANGLE_FLOAT_SETTING);
	int stepperSteps =  (int) (stepperAngle / 0.176);
    
    SerialDebug.printf("BlinkLed  LED=%d, HIGH=%d, LOW=%d, BUZZ=%d, buzzStatus=%d, StepperAngle=%d, Steps=%d\n", LED, HIGH, LOW, BUZZ, buzzStatus, stepperAngle, stepperSteps);
#ifdef BOARD
    digitalWrite(BLUE_LED, HIGH);         // Flash the LED
#else
    digitalWrite(LED, HIGH);         // Flash the LED
#endif //BOARD

    if (buzzStatus)
    {
        digitalWrite(BUZZ, HIGH);         // Buzz the Piezo
    }
    delay(150);                           // Was 300
#ifdef BOARD
    digitalWrite(BLUE_LED, LOW);          // Flash the LED
#else
    digitalWrite(LED, LOW);
#endif //BOARD

    
    if (buzzStatus)
    {
        digitalWrite(BUZZ, LOW);
    }
    delay(30);                       // Kill time to detect double beep
#endif //ESP_32
}

#endif  //ESP_M5
