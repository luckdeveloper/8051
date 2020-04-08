//use this file to hide the difference of KEIL and SDCC

#ifndef PLATFORM_H__
#define PLATFORM_H__

#ifdef KEIL
    #include "reg52.h"
#else
    #include "8052.h"
#endif

#endif
