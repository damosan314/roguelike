
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "screen.h"
#include "gd.h"
#include "utils.h"
#include "log.h"

char logScreen[ 10 ][ 40 ] = { "", "", "", "", "", "", "", "", "", "" };

void addLog( char *logEntry ) {
    for( byte i = 0; i < 9; i++ )
        strcpy( logScreen[ i ], logScreen[ i + 1 ] ); 
    strcpy( logScreen[ 9 ], logEntry );
}


void showLog( void ) {
    byte i;

    a_cls();
    banner("LOG");

    a_printStringAt( 5,  5, a_sprintf( msg, "STEPS:   %u", stepsTaken ));
    a_printStringAt( 5, 20, a_sprintf( msg, "KILLS:   %u", monstersKilled ));

    a_printStringAt( 6,  5, a_sprintf( msg, "ATTACKS: %u", attacksAttempted ));
    a_printStringAt( 6, 20, a_sprintf( msg, "HITS:    %u", hitsLanded ));
    
    a_printStringAt( 7,  5, a_sprintf( msg, "FOOD:    %u", foodEaten ));
    a_printStringAt( 7, 20, a_sprintf( msg, "WATER:   %u", waterDrank ));

    a_printStringAt( 8, 5, a_sprintf( msg, "POTIONS: %u", potionsConsumed ));

    a_inverse();
    centerText( 10, "LAST 10 UPDATES" );
    a_inverse();

    a_gotoxy( 0, 11 );

    for( i = 0; i < 10; i++ ) {
        a_gotoxy( 0, 11 + i );
        a_printf("%s", logScreen[ i ] );
    }

    pause();
}


