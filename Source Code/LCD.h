#ifndef LCD_H
#define LCD_H
#include <p32xxxx.h>
#define LCD_IDLE 0x33
#define LCD_2_LINE_4_BITS 0x28
#define LCD_2_LINE_8_BITS 0x38
#define LCD_DSP_CSR 0x0c 
#define LCD_CLR_DSP 0x01
#define LCD_CSR_INC 0x06 
#define LCD_SFT_MOV 0x14
/* define macros for interfacing ports */
#define RS PORTDbits.RD0
#define E PORTDbits.RD1
typedef unsigned char uchar;
/* define constant strings for display */
/* Function prototypes */
void MCU_init(void);
void LCD_init(void);
void LCD_putchar(uchar c);
void LCD_puts(const uchar *s);
void LCD_goto(uchar addr);
void GenMsec(void);
void DelayUsec(uchar num);
void DelayMsec(uchar num);

/* initialize the PIC32 MCU */
void MCU_init() {
    /* setup I/O ports to connect to the LCD module */
    SYSKEY = 0x0; //write invalid key to force lock
    SYSKEY = 0xAA996655; //write Key1 to SYSKEY
    SYSKEY = 0x556699AA; //Write Key2 to SYSKEY
    OSCCONbits.PBDIV = 0b00; //PBCLK = SYSCLK/8
    SYSKEY = 0x0; //Write invalid key to force lock
    TRISE = 0x0000;
    PORTE = 0x0000;
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    PORTDbits.RD0 = 0;
    PORTDbits.RD1 = 0;
    /* setup Timer and interrupt */
    /* PR1 is not set */
    INTCONbits.MVEC = 1;
    asm("ei");
    T1CON = 0;
    IPC1bits.T1IP = 4;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    TMR1 = 0;
    T1CONbits.TCKPS = 0b00;
    T1CONbits.TCS = 0;
    T1CONbits.TGATE = 0;
}

/* initialize the LCD module */
void LCD_init() {
    DelayMsec(15); //wait for 15 ms
    RS = 0; //send command
    PORTE = LCD_IDLE; //function set - 8 bit interface
    DelayMsec(5); //wait for 5 ms
    PORTE = LCD_IDLE; //function set - 8 bit interface
    DelayUsec(100); //wait for 100 us
    PORTE = LCD_IDLE; //function set
    DelayMsec(5);
    PORTE = LCD_IDLE;
    DelayUsec(100);
    LCD_putchar(LCD_2_LINE_4_BITS);
    DelayUsec(40);
    LCD_putchar(LCD_DSP_CSR);
    DelayUsec(40);
    LCD_putchar(LCD_CLR_DSP);
    DelayMsec(5);
    LCD_putchar(LCD_CSR_INC);
}

/* Send one byte c (instruction or data) to the LCD */
void LCD_putchar(uchar c) {
    E = 1;
    PORTE = c; //sending higher nibble
    E = 0; //producing falling edge on E
    E = 1;
    PORTE <<= 4; //sending lower nibble through higher 4 ports
    E = 0; //producing falling edge on E
}

/* Display a string of characters *s by continuously calling LCD_putchar() */
void LCD_puts(const uchar *s) {
    RS = 1;
    int pos = 0;
    while (s[pos] != '\0') {
        LCD_putchar(s[pos++]);
        DelayUsec(40);
    }
    RS = 0;
}

/* go to a specific DDRAM address addr */
void LCD_goto(uchar addr) {
    LCD_putchar(addr | 0b10000000);
    DelayUsec(40);
}

/* configure timer SFRs to generate num us delay*/
void DelayUsec(uchar num) {
    PR1 = (num << 2);
    TMR1 = 0x0000;
    T1CONbits.ON = 1;
    while (T1CONbits.ON);
}

/* configure timer SFRs to generate 1 ms delay*/
void GenMsec() {
    PR1 = 4000;
    TMR1 = 0x0000;
    T1CONbits.ON = 1;
    while (T1CONbits.ON);
}

/* Call GenMsec() num times to generate num ms delay*/
void DelayMsec(uchar num) {
    uchar i;
    for (i = 0; i < num; i++)
        GenMsec();
}
#pragma interrupt T1_ISR ipl4 vector 4

void T1_ISR(void) {
    T1CONbits.ON = 0;
    IFS0bits.T1IF = 0;
}
#endif