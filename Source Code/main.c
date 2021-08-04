#include <p32xxxx.h>
#include <proc/p32mx795f512l.h>
#include "LCD.h"
#include <stdio.h>

char Data[10];

char direction;
// forward: 'F'
// backward: 'B'
// clockwise: 'C'
// counterclockwise: 'U'
// stop 'S'

void SetDMA() {
    IEC1CLR = 0x00010000;
    IFS1CLR = 0x00010000;
    DMACONSET = 0x00008000;
    DCH0CON = 0x03;
    DCH0ECON = (27 << 8) | 0x10;
    DCH0SSA = ((int) (&U1RXREG)) & 0x1FFFFFFF;
    DCH0DSA = ((int) (Data)) & 0x1FFFFFFF;
    DCH0SSIZ = 1;
    DCH0DSIZ = 10;
    DCH0CSIZ = 1; //one byte per UART transfer request
    DCH0INTCLR = 0x00ff00ff; //clear existing events, 
    DCH0INTbits.CHBCIE = 1;
    // DCH0INTSET = 0x00090000; //enable Block Complete and error intr
    IPC9bits.DMA0IP = 0b101; //set interrupt priority to 5,
    IPC9bits.DMA0IS = 0b010; //set interrupt sub-priority to 2
    IEC1SET = 0x00010000; //enable DMA channel 0 interrupt
    asm("di");
    IPC9bits.DMA0IP = 3;
    IFS1bits.DMA0IF = 0;
    IEC1bits.DMA0IE = 1;
    asm("ei");
    DCH0CONSET = 0x80; //turn on channel 0
}
#pragma interrupt DMA_C0_ISR ipl3 vector 36

void DMA_C0_ISR() {
    int forward = 0;
    int backward = 0;
    int clockwise = 0;
    int counterclockwise = 0;
    int stop = 0;
    for (int i = 0; i < 1; i++) {
        if (Data[i] == 'F')
            forward++;
        else if (Data[i] == 'B')
            backward++;
        else if (Data[i] == 'C')
            clockwise++;
        else if (Data[i] == 'U')
            counterclockwise++;
        else stop++;
        Data[i] = 'S';
    }
    direction = 'S';
    int curr_max = stop;
    if (curr_max < forward) {
        direction = 'F';
        curr_max = forward;
    }
    if (curr_max < backward) {
        direction = 'B';
        curr_max = backward;
    }
    if (curr_max < clockwise) {
        direction = 'C';
        curr_max = clockwise;
    }
    if (curr_max < counterclockwise)
        direction = 'U';
    U1TXREG = direction;
    SetDMA();
}

void SetUART1(unsigned int baud_setting) {
    U1BRG = baud_setting;
    U1MODE = 0; // default all zero
    U1STA = 0;
    IFS0bits.U1RXIF = 0;
    U1STAbits.URXEN = 1;
    U1STAbits.UTXEN = 1;
    U1MODEbits.ON = 1;
}

void main() {
    direction = 'S';
    for (int i = 0; i < 10; i++)
        Data[i] = 'S';
    MCU_init();
    LCD_init();
    SetUART1(25);
    LCD_puts("ON");
    SetDMA();
    while (1);
}
