#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File

#ifndef _DEMO5_H_                   //ifndef ����if no define ����д

#define _DEMO5_H_
/****************��������*******************/
void Gpio_select5(void); //gpio�˿������ӳ���
void delay_loop(void);
void Scan_Key5(void);
void demo5(void);
#endif

/****************�˿ں궨��*****************/
#define S1 GpioDataRegs.GPBDAT.bit.GPIOB5
#define S4 GpioDataRegs.GPBDAT.bit.GPIOB4
#define S7 GpioDataRegs.GPBDAT.bit.GPIOB3

