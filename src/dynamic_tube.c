//
// The tube was connected to P0 at the develop board.
// common cathode 
// use 74HC138 Line Decoder


#include "platform.h"
#include "tube.h"

//============timer0 int =====================//
#define OSC_FREQ	12000000UL

#define TIMER_MODE0		0x00
#define TIMER_MODE1		0x01
#define TIMER_MODE2		0x02
#define TIMER_MODE3		0x03





//===========================================
unsigned char g_tube_code_table[] = {
    0x3f, 0x06, 0x5b, 0x4f, // 0, 1, 2, 3
    0x66, 0x6d, 0x7d, 0x07, // 4, 5, 6, 7
    0x7f, 0x6f, 0x77, 0x7c, // 8, 9, a, b
    0x39, 0x5e, 0x79, 0x71, // c, d, e, f
};

#define TUBE_CODE_TABLE_COUNT (sizeof(g_tube_code_table)/sizeof(g_tube_code_table[0]))

// 74HC138 line decoder define
unsigned char g_line_decorder[][3] = {
    {0, 0, 0}, // 0
    {1, 0, 0}, // 1
    {0, 1, 0}, // 2
    {1, 1, 0}, // 3
    {0, 0, 1}, // 4
    {1, 0, 1}, // 5
    {0, 1, 1}, // 6
    {1, 1, 1}  // 7
};
#define LINE_COUNT (sizeof(g_line_decorder)/sizeof(g_line_decorder[0]))


int g_number_index = 0;
int g_line_deocorder_index = 0;

//==============forward declartion============
void timer0_init(void);
void change_tube(void);
void change_number(void);
void timer0_int_1ms();
void revert_led();

void main()
{
    timer0_init();
    //P0 = g_tube_code_table[3];

    for (;;)
    {
        
    }
}



void change_tube()
{
    lsa = g_line_decorder[g_line_deocorder_index][0];
    lsb = g_line_decorder[g_line_deocorder_index][1];
    lsc = g_line_decorder[g_line_deocorder_index][2];

    // move to next tube
    g_line_deocorder_index = (g_line_deocorder_index+1) % LINE_COUNT;
}

void change_number()
{
    GPIO_DIGTAL_TUBE = g_tube_code_table[g_number_index];

    //move to next number
    g_number_index = (g_number_index + 1) % TUBE_CODE_TABLE_COUNT;
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

/**
 * init timer
 * @author qiufengli (2020/4/15)
 * 
 * @param void 
 */
void timer0_init(void)
{
    TMOD &= 0xF0;
    TMOD |= TIMER_MODE1; /* Timer 0 work on 16-bit timer mode */

    // 1000us
    TH0 = (65536 - 1000 * (OSC_FREQ / 12000000)) >> 8;
    TL0 = (65536 - 1000 * (OSC_FREQ / 12000000)) & 0x00FF;

    ET0 = 1;        // open timer 0 interrupt
    EA = 1;         // enable CPU interrupt

    TR0 = 1;        /* Turn on timer 0 */
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
        change_tube();
        change_number();
        revert_led();
    }

}


#if defined SDCC || defined __SDCC
void timer0_int() __interrupt (1)
#else
void timer0_int() interrupt 1
#endif
{
    timer0_int_1ms();
}

