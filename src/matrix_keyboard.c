//
// The tube was connected to P0 at the develop board.
// common cathode
// use 74HC138 Line Decoder
// the matrix keyboard was connect from P10~P17, 
// all the negtive pin are connected from P10-P13, add the positive pin are connected from P14-P17
// if the key is down, the pins are set to low level



#include "platform.h"

#define GPIO_DIGITAL_TUBE   P0
#define GPIO_KEY            P1

#if defined SDCC || defined __SDCC
    #define lsa P2_2
    #define lsb P2_3
    #define lsc P2_4
#else
    sbit lsa = P2 ^ 2;
    sbit lsb = P2 ^ 3;
    sbit lsc = P2 ^ 4;
#endif


unsigned char tube_code_table[] = {
    0x3f, 0x06, 0x5b, 0x4f, // 0, 1, 2, 3
    0x66, 0x6d, 0x7d, 0x07, // 4, 5, 6, 7
    0x7f, 0x6f, 0x77, 0x7c, // 8, 9, a, b
    0x39, 0x5e, 0x79, 0x71, // c, d, e, f
    0x00                    // null
};

unsigned char g_key_down = 0;

// forward declartion
void key_down_scan();
void delay(unsigned int);


void main()
{
    // set 74hc138 Line Decoder to zero
    lsa = 0;
    lsb = 0;
    lsc = 0;

    for (;;)
    {
        key_down_scan();
        GPIO_DIGITAL_TUBE = tube_code_table[g_key_down];
    }
}

/**
 * When NO key is pressed, the port is in high level 
 * When key is pressed, the connected line is in low level
 * 
 * @author qiufengli (2020/4/13)
 */
void key_down_scan(void)
{
    char a = 0;
    unsigned char key;


    // 1. detective which column
    GPIO_KEY = 0x0f;

    if (GPIO_KEY != 0x0f) // detect which colomn
    {
        delay(1000); // software d-jitter buffer
        if (GPIO_KEY != 0x0f) // check again
        {
            GPIO_KEY = 0X0F;
            switch (GPIO_KEY)
            {
            case(0X07):
                key = 0; break;
            case(0X0b):
                key = 1; break;
            case(0X0d):
                key = 2; break;
            case(0X0e):
                key = 3; break;
            }
            // 2. detect which row
            GPIO_KEY = 0XF0;
            switch (GPIO_KEY)
            {
            case(0X70): // binary: 0111 0000
                key = key; break;
            case(0Xb0): // binary: 1011 0000
                key = key + 4; break;
            case(0Xd0): // 1101 0000
                key = key + 8; break;
            case(0Xe0): // 1110 0000
                key = key + 12; break;
            }

            // 1. unless when key is up 
            // 2. or press time hold for 500ms
            //
            while ((a < 50) && (GPIO_KEY != 0xf0)) 
            {
                delay(1000);
                a++;
            }

            g_key_down = key;
        }
    }
}

void delay(unsigned int i)
{
    while (i--)
    {
    }
}

