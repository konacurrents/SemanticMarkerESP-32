#ifndef StepperDriver_h
#define StepperDriver_h

#include "../../Defines.h"
#ifdef ATOM_STEPPER_MOTOR_MODULE

//!@see https://github.com/m5stack/M5-ProductExampleCodes/tree/master/AtomBase/Atomic_StepMotor/Atomic_StepMotor

// library interface description
class StepperDriver {
public:
    // constructors:
    StepperDriver(int number_of_steps, int step_division, int en_pin, int dir_pin, int step_pin);
    
    // speed setter method:
    void setSpeed(float rpm);
    
    void powerEnable(bool ena);
    
    // mover method:
    void step(long steps_to_move);
    
    void step(long steps_to_move, long steps_acc, long steps_dec);
    
private:
    void setDirection(long steps_to_move);
    
    void move();
    
    void dynamicMove(int s1, int s2);
    
    void moveInterval(unsigned long target_delay);
    
    
    int number_of_steps;
    int step_division;
    unsigned long step_interval;
    unsigned long last_step_time;
    unsigned long target_step_time1;
    unsigned long target_step_time2;
    
    // motor pin numbers:
    int en_pin;
    int dir_pin;
    int step_pin;
    
};
#endif // ATOM_STEPPER_MOTOR_MODULE

#endif
