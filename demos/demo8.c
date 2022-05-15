/********************************************************************
**  �ļ�����  �������ʾ����/��ѹ�����ʵ��
*   ����:   ��ѹ�����ʵ�� ��D8 ��������Ƕ�Ӧ�ŵģ�����ܹ���1.5 D9�ͻ�������D9��Ӧ����һ��ͨ������ֵ�ǿ������ģ����ǿ�debug
*   �Ҳ�֪������0֮���ͨ�����Ǹ���ģ��õ�����ֵҲ����ֵֹģ�Ŀǰ��֪��1��2��7�����õģ�������û�Թ���8��9�ֲ���
*   Author:icarusH
**********************************************************************/
/********************************************************************
// ��������:
//      ������ñ��J9�����1,2���Ŷ̽�
//    ͨ��ADCͨ��A0�ɼ�ģ���ѹֵ��Ȼ��ͨ��SPI����������
//    ͨ���������ʾ��ѹֵ
********************************************************************/

#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File

#define stuNum 0x08 // ѧ��ĩλ�������ڶ�ͨ��

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


/*********************************************��������************************************************/
//void delay(Uint32 t);
static void DisData_Trans(Uint16 data);
static void Sellect_Bit(Uint16 i);
static void Init_LEDS_Gpio(void);
static void spi_xmit(Uint16 a);
static void spi_fifo_init(void);
static void spi_init(void);
static void delay(Uint16 t);
/*****************************************************************************************************/

/************************************������ر���*********************************************/
static unsigned char msg[10]={0xC0,0xf9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};  //���룺0~9
static unsigned char DisData_Bit[4] = {0};                                         //��Ų�ֺ����λ����
static Uint16 DisData = 1234;                                                          //��ʾ������
static Uint16 Loop = 0;

static Uint16 LedBuffer[2];
static Uint16 showdata;

// ADC��������
#define ADC_MODCLK 0x3   // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)     = 25MHz
#define ADC_CKPS   0x0   // ADC ģ��ʱ�� = HSPCLK/1      = 25MHz/(1)     = 25MHz
#define ADC_SHCLK  0x1   // ��������ʱ��                 = 2 ADC ����
#define AVG        1000  // ƽ��������ֵ
#define ZOFFSET    0x00
#define BUF_SIZE   1024  // �����������ߴ�

// ������ʹ�õ�ȫ�ֱ���
//Uint16 SampleTable[BUF_SIZE];
static Uint16 SampleTable0[BUF_SIZE];
static Uint16 SampleTable1[BUF_SIZE];
static Uint16* SampleTable[2] = {SampleTable0, SampleTable1};
                                                        //ѭ��ɨ�����

/*****************************************************************************************************/

/******************************�����λѡ IO �ӿڳ�ʼ��*******************************************/

static void Init_LEDS_Gpio(void)
{
    EALLOW;

    GpioMuxRegs.GPAMUX.all=0x0000;
    GpioMuxRegs.GPADIR.all=0xFFFF;        // GPIO PORTs  as output
    GpioMuxRegs.GPAQUAL.all=0x0000;       // Set GPIO input qualifier values



    GpioDataRegs.GPBSET.bit.GPIOB8 = 1;                     // ����ߵ�ƽ
    GpioMuxRegs.GPBMUX.bit.CAP4Q1_GPIOB8 = 0;                   // GPIOB8 = GPIO
    GpioMuxRegs.GPBDIR.bit.GPIOB8 = 1;                      // GPIOB8 = output

    GpioDataRegs.GPBSET.bit.GPIOB9 = 1;                     // ����ߵ�ƽ
    GpioMuxRegs.GPBMUX.bit.CAP5Q2_GPIOB9 = 0;                   // GPIOB9 = GPIO
    GpioMuxRegs.GPBDIR.bit.GPIOB9 = 1;                      // GPIOB9 = output

    GpioDataRegs.GPBSET.bit.GPIOB10 = 1;                    // ����ߵ�ƽ
    GpioMuxRegs.GPBMUX.bit.CAP6QI2_GPIOB10 = 0;                     // GPIOB10 = GPIO
    GpioMuxRegs.GPBDIR.bit.GPIOB10 = 1;                     // GPIOB10 = output

    GpioDataRegs.GPBSET.bit.GPIOB13 = 1;                    // ����ߵ�ƽ
    GpioMuxRegs.GPBMUX.bit.C4TRIP_GPIOB13 = 0;                      // GPIOB13 = GPIO
    GpioMuxRegs.GPBDIR.bit.GPIOB13 = 1;                     // GPIOB13 = output

    EDIS;

    RST_BIT1;//�ر��������ʾ
    RST_BIT2;
    RST_BIT3;
    RST_BIT4;
}

static void Gpio_select(void)
{

    Uint16 var1;
    Uint16 var2;
    Uint16 var3;

    var1= 0x0000;       // sets GPIO Muxs as I/Os
    var2= 0xFFFF;       // sets GPIO DIR as outputs
    var3= 0x0000;       // sets the Input qualifier values

    EALLOW;

    GpioMuxRegs.GPAMUX.all=var1;
//    GpioMuxRegs.GPBMUX.all=var1;
    GpioMuxRegs.GPDMUX.all=var1;
    GpioMuxRegs.GPFMUX.all=var1;
    GpioMuxRegs.GPEMUX.all=var1;
    GpioMuxRegs.GPGMUX.all=var1;

    GpioMuxRegs.GPADIR.all=var2;        // GPIO PORTs  as output
//    GpioMuxRegs.GPBDIR.all=0xFFDF;          // GPIO��B��ȫ������Ϊ�������GPIOB5����Ϊ����
    GpioMuxRegs.GPDDIR.all=var2;
    GpioMuxRegs.GPEDIR.all=var2;
    GpioMuxRegs.GPFDIR.all=var2;
    GpioMuxRegs.GPGDIR.all=var2;

    GpioMuxRegs.GPAQUAL.all=var3;       // Set GPIO input qualifier values
//    GpioMuxRegs.GPBQUAL.all=var3;
    GpioMuxRegs.GPDQUAL.all=var3;
    GpioMuxRegs.GPEQUAL.all=var3;

    EDIS;

}
/*****************************************************************************************************/


/******************************�����λѡ�������ӵ�λ����λɨ�裩***************************************************/
static void Sellect_Bit(Uint16 i)
{
    switch(i)
    {
        case 0:
            RST_BIT4;                                   //�ض�����ܵ���λ
            SET_BIT1;                                   //ѡͨ����ܵ�һλ
            break;

        case 1:
            RST_BIT1;                                   //�ض�����ܵ�һλ
            SET_BIT2;                                   //ѡͨ����ܵڶ�λ
            break;

        case 2:
            RST_BIT2;                                   //�ض�����ܵڶ�λ
            SET_BIT3;                                   //ѡͨ����ܵ���λ
            break;

        case 3:
            RST_BIT3;                                   //�ض�����ܵ���λ
            SET_BIT4;                                   //ѡͨ����ܵ���λ
            break;

        default:
            break;
    }
}
/*****************************************************************************************************/

/************************** ���Ҫ��ʾ����λ�����浽����DisData_Trans����*****************************/
static void DisData_Trans(Uint16 data)
{
    DisData_Bit[3] = data / 1000;                       //ǧλ��
    DisData_Bit[2] = data % 1000 / 100 ;                //��λ��
    DisData_Bit[1] = data % 100 / 10;                   //ʮλ��
    DisData_Bit[0] = data % 10;                         //��λ��
}
/*****************************************************************************************************/



/*********************************************��ʱ����************************************************/
static void delay(Uint16 t)
{
    Uint16 i = 0;
    for (i = 0; i < t; i++);
}
/*****************************************************************************************************/

/*********************************************Spi��ʼ��************************************************/

static void spi_init()
{
    SpiaRegs.SPICCR.all =0x004F;                            // Reset on, rising edge, 16-bit char bits
                                                            //0x000F��ӦRising Edge��0x004F��ӦFalling Edge
    SpiaRegs.SPICTL.all =0x0006;                            // Enable master mode, normal phase,
                                                            // enable talk, and SPI int disabled.
    SpiaRegs.SPIBRR =0x007F;
    SpiaRegs.SPICCR.all =0x00DF;                            // Relinquish SPI from Reset
    SpiaRegs.SPIPRI.bit.FREE = 1;                           // Set so breakpoints don't disturb xmission
}
/*****************************************************************************************************/

/****************************************Spiģ��FIFO����**********************************************/
static void spi_fifo_init()
{
//  ��ʼ�� SPI FIFO �Ĵ���
    SpiaRegs.SPIFFTX.all=0xE040;
    SpiaRegs.SPIFFRX.all=0x204f;
    SpiaRegs.SPIFFCT.all=0x0;
}

/*****************************************************************************************************/

/*********************************************Spi����************************************************/

static void spi_xmit(Uint16 a)
{
    SpiaRegs.SPITXBUF=a;
}
/*****************************************************************************************************/
//�ӳٺ���

static void delay_loop()
{
    long      i;
    for (i = 0; i < 4500000; i++) {}
}


void demo8(void)
{
   Uint16 i;
   Uint16 j;
   Uint16 k;
   Uint32 Sum=0;
   Uint32 Vin;


// ���� 1. ��ʼ��ϵͳ����:
// ����PLL, WatchDog, ʹ������ʱ��
// ��������������Դ�DSP281x_SysCtrl.c�ļ����ҵ�.
   InitSysCtrl();

// ���� 2. ��ʼ��ͨ�����������·������GPIO:
// ���������DSP281x_Gpio.cԴ�ļ��б�������
// InitGpio();  // ����ֱ�������ò���
// ��������ӦGPIOΪΪSPI��������

// �����ض���ʱ������
   EALLOW;
   SysCtrlRegs.HISPCP.all = ADC_MODCLK; // HSPCLK = SYSCLKOUT/ADC_MODCLK
   EDIS;



   //��ʼ��SPI
   EALLOW;
   GpioMuxRegs.GPFMUX.all=0x000F;   // ѡ��GPIOΪSPI����
                                    // �˿�F MUX - x000 0000 0000 1111
   EDIS;

//   Gpio_select();
   Init_LEDS_Gpio();


// ���� 3. ��������ж�,��ʼ���ж�������:
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


// ���� 4.��ʼ��Ƭ������:
   InitAdc();  // For this example, init the ADC

   spi_fifo_init();   // ��ʼ��Spi FIFO
   spi_init();        // ��ʼ�� SPI

   // ������Ҫ����ADC����
   AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;
   AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;  // ADC ģ��ʱ�� = HSPCLK/1      = 25MHz/(1)     = 25MHz
   AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;        // 1  ����ģʽ
   AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;   //ADCͨ�� ��˳�� �� ����ͨ��ADCIN0
   AdcRegs.ADCCHSELSEQ1.bit.CONV01 = stuNum;   //ADCͨ�� ��˳�� ���� ����ͨ��ADCINx, ����xָ����ѧ��ĩβ��
   AdcRegs.ADCTRL1.bit.CONT_RUN = 1;       // ����Ϊ��������
   AdcRegs.ADCMAXCONV.all = 0x0001;         // ������ AdcRegs.ADCMAXCONV.all+1 ��

// Step 5.�û�ָ�����룬ʹ���ж�:

    GpioDataRegs.GPADAT.all    =0x0000;//GPIOA0-A7�����0��ʹLED1����
//    GpioDataRegs.GPBDAT.all   =0xFFFF;
//    GpioDataRegs.GPBDAT.all   &=0xFFFE;//GPIOB0�����0�������ø�


// ��������0
   for (i=0; i<BUF_SIZE; i++)
   {
     SampleTable0[i] = 0;
     SampleTable1[i] = 0;
   }
   //������ܣ�
   spi_xmit(0xFFFF);
     //�ӳ�
    delay(2500);
//      delay_loop();

   // �������SEQ1,SEQ2
   AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;
   AdcRegs.ADCTRL2.bit.SOC_SEQ2 = 1;

   // ȡADC���ݲ�д��������ݱ�
   for(;;)
   {

     for (i=0; i<AVG; i++)
     {
//        while (AdcRegs.ADCST.bit.INT_SEQ1== 0) {} // �ȴ��ж�
//        AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
//        SampleTable[i] =((AdcRegs.ADCRESULT0>>4) );

        while (AdcRegs.ADCST.bit.INT_SEQ1== 0 ) {} // �ȴ��ж�
        AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
        SampleTable0[i] =((AdcRegs.ADCRESULT0>>4) ); // AdcRegs.ADCCHSELSEQ1.bit.CONV00�Ľ��
        SampleTable1[i] =((AdcRegs.ADCRESULT1>>4) ); // AdcRegs.ADCCHSELSEQ1.bit.CONV01�Ľ��
     }


     for (i=0;i<2;++i){
        Sum = 0;
        for (j=0;j<AVG;j++)
        {
            Sum+=SampleTable[i][j];
        }
        Sum=Sum/AVG;
        //�����ѹ��ADֵ֮��Ĺ�ϵVin/Sum=3/4096��
        Vin=Sum*3*10000/4096;      //�������ѹ�Ŵ�100�����Ա��ڵ�2λ��ЧС��������������㣻
        if(Vin%10>=5)//���һλ����>=5ʱ��Ҫ���룻
           showdata=Vin/10+1;
        else
           showdata=Vin/10;//Ҫ���᣻

        if (i == 1){
            if ( showdata > 1500){
                 GpioDataRegs.GPADAT.bit.GPIOA1   =1; // ����D9
            }else{
                GpioDataRegs.GPADAT.bit.GPIOA1    =0; //GPIOA0-A7�����0��ʹLED1����
            }
        }
        if (i==0)
        {
            if ( showdata > 1500){
                GpioDataRegs.GPADAT.bit.GPIOA0    =1; // ����D8
            }else{
                GpioDataRegs.GPADAT.bit.GPIOA0    =0; //GPIOA0-A7�����0��ʹLED1����
}
            for(k=0;k<100;k++)
                    {

                        DisData_Trans(showdata);                                    //�����λ��
                        for(Loop=0;Loop<4;Loop++)                               //�ֱ���ʾ��λ
                        {
                            Sellect_Bit(Loop);                                  //ѡ��Ҫɨ��������λ
                            if(Loop==3)
                                spi_xmit(msg[DisData_Bit[Loop]]+0x80);
                            else
                            spi_xmit(msg[DisData_Bit[Loop]]);                   //�������Ҫ��ʾ������
                            delay(2500);                                       //��ʱ������۷�Ӧʱ��
                        }


                    }
            }
     }

   }
}



//===========================================================================
// No more.
//===========================================================================

