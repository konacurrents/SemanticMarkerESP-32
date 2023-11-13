

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
void messageSetVal_ATOMQRCodeModule(char *setName, char* valValue);


#endif
