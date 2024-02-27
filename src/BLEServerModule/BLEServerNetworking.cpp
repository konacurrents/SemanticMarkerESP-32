/**
*  \link BLEServerNetworking
*/
//
//  BLENetworking.cpp
//  M5Stick
//
//  Created by Scott Moody on 1/19/22.
//

#include "BLEServerNetworking.h"
#include <NimBLEDevice.h>
#include "../../Defines.h"
#define pettutorApproach
//https://h2zero.github.io/esp-nimble-cpp/class_nim_b_l_e_device.html
//!the BLE Server
static NimBLEServer* _pBLEServer;
NimBLECharacteristic* _pCharacteristic;

//!device name
char *_deviceName_BLEServer = NULL;

char _serviceName_BLEServer[100];

//!need to device way to change these ...
//#define SERVICE_UUID        "B0E6A4BF-CCCC-FFFF-330C-0000000000F0"  //Pet Tutor feeder service for feed
//#define CHARACTERISTIC_UUID "B0E6A4BF-CCCC-FFFF-330C-0000000000F1"  //Pet Tutor feeder characteristic
char *_SERVICE_UUID;
char *_CHARACTERISTIC_UUID;

#define MAX_MESSAGE 600
char _asciiMessage[MAX_MESSAGE];


//! retrieve the service name (PTFEEDER, PTFeeder:Name, PTClicker:Name, etc)
char *getServiceName_BLEServerNetworking()
{
    return _serviceName_BLEServer;
}

//!NOT CALLED...
//!sets the device name in the advertising  
void setBLEServerDeviceName(char *deviceName)
{
    _pCharacteristic->setValue(deviceName);

    //save the deviceName
    _deviceName_BLEServer = deviceName;
}

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class BLEServeNetworkingCallbacks : public NimBLEServerCallbacks
{
    void onConnect(NimBLEServer* pServer)
    {
        SerialInfo.println("Client connected");
        SerialInfo.println("Multi-connect support: start advertising");
        NimBLEDevice::startAdvertising();
    };
    /** Alternative onConnect() method to extract details of the connection.
     See: src/ble_gap.h for the details of the ble_gap_conn_desc struct.
     */
    void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc)
    {
        SerialInfo.print("Client address: ");
        SerialInfo.println(NimBLEAddress(desc->peer_ota_addr).toString().c_str());
        /** We can use the connection handle here to ask for different connection parameters.
         Args: connection handle, min connection interval, max connection interval
         latency, supervision timeout.
         Units; Min/Max Intervals: 1.25 millisecond increments.
         Latency: number of intervals allowed to skip.
         Timeout: 10 millisecond increments, try for 5x interval time for best results.
         */
        //GOOD:
        pServer->updateConnParams(desc->conn_handle, 24, 48, 0, 60);
        
        //try:
        //pServer->updateConnParams(desc->conn_handle, 100, 200, 10, 160);
        
    };
    void onDisconnect(NimBLEServer* pServer)
    {
        SerialInfo.println("Client disconnected - start advertising");
        NimBLEDevice::startAdvertising();
    };
    void onMTUChange(uint16_t MTU, ble_gap_conn_desc* desc)
    {
        SerialInfo.printf("MTU updated: %u for connection ID: %u\n", MTU, desc->conn_handle);
    };
    
    /********************* Security handled here **********************
     ****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest()
    {
        SerialDebug.println("Server Passkey Request");
        /** This should return a random 6 digit number for security
         or make your own static passkey as done here.
         */
        return 123456;
    };
    
    bool onConfirmPIN(uint32_t pass_key)
    {
        SerialDebug.print("The passkey YES/NO number: "); SerialDebug.println(pass_key);
        /** Return false if passkeys don't match. */
        return true;
    };
    
    void onAuthenticationComplete(ble_gap_conn_desc* desc)
    {
        /** Check that encryption was successful, if not we disconnect the client */
        if (!desc->sec_state.encrypted)
        {
            NimBLEDevice::getServer()->disconnect(desc->conn_handle);
            SerialDebug.println("Encrypt connection failed - disconnecting client");
            return;
        }
        SerialDebug.println("Starting BLE work!");
    };
};

/** Handler class for characteristic actions */
//!Note: the onRead and onWrite are from the perspective of the client. So onRead is the client calling this server, over BLE, and reading a value. The onWrite is the client writing to this server (sending a BLE message). This is where JSON or raw values are used.
class BLEServerNetworkingCharacteristicCallbacks : public NimBLECharacteristicCallbacks
{
    //!called when wanting to "read" from this server
    void onRead(NimBLECharacteristic* pCharacteristic)
    {
        SerialDebug.print(pCharacteristic->getUUID().toString().c_str());
        SerialDebug.print(": onRead(), value: ");
        // SerialDebug.print(_fullJSONString);
        SerialDebug.println(pCharacteristic->getValue().c_str());
    };
    
    //!called when writing to the server.
    void onWrite(NimBLECharacteristic* pCharacteristic)
    {
        SerialDebug.print(pCharacteristic->getUUID().toString().c_str());
        SerialDebug.print(": onWrite(), value: ");
//        SerialDebug.println(pCharacteristic->getValue().c_str());
//        std::string rxValue = pCharacteristic->getValue().c_str();
        //??
        //      ProcessClientCmd(rxValue[0]);   //?? client sent a command now see what it is
        //      pCharacteristic->setValue(0x01);  //??  This is the acknowlege(ACK) back to client.  Later this should be contigent on a feed completed
        /*******************************MQTT*************************************/
        std::string value = pCharacteristic->getValue();
        SerialDebug.println(value.c_str());

        if (value.length() > 0) {
            SerialDebug.println("*********");
            SerialDebug.printf("New value: %d - \n", value.length());
            
            //create a string 'ascii' from the values
            for (int i = 0; i < value.length(); i++)
            {
                _asciiMessage[i] = value[i];
                _asciiMessage[i + 1] = 0;
                if (i >= value.length())
                    break;
            }
            //reply right away .. see what happens..
            //sendBLEMessageACKMessage();
            //pCharacteristic->setValue(0x01);  //??  This is the acknowlege(ACK) back to client.  Later this should be contigent on a feed completed
            SerialDebug.println(pCharacteristic->getUUID().toString().c_str());

#ifdef TRY_LATER
            //try .. #85
         //   if (value.compare("_ESP_32"==0))
            {SerialInfo.println("pChar->setValue(_ESP_32)");
                //send it back...
                pCharacteristic->setValue((char*)"_ESP_32");
            }
#endif
            //call the callback specified from the caller (eg. NimBLE_PetTutor_Server .. or others)
            // (*_callbackFunction)(rxValue);
            callCallbackMain(CALLBACKS_BLE_SERVER, BLE_SERVER_CALLBACK_ONWRITE, _asciiMessage);

#ifdef ESP_M5
            SerialDebug.println("*** performing an M5 specific command from BLE trigger");
            // do something ..
            addToTextMessages_displayModule("BLE MSG");
            
           // M5.Beep.beep();
            
            //!Increment the screen color 0..n cache for getting the screen color 0..max (max provided by sender)
            //!This is implemented by incrementScreenColor_mainModule() since it knows the MAX value of colors
            incrementScreenColor_displayModule();
   
#ifdef ESP_M5_CAMERA
#ifdef USE_CAMERA_MODULE

            //!take a picture
            takePicture_CameraModule();
#endif
#endif // camera
#endif
#ifdef NOT_NOW_MQTT_NETWORKING
            //This should be a 'register' command..
            
            // will return true if the message was JSON and was processes, false otherwise.
            if (processJSONMessage(ascii))
            {
                //processed by the MQTTNetworking code..
                pCharacteristic->setValue(0x01);  //??  This is the acknowlege(ACK) back to client.  Later this should be contigent on a feed completed
            }
            else
            {
                //perform feed...
                SerialInfo.println("Perform FEED");
                
                //std::string rxValue = pCharacteristic->getValue().c_str();  //??
                //stepperModule_ProcessClientCmd(rxValue[0]);   //?? client sent a command now see what it is
                processClientCommandChar_mainModule(rxValue[0]);
                pCharacteristic->setValue(0x01);  //??  This is the acknowlege(ACK) back to client.  Later this should be contigent on a feed completed

#ifdef ESP_M5
                
//#ifdef TEST_M5_TO_M5
                // do something ..
                showText_displayModule("BLE FEED");

                //!Increment the screen color 0..n cache for getting the screen color 0..max (max provided by sender)
                //!This is implemented by incrementScreenColor_mainModule() since it knows the MAX value of colors
                incrementScreenColor_displayModule();                }
            
//#endif
#endif
            }
            SerialDebug.println("*********");
            
#endif
        }
        
        /*******************************MQTT*************************************/
        
    };
    
    /** Called before notification or indication is sent,
     the value can be changed here before sending if desired.
     */
    void onNotify(NimBLECharacteristic * pCharacteristic)
    {
        SerialDebug.println("Sending notification to clients");
    };
    
    
    /** The status returned in status is defined in NimBLECharacteristic.h.
     The value returned in code is the NimBLE host return code.
     */
    void onStatus(NimBLECharacteristic * pCharacteristic, Status status, int code)
    {
#if (SERIAL_DEBUG_DEBUG)
        String str = ("Notification/Indication status code: ");
        str += status;
        str += ", return code: ";
        str += code;
        str += ", ";
        str += NimBLEUtils::returnCodeToString(code);
        SerialDebug.println(str);
#endif //serial_debug_debug
    };
    
    void onSubscribe(NimBLECharacteristic * pCharacteristic, ble_gap_conn_desc * desc, uint16_t subValue)
    {
#if (SERIAL_DEBUG_DEBUG)

        String str = "Client ID: ";
        str += desc->conn_handle;
        str += " Address: ";
        str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
        if (subValue == 0) {
            str += " Unsubscribed to ";
        }
        else if (subValue == 1) {
            str += " Subscribed to notfications for ";
        }
        else if (subValue == 2) {
            str += " Subscribed to indications for ";
        }
        else if (subValue == 3) {
            str += " Subscribed to notifications and indications for ";
        }
        str += std::string(pCharacteristic->getUUID()).c_str();
        
        SerialDebug.println(str);
#endif //serial_debug_debug
    };
};

/** Handler class for descriptor actions */
class BLEServerNetworkingDescriptorCallbacks : public NimBLEDescriptorCallbacks
{
    void onWrite(NimBLEDescriptor* pDescriptor)
    {
#if (SERIAL_DEBUG_DEBUG)

#define VERSION_1_4_1
#ifdef VERSION_1_4_1
        SerialDebug.print("Descriptor written value:");
        SerialDebug.println(pDescriptor->getValue());
#else
        std::string dscVal((char*)pDescriptor->getValue(), pDescriptor->getLength());
        SerialDebug.print("Descriptor written value:");
        SerialDebug.println(dscVal.c_str());
#endif

#endif
    };
    
    void onRead(NimBLEDescriptor* pDescriptor)
    {
        SerialDebug.print(pDescriptor->getUUID().toString().c_str());
        SerialDebug.println("Descriptor read");
        SerialDebug.printf(" DeviceName = %s\n", _deviceName_BLEServer?_deviceName_BLEServer:"nil");
    };
};


/** Define callback instances globally to use for multiple Charateristics \ Descriptors */
//static DescriptorCallbacks dscCallbacks;
//static CharacteristicCallbacks chrCallbacks;


/** Define callback instances globally to use for multiple Charateristics \ Descriptors */
static BLEServerNetworkingDescriptorCallbacks _descriptorBLEServerCallbacks;
static BLEServerNetworkingCharacteristicCallbacks _characteristicBLEServerCallbacks;

//!the 'loop' for this module BLEServerNetworking. 
void loop_BLEServerNetworking()
{

    //    SerialDebug.printf("# Clients:  %d \n\r", pServer->getConnectedCount()); // nimconfig.h can change the max clients allowed(up to 9). default is 3  wha
#ifdef originalApproach
    if (_pBLEServer->getConnectedCount()) {
        NimBLEService* pSvc = pServer->getServiceByUUID("BAAD");
        if (pSvc) {
            NimBLECharacteristic* pChr = pSvc->getCharacteristic("F00D");
            if (pChr) {
                pChr->notify(true);
            }
        }
    }
#endif
#ifdef pettutorApproach
    if (_pBLEServer->getConnectedCount()) {
        NimBLEService* pSvc = _pBLEServer->getServiceByUUID(_SERVICE_UUID);
        if (pSvc) {
            NimBLECharacteristic* pChr = pSvc->getCharacteristic(_CHARACTERISTIC_UUID);
            if (pChr) {
                pChr->notify(true);
            }
        }
    }
#endif
}
//!the 'setup' for this module BLEServerNetworking. Here the service name is added (and potentially more later)
void setup_BLEServerNetworking(char *serviceName, char * deviceName, char *serviceUUID, char *characteristicUUID)
{
    
    _SERVICE_UUID = serviceUUID;
    _CHARACTERISTIC_UUID = characteristicUUID;

    SerialMin.printf("setupBLEServerNetworking(%s,%s,%s,%s)\n", serviceName?serviceName:"NULL", deviceName?deviceName:"NULL", serviceUUID?serviceUUID:"NULL", characteristicUUID?characteristicUUID:"NULL");

    char *storedDeviceName = deviceName_mainModule();
    SerialTemp.print("Stored DeviceName = ");
    SerialTemp.println(storedDeviceName);
    if (strcmp(serviceName, "PTClicker")==0)
    {
        //! currently not using the option, just doing it...
        sprintf(_serviceName_BLEServer, "%s:%s", serviceName, storedDeviceName);
    }
    else
    {
#ifdef FORCE_USE_BLE_SERVER_DEVICE_NAME
        if (true)
#else
         if (getPreferenceBoolean_mainModule(PREFERENCE_BLE_SERVER_USE_DEVICE_NAME_SETTING))
#endif
        {
            sprintf(_serviceName_BLEServer, "%s:%s", serviceName, storedDeviceName);
        }
        else
        {
            strcpy(_serviceName_BLEServer, serviceName);
        }
    }
    
    SerialMin.print("Setting BLE serviceName: ");
    SerialMin.println(_serviceName_BLEServer);
   // SerialMin.printf("LEN = %d\n", strlen(_serviceName_BLEServer));
  
    
    //!! Seems to be a length of 31 too long
    //!    //DEBUG: crashed
    // assert failed: static void NimBLEDevice::init(const string&) NimBLEDevice.cpp:901 (rc == 0)
    // void NimBLEDevice::init(const std::string &deviceName) {
    //!@see https://github.com/espressif/arduino-esp32/issues/7894
    int len = strlen(_serviceName_BLEServer);
    if (len > 29)
    {
        _serviceName_BLEServer[28] = 0;
        SerialMin.println(_serviceName_BLEServer);

    }
    
    
    /** sets device name */
    //??original    NimBLEDevice::init("NimBLE-Arduino");
    //  NimBLEDevice::init("PTFeeder");
    //NimBLEDevice::init(serviceName);
    NimBLEDevice::init(_serviceName_BLEServer);

    /** Optional: set the transmit power, default is 3db */
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
    
    /** Set the IO capabilities of the device, each option will trigger a different pairing method.
     BLE_HS_IO_DISPLAY_ONLY    - Passkey pairing
     BLE_HS_IO_DISPLAY_YESNO   - Numeric comparison pairing
     BLE_HS_IO_NO_INPUT_OUTPUT - DEFAULT setting - just works pairing
     */
    //NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY); // use passkey
    //NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_YESNO); //use numeric comparison
    
    /** 2 different ways to set security - both calls achieve the same result.
     no bonding, no man in the middle protection, secure connections.
     
     These are the default values, only shown here for demonstration.
     */
    //NimBLEDevice::setSecurityAuth(false, false, true);
    NimBLEDevice::setSecurityAuth(/*BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM |*/ BLE_SM_PAIR_AUTHREQ_SC);
    //** NOTE: DEAD is a hex string .. and BEEF is too, so they are not really strings but UUID's in HEX.

    _pBLEServer = NimBLEDevice::createServer();
    _pBLEServer->setCallbacks(new BLEServerCallbacks());
    /******************  DEAD service              BEEF characteristic 2904notify,R/W/W_ENC(pairing REQUIRED!)                    ********************/
    NimBLEService* pDeadService = _pBLEServer->createService("DEAD");
    //??   NimBLEService* pDeadService = pServer->createService(SERVICE_UUID);
    NimBLECharacteristic* pBeefCharacteristic = pDeadService->createCharacteristic(
                                                                                   "BEEF",
                                                                                   //?? NimBLECharacteristic * pBeefCharacteristic = pDeadService->createCharacteristic(
                                                                                   //??    CHARACTERISTIC_UUID,
                                                                                   NIMBLE_PROPERTY::READ |
                                                                                   NIMBLE_PROPERTY::WRITE |
                                                                                   /** Require a secure connection for read and write access */
                                                                                   NIMBLE_PROPERTY::READ_ENC |  // only allow reading if paired / encrypted
                                                                                   NIMBLE_PROPERTY::WRITE_ENC   // only allow writing if paired / encrypted
                                                                                   );
    //NOTE: IS THIS RIGHT? vs the other characteristic?? 5.19.22
    //assign to global: It's the Food charactestic..
#ifdef OLD_BLE_NO_ACK_APPROACH
   _pCharacteristic = pBeefCharacteristic;
#endif


 
    /*******************************MQTT*************************************/
    //#ifdef MQTT_NETWORKING
    pBeefCharacteristic->setValue(deviceName);
    
    //#else
    //    pBeefCharacteristic->setValue("Burger");
    //#endif
    /*******************************MQTT*************************************/
    
    pBeefCharacteristic->setCallbacks(&_characteristicBLEServerCallbacks);
    
    /** 2904 descriptors are a special case, when createDescriptor is called with
     0x2904 a NimBLE2904 class is created with the correct properties and sizes.
     However we must cast the returned reference to the correct type as the method
     only returns a pointer to the base NimBLEDescriptor class.
     */
    NimBLE2904* pBeef2904 = (NimBLE2904*)pBeefCharacteristic->createDescriptor("2904");
    pBeef2904->setFormat(NimBLE2904::FORMAT_UTF8);
    pBeef2904->setCallbacks(&_descriptorBLEServerCallbacks);
    
    /********  BAAD service              F00D characteristic R/W/N  C01D Descriptor                   ********************************/
#ifdef originalApproach
    NimBLEService* pBaadService = pServer->createService("BAAD");
    NimBLECharacteristic* pFoodCharacteristic = pBaadService->createCharacteristic(
                                                                                   "F00D",
                                                                                   NIMBLE_PROPERTY::READ |
                                                                                   NIMBLE_PROPERTY::WRITE |
                                                                                   NIMBLE_PROPERTY::NOTIFY
                                                                                   );
#endif
#ifdef pettutorApproach
    NimBLEService* pBaadService = _pBLEServer->createService(_SERVICE_UUID);
    NimBLECharacteristic* pFoodCharacteristic = pBaadService->createCharacteristic(
                                                                                   _CHARACTERISTIC_UUID,
                                                                                   NIMBLE_PROPERTY::READ |
                                                                                   NIMBLE_PROPERTY::WRITE // |
                                                                                   //       NIMBLE_PROPERTY::NOTIFY
                                                                                   );
#endif
    
#ifdef originalApproach
    pFoodCharacteristic->setValue("Fries");
#endif
    
    pFoodCharacteristic->setCallbacks(&_characteristicBLEServerCallbacks);
    
    /** Note a 0x2902 descriptor MUST NOT be created as NimBLE will create one automatically
     if notification or indication properties are assigned to a characteristic.
     */
    
#ifdef originalApproach
    /** Custom descriptor: Arguments are UUID, Properties, max length in bytes of the value */
    NimBLEDescriptor* pC01Ddsc = pFoodCharacteristic->createDescriptor(
                                                                       "C01D",
                                                                       NIMBLE_PROPERTY::READ |
                                                                       NIMBLE_PROPERTY::WRITE |
                                                                       NIMBLE_PROPERTY::WRITE_ENC, // only allow writing if paired / encrypted
                                                                       20
                                                                       );
    pC01Ddsc->setValue("Send it back!");
    pC01Ddsc->setCallbacks(&dscCallbacks);
#endif
#ifdef pettutorApproach
    /** Custom descriptor: Arguments are UUID, Properties, max length in bytes of the value */
    NimBLEDescriptor* pPetTutordsc = pFoodCharacteristic->createDescriptor(
                                                                           "C01D", //the UUID is 0xC01D
                                                                           NIMBLE_PROPERTY::READ |
                                                                           NIMBLE_PROPERTY::WRITE |
                                                                           20
                                                                           );
    pPetTutordsc->setValue("feed s/a/j type u/m ");
    pPetTutordsc->setCallbacks(&_descriptorBLEServerCallbacks);
#endif
    
#ifdef CURRENT_BLE_ACK_APPROACH
    //THIS IS RIGHT characteristic
    _pCharacteristic = pFoodCharacteristic;
    // end setup of services and characteristics
#endif

//#define TRY_THIRD_SERVICE_WITH_NAME
#ifdef TRY_THIRD_SERVICE_WITH_NAME
#define DEVICE_NAME_UUID      "53636F74-7479426F79" //ScottyBoy
#define DEVICE_NAME_SERVICE      "6E616D65" //name
    /******************  DEAD service              BEEF characteristic 2904notify,R/W/W_ENC(pairing REQUIRED!)                    ********************/
    NimBLEService* pNameService = _pBLEServer->createService(DEVICE_NAME_SERVICE);
    //??   NimBLEService* pDeadService = pServer->createService(SERVICE_UUID);
    NimBLECharacteristic* pNameCharacteristic = pNameService->createCharacteristic(
                                                                                   DEVICE_NAME_UUID,
                                                                                   //?? NimBLECharacteristic * pBeefCharacteristic = pDeadService->createCharacteristic(
                                                                                   //??    CHARACTERISTIC_UUID,
                                                                                   NIMBLE_PROPERTY::READ |
                                                                                   NIMBLE_PROPERTY::WRITE |
                                                                                   /** Require a secure connection for read and write access */
                                                                                   NIMBLE_PROPERTY::READ_ENC |  // only allow reading if paired / encrypted
                                                                                   NIMBLE_PROPERTY::WRITE_ENC   // only allow writing if paired / encrypted
                                                                                   );
        
    
    /*******************************MQTT*************************************/
    //#ifdef MQTT_NETWORKING
    pNameCharacteristic->setValue(deviceName);
    
    //#else
    //    pBeefCharacteristic->setValue("Burger");
    //#endif
    /*******************************MQTT*************************************/
    
    pNameCharacteristic->setCallbacks(&_characteristicBLEServerCallbacks);
    
    /** 2904 descriptors are a special case, when createDescriptor is called with
     0x2904 a NimBLE2904 class is created with the correct properties and sizes.
     However we must cast the returned reference to the correct type as the method
     only returns a pointer to the base NimBLEDescriptor class.
     */
    NimBLE2904* pName2904 = (NimBLE2904*)pNameCharacteristic->createDescriptor("2904");
    pName2904->setFormat(NimBLE2904::FORMAT_UTF8);
    pName2904->setCallbacks(&_descriptorBLEServerCallbacks);
    pNameService->start();

#endif
    
    /** Start the services when finished creating all Characteristics and Descriptors */
    pDeadService->start();
    pBaadService->start();
    
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    /** Add the services to the advertisment data **/
    pAdvertising->addServiceUUID(pDeadService->getUUID());
    pAdvertising->addServiceUUID(pBaadService->getUUID());
    /** If your device is battery powered you may consider setting scan response
     to false as it will extend battery life at the expense of less data sent.
     */
#ifdef TRY_THIRD_SERVICE_WITH_NAME
    pAdvertising->addServiceUUID(pNameService->getUUID());

#endif
    pAdvertising->setScanResponse(true);
    pAdvertising->start();
    //try the user supplied local name:  Local Name == ScottyBoy in Advertisment
//    char *storedDeviceName = deviceName_mainModule();
//    SerialTemp.print("Stored DeviceName = ");
//    SerialTemp.println(storedDeviceName);
    //!setName shows up ad LocalName in Punchthrough Scanner, but
   // pAdvertising->setName(storedDeviceName);

    
    SerialInfo.println("Advertising Started");
}


//!send ACK over bluetooth, this right now is 0x01
void sendBLEMessageACKMessage()
{
    SerialTemp.printf("%d sendBLEMessageACKMessage._pCharacteristic\n", _pCharacteristic);
    _pCharacteristic->setValue(0x01);  //??  This is the acknowlege(ACK) back to client.  Later this should be contigent on a feed completed
}


