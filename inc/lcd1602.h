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
sbit LCD1602_RS = P2^6;
sbit LCD1602_RW = P2^5;
sbit LCD1602_EN = P2^7;
#define LCD1602_DB      P0
#endif


/**
 * LCD1602 control command 
 */
#define LCD1602_CMD_CLEAR       0x01    // clear display
#define LCD1602_CMD_RESET_CURSOR 0x02   // reset cursor to left corner, 
                                        // and keep data                                       
#define LCD1602_CMD_MODE1602    0x38    // 16 cols *2 rows, 
                                        // 5*7 lattice, 
                                        // 8bit data bus len

#define LCD1602_CMD_MODE_MOVE_CURSOR            0x06    // move cursor when new data is written
#define LCD1602_CMD_OPEN_WITHOUT_CURSOR         0x08    // turn on without cursour
#define LCD1602_CMD_OPEN_WITH_CURSOR            0x0f    // turn on with blink cursor   


/***************************************************************************************************
                             Commonly used LCD macros/Constants
***************************************************************************************************/
#define C_DefaultDigits 	10

#define C_GlcdFirstLine_U8 0x00u
#define C_GlcdLastLine_U8  0x07u

#define C_FirstLineAddress_U8 0xB8
#define C_LastLineAddress_U8  0xBF

#define C_DisplayDefaultDigits_U8            0xffu // Will display the exact digits in the number
#define C_MaxDigitsToDisplay_U8              10u   // Max decimal/hexadecimal digits to be displayed
#define C_NumOfBinDigitsToDisplay_U8         16u   // Max bits of a binary number to be displayed
#define C_MaxDigitsToDisplayUsingPrintf_U8   C_DisplayDefaultDigits_U8 /* Max dec/hexadecimal digits to be displayed using printf */

#define C_MaxBarGraphs_U8     4

#define BUSY_FLAG     7
/**************************************************************************************************/
#endif // LCD1602_H__
