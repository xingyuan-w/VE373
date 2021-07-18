#ifndef I2C_H
#define I2C_H
#include <p32xxxx.h>
void i2c_master_setup(); // set up I2C 1 as a master, at 400 kHz
void i2c_master_start(); // send a START signal
void i2c_master_restart(); // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_receive(int val); // receive a byte of data
void i2c_master_ack(int val); // send an ACK (0) or NACK (1)
void i2c_master_stop(); // send a stop
#endif
