#include "platform.h"

//#define LARGE_INT

#define OSC_FREQ	12000000UL

#define TIMER_MODE0		0x00
#define TIMER_MODE1		0x01
#define TIMER_MODE2		0x02
#define TIMER_MODE3		0x03

#ifdef KEIL
sbit led = P2 ^ 0; // led connected with P2
#else
#define led P2_0
#endif

void timer0_init(void);

void main()
{
    timer0_init();
    for (;;)
    {}
}



void timer0_init(void)
{
    TMOD &= 0xF0;
    TMOD |= TIMER_MODE1; /* Timer 0 work on 16-bit timer mode */

#ifdef LARGE_INT
    // 65535us
    TH0 = (65536 - 65535 * (OSC_FREQ / 12000000)) >> 8;
    TL0 = (65536 - 65535 * (OSC_FREQ / 12000000)) & 0x00FF;
#else
    // 1000us
    TH0 = (65536 - 1000 * (OSC_FREQ / 12000000)) >> 8;
    TL0 = (65536 - 1000 * (OSC_FREQ / 12000000)) & 0x00FF;
#endif

    ET0 = 1;        // open timer 0 interrupt
    EA = 1;         // enable CPU interrupt

    TR0 = 1;        /* Turn on timer 0 */
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

void timer0_int_1ms()
{
    static unsigned int count = 0;

    count++;
    // reconfigure 1ms
    TH0 = (65536 - 1000 * (OSC_FREQ / 12000000)) >> 8;
    TL0 = (65536 - 1000 * (OSC_FREQ / 12000000)) & 0x00FF;
    if (count >= 1000)
    {
        count = 0;
        revert_led();
    }

}



void timer0_int_large()
{
    // reconfigure 65535us
    TH0 = (65536 - 65535 * (OSC_FREQ / 12000000)) >> 8;
    TL0 = (65536 - 65535 * (OSC_FREQ / 12000000)) & 0x00FF;
    revert_led();
}

#ifdef KEIL
void timer0_int() interrupt 1
#else
void timer0_int() __interrupt (1)
#endif
{
#ifdef LARGE_INT
    timer0_int_large();
#else
    timer0_int_1ms();
#endif


}
