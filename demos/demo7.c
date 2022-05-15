#include "assert.h"
#include "demo2.h"
#include "demo3.h"


static unsigned int msg[10]={0xC0,0xf9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};  //���룺0~9
static unsigned int DisData_Bit[4] = {0};                                         //��Ų�ֺ����λ����
static unsigned int time_T;
static unsigned int time_show = 0;
static unsigned int time_counter = 0;

static __interrupt void cpu_timer0_isr(void);
static void show_time(Uint16 second);
static void DisData_Trans(Uint16 data);
static void Sellect_Bit(Uint16 i);

void demo7(void)
{

// step 1. ��ʼ��ϵͳ����:
// ����PLL, WatchDog, ʹ������ʱ��
// ��������������Դ�DSP281x_SysCtrl.c�ļ����ҵ�.
   InitSysCtrl();

// step 2. ��ʼ��ͨ�����������·������GPIO:
// ���������DSP281x_Gpio.cԴ�ļ��б�������


   //��ʼ��SPI
   EALLOW;
   GpioMuxRegs.GPFMUX.all=0x000F;   // ѡ��GPIOΪSPI����
                                    // �˿�F MUX - x000 0000 0000 1111

// ��ʼ��Gpio

   Init_LEDS_Gpio(); //�����
   Init_Keys_Gpio(); //����

// step 3. ��������ж�,��ʼ���ж�������:
// ��ֹCPUȫ���ж�
   DINT;

// ��ʼ��PIE���ƼĴ��������ǵ�Ĭ��״̬.
// ���Ĭ��״̬���ǽ�ֹPIE�жϼ��������PIE�жϱ�־
// �����������DSP281x_PieCtrl.cԴ�ļ���
   InitPieCtrl();

// ��ֹCPU�жϣ����CPU�жϱ�־λ
   IER = 0x0000;
   IFR = 0x0000;

//��ʼ��PIE�ж���������ʹ��ָ���жϷ����ӳ���ISR��
// ��Щ�жϷ����ӳ��򱻷�����DSP281x_DefaultIsr.cԴ�ļ���
// �������������DSP281x_PieVect.cԴ�ļ�����.
   InitPieVectTable();

// step 4.��ʼ��Ƭ������:
   spi_fifo_init();   // ��ʼ��Spi FIFO
   spi_init();        // ��ʼ�� SPI



// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.
  EALLOW;  // This is needed to write to EALLOW protected registers
  PieVectTable.TINT0 = &cpu_timer0_isr;


  EDIS;    // This is needed to disable write to EALLOW protected registers

// Step 5. Initialize all the Device Peripherals:
// This function is found in DSP281x_InitPeripherals.c
// InitPeripherals(); // Not required for this example
  InitCpuTimers();   // For this example, only initialize the Cpu Timers

// Configure CPU-Timer 0 to interrupt every second:
// 100MHz CPU Freq, 1 second Period (in uSeconds)
  ConfigCpuTimer(&CpuTimer0, 150, 200000);

//   ConfigCpuTimer(&CpuTimer0, 150, 500000);
  StartCpuTimer0();
// Step 6. User specific code, enable interrupts:

// Enable CPU INT1 which is connected to CPU-Timer 0:
  IER |= M_INT1;

// Enable TINT0 in the PIE: Group 1 interrupt 7
  PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
  PieCtrlRegs.PIEIER2.bit.INTx7 = 1;

// Enable global Interrupts and higher priority real-time debug events:
  EINT;   // Enable Global interrupt INTM
  ERTM;   // Enable Global realtime interrupt DBGM

  time_T = 5; //default: 5 -> 5*200ms=1s

// Step 6. IDLE loop. Just sit and loop forever (optional):
  while(1){
      show_time(time_show);
      struct Key_Pos keypress;
      keypress = Scan_Key_Horizon_Vertical();

      printf("keypress:%d\n", keypress.val);
      if(keypress.val == 3 && time_T<15) //when time_T = 15, T = 15*200ms=3s
      {
          time_T++;
          printf("time_T:%d\n", time_T);
      }
      if(keypress.val == 6) //when time_T = 1, T = 1*200ms=0.2s
      {
        time_T = 5;
        printf("time_T:%d\n", time_T);
      }
      if(keypress.val == 9 && time_T>1) //when time_T = 1, T = 1*200ms=0.2s
      {
          time_T--;
          printf("time_T:%d\n", time_T);
      }

  };

}

static void DisData_Trans(Uint16 data)
{
    DisData_Bit[3] = data / 1000;                       //ǧλ��
    DisData_Bit[2] = data % 1000 / 100 ;                //��λ��
    DisData_Bit[1] = data % 100 / 10;                   //ʮλ��
    DisData_Bit[0] = data % 10;                         //��λ��
}

static void Sellect_Bit(Uint16 i)
{
    switch(i)
    {
        case 0:
            RST_BIT4;
            SET_BIT1;
            break;

        case 1:
            RST_BIT1;
            SET_BIT2;
            break;

        case 2:

            RST_BIT2;
            SET_BIT3;                                   //ѡͨ����ܵ���λ
            break;

        case 3:

            RST_BIT3;
            SET_BIT4;
            break;

        default:
            break;
    }
}

void show_time(Uint16 second)
{
//    assert(second >= 0);
    if(second > 1000) second = 1000;
    DisData_Trans(second);

    int Loop;
    for(Loop = 0;Loop < 80;Loop++){
        Sellect_Bit(Loop%4);                                  //ѡ��Ҫɨ��������λ
        spi_xmit(msg[DisData_Bit[Loop%4]]);                   //�������Ҫ��ʾ������
        if(Loop < 4) printf("%d", DisData_Bit[3-Loop%4]);
        delay1(1500);
    }
    printf("\n");

}

static __interrupt void cpu_timer0_isr(void)
{
//    show_time(time_show);
    time_counter++;
//    GpioDataRegs.GPACLEAR.all = 0x00FF;
    if(time_counter % time_T == 0) {
        time_show++;
    }


//    printf("time_counter:%d\n", time_counter);
   CpuTimer0.InterruptCount++;

   // Acknowledge this interrupt to receive more interrupts from group 1
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
