#include "I2C.h"

void i2c_master_setup() {
    // 4M FPB
    I2C1BRG = 3; // 400 kHz
    I2C1CONbits.ON = 1; // turn on the I2C1 module
}

void i2c_master_start() {
    I2C1CONbits.SEN = 1; // send the start bit
    while (I2C1CONbits.SEN);
}

void i2c_master_restart() {
    I2C1CONbits.RSEN = 1; // send a restart
    while (I2C1CONbits.RSEN);
}

void i2c_master_send(unsigned char byte) {
    I2C1TRN = byte; // if an address, bit 0 = 0 for write, 1 for read
    while (I2C1STATbits.TRSTAT);
}

unsigned char i2c_master_receive(int val) {
    I2C1CONbits.RCEN = 1; // start receiving data
    while (!I2C1STATbits.RBF);
    char data = I2C1RCV;
    i2c_master_ack(val);
    return data; // read and return the data
}

void i2c_master_ack(int val) {
    I2C1CONbits.ACKDT = val; // store ACK/NACK in ACKDT
    I2C1CONbits.ACKEN = 1; // send ACKDT
    while (I2C1CONbits.ACKEN);
}

void i2c_master_stop() {
    // send a STOP:
    I2C1CONbits.PEN = 1; //master relinquishes bus
    while (I2C1CONbits.PEN);
}
