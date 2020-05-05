//use this file to hide the difference of KEIL and SDCC

#ifndef PLATFORM_H__
#define PLATFORM_H__

#if defined SDCC || defined __SDCC
    #include "8052.h"
#else
    #include "reg52.h"
#endif


#endif
