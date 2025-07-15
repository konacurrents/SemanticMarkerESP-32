

#ifndef SensorClassType_h
#define SensorClassType_h

#include "../../defines.h"
#ifdef ESP_M5

//!a pointer to a callback function that takes (char*) and returns void
typedef void sensorCallbackSignature(char *, boolean);

//!a pointer to a callback function that takes (char*) and returns void
//!optional
typedef void sensorCallbackValueSignature(char *, int);


//!a pointer to a callback function that takes (char*) in JSON format
//!optional
typedef void sensorCallbackJSONSignature(char *);

//! An abstract class
//! @see https://www.geeksforgeeks.org/pure-virtual-functions-and-abstract-classes/
//! @see https://www.programiz.com/cpp-programming/pure-virtual-funtion#:~:text=A%20pure%20virtual%20function%20doesn,assigning%200%20to%20the%20function.
class SensorClassType {
public:
    //! Data members of class
	 //! Generic config info .. specific the the sensor type
	 char *_config;
  
    //! Pure Virtual Function
	 //! The config will be used by 
  
    SensorClassType(char* config)
    {
        _config = config;
    };
    
    //! destructor
    ~SensorClassType();
    
    //! Pure Virtual Function
    virtual void loop() = 0;
    
     //! Pure Virtual Function
    virtual void setup() = 0;
 
    /* Other members class-wide, Not Abstract, so in SensorClassType.cpp*/
    //! set PIN 1 and PIN 2 .
    //! the class instance has to decide what to do with the pin definitions
    void setPinValues(int pin1, int pin2);
    
    //! the callback when events in the sensorClass occur
    //! This is provided on instantiation
    void registerCallback(sensorCallbackSignature *callback);
    
    //! the callback when events in the sensorClass occur
    //! This is provided on instantiation
    void registerCallbackValue(sensorCallbackValueSignature *callback);
    
    
    //! the callback when events in the sensorClass occur
    //! This is provided on instantiation
    void registerCallbackJSON(sensorCallbackJSONSignature *callback);
    
    
    //!call the callback
    void callCallback(char *info, boolean flag);
    
    //!call the callback
    void callCallbackValue(char *info, int value);
    
    //!call the callback for JSON
    void callCallbackJSON(char *JSONString);
    
    //! These can be overridden and implemented
#define THESE_ARE_VIRTUAL_OVERRIDE_THESE
    //! adding the messages as well
    //! 5.14.25 (Laura/Paul flying). 5.14.74 great Dead
    //! 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the set,val
    //! 12.27.23 support setName == "socket"
    //! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
    //! It's up to the receiver to decide if it has to be specified
    virtual void messageSetVal_SensorClassType(char *setName, char* valValue, boolean deviceNameSpecified) { }
    
    //! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the  send -
    virtual void messageSend_SensorClassType(char *sendValue) { };
    
    //! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the cmd
    virtual void messageCmd_SensorClassType(char *cmdValue) { };
    
    //! 5.15.25 try a special command local to this class
    virtual void messageLocal_SensorClassType(char *message) { };
    
    //! pin definitions
    //! pin1
    int _pin1 = 0;
    //! pin2
    int _pin2 = 0;
private:
    
    //! callback storage
    sensorCallbackSignature* _callback = NULL;
    
    //! callback storage for the value
    sensorCallbackValueSignature* _callbackValue = NULL;
    
    //! callback storage for the JSON
    sensorCallbackJSONSignature* _callbackJSON = NULL;
    
};

#endif

#endif
