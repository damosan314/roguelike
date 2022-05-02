#ifndef __ATARI_H
#define __ATARI_H

#include <stdbool.h>
#include "common.h"

#define BYTE_REG( v ) (*(byte *) (v))
#define WORD_REG( v ) (*(word *) (v))

#define RTCLK       BYTE_REG( 0x12 )            // realtime clock
#define RTCLK1      BYTE_REG( 0x13 )            // realtime clock
#define RTCLK2      BYTE_REG( 0x14 )            // realtime clock

#define SOUNDR      BYTE_REG( 0x41 )            // when 0 disables IO noise when accessing sotrage
#define CRITIC      BYTE_REG( 0x42 )            // critical flag when non-zero.  Used by the OS but apps can set it as well...
#define ATRACT      BYTE_REG( 0x4d )            // screen atract mode register - set to 0 to reset

#define LMARGIN     BYTE_REG( 0x52 )            // screen left margin
#define RMARGIN     BYTE_REG( 0x53 )            // screen right margin
#define ROWCRS      BYTE_REG( 0x54 )            // current row
#define COLCRS      WORD_REG( 0x55 )            // current col

#define SAVMSC      WORD_REG( 0x58 )            // 0x58/0x59 hold pointer to screen memory

#define RAMTOP      BYTE_REG( 0x6a )            // number of 256 byte pages of RAM

#define SDMCTL      BYTE_REG( 0x022f )          // set to 0 to disable screen (makes things faster) AND sets PMG params
#define SDLSTL      WORD_REG( 0x0230 )          // address to the current display list
#define STICK0      BYTE_REG( 0x0278 )          // stick 0
#define STICK1      BYTE_REG( 0x0279 )          // stick 1

#define STRIG0      BYTE_REG( 0x0284 )
#define STRIG1      BYTE_REG( 0x0285 )

#define NOCLIK      BYTE_REG( 0x02db )
#define CRSINH      BYTE_REG( 0x02f0 )          // 0x00 = visible, 0x01 .. 0xff = hide cursor
#define CH1         BYTE_REG( 0x02f2 )          // last key accepted from the keyboard
#define CH          BYTE_REG( 0x02fc )          // set to 255 to read the next character
#define RANDOM      BYTE_REG( 0xd20a )          // random number (from pokey)
#define CHBAS	    BYTE_REG( 0x02f4 )          // shadow register...modify this one instead of CHBASE
#define CHBASE	    BYTE_REG( 0xd409 )          // high byte of font table (0xe000 is the default)

#define COLOR1      BYTE_REG( 0x02c5 )
#define COLOR2      BYTE_REG( 0x02c6 )
#define COLOR4      BYTE_REG( 0x02c8 )

#define IOCB0       BYTE_REG( 0x0340 )          // normally the E: device
#define IOCB1       BYTE_REG( 0x0350 )
#define IOCB2       BYTE_REG( 0x0360 )
#define IOCB3       BYTE_REG( 0x0370 )
#define IOCB4       BYTE_REG( 0x0380 )
#define IOCB5       BYTE_REG( 0x03a0 )          // normally the S: device
#define IOCB6       BYTE_REG( 0x03b0 )          // normally for the printer in basic

#define EOL         0x9b
#define CIO_OPEN    0x03
#define CIO_CLOSE   0x0c

#define STICK_LEFT              11
#define STICK_RIGHT             7
#define STICK_UP                14
#define STICK_DOWN              13
#define STICK_TRIGGER_PRESSED   0   

//
// key codes
//

typedef enum { 
    LC_KEY_A = 63,
    UC_KEY_A = 127,
    LC_KEY_B = 21, 
    UC_KEY_B = 85,
    LC_KEY_C = 18, 
    UC_KEY_C = 82,
    LC_KEY_D = 58, 
    UC_KEY_D = 122,
    LC_KEY_E = 42, 
    UC_KEY_E = 106,
    LC_KEY_F = 56, 
    UC_KEY_F = 120,
    LC_KEY_G = 61,
    UC_KEY_G = 125, 
    LC_KEY_H = 57, 
    UC_KEY_H = 121,
    LC_KEY_I = 13,
    UC_KEY_I = 77, 
    LC_KEY_J = 1, 
    UC_KEY_J = 65,
    LC_KEY_K = 5, 
    UC_KEY_K = 69,
    LC_KEY_L = 0, 
    UC_KEY_L = 64,
    LC_KEY_M = 37,
    UC_KEY_M = 101, 
    LC_KEY_N = 35, 
    UC_KEY_N = 99,
    LC_KEY_O = 8, 
    UC_KEY_O = 72,
    LC_KEY_P = 10,
    UC_KEY_P = 74, 
    LC_KEY_Q = 47,
    UC_KEY_Q = 111, 
    LC_KEY_R = 40, 
    UC_KEY_R = 104,
    LC_KEY_S = 62, 
    UC_KEY_S = 126,
    LC_KEY_T = 45, 
    UC_KEY_T = 109,
    LC_KEY_U = 11, 
    UC_KEY_U = 75,
    LC_KEY_V = 16,
    UC_KEY_V = 80,  
    LC_KEY_W = 46, 
    UC_KEY_W = 110,
    LC_KEY_X = 22, 
    UC_KEY_X = 86, 
    LC_KEY_Y = 43, 
    UC_KEY_Y = 107,
    LC_KEY_Z = 23, 
    UC_KEY_Z = 87,
    KEY_1 = 31, 
    KEY_2 = 30, 
    KEY_3 = 26,
    KEY_4 = 24,
    KEY_5 = 29,
    KEY_6 = 27,
    KEY_7 = 51,
    KEY_8 = 53,
    KEY_9 = 48,
    KEY_0 = 50,
    KEY_ENTER = 12,
    KEY_QUESTION = 102
} ATARI_SCAN_CODES;


typedef struct { 
    byte    ICHID;      // HATAB index;
    byte    ICDNO;      // device number 1 = D1:, 2 = D2, ...
    byte    ICCOM;      // command
    byte    ICSTA;      // status
    void    *ICBAL;     // ICBAL L/H - address of buffer to filename or status...
    void    *ICPTL;     // ICPTL L/H - OS provided address of put-one-byte routine
    word    ICBLL;      // number of bytes to transfer in PUT/GETs
    byte    ICAX1;      // aux byte number one - used to specify open mode
    byte    ICAX2;      // aux byte number 2
    word    ICAX3;      // ICAX3/4 - maintains record of disk sector
    byte    ICAX5;      // used by NOTE and POINT in basic
    byte    ICAX6;      // spare aux byte
} IOCB;

//
// CIO error codes
//
#define CIO_SUCCESS         0x01
#define CIO_NO_DEVICE       0x82        // try to open D2:test.dat and no second drive...
#define CIO_NOT_OPEN        0x85        // file or device not open
#define CIO_END_OF_FILE     0x88
#define CIO_TIMEOUT         0x8a
#define CIO_DISK_FULL       0xa2
#define CIO_FATAL_DISK      0xa3
#define CIO_FILE_NOT_FOUND  0xaa

//
// CIO commands
//
#define ICCOM_OPEN          0x03    // open channel
#define ICCOM_GET_REC       0x05    // read till end of line/record (0x9b)
#define ICCOM_GET_CHAR      0x07
#define ICCOM_PUT_CHAR      0x0b
#define ICCOM_PUT_REC       0x09
#define ICCOM_CLOSE         0x0c
#define ICCOM_DOS_RENAME    0x20
#define ICCOM_DOS_DELETE    0x21

//
// AUX parameters for when CIO command deals with disk
//
#define ICAX_DISK_OPEN_FILE 0x04    // read
#define ICAX_DISK_READ_DIR  0x06    // read disk directory
#define ICAX_DISK_WRITE_NEW 0x08    // overwrite an existing file
#define ICAX_DISK_WRITE_APP 0x09    // write-append mode
#define ICAX_DISK_WRITE_OVR 0x12    // write-overwrite mode

typedef unsigned char FILE;

void    atari_init( void );
FILE    a_fopen( char *filename, char *mode );
void    a_fclose( FILE f );
word    a_fwrite( FILE f, void *data, word size );
word    a_fread( FILE f, void *data, word size );
bool    a_feof( FILE f );
bool    a_fdelete( char *filename );

// assembly routine....
byte    _cio( byte A );

// defined globally in rl2.c
extern const word yylookup_gr0[];

// defined in atari.c
extern byte a_error;


#endif