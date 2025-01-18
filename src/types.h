#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

typedef unsigned char uch;

/* Status will be used in fn. return type */
typedef enum
{
    e_failure,
    e_success
} Status;

typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

/* Uisng Bit Fields for flags */
struct NIBBLE
{
    uch flag1 : 2;
    uch flag2 : 1;
    uch flag3 : 1;
};

/* Initializing structure for flags */
struct NIBBLE nibble;
#endif
