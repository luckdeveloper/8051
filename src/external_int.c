#include "platform.h"

#if defined SDCC || defined __SDCC
    #define led 	P2_0
    #define key_3 	P3_2 
#else
    sbit led 	= P2^0; // led connected with P2
    sbit key_3 	= P3^2;	// key3 connected with P3^2(INT0)
#endif


void delay(unsigned int i);
void init_int0(void);

void main()
{	 
    init_int0();	
    for (;;)
    {
    	
    }
}

void init_int0()
{
   INT0 = 1; // Edge Triggering
   // INT0 = 0; // Level Triggering

    EX0 = 1;    // enable external INT0 interrupt 
    EA = 1;	// enable CPU interrupt
}

void delay(unsigned int i)
{ 
    while (i--);
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


#if defined SDCC || defined __SDCC
void int0() __interrupt (0)
#else
void int0() interrupt 0
#endif
{
    delay(10000); // software de-jitter filter

    if (key_3 == 0)	 // key is pressed
    {
        revert_led();
    }
}