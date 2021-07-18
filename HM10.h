#ifndef HM10_H
#define HM10_H
#include <p32xxxx.h>
extern static char RX_DATA;
void set_uart1_tx();
void set_uart1_rx();
void send_byte(char data);
#pragma interrupt RX_ISR ipl1 vector 24
void RX_ISR();
#endif