#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File
#include <stdio.h>
/********************************�궨�������λѡ IO �ӿ�*******************************************/
#define  SET_BIT4   GpioDataRegs.GPBSET.bit.GPIOB8   = 1        //������� 8_LEDS ���ӵ� IOB8 ��Ӧ
#define  RST_BIT4   GpioDataRegs.GPBCLEAR.bit.GPIOB8 = 1        //������� 8_LEDS ���ӵ� IOB8 ��Ӧ
#define  SET_BIT3   GpioDataRegs.GPBSET.bit.GPIOB9   = 1        //������� 8_LEDS ���ӵ� IOB9 ��Ӧ
#define  RST_BIT3   GpioDataRegs.GPBCLEAR.bit.GPIOB9 = 1        //������� 8_LEDS ���ӵ� IOB9 ��Ӧ
#define  SET_BIT2   GpioDataRegs.GPBSET.bit.GPIOB10  = 1        //������� 8_LEDS ���ӵ� IOB10 ��Ӧ
#define  RST_BIT2   GpioDataRegs.GPBCLEAR.bit.GPIOB10 = 1       //������� 8_LEDS ���ӵ� IOB10 ��Ӧ
#define  SET_BIT1   GpioDataRegs.GPBSET.bit.GPIOB13  = 1        //������� 8_LEDS ���ӵ� IOB13 ��Ӧ
#define  RST_BIT1   GpioDataRegs.GPBCLEAR.bit.GPIOB13 = 1       //������� 8_LEDS ���ӵ� IOB13 ��Ӧ
/*****************************************************************************************************/

void demo8(void);
