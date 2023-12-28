#include "../../Defines.h"
#ifdef ATOM_QRCODE_MODULE
#include "ATOMQRCodeModule.h"

#include "ATOMQR_UART_CMD.h"
//#include "ATOMQRButtons.h"


//!THIS IS the setup() and loop() but using the "component" name, eg AudioModule()
//!This will perform preference initializtion as well
//! called from the setup()
//! Pass in the method to call on a loud (over a threshhold. The parameter for value will be sent
//void setup_AudioModule(void (*loudCallback)(int));
void setup_ATOMQRCodeModule()
{
//    setup_ATOMQRButtons();
    setup_ATOMQR_UARD_CMD();
}

//! called for the loop() of this plugin
void loop_ATOMQRCodeModule()
{
//    loop_ATOMQRButtons();
    loop_ATOMQR_UARD_CMD();
}

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
void messageSetVal_ATOMQRCodeModule(char *setName, char* valValue)
{
    messageSetVal_ATOMQR_UARD_CMD(setName, valValue);
}



//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void buttonA_ShortPress_ATOMQRCodeModule()
{
    
    //! BUTTON PROCESSING abstraction
    //!short press on buttonA (top button)
    buttonA_ShortPress_ATOMQR_UARD_CMD();
   

}
//!long press on buttonA (top button)
void buttonA_LongPress_ATOMQRCodeModule()
{
    
    //! BUTTON PROCESSING abstraction
   
    //!long press on buttonA (top button)
    buttonA_LongPress_ATOMQR_UARD_CMD();

}

#endif
