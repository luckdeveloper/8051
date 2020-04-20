#ifndef LCD1602_H__
#define LCD1602_H__


/**
 * LCD1602 port definition
 */
#if defined SDCC || defined __SDCC
#define LCD1602_RS      P2_6
#define LCD1602_RW      P2_5
#define LCD1602_EN      P2_7
#define LCD1602_DB      P0
#else
sbit LCD1602_RS = P2^4;
sbit LCD1602_RW = P2^5;
sbit LCD1602_EN = P2^6;
#define LCD1602_DB      P0
#endif

#endif // LCD1602_H__
