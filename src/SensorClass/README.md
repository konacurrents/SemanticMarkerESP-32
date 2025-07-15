# Types of Object Oriented Classes
1. Those that plug into the M5Atom's (The L shaped controllers).
       "sensorPlugs"
2. Sensors that are on-the-wire, so they have PIN numbers. And there can be many.
   These should be "sensorPlugs" - or we morph Sensor Plugs
3. Also the HAT - which is plugged into the Top of the M5
    
4. Note some sensors, like HAT, can work while running, and just plugged in. Others, like the LUX
   take up too much CPU trying .. when it's not plugged in.
5. I2C network. This uses wthe Wire


 This program scans the addresses 1-127 continuosly and shows the devices found
on the TFT. 

```c
Wire.begin();  // Init wire and join the I2C network.
                   // åååwireïå¶äå å¥å°I2Cçç
    // For HAT
    // Wire1.begin(0,26);
    // Wire.begin(21, 22); //Detect internal I2C, if this sentence is not added,
    // it will detect external I2C.

 M5.Lcd.println("scanning Address [HEX]");

    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(
            address);  // Data transmission to the specified device address
                       // starts.
        Wire1.beginTransmission(address);
        error = Wire.endTransmission(); /*Stop data transmission with the slave.
		  /*
                 : success. : The amount of data
                  exceeds the transmission buffer capacity limit.
                  Received NACK when sending address. 
                  Received NACK when transmitting data
		  */
        error1 = Wire1.endTransmission();
        if ((error == 0) || (error1 == 0)) {
            M5.Lcd.print(address, HEX);
            M5.Lcd.print(" ");
        } else
            M5.Lcd.print(".");

```
# SensorClassType

SensorClassType is an attempt on an OO object that represents the various plug-in sensors on the internal M5 message bus.

This needs to be morphed into the other OO types, where the config is the name of the sensor class (eg. KeyUnitSensorClass)

Then additional configuration information would define how this sensor is used (or even if used). And in particular what the PIN configuration is, which can be the dynamic I2C bus.

An EPROM value will define the use of the sensors in an runtime.

This was initially SensorPlugs, which is nice, so we might keep that, and rename the M5AtomClassModule's use of their EPROM.

M5AtomClass is what defines the plugin to the M5Atom. Thus the HDriver, QRCode Scanner, Socket, and others (like the memory card reader).

```
if  (strcasecmp(setCmdString,"sensorPlugs")==0)
   savePreference_mainModule(PREFERENCE_SENSOR_PLUGS_SETTING, valCmdString);
```
This will be renamed the M5AtomClass_Setting

Currently the M5AtomClass if an HDriver, can have various StepperModules.

Finally,

```c
//! new 1.4.24 setting ATOM kind (eg. M5AtomSocket, M5AtomScanner)
//! 1.5.24 also set the initial atom storage
//! Then the device reboots.. so setup() and loop() are for the correct ATOM
void savePreferenceATOMKind_MainModule(String value)
{
    SerialDebug.printf("M5AtomKind = %s\n", value.c_str());
    _firstTimeAtomKind = true;
    savePreference_mainModule(PREFERENCE_ATOM_KIND_SETTING,  value);
    
    //! 1.5.24 also set the initial atom storage
    //! 1.4.24 use the _atomKind (which CAN change)
    switch (getM5ATOMKind_MainModule())
    {
        case ATOM_KIND_M5_SCANNER:
            //! 8.1.23 for the ATOM Lite QRCode Reader
            savePreference_mainModule(PREFERENCE_ATOMS_SETTING,"smscanner=on");
            break;
        case ATOM_KIND_M5_SOCKET:
            //! 12.26.23 for the ATOM Socket Power
            savePreference_mainModule(PREFERENCE_ATOMS_SETTING,"socket=off");
            break;
    }

}
```
