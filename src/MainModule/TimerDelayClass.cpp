//
//  TimerDelayClass.cpp
//  
//
//  Created by Scott Moody on 3/25/25.
//

#include "TimerDelayClass.h"

//! delay seconds
//! constructor
TimerDelayClass::TimerDelayClass(float defaultDelayAmountSeconds)
{
    _defaultDelayAmountSeconds = defaultDelayAmountSeconds;
    _delayRunning = false;
}

//! stops delay
void TimerDelayClass::stopDelay()
{
    SerialLots.println("stopDelay _delayRunning=false");
    
    _delayRunning = false;
}


//! whether the currently delay is finished, false if not running at all
boolean TimerDelayClass::delayFinished()
{
    if (_delayRunning && ((millis() - _delayStart) >= (_delaySeconds * 1000)))
    {
        _delayRunning = false;
        SerialLots.println("delayFinished..");
        return true;
    }
    return false;
}

//!starts delay calculation
void TimerDelayClass::startDelay(float seconds)
{
    SerialLots.printf("startDelay: %f\n", seconds);
    
    _delayStart = millis();   // start delay
    _delayRunning = true; // not finished yet
    _delaySeconds = seconds;
    
}
