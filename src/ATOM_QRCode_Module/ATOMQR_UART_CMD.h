
#ifndef ATOMQR_UART_CMD_h
#define ATOMQR_UART_CMD_h

void setup_ATOMQR_UARD_CMD();
void  loop_ATOMQR_UARD_CMD();

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
void messageSetVal_ATOMQR_UARD_CMD(char *setName, char* valValue);



//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void buttonA_ShortPress_ATOMQR_UARD_CMD();
//!long press on buttonA (top button)
void buttonA_LongPress_ATOMQR_UARD_CMD();

#endif
