

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

void addAtRandomLocation( char theGlyph ) {
    LOCATION l = findRandomFloorSpace();
    mapSetData( l.x, l.y, theGlyph );
}


void populateDungeon( byte level ) {
    LOCATION    playerStart = mapGetStartLocation();
    LOCATION    monsterLoc;
    byte        monstersToAdd = 3 + dieRoll( 1, 3 );        // each level should have 4-7 monsters on it
    byte        monsterHD =  level + 1;           // monsters on par with the player
    byte        monsterToAdd = 0;
    byte        nthMonster;

    while( monstersToAdd ) {
        monsterLoc = findRandomFloorSpace();

tryAgain:
        nthMonster = dieRoll( 1, monsterCount - 1 ) - 1;
        if( masterMonsterList[ nthMonster ].hitDie <= monsterHD ) 
            goto foundMonster;
        goto tryAgain;
        
foundMonster:
        roamingMonsterList[ roamingCount ] = masterMonsterList[ nthMonster ];
        roamingMonsterList[ roamingCount ].loc = monsterLoc;
        roamingMonsterList[ roamingCount ].hitPoints = dieRoll( roamingMonsterList[ roamingCount ].hitDie, 6 );
        roamingCount++;

        // place temp. marker
        mapSetData( monsterLoc.x, monsterLoc.y, G_PLAYER );
        monstersToAdd--;
    }

    mapSearchReplace( G_PLAYER, FLOOR );
}


void randomAllocate( void ) {
    byte row, col;

    memset( dungeon.mapData, WALL, sizeof( dungeon.mapData ));
    for( row = 1; row < 21; row++ )
        for( col = 1; col < 39; col++ )
            if( dieRoll( 1, 100) <= 55 )
                mapSetData( col, row, FLOOR );
}


byte countNeighbors( byte row, byte col ) {
    byte rv = 0;

    for( sbyte yy = -1; yy <= 1; yy++ )
        for( sbyte xx = -1; xx <= 1; xx++ )
            if( yy != 0 && xx != 0 ) 
                if( mapGetData( (sbyte)col + xx, (sbyte)row + yy ) == WALL )
                    ++rv;
    
    return rv;
}

byte countOrthogonalNeighbors( byte row, byte col ) {
    byte rv = 0;

    rv += mapGetData( col - 1, row ) == WALL ? 1 : 0;
    rv += mapGetData( col, row - 1 ) == WALL ? 1 : 0;
    rv += mapGetData( col + 1, row ) == WALL ? 1 : 0;
    rv += mapGetData( col, row + 1 ) == WALL ? 1 : 0;

    return rv;
}



void automoton( void ) {
    byte row, col;

    for( row = 1; row < 21; row++ ) 
        for( col = 1; col < 39; col++ ) {
            byte neighbors = countNeighbors( row, col );
            if( mapGetData( col, row ) == WALL && ( neighbors == 0 || neighbors > 4 ))
                mapSetData( col, row, FLOOR );            
        }
}

void automoton2( void ) {
    byte row, col;

    for( row = 1; row < 21; row++ ) 
        for( col = 1; col < 39; col++ ) {
            byte neighbors = countNeighbors( row, col );
            if( neighbors == 0 && mapGetData( col, row ) == WALL )
                mapSetData( col, row, FLOOR );            
        }
}

word automoton3( void ) {
    byte row, col;
    word rv = 0;

    for( row = 1; row < 21; row++ ) 
        for( col = 1; col < 39; col++ ) {
            byte neighbors = countOrthogonalNeighbors( row, col );
            if( neighbors >= 3 && mapGetData( col, row ) == FLOOR ) {
                mapSetData( col, row, WALL );            
                ++rv;
            }
        }
    return rv;
}

void automoton4( void ) {
    byte row, col;

    for( row = 1; row < 21; row++ ) 
        for( col = 1; col < 39; col++ ) 
            if( mapGetData( col, row ) == WALL ) {
                byte oNeighbors = countOrthogonalNeighbors( row, col );
                byte neighbors = countNeighbors( row, col );
                if( neighbors == 0 || ( neighbors == 1 && oNeighbors == 0 ) || ( oNeighbors == 2 && neighbors > 2 ))
                    mapSetData( col, row, FLOOR );
            }
}



void addItem( byte which ) {
    bool searching = true;

    while( searching ) {
        byte col = dieRoll( 1, 40 ) - 1;
        byte row = dieRoll( 1, 22 ) - 1;

        if( mapGetData( col, row ) == FLOOR ) {
            mapSetData( col, row, which );
            return;
        }
    }
}


void newDungeon( byte level ) {
    newMap();
    randomAllocate();
    automoton();  
    while( automoton3() != 0 )
        ;

    // add basic dungeon stuff
    addAtRandomLocation( STAIRS_UP );
    if( level != 100 )
        addAtRandomLocation( STAIRS_DOWN );
    if( dieRoll( 1, 2 ) == 1 )
        addAtRandomLocation( GWATER );
    if( dieRoll( 1, 2 ) == 1 )
        addAtRandomLocation( G_FOOD );

    if( level < 50 )
        populateDungeon( level );
    else {
        roamingCount = 0;    
        roamingMonsterList[ roamingCount ] = masterMonsterList[ monsterCount - 1 ];     // the main boss
        roamingMonsterList[ roamingCount ].loc.x = 18;
        roamingMonsterList[ roamingCount ].loc.y = 9;
        roamingMonsterList[ roamingCount ].hitPoints = 1;
        roamingCount++;
    }

        // sprinkle artifacts
    if( level == 20 || level == 30 || level == 40 ) {
        EQUIPMENT artifact;
        
        artifact.loc = findRandomFloorSpace();

        switch( level ) {
            case 20:
                artifact.id = 200;
                break;
            case 30:
                artifact.id = 201;    
                break;
            case 40:
                artifact.id = 202;    
                break;
        }

        droppedEquipmentList[ droppedCount++ ] = artifact;
    }
}