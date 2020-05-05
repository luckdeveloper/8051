#ifndef KEYBOARD_H__
#define KEYBOARD_H__

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

#endif// KEYBOARD_H__
