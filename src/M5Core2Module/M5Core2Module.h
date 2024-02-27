
#ifndef M5Core2Module_h
#define M5Core2Module_h


//!THIS IS the setup() and loop() but using the "component" name, eg AudioModule()
//!This will perform preference initializtion as well
//! called from the setup()
//! Pass in the method to call on a loud (over a threshhold. The parameter for value will be sent
//void setup_AudioModule(void (*loudCallback)(int));
void setup_M5Core2Module();

//! called for the loop() of this plugin
void loop_M5Core2Module();


//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void messageSetVal_M5Core2Module(char *setName, char* valValue, boolean deviceNameSpecified);

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
void messageSend_M5Core2Module(char* sendValue);



//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void buttonA_ShortPress_M5Core2Module();
//!long press on buttonA (top button)
void buttonA_LongPress_M5Core2Module();
//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void buttonB_ShortPress_M5Core2Module();
//!long press on buttonA (top button)
void buttonB_LongPress_M5Core2Module();

//!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
//!//!starts with "&"*
char * currentStatusURL_M5Core2Module();


//!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * currentStatusJSON_M5Core2Module();

#endif
