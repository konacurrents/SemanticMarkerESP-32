//! \link ButtonProcessing
//
//  ButtonProcessing.c
//  M5Stick
//
//  Created by Scott Moody on 3/9/22.
//

#include "ButtonProcessing.h"
#ifdef USE_BUTTON_MODULE
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
#include <M5StickCPlus.h>
#endif

#ifdef ESP_M5_TRY
//!https://docs.m5stack.com/en/api/stickc/pwm
//!
int _freq = 1800;
int _channelBuzzer = 0;
int _channelREDLED = 1;

int _resolution_bits = 8;
int _buzzerPin = 2;
int _LEDPin = 10;
#endif

//**** Delay Methods*******
#define SINGLE_DELAY
#ifdef SINGLE_DELAY
//https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html
//! the time the delay started
unsigned long _delayStart_ButtonProcessing;
//! true if still waiting for delay to finish
boolean _delayRunning_ButtonProcessing = false;
//! length of delay
int _delaySeconds_ButtonProcessing;
//!init the delay
void startDelay_ButtonProcessing(int seconds)
{
    SerialCall.printf("startDelay_ButtonProcessing: %d\n", seconds);
    
    _delayStart_ButtonProcessing = millis();   // start delay
    _delayRunning_ButtonProcessing = true; // not finished yet
    _delaySeconds_ButtonProcessing = seconds;
    
}
//!if finished..
boolean delayFinished_ButtonProcessing()
{
    if (_delayRunning_ButtonProcessing && ((millis() - _delayStart_ButtonProcessing) >= (_delaySeconds_ButtonProcessing * 1000)))
    {
        _delayRunning_ButtonProcessing = false;
        SerialCall.println("delayFinished_ButtonProcessing..");
        return true;
    }
    return false;
}

//!stop the delay
void stopDelay_ButtonProcessing()
{
    SerialCall.println("stopDelay_ButtonProcessing _delayRunning=false");
    
    _delayRunning_ButtonProcessing = false;
}

//!get the delay values
int getDelayNoClickPoweroffSetting()
{
    return getPreferenceInt_mainModule(PREFERENCE_NO_BUTTON_CLICK_POWEROFF_SETTING);
}

//!set when any button touched..
void refreshDelayButtonTouched()
{
    startDelay_ButtonProcessing(getDelayNoClickPoweroffSetting());
}

//! called by the feed operation to say the device is still running.. and count it as a button click.
void refreshDelayButtonTouched_ButtonProcessing()
{
    SerialLots.println("refreshDelayButtonTouched_ButtonProcessing");
    //! call the local implementation
    refreshDelayButtonTouched();
}


//!checks delay
void checkDelaySinceButtonTouched()
{
    if (delayFinished_ButtonProcessing())
    {
        if (getPreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE))
        {
            SerialMin.printf("GATEWAY so no poweroff, but %d seconds with no buttons/feed\n", getDelayNoClickPoweroffSetting());
        }
        else if (getDelayNoClickPoweroffSetting() == NO_POWEROFF_AMOUNT_MAIN)
        {
            SerialMin.printf("User set no poweroff, but %d seconds with no buttons/feed\n", getDelayNoClickPoweroffSetting());
        }
        else
        {
            
            //let others know what's happening..
            sendMessageString_mainModule((char*)"NoButtonActivity.. poweroff");
            
            SerialMin.printf("buttonProcessor invoking poweroff after %d seconds with no buttons\n", getDelayNoClickPoweroffSetting());
            // poweroff.. but send MQTT first..
            main_dispatchAsyncCommand(ASYNC_POWEROFF);
        }

    }
}

#endif //SINGLE_DELAY

/**
 Idea for the 2 buttons: (the side button is the 'select' button, which changes the "state" of the M5. Then the
 top button is the 'select' on the state. So if the state was "feed" then the top button would be feed.
 
 But since their are many options for the device, th
 */

//defaults (mode is incremented on waking)
char _smMessage[30];

//!toggle the showing of the SemanticMarker and the ZOOM version
void toggleSM_Zoomed()
{
    togglePreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE);
    //now redraw the semantic marker (zoomed or not zoomed)
    redrawSemanticMarker_displayModule(START_NEW);
}

//!invokes the semantic marker (which changes the display), this uses the getCurrentSMMode_mainModule()
void invokeCurrentSemanticMarker()
{
    
#ifdef USE_MQTT_NETWORKING
    //! Ths processJSONMessageMQTT( with cmd:smN } will trigger displaying the SemanticMarker for that page, etc.
    //!
    sprintf(_smMessage,"{'cmd':'%s'}",charSMMode_mainModule(getCurrentSMMode_mainModule()));
    //sends message internally in JSON format.. (same as if message came from MQTT via node-red,
    // which is where the SemanticMarkers are linked to .. the node-red backend (LinkAndSync)
    processJSONMessageMQTT(_smMessage, TOPIC_TO_SEND);
#else
    //display something
    redrawSemanticMarker_displayModule(START_NEW);
#endif
}

//!calls the resetFeed after we figure out the max
void resetFeedCount()
{
    resetFeedCount_mainModule();
}

//!process the current command (from some button combination) This is the LONG press on the Big button A
void performProcessCurrentMode()
{
#ifdef ESP_M5
    
    //! long press. This will "select" the mode we are in..
    //!  short press = always feed..
    
    //!send the async FEED. The logic there determines if BLE or MQTT
    //! dispatches a call to the command specified. This is run on the next loop()
    boolean justWakeup = isBlankScreen_displayModule();
    
    //!debug
    SerialCall.print("performProcessCurrentMode: blank:");
    SerialCall.println(justWakeup);

    //!whether screen is blank (so the button can be a wake, vs action..)
    if (!justWakeup)
    {
        SerialTemp.print("currentSMMode: ");
        SerialTemp.println(getCurrentSMMode_mainModule());
        
        int currentSMMode = getCurrentSMMode_mainModule();
        //! see if there is a model..
        ModelStateStruct *model = hasModelForSM_Mode(getCurrentSMMode_mainModule());
        if (model)
        {
            //print the model..

            //invoke the operation.. the selection (wrapping around)
            boolean modelChanged = invokeMenuState(model->modelKindEnum);
            SerialTemp.printf("modelChanged(%d)\n", modelChanged);
            //set based on modelChanged
            int startFlag = modelChanged?START_NEW:KEEP_SAME;
            //!redraws the Semantic Marker image..
           // redrawSemanticMarker_displayModule(KEEP_SAME);
            
            int currentSMModeNow = getCurrentSMMode_mainModule();
            
            //!basically some of the invocation changes the pages, so if it's different, invoke the current one..
            SerialTemp.printf("SMmode=%d, now=%d\n", currentSMMode,currentSMModeNow);
            if (currentSMMode != currentSMModeNow)
                invokeCurrentSemanticMarker();
            else
                redrawSemanticMarker_displayModule(startFlag);
            
            return;
        }
                                                 
        ///the mode is executable now..
        switch (getCurrentSMMode_mainModule())
        {
            case SM_home_simple:
                SerialCall.println("SM_home_simple");

                savePreferenceBoolean_mainModule(PREFERENCE_IS_MINIMAL_MENU_SETTING, true);

                //long press == tilt toggle..
                togglePreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE);
                invokeCurrentSemanticMarker();
         
                break;
            case SM_home_simple_1:
                SerialCall.println("SM_home_simple_1");

                togglePreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE);
#ifdef USE_BLE_CLIENT_NETWORKING
                //!optimized to not send the wrapper around the message
                // need to send B or b .. depending on result
                if (getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE))
                    sendCommandBLEClient_13orLess("B");
                else
                    sendCommandBLEClient_13orLess("b");

#endif
                invokeCurrentSemanticMarker();
            
                //long press == buzz toggle..
                break;
            case SM_home_simple_2:
            {
                SerialCall.println("SM_home_simple_2");
                
                //!calls the resetFeed after we figure out the max
                resetFeedCount();
                
                //togglePreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE);
                invokeCurrentSemanticMarker();
            }
                //long press == buzz toggle..
                break;
            case SM_home_simple_3:   //trying to mimick the HELP ...
                //new for the simple homepages..
                SerialCall.println("SM_home_simple_3");
                //togglePreferenceBoolean_mainModule(PREFERENCE_IS_MINIMAL_MENU_SETTING);
                savePreferenceBoolean_mainModule(PREFERENCE_IS_MINIMAL_MENU_SETTING, false);
                
                //also change to the next item, maybe anothertime popup a message stating what's happening..
                incrementSMMode_mainModule();
                invokeCurrentSemanticMarker();
                
                break;
                //!home page, long pr3ess will be MQTT feed message
            case SM_smart_clicker_homepage:
#ifdef USE_MQTT_NETWORKING
                ///feed always  (done after the code below..)
//                main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
                sendMessageMQTT((char*)"#FEED");
#endif
                break;

                //!guest page. Long press sends our WIFI credentials over BLE to the server (like a feeder)
            case SM_guest_page:
            {
                //!grab the WIFI credentials in JSON format
                char *credentials = main_JSONStringForWIFICredentials();
                //!send over BLE
                //!send an async call with a string parameter. This will set store the value and then async call the command (passing the parameter)
                //!These are the ASYNC_CALL_PARAMETERS_MAX
                //!since cannot send over 13 characters lets just sent the swap of network
                main_dispatchAsyncCommandWithString(ASYNC_CALL_BLE_CLIENT_PARAMETER, credentials);
            }
                break;
            case SM_guest_feed: //guest feed
                //NO-OP in zoom mode, the SemanticMarker shows the guest feed ..
                // or make it just feed?
#ifdef USE_MQTT_NETWORKING
                //!create a device name feeding message
                //!send this as a message, not this tacks on the # in front and other after
                sendMessageString_mainModule((char*)"FEED");
#endif
                break;
                //guest feed and wifi ssid both change the credentials..
            case SM_WIFI_ssid:
            {
                //NOTE: this might be where we toggle credentials?? TODO
                //found other one..
                char *credentials = main_nextJSONWIFICredential();
#ifdef USE_MQTT_NETWORKING

                //!These are the ASYNC_CALL_PARAMETERS_MAX
                //!NO: just change our credentials ...
                //send to 
                //main_dispatchAsyncCommandWithString(ASYNC_CALL_BLE_CLIENT_PARAMETER, credentials);
                processJSONMessageMQTT(credentials, TOPIC_TO_SEND);
#endif
                //redraw window..
                invokeCurrentSemanticMarker();

            }
                break;
                
            case SM_pair_dev: //pair device
            {
                char *pairDevice = getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING);
#ifdef USE_MQTT_NETWORKING
                //!create a device name feeding message
                sprintf(_smMessage,"FEED {'deviceName':'%s'}",pairDevice);
                //!send this as a message, not this tacks on the # in front and other after
                sendMessageString_mainModule(_smMessage);
#endif
            }
                break;
            case SM_status: //status
            {
                //send this onto the DOCFollow message
                const char* sm = currentSemanticMarkerAddress_displayModule();
                sendSemanticMarkerDocFollow_mainModule(sm);
#ifdef USE_MQTT_NETWORKING
                //!also send a #STATUS
                sendMessageMQTT((char*)"#STATUS");
#endif

            }
                break;
            case SM_help: //help
            {
                //!and print any preferences to show
                printPreferenceValues_mainModule();
#ifdef CHANGED_TO_JUST_MIN_MAX
                //new logic: long press big buton ButA
                //  if in zoomed -- then go to unzoomed (which shows semantic marker)
                //      in unzoomed, also show all the marker (so Min = false)
                togglePreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE);
                //now what will it be..
                boolean isZoomed = getPreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE);
                
                SerialCall.print("Setting MINIMAL_MENU: ");
                SerialCall.println(isZoomed?"ZOOMED":"UNZOOMED");
                
                // if zoomed in (then use the short or min setting
                savePreferenceBoolean_mainModule(PREFERENCE_IS_MINIMAL_MENU_SETTING, isZoomed?true:false);
#endif
                togglePreferenceBoolean_mainModule(PREFERENCE_IS_MINIMAL_MENU_SETTING);

                //a!lso change to the next item, maybe anothertime popup a message stating what's happening..
                incrementSMMode_mainModule();
                invokeCurrentSemanticMarker();
            }
                break;
            case SM_ap_mode: //AP MODE
                              //send the async FEED
                              //! dispatches a call to the command specified. This is run on the next loop()
                main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_CREDENTIALS);
                break;
                
            case SM_doc_follow:
            {
                //The long press will send A semanticMarker of some kind onto the network..
                // The getSM is already being shown..
                
                //This is where options occur.. what to send out?
                /// FOR NOW lets sed the dynamic status message..
                //send this onto the DOCFollow message
                //const char* sm = currentSemanticMarkerAddress_displayModule();
                char *statusURL = main_currentStatusURL();

                SerialTemp.print("SM_doc_follow: ");
                SerialTemp.println(statusURL);
                sendSemanticMarkerDocFollow_mainModule(statusURL);
                               
            }
                break;
            case SM_reboot:
            {
                SerialMin.println("rebooting..");
                main_dispatchAsyncCommand(ASYNC_REBOOT);

                return;
            }
                break;
            default:
                //these will be just SemanticMarkers to scan..
                break;
        } //switch
    }
    else
    {
        SerialCall.print("currentSMMode: ");
        SerialCall.println(getCurrentSMMode_mainModule());
    }
    //now redraw the semantic marker (zoomed or not zoomed)
    redrawSemanticMarker_displayModule(KEEP_SAME);

#endif
}

//!invokes a buzzer sound on the M5
void buzzerM5_buttonModule()
{
#ifdef ESP_M5_NOT_NOW
    //!buzz the M5 (super complicated...)
    M5.Beep.beep();

#endif
    
#ifdef ESP_M5_TRY

    //!try the RED LED TOO  .. turns it on but not off...
    ledcSetup(_channelREDLED, _freq, _resolution_bits);
    ledcAttachPin(_LEDPin, _channelREDLED);
    
    // ledcWrite(_channelBuzzer, 128);
    ledcWrite(_channelBuzzer, 300);
    delay(200);
    ledcWrite(_channelBuzzer, 0);
    
    // ledcWrite(_channelBuzzer, 128);
//    ledcWrite(_channelREDLED, 300);
//    delay(200);
    ledcWrite(_channelREDLED, 0);
    
//    ledcSetup(_channelREDLED, 0, 0);
//    ledcAttachPin(_LEDPin, _channelREDLED);
    
#endif
}
//!defines M5 info: https://docs.rs-online.com/e4eb/A700000008182827.pdf

//!long press on buttonA (top button)
void buttonA_LongPress()
{
    SerialCall.println("long press.. processMode");
    
    //!long press
    performProcessCurrentMode();
    //! buzz
    buzzerM5_buttonModule();

}


//!short press on buttonA (top button)
void buttonA_ShortPress()
{
    // always feed..
    SerialCall.println("buttonA_ShortPress");
        
    //! see if there is a model..
    ModelStateStruct *model = hasModelForSM_Mode(getCurrentSMMode_mainModule());
    if (model)
    {
        boolean justWakeup = isBlankScreen_displayModule();
        if (justWakeup)
        {
            //!redraws the Semantic Marker image..
            redrawSemanticMarker_displayModule(START_NEW);
        }
        else
        {
            //increment the selection (wrapping around)
            incrementMenuState(model->modelKindEnum);
            
            //!redraws the Semantic Marker image..
            redrawSemanticMarker_displayModule(KEEP_SAME);
        }
        
        //!no more action .. like no feed
        return;
    }

    ///feed always  (done after the code below..)
    main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
    
#ifdef USE_MQTT_NETWORKING
    //!unfortunately, the incrementFeedCount() is AFTER the redrawSemanticMarker..
    /// This sets the semantic marker  .. which is current SM
    sprintf(_smMessage,"{'cmd':'%s'}",charSMMode_mainModule(getCurrentSMMode_mainModule()));
    //   processJSONMessageMQTT(charSMMode_mainModule(getCurrentSMMode_mainModule()), TOPIC_TO_SEND);
    processJSONMessageMQTT(_smMessage, TOPIC_TO_SEND);
    
#endif
    //!redraws the Semantic Marker image..
    redrawSemanticMarker_displayModule(KEEP_SAME);
    
    //shows that we clicked the button... (vs waiting for another to show it..)
    showText_displayModule("FEED..");
}

//!big button on front of M5StickC Plus
void checkButtonA_ButtonProcessing()
{
    boolean buttonTouched = true;
#ifdef ESP_M5
    //was 1000
    if (M5.BtnA.wasReleasefor(500))
    {
        buttonA_LongPress();
    }
    else if (M5.BtnA.wasReleased())
    {
        buttonA_ShortPress();
    }
    else
    {
        buttonTouched = false;
    }
    //if a button was touched, update the delay since no touch..
    if (buttonTouched)
    {
        refreshDelayButtonTouched();
    }

    
    
#endif //ESP_M5
}

//!the long press of the side button
void buttonB_LongPress()
{
#ifdef ESP_M5
    
    SerialCall.println("B.long press.. toggleZoom or incrementColor");

    switch (getCurrentSMMode_mainModule())
    {
        case SM_home_simple:
        case SM_home_simple_1:
        case SM_home_simple_2:
        case SM_ap_mode: //AP MODE

            //! for now, the color is incremented in the displayModule
            incrementScreenColor_displayModule();
            //now redraw the semantic marker (zoomed or not zoomed)
            redrawSemanticMarker_displayModule(KEEP_SAME);
            break;
        case SM_home_simple_3:
            // send a STATUS message
#ifdef USE_MQTT_NETWORKING
            //!also send a #STATUS
            sendMessageMQTT((char*)"#STATUS");
#endif
            break;
        default:
            //long press
            //!toggle the showing of the SemanticMarker and the ZOOM version
            toggleSM_Zoomed();
            break;
    }
    
   
    buzzerM5_buttonModule();

#endif
}

//!the short press of the side button
void buttonB_ShortPress()
{
    //!and print any preferences to show
    //printPreferenceValues_mainModule();
    SerialCall.println("buttonB shortPress");

#ifdef ESP_M5
    //!side button.. cycles through choices..
    //!NOW: for 
    
    boolean justWakeup = isBlankScreen_displayModule();
    //!whether screen is blank (so the button can be a wake, vs action..)
    if (!justWakeup)
    {
#define NEW_VERSION
#ifdef NEW_VERSION
        if (getCurrentSMMode_mainModule() > SM_HOME_SIMPLE_LAST)
        {
            // set back to the SM_HOME_SIMPLE_LAST = table of contents..
            setCurrentSMMode_mainModule(SM_HOME_SIMPLE_LAST);
        }
        else
#endif
            incrementSMMode_mainModule();
        SerialCall.print("current mode:");
        SerialCall.println(getCurrentSMMode_mainModule());
        //not used right now.. min and max are same ..

    }
    
    //NOTE: starting at SM5, these are groups of 2 (on/off) and the drawing of the semantic marker
    invokeCurrentSemanticMarker();
    
#endif
}
//!small button on right side of M5StickC Plus
void checkButtonB_ButtonProcessing()
{
    //SerialTemp.println("checkButtonB_ButtonProcessing");
    boolean buttonTouched = true;
#ifdef ESP_M5
    
    //was 1000
    if (M5.BtnB.wasReleasefor(500))
    {
        SerialCall.println("buttonB_LongPress");

        buttonB_LongPress();
    }
    //side button.. cycles through choices..
    else if (M5.BtnB.wasReleased())
    {
        SerialCall.println("buttonB_ShortPress");

        buttonB_ShortPress();
    }
    else
    {

        buttonTouched = false;
    }
    //if a button was touched, update the delay since no touch..
    if (buttonTouched)
    {
        SerialLots.println(" .. buttonB .. calling refresh");
        refreshDelayButtonTouched();
    }
#endif // ESP_M5

}



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

//!looks at M5.IMU sensor to see if changed since last time..
//!Return "true" on motion (over sensitivity) and false otherwise
bool checkMotion_ButtonProcessing()
{
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

        M5.IMU.getAccelData(&_IMU.accX,&_IMU.accY,&_IMU.accZ);
      //  M5.IMU.getAhrsData(&_IMU.pitch,&_IMU.roll,&_IMU.yaw);

        //debug print
        //SerialTemp.printf("%5.2f  %5.2f  %5.2f   \n\r", _IMU.accX, _IMU.accY, _IMU.accZ);
//        SerialTemp.printf("%5.2f  %5.2f  %5.2f   \n\r", _IMU.gyroX, _IMU.gyroY, _IMU.gyroZ);
//        SerialTemp.printf("%5.2f  %5.2f  %5.2f   \n\r", _IMU.pitch, _IMU.roll, _IMU.yaw);

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
void setup_ButtonProcessing()
{
    setCurrentSMMode_mainModule(0);
    
#ifdef ESP_M5
    //!setup the plugin PIR (if there..)
    pinMode(PIR_PIN,INPUT_PULLUP);
    
    //M5.begin();  already called..
    //Init IMU.  初始化IMU

    M5.Imu.Init(); 
    M5.Axp.begin();
    _InactivityTimeOut = Elapsed3mins;
    
    //!https://docs.m5stack.com/en/api/stickc/pwm
    //!
//    ledcSetup(_channelBuzzer, _freq, _resolution_bits);
//    ledcAttachPin(_buzzerPin, _channelBuzzer);
  //  M5.Beep.begin();


#endif
}

boolean _firstLoopProcessing = true;
//!the loop for buttonProcessing (extension of ButtonModule)
void loop_ButtonProcessing()
{
    //not sure if MQTT or other running .. but seems like a try
    if (_firstLoopProcessing)
    {
        invokeCurrentSemanticMarker();
        _firstLoopProcessing = false;
    }

#ifdef ESP_M5
    M5.update(); //Read the press state of the key. ONLY call once per loop or the status of B button is lost
#endif
  
   

#ifdef LATER
    if ( (millis() - PrevTriggerTime) > InactivityTimeOut) {
        M5.Axp.PowerOff(); // shutoff after no activity
    }
#endif
   
    //try..
    
    // check for tilt etc..
    if (checkMotion_ButtonProcessing())
    {
        SerialTemp.println("motion detected");
        sendMessageString_mainModule((char*)"TiltDetected");

        //This (in mainModule) will look if connected so sends over BLE, otherwise MQTT (if connected)
        main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
    }
    
    // check for PIR etc..
    if (checkPIR_ButtonProcessing())
    {
        SerialTemp.println("PIR detected");
        sendMessageString_mainModule((char*)"PIRDetected");

        //This (in mainModule) will look if connected so sends over BLE, otherwise MQTT (if connected)
        main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
    }
    
    //checkBothPressed_ButtonProcessing();
    checkButtonA_ButtonProcessing();
    checkButtonB_ButtonProcessing();
    
    //see if the time is up..
    checkDelaySinceButtonTouched();
}

#endif //USE_BUTTON_MODULE
