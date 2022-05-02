#ifndef __SCREEN_H
#define __SCREEN_H

#include <stdbool.h>
#include "common.h"

void a_cls( void );
void a_puts( char * );
void a_gotoxy( byte, byte );
void a_newline( void ); 
void a_init( void );                            // called by atari_init() in atari.c
void a_cursorOff( void );
void a_cursorOn( void );

void a_printf( char *fmt, ... );
char *a_sprintf( char *output, char *fmt, ... );

void a_putchar( char );                         // converts ascii -> atascii
void a_putchar_raw( byte );                     // does no conversion

void a_inverse( void );                         // turns inverse characters on/off
char a_ascii_to_atascii( char );
void a_printStringAt( byte row, byte col, char *msg );

#endif