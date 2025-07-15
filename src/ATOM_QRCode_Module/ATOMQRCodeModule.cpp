#include "../../Defines.h"
#ifdef ATOM_QRCODE_MODULE
#include "ATOMQRCodeModule.h"


/*
 *******************************************************************************
 *                  Equipped with Atom-Lite/Matrix sample source code
 *                          配套  Atom-Lite/Matrix 示例源代码
 * Visit for more information: https://docs.m5stack.com/en/atom/atomic_qr
 * 获取更多资料请访问：https://docs.m5stack.com/zh_CN/atom/atomic_qr
 *
 * Product:  ATOM QR-CODE UART control.
 * Date: 2021/8/30
 *******************************************************************************
 Press the button to scan, and the scan result will be printed out through
 Serial. More Info pls refer: [QR module serial control command
 list]
 (https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/atombase/AtomicQR/ATOM_QRCODE_CMD_EN.pdf)
 
 @ee https://github.com/konacurrents/ESP_IOT/issues/261
 
 Remark:
 When the scanning head is not working, it is in the sleep state. In the sleep state, it must wake up first and then send a valid command (send the wake-up
 command 0x00, and then send the valid command after 50ms).
 Start decoding and stop decoding serial commands need to be valid in the host mode, please switch to the host mode first (see the serial port command
 table for details)
 */

//! semantic marker processing
#include "ATOM_SM.h"


boolean _shortPress_ATOMQRCodeModule = false;
boolean _longPress_ATOMQRCodeModule = false;
boolean _longLongPress_ATOMQRCodeModule = false;
#define MAX_SM 500
//! needs to be initialized
char _lastSemanticMarker[MAX_SM];
//! 3.29.25 Dead 3.29.90 great
//! add UUID and FLOWnum
//char _lastSM_UUID_Flownum[60];

//#include <M5Atom.h>

#ifdef USE_FAST_LED
#include "../ATOM_LED_Module/M5Display.h"
#endif

//! not working .. 2.5.24 (birthday) the https call breaks (side effect some how)
//!         sendSecureRESTCall(getCommand);

//! temporary state of invoking the semantic marker (versus just sending as DOCFOLLOW)
boolean _ATOM_semanticMarkerInvocationState = true;

//! 3.26.24
//! set a temporary state of invoking the semantic marker (versus just sending as DOCFOLLOW)
//! invocationState - true then "invoke"
void ATOM_setSemanticMarkerInvocationState(boolean state)
{
    _ATOM_semanticMarkerInvocationState = state;
#ifdef USE_FAST_LED
    if (state)
       fillpix(L_RED);
    else
       fillpix(L_BLUE);
#endif
}
//! TODO.. have only 1 of these included in build, but change the callback
//! That requires that ATOM be a class, OR there is a single Sensor but different callback..
//#define KEY_UNIT_SENSOR_CLASS in defines.h
#ifdef  KEY_UNIT_SENSOR_CLASS

#include "../SensorClass/SensorClassType.h"
#include "../SensorClass/KeyUnitSensorClass.h"



//! keyUnitSensorClass object for the BUTTON
KeyUnitSensorClass *_KeyUnitSensorClass_ATOMQRCodeModule;


//a pointer to a callback function that takes (char*) and returns void
void M5AtomCallback(char *parameter, boolean flag)
{
    SerialDebug.printf("M5Atom.sensorCallbackSignature(%s,%d)\n", parameter, flag);
    
 //   sendMessageString_mainModule((char*)"M5Atom.KEY Pressed ");
    
#ifdef USE_FAST_LED
    fillpix(L_YELLOW);
#endif
    SerialDebug.printf("1.Sending last SM = '%s'\n", _lastSemanticMarker);
    // send the _lastSemanticMarker again ...
    //!send this as a DOCFOLLOW message
    //  sendSemanticMarkerDocFollow_mainModule(_lastSemanticMarker);
    
    //!process the semantic marker AGAIN
    //!used _lastSemanticMarker
    //!NOTE: this might be a TierII semantic marker (thus no UUID and FLOW)
    //!https://iDogWatch.com/bot/cmddevice/scott@konacurrents.com/PASS/M5AtomSocket/togglesocket
    boolean saveSM = ATOM_processSemanticMarker(_lastSemanticMarker, _lastSemanticMarker);
    if (saveSM)
    {
        //!send this as a DOCFOLLOW message
        sendSemanticMarkerDocFollow_mainModule(_lastSemanticMarker);
        
        //!save to spiff
        printSM_SPIFFModule(_lastSemanticMarker);
    }
}
#endif //KEY_UNIT_SENSOR_CLASS

//! 4.10.24 GPS sensor
//! 4.14.24 finally creating instance
#ifdef USE_GPS_SENSOR_CLASS
#include "../SensorClass/GPSSensorClass.h"
//! GPSSensorClass for reading GPS
GPSSensorClass *_GPSSensorClass_ATOMQRCodeModule;

//a pointer to a callback function that takes (char*) and returns void
void M5AtomCallback_GPSData(char *parameter, boolean flag)
{
    SerialDebug.printf("M5Atom_GPSData.sensorCallbackSignature(%s,%d)\n", parameter, flag);
    
    //   sendMessageString_mainModule((char*)"M5Atom.KEY Pressed ");
    
#ifdef USE_FAST_LED
    fillpix(L_YELLOW);
#endif

}

#endif

//! turn on/off the scanning .. actually go to Host mode ..
void setHostMode_ATOMQRCodeModule(boolean flag);

//!4.28.24 LA (after Greg Browning walk-of-fame introduction)
//!support toggling the scanning
boolean _scanningQRonMode = true;
//! ability to turn on/off the ATOMQRCodeModule's scanning
boolean _isOn_ATOMQRCodeModule = true;

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void messageSetVal_ATOMQRCodeModule(char *setName, char* valValue, boolean deviceNameSpecified)
{

    //! process specific commands ...
    if (strcmp(setName, "atomscanner")==0)
    {
        boolean isTrue =  isTrueString_mainModule(valValue);

        SerialTemp.printf("messageSetVal.ATOMQRCodeModule(%s,%s - %s)\n", setName, valValue, isTrue?"TRUE":"FALSE");

        _isOn_ATOMQRCodeModule = isTrue;
        //LATER..  scared.. to touch the QR scanner again..
    }
    
    //! 3.22.25 - Nice day. Stormy week.
    //! add the ScannedSemanticMarker that will set the _lastSemanticMarker
    if (strcmp(setName, "ScannedSemanticMarker")==0)
    {
        if (deviceNameSpecified)
        {
            SerialTemp.printf("ScannedSemanticMarker.ATOMQRCodeModule(%s)\n", valValue);
            strcpy(_lastSemanticMarker,valValue);
            //! 3.29.25 add UUID/FLOW if set
            
         }
        else
        {
            SerialTemp.printf("ScannedSemanticMarker.ATOMQRCodeModule(%s) - but NOT our deviceName\n", valValue);
        }
    }
}



//!forward reference .. since if button press called externally..
void loopCode_ATOMQRCodeModule();


//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void buttonA_ShortPress_ATOMQRCodeModule()
{
    _shortPress_ATOMQRCodeModule = false;
    _longPress_ATOMQRCodeModule = false;
    _longLongPress_ATOMQRCodeModule = false;
    
    _shortPress_ATOMQRCodeModule = true;
    loopCode_ATOMQRCodeModule();
}
//!long press on buttonA (top button)
void buttonA_LongPress_ATOMQRCodeModule()
{
    _shortPress_ATOMQRCodeModule = false;
    _longPress_ATOMQRCodeModule = false;
    _longLongPress_ATOMQRCodeModule = false;
    
    _longPress_ATOMQRCodeModule = true;
    loopCode_ATOMQRCodeModule();
}


//!big button on front of M5StickC Plus
void checkButtonB_ATOMQRCodeModule()
{
    _shortPress_ATOMQRCodeModule = false;
    _longPress_ATOMQRCodeModule = false;
    _longLongPress_ATOMQRCodeModule = false;
    
#ifdef ESP_M5
    //    SerialDebug.println(" checkButtonA_ATOMQRCodeModule ***");
    //    if (M5.BtnB.isPressed()) {
    //                SerialDebug.println("M5.BtnB.isPressed");
    //        _shortPress_ATOMQRCodeModule = true;
    //    }
    
    //  M5.dis.fillpix(0x00ff00);  // GREEN  绿色
    //    while (Serial2.available() > 0) {
    //        char ch = Serial2.read();
    //        SerialDebug.printf("CH= %c\n", ch);
    //    }
    
    
#ifdef ESP_M5_ATOM_S3
#else
    //!NOTE: ths issue is the timer is interruped by the scanner.. so make long-long very long..
    //was 1000  (from 500)
    if (M5.BtnB.wasReleasefor(3500))
    {
        //        buttonA_longPress_ATOMQRCodeModule();
        SerialDebug.println("ATOMQRCodeModule **** LONG LONG PRESS ***");
        _longLongPress_ATOMQRCodeModule = true;
    }
    else if (M5.BtnB.wasReleasefor(1000))
    {
        //        buttonA_longPress_ATOMQRCodeModule();
        SerialDebug.println("ATOMQRCodeModule **** LONG PRESS ***");
        _longPress_ATOMQRCodeModule = true;
    }
    else if (M5.BtnB.wasReleased())
    {
        //        buttonA_shortPress_ATOMQRCodeModule();
        SerialDebug.println("ATOMQRCodeModule **** SHORT PRESS ***");
        _shortPress_ATOMQRCodeModule = true;
        
    }
#endif
    
#endif //ESP_M5
}

uint8_t wakeup_cmd       = 0x00;
uint8_t start_scan_cmd[] = {0x04, 0xE4, 0x04, 0x00, 0xFF, 0x14};
uint8_t stop_scan_cmd[]  = {0x04, 0xE5, 0x04, 0x00, 0xFF, 0x13};
uint8_t host_mode_cmd[]  = {0x07, 0xC6, 0x04, 0x08, 0x00, 0x8A, 0x08, 0xFE, 0x95};


uint8_t buzzerVolumeHigh[]    = {0x07, 0xC6, 0x04, 0x08, 0x00, 0xFE, 0x9B};
uint8_t buzzerVolumeMiddle[]  = {0x07, 0xC6, 0x04, 0x08, 0x01, 0xFE, 0x9A};
uint8_t buzzerVolumeLow[]     = {0x07, 0xC6, 0x04, 0x08, 0x02, 0xFE, 0x99};

uint8_t bootSoundProhibit[]   = {0x08, 0xC6, 0x04, 0x08, 0x00, 0xF2, 0x0D, 0x00, 0xFE, 0x27};

//more
uint8_t continuous_mode_cmd[]  = {0x07, 0xC6, 0x04, 0x08, 0x00, 0x8A, 0x04, 0xFE, 0x99};

//more
uint8_t enable_scanning_config_mode_cmd[]    = {0x07, 0xC6, 0x04, 0x08, 0x00, 0xEC, 0x01, 0xFE, 0x3A};
uint8_t prohibit_scanning_config_mode_cmd[]  = {0x07, 0xC6, 0x04, 0x08, 0x00, 0xEC, 0x00, 0xFE, 0x3B};

//!sound settting
//!prompt sound when decoding is successful
uint8_t prohibit_prompt_sound_decode_mode_cmd[]  = {0x07, 0xC6, 0x04, 0x08, 0x38, 0x00, 0xFE, 0xEF};
uint8_t enable_prompt_sound_decode_mode_cmd[]    = {0x07, 0xC6, 0x04, 0x08, 0x38, 0x01, 0xFE, 0xEF};


//! init any globals
//! 1.5.24
void initGlobals_ATOMQRCodeModule()
{
    SerialDebug.println("init _lastSemanticMarker");
    //! 8.20.24 for version with just the button, do a feed, if a QR Scanner.. this lastSemanticMarker will be sed..
#define  DEFAULT_TO_FEED
#ifdef DEFAULT_TO_FEED
    strcpy(_lastSemanticMarker,"https://SemanticMarker.org/bot/feed");
#else
    strcpy(_lastSemanticMarker,"https://SemanticMarker.org/bot/smart?uuid=QHmwUurxC3&flow=1681406810235&flowname=SemanticMarker");
#endif
    //! 3.29.25 init with empty
    //strcpy(_lastSM_UUID_Flownum,"");

    SerialDebug.println(_lastSemanticMarker);
}

//!  the setup() for this ATOM
void setup_ATOMQRCodeModule()
{
 
#ifdef ESP_M5_ATOM_S3
    M5.begin();
#else
    //M5.begin(true, false, true);
    M5.begin(false, false, true);
#endif
    
    SerialDebug.println("setup_ATOMQRCodeModule");
    initGlobals_ATOMQRCodeModule();
    
   
    
//#define UART_VERSION
    /**
     Getting error:
     
     rst:0x7 (TG0WDT_SYS_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
     flash read err, 1000
     ets_main.c 371
     ets Jun  8 2016 00:22:57

     https://esp32.com/viewtopic.php?t=19176
     
     */
#ifdef UART_VERSION
#define UNIT_QRCODE_UART_BAUD 115200

#define RX 16
#define TX  17
    Serial2.begin(
                  UNIT_QRCODE_UART_BAUD, SERIAL_8N1, RX,
                  TX);
#else
#define RX 22
#define TX 19
    Serial2.begin(
                  9600, SERIAL_8N1, 22,
                  19);  // Set the baud rate of serial port 2 to 115200,8 data bits, no
                        // parity bits, and 1 stop bit, and set RX to 22 and TX to 19.
                        // 设置串口二的波特率为115200,8位数据位,没有校验位,1位停止位,并设置RX为22,TX为19
#endif
    //! 5.3.25 register our PIN use
    registerPinUse_mainModule(RX, "RX", "ATOMQRCodeModule", false);
    registerPinUse_mainModule(TX, "TX", "ATOMQRCodeModule", false);
    
#ifdef USE_FAST_LED
    //!NOTE: this could probably be done by ESP_IOT.ino .. but for now keep here (and in the other ATOM code..)
    setup_M5Display();
    fillpix(L_BLUE);
#endif
    
        
#ifdef KEY_UNIT_SENSOR_CLASS
        _KeyUnitSensorClass_ATOMQRCodeModule = new KeyUnitSensorClass((char*)"KeyUnitInstanceM5AtomQRCode");
        //! specify the callback
    _KeyUnitSensorClass_ATOMQRCodeModule->registerCallback(&M5AtomCallback);
        //! call the setup
    _KeyUnitSensorClass_ATOMQRCodeModule->setup();
    SerialDebug.printf("_Key = %d\n", _KeyUnitSensorClass_ATOMQRCodeModule);
#endif
    
    //! 4.10.24 GPS sensor
    //! 4.14.24 finally creating instance
#ifdef USE_GPS_SENSOR_CLASS

    //! GPSSensorClass for reading GPS
    _GPSSensorClass_ATOMQRCodeModule = new GPSSensorClass((char*)"GPSSensorCXlassM5AtomQRCode");
    _GPSSensorClass_ATOMQRCodeModule->registerCallback(&M5AtomCallback_GPSData);
    //! call the setup
    _GPSSensorClass_ATOMQRCodeModule->setup();
    
#endif
    //! NOTE: it seems that a startup of a new ATOM with QRReader, requires the HOST most first, then
    //! the continuous will work ... 12.25.23
#ifdef UART_VERSION
    uint8_t cmd[] = {0x23, 0x61, 0x41};
    Serial2.write(cmd, 3);
#else
    //! first wakeup the device
    Serial2.write(wakeup_cmd);
#endif
    delay(50);
    
 //   Serial2.write(buzzerVolumeLow, sizeof(buzzerVolumeLow));
    
    //! then send the command (host .. or maybe continuous)
    Serial2.write(host_mode_cmd, sizeof(host_mode_cmd));
    SerialDebug.println("Setting: host_mode_cmd");
    delay(50);

    //!4.25.24 in LA (ready for Star Wars/SDI at Regan Library)
    Serial2.write(prohibit_prompt_sound_decode_mode_cmd, sizeof(prohibit_prompt_sound_decode_mode_cmd));
    SerialDebug.println("Setting: prohibit_prompt_sound_decode_mode_cmd");
    delay(50);

    Serial2.write(bootSoundProhibit, sizeof(bootSoundProhibit));
    SerialDebug.println("Setting: bootSoundProhibit");


    //NOT WORKING.. it was 12.9.23 (but then no beep on detection either)
    //    Serial2.write(bootSoundProhibit, sizeof(bootSoundProhibit));
    //    delay(50);
    //    Serial2.write(buzzerVolumeLow, sizeof(buzzerVolumeLow));
    //    delay(50);
#ifdef UART_VERSION
#else
    //!@see https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/atombase/AtomicQR/ATOM_QRCODE_CMD_EN.pdf
    //   Serial2.write(prohibit_scanning_config_mode_cmd, sizeof(prohibit_scanning_config_mode_cmd));
    delay(50);
    Serial2.write(enable_scanning_config_mode_cmd, sizeof(enable_scanning_config_mode_cmd));
    SerialDebug.println("Setting: enable_scanning_config_mode_cmd");

#endif
    delay(50);
    //#define TRY_HOST
#ifdef TRY_HOST
    SerialDebug.println("TRY_HOST");
    
    //! then send the command (host .. or maybe continuous)
    Serial2.write(host_mode_cmd, sizeof(host_mode_cmd));
#else
#define TRY_CONTINUOUS
#endif
    
    
    //    Serial2.write(buzzerVolumeLow, sizeof(buzzerVolumeLow));
    //    Serial2.write(bootSoundProhibit, sizeof(bootSoundProhibit));
    //#define TRY_CONTINUOUS
#ifdef  TRY_CONTINUOUS
    //THIS IS WORKING..
    SerialDebug.println("TRY_CONTINUOUS");
#ifdef UART_VERSION
#else
    Serial2.write(continuous_mode_cmd, sizeof(continuous_mode_cmd));
    SerialDebug.println("Setting: continuous_mode_cmd");
#endif
    
#endif
    
    // strcpy(_lastSemanticMarker,"https://iDogWatch.com/bot/feed/test/test");
    //  strcpy(_lastSemanticMarker,"https://iDogWatch.com/bot/smart/test/test");
    
  //  strcpy(_lastSemanticMarker,"https://SemanticMarker.org/bot/smart?uuid=QHmwUurxC3&flow=1674517131429");
    
    //! 8.20.24 for version with just the button, do a feed, if a QR Scanner.. this lastSemanticMarker will be sed..
#define  DEFAULT_TO_FEED
#ifdef DEFAULT_TO_FEED
    strcpy(_lastSemanticMarker,"https://SemanticMarker.org/bot/feed");
#else
    strcpy(_lastSemanticMarker,"https://SemanticMarker.org/bot/smart?uuid=QHmwUurxC3&flow=1681406810235&flowname=SemanticMarker");
#endif
    SerialDebug.println("_lastSemanticMarker 2025");
    SerialDebug.println(_lastSemanticMarker);
}

//! turn on/off the scanning .. actually go to Host mode ..
void setHostMode_ATOMQRCodeModule(boolean flag)
{
    if (flag)
        Serial2.write(host_mode_cmd, sizeof(host_mode_cmd));
    else
        Serial2.write(continuous_mode_cmd, sizeof(continuous_mode_cmd));
}


//! the loop
void loop_ATOMQRCodeModule()
{
    M5.update();
    //    checkButtonA_ATOMQRCodeModule();
    checkButtonB_ATOMQRCodeModule();
    
    //! do loop code
    loopCode_ATOMQRCodeModule();
        
#ifdef KEY_UNIT_SENSOR_CLASS
    //SerialDebug.printf("_Key = %d\n", _KeyUnitSensorClass_ATOMQRCodeModule);

    _KeyUnitSensorClass_ATOMQRCodeModule->loop();
#endif
    
    //! 4.10.24 GPS sensor
    //! 4.14.24 finally creating instance
#ifdef USE_GPS_SENSOR_CLASS
    
    //! GPSSensorClass for reading GPS
    _GPSSensorClass_ATOMQRCodeModule->loop();
#endif
}

//! LOOP code.. refactored so it's also called when a buttonPress message arrives
void loopCode_ATOMQRCodeModule()
{
    
    //! this might be a mode..
    //#ifndef TRY_CONTINUOUS
#ifdef NOT_HERE
    //  if (M5.BtnA.isPressed()) {
    if (!_shortPress_ATOMQRCodeModule && !_longPress_ATOMQRCodeModule)
    {
        //        SerialDebug.println("button pressed");
        
        //     M5.dis.fillpix(0x0000ff);
        //        Serial2.write(wakeup_cmd);
        
        delay(50);
        Serial2.write(start_scan_cmd, sizeof(start_scan_cmd));
        delay(1000);
        Serial2.write(stop_scan_cmd, sizeof(stop_scan_cmd));
        
        //continuous not working..
        
        //        Serial2.write(buzzerVolumeLow, sizeof(buzzerVolumeLow));
        //        Serial2.write(bootSoundProhibit, sizeof(bootSoundProhibit));
        
        //bug in original .. typo
        // Serial2.write(start_scan_cmd, sizeof(stop_scan_cmd));
    }
#endif
    
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
    //???
    //    if (Serial2.available()) {
    //        int ch = Serial2.read();
    //        Serial.write(ch);
    //    }
    char semanticMarker[MAX_SM];
    char buf[2];
    sprintf(semanticMarker,"");
    boolean validScannedSM = false;
    //! This read from the Serial2 -- the QR Scanner device, and outputs to the serial debug
    //! BUT there seems to be strange charancters...
    while (Serial2.available() > 0)
    {
        char ch = Serial2.read();
        if (ch >= '!' && ch <= '~')
        {
            if (ch != ',')
            {
                validScannedSM = true;
                SerialCall.printf("0x%x ",ch);
                //                SerialDebug.print(ch);
                sprintf(buf,"%c",ch);
                strcat(semanticMarker,buf); //adds NULL
                
                if (strlen(semanticMarker) >= MAX_SM)
                {
                    SerialDebug.printf(" *** TOO LONG A STRING *** '%s'\n", semanticMarker);
                    strcpy(semanticMarker,"");
                    validScannedSM = false;
                }
            }
        }
    }
    
    //!sometimes it's not valid scan..
    if (validScannedSM && strlen(semanticMarker) < 5)
        validScannedSM = false;
    
    //! if a valid scanned Semantic Marker .. process it
    if (validScannedSM)
    {
#ifdef USE_FAST_LED
        fillpix(L_GREEN);
        delay(150);
#endif
        //        SerialDebug.println();
//        sprintf(buf,"%c",'\0');
//        
//        strcat(semanticMarker,buf);
        SerialDebug.printf("\n***validScannedSM = '%s'\n", semanticMarker);
        
        //!process the semantic marker.  It will save to _lastSemanticMarker unless scannedDevice
        boolean saveSM = ATOM_processSemanticMarker(semanticMarker, _lastSemanticMarker);
        if (saveSM)
        {
            SerialDebug.println(" **** setting semanticMarker *** ");
            //!save globally..
            strcpy(_lastSemanticMarker, semanticMarker);
            
            //!send this as a DOCFOLLOW message
            sendSemanticMarkerDocFollow_mainModule(semanticMarker);
            
            //!save to SPIFF
            printSM_SPIFFModule(_lastSemanticMarker);
        }
        
    }
#define LONG_PRESS_TOGGLE_SCANNING
#ifdef LONG_PRESS_TOGGLE_SCANNING
    //!todo: change long press to be TURN off scanning .. TOGGLE turning on/off scanning
    else if (_longPress_ATOMQRCodeModule)
    {
        //!toggle scan mode..
        _scanningQRonMode = !_scanningQRonMode;

        SerialDebug.printf("2._longPress_ATOMQRCodeModule");
        
        if (_scanningQRonMode)
        {
            Serial2.write(enable_scanning_config_mode_cmd, sizeof(enable_scanning_config_mode_cmd));
            SerialDebug.println("Setting: enable_scanning_config_mode_cmd");
        }
        else
        {
            Serial2.write(prohibit_scanning_config_mode_cmd, sizeof(prohibit_scanning_config_mode_cmd));
            SerialDebug.println("Setting: prohibit_scanning_config_mode_cmd");
        }

    }
#else
    //!todo: change long press to be TURN off scanning .. TOGGLE turning on/off scanning
    else if (_longPress_ATOMQRCodeModule)
    {
#ifdef USE_FAST_LED
        fillpix(L_YELLOW);
#endif
        SerialDebug.printf("2.Sending last SM = '%s'\n", _lastSemanticMarker);
        // send the _lastSemanticMarker again ...
        //!send this as a DOCFOLLOW message
        //  sendSemanticMarkerDocFollow_mainModule(_lastSemanticMarker);
        
        //!process the semantic marker AGAIN
        //!used _lastSemanticMarker
        boolean saveSM = ATOM_processSemanticMarker(_lastSemanticMarker, _lastSemanticMarker);
        if (saveSM)
        {
            //!send this as a DOCFOLLOW message
            sendSemanticMarkerDocFollow_mainModule(_lastSemanticMarker);
        }
    }
#endif// LONG_PRESS_TOGGLE_SCANNING

    else if (_longLongPress_ATOMQRCodeModule)
    {
#ifdef USE_FAST_LED
        fillpix(L_RED);
#endif
        SerialDebug.printf("CLEAN CREDENTIALS and reboot to AP mode\n");
        
        //! dispatches a call to the command specified. This is run on the next loop()
        main_dispatchAsyncCommand(ASYNC_CALL_CLEAN_CREDENTIALS);
    }
#ifdef TRY_HOST  ///no
    else if (_shortPress_ATOMQRCodeModule)
    {
        delay(50);
        Serial2.write(start_scan_cmd, sizeof(start_scan_cmd));
        delay(1000);
        Serial2.write(stop_scan_cmd, sizeof(stop_scan_cmd));
    }
#else
    else if (_shortPress_ATOMQRCodeModule)
    {
#ifdef USE_FAST_LED
        fillpix(L_BLUE);
#endif
        
        SerialDebug.printf("Feed BLE\n");
        // send the _lastSemanticMarker again ...
        //!send this as a DOCFOLLOW message
        //sendSemanticMarkerDocFollow_mainModule(_lastSemanticMarker);
        //        strcpy(_lastSemanticMarker,"https://iDogWatch.com/bot/feed/test/test");
        
        ///feed always  (done after the code below..)
        main_dispatchAsyncCommand(ASYNC_SEND_MQTT_FEED_MESSAGE);
    }
#endif
    else
    {
#ifdef USE_FAST_LED
    //    fillpix(L_WHITE);
#endif
    }
}

//! 3.31.25 add buffer to store
char _statusBuffer_ATOMQRCodeModule[100];

char *parsedSM_fromSemanticMarker(char *semanticMarker)
{
    //! 3.29.25 Raiiiinier Beeer movie last night
    //! add returning a UUID.FLOWNUM if valid, or nil if nota
    char *parsedSM = parseSM_For_UUID_Flownum(semanticMarker);
    if (strlen(parsedSM) == 0)
    {
        if (strstr(_lastSemanticMarker,"togglesocket")>0)
            parsedSM = (char*)"togglesocket";
        else if (strstr(_lastSemanticMarker,"feed")>0)
            parsedSM = (char*)"feed";
        else
            //! check for togleSocket (just for now)
            parsedSM = (char*)"default";
    }
    return parsedSM;
}
//!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
char * currentStatusURL_ATOMQRCodeModule()
{
    char *parsedSM = parsedSM_fromSemanticMarker(_lastSemanticMarker);
    sprintf(_statusBuffer_ATOMQRCodeModule,"&smscanner=on&sm=%s", parsedSM);
    return _statusBuffer_ATOMQRCodeModule;
}

//!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * currentStatusJSON_ATOMQRCodeModule()
{
    //! 3.29.25 Raiiiinier Beeer movie last night
    //! add returning a UUID.FLOWNUM if valid, or nil if nota
    char *parsedSM = parsedSM_fromSemanticMarker(_lastSemanticMarker);
    sprintf(_statusBuffer_ATOMQRCodeModule,"'smscanner':'on','sm':'%s'", parsedSM);
    return _statusBuffer_ATOMQRCodeModule;
}

/*
 Seems to have a length issue on one loop of the serial ..  OR the next one shows up.. see line 2 below.. https starts up ..
 SM = 'https://SemanticMarker.org/bot/smflowinfo?uuid=QHmwUurxC3&flow=1681834107821&flowname=MotionSensor&flowcat=ESP&USERNAME='
 SM = '&PASSWORD=&M5_DEVICE_1=https://idogwatch.com/bot/feedguestdevice/scott@konacurrents.com/doggy/HowieFeeder'
 SM = 'https://idogwatch.com/bot/feedguestdevice/scott@konacurrents.com/doggy/ScoobyDoo'
 SM = 'https://iDogWatch.com/bot/set/scott@konacurrents.com/pass/SM_VideoPlayback/on'
 SM = 'https://idogwatch.com/bot/feedguestdevice/scott@konacurrents.com/doggy/ScoobyDoo'
 
 */
#endif
