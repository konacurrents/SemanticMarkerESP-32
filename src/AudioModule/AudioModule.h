

#ifndef AudioModule_h
#define AudioModule_h


//!THIS IS the setup() and loop() but using the "component" name, eg AudioModule()
//!This will perform preference initializtion as well
//! called from the setup()
//! Pass in the method to call on a loud (over a threshhold. The parameter for value will be sent
//void setup_AudioModule(void (*loudCallback)(int));
void setup_AudioModule();

//! called for the loop() of this plugin
void loop_AudioModule();

#endif
