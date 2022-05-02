
#include <stdbool.h>
#include "atari.h"
#include "die.h"
#include "screen.h"

byte dieRoll( byte n, byte die ) {
    volatile byte rv = 0;

    while( n-- )
        rv += (RANDOM % die ) + 1;

    return rv;
}
