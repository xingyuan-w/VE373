#include "CN.h"

void CN_init() {
    asm("di");
    TRISDbits.TRISD5 = 1;
    INTCONbits.MVEC = 1;
    CNCONbits.ON = 1;
    CNENbits.CNEN15 = 1; // This is RD6/CN15
    // CNPUEbits.CNPUE15 = 1;
    readD = PORTD;
    IPC6bits.CNIP = 3; // Set priority level = 3 cannot be higher than the timer
    IPC6bits.CNIS = 3; // Set subpriority level = 3
    IFS1bits.CNIF = 0; // Clear interrupt flag
    IEC1bits.CNIE = 1; // Enable Change Notice interrupts
    asm("ei");
}

void CN_ISR(void) {
    IEC1bits.CNIE = 0;
    while (PORTDbits.RD6) {
        // while there are data
        // we do not need such high sample frequency
        // it won't be helpful unless MCU is ready
        mpu_data = MPU_read();
    }
    readD = PORTD;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
}
