#include <p32xxxx.h>
#include <proc/p32mx795f512l.h>
#pragma interrupt T1_ISR ipl5 vector 4

void T1_ISR(void) {
    T1CONbits.ON = 0;
    IFS0bits.T1IF = 0;
}

void GenMsec() {
    PR1 = 4000;
    TMR1 = 0x0000;
    T1CONbits.ON = 1;
    while (T1CONbits.ON);
}

void DelayMsec(int num) {
    int i;
    for (i = 0; i < num; i++) {
        GenMsec();
    }
}

void DelayUsec(int num) {
    PR1 = (num << 2);
    TMR1 = 0x0000;
    T1CONbits.ON = 1;
    while (T1CONbits.ON);
}

char CheckObs() {
    TRISDbits.TRISD10 = 1;
    PR3 = 59999; // period = 60ms
    T3CONbits.TCKPS = 0b011; // 1:8
    TMR3 = 0;
    int start = 0;
    int finish = 0;
    T3CONbits.ON = 1;
    TRISBbits.TRISB1 = 0;
    PORTBbits.RB1 = 1;
    DelayUsec(10);
    PORTBbits.RB1 = 0;
    while (PORTDbits.RD10 == 0 && TMR3 < 55000);
    if (TMR3 < 55000) start = TMR3;
    while (PORTDbits.RD10 == 1 && TMR3 < 55000);
    if (TMR3 < 55000) finish = TMR3;
    while (TMR3 < 55000);
    T3CONbits.ON = 0;
    if (start != 0 && finish != 0 && (finish - start) < 2000)
        return 'N';
    return 'Y';
}

void Init() {
    SYSKEY = 0x0; //write invalid key to force lock
    SYSKEY = 0xAA996655; //write Key1 to SYSKEY
    SYSKEY = 0x556699AA; //Write Key2 to SYSKEY
    OSCCONbits.PBDIV = 0b00;
    SYSKEY = 0x0; //Write invalid key to force lock

    U1BRG = 25;
    U1MODE = 0;
    U1STA = 0;
    IFS0bits.U1RXIF = 0;
    U1STAbits.URXEN = 1;
    U1MODEbits.ON = 1;

    asm("di");
    INTCONbits.MVEC = 1;
    T1CON = 0; // Reset Timer1 control
    IPC1bits.T1IP = 5; // Timer1 Group Priority = 2
    IFS0bits.T1IF = 0; // Reset Interrupt flag
    IEC0bits.T1IE = 1; // Enable Timer1 Individual Interrupt
    TMR1 = 0; // Reset TMR1
    T1CONbits.TCKPS = 0b00; // T1CLK = PBCLK = 4Mhz
    T1CONbits.TCS = 0; // Source: PBCLK
    T1CONbits.TGATE = 0; // Disable gated mode
    asm("ei");

    PR2 = 9999; // PWM signal period = 0.01s
    T2CONbits.TCKPS = 0b011; // 1:8
    T2CONSET = 0x8000; // start Timer 2

    OC1CON = 0x0000; // stop OC1 module
    OC1RS = 750; // initialize duty cycle register
    OC1R = 750; // initialize OC1R register for the first time
    OC1CON = 0x0006; // OC1 16-bit, Timer 2, in PWM mode w/o FP
    OC1CONSET = 0x8000;

    OC2CON = 0x0000;
    OC2RS = 750;
    OC2R = 750;
    OC2CON = 0x0006;
    OC2CONSET = 0x8000;

    OC3CON = 0x0000;
    OC3RS = 750;
    OC3R = 750;
    OC3CON = 0x0006;
    OC3CONSET = 0x8000;

    OC4CON = 0x0000;
    OC4RS = 750;
    OC4R = 750;
    OC4CON = 0x0006;
    OC4CONSET = 0x8000;

    DelayMsec(500);
}

void left() {
    OC1RS = 900;
    DelayMsec(200);
    OC2RS = 900;
    DelayMsec(200);
    OC1RS = 750;
    OC3RS = 550;
    DelayMsec(200);
    OC2RS = 750;
    DelayMsec(200);
    OC3RS = 750;
    DelayMsec(200);
}

void right() {
    OC3RS = 550;
    DelayMsec(200);
    OC4RS = 600;
    DelayMsec(200);
    OC3RS = 750;
    OC1RS = 900;
    DelayMsec(200);
    OC4RS = 750;
    DelayMsec(200);
    OC1RS = 750;
    DelayMsec(200);
}

void stop() {
    OC1RS = 750;
    OC2RS = 750;
    OC3RS = 750;
    OC4RS = 750;
    DelayMsec(200);
}

void forward() {
    if (CheckObs() == 'N') {
        stop();
        return;
    }
    OC3RS = 550;
    for (int i = 0; i < 10; i++) {
        OC2RS += 20;
        DelayMsec(50);
    }
    DelayMsec(200);
    OC1RS = 1000;
    DelayMsec(200);
    OC2RS = 750;
    DelayMsec(200);
    OC1RS = 750;
    OC3RS = 750;
    DelayMsec(200);

    OC1RS = 900;
    for (int i = 0; i < 10; i++) {
        OC4RS -= 20;
        DelayMsec(50);
    }
    DelayMsec(200);
    OC3RS = 550;
    DelayMsec(200);
    OC4RS = 750;
    DelayMsec(200);
    OC3RS = 750;
    OC1RS = 750;
    DelayMsec(200);
}

void backward() {
    OC3RS = 550;
    DelayMsec(200);
    OC4RS = 550;
    DelayMsec(200);
    OC3RS = 750;
    OC1RS = 1000;
    DelayMsec(200);
    for (int i = 0; i < 10; i++) {
        OC4RS += 20;
        DelayMsec(50);
    }
    DelayMsec(200);
    OC1RS = 750;
    DelayMsec(200);

    OC1RS = 950;
    DelayMsec(200);
    OC2RS = 900;
    DelayMsec(200);
    OC1RS = 750;
    OC3RS = 500;
    DelayMsec(200);
    for (int i = 0; i < 10; i++) {
        OC2RS -= 15;
        DelayMsec(50);
    }
    DelayMsec(200);
    OC3RS = 750;
    DelayMsec(200);
}

void main() {
    char dir = 'S';
    Init();
    while (1) {
        while (U1STAbits.URXDA)
            dir = U1RXREG;
        if (dir == 'C') right();
        if (dir == 'U') left();
        if (dir == 'F') forward();
        if (dir == 'B') backward();
        else stop();
    }
}
