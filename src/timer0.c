#include "platform.h"

// in develop board, 8 leds are connect to P2, from P2^0 to P2^7
#ifdef KEIL
    sbit led = P2 ^ 0; // led connected with P2
#else
    #define led P2_0
#endif

#define OSC_FREQ	12000000UL

#define TIMER_MODE0		0x00
#define TIMER_MODE1		0x01
#define TIMER_MODE2		0x02
#define TIMER_MODE3		0x03

void timer0_init(void)
{
    TMOD &= 0xF0;
    TMOD |= TIMER_MODE1; /* Timer 0 work on 16-bit timer mode */
    TH0 = (65536 - 1000 * (OSC_FREQ / 12000000)) >> 8;
    TL0 = (65536 - 1000 * (OSC_FREQ / 12000000)) & 0x00FF;
    TR0 = 1;        /* Turn on timer 0 */
}

void delay_ms(unsigned int n)
{
    while (n--)
    {
        while (!TF0);

        // reset timer to 1 ms
        TF0 = 0;
        TH0 = (65536 - 1000 * (OSC_FREQ / 12000000)) >> 8;
        TL0 = (65536 - 1000 * (OSC_FREQ / 12000000)) & 0x00FF;
    }
}

void revert_led()
{
#ifdef KELI
    led = ~led;
#else
    if (led == 0)
    {
        led = 1;
    }
    else
    {
        led = 0;
    }
#endif
}

void main()
{
    int i = 0;

    timer0_init();

    while (1)
    {
        // flash led 0
        revert_led();
        delay_ms(1000);
    }
}

