#ifndef StepperModule_h
#define StepperModule_h

#include "../../Defines.h"

//#include "../UIModule/UI.h"

//!the main loop for the StepperModule (this is the FeederStateMachine)
void loop_StepperModule();

//!called on setup()
void setup_StepperModule();

//!the Blink the LED - and it will use the latest BUZZER status (so MQTT could set buzzer on.. then call this)
//void stepperModule_BlinkLED();

/**
SerialDebug.println("Valid Commands: ");
SerialDebug.println("         H == help, this message");
SerialDebug.println(" 0x0, s, c == Single Feed ");
SerialDebug.println("         a == AutoFeed ");
SerialDebug.println("         u == UNO ");
SerialDebug.println("         m == MINI ");
SerialDebug.println("         L == Tumbler ");
SerialDebug.println("         B == Buzzer On");
SerialDebug.println("         b == Buzzer Off");
SerialDebug.println("         G == Gateway On");
SerialDebug.println("         g == gateway Off");
SerialDebug.println("         R == clean credentials");
SerialDebug.println("         O == OTA update");
SerialDebug.println("         X == clean EPROM");
 SerialDebug.println("         T == tilt on");
 SerialDebug.println("         t == tilt off");


*/
//!processes a single character command from BLE (or from MQTT converted to a single character)
void stepperModule_ProcessClientCmdFinal(char cmd);



/*volume info, B = buzzOn, b = buzzOff*/
//#define BUZZON 1
//#define BUZZOFF 0

/*feederType*/
#define UNO			1
#define MINI		2
#define TUMBLER     3
/*ESP32 GPIO*/
#define LED			2
//#define BUZZ     26


//! retreives the feeder type, UNO or MINI
extern int getFeederType();

//! retrieves the buzzer status
boolean getBuzzStatus_StepperModule();

//! 4.1.25 April Fools day. Mt Peak hike (162 bmp strange spike)
//! get the identity of the SenasorPlug
//! nil if non
char *stepperIdentity_StepperModule();

#endif // Dispense_h

