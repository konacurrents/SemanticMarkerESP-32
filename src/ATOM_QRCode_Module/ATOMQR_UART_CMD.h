
#ifndef ATOMQR_UART_CMD_h
#define ATOMQR_UART_CMD_h

void setup_ATOMQR_UARD_CMD();
void  loop_ATOMQR_UARD_CMD();

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
void messageSetVal_ATOMQR_UARD_CMD(char *setName, char* valValue);

#endif
