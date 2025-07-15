
#include "../../Defines.h"

#ifdef USE_GPS_SENSOR_CLASS
#include "GPSSensorClass.h"

//#include <iostream>
#include <string.h>
#include <stdio.h>
/**
Connect UNIT GPS to port C, Use GPS Unit to get the coordinate data and time
of the current location. 将UNIT GPS
连接到C端口,使用GPS单元获取当前位置的坐标数据和时间。
*/

#include <TinyGPSPlus.h>

static const uint32_t GPSBaud = 9600;

// Creat The TinyGPS++ object.  创建GPS实例
TinyGPSPlus gps;

// The serial connection to the GPS device.  与GPS设备的串行连接
HardwareSerial ss(2);

GPSSensorClass::GPSSensorClass(char *config) : SensorClassType(config)
{
    
    printf("GPSSensorClass init %s\n", config);
}


// This custom version of delay() ensures that GPS objects work properly.
// 这个自定义版本的delay()确保gps对象正常工作。
static void smartDelay(unsigned long ms) {
    unsigned long start = millis();
    do {
        while (ss.available()) gps.encode(ss.read());
    } while (millis() - start < ms);
}

//!@see https://www.w3schools.com/cpp/cpp_class_methods.asp
void GPSSensorClass::loop()
{
    if (gps.location.isValid())
    {
        SerialDebug.printf("%f,%f,%f\n",  gps.location.lat(), gps.location.lng(), gps.altitude.isValid()?gps.altitude.meters():0.0);
    }
#ifdef LATER
    SerialDebug.printf("GPSSensorClass::loop()\n", gps.location.lat(), gps.location.lon());
    static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
    
    SerialDebug.printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
    SerialDebug.printInt(gps.hdop.value(), gps.hdop.isValid(), 5);
    SerialDebug.printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
    SerialDebug.printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
    SerialDebug.printInt(gps.location.age(), gps.location.isValid(), 5);
    SerialDebug.printDateTime(gps.date, gps.time);
    SerialDebug.printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
    SerialDebug.printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
    SerialDebug.printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
    SerialDebug.printStr(
             gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ",
             6);
    
    unsigned long distanceKmToLondon =
    (unsigned long)TinyGPSPlus::distanceBetween(
                                                gps.location.lat(), gps.location.lng(), LONDON_LAT, LONDON_LON) /
    1000;
    SerialDebug.printInt(distanceKmToLondon, gps.location.isValid(), 9);
    
    double courseToLondon = TinyGPSPlus::courseTo(
                                                  gps.location.lat(), gps.location.lng(), LONDON_LAT, LONDON_LON);
    
    SerialDebug.printFloat(courseToLondon, gps.location.isValid(), 7, 2);
    
    const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);
    
    SerialDebug.printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);
    
    SerialDebug.printInt(gps.charsProcessed(), true, 6);
    SerialDebug.printInt(gps.sentencesWithFix(), true, 10);
    SerialDebug.printInt(gps.failedChecksum(), true, 9);
#endif
    smartDelay(1000);
    
    if (millis() > 5000 && gps.charsProcessed() < 10)
        SerialDebug.println(F("No GPS data received: check wiring"));
    
}

void GPSSensorClass::setup()
{
    printf("GPSSensorClass::setup()\n");
    
    ss.begin(
             GPSBaud);  // It requires the use of SoftwareSerial, and assumes that
                        // you have a 4800-baud serial GPS device hooked up on pins
                        // 4(rx) and 3(tx).
                        // 它需要使用SoftwareSerial，并假设您有一个4800波特的串行GPS设备连接在引脚4(rx)和3(tx)。
    
    M5.Lcd.println(F(
                     "Sats HDOP Latitude   Longitude   Fix  Date       Time     Date Alt    "
                     "Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
    M5.Lcd.println(
                   F("          (deg)      (deg)       Age                      Age  (m)  "
                     "  --- from GPS ----  ---- to London  ----  RX    RX        Fail"));
    M5.Lcd.println(
                   F("--------------------------------------------------------------------"
                     "-------------------------------------"));
}



#ifdef NOT_USED
//! 5.15.25 try a special command local to this class
void GPSSensorClass::messageLocal_SensorClassType(char *message)
{
    
}

//! adding the messages as well
//! 5.14.25 (Laura/Paul flying). 5.14.74 great Dead
//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 12.27.23 support setName == "socket"
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void GPSSensorClass::messageSetVal_SensorClassType(char *setName, char* valValue, boolean deviceNameSpecified)
{
    
}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the  send -
void GPSSensorClass::messageSend_SensorClassType(char *sendValue)
{
    
}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the cmd
void GPSSensorClass::messageCmd_SensorClassType(char *cmdValue)
{
    
}
#endif

#else
#ifdef NOT_NOW
//!@see https://www.w3schools.com/cpp/cpp_class_methods.asp
void GPSSensorClass::loop()
{
    printf("GPSSensorClass::loop()\n");
}

void GPSSensorClass::setup()
{
    printf("GPSSensorClass::setup()\n");
}
#endif //NOT_NOW

#endif //gps defined
