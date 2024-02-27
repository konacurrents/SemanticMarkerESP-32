

#ifndef SensorClassType_h
#define SensorClassType_h
#include "../../defines.h"

//a pointer to a callback function that takes (char*) and returns void
typedef void sensorCallbackSignature(char *, boolean);

//a pointer to a callback function that takes (char*) and returns void
typedef void sensorCallbackValueSignature(char *, int);

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
    
    
    //! Pure Virtual Function
    virtual void loop() = 0;
    
     //! Pure Virtual Function
    virtual void setup() = 0;
 
    /* Other members class-wide, Not Abstract, so in SensorClassType.cpp*/
    
    //! the callback when events in the sensorClass occur
    //! This is provided on instantiation
    void registerCallback(sensorCallbackSignature *callback);
    
    //! the callback when events in the sensorClass occur
    //! This is provided on instantiation
    void registerCallbackValue(sensorCallbackValueSignature *callback);
    
    //!call the callback
    void callCallback(char *info, boolean flag);
    
    //!call the callback
    void callCallbackValue(char *info, int value);
private:
    
    //! callback storage
    sensorCallbackSignature* _callback = NULL;
    
    //! callback storage for the value
    sensorCallbackValueSignature* _callbackValue = NULL;
};

#endif
