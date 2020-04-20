#ifndef DIGTAL_TUBE_H__
#define DIGTAL_TUBE_H__
//==============================================//

#define GPIO_DIGTAL_TUBE P0 // the tube was connected to PO

#if defined SDCC || defined __SDCC
    #define lsa P2_2
    #define lsb P2_3
    #define lsc P2_4
    #define led P2_0
#else
    // 74HC138 line decoder 
    sbit lsa = P2 ^ 2;
    sbit lsb = P2 ^ 3;
    sbit lsc = P2 ^ 4; 
    // led connected with P2
    sbit led = P2 ^ 0; 

#endif

#endif // DIGTAL_TUBE_H__
