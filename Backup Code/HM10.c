#include "HM10.h"

void set_uart1_tx() {
    OSCCONbits.PBDIV = 0; // 4M PBCLK
    U1BRG = 25; // must ensure 9600 baud rate
    U1MODE = 0; // default all zero
    U1STA = 0;
    U1STAbits.UTXEN = 1;
    U1MODEbits.ON = 1;
    // config HM10 in the central mode
    char command0[] = "AT+IMME1";
    char command1[] = "AT+ROLE1";
    char command2[] = "AT+CONA81B6AAE5221"; // WARNING: GET THE ADDRESS
    char * temp;
    for (temp = command0;(*temp)!='\0';temp++)
        send_byte(*temp);
    for (temp = command1;(*temp)!='\0';temp++)
        send_byte(*temp);
    for (temp = command2;(*temp)!='\0';temp++)
        send_byte(*temp);
}

void set_uart1_rx() {
    U1BRG = 25;
    U1MODE = 0; // default all zero
    U1STA = 0;
    U1STAbits.URXEN = 1;
    asm("di");
    // only need receiver interrupt
    IEC0bits.U1RXIE = 1;
    IPC6bits.U1IP = 1;
    IPC6bits.U1IS = 1;
    IFS0bits.U1RXIF = 0;
    asm("ei");
    U1MODEbits.ON = 1;
    U1STAbits.UTXEN = 1;
    // slave mode
    char command0[] = "AT+ROLE0";
    // command1 = "AT+ADDR?" to get addr
    char * temp;
    for (temp = command0;(*temp)!='\0';temp++)
        send_byte(*temp);
    U1STAbits.UTXEN = 0;
}

void send_byte(char data) {
    U1TXREG = data;
    while (U1STAbits.TRMT == 0);
}

void RX_ISR() {
    if (U1STAbits.URXDA) {
        RX_DATA = U1RXREG;
    }
    IFS0bits.U1RXIF = 0;
}