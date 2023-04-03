#include "platform.h"

#if defined SDCC || defined __SDCC
    #define led P2_0
#else
    sbit led = P2 ^ 0; // led connected with P2
#endif


void delay(unsigned int i);
void delay_1000ms(); //@11.0592MHz

void main()
{
    for (;;)
    {
        led = 0;
        delay_1000ms();
        led = 1;
        delay_1000ms();
    }
}

void delay(unsigned int i)
{
    while (i--);
}

void delay_1000ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	//_nop_();
	i = 8;
	j = 1;
	k = 243;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

