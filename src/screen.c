
#include "common.h"
#include "atari.h"
#include "screen.h"
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

//#define SUPPORT_LONGS

static byte *scrn;

static byte xx, yy;                             // current X/Y of the virtual cursor
static bool inverse = false;                    // should characters be inversed?  (ch & 0x80)
static byte *scrnRows[ 24 ];                    // index into screen memory starting at SAVMSC
static char rv[ 7 ] = "000000";                 // used for various *tos functions

static void updateRowColCRS( void ) {
    ROWCRS = (word)yy;
    COLCRS = xx;
}

void a_init( void ) {
    byte row;

    scrn = (byte *)SAVMSC;
    
    for( row = 0; row < 24; row++ )
        scrnRows[ row ] = scrn + yylookup_gr0[ row ];
    
    a_cursorOff();
    
    xx = yy = 0;
    LMARGIN = 0;

    updateRowColCRS();
    a_cls();
}

void a_inverse( void )      { inverse = !inverse;   }
void a_cursorOff( void )    { CRSINH = 0xff;        }
void a_cursorOn( void )     { CRSINH = 0x00;        }

void a_cls( void ) {
    xx = yy = 0;
    updateRowColCRS();
    memset( scrn, 0, 960 );
}


char a_ascii_to_atascii( char character ) {
  
    if( character == '\r' )
        return '\n';

    if( character < 32 )
        character += 64;
    else if( character < 96 )
        character -= 32;
   
    return character;
}


void __putchar( char character ) {
    byte *basePtr = scrnRows[ yy ];

    switch( character ) {
        case '\n':                      // ch = 10...
            a_newline();
            break;
        default:
            basePtr[ xx++ ] = inverse ? character | 0x80 : character;
            if( xx > 39 )
                a_newline();
            break;
    }
}


void a_putchar( char character )     { __putchar( a_ascii_to_atascii( character )); }
void a_putchar_raw( byte character ) { __putchar( character ); }


void a_puts( char *string ) {
    
    while( *string )
        a_putchar( *string++ );

    a_newline();

    while( xx > 40 ) {
        ++yy;
        xx -= 40;
    }

    updateRowColCRS();
}


void a_gotoxy( byte col, byte row ) { 
    xx = col;
    yy = row;
    updateRowColCRS();
}


void a_printStringAt( byte row, byte col, char *msg ) {
    a_gotoxy( col, row );
    while( *msg )
        a_putchar( *msg++ );
}


void a_newline( void ) {
    xx = 0;

    if( yy == 23 ) {
        memcpy( (byte *)SAVMSC, scrnRows[1], 920 );
        memset( scrnRows[ 23 ], 0, 40 );
    } else
        ++yy;

    updateRowColCRS();
}



static char *itos( int v ) {
    char *p;
    byte index = 5;
    bool negative = v < 0 ? true : false;

    if( negative )
        v = 1 + ~v;

    if( v == 0 )
        rv[ index-- ] = '0';
    else
        while( v ) {
            int digit = v % 10;
            v = v / 10;
            rv[ index-- ] = '0' + digit;
        }

    if( negative )
        rv[ index-- ] = '-';

    p = &rv[ index + 1 ];

    return p;
}


static char *uitos( unsigned int v ) {
    char *p;
    byte index = 5;


    if( v == 0 )
        rv[ index-- ] = '0';
    else
        while( v ) {
            int digit = v % 10;
            v = v / 10;
            rv[ index-- ] = '0' + digit;
        }

    p = &rv[ index + 1 ];

    return p;
}


#ifdef SUPPORT_LONGS

static char *ltos( long v ) {
    static char rv[ 12 ] = "00000000000";
    char *p;
    byte index = 10;
    bool negative = v < 0L ? true : false;

    if( negative )
        v = 1 + ~v;

    if( v == 0 )
        rv[ index-- ] = '0';
    else
        while( v ) {
            int digit = v % 10L;
            v = v / 10L;
            rv[ index-- ] = '0' + digit;
        }

    if( negative )
        rv[ index-- ] = '-';

    p = &rv[ index + 1 ];

    return p;
}

#endif


static char *__printf( char *outbuffer, char *fmt, va_list arguments ) {
    char *ptr = outbuffer;
    char *temp;
    char temp2[2];

    temp2[ 1 ] = '\0';

    while( *fmt ) {
        switch( *fmt ) {
            case '%':
                ++fmt;
                temp = NULL;

                switch( *fmt ) {
                    case '%':
                        temp = "%";
                        break;
                    case 'c':
                    case 'C':
                        temp2[ 0 ] = (byte) va_arg( arguments, unsigned int );
                        temp = temp2;
                        break;
                    case 'u':
                    case 'U':
                    case 'b':
                    case 'B':
                        temp = uitos( (unsigned int) va_arg( arguments, unsigned int )); 
                        break;
                    case 'd':
                    case 'D':
                        temp = itos( (int)va_arg( arguments, int ));
                        break;
#ifdef SUPPORT_LONGS
                    case 'l':
                    case 'L':     
                        temp = ltos( (long)va_arg( arguments, long ));
                        break;
#endif
                    case 's':
                    case 'S':
                        temp = (char *)va_arg( arguments, char * );
                        break;
                }

                // move characters to output buffer - inversing them if need be
                while( *temp ) {
                    if( inverse )
                        *ptr++ = a_ascii_to_atascii( *temp++ ) | 0x80;
                    else
                        *ptr++ =  *temp++;
                }
                break;

            default:
                if( inverse )
                    *ptr++ = a_ascii_to_atascii( *fmt ) | 0x80;
                else
                    *ptr++ = *fmt;
                break;
        }
        ++fmt;
    }

    *ptr = '\0';

    return outbuffer;
}


void a_printf( char *fmt, ... ) {
    char outbuffer[ 41 ];
    char *p = outbuffer;

    va_list arguments;

    va_start( arguments, fmt );
    __printf( outbuffer, fmt, arguments );
    va_end( arguments );
    
    while( *p )
        a_putchar( *p++ );
}

char *a_sprintf( char *outbuffer, char *fmt, ... ) {
    char *saveOutBuffer = outbuffer;

    va_list arguments;

    va_start( arguments, fmt );
    __printf( outbuffer, fmt, arguments );
    va_end( arguments );
    
    return saveOutBuffer;
}
