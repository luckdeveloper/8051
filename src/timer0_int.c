#include "platform.h"

//#define LARGE_INT

#define OSC_FREQ	12000000UL

#define TIMER_MODE0		0x00
#define TIMER_MODE1		0x01
#define TIMER_MODE2		0x02
#define TIMER_MODE3		0x03

/**
 *  timer0 definition 
 *  
 */

#define MAX_TIMER0_MS (50)

typedef void (*TIMER0_INT_CB)(void);

typedef struct stTimer0
{
    unsigned int delay_in_ms;
    TIMER0_INT_CB cb;
}TIMER0_INT_S;
int timer0_int_init(unsigned int delay_in_ms, TIMER0_INT_CB cb);
void timer0_int_reset();

enum eTimer0RetValue
{
    RET_SUCCESS = 0,
    RET_EXCEED_RANGE = 1,
};
/**
 *  
 *  global variable
 */
#if defined SDCC || defined __SDCC
    #define led P2_0
#else
    sbit led = P2 ^ 0; // led connected with P2
#endif

//


// forward declaration
void timer0_int_cb();

static TIMER0_INT_S g_stTimer0Int;

/**
 * init timer0 int 
 * 
 * @author qiufengli (2020/4/15)
 * 
 * @param delay_in_ms 
 * @param cb 
 * 
 * @return int 
 */
int timer0_int_init(unsigned int delay_in_ms, TIMER0_INT_CB cb)
{
    if (delay_in_ms > MAX_TIMER0_MS)
    {
        return RET_EXCEED_RANGE;
    }
    g_stTimer0Int.delay_in_ms = delay_in_ms;
    g_stTimer0Int.cb = cb;

    TMOD &= 0xF0;
    TMOD |= TIMER_MODE1; /* Timer 0 work on 16-bit timer mode */

    // delay_in_ms* 1000us
    TH0 = (65536 - delay_in_ms*1000 * (OSC_FREQ / 12000000)) >> 8;
    TL0 = (65536 - delay_in_ms*1000 * (OSC_FREQ / 12000000)) & 0x00FF;

    ET0 = 1;        // open timer 0 interrupt
    EA = 1;         // enable CPU interrupt

    TR0 = 1;        /* Turn on timer 0 */

    return RET_SUCCESS;
}

/**
 * You Must call this function in your callback
 * 
 * @author qiufengli (2020/4/15)
 */
void timer0_int_reset()
{
    // reconfigure 1ms
    TH0 = (65536 - g_stTimer0Int.delay_in_ms* 1000 * (OSC_FREQ / 12000000)) >> 8;
    TL0 = (65536 - g_stTimer0Int.delay_in_ms* 1000 * (OSC_FREQ / 12000000)) & 0x00FF;
}


void main()
{
    timer0_int_init(1, timer0_int_cb);
    for (;;)
    {}
}

void revert_led()
{
#if defined SDCC || defined __SDCC
    if (led == 0)
    {
        led = 1;
    }
    else
    {
        led = 0;
    }
#else
    led = ~led;
#endif
}

void timer0_int_cb()
{
    static unsigned int count = 0;

    timer0_int_reset();

    if (count++ >= 1000)
    {
        count = 0;
        revert_led();
    }

}


#if defined SDCC || defined __SDCC
void timer0_int() __interrupt (1)
#else
void timer0_int() interrupt 1
#endif
{
    if (g_stTimer0Int.cb)
    {
        g_stTimer0Int.cb();
    }
}
