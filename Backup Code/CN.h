#ifndef CN_H
#define CN_H
#include "MPU6050.h"
#include <p32xxxx.h>
extern static MPU_DATA mpu_data;
extern static int readD;
// We have enabled data-available interrupt on MPU6050
// Please do remember to connect INT pin on MPU6050 with CN15
// Thank you!
void CN_init();
#pragma interrupt CN_ISR ipl3 vector 26
void CN_ISR();
#endif