

#ifndef ATOMQRCodeModule_h
#define ATOMQRCodeModule_h


//!THIS IS the setup() and loop() but using the "component" name, eg AudioModule()
//!This will perform preference initializtion as well
//! called from the setup()
//! Pass in the method to call on a loud (over a threshhold. The parameter for value will be sent
//void setup_AudioModule(void (*loudCallback)(int));
void setup_ATOMQRCodeModule();

//! called for the loop() of this plugin
void loop_ATOMQRCodeModule();


//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void messageSetVal_ATOMQRCodeModule(char *setName, char* valValue, boolean deviceNameSpecified);

//! BUTTON PROCESSING abstraction 
//!short press on buttonA (top button)
void buttonA_ShortPress_ATOMQRCodeModule();
//!long press on buttonA (top button)
void buttonA_LongPress_ATOMQRCodeModule();

//!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
//!//!starts with "&"*
char * currentStatusURL_ATOMQRCodeModule();


//!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * currentStatusJSON_ATOMQRCodeModule();

#endif
