#include "platform.h"

#if defined SDCC || defined __SDCC
    #define led P2_0
#else
    sbit led = P2 ^ 0; // led connected with P2
#endif


void delay(unsigned int i);

void main()
{
    for (;;)
    {
        led = 0;
        delay(500);
        led = 1;
        delay(500);
    }
}

void delay(unsigned int i)
{
    while (i--);
}
