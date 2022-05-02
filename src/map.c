#include <stdlib.h>
#include <string.h>

#include "atari.h"
#include "common.h"
#include "map.h"
#include "item.h"
#include "glyphs.h"
#include "gd.h"
#include "utils.h"

void mapSearchReplace( byte oldSymbol, byte newSymbol ) {
    for( byte row = 0; row < MAP_MAX_ROW; row++ )
        for( byte col = 0; col < MAP_MAX_COL; col++ )
            if( mapGetData( col, row ) == oldSymbol ) 
                mapSetData( col, row, newSymbol );
}

static bool validMapLocation( byte x, byte y ) {
    if( x >= MAP_MAX_COL || y >= MAP_MAX_COL )
        return false;
    return true;
}

LOCATION mapGetStartLocation( void ) {
    LOCATION rv = { 0xff, 0xff };

    for( byte row = 0; row < MAP_MAX_ROW; row++ )
        for( byte col = 0; col < MAP_MAX_COL; col++ )
            if( mapGetData( col, row ) == STAIRS_UP ) {
                rv.x = col;
                rv.y = row;
                return rv;
            }
    return rv;
}


byte mapGetFlags( MAP_DATA *m, byte x, byte y ) {
    if( validMapLocation( x, y )) {
        word yoffset = yylookup_gr0[ y ];
        return m->mapFlags[ yoffset + x ];
    }
    return 0x00;
}


void mapSetFlags( MAP_DATA *m, byte x, byte y, byte value ) {
    if( validMapLocation( x, y ))
        m->mapFlags[ yylookup_gr0[ y ] + x ] = value;
}


byte mapGetDataViaLoc( LOCATION l ) {
    if( validMapLocation( l.x, l.y )) 
        return dungeon.mapData[ yylookup_gr0[ l.y ] + l.x ]; 
    return EMPTY;
}

byte mapGetData( byte x, byte y ) { 
    if( validMapLocation( x, y )) 
        return dungeon.mapData[ yylookup_gr0[ y ] + x ]; 
    return EMPTY;
}


void mapSetDataViaLoc( LOCATION l, byte value ) { 
    if( validMapLocation( l.x, l.y ))
        dungeon.mapData[ yylookup_gr0[ l.y ] + l.x ] = value; 
}


void mapSetData( byte x, byte y, byte value ) { 
    if( validMapLocation( x, y ))
        dungeon.mapData[ yylookup_gr0[ y ] + x ] = value; 
}


void newMap( void ) {
    memset( &dungeon.mapData, 0x00, MAPCELLS );
    memset( &dungeon.mapFlags, MAP_FLAG_CLEAR, MAPCELLS );
}


bool handleDoor( LOCATION l) {
	byte cell = mapGetDataViaLoc(l);
	bool rv = true;

	switch( cell ) {
		case VDOOR:
			cell = OVDOOR;
			break;
		case HDOOR:
			cell = OHDOOR;
			break;
		default:
			rv = false;
	}

	if( rv )
		mapSetDataViaLoc( l, cell );

	return( rv );
}


void unlockDoor( LOCATION l ) {
	byte cell = mapGetDataViaLoc( l );
	byte newCell = 0;

	switch( cell ) {
		case LVDOOR:
			newCell = OVDOOR;
			break;
		case LHDOOR:
			newCell = OHDOOR;
			break;
	}
	mapSetDataViaLoc( l, newCell );
}

void updateLOS( void ) {
	sbyte col =  loc.x;
	sbyte row =  loc.y;
	sbyte currentRow, currentCol;

	for( currentRow = row - 2; currentRow < row + 2; currentRow++ )
		for( currentCol = col - 2; currentCol < col + 2; currentCol++ )
			mapSetFlags( &dungeon, currentCol, currentRow, MAP_FLAG_VISIBLE );
}
