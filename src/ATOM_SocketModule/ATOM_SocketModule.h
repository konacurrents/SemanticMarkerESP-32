

#ifndef ATOM_SocketModule_h
#define ATOM_SocketModule_h

//!THIS IS the setup() and loop() but using the "component" name, eg AudioModule()
//!This will perform preference initializtion as well
//! called from the setup()
//! Pass in the method to call on a loud (over a threshhold. The parameter for value will be sent
//void setup_AudioModule(void (*loudCallback)(int));
void setup_ATOM_SocketModule();

//! called for the loop() of this plugin
void loop_ATOM_SocketModule();


//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 12.27.23 support setName == "socket" 
void messageSetVal_ATOM_SocketModule(char *setName, char* valValue);

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
void messageSend_ATOM_SocketModule(char *sendValue);

//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void buttonA_ShortPress_ATOM_SocketModule();
//!long press on buttonA (top button)
void buttonA_LongPress_ATOM_SocketModule();

#endif
