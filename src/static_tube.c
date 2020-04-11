//
// The tube was connected to P0 at the develop board.
// common cathode 
// use 74HC138 Line Decoder


#include "platform.h"

#ifdef KEIL
sbit lsa = P2 ^ 2;
sbit lsb = P2 ^ 3;
sbit lsc = P2 ^ 4; 
#else
    #define lsa P2_2
    #define lsb P2_3
    #define lsc P2_4
#endif


unsigned char tube_code_table[] = {
    0x3f, 0x06, 0x5b, 0x4f, // 0, 1, 2, 3
    0x66, 0x6d, 0x7d, 0x07, // 4, 5, 6, 7
    0x7f, 0x6f, 0x77, 0x7c, // 8, 9, a, b
    0x39, 0x5e, 0x79, 0x71, // c, d, e, f
    0x00                    // null
};


void main()
{
    // set 74hc138 Line Decoder to zero
    lsa = 0;
    lsb = 0;
    lsc = 0;

    P0 = tube_code_table[0];

    for (;;)
    {
        
    }
}


