#ifndef __MAP_H
#define __MAP_H

#include "common.h"
#include "item.h"               // pulls in LOCATION
#include "glyphs.h"


#define MAP_MAX_COL 40
#define MAP_MAX_ROW 22

#define MAPCELLS 40 * 22

typedef struct {
    byte mapData[ MAPCELLS ];
    byte mapFlags[ MAPCELLS ];
} MAP_DATA;

#define MAP_FLAG_FIRE       0x80
#define MAP_FLAG_FLOOD      0x40
#define MAP_FLAG_VISIBLE    0x20
#define MAP_FLAG_CLEAR      0x00

void        newMap( void );
byte        mapGetFlags( MAP_DATA *m, byte x, byte y );
void        mapSetFlags( MAP_DATA *m, byte x, byte y, byte value );
byte        mapGetData( byte x, byte y );
byte        mapGetDataViaLoc( LOCATION l );
void        mapSetData( byte x, byte y, byte value );
void        mapSetDataViaLoc( LOCATION l, byte value );
LOCATION    mapGetStartLocation();
void        mapSearchReplace( byte oldSymbol, byte newSymbol );
bool        handleDoor( LOCATION l );
void        unlockDoor( LOCATION l );
void        updateLOS( void );

#endif