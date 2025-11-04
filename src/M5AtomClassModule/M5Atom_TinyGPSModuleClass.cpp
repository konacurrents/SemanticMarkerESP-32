//
//  M5Atom_TinyGPSModuleClass.cpp
//  
//
//  Created by Scott Moody on 7/17/25.
//

#include "M5Atom_TinyGPSModuleClass.h"

#include "../TinyGPS/MultipleSatellite.h"

//! 7.21.25 use the ATOM_SM processor for BLE feed
//#include "../ATOM_QRCode_Module/ATOM_SM.h"
#include "../TinyGPS/TinyGPS.h"

#ifdef USE_FAST_LED
#include "../ATOM_LED_Module/M5Display.h"
#endif

//! 7.20.25 60outside. 6 month since T
//! add time from @see https://github.com/PaulStoffregen/Time/blob/master/examples/TimeGPS/TimeGPS.ino
//#include "../TimeModule/TimeLib.h"
#include <TimeLib.h>


//! TODO.. have only 1 of these included in build, but change the callback
//! That requires that ATOM be a class, OR there is a single Sensor but different callback..
//#define KEY_UNIT_SENSOR_CLASS in defines.h
#ifdef  KEY_UNIT_SENSOR_CLASS

#include "../SensorClass/SensorClassType.h"
#include "../SensorClass/KeyUnitSensorClass.h"


//! keyUnitSensorClass object for the BUTTON
KeyUnitSensorClass *_KeyUnitSensorClass_ATOMTinyGPSModule;


//a pointer to a callback function that takes (char*) and returns void
void M5AtomCallback_TinyGPS(char *parameter, boolean flag)
{
    SerialDebug.printf("M5AtomCallback_TinyGPS.sensorCallbackSignature(%s,%d)\n", parameter, flag);
    
    //   sendMessageString_mainModule((char*)"M5Atom.KEY Pressed ");
    
    //!send status after power on/off change..
    sendStatusMQTT_mainModule();
  
}
#endif //KEY_UNIT_SENSOR_CLASS

//! @see https://arduiniana.org/libraries/tinygpsplus/
static const int TinyGPS_RXPin = 22, TinyGPS_TXPin = 21;


//Be suitable for Unit GPS v1.1  https://docs.m5stack.com/en/unit/Unit-GPS%20v1.1
//Be suitable for Unit GPS SMA   https://docs.m5stack.com/en/unit/Unit-GPS%20SMA
static const uint32_t GPSBaud = 115200;

//Be suitable for Unit GPS  https://docs.m5stack.com/en/unit/gps
//static const uint32_t GPSBaud = 9600;

// Create an instance of MultipleSatellite, assuming we use the Serial1 serial port
//MultipleSatellite gps(Serial1, GPSBaud, SERIAL_8N1, TinyGPS_RXPin, TinyGPS_TXPin);

MultipleSatellite gps(Serial1, GPSBaud, SERIAL_8N1, TinyGPS_RXPin, TinyGPS_TXPin);
MultipleSatellite *gps2;//(Serial1, GPSBaud, SERIAL_8N1, TinyGPS_RXPin, TinyGPS_TXPin);

M5Atom_TinyGPSModuleClass::M5Atom_TinyGPSModuleClass(char *config) : M5AtomClassType(config)
{
    SerialDebug.printf("M5Atom_TinyGPSModuleClass init %s\n", config);
    
}


//! Data members of class
//! Generic config info .. specific the the sensor type

//!starts the PTStepper
void M5Atom_TinyGPSModuleClass::start_M5AtomClassType()
{
}

//!setup the PTStepper
void M5Atom_TinyGPSModuleClass::setup_M5AtomClassType()
{
#ifdef USE_FAST_LED
    SerialDebug.println("M5Atom_TinyGPSModuleClass.M5.begin");
    
    //  M5.begin(true,false,true);
    //!NOTE: this could probably be done by ESP_IOT.ino .. but for now keep here (and in the other ATOM code..)
    setup_M5Display();
    //  fillpix(L_GREEN);
    fillpix(L_BLUE);
    
#endif
    
    //! 5.3.25 register our PIN use
    registerPinUse_mainModule(TinyGPS_RXPin,  "TinyGPS_RXPin", "M5Atom_TinyGPSModuleClass", false);
    registerPinUse_mainModule(TinyGPS_TXPin,  "TinyGPS_TXPin", "M5Atom_TinyGPSModuleClass", false);

    
    gps.begin();
    gps.setSystemBootMode(BOOT_FACTORY_START);
    SerialDebug.println("M5Atom_TinyGPSModuleClass.setup");
    SerialDebug.println("A simple demonstration of TinyGPSPlus with an attached GPS module");
    SerialDebug.print("Testing TinyGPSPlus library v. ");
    SerialDebug.println(TinyGPSPlus::libraryVersion());
    SerialDebug.println("by Mikal Hart");
    SerialDebug.println();
//    String version = gps.getGNSSVersion();
//    SerialDebug.printf("GNSS SW=%s\r\n", version.c_str());
    delay(1000);
    // Set satellite mode
    gps.setSatelliteMode(SATELLITE_MODE_GPS);
  
    

    
#ifdef KEY_UNIT_SENSOR_CLASS
    _KeyUnitSensorClass_ATOMTinyGPSModule = new KeyUnitSensorClass((char*)"KeyUnitSensorClass_ATOMTinyGPSModule");
    //! specify the callback
    _KeyUnitSensorClass_ATOMTinyGPSModule->registerCallback(&M5AtomCallback_TinyGPS);
    //! call the setup
    _KeyUnitSensorClass_ATOMTinyGPSModule->setup();
    SerialDebug.printf("_Key = %d\n", _KeyUnitSensorClass_ATOMTinyGPSModule);
#endif
    
    SerialDebug.println("done setup");
}


//! stops motor
void M5Atom_TinyGPSModuleClass::stop_M5AtomClassType()
{
    
}

//!setup the PTStepper
void M5Atom_TinyGPSModuleClass::loop_M5AtomClassType()
{
    //SerialDebug.println("M5Atom_TinyGPSModuleClass::loop_M5AtomClassType");
    // Update data
    boolean getEachLoop = false;
    if (getEachLoop)
    {
        gps.updateGPS();
        displayInfo_TinyGPS();
    }

	 //! NOTE: THink of a way to provide this information..  STATUS ...
     //!
    
#ifdef KEY_UNIT_SENSOR_CLASS
    //SerialDebug.printf("_Key = %d\n", _KeyUnitSensorClass_ATOMQRCodeModule);
    
    _KeyUnitSensorClass_ATOMTinyGPSModule->loop();
#endif
}

//! called to update the _lat, _lon, _alt values
void M5Atom_TinyGPSModuleClass::updateLocation_TinyGPS()
{
    SerialDebug.println("M5Atom_TinyGPSModuleClass");
    gps.updateGPS();
    if (gps.location.isUpdated())
    {
        this->_lat = gps.location.lat();
        this->_lon = gps.location.lng();
        this->_alt = gps.altitude.meters();
        
        //! print status
        M5Atom_TinyGPSModuleClass::displayInfo_TinyGPS();
        
        //! set the time in the TimeLib.h
        setTime(gps.time.hour(), gps.time.minute(), gps.time.second(), gps.date.day(), gps.date.month(), gps.date.year());
        
        //! printout
        int timestamp = getTimeStamp_mainModule();

        //! TODO: use that RTC Server to get time...
        //SerialDebug.printf("T = %ld\n", timestamp);
        
#ifdef USE_SPIFF_MODULE_off_for_now
        //! add SPIFF output
        char spiffBuffer[100];
        //sprintf(spiffBuffer, "T=%d, (%d,%d,%d)", timestamp, this->_lat, this->_lon, this->_alt);
        sprintf(spiffBuffer,"{'time':'%d','lat':'%0f','lon':'%0f','alt':'%0f'},", timestamp, this->_lat, this->_lon, this->_alt);
        SerialDebug.printf("%s\n",spiffBuffer);
        //! now send to SPIFF append
        println_SPIFFModule(spiffBuffer);
#endif
    }
    else
    {
        SerialDebug.print(F("INVALID\n"));
    }
}

void M5Atom_TinyGPSModuleClass::displayInfo_TinyGPS()
{
    
    if (gps.location.isUpdated())
    {
        SerialDebug.print(F("Location: "));
        SerialDebug.printf("satellites:%d\n", gps.satellites.value());
        SerialDebug.print(gps.location.lat(), 6);
        SerialDebug.print(F(","));
        SerialDebug.print(gps.location.lng(), 6);
        SerialDebug.print(F("\n"));
        
        TinyGPSAltitude altitude = gps.altitude;

        SerialDebug.print(altitude.meters(), 6);
        SerialDebug.print(F("\n"));
        SerialDebug.println();
        
        _lat = gps.location.lat();
        _lon = gps.location.lng();
        _alt = gps.altitude.meters();
   
    }
    else
    {
        SerialDebug.print(F("INVALID\n"));
    }

    if (gps.date.isUpdated())
    {
        SerialDebug.print(F("  Date/Time: "));

        SerialDebug.print(gps.date.month());
        SerialDebug.print(F("/"));
        SerialDebug.print(gps.date.day());
        SerialDebug.print(F("/"));
        SerialDebug.print(gps.date.year());
        SerialDebug.println();

        SerialDebug.printf("DATE = %d\n", gps.date.value());

    }
    else
    {
       // SerialDebug.print(F("INVALID"));
    }

    if (gps.time.isUpdated())
    {
        SerialDebug.print(F(" "));

        if (gps.time.hour() < 10) SerialDebug.print(F("0"));
        SerialDebug.print(gps.time.hour());
        SerialDebug.print(F(":"));
        if (gps.time.minute() < 10) SerialDebug.print(F("0"));
        SerialDebug.print(gps.time.minute());
        SerialDebug.print(F(":"));
        if (gps.time.second() < 10) SerialDebug.print(F("0"));
        SerialDebug.print(gps.time.second());
        SerialDebug.print(F("."));
        if (gps.time.centisecond() < 10) SerialDebug.print(F("0"));
        SerialDebug.print(gps.time.centisecond());
        SerialDebug.println();

        SerialDebug.printf("TIME = %d\n", gps.time.value());
    }
    else
    {
        //SerialDebug.print(F("INVALID"));
    }

    //delay(100);
}


//! ATOM specific

//! these are from the ATOM

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 12.27.23 support setName == "QRCode"
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void M5Atom_TinyGPSModuleClass::messageSetVal_M5AtomClassType(char *setName, char* valValue, boolean deviceNameSpecified)
{
    //messageSetVal_M5TinyGPSModule(setName, valValue, deviceNameSpecified);
    
}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the  send -
void M5Atom_TinyGPSModuleClass::messageSend_M5AtomClassType(char *sendValue, boolean deviceNameSpecified)
{
    
}

//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void M5Atom_TinyGPSModuleClass::buttonA_ShortPress_M5AtomClassType()
{
    SerialDebug.println("M5Atom_TinyGPSModuleClass::buttonA_ShortPress_M5AtomClassType");
    //! feed
    SerialDebug.printf("Feed BLE\n");
    // send the _lastSemanticMarker again ...
    //!send this as a DOCFOLLOW message
    //sendSemanticMarkerDocFollow_mainModule(_lastSemanticMarker);
    //        strcpy(_lastSemanticMarker,"https://iDogWatch.com/bot/feed/test/test");
    
    ///feed always  (done after the code below..)
    main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
}
//!long press on buttonA (top button)
void M5Atom_TinyGPSModuleClass::buttonA_LongPress_M5AtomClassType()
{
    
    SerialDebug.println("M5Atom_TinyGPSModuleClass::buttonA_LongPress_M5AtomClassType");
    //
    SerialDebug.printf("CLEAN CREDENTIALS and reboot to AP mode\n");
    
    //! dispatches a call to the command specified. This is run on the next loop()
    main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_CREDENTIALS);
}


//!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * M5Atom_TinyGPSModuleClass::currentStatusURL_M5AtomClassType()
{
    updateLocation_TinyGPS();
    //! printout
    //! add timestamp
    int timestamp = getTimeStamp_mainModule();
    
    //! 8.4.25 add k=GPS
    sprintf(_locationBuffer,"time=%d&lat=%0f&lon=%0f&alt=%0f&k=GPS",time, _lat, _lon, _alt);
    return (char*)_locationBuffer;
}

//!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * M5Atom_TinyGPSModuleClass::currentStatusJSON_M5AtomClassType()
{
    updateLocation_TinyGPS();
    sprintf(_locationBuffer,"'lat':'%0f','lon':'%0f','alt':'%0f'", _lat, _lon, _alt);
    return (char*)_locationBuffer;
}
