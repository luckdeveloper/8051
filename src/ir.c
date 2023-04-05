
#include <stdarg.h>

#include "platform.h"
#include "lcd1602.h"
#include "stdutils.h"

#if defined SDCC || defined __SDCC
    #define ir_input P3_0
#else
    sbit ir_input = P3 ^ 0; // led connected with P3
#endif

uint32_t bitPattern = 0,newKey = 0;
unsigned char timerValue;
unsigned char msCount = 0;
int pulseCount = 0;


#if defined SDCC || defined __SDCC
void timer0_isr() __interrupt (1)
#else
void timer0_isr() interrupt 1
#endif
{
    if (msCount < 50)
        msCount++;

    TH0 = 0xFC;   // Reload the timer value for 1ms Delay
    TL0 = 0x67;
}

#if defined SDCC || defined __SDCC
void externalIntr0_ISR() __interrupt (0)
#else
void externalIntr0_ISR() interrupt 0
#endif
{
    timerValue = msCount;
    msCount = 0;

    TH0 = 0xFC;  // Reload the timer value for 1ms Delay
    TL0 = 0x67;

    pulseCount++;

    if ((timerValue >= 50)) // If the pulse width is greater than 50ms, this will mark the SOF
    { 
        pulseCount = -2; // First 2 counts needs to be skipped hence pulse count is set to -2 
        bitPattern = 0;
    }
    else if ((pulseCount>=0) && (pulseCount<32)) //Accumulate the bit values between 0-31.
    {  
        if (timerValue>=2)                      //pulse width greater than 2ms is considered as LOGIC1
        {
            bitPattern |=(unsigned long)1<<(31-pulseCount);
        }
        else
        {

        }
    }
    else if (pulseCount>=32)                  //This will mark the End of frame as 32 pulses are received
    {
        newKey = bitPattern;                // Copy the newKey(patter) and set the pulse count to 0;
        pulseCount = 0;
    }
}


/////////////////////////////////////////////////////////////////////////////// 
/// lcd driver 
void lcd1602_cmd(unsigned char cmd);
void lcd1602_display_char(unsigned char ch);
void lcd1602_display_string(char *str);

/**
 * init 1602 
 * 
 * @author qiufengli (2020/4/16)
 */
void lcd1602_init()
{              
    lcd1602_cmd(LCD1602_CMD_MODE1602);
    lcd1602_cmd(LCD1602_CMD_OPEN_WITH_CURSOR);
    lcd1602_cmd(LCD1602_CMD_MODE_MOVE_CURSOR);
}

/**
 * delay us 
 * 
 * @author qiufengli (2020/4/16)
 * 
 * @param i 
 */
void delay_us(int i)
{
    while (i--)
        ;
}

/**
 * write command to lcd 
 * step1: Send the character to LCD. 
 * step2: Select the Control Register by making RS low. 
 * step3: Select Write operation making RW low. 
 * step4: Send a High-to-Low pulse on Enable PIN with some 
 * delay_us . 
 * 
 * @author qiufengli (2020/4/16)
 * 
 * @param cmd 
 */
void lcd1602_cmd(unsigned char cmd)
{
    LCD1602_DB = cmd;
    LCD1602_RS = 0;
    LCD1602_RW = 0;

    // generate a high-to-low pulse
    LCD1602_EN = 1;
    delay_us(10);
    LCD1602_EN = 0;
    delay_us(1000);

}

/**
 * display a char 
 * step1: Send the character to LCD. 
 * step2: Select the Data Register by making RS high. 
 * step3: Select Write operation making RW low. 
 * step4: Send a High-to-Low pulse on Enable PIN with some 
 * delay_us . 
 * @author qiufengli (2020/4/16)
 * 
 * @param ch 
 */
void lcd1602_display_char(unsigned char ch)
{
    LCD1602_DB = ch;
    LCD1602_RS = 1;
    LCD1602_RW = 0;

    // generate a high-to-low pulse
    LCD1602_EN = 1;
    delay_us(10);
    LCD1602_EN = 0;
    delay_us(1000);
}

/**
 * clear display, and move cursor to the left corner
 * 
 * @author qiufengli (2020/4/16)
 */
void lcd1602_clear()
{
    lcd1602_cmd(LCD1602_CMD_CLEAR);
}

/**
 * send string to lcd 
 * 
 * @author qiufl (4/5/2023)
 * 
 * @param str    
 */
void lcd1602_display_string(char *str)
{
	while (*str != '\0')
	{
		lcd1602_display_char(*str);
		str++;
	}
}



/***************************************************************************************************
void LCD_DisplayNumber(uint8_t v_numericSystem_u8, uint32_t v_number_u32, uint8_t v_numOfDigitsToDisplay_u8)
****************************************************************************************************
 * Function name:  LCD_DisplayNumber()
 * I/P Arguments: 
                  uint8_t :  specifies type of number C_BINARY_U8(2),C_DECIMAL_U8(10), C_HEX_U8(16)
                  uint32_t: Number to be displayed on the LCD.
                  uint8_t : Number of digits to be displayed
                  
 * Return value    : none

 * description  :This function is used to display a max of 10digit decimal/Hex number OR specified 
                 number of bits for binary number.
                
                1st parameter specifies type of number C_BINARY_U8(2),C_DECIMAL_U8(10), C_HEX_U8(16)                 
                3rd parameter specifies the number of digits from the right side to be displayed
                 The output for the input combinations is as below
               
    Binary:     1.(2,10,4) then 4-LSB will be displayed ie. 1010
                2.(C_BINARY_U8,10,8) then 8-LSB will be displayed ie. 00001010
                3.(C_BINARY_U8,10,2) then 2-LSB will be displayed ie. 10
                
    Decimal:    4.(10,12345,4) then 4-digits ie. 2345 will be displayed
                5.(C_DECIMAL_U8,12345,6) then 6-digits ie. 012345 will be displayed
                6.(10,12345,C_DisplayDefaultDigits_U8) then 12345 will be displayed.\
                
    Hex:        7.(16,0x12AB,3) then 3-digits ie. 2AB will be displayed
                8.(C_HEX_U8,0x12AB,6) then 6-digits ie. 0012AB will be displayed
                9.(C_HEX_U8,0x12AB,C_DisplayDefaultDigits_U8) then 12AB will be displayed.                
****************************************************************************************************/
void lcd1602_display_number(uint8_t v_numericSystem_u8, uint32_t v_number_u32, uint8_t v_numOfDigitsToDisplay_u8)
{
    uint8_t i=0,a[10];
    
    if(C_BINARY_U8 == v_numericSystem_u8)
    {
        while(v_numOfDigitsToDisplay_u8!=0)
        {
          /* Start Extracting the bits from the specified bit positions.
          Get the Acsii values of the bits and display */
          i = util_GetBitStatus(v_number_u32,(v_numOfDigitsToDisplay_u8-1));
          lcd1602_display_char(util_Dec2Ascii(i));
          v_numOfDigitsToDisplay_u8--;
        }        
    }    
    else if(v_number_u32==0)
    {
        /* If the number is zero then update the array with the same for displaying */
        for(i=0;((i<v_numOfDigitsToDisplay_u8) && (i<C_MaxDigitsToDisplay_U8));i++)
            lcd1602_display_char('0');
    }
    else
    {
        for(i=0;i<v_numOfDigitsToDisplay_u8;i++)
        {
            /* Continue extracting the digits from right side
               till the Specified v_numOfDigitsToDisplay_u8 */
            if(v_number_u32!=0)
            {
                /* Extract the digits from the number till it becomes zero.
                First get the remainder and divide the number by TypeOfNum(10-Dec, 16-Hex) each time.
                
                example for Decimal number: 
                If v_number_u32 = 123 then extracted remainder will be 3 and number will be 12.
                The process continues till it becomes zero or max digits reached*/
                a[i]=util_GetMod32(v_number_u32,v_numericSystem_u8);
                v_number_u32=v_number_u32/v_numericSystem_u8;
            }
            else if( (v_numOfDigitsToDisplay_u8 == C_DisplayDefaultDigits_U8) ||
                     (v_numOfDigitsToDisplay_u8 > C_MaxDigitsToDisplay_U8))
            {
                /* Stop the iteration if the Max number of digits are reached or 
                 the user expects exact(Default) digits in the number to be displayed */ 
                break;
            }
            else
            {
                /* In case user expects more digits to be displayed than the actual digits in number,
                  then update the remaining digits with zero.
               Ex: v_num_u32 is 123 and user wants five digits then 00123 has to be displayed */
                a[i]=0;
            }
        }
        
         while(i!=0)
        { 
          /* Finally get the ascii values of the digits and display*/
          lcd1602_display_char(util_Hex2Ascii(a[i-1]));
          i--;
        }
    }
}


void lcd1602_printf(const char *argList, ...)
{
    const char *ptr;
    va_list argp;
    sint16_t v_num_s16;
    sint32_t v_num_s32;
    uint16_t v_num_u16;
    uint32_t v_num_u32;
    char *str;
    char  ch;
    uint8_t v_numOfDigitsToDisp_u8;
#if (Enable_LCD_DisplayFloatNumber == 1)  
    double v_floatNum_f32;
#endif

    va_start(argp, argList);

    /* Loop through the list to extract all the input arguments */
    for(ptr = argList; *ptr != '\0'; ptr++)
    {

        ch= *ptr;
        if(ch == '%')         /*Check for '%' as there will be format specifier after it */
        {
            ptr++;
            ch = *ptr;
           if((ch>=0x30) && (ch<=0x39))
            {
               v_numOfDigitsToDisp_u8 = 0;
               while((ch>=0x30) && (ch<=0x39))
                {
                   v_numOfDigitsToDisp_u8 = (v_numOfDigitsToDisp_u8 * 10) + (ch-0x30);
                   ptr++;
                   ch = *ptr;
                }
            }
            else
            {
              v_numOfDigitsToDisp_u8 = C_MaxDigitsToDisplayUsingPrintf_U8;
            }                


            switch(ch)       /* Decode the type of the argument */
            {
            case 'C':
            case 'c':     /* Argument type is of char, hence read char data from the argp */
                ch = va_arg(argp, int);
                lcd1602_display_char(ch);
                break;

            case 'd':    /* Argument type is of signed integer, hence read 16bit data from the argp */
                v_num_s16 = va_arg(argp, int);
                if(v_num_s16<0)
                 { /* If the number is -ve then display the 2's complement along with '-' sign */ 
                   v_num_s16 = -v_num_s16;
                   lcd1602_display_char('-');
                 }
                lcd1602_display_number(C_DECIMAL_U8,v_num_s16,v_numOfDigitsToDisp_u8);
                break;
                
            case 'D':    /* Argument type is of integer, hence read 16bit data from the argp */
                v_num_s32 = va_arg(argp, sint32_t);
                if(v_num_s32<0)
                 { /* If the number is -ve then display the 2's complement along with '-' sign */
                   v_num_s32 = -v_num_s32;
                   lcd1602_display_char('-');
                 }
                lcd1602_display_number(C_DECIMAL_U8,v_num_s32,v_numOfDigitsToDisp_u8);              
                break;    

            case 'u':    /* Argument type is of unsigned integer, hence read 16bit unsigned data */
                v_num_u16 = va_arg(argp, int);
                lcd1602_display_number(C_DECIMAL_U8,v_num_u16,v_numOfDigitsToDisp_u8);
                break;
            
            case 'U':    /* Argument type is of integer, hence read 32bit unsigend data */
                v_num_u32 = va_arg(argp, uint32_t);
                lcd1602_display_number(C_DECIMAL_U8,v_num_u32,v_numOfDigitsToDisp_u8);               
                break;            

            case 'x':  /* Argument type is of hex, hence hexadecimal data from the argp */
                v_num_u16 = va_arg(argp, int);
                lcd1602_display_number(C_HEX_U8,v_num_u16,v_numOfDigitsToDisp_u8);
                break;

            case 'X':  /* Argument type is of hex, hence hexadecimal data from the argp */
                v_num_u32 = va_arg(argp, uint32_t);
                lcd1602_display_number(C_HEX_U8,v_num_u32,v_numOfDigitsToDisp_u8);                
                break;

            
            case 'b':  /* Argument type is of binary,Read int and convert to binary */
                v_num_u16 = va_arg(argp, int);
                if(v_numOfDigitsToDisp_u8 == C_MaxDigitsToDisplayUsingPrintf_U8)
                   v_numOfDigitsToDisp_u8 = 16;
                lcd1602_display_number(C_BINARY_U8,v_num_u16,v_numOfDigitsToDisp_u8);                
                break;

            case 'B':  /* Argument type is of binary,Read int and convert to binary */
                v_num_u32 = va_arg(argp, uint32_t);
                if(v_numOfDigitsToDisp_u8 == C_MaxDigitsToDisplayUsingPrintf_U8)
                   v_numOfDigitsToDisp_u8 = 16;                
                lcd1602_display_number(C_BINARY_U8,v_num_u32,v_numOfDigitsToDisp_u8);                
                break;


            case 'F':
            case 'f': /* Argument type is of float, hence read double data from the argp */
#if (Enable_LCD_DisplayFloatNumber == 1)  
                v_floatNum_f32 = va_arg(argp, double);              
                LCD_DisplayFloatNumber(v_floatNum_f32);
#endif
                break;


            case 'S':
            case 's': /* Argument type is of string, hence get the pointer to sting passed */
                str = va_arg(argp, char *);
                lcd1602_display_string(str);
                break;

            case '%':
                lcd1602_display_char('%');
                break;
            }
        }
        else
        {
            /* As '%' is not detected display/transmit the char passed */
            lcd1602_display_char(ch);
        }
    }

    va_end(argp);
}
////////////////////////////////////////////////////////////////////////////////////

void main()
{
	lcd1602_init();

    TMOD |= 0x01; // Timer0 MODE1(16-bit timer)
    TH0 = 0xFC;   //Timer value for 1ms at 11.0592Mhz clock
    TL0 = 0x67;
    TR0 = 1;       // Start the Timer
    ET0 = 1;       // Enable the Timer0 Interrupt
    
    IT0 = 1;       // Configure INT0 falling edge interrupt
    EX0 = 1;       // Enable the INT0 External Interrupt
    
    EA  = 1;       // Enable the Global Interrupt bit

	lcd1602_display_string("Init Done");

    while(1)
    {
        if(newKey!=0)		 //Wait till newKey press is detected
        {  
            lcd1602_clear();	 //Clear the LCD and Display the new Key Code
            //lcd1602_displaynumber(C_HEX_U8, newKey, 2);
            lcd1602_printf("newKey:%X",newKey);
            newKey = 0;		 //CLear the newKey value for next cycle
        }
    }

}