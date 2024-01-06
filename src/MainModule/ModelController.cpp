#include "../../Defines.h"
#include "ModelController.h"

//*** For the MenuSelection MODEL
//! a pseudo Class

#define MenusModel_Help 0
#define MenusModel_Settings 1
#define MenusModel_Pairing 2
#define MenusModel_APMode 3
#define MenusModel_Timer 4
#define MenusModel_Status 5
#define MenusModel_Advanced 6
#define MenusModel_WIFIShare 7
#define MenusModel_GuestFeed 8
#define MenusModel_GuestPage 9
#define MenusModel_DocFollow 10
#define MenusModel_Max 11

//!retrieves the state model for the modelKind
ModelStateStruct *getModel(ModelKindEnum modelKind);

//!storage of mapping
int _menuToSMMode[MenusModel_Max];
//!init the mapping
void initMenuToSMMode()
{
    for (int i=0; i< MenusModel_Max; i++)
    {
        switch (i)
        {
            case MenusModel_Pairing:
                _menuToSMMode[i] = SM_pair_dev;
                break;
            case MenusModel_APMode:
                _menuToSMMode[i] = SM_ap_mode;
                break;
            case MenusModel_Timer:
                _menuToSMMode[i] = SM_timer;
                break;
            case MenusModel_Advanced:
                _menuToSMMode[i] = SM_smart_clicker_homepage;
                break;
            case MenusModel_Status:
                _menuToSMMode[i] = SM_status;
                break;
            case MenusModel_WIFIShare:
                _menuToSMMode[i] = SM_WIFI_ssid;
                break;
            case MenusModel_GuestFeed:
                _menuToSMMode[i] = SM_guest_feed;
                break;
            case MenusModel_GuestPage:
                _menuToSMMode[i] = SM_guest_page;
                break;
            case MenusModel_Settings:
                _menuToSMMode[i] = SM_reboot;
                break;
            case MenusModel_DocFollow:
                _menuToSMMode[i] = SM_doc_follow;
                break;
            case MenusModel_Help:
                _menuToSMMode[i] = SM_help;
                break;
            default:
                SerialMin.println(" *** invalid menu item");
                break;
        }
    }
}
//! 
//!create state variables for the ModelKindEnum entries
ModelStateStruct _modelStateStructs[ModelKindEnumMax];
char _timerBuffer[10];

//!initialize the objects
void initModelStructs_ModelController()
{
    strcpy(_timerBuffer,"5");
    initMenuToSMMode();
    
    for (int i=0;i< ModelKindEnumMax;i++)
    {
        _modelStateStructs[i].modelKindEnum = (ModelKindEnum) i;
        _modelStateStructs[i].currentItem = 0;
        switch (i)
        {
            case pairedDeviceModel:
                _modelStateStructs[i].perfersBigText = false;
                _modelStateStructs[i].SM_Mode = SM_pair_dev;
                break;
            case rebootModel:
                _modelStateStructs[i].perfersBigText = false;
                _modelStateStructs[i].SM_Mode = SM_reboot;
                
                //STATE info..
                //! for the next page state 0..n
                _modelStateStructs[i].currentPageState = 0;
                // the max before goes back to 0, use SM_LAST from MainModule.h
                _modelStateStructs[i].maxPageState =  MAX_SM_EXPANDED_MODES;
                _modelStateStructs[i].currentStreamNum = 0;
                _modelStateStructs[i].maxStream = 3;
                _modelStateStructs[i].waveName = (char*)"KikahaPhotos"; // for now..  TODO .. explain this..
                break;
            case menusModel:
                _modelStateStructs[i].perfersBigText = false;
                _modelStateStructs[i].SM_Mode = SM_home_simple_3;
                break;
            case timerModel:
                //!initialize the random:
                //!@see https://www.arduino.cc/reference/en/language/functions/random-numbers/random/
                randomSeed(101010);
                
                _modelStateStructs[i].perfersBigText = false;
                _modelStateStructs[i].SM_Mode = SM_timer;
                //! true if still waiting for delay to finish
                _modelStateStructs[i].delayRunning = false;
                //! length of delay
                //!get from in EPROM
                _modelStateStructs[i].delaySeconds = getPreferenceInt_mainModule(PREFERENCE_TIMER_INT_SETTING);
                _modelStateStructs[i].delaySecondsMax = getPreferenceInt_mainModule(PREFERENCE_TIMER_MAX_INT_SETTING);
                _modelStateStructs[i].currentCounterSeconds = _modelStateStructs[i].delaySeconds;
                SerialCall.printf("initTimer: sec = %d\n", _modelStateStructs[i].currentCounterSeconds);
#if defined(ESP_M5_CAMERA) || defined(ESP_32)
                //! feed local instead of MQTT or BLE..
                //! if camera then feed local == take a picture..
                _modelStateStructs[i].feedLocalOnly = true;
#else
                //! feed local instead of MQTT or BLE..
                _modelStateStructs[i].feedLocalOnly = false;
#endif //ESP_32 or CAMERA
                break;
        }
    }
}

//!! TIMER Remote control set delay seconds
//!MQTT:  set: timerdelay, val:seconds
void setTimerDelaySeconds_mainModule(int delaySeconds)
{
    SerialDebug.printf("setTimerDelaySeconds_mainModule(%d)\n", delaySeconds);

    ModelStateStruct *timerModelStruct = getModel(timerModel);
    timerModelStruct->delaySeconds = delaySeconds;
    timerModelStruct->currentCounterSeconds = delaySeconds;
    //!save in EPROM
    savePreferenceInt_mainModule(PREFERENCE_TIMER_INT_SETTING, delaySeconds);

}

//!! TIMER Remote control set delay seconds
//!MQTT:  set: timerdelay,Max val:seconds
void setTimerDelaySecondsMax_mainModule(int delaySeconds)
{
    SerialDebug.printf("setTimerDelaySecondsMax_mainModule(%d)\n", delaySeconds);
    
    ModelStateStruct *timerModelStruct = getModel(timerModel);
    timerModelStruct->delaySecondsMax = delaySeconds;
    timerModelStruct->currentCounterSeconds = delaySeconds;
    //!save in EPROM
    savePreferenceInt_mainModule(PREFERENCE_TIMER_MAX_INT_SETTING, delaySeconds);
    
}

//! set the next random time .. since called from a couple places
//! will set the model->delayStartMillis  == currentCounterSeconds
void setNextRandomTime()
{
    ModelStateStruct *timerModelStruct = getModel(timerModel);

    // calculate a random
    int nextRandomTimeSeconds = random(timerModelStruct->delaySeconds, timerModelStruct->delaySecondsMax);
    timerModelStruct->delayStartMillis = millis();   // when started
    timerModelStruct->currentCounterSeconds = nextRandomTimeSeconds;
    
    //!the value returned from random .. the diff is taken off this number
    //! but this number doesn't change except next random call
    timerModelStruct->counterLoopAbsoluteSeconds = nextRandomTimeSeconds;
    
    SerialCall.printf("nextRandomTimeSeconds(%d,%d)     = %d\n", timerModelStruct->delaySeconds, timerModelStruct->delaySecondsMax, nextRandomTimeSeconds);
}

//!! TIMER Remote control start
//!MQTT:  set: starttimer, val: true/false  (true == start timer, false = stop timer)
void startStopTimer_mainModule(boolean startTimer)
{
    ModelStateStruct *timerModelStruct = getModel(timerModel);
    timerModelStruct->delayRunning = startTimer;
    
    if (startTimer)
    {
        setNextRandomTime();
    }
    SerialDebug.printf("startStopTimer_mainModule(%d, %d)\n", startTimer, timerModelStruct->currentCounterSeconds);

}

//!print it out..
void printDeviceState(ModelStateStruct *deviceState)
{
//#define PRINT_THIS
#ifdef PRINT_THIS
    ModelKindEnum modelKind = deviceState->modelKindEnum;
    switch (modelKind)
    {
        case pairedDeviceModel:
        {
            
            SerialTemp.print("deviceState->pairedDeviceStateEnum: ");
            switch (deviceState->pairedDeviceStateEnum)
            {
                    //paired to a device, but BLE NOT connected right now
                case pairedButNotConnectedEnum:
                    SerialLots.println("pairedButNotConnectedEnum");
                    break;
                    //paired to a device, and BLE connected right now
                case pairedAndConnectedEnum:
                    SerialLots.println("pairedAndConnectedEnum");
                    break;
                    //not paired (but could be paired as it's a named device])
                case pairableAndConnectedEnum:
                    SerialLots.println("pairableAndConnectedEnum");
                    break;
                    //!factory default but not connecte to anything..
                case notConnectedEnum:
                    SerialLots.println("notConnectedEnum");
                    break;
            }
            break;
        }
        case rebootModel:
        {
            SerialTemp.println("deviceState = rebootModel ");
        }
            break;
        case menusModel:
        {
            //!MENUS
            //! WIFI Feed
            //! Advanced
            //! Status
            //! WIFI sharing
            //! APMode
            //! Guest Feed
            //! Settings
            //! Pairing
            //! DocFollow
            //! help
            //! HomePage
            
            SerialTemp.println("deviceState = menusModel ");
        }
            break;
        case timerModel:
        {
            SerialTemp.println("deviceState = timerModel ");
        }
            break;
    }
    SerialTemp.printf("deviceState->maxItems (%d) \n", deviceState->maxItems );
    SerialTemp.printf("deviceState->currentItem (%d) \n", deviceState->currentItem );
#endif
}


//!retrieves the state model for the modelKind
ModelStateStruct *getModel(ModelKindEnum modelKind)
{
    ModelStateStruct *model = &_modelStateStructs[modelKind];
    SerialCall.printf("getModel(%d) = %d\n", modelKind, model);
    printDeviceState(model);

    return model;
}

//!retrieves the state model for the SM_Mode (SM_0 .. SM_LAST) null if none
ModelStateStruct *hasModelForSM_Mode(int SM_Mode)
{
    for (int i=0;i< ModelKindEnumMax;i++)
    {
        if (_modelStateStructs[i].SM_Mode == SM_Mode)
        {
            SerialCall.printf("hasModelForSM_Mode(%d) == yes\n", SM_Mode);
            
            printDeviceState(&_modelStateStructs[i]);
            return &_modelStateStructs[i];
        }
    }
    SerialCall.printf("hasModelForSM_Mode(%d) == NO\n", SM_Mode);

    return NULL;
}

//!increments the device states deviceState (wrapping around)
void incrementMenuState(ModelKindEnum modelKind)
{
    ModelStateStruct *deviceState = getModel(modelKind);
    
    
    int item = deviceState->currentItem;
    int max = deviceState->maxItems;
    item = (item + 1) % max;
    deviceState->currentItem = item;
    
    SerialCall.printf("incrementMenuState(%d) = %d \n", modelKind, item);;
    
}

//!restarts all the menu states to the first one .. useful for getting a clean start. This doesn't care if the menu is being shown
void restartAllMenuStates_ModelController()
{
    SerialDebug.println("restartAllMenuStates");
    for (int i=0;i< ModelKindEnumMax;i++)
    {
        ModelKindEnum modelKind = (ModelKindEnum)i;
        ModelStateStruct *deviceState = getModel(modelKind);
        deviceState->currentItem = 0;
    }
}

//!storage..
//char _smMessage_ModelController[30];

//!send the command to the stepper..  not a BLE command
void invokeFeedLocally()
{
    processClientCommandChar_mainModule('s');
}

//!performs the BLE feed
void invokeFeed_ModelController()
{
    char _smMessage_ModelController[30];
    
    ///feed always  (done after the code below..)
    main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
    
#ifdef USE_MQTT_NETWORKING
    //!unfortunately, the incrementFeedCount() is AFTER the redrawSemanticMarker..
    /// This sets the semantic marker  .. which is current SM
    sprintf(_smMessage_ModelController,"{'cmd':'%s'}",charSMMode_mainModule(getCurrentSMMode_mainModule()));
    //   processJSONMessageMQTT(charSMMode_mainModule(getCurrentSMMode_mainModule()), TOPIC_TO_SEND);
    processJSONMessageMQTT(_smMessage_ModelController, TOPIC_TO_SEND);
    
#endif
}

//! just unpair .. don't skip
//!performs the unpairing
void invokeUnpairNoNameBASE_ModelController()
{
    savePreference_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING, (char*)"");
    savePreference_mainModule(PREFERENCE_PAIRED_DEVICE_ADDRESS_SETTING, (char*)"");
    
#ifdef NO_MORE_PREFERENCE_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING
    savePreferenceBoolean_mainModule(PREFERENCE_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING, false);
#endif
    //!also unset the main storage..
    setConnectedBLEDevice_mainModule((char *)"",false);
    
}
//! just unpair .. don't skip
//!performs the unpairing
void invokeUnpairNoName_ModelController()
{
    //!refactored base
    invokeUnpairNoNameBASE_ModelController();
    
#ifdef USE_BLE_CLIENT_NETWORKING
    disconnect_BLEClientNetworking();
#endif
}

//!performs the unpairing
void invokeUnpair_ModelController(char *nameOrAddress)
{
    //!refactored base
    invokeUnpairNoNameBASE_ModelController();
    
#ifdef USE_BLE_CLIENT_NETWORKING
    skipNameOrAddress_BLEClientNetworking(nameOrAddress);
    disconnect_BLEClientNetworking();
#endif
    
}
//!performs the pairing..  to whatever is currently connected, this means a message could make that happen
//!for a device (ESP-32) with no user interface.
void invokePair_ModelController()
{
    char *deviceName = connectedBLEDeviceName_mainModule();
    char *deviceNameAddress = connectedBLEDeviceNameAddress_mainModule();
    boolean isGEN3 = connectedBLEDeviceIsGEN3_mainModule();
    
    SerialLots.printf("invokePair(%s, gen3=%d)\n", deviceName, isGEN3);

    //TODO...
    
    //!this is where it needs to know if GEN3. If so, then if their is a paired name .. If GEN3, don't overreight the device name
    //!in case it was set by the user..
    if (!isGEN3)
        savePreference_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING, deviceName);
    
    //! always set the ADDRESS
    savePreference_mainModule(PREFERENCE_PAIRED_DEVICE_ADDRESS_SETTING, deviceNameAddress);
    
    //!This means the name
#ifdef NO_MORE_PREFERENCE_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING
    savePreferenceBoolean_mainModule(PREFERENCE_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING, true);
#endif
}

//!performs skip
void invokeSkip_ModelController(char *nameOrAddress)
{
#ifdef USE_BLE_CLIENT_NETWORKING

    skipNameOrAddress_BLEClientNetworking(nameOrAddress);

    //TODO: look to see if the Address can be used to not connect right away to same one...
    disconnect_BLEClientNetworking();
#endif
}

//! toggles the GEN3 setting
void invokeToggleGen3_ModelController()
{
    //!change GEN3 preference
    togglePreferenceBoolean_mainModule(PREFERENCE_ONLY_GEN3_CONNECT_SETTING);

    //!if connected and not a GEN3 but want only GEN3 .. then disconnect, otherwise keep connected
    if (!connectedBLEDeviceIsGEN3_mainModule() && getPreferenceBoolean_mainModule(PREFERENCE_ONLY_GEN3_CONNECT_SETTING))
    {
        SerialTemp.println(" *** GEN3 only but connected to a non GEN3 .. so unpair **");
        //!also unpair..
        invokeUnpair_ModelController(getPairedDevice_mainModule());
    }
}


//!updates the model for the menu state, this sets max etc
void updateMenuState(ModelKindEnum modelKind)
{
    ModelStateStruct *deviceState = getModel(modelKind);
    SerialCall.printf("updateMenuState(%d) \n", modelKind);
    
    switch (modelKind)
    {
        case pairedDeviceModel:
        {
            //! must save since many preferences are called...
            char pairedDeviceString[50];
            char pairedDeviceAddressString[50];
            //* NEW PLAN..
            //!  the pairedName stored in EPROM can be an Address (eg. 03:34:23:33)
            //!
            //!show the BLE connected status at the bottom (G3 if gen3), WIFI, and AP if APmode
#ifdef USE_BLE_CLIENT_NETWORKING
            boolean isConnectedBLE = isConnectedBLEClient();
#else
            boolean isConnectedBLE = false;
#endif
            SerialLots.printf("\nPAIRED_DEVICE_MODEL\n");
            SerialLots.printf("isConnectedBLE(%d)\n", isConnectedBLE);
            
            boolean gen3Only = getPreferenceBoolean_mainModule(PREFERENCE_ONLY_GEN3_CONNECT_SETTING);
            if (gen3Only)
                SerialLots.printf("gen3Only(%d)\n", gen3Only);
            
            //! whether the gateway is on.. this isn't using the Gx just BLE or GE3
            boolean gatewayOn = getPreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE);
            if (gatewayOn)
                SerialLots.printf("gatewayOn(%d)\n", gatewayOn);

            //! name of paired device (if any) in EPROM
            char *pairedDeviceTemp = getPairedDevice_mainModule();
            strcpy(pairedDeviceString, pairedDeviceTemp);
            SerialLots.printf("pairedDevice(%s)\n", pairedDeviceString);
            
            char *pairedDeviceAddressTemp = getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_ADDRESS_SETTING);
            strcpy(pairedDeviceAddressString, pairedDeviceAddressTemp);

            SerialLots.printf("pairedDeviceAddress(%s)\n", pairedDeviceAddressString);
            
                      //!name of connected device, or "" if not specified, which means not pairable
            char *connectedBLEDeviceName = connectedBLEDeviceName_mainModule();
            SerialLots.printf("connectedBLEDeviceName(%s)\n", connectedBLEDeviceName);
            //!compare to what's connected ..
            char *connectedBLEDeviceNameAddress = connectedBLEDeviceNameAddress_mainModule();
            SerialLots.printf("connectedBLEDeviceNameAddress(%s)\n", connectedBLEDeviceNameAddress);

            //!is paired and connected (to that pair .. if BLECLient working right)
            boolean pairedAndConnected = false;
            boolean pairedButNotConnected = false;
            boolean pairableAndConnected = false;
            boolean unpaired = false;

            //!if connectedBLE then either paired or not-paired
            if (isConnectedBLE)
            {
                //!if the EPROM's pairedDevice/Address == what's connected, then we are paired and connected
                if (strcmp(pairedDeviceString,connectedBLEDeviceName)==0 ||
                    strcmp(pairedDeviceAddressString,connectedBLEDeviceNameAddress)==0)
                    //! is paired
                    pairedAndConnected = true;
                else
                    pairableAndConnected = true;
            }
            //! not connected, so paired if a valid paired name/address
            else if (strlen(pairedDeviceString) > 0 || strlen(pairedDeviceAddressString) > 0)
            {
                if (strlen(pairedDeviceString) > 0 && strcmp(pairedDeviceString,(char*)"NONE")==0)
                    unpaired = true;
                else if (strlen(pairedDeviceAddressString) > 0 && strcmp(pairedDeviceAddressString,(char*)"NONE")==0)
                    unpaired = true;
                else
                    //!if the EPROM's pairedDevice/Address is set then paired and NOT connected
                    pairedButNotConnected = true;
            }
            else
            {
                //!unpaired
                unpaired = true;
            }
            
            if (pairedButNotConnected)
                SerialLots.printf("pairedButNotConnected(%d)\n", pairedButNotConnected);
            if (pairedAndConnected)
                SerialLots.printf("pairedAndConnected(%d)\n", pairedAndConnected);
            if (pairableAndConnected)
                SerialLots.printf("pairableAndConnected(%d)\n", pairableAndConnected);
            if (unpaired)
                SerialLots.printf("unpaired(%d)\n", unpaired);
            
            if (pairedButNotConnected)
            {
                // scan, unpair, gen3
                deviceState->pairedDeviceStateEnum = pairedButNotConnectedEnum;
                deviceState->maxItems = 3;
            }
            else if (pairedAndConnected)
            {
                // feed, unPair, gen3
                deviceState->pairedDeviceStateEnum = pairedAndConnectedEnum;
                deviceState->maxItems = 3;
            }
            else if (pairableAndConnected)
            {
                // feed, skip, pair, gen3only
                deviceState->pairedDeviceStateEnum = pairableAndConnectedEnum;
                deviceState->maxItems = 4;
            }
            else
            {
                // scan, gen3Only
                deviceState->pairedDeviceStateEnum = notConnectedEnum;
                deviceState->maxItems = 2;
            }
           
            break;
        }
        case rebootModel:
            
            //reboot and poweroff, blank, color, semanticMarker, wifiTOO, WIFI Page, next wave page, next wave stream..
            // 9 = PTClicker, 10 == Uno, 11 = Tumbler
            deviceState->maxItems = 12;
            break;
            
        case menusModel:
            //!MENUS
            //! Pairing
            //! APMode
            //! Timer
            //! Advanced
            //! Status
            //! WIFI sharing
            //! Guest Feed
            //! Settings
            //! DocFollow
            //! help
            //! HomePage
            //! /WIFI Feed
            //! WIFI Share
            
            deviceState->maxItems = MenusModel_Max;
            break;
            //updateMenu
        case timerModel:
        {
            SerialCall.printf("updateMenuState timerRunning(%d)\n", deviceState->delayRunning);
            //! calculate the current second counter..
            if (deviceState->delayRunning)
            {
                //counter.., running
                deviceState->maxItems = 2;
                
                /*
                 Design:
                     currentCounterSeconds == the countdown timer
                        The problem is this code isn't guarenteed to be run every second. So we need
                 a way to know the different to subtract seconds from the counter seconds
                 
                 delayStartMillis == when the loop started in milis
                 currentTimeMillis == current time
                    diff == number of seconds since first time
                 
                 So either diff is subtracted from original (random number) -- or 
                    
                */
                
                long currentTimeMillis = millis();
                SerialCall.printf("currentTimeMillis    = %d\n", currentTimeMillis);
                SerialCall.printf("delayStartMillis     = %d\n", deviceState->delayStartMillis);

                //! the number of seconds since starting the loop
                float diffSec = (currentTimeMillis - deviceState->delayStartMillis) / 1000;
                deviceState->currentCounterSeconds = deviceState->counterLoopAbsoluteSeconds - diffSec;
                
                SerialCall.printf("currentCounterSeconds = %d\n", deviceState->currentCounterSeconds );
                
                //!if count <=0 then finished..
                if (deviceState->currentCounterSeconds <= 0)
                {
                    SerialCall.printf("delayFinished \n");
                    
                    //! 11.29.23 use a random
                    //! calculate a new random
                    setNextRandomTime();
                                                            
                    //!if local .. send a command directly to the device..
                    if (deviceState->feedLocalOnly)
                    {
                        //!send the command to the stepper..  not a BLE command
                        invokeFeedLocally();
                    }
                    else
                    {
                        //! for now only invoke the feed ... todo mroe things.. STATUS, WIFI FEED, DOCFOLLOW
                        invokeFeed_ModelController();
                    }
                }
            }
            else
            {
                // NOT running..
                //counter.., running, change time..
                //! added a max delay
                deviceState->maxItems = 4;
            }
        }
           
         
            break;
    }
    
    //!TODO: look at a scrolling idea if > 6 ...
    if (deviceState->currentItem  >= deviceState->maxItems)
    {
        // go to first one .. basically the menues changed since last time
        deviceState->currentItem = 0;
    }
    printDeviceState(deviceState);
}

//!returns the menu string for the deviceState's item number (use woudl go 0..maxItems -1
char *menuForState(ModelKindEnum modelKind, int item)
{
    SerialCall.printf("menuForState(%d,%d)\n", modelKind, item);
    ModelStateStruct *deviceState = getModel(modelKind);
    SerialCall.printf("connectedBLEDeviceName_mainModule(%s)\n", connectedBLEDeviceName_mainModule());
    SerialCall.printf("getFullBLEDeviceName_mainModule(%s)\n", getFullBLEDeviceName_mainModule());
    
    
    char *menu;
    switch (modelKind)
    {
        case pairedDeviceModel:
        {
            switch (deviceState->pairedDeviceStateEnum)
            {
                case pairedButNotConnectedEnum:
                    switch (item)
                    {
                        case 0:
                            menu = (char*)"scan..";
                            break;
                        case 1:
                            //unpair is to forget and look for others..
                            menu = (char*)"unpair";
                            break;
                        case 2:
                            if (getPreferenceBoolean_mainModule(PREFERENCE_ONLY_GEN3_CONNECT_SETTING))
                                menu = (char*)"Gen3";
                            else
                                menu = (char*)"AnyBLE";
                            break;
                        default:
                            SerialMin.printf("a.**** Invalid item: %d\n",item);
                            break;
                    }
                    break;
                case pairedAndConnectedEnum:
                    switch (item)
                    {
                        case 0:
                            menu = (char*)"feed";
                            break;
                        case 1:
                            menu = (char*)"unpair";
                            break;
                        case 2:
                            if (getPreferenceBoolean_mainModule(PREFERENCE_ONLY_GEN3_CONNECT_SETTING))
                                menu = (char*)"Gen3";
                            else
                                menu = (char*)"AnyBLE";
                            break;
                        default:
                            SerialMin.printf("b. **** Invalid item: %d\n",item);
                            break;
                    }
                    break;
                case pairableAndConnectedEnum:
                    switch (item)
                    {
                        case 0:
                            menu = (char*)"feed";
                            break;
                        case 1:
                            menu = (char*)"skip";
                            break;
                        case 2:
                            menu = (char*)"pair";
                            break;
                        case 3:
                            if (getPreferenceBoolean_mainModule(PREFERENCE_ONLY_GEN3_CONNECT_SETTING))
                                menu = (char*)"Gen3";
                            else
                                menu = (char*)"AnyBLE";
                            break;
                        default:
                            SerialMin.printf("c. **** Invalid item: %d\n",item);
                            break;
                    }
                    break;
                case notConnectedEnum:
                    switch (item)
                    {
                        case 0:
                            menu = (char*)"scan..";
                            break;
                        case 1:
                            if (getPreferenceBoolean_mainModule(PREFERENCE_ONLY_GEN3_CONNECT_SETTING))
                                menu = (char*)"Gen3";
                            else
                                menu = (char*)"AnyBLE";
                            break;
                        default:
                            SerialMin.printf("d. **** Invalid item: %d\n",item);
                            break;
                    }
                    break;
            }
        }
            break;
        case rebootModel:
        {
            //Max for >4 items is 10 characters..
            switch (item)
            {
                case 0:
                    menu = (char*)"reboot";
                    break;
                case 1:
                    menu = (char*)"poweroff";
                    break;
                case 2: // blankscreen
                        // poweroff.. but send MQTT first..
                    menu = (char*)"blankscrn";
                    break;
                    
                case 3: // Uno
                    menu = (char *)"Set Uno";
                    break;
                case 4: //FeedKindTumbler
                    menu = (char *)"Tumbler";
                    break;
                    
                case 5: // change color
                    menu = (char*)"colorscrn";
                    break;
                case 6: // SemanticMarker
                    menu = (char*)"SemMarker";
                    break;
                case 7: // Extra sends
                    if (getPreferenceBoolean_mainModule(PREFERENCE_SENDWIFI_WITH_BLE))
                        menu = (char*)"BLE+WIFI";
                    else
                        menu = (char*)"BLEOnly";
                    //  .. switch to WIFI too
                    break;
                case 8: // WIFI Change Page on M5 units..
                    menu = (char*)"CycleScrn";
                    //  .. switch to WIFI too
                    break;
                case 9: // WIFI chage wave page, evetually Wave-Page, Wave+Stream
                    menu = (char*)"Wave+Page";
                    //  .. switch to WIFI too
                    break;
                case 10: // WIFI chage wave page, evetually Wave-Page, Wave+Stream
                    menu = (char*)"Wave+Strm";
                    //  .. switch to WIFI too
                    break;
                case 11: // look for PTClicker
                    if (getDiscoverM5PTClicker())
                        menu = (char*)"PTClicker";
                    else
                        menu = (char*)"PTFeeder";
                    break;

                default:
                    SerialMin.printf("e. **** Invalid item: %d\n",item);
                    break;
            }
        }
            break;
            
        case menusModel:
        {
            //!MENUS
            //! Pairing
            //! APMode
            //! Timer
            //! Advanced
            //! Status
            //! WIFI sharing
            //! Guest Feed
            //! Settings
            //! DocFollow
            //! help
            //! HomePage
            //! /WIFI Feed
            //! WIFI Share

            
            switch (item)
            {
                case MenusModel_Pairing:
                    menu = (char*)"BLEPair";
                    break;
                case MenusModel_APMode:
                    menu = (char*)"APMode";
                    break;
                case MenusModel_WIFIShare:
                    menu = (char*)"WIFIShare";
                    break;
                case MenusModel_Timer:
                    menu = (char*)"Timer";
                    break;
                case MenusModel_Advanced:
                    menu = (char*)"Advanced";
                    break;
                case MenusModel_Status:
                    menu = (char*)"Status";
                    break;
                case MenusModel_GuestPage:
                    menu = (char*)"GuestPage";
                    break;
                case MenusModel_GuestFeed:
                    menu = (char*)"GuestFeed";
                    break;
                case MenusModel_Settings:
                    menu = (char*)"Settings";
                    break;
                case MenusModel_DocFollow:
                    menu = (char*)"DocFollow";
                    break;
                case MenusModel_Help:
                    menu = (char*)"Help";
                    break;

                default:
                    SerialMin.printf("f. **** Invalid item: %d\n",item);
                    break;
            }
        }
            break;
           
            //menuForState. Max == 2 if running (showing STOP , TIME)
            //!                       Max == 4 if not running (START, TIME, MAXTIME, DELAY)
        case timerModel:
        {
            switch (item)
            {
               
                case 0:
                    //!draw the state we are in..
                    if (deviceState->delayRunning)
                        menu = (char*)"Stop";
                    else
                        menu = (char*) "Start";
                    break;
                case 1:
                {
                    //!update menu with seconds countdown
                    if (deviceState->delayRunning)
                        sprintf(_timerBuffer,"%d", deviceState->currentCounterSeconds);
                    else
                        sprintf(_timerBuffer,"%d", deviceState->delaySeconds);
                    
                    menu = _timerBuffer;
                }
                    break;
                    
                    //! only if stopped... max == 4, other max == 2
                case 2:
                {
                    //!show max
                    sprintf(_timerBuffer,"%d", deviceState->delaySecondsMax);
                    menu = _timerBuffer;
                }
                    break;
                case 3:
                {
                   
                    menu = (char*)"Delay";
                }
                    break;
                default:
                    SerialMin.printf("g. **** Invalid item: %d\n",item);
                    break;
            }
        }
            break;
    }
    return menu;
}

//!invokes the menu state, return true if the model state has change enough to refreesh your View (such as new menu items)
//!NOTE: This is the LONG press on the M5 button (the BLUE item)
boolean invokeMenuState(ModelKindEnum modelKind)
{
    //!default is the view is the same
    boolean modelChanged = false;
    //!what model struct to use
    ModelStateStruct *deviceState = getModel(modelKind);
    SerialTemp.printf("invokeMenuState(%d, pair=%d) \n", modelKind, deviceState->pairedDeviceStateEnum);
    
    int item = deviceState->currentItem;
    switch (modelKind)
    {
            //! the pairdDeviceModel kind
        case pairedDeviceModel:
        {
            SerialCall.printf("pairedDeviceModel");

            switch (deviceState->pairedDeviceStateEnum)
            {
                    //paired to a device, but BLE NOT connected right now
                case pairedButNotConnectedEnum:
                    switch (item)
                    {
                        case 0:
                            //scan .. default
                            break;
                        case 1:
                            //unpair
                            invokeUnpair_ModelController(getPairedDevice_mainModule());
                            modelChanged = true;
                            break;
                        case 2:
                            //toggle..
                            invokeToggleGen3_ModelController();
                            modelChanged = true;
                            break;
                    }
                    break;
                    //paired to a device, and BLE connected right now
                case pairedAndConnectedEnum:
                    switch (item)
                    {
                        case 0:
                            //feed
                            //send BLE feed ... this help figure out which BLE is connected
                            invokeFeed_ModelController();
                            break;
                        case 1:
                            //unpair
                            invokeUnpair_ModelController(getPairedDevice_mainModule());
                            modelChanged = true;
                            break;
                        case 2:
                            //toggle..
                            invokeToggleGen3_ModelController();
                            modelChanged = true;
                            break;
                    }
                    break;
                    //not paired (but could be paired as it's a named device or address])
                case pairableAndConnectedEnum:
                    switch (item)
                    {
                        case 0:
                            //feed
                            invokeFeed_ModelController();
                            break;
                        case 1:
                            //skip
                            invokeSkip_ModelController(connectedBLEDeviceName_mainModule());
                            modelChanged = true;
                            break;
                        case 2:
                            // pair
                            invokePair_ModelController();
                            modelChanged = true;
                            break;
                        case 3:
                            //toggle..
                            invokeToggleGen3_ModelController();
                            modelChanged = true;
                            break;
                    }
                    break;
                    //!factory default but not connecte to anything..
                case notConnectedEnum:
                    switch (item)
                    {
                        case 0:
                            //scan
                            break;
                        case 1:
                            //toggle..
                            invokeToggleGen3_ModelController();
                            modelChanged = true;
                            break;
                    }
                    break;
            }
        }
            break;
            
        case rebootModel:
        {
            SerialCall.println("*** rebootModel ***");

            switch (item)
            {
                case 0: // reboot
                    main_dispatchAsyncCommand(ASYNC_REBOOT);
                    break;
                case 1: // poweroff
                        // poweroff.. but send MQTT first..
                    main_dispatchAsyncCommand(ASYNC_POWEROFF);
                    break;
                case 2: // blankscreen
                    main_dispatchAsyncCommand(ASYNC_BLANKSCREEN);
                    break;
                case 3: // Uno
#ifdef USE_BLE_CLIENT_NETWORKING
                    sendCommandBLEClient_13orLess("u");
#endif
                    break;
                case 4: //FeedKindTumbler
#ifdef USE_BLE_CLIENT_NETWORKING
                    sendCommandBLEClient_13orLess("L");
#endif
                    break;
                    
                case 5: //color
                        //! for now, the color is incremented in the displayModule
                    incrementScreenColor_displayModule();
                    break;
                case 6: // SemanticMarker
                    togglePreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE);
                    modelChanged = true;
                    break;
                case 7: // extra..
                    togglePreferenceBoolean_mainModule(PREFERENCE_SENDWIFI_WITH_BLE);
                    modelChanged = true;
                    break;
                case 8:
                    //send WIFI to all except this page.. but keep some state.
#ifdef USE_MQTT_NETWORKING
                    //eg: https://iDogWatch.com/bot/cmddevice/tao49@comcast.net/PASS/dev/sm1
                    //! BUT at the MQTT level not web page level
                    
                    //!9.30.22  IF SET .. send a feed but to all devices except ours and our pair (if any)
                    //! uses new wildcard syntax either  ! OUR NAME  [ & ! OUR_CONNECTED_NAME
                    // if (getTransientPreference_mainModule(TRANSIENT_PREFERENCE_SENDWIFI_WITH_BLE))
                {
                    char pubString[60];
                    //for now .. just send smN
                    sprintf(pubString,"{'dev':'!%s', 'cmd':'sm%0d'}",deviceName_mainModule(), deviceState->currentPageState);
                    publishMQTTMessageDefaultTopic(pubString);
                    //incremenet model, wrap around
                    deviceState->currentPageState = (deviceState->currentPageState +1 ) % deviceState->maxPageState;
                }
                    
#endif // USE_MQTT_NETWORKING
                    break;
                case 9:
                    // WIFI chage wave page
#ifdef USE_MQTT_NETWORKING
                    //eg: https://iDogWatch.com/bot/cmddevice/tao49@comcast.net/PASS/dev/sm1
                    //! BUT at the MQTT level not web page level
                    
                    //!9.30.22  IF SET .. send a feed but to all devices except ours and our pair (if any)
                    //! uses new wildcard syntax either  ! OUR NAME  [ & ! OUR_CONNECTED_NAME
                    // if (getTransientPreference_mainModule(TRANSIENT_PREFERENCE_SENDWIFI_WITH_BLE))
                {
                    char pubString[100];
                    //for now .. just send 'next' on a common wave (TODO: select wave we are in..)'
                    sprintf(pubString,(char*)"#followMe {AVM=https://SemanticMarker.org/bot/wave/%s?action=next}", deviceState->waveName);
                    //! this has the # so it is treated slightly different, but tacking on the device for example..
                    sendMessageMQTT(pubString);
                    
                }
                    
#endif // USE_MQTT_NETWORKING
                    break;
                case 10:
                    // WIFI change stream (1..3)
#ifdef USE_MQTT_NETWORKING
                    //eg: https://iDogWatch.com/bot/cmddevice/tao49@comcast.net/PASS/dev/sm1
                    //! BUT at the MQTT level not web page level
                    
                    //!9.30.22  IF SET .. send a feed but to all devices except ours and our pair (if any)
                    //! uses new wildcard syntax either  ! OUR NAME  [ & ! OUR_CONNECTED_NAME
                    // if (getTransientPreference_mainModule(TRANSIENT_PREFERENCE_SENDWIFI_WITH_BLE))
                {
                    char pubString[100];
                    //for now .. just send 'next' on a common wave (TODO: select wave we are in..)'
                    //! This just goes to the 0'th page in the stream..
                    sprintf(pubString,(char*)"#followMe {AVM=https://SemanticMarker.org/bot/wave/%s?stream=%0d&page=0}", deviceState->waveName, deviceState->currentStreamNum+1);
                    //! 1 based (not 0 based)
                    //publishMQTTMessageDefaultTopic(pubString);
                    //! this has the # so it is treated slightly different, but tacking on the device for example..
                    sendMessageMQTT(pubString);
                    
                    deviceState->currentStreamNum = (deviceState->currentStreamNum +1 ) % deviceState->maxStream;
                }
                    
#endif // USE_MQTT_NETWORKING
                    break;
                case 11:
                    //! set transient look for PTClicker
                    setDiscoverM5PTClicker(true);
                    break;
                    //! 11.4.22 (after Maggie Bluetic was fed with Pumpking Uno and GreyGoose
                

                default:
                    SerialMin.println("h, *** invalid item ***");
            }
        }
            break;
       
        case menusModel:
        {
            modelChanged = true;
            SerialCall.println("*** menusModel ***");
            //!use lookup
            int SMMode = _menuToSMMode[item];
            // invoke that page (note now those pages return here on ButB_longpress)
            setCurrentSMMode_mainModule(SMMode);
            
            //! Issue: #222 for #206, this sets the current mode to SM_doc_follow, but
            //! when at that page in the the current mode (which is now SM_doc_follow) won't let
            //! the page go somewhere else (except in this case we are the same page). Only but a
            //! physical button click.
            //! I THINK THE ANSWER: if current and next are the same an SM_doc_follow, then do the page change..
            
            //! if HELP then printout preferences to the debug window
            if (SMMode == SM_help)
                //! if HELP .. show the status
                //!print the preferences to SerialDebug
                printPreferenceValues_mainModule();
        }
            break;
        case timerModel:
        {
            //invokeMenuState
            SerialTemp.printf("*** timerModel (%d) *** %d\n", item, deviceState->delayRunning);
            
            /*
             When running 0 == STOP, and 1 == current time (and a long press will reset it..)
             When not running 0 == START, 1 = min, 2 = max , 3 = DELAY ??
             */
            switch (item)
            {
                case 0:
                    // stop the timer..
                    //start stop..
                    deviceState->delayRunning = !deviceState->delayRunning;
                    if (deviceState->delayRunning)
                    {
                        // just started..
                        setNextRandomTime();
                    }
                    break;
                case 1:
                    if (deviceState->delayRunning)
                    {
                        //! This will restart the timer..
                        setNextRandomTime();
                        break;
                    }
                    //otherwise fall through..
                case 2:
                {
                    //menu = (char*)"Bounds";  .. change timer  5 .. 30 .. 60 .. 120 .. 5
                    int currentDelay;
                    if (item == 1)
                        currentDelay = deviceState->delaySeconds;
                    else
                        currentDelay = deviceState->delaySecondsMax;
                    //!same login for each
                    if (currentDelay == 5)
                        currentDelay = 30;
                    else if (currentDelay == 30)
                        currentDelay = 60;
                    else if (currentDelay == 60)
                        currentDelay = 120;
                    else if (currentDelay == 120)
                        currentDelay = 600;
                    else
                        currentDelay = 5;
                    SerialTemp.printf(".. change delay = %d\n", currentDelay);
                    if (item == 1)
                    {
                        deviceState->delaySeconds = currentDelay;
                        //save in EPROM
                        savePreferenceInt_mainModule(PREFERENCE_TIMER_INT_SETTING, currentDelay);
                        
                        //! make sure the max is >= min
                        if (deviceState->delaySecondsMax < currentDelay)
                        {
                            deviceState->delaySecondsMax = currentDelay;
                            //save in EPROM
                            savePreferenceInt_mainModule(PREFERENCE_TIMER_MAX_INT_SETTING, currentDelay);
                        }
                    }
                    else
                    {
                        //! 11.29.23 add the random feed
                        if (currentDelay < deviceState->delaySeconds)
                            currentDelay = deviceState->delaySeconds;
                        deviceState->delaySecondsMax = currentDelay;
                        deviceState->currentCounterSeconds = deviceState->delaySeconds;
                        //save in EPROM
                        savePreferenceInt_mainModule(PREFERENCE_TIMER_MAX_INT_SETTING, currentDelay);
                    }
                    
                    //!now use those values to create a new random time..
                    //!not needed until the START above
                    //setNextRandomTime();
                }
                    break;
                case 3:
                    //! Delay .. no-opp
                    break;
                default:
                    SerialMin.println("i2.  *** invalid item ***");
                    
            }
        }
            break;
    }
    return modelChanged;
}

//!retrieves a semantic marker for the current selected item
char *getModelSemanticMarker(ModelKindEnum modelKind)
{
    //!default is the view is the same
    boolean modelChanged = false;
    //!what model struct to use
    ModelStateStruct *deviceState = getModel(modelKind);
    SerialLots.printf("invokeMenuState(%d, %d) \n", modelKind, deviceState->pairedDeviceStateEnum);
    
    char *semanticMarker = NULL;
    
#ifdef NOTHING_YET_SAVE_SOME_MEMORY
    int item = deviceState->currentItem;

    switch (modelKind)
    {
        case rebootModel:
        {
            switch (item)
            {
                case 0: // reboot
                    //REboot the device
                    //sprintf(_semanticMarkerString,"%s/%s/%s", "https://SemanticMarker.org/bot/reboot", _mqttUserString?_mqttUserString:"NULL", guestPassword?guestPassword:"NULL");
                    break;
                case 1: // poweroff
                        // poweroff.. but send MQTT first..
                    break;
                case 2: // blankscreen
                    break;
                case 3: //color
                        //! for now, the color is incremented in the displayModule
                    break;
                case 4: // SemanticMarker
                    break;
                case 5: // none
                    break;
                case 6: // next page .. actually there is one .. later
                    break;
                case 7: // next page .. actually there is one .. later
                    break;
            }
        }
    }
#endif
    return semanticMarker;
}
