/**
* \link BLEClientNetworking
*/
//
//  BLEClientNetworking.cpp
//  M5Stick
//
//  Created by Scott Moody on 1/20/22.
//

#include "BLEClientNetworking.h"

#include <NimBLEDevice.h>
#include "../../Defines.h"

// seems 20 bytes is the max..
//https://www.esp32.com/viewtopic.php?t=4546
//!@see https://h2zero.github.io/esp-nimble-cpp/nimconfig_8h_source.html
//#define CONFIG_BT_NIMBLE_MSYS1_BLOCK_COUNT 20

//**** NIM BLE Client Code

//!need to device way to change these ...

//  uses  NimBLEUUID

static NimBLEUUID _BLEClientServiceUUID(PT_SERVICE_UUID);
//"B0E6A4BF-CCCC-FFFF-330C-0000000000F0"); //??
static NimBLEUUID _BLEClientCharacteristicUUID(PT_CHARACTERISTIC_UUID);
//"b0e6a4bf-cccc-ffff-330c-0000000000f1");
static NimBLERemoteCharacteristic* _BLEClientCharacteristicFeed;

//forward called on the end of the scan
void scanEndedCB_BLEClient(NimBLEScanResults results);

static NimBLEAdvertisedDevice* _advertisedDevice;
//static uint32_t _scanTime = 0; /** 0 = scan forever */
static bool _doConnect = false;
static bool _isConnected_BLEClient = false;
//try storing globally so we can disconnect..
NimBLEClient *_pClient = nullptr;

//! returns if the BLEClient is turned on.. note, if connected to a BLE device, then disconnect
boolean useBLEClient()
{
    boolean val = getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_CLIENT_VALUE);
    return val;
}

//!skip logic
int _skipAddressCount = 0;
int _maxSkipAddressCount = 4;
//!address to skip (for a number of times) .. how about 1 for now..
char _skipNameOrAddress_BLEClientNetworking[50];
//! an address or name to skip (for at least 1 time)
void skipNameOrAddress_BLEClientNetworking(char *nameOrAddress)
{
    SerialTemp.print("set skipAddress: ");
    SerialTemp.println(nameOrAddress);
    
    strcpy(_skipNameOrAddress_BLEClientNetworking, nameOrAddress);
    _skipAddressCount = 0;
}
//!stops the skip after found something..
void stopSkip_BLEClientNetworking()
{
    strcpy(_skipNameOrAddress_BLEClientNetworking, "");
}

//!helper for skip, checking if an actuall address in the skip name or address
boolean containsSkipAddress(String deviceInfo)
{
    boolean flag = false;
    
    if (strlen(_skipNameOrAddress_BLEClientNetworking) > 0)
    {
        flag = (containsSubstring(deviceInfo,_skipNameOrAddress_BLEClientNetworking));
    }
    SerialTemp.printf("containsSkipAddress(%d): ", flag);
    SerialTemp.print(deviceInfo);
    SerialTemp.print(" skip=");
    SerialTemp.println(_skipNameOrAddress_BLEClientNetworking);
    return flag;
}


/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class ClientCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pClient) {
        SerialMin.printf("TIME: %d: ",getTimeStamp_mainModule());
        SerialMin.println(" BLE Connected");
        /** After connection we should change the parameters if we don't need fast response times.
         *  These settings are 150ms interval, 0 latency, 450ms timout.
         *  Timeout should be a multiple of the interval, minimum is 100ms.
         *  I find a multiple of 3-5 * the interval works best for quick response/reconnect.
         *  Min interval: 120 * 1.25ms = 150, Max interval: 120 * 1.25ms = 150, 0 latency, 60 * 10ms = 600ms timeout
         */
        
        //@see https://h2zero.github.io/esp-nimble-cpp/class_nim_b_l_e_client.html#aff7d389ec48567286ea732c54d320526
        /**
         [in]    minInterval    The minimum connection interval in 1.25ms units.
         [in]    maxInterval    The maximum connection interval in 1.25ms units.
         [in]    latency          The number of packets allowed to skip (extends max interval).
         [in]    timeout          The timeout time in 10ms units before disconnecting.
         */
        pClient->updateConnParams(120, 120, 0, 60);
        //tried: no different .. pClient->updateConnParams(120, 120, 5, 120);

        //set the connected flag
        _isConnected_BLEClient = true;
    };
    
    void onDisconnect(NimBLEClient* pClient) {
        SerialMin.printf("TIME: %d: ",getTimeStamp_mainModule());
        //SerialLots.printf("Unix Time: %d\n", now);

        SerialMin.println(" BLE Disconnected - Starting scan");
        SerialMin.print(pClient->getPeerAddress().toString().c_str());
        
        if (!stopAllProcesses_mainModule())
        {
            NimBLEDevice::getScan()->start(PSCAN_TIME, scanEndedCB_BLEClient);
        }
        else
        {
            SerialMin.println("*** in OTA update  . no scanning ***");
        }
        //print the time of day...
        
        //set the connected flag = false
        _isConnected_BLEClient = false;
    };
    
    /** Called when the peripheral requests a change to the connection parameters.
     *  Return true to accept and apply them or false to reject and keep
     *  the currently used parameters. Default will return true.
     */
    bool onConnParamsUpdateRequest(NimBLEClient* pClient, const ble_gap_upd_params* params) {
        SerialMin.printf("TIME: %d: ",getTimeStamp_mainModule());
        SerialMin.println(" onConnParamsUpdateRequest");
        if (params->itvl_min < 24) { /** 1.25ms units */
            return false;
        }
        else if (params->itvl_max > 40) { /** 1.25ms units */
            return false;
        }
        else if (params->latency > 2) { /** Number of intervals allowed to skip */
            return false;
        }
        else if (params->supervision_timeout > 100) { /** 10ms units */
            return false;
        }
        
        return true;
    };
    
    /********************* Security handled here **********************
     ****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest() {
        SerialLots.println("Client Passkey Request");
        /** return the passkey to send to the server */
        return 123456;
    };
    
    bool onConfirmPIN(uint32_t pass_key) {
        SerialLots.print("The passkey YES/NO number: ");
        SerialLots.println(pass_key);
        /** Return false if passkeys don't match. */
        return true;
    };
    
    /** Pairing process complete, we can check the results in ble_gap_conn_desc */
    void onAuthenticationComplete(ble_gap_conn_desc* desc) {
        if (!desc->sec_state.encrypted) {
            SerialDebug.println("Encrypt connection failed - disconnecting");
            /** Find the client with the connection handle provided in desc */
            NimBLEDevice::getClientByID(desc->conn_handle)->disconnect();
            
            //not connected
            _isConnected_BLEClient = false;
            return;
        }
    };
};


/** Define a class to handle the callbacks when advertisments are received */
class AdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
    
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        SerialLots.printf(" *** onResult adv, useBLEClient = %d\n", useBLEClient());
        if (!useBLEClient() || stopAllProcesses_mainModule())

        {
            //This helps if not already connected...
            _doConnect = false;
            return;
        }
        SerialLots.print("BLE Devices Advertised: ");
        SerialLots.println(advertisedDevice->toString().c_str());
        //The found service, see if its our service..
        String deviceInfo = advertisedDevice->toString().c_str();
        
        
        //!if the only-connect-to-GEN3 is turned on .. then wait until a GEN3 is found before connecting.
        boolean foundGEN3 = false;
        //temp:
        boolean requireGEN3 = getPreferenceBoolean_mainModule(PREFERENCE_ONLY_GEN3_CONNECT_SETTING);
        if (advertisedDevice->isAdvertisingService(NimBLEUUID("DEAD"))) //this was the original code service called UUID "DEAD"
        {
            SerialMin.print("Device advertised: ");
            SerialMin.println(advertisedDevice->getName().c_str());
            SerialLots.print("Device URI: ");
            SerialLots.println(advertisedDevice->getURI().c_str());
            
            //SerialDebug.println(" **** Found a DEAD service .. meaning it's not a GEN3");
            // if (strcmp(advertisedDevice->getName().c_str(), MAIN_BLE_CLIENT_SERVICE)!= 0)
            if (getDiscoverM5PTClicker() && (strncmp(advertisedDevice->getName().c_str(), M5_BLE_CLIENT_SERVICE, strlen(M5_BLE_CLIENT_SERVICE)) == 0))
            {
                SerialDebug.printf("** DiscoverM5PTClicker && Found a PTCLICKER .. go for it.. %w\n",advertisedDevice->getName().c_str() );
            }
            else if (getDiscoverM5PTClicker())
            {
                //! the DiscoverM5PTClicker is set .. so only find PTClickers .. helps in crowded field
                SerialDebug.printf("** DiscoverM5PTClicker && **** ESP32 Feeder (%s) but not 'PTClicker M5' skipping..\n", advertisedDevice->getName().c_str());
                return;
            }
            else if (strncmp(advertisedDevice->getName().c_str(), MAIN_BLE_CLIENT_SERVICE, strlen(MAIN_BLE_CLIENT_SERVICE)) != 0)
            {
                SerialDebug.printf(" **** ESP32 Feeder (%s) but not 'PTFeeder' skipping..\n", advertisedDevice->getName().c_str());
                //! look for "PTClicker"
                return;
            }
        }
        else  if (advertisedDevice->isAdvertisingService(NimBLEUUID(_BLEClientServiceUUID))  || (deviceInfo.indexOf(PT_SERVICE_UUID)  > 0))

        {
            SerialDebug.println(" **** Found a GEN3 Device");
            foundGEN3 = true;
        }
        if (requireGEN3 and !foundGEN3)
        {
            SerialDebug.println(" **** Looking for GEN3 but didn't find it .. so not connecting");
            _doConnect = false;
            return;
        }
      //!look for name of service as well.... PTClicker vs PTFeeder
        if (advertisedDevice->isAdvertisingService(NimBLEUUID(_BLEClientServiceUUID))  || (deviceInfo.indexOf(PT_SERVICE_UUID)  > 0)) //this was the original code service called "DEAD"
        {
            SerialTemp.println("Found candidate BLE Service: ");
            SerialTemp.println(advertisedDevice->toString().c_str());
            String deviceInfo = advertisedDevice->toString().c_str();
            //"Name: PTFeeder:PumpkinUno, Address: 08:3a:f2:51:7c:3e, serviceUUID: 0xdead"
            // Name: PTFeeder, Address: 00:05:c6:75:55:15, txPower: 0  (GEN3 version)

            //see if its one we are skipping..
            if (containsSkipAddress(deviceInfo))
            {
                _skipAddressCount++;
                if (_skipAddressCount > _maxSkipAddressCount)
                {
                    SerialTemp.print("Found device, but skip is max .. so connect");

                    //and the count of times
                    stopSkip_BLEClientNetworking();
                    //fall through ..
                }
                else
                {
                    SerialTemp.print("Found device, but will skip as it contains nameOrAddress: ");
                    SerialTemp.println(_skipNameOrAddress_BLEClientNetworking);
                    
                    _doConnect = false;
                    return;
                }
            }

            //! THIS CODE doesn't know what pairedDevice name is used (the :name or the :address)
            //! But just looks for an occurance of that address in the deviceInfo string..
            //!
        
            // let the main know we discovered a device (full name, let them parse it)
            // only if connected...
            
            //!if not a GEN3 .. look for a requiring a paired device in the deviceIno
           // if (!foundGEN3)
            // The address part can also be used for the GEN3 now..
            {
                //! look for a paired device in the advertisment..
                //!P = paired
                
                //! This just returns that a paired device name or address is set..
                //! The problem is that for a GEN3 that name is meaninless (except PTFeeder)
                if (isValidPairedDevice_mainModule())
#ifdef NO_MORE_PREFERENCE_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING

                //if (getPreferenceBoolean_mainModule(PREFERENCE_BLE_USE_DISCOVERED_PAIRED_DEVICE_SETTING))
#endif
                {
                    //!REQUIRE a match..
                    _doConnect = false;
                    
                    //! Having a paired name but not a paired address .. breaks the GEN3 search .. fix it (10.23.22)
                   
                    //! look for the device name to contain the paired device name (no string match)
                    //! NOTE: don't name your device PTFEEDER or it defeats somewhat (but look for a ":")
                    //! Name: PTFeeder, Address: 00:05:c6:75:55:15, txPower: 0  (GEN3 version)
                    if (containsSubstring(deviceInfo,":"))
                    {
                        boolean found = false;
                        char *pairedDeviceName = getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING);
                        
                        //! syntax correct, look for the pairedDeviceName  (will be false for GEN3 unless paired with Address)
                        if (containsSubstring(deviceInfo, pairedDeviceName))
                            found = true;
                        //!should be false for any where not matching..
                        SerialTemp.printf(" containsSubstring(%d, %s)\n", found, pairedDeviceName);
                        if (!found)
                        {
                            //! this only checks if we previously paired to the name or address, but not a new GEN3
                            char *pairedDeviceAddress = getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_ADDRESS_SETTING);
                            if (containsSubstring(deviceInfo,pairedDeviceAddress))
                                found = true;
                            
                            SerialTemp.printf(" containsSubstring(%d, '%s')\n", found, pairedDeviceAddress);
                            
                            if (!found && foundGEN3 && strlen(pairedDeviceAddress)==0)
                            {
                                found = true;
                                SerialTemp.printf(" no paired address but a GEN3 so good(%d)\n", found);
                                
                            }
                        }
                        if (found) {
                            if (foundGEN3)
                                SerialTemp.println("*** FOUND GEN3 PAIRED Device Name: ");
                            else
                                SerialTemp.println("*** FOUND BLE PAIRED Device Name: ");

                            SerialTemp.println(pairedDeviceName);
                            _doConnect = true;
                        }
                        else
                        {
                            SerialTemp.println(" *** Nothing paired ****");
                        }
                    }
                    if (!_doConnect)
                    {
                        SerialTemp.print("Not matching desired device name: '");
                        SerialTemp.print(getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_SETTING));
                        SerialTemp.print("' .. or address: ");
                        SerialTemp.printf("'%s'\n", getPreferenceString_mainModule(PREFERENCE_PAIRED_DEVICE_ADDRESS_SETTING));

                        return;
                    }
                }
            }
            //!continue on as we are good.. so stop scanning,
            setConnectedBLEDevice_mainModule(&deviceInfo[0], foundGEN3);
            
            //!invalidate the skip
            stopSkip_BLEClientNetworking();
            
            /** stop scan before connecting */
            NimBLEDevice::getScan()->stop();
            /** Save the device reference in a global for the client to use*/
            _advertisedDevice = advertisedDevice;
            /** Ready to connect now */
            _doConnect = true;
        }
    };
};


/** Notification / Indication receiving handler callback */
void notifyCB(NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
#if (SERIAL_DEBUG_DEBUG)
    std::string str = (isNotify == true) ? "Notification" : "Indication";
    str += " from ";
    /** NimBLEAddress and NimBLEUUID have std::string operators */
    str += std::string(pRemoteCharacteristic->getRemoteService()->getClient()->getPeerAddress());
    str += ": Service = " + std::string(pRemoteCharacteristic->getRemoteService()->getUUID());
    str += ", Characteristic = " + std::string(pRemoteCharacteristic->getUUID());
    str += ", Value = " + std::string((char*)pData, length);
    SerialDebug.println(str.c_str());
#endif //SerialDebug
}

//!NOTE: this is called all the time... 4.22.22  --even if the "scan-stop" invoked.
/** Callback to process the results of the last scan or restart it */
void scanEndedCB_BLEClient(NimBLEScanResults results) {
    SerialLots.println("Scan Ended");
}

/** Create a single global instance of the callback class to be used by all clients */
static ClientCallbacks _clientCB_BLEClient;

/** Handles the provisioning of clients and connects / interfaces with the server */
bool connectToServer_BLEClient()
{
    SerialCall.println("connectToServer_BLEClient");
    NimBLEClient* pClient = nullptr; //creates a new instance of a pointer to a client(1 for each server it connects to)
    /** Check if we have a client we should reuse first **/
    if (NimBLEDevice::getClientListSize()) {  //this runs only if #clients>= 1...ie no client instance start yet
        /** Special case when we already know this device, we send false as the
         *  second argument in connect() to prevent refreshing the service database.
         *  This saves considerable time and power.
         */
        pClient = NimBLEDevice::getClientByPeerAddress(_advertisedDevice->getAddress()); //if the server was connected to before there should be an address stored for it -wha
        if (pClient) {
            if (!pClient->connect(_advertisedDevice, false)) {
                SerialLots.println("Reconnect failed");
                return false;
            }
            SerialDebug.println("Reconnected pClient");
        }
        /** We don't already have a client that knows this device,
         *  we will check for a client that is disconnected that we can use.
         */
        else {

            pClient = NimBLEDevice::getDisconnectedClient(); //reusing a client that was created but is disconnected  -wha
            if (pClient)
            {
                SerialLots.printf("Reusing a disconnected pClient: %s\n",pClient? pClient->getPeerAddress().toString().c_str():"NULL");
            }
        }
    }
    
    /** No client to reuse? Create a new one. */
    if (!pClient) { //if a client instance is created from above this will not be executed -wha
//go back to >= 1 (4.22.22)
//#define ONLY_DIFFERENCE_FROM_WORKING_VERSION
#ifdef ONLY_DIFFERENCE_FROM_WORKING_VERSION
        if (NimBLEDevice::getClientListSize() >= NIMBLE_MAX_CONNECTIONS)
#else
            if (NimBLEDevice::getClientListSize() >= 1 )    //wha -original example code used the max configuration of 3 --> NIMBLE_MAX_CONNECTIONS) {
#endif
            {
                //SerialDebug.println("Max clients reached - no more connections available");
                return false;
            }
        
        pClient = NimBLEDevice::createClient();
        
        SerialDebug.printf("***New client created(%d)\n", pClient);

        pClient->setClientCallbacks(&_clientCB_BLEClient, false);

        /** Set initial connection parameters: These settings are 15ms interval, 0 latency, 120ms timout.
         *  These settings are safe for 3 clients to connect reliably, can go faster if you have less
         *  connections. Timeout should be a multiple of the interval, minimum is 100ms.
         *  Min interval: 12 * 1.25ms = 15, Max interval: 12 * 1.25ms = 15, 0 latency, 51 * 10ms = 510ms timeout
         */
        /**

        [in]    minInterval    The minimum connection interval in 1.25ms units.
        [in]    maxInterval    The maximum connection interval in 1.25ms units.
        [in]    latency    The number of packets allowed to skip (extends max interval).
        [in]    timeout    The timeout time in 10ms units before disconnecting.
        [in]    scanInterval    The scan interval to use when attempting to connect in 0.625ms units.
        [in]    scanWindow    The scan window to use when attempting to connect in 0.625ms units.
*/
        
        pClient->setConnectionParams(12, 12, 0, 51);
        //try: no different pClient->setConnectionParams(12, 12, 10, 51);

        /** Set how long we are willing to wait for the connection to complete (seconds), default is 30. */
        pClient->setConnectTimeout(5);
        Serial.printf("pClient=%d\n",pClient);   //_advertisedDevice == 0 !!!
        if (!_advertisedDevice)
        {
            SerialMin.println(" **** ERROR: advertisedDevice == NULL");
            return false;
        }
        Serial.printf("_advertisedDevice=%d\n",_advertisedDevice);
        
        if (!pClient->connect(_advertisedDevice)) {

            /** Created a client but failed to connect, don't need to keep it as it has no data */
            NimBLEDevice::deleteClient(pClient);
            SerialDebug.println("Failed to connect, deleted client");
            return false;
        }
    }

    //try ..  doesn't seem to matter..
    // deleteClient might delete ourself?
    //_pClient = nullptr; (only useful in the code before 'return')
    if (!pClient->isConnected()) {
        if (!pClient->connect(_advertisedDevice)) {
            SerialDebug.println("Failed to connect");
            
            //not connected
            _isConnected_BLEClient = false;
            return false;
        }
    }
    _pClient = pClient;
    
    SerialLots.printf("number clients=   %d \n\r", NimBLEDevice::getClientListSize()); //
    SerialMin.printf("TIME: %d: ",getTimeStamp_mainModule());
    SerialMin.print(" - Connected to pClient: ");
    SerialMin.println(pClient->getPeerAddress().toString().c_str());
    SerialLots.print("RSSI: ");
    SerialLots.println(pClient->getRssi());
    
    /** Now we can read/write/subscribe the charateristics of the services we are interested in */
    NimBLERemoteService* pSvc = nullptr;
    NimBLERemoteCharacteristic* pChr = nullptr;
    NimBLERemoteDescriptor* pDsc = nullptr;
#define NEW_WAY
#ifdef NEW_WAY
    //!Try to get our new service == DEAD (for now.. this is not in the GEN3)
    pSvc = pClient->getService("DEAD");
    if (pSvc)
    {
        //! THIS IS A new ESP32 feeder
        SerialTemp.printf("*** v2.0 This is an ESP Based Feeder (not GEN3)***\n");
        // unset the gatewayFlag
        //! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE, false);
    }
    else
    {
        //! THIS IS A new ESP32 feeder
        SerialTemp.println("*** v2.0 This is an GEN3 Feeder (not ESP32)***");
        //set the gateway flag!!
        //! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE, true);
        
    }
#endif // NEW_WAY
   
    // the original was setup for service/characteristic : DEAD-BEEF   ..but we are revising for BAAD/F00D
    pSvc = pClient->getService(_BLEClientServiceUUID);  // char* _BLEClientServiceUUID   36 characters
    if (pSvc) 
    {     /** make sure it's not null */
        pChr = pSvc->getCharacteristic(_BLEClientCharacteristicUUID);  //this was the original example code characteristic called  "BEEF"
        _BLEClientCharacteristicFeed = pChr;
        if (!pChr) 
        {
          SerialLots.println(" **** BLEClientNetworking is nil in creation...");
        }
        else
        {     /** make sure it's not null */
            if (pChr->canRead()) 
            {
                SerialLots.print(pChr->getUUID().toString().c_str());
                SerialLots.print(" Value: ");
                SerialLots.println(pChr->readValue().c_str());
            }
            
            //NOTE: this string is also sent onto the ProcessClientCmd() which look at the first character
            // and if it's say 'B' it turns on the buzzer. So let's make this a character that isn't used..
#ifdef DO_IT_ANOTHER_WAY_WITHOUT_SENDING_VALUE
            String sentValue = "_ESP_32";

            if (pChr->canWrite()) 
            {
                //we are going to send a known string "sendValue = BLEClient"
                // and if we read it back below, we are a new ESP32 feeder, otherwise GEN3
                if (pChr->writeValue(sentValue)) {
                //if (pChr->writeValue("Tasty")) {
                    SerialDebug.print("Wrote new value(1) to: ");
                    SerialDebug.println(pChr->getUUID().toString().c_str());
                    SerialDebug.printf("'%s'\n", sentValue);
                }
                else 
                {
                  SerialDebug.println("Disconnecting if write failed");
                    /** Disconnect if write failed */
                    pClient->disconnect();
                    return false;
                }

                //NOTE: this looks good on the Arduino C++ supported (not same, like no "compare" but "compareTo" -- now they tell me:-) 4.23.22
                // read reply..
                if (pChr->canRead()) 
                {
                    SerialDebug.print("The value(1) of: ");
                    SerialDebug.print(pChr->getUUID().toString().c_str());
                    SerialDebug.print(" is now: '");
                    SerialDebug.print(pChr->readValue().c_str());
                    SerialDebug.println("'");
                    if (sentValue.compareTo(pChr->readValue().c_str()) == 0)
                    {
                        //! THIS IS A new ESP32 feeder
                        SerialTemp.printf("*** This is an ESP Based Feeder (not GEN3)***\n");
                        // unset the gatewayFlag
                        //! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
                        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE, false);
                    }
                    else
                    {
                        //! THIS IS A new ESP32 feeder
                        SerialTemp.println("*** This is an GEN3 Feeder (not ESP32)***");
                        //set the gateway flag!!
                        //! called to set a preference (which will be an identifier and a string, which can be converted to a number or boolean)
                        savePreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE, true);
                        
                    }
                    //NOTE:

                }
            }
#endif //DO_IT_ANOTHER_WAY_WITHOUT_SENDING_VALUE
            /** registerForNotify() has been deprecated and replaced with subscribe() / unsubscribe().
             *  Subscribe parameter defaults are: notifications=true, notifyCallback=nullptr, response=false.
             *  Unsubscribe parameter defaults are: response=false.
             */
            if (pChr->canNotify()) 
            {
                //if(!pChr->registerForNotify(notifyCB)) {
                if (!pChr->subscribe(true, notifyCB)) {
                    /** Disconnect if subscribe failed */
                    pClient->disconnect();

                    return false;
                }
            }
            else if (pChr->canIndicate()) 
            {

                /** Send false as first argument to subscribe to indications instead of notifications */
                //if(!pChr->registerForNotify(notifyCB, false)) {
                if (!pChr->subscribe(false, notifyCB)) 
                {
                    /** Disconnect if subscribe failed */
                    pClient->disconnect();
                    return false;
                }
            }
        }

    }
    else 
    {
        SerialLots.println("DEAD service not found.");
    }

    // original example  pSvc = pClient->getService("BAAD"); // reference only---remove after testing
    if (pSvc)
    {
        //SerialDebug.printf("pSvc = %s, pChr = %d\n", pSvc?pSvc:(char*)"NULL", pChr?pChr:(char*)"NULL");
        /** make sure it's not null */
        // origional example:  pChr = pSvc->getCharacteristic("F00D"); // reference only---remove after testing
        if (pChr)
        {
            /** make sure it's not null */
            if (pChr->canRead()) 
            {

                SerialDebug.print(pChr->getUUID().toString().c_str());
                SerialDebug.print(" Value: ");
                SerialDebug.println(pChr->readValue().c_str());
            }
            //SerialDebug.println(" .. NEXT LINE HANGS..."); // then reboots
            /*
             Guru Meditation Error: Core  1 panic'ed (Unhandled debug exception)
             Debug exception reason: Stack canary watchpoint triggered (loopTask)
             Core 1 register dump:
             */
            
            //FOR SOME REASON, the getdescriptor hangs on the GEN3 but not on ESP32
            //So this 'return true'  makes things working..
            return true;
#ifdef NOT_USED_FOR_SOME_REASON
            pDsc = pChr->getDescriptor(NimBLEUUID("C01D"));
            
            SerialDebug.println(" .. DOESNT GET HERE");

            if (pDsc) 
            {   /** make sure it's not null */
                SerialDebug.print("Descriptor: ");
                SerialDebug.print(pDsc->getUUID().toString().c_str());
                SerialDebug.print(" Value: ");
                SerialDebug.println(pDsc->readValue().c_str());
            }
            SerialDebug.println(" .. 111");
#else // try the following code again... 4.22.22
            if (pChr->canWrite()) 
            {
                if (pChr->writeValue("No tip!")) {
                    SerialDebug.print("Wrote new value(2) to: ");
                    SerialDebug.println(pChr->getUUID().toString().c_str());
                }
                else {
                    /** Disconnect if write failed */
                    pClient->disconnect();
                    return false;
                }
                
                if (pChr->canRead()) {
                    SerialDebug.print("The value(2) of: ");
                    SerialDebug.print(pChr->getUUID().toString().c_str());
                    SerialDebug.print(" is now: ");
                    SerialDebug.println(pChr->readValue().c_str());
                }
            }
            SerialDebug.println(" .. 222");

            /** registerForNotify() has been deprecated and replaced with subscribe() / unsubscribe().
             *  Subscribe parameter defaults are: notifications=true, notifyCallback=nullptr, response=false.
             *  Unsubscribe parameter defaults are: response=false.
             */
            if (pChr->canNotify()) {
                //if(!pChr->registerForNotify(notifyCB)) {
                if (!pChr->subscribe(true, notifyCB)) {
                    /** Disconnect if subscribe failed */
                    pClient->disconnect();
                    return false;
                }
            }
            else if (pChr->canIndicate()) {
                /** Send false as first argument to subscribe to indications instead of notifications */
                //if(!pChr->registerForNotify(notifyCB, false)) {
                if (!pChr->subscribe(false, notifyCB)) {
                    /** Disconnect if subscribe failed */
                    pClient->disconnect();
                    return false;
                }
            }
            SerialDebug.println(" .. 333");
#endif // not used
        }
        
    }
    else {
        SerialLots.println("BAAD service not found.");
    }
    
    SerialDebug.println("Done with this device!");
    return true;
}

//! returns whether connected over BLE as a client to a server(like a ESP feeder)
boolean isConnectedBLEClient()
{
    //not connected
    //_isConnected_BLEClient = false;
    // seems the _BLEClientCharacteristicFeed is not null for ever.. so not usable
    //boolean isConnected = _BLEClientCharacteristicFeed?true:false;
    boolean isConnected = _isConnected_BLEClient;

    SerialLots.printf("isConnectedBLEClient: %d\n",isConnected);
    return isConnected;
}

//!sends the "feed" command over bluetooth to the connected device..
void sendFeedCommandBLEClient()
{
    SerialLots.println("sendFeedCommandBLEClient()");
    // if (FeedFlag == true)
    //flag is set in  button_task
    // Set the characteristic's value to be the array of bytes that is actually a string.
    std::string newValue = "s"; // this sets a value to the GATT which is the trigger value for the BLE server feeder
    const uint8_t newValueFeed = { 0x00 };
    if (!_BLEClientCharacteristicFeed)
    {
        SerialLots.println(" **** Error _BLEClientCharacteristicFeed is nil ***");
        return;
    }
    if (_BLEClientCharacteristicFeed->writeValue(newValueFeed, 1)) {  //force the length to 1.  newValue.length() may return 0 if newValue=null
        SerialMin.print("BLEClient writeValue: '");
        SerialMin.print(newValue.c_str());
        SerialMin.println("' - sent FEED");
        
        
        
        // PrevTriggerTime = millis();
        // FeedCount--;
        //         M5.Beep.beep(); //M5.Beep.tone(4000,200);
        // delay(50);
        // M5.Beep.mute();
        //            if (FeedCount <= 0){
        //                FeedCount = 0;//avoid display of negative values
        //            }
        //            M5.Lcd.setCursor(0, 80);
        //            M5.Lcd.printf("Treats= %d \n\r",FeedCount);
    }
    else {
        SerialDebug.print(newValue.c_str());
        SerialDebug.println("FAILED GATT write(1)");
        callCallbackMain(CALLBACKS_BLE_CLIENT, BLE_CLIENT_CALLBACK_STATUS_MESSAGE,(char*)"#GATTFailed");

    }
    //  FeedFlag = false;
    //  delay(100);
    /* check for the acknowledge from the server which is 0x01   */
    if (_BLEClientCharacteristicFeed->canRead()) {
        std::string value = _BLEClientCharacteristicFeed->readValue();
        SerialTemp.printf("BLE Server response: '%s'\n",(value[0]==0x01)?(char*)"01":(char*)"");
        //SerialTemp.print(value.c_str());
        //SerialTemp.println("'");
        if (value[0] == 0x01) {  // server ack is 0x01....so flash client led for the ack from the server  wha 9-28-21
                                 //                Led_ON();
                                 //                delay(150);
                                 //                Led_OFF();//
                                 //                delay(150);
            
            //call what is registered
            callCallbackMain(CALLBACKS_BLE_CLIENT, BLE_CLIENT_CALLBACK_BLINK_LIGHT,(char*)"blink");
            
            //call what is registered
            callCallbackMain(CALLBACKS_BLE_CLIENT, BLE_CLIENT_CALLBACK_STATUS_MESSAGE,(char*)"#FED");

        }
        else
        {
            SerialDebug.println("Didn't get a response 0x01");
            //call what is registered
            callCallbackMain(CALLBACKS_BLE_CLIENT, BLE_CLIENT_CALLBACK_BLINK_LIGHT,(char*)"blink");
            
            //call what is registered
            callCallbackMain(CALLBACKS_BLE_CLIENT, BLE_CLIENT_CALLBACK_STATUS_MESSAGE,(char*)"#BLE_NO_RESPONSE");

        }
    }
    
}

//!send a string of 13 characters or less
void sendCommandBLEClient_13orLess(String cmdString);

//!FOR NOW THIS IS HERE.. but it should be more generic. eg:  sendBluetoothCommand() ..
//!send the feed command
//!NOTE: if we are a gateway (gen3) then this won't work, as it's not talking back..
void sendCommandBLEClient(String cmdString)
{
    SerialDebug.print("sendCommandBLEClient(");
    SerialDebug.print(cmdString.length());
    SerialDebug.print("): '");
    SerialDebug.print(cmdString);
    SerialDebug.print("' gateway=");
    boolean isGatewayGEN3 = getPreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE);
    SerialDebug.println(isGatewayGEN3?"gen3":"esp32");
    
    //!if GEN3 then this handshaking will not work.. so just send the cmd, if longer than 1 character, it hangs .. so don't send
    if (isGatewayGEN3)
    {
        if (cmdString.length() > 0)
        {
            SerialDebug.println(" *** GEN3 and command longer than 1 character .. not sending");
        }
        else
            sendCommandBLEClient_13orLess(cmdString);
        return;
    }
    
    //Try to break the packets..
    //send in packets of 13
    int len = cmdString.length();
    String startCmd = "#MSG_START";
    sendCommandBLEClient_13orLess(startCmd);
    int start = 0;
    for (int i = 0; i < len;i+=13)
    {
        String s;
        if (start + 13 > cmdString.length())
            s = cmdString.substring(start);
        else
            s = cmdString.substring(start,start+13);
        start += 13;
        //never send a 1 character so the other side can exit on error
        if (s.length()==1)
            s += " ";
        //send this fragment
        sendCommandBLEClient_13orLess(s);
    }
    
    String endCmd = "#MSG_END";
    sendCommandBLEClient_13orLess(endCmd);
}
//!FOR NOW THIS IS HERE.. but it should be more generic. eg:  sendBluetoothCommand() ..
//!send the feed command
void sendCommandBLEClient_13orLess(String cmdString)
{
    SerialDebug.print("sendCommandBLEClient13(");
    SerialDebug.print(cmdString.length());
    SerialDebug.print("): '");
    SerialDebug.print(cmdString);
    SerialDebug.println("'");


    //! if (FeedFlag == true)
    //!flag is set in  button_task
    //! Set the characteristic's value to be the array of bytes that is actually a string.
    //! this sets a value to the GATT which is the trigger value for the BLE server feeder
    if (!_BLEClientCharacteristicFeed)
    {
        SerialDebug.println(" **** Error _BLEClientCharacteristicFeed is nil ***");
        return;
    }
    if (_BLEClientCharacteristicFeed->writeValue(cmdString, cmdString.length()),true) {
        SerialDebug.print("writeValue:");
        SerialDebug.println(cmdString.c_str());
        
    }
    else {
        SerialDebug.print(cmdString.c_str());
        SerialDebug.println("FAILED GATT write(2)");
        callCallbackMain(CALLBACKS_BLE_CLIENT, BLE_CLIENT_CALLBACK_STATUS_MESSAGE,(char*)"#GATTFailed");
        
    }
    //!delay
    delay(100);
    //! check for the acknowledge from the server which is 0x01
    if (_BLEClientCharacteristicFeed->canRead()) {
        std::string value = _BLEClientCharacteristicFeed->readValue();
        SerialLots.print("BLE Server response: '");
        SerialLots.print(value.c_str()); SerialDebug.println("'");
        if (value[0] == 0x01) {  // server ack is 0x01....so flash client led for the ack from the server  wha 9-28-21
                                 //                Led_ON();
                                 //                delay(150);
                                 //                Led_OFF();//
                                 //                delay(150);
            
            //call what is registered
            callCallbackMain(CALLBACKS_BLE_CLIENT, BLE_CLIENT_CALLBACK_BLINK_LIGHT,(char*)"blink");
            
            //call what is registered
            //callCallbackMain(CALLBACKS_BLE_CLIENT, BLE_CLIENT_CALLBACK_STATUS_MESSAGE,(char*)"#FED");
            
        }
        else
        {
            SerialDebug.println("Didn't get a response 0x01");
            //call what is registered
            callCallbackMain(CALLBACKS_BLE_CLIENT, BLE_CLIENT_CALLBACK_BLINK_LIGHT,(char*)"blink");
            
            //call what is registered
            callCallbackMain(CALLBACKS_BLE_CLIENT, BLE_CLIENT_CALLBACK_STATUS_MESSAGE,(char*)"#BLE_NO_RESPONSE");
            
        }
    }
}

//!the setup() and loop() passing the serviceName to look for..
void setup_BLEClientNetworking(char *serviceName, char *serviceUUID, char *characteristicUUID)
{
    //!address to skip (for a number of times) .. how about 1 for now..
    strcpy(_skipNameOrAddress_BLEClientNetworking,"");
    
    //!start the bluetooth discovery..
    SerialDebug.println("Starting NimBLE BLEClientNetworking");
    /** Initialize NimBLE, no device name spcified as we are not advertising */
    NimBLEDevice::init("");
       // NimBLEDevice::init(serviceName);

    //https://github.com/nkolban/esp32-snippets/issues/945
    //https://www.esp32.com/viewtopic.php?t=4546
    // NOTE working .. still only 16 chars (my 13 + 3)
    //NOT WORKING...
    //NimBLEDevice::setMTU(200);

    
    /** Set the IO capabilities of the device, each option will trigger a different pairing method.
     *  BLE_HS_IO_KEYBOARD_ONLY    - Passkey pairing
     *  BLE_HS_IO_DISPLAY_YESNO   - Numeric comparison pairing
     *  BLE_HS_IO_NO_INPUT_OUTPUT - DEFAULT setting - just works pairing
     */
    //NimBLEDevice::setSecurityIOCap(BLE_HS_IO_KEYBOARD_ONLY); // use passkey
    //NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_YESNO); //use numeric comparison
    
    /** 2 different ways to set security - both calls achieve the same result.
     *  no bonding, no man in the middle protection, secure connections.
     *
     *  These are the default values, only shown here for demonstration.
     */
    //NimBLEDevice::setSecurityAuth(false, false, true);
    NimBLEDevice::setSecurityAuth(/*BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM |*/ BLE_SM_PAIR_AUTHREQ_SC);
    
    /** Optional: set the transmit power, default is 3db */
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
    
    /** Optional: set any devices you don't want to get advertisments from */
    // NimBLEDevice::addIgnored(NimBLEAddress ("aa:bb:cc:dd:ee:ff"));
    
    /** create new scan */
    NimBLEScan* pScan = NimBLEDevice::getScan();
    
    /** create a callback that gets called when advertisers are found */
    pScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
    
    /** Set scan interval (how often) and window (how long) in milliseconds */
//    pScan->setInterval(45);
//    pScan->setWindow(15);
    
    pScan->setInterval(PSCAN_INTERVAL);
    pScan->setWindow(PSCAN_WINDOW);

    
    /** Active scan will gather scan response data from advertisers
     *  but will use more energy from both devices
     */
    pScan->setActiveScan(true);
    /** Start scanning for advertisers for the scan time specified (in seconds) 0 = forever
     *  Optional callback for when scanning stops.
     */
    pScan->start(PSCAN_TIME, scanEndedCB_BLEClient);
    //M5.Beep.tone(1000);
    // M5.Beep.setVolume(90);  //???? not sure this call is working wha
}

//! try to disconnect..
void disconnect_BLEClientNetworking()
{
    SerialDebug.println("calling _pClient->disconnect()");
    
    //disconnect../
    if (_pClient)
        _pClient->disconnect();
    
    //!force saying it's disconnected
    _isConnected_BLEClient = false;
    _doConnect = true;
    
    SerialDebug.println("DONE calling _pClient->disconnect()");

}


//!the loop()
void loop_BLEClientNetworking()
{
    //Try to disconnect ..
    //   if (_isConnected_BLEClient && !useBLEClient())
    if (!useBLEClient())
    {
        _doConnect = false;
        _isConnected_BLEClient = false;
        if (_pClient)
        {
            //SerialDebug.println("calling _pClient->disconnect()");
            
            //disconnect../
            _pClient->disconnect();
            
        }
        else
        {
            //SerialDebug.println("_isConnectedBLEClient but no _pClient");
        }
    }

    //_doConnect is a command to try and connect using connectToServer_BLEClient()  wha 8-11-21
    if (_doConnect == true) {
        SerialLots.println("_doConnect == true");
        if (connectToServer_BLEClient()) {
            SerialDebug.println("!! Connected to BLE Server.");
            callCallbackMain(CALLBACKS_BLE_CLIENT, BLE_CLIENT_CALLBACK_STATUS_MESSAGE,(char*)"#BLEServerConn");
            
            //!a specialy display for now..
            //! 1 = C connected BLE
            //specialScreen_displayModule(1);
            _doConnect = false; // TRY 6.11.22 (Family Day)
            return; /// try 4.22.22
        }
        else {
#ifdef DONT_USE_ONLY_ONE_CONNECTED
            SerialDebug.println("We have failed to connect to the server; there is nothin more we will do.");
            callCallbackMain(CALLBACKS_BLE_CLIENT, BLE_CLIENT_CALLBACK_STATUS_MESSAGE,(char*)"#BLEServerConFail");
#endif
        }
        _doConnect = false;
        //  0=stop scanning after first device found
        NimBLEDevice::getScan()->start(PSCAN_TIME, scanEndedCB_BLEClient); //resume scanning for more BLE servers
    }
    
    //!note: https://github.com/espressif/esp-idf/issues/5105
    //!  Might address the error I see: 
}
