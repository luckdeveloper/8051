#include "platform.h"
#include "lcd1602.h"


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

void lcd1602_init(void);
void lcd1602_cmd(unsigned char);
void lcd1602_putchar(unsigned char);
void lcd1602_clear(void);

// utilities function
void delay_us(unsigned int i);

void main()
{

    lcd1602_init();
    lcd1602_putchar('h');
    lcd1602_putchar('e');
    lcd1602_putchar('l');
    lcd1602_putchar('l');
    lcd1602_putchar('o');

    for (;;)
    {
    }
}


/////////////////////////////////////////////////////////////////////////////// 
/// lcd driver 

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
void delay_us(unsigned int i)
{
    while (i--);
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
void lcd1602_putchar(unsigned char ch)
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

