//
// generates a new random dungeon - the only public routine is newDungeon() - the rest of the 
// code is static helpers...
//

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "atari.h"
#include "common.h"
#include "map.h"
#include "glyphs.h"
#include "die.h"
#include "monster.h"
#include "gd.h"
#include "screen.h"
#include "utils.h"
#include "input.h"

typedef enum { ROOM, CELL_EMPTY } CELL_TYPE;

static byte         currentRoom = 0;
static CELL_TYPE    levelMap[ 4 ][ 8 ];

static byte roomTemplate[5][5] = {
    { WALL, WALL,  HDOOR,  WALL,  WALL },
    { WALL, FLOOR, FLOOR, FLOOR, WALL },
    { VDOOR, FLOOR, FLOOR, FLOOR, VDOOR },
    { WALL, FLOOR, FLOOR, FLOOR, WALL },
    { WALL, WALL,  HDOOR,  WALL,  WALL }
};

static byte emptyTemplate[5][5] = {
    { FLOOR, FLOOR, FLOOR, FLOOR, FLOOR },
    { FLOOR, FLOOR, FLOOR, FLOOR, FLOOR },
    { FLOOR, FLOOR, FLOOR, FLOOR, FLOOR },
    { FLOOR, FLOOR, FLOOR, FLOOR, FLOOR },
    { FLOOR, FLOOR, FLOOR, FLOOR, FLOOR }
};


void        setLevelMap( byte row, byte col, CELL_TYPE c )  { levelMap[ row ][ col ] = c;    }
CELL_TYPE   getLevelMap( byte row, byte col )               { return levelMap[ row ][ col ]; }


void generateLevel( void ) {
    for( byte row = 0; row < 4; row++ ) {
        setLevelMap( row, 0, CELL_EMPTY );
        setLevelMap( row, 7, CELL_EMPTY );
        for( byte col = 1; col < 7; col++ ) {
            if( dieRoll( 1, 100 ) < 50 )
                setLevelMap( row, col, ROOM );
            else
                setLevelMap( row, col, CELL_EMPTY );
        }
    }
}


byte neighbors( byte row, byte col ) {
    byte rv = 0;
    byte cell;

    cell = mapGetData( col - 1, row );
    if( cell == HDOOR || cell == VDOOR || cell == OVDOOR || cell == OHDOOR )
        ++rv;

    cell = mapGetData( col + 1, row );
    if( cell == HDOOR || cell == VDOOR || cell == OVDOOR || cell == OHDOOR )
        ++rv;

    cell = mapGetData( col, row - 1 );
    if( cell == HDOOR || cell == VDOOR || cell == OVDOOR || cell == OHDOOR )
        ++rv;

    cell = mapGetData( col, row + 1 );
    if( cell == HDOOR || cell == VDOOR || cell == OVDOOR || cell == OHDOOR )
        ++rv;


    return rv;
}


// could use while(true) {} instead of the goto...but this produces smaller code
LOCATION findRandomFloorSpace( void ) {
    LOCATION rv;
    byte x, y;

tryAgain:
    x = dieRoll( 1, 40 ) - 1;
    y = dieRoll( 1, 22 ) - 1;

    if( mapGetData( x, y ) == FLOOR ) {
        rv.x = x;
        rv.y = y;
        return rv;
    }

    goto tryAgain;
}

static void paintCell( byte row, byte col ) {
    typedef byte someArray[5][5];
    someArray *sourceData;
    byte p[5][5];
    byte rRow, rCol;

    switch( getLevelMap( row, col )) {
        case ROOM:
            sourceData = &roomTemplate;
            break;
        case CELL_EMPTY:
            sourceData = &emptyTemplate;
            break;
        default:
            return;
    }

    memcpy( p, sourceData, 25 );

    rRow  = row * 5;
    rCol  = col * 5;

    memcpy( dungeon.mapData + yylookup_gr0[ rRow++ ] + rCol, p[0], 5 );
    memcpy( dungeon.mapData + yylookup_gr0[ rRow++ ] + rCol, p[1], 5 );
    memcpy( dungeon.mapData + yylookup_gr0[ rRow++ ] + rCol, p[2], 5 );
    memcpy( dungeon.mapData + yylookup_gr0[ rRow++ ] + rCol, p[3], 5 );
    memcpy( dungeon.mapData + yylookup_gr0[ rRow   ] + rCol, p[4], 5 );
}


char randomLockedDoor( char theDoor ) {
    if( dieRoll( 1, 10 ) == 1 ) {           // 10% chance a door is locked
        if( theDoor == HDOOR )
            return LHDOOR;
        else if( theDoor == VDOOR )
            return LVDOOR;
    }
    return theDoor;
}


void addAtRandomLocation( char theGlyph ) {
    LOCATION l = findRandomFloorSpace();
    mapSetDataViaLoc( l, theGlyph );
} 


void populateDungeon( byte level ) {
    LOCATION    playerStart   = mapGetStartLocation();
    LOCATION    monsterLoc;
    byte        monstersToAdd = 10;        // each level should have 4-7 monsters on it
    byte        monsterHD     = level + 1;           // monsters on par with the player
    byte        monsterToAdd  = 0;
    byte        nthMonster;


    while( monstersToAdd ) {
        monsterLoc = findRandomFloorSpace();

        nthMonster = dieRoll( 1, monsterCount ) - 1;
        
        roamingMonsterList[ roamingCount ] = masterMonsterList[ nthMonster ];
        roamingMonsterList[ roamingCount ].loc = monsterLoc;
        roamingMonsterList[ roamingCount ].hitPoints = dieRoll( roamingMonsterList[ roamingCount ].hitDie, 6 ) + ( currentLevel % 10 );
        roamingCount++;

        // place monster placeholder
        mapSetDataViaLoc( monsterLoc, G_PLAYER );
        monstersToAdd--;
    }

    // clear monster placeholders
    mapSearchReplace( G_PLAYER, FLOOR );
}

void updateDungeon( void ) {
    for( byte row = 0; row < 4; row++ )
        for( byte col = 0; col < 8; col++ )
            paintCell( row, col );
}


void patchDoors( void ) {   

    for( byte row = 1; row < 19; row++ ) {
        for( byte col = 1; col < 38; col++ ) {
            if( mapGetData( col, row ) == HDOOR || mapGetData( col, row ) == VDOOR ) {
                if( neighbors( row, col ) != 0 ) {
                    if( mapGetData( col, row ) == HDOOR )
                        mapSetData( col, row, OHDOOR );
                    else
                        mapSetData( col, row, OVDOOR );
                }
            }
        }
    }

}



void newDungeon( byte level ) {
    newMap();
    generateLevel();
    updateDungeon();    // convert levelMap to mapData
    patchDoors();

    // add basic dungeon stuff
    
    mapSetData( 0, 0, STAIRS_UP );
    mapSetData( 39, 19, STAIRS_DOWN );

    if( dieRoll( 1, 2 ) == 1 )
        addAtRandomLocation( GWATER );
    if( dieRoll( 1, 2 ) == 1 )
        addAtRandomLocation( G_FOOD );

    // add zombies
    populateDungeon( level );
}
    