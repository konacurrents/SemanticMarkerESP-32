//! \link ButtonProcessing
//
//  ButtonProcessing.c
//  M5Stick
//
//  Created by Scott Moody on 3/9/22.
//

#include "ButtonProcessing.h"
#ifdef USE_BUTTON_MODULE

#define USE_TIMER_DELAY_CLASS
//! 3.29.25 RaiiiinIeeeeR Beer movie
#ifdef  USE_TIMER_DELAY_CLASS
TimerDelayClass* _timerDelayClass_ButtonModule = new TimerDelayClass(1.0);
void startDelay_ButtonProcessing(int seconds)
{
    _timerDelayClass_ButtonModule->startDelay((float)seconds);
}
boolean delayFinished_ButtonProcessing()
{
    return _timerDelayClass_ButtonModule->delayFinished();
}
void stopDelay_ButtonProcessing()
{
    _timerDelayClass_ButtonModule->stopDelay();
}
#else
//! not USE_TIMER_DELAY_CLASS
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
#endif //SINGLE_DELAY
#endif //USE_TIMER_DELAY_CLASS

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

//#endif //SINGLE_DELAY

//!calls the resetFeed after we figure out the max
void resetFeedCount()
{
    resetFeedCount_mainModule();
}

//   ******************  START ESP_M5
#ifdef ESP_M5
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
    
    //! 8.16.25 MQTT
    //! Ths processJSONMessageMQTT( with cmd:smN } will trigger displaying the SemanticMarker for that page, etc.
    //!
    sprintf(_smMessage,"{'cmd':'%s'}",charSMMode_mainModule(getCurrentSMMode_mainModule()));
    //sends message internally in JSON format.. (same as if message came from MQTT via node-red,
    // which is where the SemanticMarkers are linked to .. the node-red backend (LinkAndSync)
    processJSONMessageMQTT(_smMessage, TOPIC_TO_SEND);

}

//!process the current command (from some button combination) This is the LONG press on the Big button A
void performProcessCurrentMode()
{
    
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
                //! 8.16.25 BLE CLIENT                //!optimized to not send the wrapper around the message
                // need to send B or b .. depending on result
                if (getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE))
                    sendCommandBLEClient_13orLess("B");
                else
                    sendCommandBLEClient_13orLess("b");

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
                //! 8.16.25 MQTT
                ///feed always  (done after the code below..)
//                main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
                sendMessageMQTT((char*)"#FEED");
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
                //! 8.16.25 MQTT
                //!create a device name feeding message
                //!send this as a message, not this tacks on the # in front and other after
                sendMessageString_mainModule((char*)"FEED");
                break;
                //guest feed and wifi ssid both change the credentials..
            case SM_WIFI_ssid:
            {
                //NOTE: this might be where we toggle credentials?? TODO
                //found other one..
                char *credentials = main_nextJSONWIFICredential();
                //! 8.16.25 MQTT

                //!These are the ASYNC_CALL_PARAMETERS_MAX
                //!NO: just change our credentials ...
                //send to 
                //main_dispatchAsyncCommandWithString(ASYNC_CALL_BLE_CLIENT_PARAMETER, credentials);
                processJSONMessageMQTT(credentials, TOPIC_TO_SEND);
                //redraw window..
                invokeCurrentSemanticMarker();

            }
                break;
                
            case SM_pair_dev: //pair device
            {
                char *pairDevice = getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING);
                //! 8.16.25 MQTT
                //!create a device name feeding message
                sprintf(_smMessage,"FEED {'deviceName':'%s'}",pairDevice);
                //!send this as a message, not this tacks on the # in front and other after
                sendMessageString_mainModule(_smMessage);
            }
                break;
            case SM_status: //status
            {
                //send this onto the DOCFollow message
                const char* sm = currentSemanticMarkerAddress_displayModule();
                sendSemanticMarkerDocFollow_mainModule(sm);
                //! 8.16.25 MQTT
                //!also send a #STATUS
                sendMessageMQTT((char*)"#STATUS");

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
                char *statusURL = main_currentStatusURL(true);

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

}


//!defines M5 info: https://docs.rs-online.com/e4eb/A700000008182827.pdf

//!long press on buttonA (top button)
void buttonA_LongPress()
{
    SerialTemp.println("long press.. processMode");
    
    //!long press
    performProcessCurrentMode();

}


//!short press on buttonA (top button)
void buttonA_ShortPress()
{
    // always feed..
    SerialTemp.println("buttonA_ShortPress");
        
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
    
    //! 8.16.25 MQTT
    //!unfortunately, the incrementFeedCount() is AFTER the redrawSemanticMarker..
    /// This sets the semantic marker  .. which is current SM
    sprintf(_smMessage,"{'cmd':'%s'}",charSMMode_mainModule(getCurrentSMMode_mainModule()));
    //   processJSONMessageMQTT(charSMMode_mainModule(getCurrentSMMode_mainModule()), TOPIC_TO_SEND);
    processJSONMessageMQTT(_smMessage, TOPIC_TO_SEND);
    
    //!redraws the Semantic Marker image..
    redrawSemanticMarker_displayModule(KEEP_SAME);
    
    //shows that we clicked the button... (vs waiting for another to show it..)
    showText_displayModule("FEED..");
}


//!the long press of the side button
void buttonB_LongPress()
{
    
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
            //! 8.16.25 MQTT
            //!also send a #STATUS
            sendMessageMQTT((char*)"#STATUS");
            break;
        default:
            //long press
            //!toggle the showing of the SemanticMarker and the ZOOM version
            toggleSM_Zoomed();
            break;
    }
    
   
}

//!the short press of the side button
void buttonB_ShortPress()
{
    //!and print any preferences to show
    //printPreferenceValues_mainModule();
    SerialCall.println("buttonB shortPress");

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
    
}
#endif  //ESP_M5
//   ******************  END ESP_M5

//!the setup for buttonProcessing (extension of ButtonModule)
//!  在 M5StickC Plus 启动或者复位后，即会开始执行setup()函数中的程序，该部分只会执行一次。
void setup_ButtonProcessing()
{
    setCurrentSMMode_mainModule(0);
 
}

boolean _firstLoopProcessing = true;
//!the loop for buttonProcessing (extension of ButtonModule)
void loop_ButtonProcessing()
{
#ifdef ESP_M5
    //not sure if MQTT or other running .. but seems like a try
    if (_firstLoopProcessing)
    {
        invokeCurrentSemanticMarker();
        _firstLoopProcessing = false;
    }
#endif //ESP_M5
    //see if the time is up..
//    checkDelaySinceButtonTouched();

}

#ifdef ESP_M5
#else
//!short press on buttonA (top button)
void buttonA_ShortPress() {}
//!long press on buttonA (top button)
void buttonA_LongPress() {}
//!the long press of the side button
void buttonB_LongPress() {}
//!the short press of the side button
void buttonB_ShortPress() {}
#endif //ESP_M5
#endif //USE_BUTTON_MODULE
