
#include <stdbool.h>
#include "common.h"
#include "atari.h"
#include "input.h"
#include "screen.h"


bool kbhit( void ) { return CH != 0xff; }

// attribute added because clang likes to optimize this routine to the point
// where it doesn't work.
byte getch( void )  __attribute__ ((optnone)) {
    byte rv = CH;

    while( !kbhit() )
        ;
    
    rv = CH;

    CH = 0xff;                  // tell OS we're ready for the next key

    if( rv >= 0xc0 )
        rv = rv & 0x3f;         // strip off control chars

    return  rv ;
}
