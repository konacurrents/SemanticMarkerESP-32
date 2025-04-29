//
//  TimerDelayClass.hpp
//  
//
//  Created by Scott Moody on 3/25/25.
//

#ifndef TimerDelayClass_hpp
#define TimerDelayClass_hpp
#include "../../Defines.h"

//! An concrete class
class TimerDelayClass
{
public:
    //! constructor
    TimerDelayClass(float defaultDelayAmountSeconds);
    
    //! destructor
    ~TimerDelayClass();
    
    //! Data members of class
    //! Generic config info .. specific the the sensor type
    
    //!starts delay calculation
    void startDelay(float delayAmountSeconds);
    
    //! stops delay
    void stopDelay();
    
    //! whether the currently delay is finished, false if not running at all
    boolean delayFinished();
    
private:
    //! local class variables. (Haven't figured out how to hide these in C++)
    float _defaultDelayAmountSeconds = 0.5;
    
    //https://forum.arduino.cc/t/getting-a-loop-to-execute-in-a-defined-amount-of-time/919975/3
    
    //https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html
    //! the time the delay started
    unsigned long _delayStart;
    //! true if still waiting for delay to finish
    boolean _delayRunning = false;
    //! length of delay
    float _delaySeconds;
};


#endif /* TimerDelayClass_hpp */
