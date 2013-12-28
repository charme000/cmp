#ifndef __DEFINES_HEADER__
#define __DEFINES_HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <fcntl.h>

#ifdef WIN32
#include <io.h>
#endif

#define INTERNAL_F

#ifdef __DEBUG
#define EXPORT_F __attribute((visibility("default")))
#else
#define EXPORT_F 
#endif

#define uchar unsigned char
#define schar char
#define uint unsigned int
#define uint16 unsigned short
#define uint32 unsigned long
#define sint int
#define sint16 short
#define sint32 long

#define max_value(x,y) ((x>y)?x:y)

#endif