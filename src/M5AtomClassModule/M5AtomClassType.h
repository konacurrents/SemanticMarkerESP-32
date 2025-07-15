//
//  M5AtomClassType.hpp
//  
//
//  Created by Scott Moody on 5/6/25.
//

#ifndef M5AtomClassType_h
#define M5AtomClassType_h

#include "../../Defines.h"

//! An mostly virtual class
class M5AtomClassType
{
public:
    //! constructor. Note: config is currently just
    //! the name of the plug (eg. HDriverStepperClass
    //! or DCMotorStepperClass
    //! BUT: it could be extended in the future
    //! to include the PIN numbers, etc.
    //! This might be JSON:  {'id':'DCMotorStepperClass,'pin1':'33'}
    M5AtomClassType(char *config);
    
    //! destructor
    ~M5AtomClassType();
    
    //! returns the identity .. actaully the config
    //! This can be used to see if this class instance matches
    //! a known name (like DCMotorStepper, etc)
    char *classIdentity();
    
    //! Data members of class
    //! Generic config info .. specific the the sensor type
    //! note: no loop()
    //!starts the PTStepper
    virtual void start_M5AtomClassType() = 0;
    
    //!setup the PTStepper
    virtual void setup_M5AtomClassType() = 0;
    
    //! stop the motor
    virtual void stop_M5AtomClassType() = 0;
    
    //!loop the PTStepper (so timer can run)
    virtual void loop_M5AtomClassType() = 0;
    
    //! these are from the ATOM
    
    //! 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the set,val
    //! 12.27.23 support setName == "socket"
    //! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
    //! It's up to the receiver to decide if it has to be specified
    virtual void messageSetVal_M5AtomClassType(char *setName, char* valValue, boolean deviceNameSpecified) = 0;
    
    //! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the  send -
    //! 5.21.25 SEND and CMD will be the same
    virtual void messageSend_M5AtomClassType(char *sendValue, boolean deviceNameSpecified) = 0;
    
    //! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the cmd
  //  virtual void messageCmd_M5AtomClassType(char *cmdValue) = 0;
    
    //! BUTTON PROCESSING abstraction
    //!short press on buttonA (top button)
    virtual void buttonA_ShortPress_M5AtomClassType() = 0;
    //!long press on buttonA (top button)
    virtual void buttonA_LongPress_M5AtomClassType() = 0;
    
    //!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
    //!starts with "&"*
    virtual char * currentStatusURL_M5AtomClassType() = 0;
    
    //!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
    //!starts with "&"*
    virtual char * currentStatusJSON_M5AtomClassType() = 0;
    
    //!6.6.24 D-Day 81 years
    //!return true or false if this should be a PTFeeder or PTClicker
    //!default 'false' to PTClicker.   The Bluetooth will use this
    boolean isPTFeeder_M5AtomClassType();
    
private:
    //! 5.3.25 create storage here. This will create dynamic memory
    //! and answer to the classIIdentity() method
    char *_identityString;
};
#endif /* M5AtomClassType_h */
