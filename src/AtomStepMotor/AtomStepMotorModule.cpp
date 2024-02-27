#include "StepperDriver.h"

#ifdef ATOM_STEPPER_MOTOR_MODULE
#include <M5Atom.h>

int _motor_steps = 200;
int _step_divisition = 32;
int _en_pin = 22;
int _dir_pin = 23;
int _step_pin = 19;

int _step = 0;
int _speed = 0;


StepperDriver _stepperDriver(_motor_steps, _step_divisition, _en_pin, _dir_pin, _step_pin);

void setup_AtomStepMotorModule()
{
    M5.begin(true, false, true);
    _stepperDriver.setSpeed(0);
    _stepperDriver.powerEnable(true);
    delay(1600);
}

void loop_AtomStepMotorModule()
{
    if(M5.Btn.wasPressed()) {
        _stepperDriver.setSpeed(300);
        _stepperDriver.step(5000);
        _stepperDriver.step(-5000);
    }
    M5.update();
}
#else
void setup_AtomStepMotorModule()
{

}

void loop_AtomStepMotorModule()
{
    
}
#endif
