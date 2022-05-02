
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "common.h"
#include "atari.h"
#include "screen.h"
#include "utils.h"
#include "screen.h"
#include "input.h"
#include "item.h"
#include "map.h"
#include "glyphs.h"
#include "gd.h"
#include "log.h"


bool monsterThere( LOCATION l ) {
	for( byte i = 0; i < roamingCount; i++ )
		if( locEqual( l, roamingMonsterList[ i ].loc) && roamingMonsterList[ i ].hitPoints > 0 )
			return true;
	return false;
}


byte atLeastOne( sbyte value ) {
    if( value < 1 )
        return 1;
    else
        return (byte)value;
}

void banner( char *msg  ) { 
    a_cls();
    memset( (byte *)SAVMSC, 0x80, 120 ); 
    a_inverse();
    centerText( 1, msg );
    a_inverse();
}

void centerText( byte row, char *str ) {
	byte x = ( 40 - strlen( str ) ) >> 1;
	a_printStringAt(  row, x, str );
}

bool showFile( char *filename ) {
	FILE f = a_fopen( filename, "r" );
	byte ch;

	a_cls();
	if( f != 0xff ) {
        while( !a_feof( f )) {
            a_fread( f, &ch, 1 );
            if( ch != 10 )                  // skip over ascii line feeds...
                a_putchar( ch );
        }
        a_fclose( f );
	} 

    pause();
    return true;
}

void pause( void ) {
	centerText( 22, "PRESS ANY KEY");
	(void)getch();
}

void shortPause( void ) {
    a_printf(" - PAUSE");
    (void)getch();
}

ID findMasterEquipmentByName( char *name ) {
    for( byte i = 1; i < equipmentCount; i++ ) 
        if( !strcmp(  masterEquipmentList[ i ].name, name )) 
            return  masterEquipmentList[ i ].id;

    return 0;       // returns 0 on failure
}

EQUIPMENT findMasterEquipmentByID( ID theID ) {
    for( byte i = 1; i < equipmentCount; i++ )
        if( masterEquipmentList[ i ].id == theID )
            return  masterEquipmentList[ i ];

    return masterEquipmentList[ 0 ];                     // return the NULL equipment entry
}

byte yesNo( void ) {
    while( true ) {
        byte ch = getch();
        if( ch == UC_KEY_Y || ch == LC_KEY_Y ) return UC_KEY_Y;
        if( ch == UC_KEY_N || ch == LC_KEY_N ) return UC_KEY_N;
        if( ch == UC_KEY_C || ch == LC_KEY_C ) return UC_KEY_C;
    }
}

void clearStatusLine( void ) { memset( (byte *)SAVMSC + yylookup_gr0[ 22 ], 0x00, 40 ); }

void footerMessage( char *msg ) {
	clearStatusLine();
    a_printStringAt( 22, 0, msg );
    addLog( msg );
}

void addKarma( sbyte amount ) {

    if( cheat + amount > 200 )
        cheat = 200;
    else {
        int working = cheat + amount;
        if( working < 0 )
            cheat = 0;
        else
            cheat += amount;
    }
}

void displayPlayerData( void ) {
    byte i, depth;
    char *no = "NO";
	char *yes = "YES";

   	a_printStringAt(  4, 2, a_sprintf( msg, "STR      %b ",  strength ));
	a_printStringAt(  5, 2, a_sprintf( msg, "DEX      %b ",  dexterity ));
	a_printStringAt(  6, 2, a_sprintf( msg, "CON      %b ",  constitution ));
	a_printStringAt(  7, 2, a_sprintf( msg, "CHA      %b ",  charisma ));

	a_printStringAt(  9, 2, a_sprintf( msg, "HP       %b/%b ",  currentHitPoints,  hitPoints ));
	a_printStringAt( 10, 2, a_sprintf( msg, "AC       %d ",  (int)computePlayerAC()));
	a_printStringAt( 11, 2, a_sprintf( msg, "LEVEL    %b ",  level ));
	a_printStringAt( 12, 2, a_sprintf( msg, "XP       %u ",  experience ));
  
	a_printStringAt( 14, 2, a_sprintf( msg, "SICK     %s", ( isSick == 0 ? no : yes )));
	a_printStringAt( 15, 2, a_sprintf( msg, "POISONED %s", ( isPoisoned == 0 ? no : yes )));
	a_printStringAt( 16, 2, a_sprintf( msg, "HUNGRY   %s", ( isFed < 20 ? yes : no )));
	a_printStringAt( 17, 2, a_sprintf( msg, "THIRSTY  %s", ( isWatered < 20 ? yes : no )));
	a_printStringAt( 18, 2, a_sprintf( msg, "BLESSED  %s", ( isBlessed == 0 ? no : yes )));

    a_printStringAt( 4, 25, a_sprintf( msg, "WATER   %b ",  water ));
    a_printStringAt( 5, 25, a_sprintf( msg, "RATIONS %b ",  rations ));
	a_printStringAt( 6, 25, a_sprintf( msg, "POTIONS %b ",  potions ));
    a_printStringAt( 7, 25, a_sprintf( msg, "GOLD    %u ",  gold ));
    a_printStringAt( 8, 25, a_sprintf( msg, "KARMA   "));

	if( cheat > 100 )
		a_printf("BAD ");
	else if( cheat > 25 )
		a_printf("POOR");
	else
		a_printf("GOOD");

    a_printStringAt( 10, 25, "INVENTORY");

	depth = 11;
	for( i = 0; i < 10; i++ )
		if(  slotUsed[ i ] == true ) 
            a_printStringAt( depth++, 26, a_sprintf( msg, "%s",  inventory[ i ].name ));
}


// lifted from http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C
word square_root( word n ) {
    word root, remainder, place;

    root = 0;
    remainder = n;
    place = 0x4000; 

    while( place > remainder )
        place = place >> 2;

    while( place ) {
        if( remainder >= root + place ) {
            remainder = remainder - root - place;
            root = root + (place << 1);
        }
        root = root >> 1;
        place = place >> 2;
    }
    return root;
}

word range( LOCATION from, LOCATION to ) {
	int dX = abs( from.x - to.x );
	int dY = abs( from.y - to.y );
	 
	return square_root(( dX * dX ) + ( dY * dY ));
}

bool locEqual( LOCATION a, LOCATION b ) { return a.x == b.x && a.y == b.y; }

bool canSeeThrough( sbyte x, sbyte y ) {
    byte cell = mapGetData( (byte)x, (byte)y );

    switch( cell ) {
        case FLOOR:
        case OVDOOR:
        case OHDOOR:
        case STAIRS_DOWN:
        case STAIRS_UP:
        case GWATER:
        case G_LOOT:
        case TUNNEL:
            return true;
        default:
            return false;
    }
}

// draw a LOS from LOCATION a to LOCATION b
// lifted from rosetta code
bool drawLOS( LOCATION a, LOCATION b ) {
  int x0 = a.x;
  int y0 = a.y; 
  int x1 = b.x;
  int y1 = b.y;
 
  int dx = abs( x1 - x0 ), sx = x0 < x1 ? 1 : -1;
  int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1; 
  int err = (dx > dy ? dx : -dy)/2;
  int e2;
 
  for(;;){
    if(!canSeeThrough( x0, y0 ))
        return false;
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
  return true;
}
