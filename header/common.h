
#ifndef __COMMON_H
#define __COMMON_H

typedef unsigned char   byte;
typedef signed char     sbyte;
typedef unsigned int    word;

#define _S(v)     (char *)(v)
#define _C(v)     (char)(v)

#define MAX_STRING 21

#define MAX_ITEMS 256

extern word _id;
#define ID  word

#endif