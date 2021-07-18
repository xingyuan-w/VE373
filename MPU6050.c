#include <p32xxxx.h>
#include "MPU6050.h"

void MPU_init() {
    OSCCONbits.PBDIV = 0b00; // 4M PBCLK
    INTCONbits.MVEC = 1;
    asm("di");
    T1CON = 0;
    IPC1bits.T1IP = 4;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    TMR1 = 0;
    T1CONbits.TCKPS = 0b00;
    T1CONbits.TCS = 0;
    T1CONbits.TGATE = 0;
    asm("ei");
    for (int i = 0; i < 100; i++) delay_msec();
    i2c_master_setup();

    // sample rate
    i2c_master_start();
    i2c_master_send(0xd0);
    i2c_master_send(SMPLRT_DIV);
    i2c_master_send(0xff); // 31.25 Hz
    i2c_master_stop();

    // clock source
    i2c_master_start();
    i2c_master_send(0xd0);
    i2c_master_send(PWR_MGMT_1);
    i2c_master_send(0x01);
    i2c_master_stop();

    // DLPF
    i2c_master_start();
    i2c_master_send(0xd0);
    i2c_master_send(CONFIG);
    i2c_master_send(0x00);
    i2c_master_stop();

    // ACCEL
    i2c_master_start();
    i2c_master_send(0xd0);
    i2c_master_send(ACCEL_CONFIG);
    i2c_master_send(0x00);
    i2c_master_stop();

    // GYRO
    i2c_master_start();
    i2c_master_send(0xd0);
    i2c_master_send(GYRO_CONFIG);
    i2c_master_send(0x18);
    i2c_master_stop();

    // data ready interrupt
    i2c_master_start();
    i2c_master_send(0xd0);
    i2c_master_send(INT_ENABLE);
    i2c_master_send(0x01);
    i2c_master_stop();
}

MPU_DATA MPU_read() {
    MPU_DATA data;
    i2c_master_start();
    i2c_master_send(0xd0);
    i2c_master_send(ACCEL_XOUT_H);
    i2c_master_stop();

    i2c_master_start();
    i2c_master_send(0xd1);
    data.Ax = ((int) i2c_master_receive(0) << 8) | (int) i2c_master_receive(0);
    data.Ay = ((int) i2c_master_receive(0) << 8) | (int) i2c_master_receive(0);
    data.Az = ((int) i2c_master_receive(0) << 8) | (int) i2c_master_receive(0);
    data.T = ((int) i2c_master_receive(0) << 8) | (int) i2c_master_receive(0);
    data.Gx = ((int) i2c_master_receive(0) << 8) | (int) i2c_master_receive(0);
    data.Gy = ((int) i2c_master_receive(0) << 8) | (int) i2c_master_receive(0);
    data.Gz = ((int) i2c_master_receive(0) << 8) | (int) i2c_master_receive(1);
    i2c_master_stop();
    return data;
}

void delay_msec() {
    PR1 = 4000;
    TMR1 = 0x0000;
    T1CONbits.ON = 1;
    while (T1CONbits.ON);
}

void T1_ISR() {
    T1CONbits.ON = 0;
    IFS0bits.T1IF = 0;
}