/**
 Use ATOM Socket to monitor the socket power, press the middle button of ATOM
 to switch the socket power on and off. Connect to the AP hotspot of the device
 and access 192.168.4.1 to wirelessly control the socket power and view the power
 voltage, current and power information.
 使用ATOM插座监控插座电源，按ATOM的中间按钮即可打开和关闭插座电源。
 连接到设备的AP热点，接入192.168.4.1，无线控制插座电源，查看电源电压、电流和电源信息。
 */

 
#include "../../Defines.h"
#ifdef ATOM_SOCKET_MODULE
#include "ATOM_SocketModule.h"

#include "AtomSocket.h"


#ifdef USE_FAST_LED
#include "../ATOM_LED_Module/M5Display.h"
#endif

//! info from the socket power
int _Voltage = 0;
int _ActivePower = 0;
float _Current = 0;

//! create the ATOMSOCKET  connection to the power socket
ATOMSOCKET ATOM;

#define RXD   22
#define RELAY 23

//! interface to the hardware
HardwareSerial AtomSerial(2);

//! LOOP code.. refactored so it's also called when a buttonPress message arrives
void loopCode_ATOM_SocketModule();

//!big button on front of M5StickC Plus
void checkButtonB_ATOM_SocketModule();


//! not beware of linking another ATOM these names will overload..
boolean _shortPress_ATOM_SocketModule = false;
boolean _longPress_ATOM_SocketModule = false;
boolean _longLongPress_ATOM_SocketModule = false;

//! whether the device is ON  (default of)
//! called "RelayFlag" is the example
boolean _isOn_ATOM_SocketModule;
//! turn on/off the socket
void toggle_ATOM_SocketModule();

//#define KEY_UNIT_SENSOR_CLASS in defines.h
#ifdef  KEY_UNIT_SENSOR_CLASS

#include "../SensorClass/SensorClassType.h"
#include "../SensorClass/KeyUnitSensorClass.h"

KeyUnitSensorClass *_KeyUnitSensorClass_ATOMSocketModule = NULL;


//a pointer to a callback function that takes (char*) and returns void
void M5AtomSocketCallback(char *parameter, boolean flag)
{
    SerialDebug.printf("M5AtomSocket.sensorCallbackSignature(%s,%d)\n", parameter, flag);
    
    sendMessageString_mainModule((char*)"M5AtomSocket.KEY Pressed ");
    
#ifdef USE_FAST_LED
    fillpix(L_YELLOW);
#endif
    toggle_ATOM_SocketModule();

}
#endif

//! turn on/off the socket
void set_ATOM_SocketModule(boolean flag)
{
    //!store globally
    _isOn_ATOM_SocketModule = flag;
    SerialDebug.printf("set_ATOM_SocketModule: %s\n",flag?"ON":"OFF");
    
    if (_isOn_ATOM_SocketModule)
    {
        ATOM.SetPowerOn();
#ifdef USE_FAST_LED
        fillpix(L_GREEN);
#endif
        //! 1.1.24 syntax:   socket=on  or socket=off (somewhere in the string..) .. this version, only string avaialble..
        storePreference_mainModule(PREFERENCE_ATOMS_SETTING, "socket=on");
    }
    else
    {
        ATOM.SetPowerOff();
#ifdef USE_FAST_LED
        fillpix(L_YELLOW);
        //or turn light off??
#endif
        //! 1.1.24 syntax:   socket=on  or socket=off (somewhere in the string..)
        storePreference_mainModule(PREFERENCE_ATOMS_SETTING, "socket=off");
    }
    
    //!send status after power on/off change..
    sendStatusMQTT_mainModule();
    
}

//! turn on/off the socket
void toggle_ATOM_SocketModule()
{
    set_ATOM_SocketModule(!_isOn_ATOM_SocketModule);
}


//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 2.27.23 support setName == "socket"
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
//! 1.12.24 AtomSocketGlobalOnOff  to turn on/off global onoff
void messageSetVal_ATOM_SocketModule(char *setName, char* valValue, boolean deviceNameSpecified)
{
    //! see if global on/off is supported..
    //! So if globalOnOff definitely perform action. If deviceNameSpecified - perform action so OR of them
    boolean globalOnOff = getPreferenceBoolean_mainModule(PREFERENCE_ATOM_SOCKET_GLOBAL_ONOFF_SETTING);

    boolean isTrue =  isTrueString_mainModule(valValue);
    if (strcmp(setName,"socket")==0)
    {
        SerialDebug.printf("messageSetVal_ATOM_SocketModule(%s - global =%d, dev=%d)\n", valValue, globalOnOff, deviceNameSpecified);
        if (globalOnOff || deviceNameSpecified)
        {
            set_ATOM_SocketModule(isTrue);
        }
    }
}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the send
void messageSend_ATOM_SocketModule(char *sendValue)
{
    SerialDebug.printf("messageSend_ATOM_SocketModule(%s)\n", sendValue);
    if (strcasecmp(sendValue, "togglesocket")==0)
    {
        toggle_ATOM_SocketModule();
    }
}

//! init any globals
//! 1.5.24
void initGlobals_ATOM_SocketModule()
{
}


//!THIS IS the setup() and loop() but using the "component" name, eg AudioModule()
//!This will perform preference initializtion as well
//! called from the setup()
//! Pass in the method to call on a loud (over a threshhold. The parameter for value will be sent
//void setup_AudioModule(void (*loudCallback)(int));
void setup_ATOM_SocketModule()
{
    initGlobals_ATOM_SocketModule();
    
    //! 5.3.25 register our PIN use
    registerPinUse_mainModule(RXD, "RXD", "ATOM_SocketModule", false);
    registerPinUse_mainModule(RELAY, "RELAY", "ATOM_SocketModule", false);
    
    //! 1.1.24 syntax:   socket=on  or socket=off (somewhere in the string..)
    char *preference = getPreference_mainModule(PREFERENCE_ATOMS_SETTING);
    SerialDebug.printf("ATOM Pref = %s\n", preference);
    if (containsSubstring(preference,(char*)"socket=on"))
        _isOn_ATOM_SocketModule = true;
    else
        _isOn_ATOM_SocketModule = false;
    
    //! TODO..
#ifdef ESP_M5_ATOM_S3
    M5.begin();
#else
    SerialDebug.println("setup_ATOM_SocketModule.M5.begin");

    M5.begin(true, false, true);
#endif
    
    
#ifdef USE_FAST_LED
    //!NOTE: this could probably be done by ESP_IOT.ino .. but for now keep here (and in the other ATOM code..)
    setup_M5Display();
    fillpix(L_GREEN);
#endif
    
    //! ijnit the ATOM Socket
    ATOM.Init(AtomSerial, RELAY, RXD);
    
    //!default whatever was the last mode Persistently..
    set_ATOM_SocketModule(_isOn_ATOM_SocketModule);

#ifdef KEY_UNIT_SENSOR_CLASS
    //SensorStruct* sensor = getSensor_mainModule("KeyUnitInstanceM5AtomSocket");
    //!NOTE: error without (char*) -- *warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings]
    SensorStruct* sensor = getSensor_mainModule((char*)"BuzzerSensorClass");

    //! if the Buzzer isn't on .. then we are good to go
    //! try this 5.14.25  .. But this is probablly wrong .. is should be just the KeyUnitSensorClass .. and maybe more generic..
    if (!sensor)
    {
        //! create instance..
        _KeyUnitSensorClass_ATOMSocketModule = new KeyUnitSensorClass((char*)"KeyUnitInstanceM5AtomSocket");
        //! specify the callback
        _KeyUnitSensorClass_ATOMSocketModule->registerCallback(&M5AtomSocketCallback);
        //! call the setup
        _KeyUnitSensorClass_ATOMSocketModule->setup();
    }
    else
    {
        SerialDebug.println("BuzzerSensorClass already specified in sensors");
    }
#endif
}

//! called for the loop() of this plugin
void loop_ATOM_SocketModule()
{
    //! TODO..
    //!
#ifdef CALL_LATER
    ATOM.SerialReadLoop();
    if (ATOM.SerialRead == 1)
    {
        //SerialDebug.println("*** ATOM.SerialRead == 1");

        _Voltage     = ATOM.GetVol();
        _Current     = ATOM.GetCurrent();
        _ActivePower = ATOM.GetActivePower();
        
        //! use STATUS, and Long Press to get info..
    }
#endif
    
    M5.update();
    //    checkButtonA_ATOM_SocketModule();
    checkButtonB_ATOM_SocketModule();
    
    //! do loop code
    loopCode_ATOM_SocketModule();
    
#ifdef KEY_UNIT_SENSOR_CLASS
    if (_KeyUnitSensorClass_ATOMSocketModule)
        _KeyUnitSensorClass_ATOMSocketModule->loop();
#endif
}

//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void buttonA_ShortPress_ATOM_SocketModule()
{
    _shortPress_ATOM_SocketModule = false;
    _longPress_ATOM_SocketModule = false;
    _longLongPress_ATOM_SocketModule = false;
    
    _shortPress_ATOM_SocketModule = true;
    loopCode_ATOM_SocketModule();
}

//!long press on buttonA (top button)
void buttonA_LongPress_ATOM_SocketModule()
{
    _shortPress_ATOM_SocketModule = false;
    _longPress_ATOM_SocketModule = false;
    _longLongPress_ATOM_SocketModule = false;
    
    _longPress_ATOM_SocketModule = true;
    loopCode_ATOM_SocketModule();
}


//!big button on front of M5StickC Plus
void checkButtonB_ATOM_SocketModule()
{
    _shortPress_ATOM_SocketModule = false;
    _longPress_ATOM_SocketModule = false;
    _longLongPress_ATOM_SocketModule = false;
    
#ifdef ESP_M5
   
#ifdef ESP_M5_ATOM_S3
#else
    //!NOTE: ths issue is the timer is interruped by the scanner.. so make long-long very long..
    //was 1000  (from 500)
    if (M5.BtnB.wasReleasefor(3500))
    {
        //        buttonA_longPress_ATOM_SocketModule();
        SerialDebug.println("ATOM_SocketModule **** LONG LONG PRESS ***");
        _longLongPress_ATOM_SocketModule = true;
    }
    else if (M5.BtnB.wasReleasefor(1000))
    {
        //        buttonA_longPress_ATOM_SocketModule();
        SerialDebug.println("ATOM_SocketModule **** LONG PRESS ***");
        _longPress_ATOM_SocketModule = true;
    }
    else if (M5.BtnB.wasReleased())
    {
        //        buttonA_shortPress_ATOM_SocketModule();
        SerialDebug.println("ATOM_SocketModule **** SHORT PRESS ***");
        _shortPress_ATOM_SocketModule = true;
    }
#endif
#endif //ESP_M5
}

//! LOOP code.. refactored so it's also called when a buttonPress message arrives
void loopCode_ATOM_SocketModule()
{
    
    
    //#define read_serial_monitor
#ifdef read_serial_monitor
    //???  reads from the serial monitor it seems
    // Actually .. there is a way to send a message form the serial monitor..
    boolean serialData = false;
    while (Serial.available()) {
        serialData = true;
        int ch = Serial.read();
        //        Serial2.write(ch);
        SerialDebug.write(ch);
    }
    if (serialData)
        SerialDebug.println();
    
#endif
 
    //! 1.11.24 one idea, use the PIR_SM and let this send that message
    //! That way a long press could turn on all the devices, for example..
    //! OR: just have a long press toggle but for everyone..
    //! TODO..
    if (_longPress_ATOM_SocketModule)
    {
#ifdef USE_FAST_LED
       // fillpix(L_YELLOW);
#endif
        ATOM.SerialReadLoop();
        if (ATOM.SerialRead == 1)
        {
            SerialDebug.println("*** ATOM.SerialRead == 1");
            _Voltage     = ATOM.GetVol();
            _Current     = ATOM.GetCurrent();
            _ActivePower = ATOM.GetActivePower();
            
            //! use STATUS, and Long Press to get info..
        }
        else
        {
            _Voltage     = 0;
            _Current     = 0;
            _ActivePower = 0;
        }
        
        SerialDebug.printf("Voltage=%d, Current=%d, ActivePower=%f\n", _Voltage, _Current,_ActivePower);
        //TODO .. send this message out on MQTT
        //TODO:  add a callback for requesting "STATUS"
    }
    else if (_longLongPress_ATOM_SocketModule)
    {
#ifdef USE_FAST_LED
        fillpix(L_RED);
#endif
        SerialDebug.printf("CLEAN CREDENTIALS and reboot to AP mode\n");
        
        //! dispatches a call to the command specified. This is run on the next loop()
        main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_CREDENTIALS);
    }
    else if (_shortPress_ATOM_SocketModule)
    {
        toggle_ATOM_SocketModule();
    }
}

//!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
char * currentStatusURL_ATOM_SocketModule()
{
    //! 8.4.25 add k=PWR
    if (_isOn_ATOM_SocketModule)
        return (char*)"&socket=on&k=PWR";
    else
        return (char*)"&socket=off&k=PWR";
}

//!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * currentStatusJSON_ATOM_SocketModule()
{
    if (_isOn_ATOM_SocketModule)
        return (char*)"'socket':'on'";
    else
        return (char*)"'socket':'off'";
}

#else


//!THIS IS the setup() and loop() but using the "component" name, eg AudioModule()
//!This will perform preference initializtion as well
//! called from the setup()
//! Pass in the method to call on a loud (over a threshhold. The parameter for value will be sent
//void setup_AudioModule(void (*loudCallback)(int));
void setup_ATOM_SocketModule() {}

//! called for the loop() of this plugin
void loop_ATOM_SocketModule(){}


//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 12.27.23 support setName == "socket"
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void messageSetVal_ATOM_SocketModule(char *setName, char* valValue, boolean deviceNameSpecified){}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 5.21.22 note tthe SEND and CMD are both routed through "send"
void messageSend_ATOM_SocketModule(char *sendValue) {}

//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void buttonA_ShortPress_ATOM_SocketModule(){}
//!long press on buttonA (top button)
void buttonA_LongPress_ATOM_SocketModule(){}

//!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * currentStatusURL_ATOM_SocketModule() { return (char*)"";}

//!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * currentStatusJSON_ATOM_SocketModule(){return (char*)"";}
#endif // ATOM_SOCKET_MODULE
