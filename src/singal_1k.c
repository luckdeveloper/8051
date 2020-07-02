#include "platform.h"

// in develop board, 8 leds are connect to P2, from P2^0 to P2^7
#if defined SDCC || defined __SDCC
    #define signal_out P2_0
#else
    sbit signal_out = P2 ^ 0; // led connected with P2
#endif


#define OSC_FREQ	12000000UL
#define DELAY_US        1000


#define TIMER_MODE0		0x00
#define TIMER_MODE1		0x01
#define TIMER_MODE2		0x02
#define TIMER_MODE3		0x03

void timer0_init_16();
void timer0_delay_ms(unsigned int);
void timer0_delay_500us(unsigned int);
void timer0_start();
void timer0_stop();


void revert_signal()
{
#if defined SDCC || defined __SDCC
    if (signal_out == 0)
    {
        signal_out = 1;
    }
    else
    {
        signal_out = 0;
    }
#else
    signal_out = ~signal_out;
#endif
}

void main()
{
    int i = 0;

    timer0_init_16();
    timer0_start();

    while (1)
    {
        revert_signal();
        timer0_delay_500us(1);
    }
}

///////////////////////////////////////////////////// 
/// 
/// 

#define TIMER0_FLAG (TF0)


/**
 * init timer0 with 16 bit mode 
 * with 1 us delay 
 * 
 * @author qiufengli (2020/4/15)
 * 
 * @param void 
 */
void timer0_init_16(void)
{
    TMOD &= 0xF0;
    TMOD |= TIMER_MODE1; /* Timer 0 work on 16-bit timer mode */
    TH0 = (65536 - DELAY_US * (OSC_FREQ / 12000000)) >> 8;
    TL0 = (65536 - DELAY_US * (OSC_FREQ / 12000000)) & 0x00FF;
}

/**
 * start timer0
 * 
 * @author qiufengli (2020/4/15)
 */
void timer0_start()
{
    TR0 = 1;        /* Turn on timer 0 */
}

/**
 * stop timer0
 * 
 * @author qiufengli (2020/4/15)
 */
void timer0_stop()
{
    TR0 = 0;
}

/**
 * timer0 delay in millseconds
 * 
 * @author qiufengli (2020/4/15)
 * 
 * @param n 
 */
void timer0_delay_ms(unsigned int n)
{
    while (n--)
    {
        while (!TF0);

        // reset timer
        TF0 = 0;
        TH0 = (65536 - DELAY_US * (OSC_FREQ / 12000000)) >> 8;
        TL0 = (65536 - DELAY_US * (OSC_FREQ / 12000000)) & 0x00FF;
    }
}

/**
 * timer0 delay in millseconds
 * 
 * @author qiufengli (2020/4/15)
 * 
 * @param n 
 */
void timer0_delay_500us(unsigned int n)
{
    while (n--)
    {
        while (!TF0);

        // reset timer
        TF0 = 0;
        TH0 = (65536 - 500 * (OSC_FREQ / 12000000)) >> 8;
        TL0 = (65536 - 500 * (OSC_FREQ / 12000000)) & 0x00FF;
    }
}
