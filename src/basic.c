#include "platform.h"

#ifdef KEIL
sbit led = P2 ^ 0; // led connected with P2
#else
    #define led P2_0
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
