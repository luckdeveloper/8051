#include "platform.h"
#include "keyboard.h"

/// 
/// 8051 doesn't have I2C controller, implement with software 
/// https://zh.wikipedia.org/wiki/I%C2%B2C 
///  
 


// Port Definitions

#define I2C_SCL P2_1
#define I2C_SDA P2_0

#define SDA I2C_SDA
#define SCL I2C_SCL

#define SDA_HIGH    (SDA = 1)
#define SDA_LOW     (SDA = 0)
#define SCL_HIGH    (SCL = 1)
#define SCL_LOW     (SCL = 0)

//============== part 1 ======================//
void i2c_start(void);
void i2c_stop(void);
void delay_us(int);
void i2c_sendbyte(unsigned char);
unsigned char i2c_recvbyte(__bit);

//============== part 2 ======================//
void at24c02_ack();
unsigned char at24c02_readbyte(unsigned char);
void at24c02_writebyte(unsigned char, unsigned char);

//============== part 3 =====================//
void timer0_init(void);
void change_tube(void);
void change_number(void);
void timer0_int_1ms();
void revert_led();


//===========================================
unsigned char g_tube_code_table[] = {
    0x3f, 0x06, 0x5b, 0x4f, // 0, 1, 2, 3
    0x66, 0x6d, 0x7d, 0x07, // 4, 5, 6, 7
    0x7f, 0x6f, 0x77, 0x7c, // 8, 9, a, b
    0x39, 0x5e, 0x79, 0x71, // c, d, e, f
};

#define TUBE_CODE_TABLE_COUNT (sizeof(g_tube_code_table)/sizeof(g_tube_code_table[0]))

// 74HC138 line decoder define
unsigned char g_line_decorder[][3] = {
    {0, 0, 0}, // 0
    {1, 0, 0}, // 1
    {0, 1, 0}, // 2
    {1, 1, 0}, // 3
    {0, 0, 1}, // 4
    {1, 0, 1}, // 5
    {0, 1, 1}, // 6
    {1, 1, 1}  // 7
};
#define LINE_COUNT (sizeof(g_line_decorder)/sizeof(g_line_decorder[0]))

char g_number_index = 0;
char g_line_deocorder_index = 0;


void main()
{

    // store g_number_index at address 0
    // store g_line_deocorder_index at address 64
//    g_number_index = at24c02_readbyte(0);
    g_line_deocorder_index = at24c02_readbyte(1);

    g_number_index = g_number_index % TUBE_CODE_TABLE_COUNT;
    g_line_deocorder_index = g_line_deocorder_index % LINE_COUNT;

    timer0_init();
    
    for (;;)
    {
    }
}


/////////////////////////////////////////////////////////////////////////////// 
/// 
///  part I
///  I2C library
/// 

/**
 * 
 * 
 * @author qiufengli (2020/4/26)
 */
void i2c_start()
{
#if 0
WAIT_BUS_READY:
    if (SDA == 0)
    {
        goto WAIT_BUS_READY;
    }

    if (SCL == 0)
    {
        goto WAIT_BUS_READY;
    }
#endif

    SCL = 1;
    SDA = 1;
    delay_us(2);
    SDA = 0;
}

/**
 * after the stop , SDA and SCL keep high level(idle)
 * 
 * @author qiufengli (2020/4/26)
 */
void i2c_stop()
{
    SDA = 0;
    delay_us(2);
    SCL = 1;
    delay_us(2);
    SDA = 1;
    delay_us(2);
}


/**
 * master send mode 
 * write one byte to bus 
 * @author qiufengli (2020/5/4)
 * 
 * @param c 
 */
void i2c_sendbyte(unsigned char c)
{
    unsigned char i;

    /// 
    /// 1.When SCL in low level, SDA can change.
    /// 2.When SCL in high level, read/write SDA, keep SDA stable
    ///   while acess SDA.
    /// 3.

    SCL_LOW;

    // from high bit to low bit
    for (i = 8; i != 0; i--) 
    {
        if (c & 0x80)
        {
            SDA = 1;
        }
        else
        {
            SDA = 0;
        }
        delay_us(2);

        /// generate a pulse at SCL 
        SCL_HIGH;
        delay_us(1);
        SCL_LOW;

        // move to next bit 
        c <<= 1;
    }

    // release SDA
    SDA_HIGH;
    delay_us(2);

    // release SCL
    SCL_HIGH;
    delay_us(1);
}


/**
 * master read mode 
 * read one byte from bus, and send a ack to slave sender
 * 
 * @author qiufengli (2020/5/4)
 * 
 * @return unsigned char 
 */
unsigned char i2c_recvbyte(__bit ack)
{
    unsigned char   i;
    unsigned char   c;

    c = 0;

    /// 
    /// 1.When SCL in low level, SDA can change.
    /// 2.When SCL in high level, read/write SDA, keep SDA stable
    ///   while acess SDA.
    /// 3.
    SCL = 0;
    SDA = 1;
    delay_us(2);

    for (i = 8; i != 0; i--) {
        c <<= 1;

        SCL = 1;
        delay_us(1);
        c |= SDA;
        SCL = 0;
        delay_us(2);
    }

    // after read one byte, send an ack
    SDA = ack;
    delay_us(1);
    SCL = 1;
    delay_us(1);

    return c;
}
/**
 * in 8051 with 12M clock, One Machine Cycle will take 1us
 * 
 * @author qiufengli (2020/4/26)
 * 
 * @param us 
 */
void delay_us(int us)
{
    int i; 
    for (i = 0; i <= us; i++)
    {
        __asm
        NOP
        __endasm;
    }
}

void key_down()
{

}

/////////////////////////////////////////////////////////////////////////////// 
/// 
///  part II
///  AT24C02 Interface search target for read command: 0xA0
///  search target for write command :0xA1
///  

#define CMD_ADDRESS_FOR_READ 0xA1
#define CMD_ADDRESS_FOR_WRITE 0xA0



/**
 * read one byte from AT24C02 
 * 
 * @author qiufengli (2020/5/4)
 * 
 * @param addr 
 * 
 * @return unsigned char : read one byte
 */
unsigned char at24c02_readbyte(unsigned char addr)
{
    unsigned char data;
    i2c_start();
    i2c_sendbyte(CMD_ADDRESS_FOR_WRITE);
    i2c_sendbyte(addr);
    i2c_start();
    i2c_sendbyte(CMD_ADDRESS_FOR_READ);
    data = i2c_recvbyte(1);
    i2c_stop();
    return data;

}

/**
 * write one byte to AT24C02
 * 
 * @author qiufengli (2020/5/4)
 * 
 * @param addr 
 * @param data 
 */
void at24c02_writebyte(unsigned char addr, unsigned char data)
{
    i2c_start();

    i2c_sendbyte(CMD_ADDRESS_FOR_WRITE); 
//    at24c02_ack();

    i2c_sendbyte(addr);
//    at24c02_ack();

    i2c_sendbyte(data);
//    at24c02_ack();

    i2c_stop();
}

void at24c02_ack()
{
    unsigned char i;
    SCL = 1;
    delay_us(2);
    while ((SDA == 1)
           && (i < 250))
    {
        i++;
    }
    SCL = 0;
    delay_us(2);
}


/////////////////////////////////////////////////////////////////////////////// 
/// 
///  part III
///  test function
///  

#include "tube.h"

//============timer0 int =====================//
#define OSC_FREQ	12000000UL

#define TIMER_MODE0		0x00
#define TIMER_MODE1		0x01
#define TIMER_MODE2		0x02
#define TIMER_MODE3		0x03







/**
 * change the tube every timer interrupt
 * 
 * @author qiufengli (2020/4/29)
 */
void change_tube()
{
    lsa = g_line_decorder[g_line_deocorder_index][0];
    lsb = g_line_decorder[g_line_deocorder_index][1];
    lsc = g_line_decorder[g_line_deocorder_index][2];

    // move to next tube
    g_line_deocorder_index = (g_line_deocorder_index+1) % LINE_COUNT;
}

/**
 * change the number every time interrupt
 * 
 * @author qiufengli (2020/4/29)
 */
void change_number()
{
    GPIO_DIGTAL_TUBE = g_tube_code_table[g_number_index];

    //move to next number
    g_number_index = (g_number_index + 1) % TUBE_CODE_TABLE_COUNT;
}

/**
 * revert the led
 * 
 * @author qiufengli (2020/4/29)
 */
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

/**
 * init timer
 * @author qiufengli (2020/4/15)
 * 
 * @param void 
 */
void timer0_init(void)
{
    TMOD &= 0xF0;
    TMOD |= TIMER_MODE1; /* Timer 0 work on 16-bit timer mode */

    // 1000us
    TH0 = (65536 - 1000 * (OSC_FREQ / 12000000)) >> 8;
    TL0 = (65536 - 1000 * (OSC_FREQ / 12000000)) & 0x00FF;

    ET0 = 1;        // open timer 0 interrupt
    EA = 1;         // enable CPU interrupt

    TR0 = 1;        /* Turn on timer 0 */
}


void timer0_int_1ms()
{
    static unsigned int count = 0;

    count++;
    // reconfigure 1ms
    TH0 = (65536 - 1000 * (OSC_FREQ / 12000000)) >> 8;
    TL0 = (65536 - 1000 * (OSC_FREQ / 12000000)) & 0x00FF;

    // use ms timer to generate one second timer
    if (count >= 1000)
    {
        count = 0;
        change_tube();
        change_number();
        revert_led();

        // store g_number_index at address 0
        // store g_line_deocorder_index at address 64
//        at24c02_writebyte(0, g_number_index);
        at24c02_writebyte(1, g_line_deocorder_index);
    }

}


#if defined SDCC || defined __SDCC
void timer0_int() __interrupt (1)
#else
void timer0_int() interrupt 1
#endif
{
    timer0_int_1ms();
}

