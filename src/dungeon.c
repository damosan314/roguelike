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

typedef enum { ROOM, CELL_EMPTY, HORIZONTAL, VERTICAL, CROSS, LOWER_T, UPPER_T, LEFT_T, RIGHT_T, LEFT_LOWER_C, RIGHT_LOWER_C, LEFT_UPPER_C, RIGHT_UPPER_C } CELL_TYPE;
typedef enum { FROM_LEFT, FROM_RIGHT } CONVERT_DIRECTION;

static byte         currentRoom = 0;
static CELL_TYPE    levelMap[ 4 ][ 8 ];

static byte roomTemplate[5][5] = {
    { WALL, WALL,  WALL,  WALL,  WALL },
    { WALL, FLOOR, FLOOR, FLOOR, WALL },
    { WALL, FLOOR, FLOOR, FLOOR, WALL },
    { WALL, FLOOR, FLOOR, FLOOR, WALL },
    { WALL, WALL,  WALL,  WALL,  WALL }
};

static byte emptyTemplate[ 5 ][ 5 ] = {
    { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY },
    { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY },
    { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY },
    { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY },
    { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY }
};

static byte horizontalTunnelTemplate[ 5 ][ 5 ] = {
    { EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY },
    { EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY },
    { TUNNEL, TUNNEL, TUNNEL, TUNNEL, TUNNEL },
    { EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY },
    { EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY }
};

static byte verticalTunnelTemplate[ 5 ][ 5 ] = {
    { EMPTY, EMPTY, TUNNEL, EMPTY, EMPTY },
    { EMPTY, EMPTY, TUNNEL, EMPTY, EMPTY },
    { EMPTY, EMPTY, TUNNEL, EMPTY, EMPTY },
    { EMPTY, EMPTY, TUNNEL, EMPTY, EMPTY },
    { EMPTY, EMPTY, TUNNEL, EMPTY, EMPTY }
};

static byte crossRoadsTunnelTemplate[ 5 ][ 5 ] = {
    { EMPTY,  EMPTY,  TUNNEL, EMPTY,  EMPTY },
    { EMPTY,  EMPTY,  TUNNEL, EMPTY,  EMPTY },
    { TUNNEL, TUNNEL, TUNNEL, TUNNEL, TUNNEL },
    { EMPTY,  EMPTY,  TUNNEL, EMPTY,  EMPTY },
    { EMPTY,  EMPTY,  TUNNEL, EMPTY,  EMPTY }
};

static byte upperTTunnelTemplate[ 5 ][ 5 ] = {
    { EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY },
    { EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY },
    { TUNNEL, TUNNEL, TUNNEL, TUNNEL, TUNNEL },
    { EMPTY,  EMPTY,  TUNNEL, EMPTY,  EMPTY },
    { EMPTY,  EMPTY,  TUNNEL, EMPTY,  EMPTY }
};

static byte lowerTTunnelTemplate[ 5 ][ 5 ] = {
    { EMPTY,  EMPTY,  TUNNEL, EMPTY,  EMPTY },
    { EMPTY,  EMPTY,  TUNNEL, EMPTY,  EMPTY },
    { TUNNEL, TUNNEL, TUNNEL, TUNNEL, TUNNEL },
    { EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY },
    { EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY }
};

static byte leftTTunnelTemplate[ 5 ][ 5 ] = {
    { EMPTY, EMPTY, TUNNEL, EMPTY,  EMPTY },
    { EMPTY, EMPTY, TUNNEL, EMPTY,  EMPTY },
    { EMPTY, EMPTY, TUNNEL, TUNNEL, TUNNEL },
    { EMPTY, EMPTY, TUNNEL, EMPTY,  EMPTY },
    { EMPTY, EMPTY, TUNNEL, EMPTY,  EMPTY }
};

static byte rightTTunnelTemplate[ 5 ][ 5 ] = {
    { EMPTY, EMPTY, TUNNEL, EMPTY,  EMPTY },
    { EMPTY, EMPTY, TUNNEL, EMPTY,  EMPTY },
    { TUNNEL, TUNNEL, TUNNEL, EMPTY, EMPTY },
    { EMPTY, EMPTY, TUNNEL, EMPTY,  EMPTY },
    { EMPTY, EMPTY, TUNNEL, EMPTY,  EMPTY }
};

static byte leftLowerCTunnelTemplate[ 5 ][ 5 ] = {
    { EMPTY, EMPTY, TUNNEL, EMPTY,  EMPTY },
    { EMPTY, EMPTY, TUNNEL, EMPTY,  EMPTY },
    { EMPTY, EMPTY, TUNNEL, TUNNEL, TUNNEL },
    { EMPTY, EMPTY, EMPTY, EMPTY,  EMPTY },
    { EMPTY, EMPTY, EMPTY, EMPTY,  EMPTY }
};

static byte rightLowerCTunnelTemplate[ 5 ][ 5 ] = {
    { EMPTY, EMPTY, TUNNEL, EMPTY,  EMPTY },
    { EMPTY, EMPTY, TUNNEL, EMPTY,  EMPTY },
    { TUNNEL, TUNNEL, TUNNEL, EMPTY, EMPTY },
    { EMPTY, EMPTY, EMPTY, EMPTY,  EMPTY },
    { EMPTY, EMPTY, EMPTY, EMPTY,  EMPTY }
};

static byte rightUpperCTunnelTemplate[ 5 ][ 5 ] = {
    { EMPTY,  EMPTY,  EMPTY,  EMPTY, EMPTY },
    { EMPTY,  EMPTY,  EMPTY,  EMPTY, EMPTY },
    { TUNNEL, TUNNEL, TUNNEL, EMPTY, EMPTY },
    { EMPTY,  EMPTY,  TUNNEL, EMPTY, EMPTY },
    { EMPTY,  EMPTY,  TUNNEL, EMPTY, EMPTY }
};

static byte leftUpperCTunnelTemplate[ 5 ][ 5 ] = {
    { EMPTY,  EMPTY,  EMPTY,  EMPTY, EMPTY },
    { EMPTY,  EMPTY,  EMPTY,  EMPTY, EMPTY },
    { EMPTY,  EMPTY,  TUNNEL, TUNNEL, TUNNEL },
    { EMPTY,  EMPTY,  TUNNEL, EMPTY, EMPTY },
    { EMPTY,  EMPTY,  TUNNEL, EMPTY, EMPTY }
};

void        setLevelMap( byte row, byte col, CELL_TYPE c )  { levelMap[ row ][ col ] = c;    }
CELL_TYPE   getLevelMap( byte row, byte col )               { return levelMap[ row ][ col ]; }


void generateLevel( void ) {
    byte row, col;

    for( row = 0; row < 4; row++ )
        for( col = 0; col < 8; col++ ) {
            if( col > 0 && col < 7 && dieRoll( 1, 100 ) < 16 )
                setLevelMap( row, col, ROOM );
            else
                setLevelMap( row, col, HORIZONTAL );
        }
}


byte neighbors( byte row, byte col ) {
    byte rv = 0;

    for( sbyte rrow = -1; rrow <  2; rrow++ )
        for( sbyte ccol = -1; ccol <  2; ccol++ )
            if( rrow != 0 && ccol != 0 && getLevelMap( (sbyte)row + rrow, (sbyte)col + ccol ) == ROOM )
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
        case HORIZONTAL:
            sourceData = &horizontalTunnelTemplate;
            break;
        case CROSS:
            sourceData = &crossRoadsTunnelTemplate;
            break;
        case LOWER_T:
            sourceData = &lowerTTunnelTemplate;
            break;
        case UPPER_T:
            sourceData = &upperTTunnelTemplate;
            break;
        case LEFT_T:
            sourceData = &leftTTunnelTemplate;
            break;
        case RIGHT_T:
            sourceData = &rightTTunnelTemplate;
            break;
        case CELL_EMPTY:
            sourceData = &emptyTemplate;
            break;
        case LEFT_LOWER_C:
            sourceData = &leftLowerCTunnelTemplate;
            break;
        case RIGHT_LOWER_C:
            sourceData = &rightLowerCTunnelTemplate;
            break;
        case LEFT_UPPER_C:
            sourceData = &leftUpperCTunnelTemplate;
            break;
        case RIGHT_UPPER_C:
            sourceData = &rightUpperCTunnelTemplate;
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
    byte        monstersToAdd = 6 + dieRoll( 1, 3 );        // each level should have 4-7 monsters on it
    byte        monsterHD     = level + 1;           // monsters on par with the player
    byte        monsterToAdd  = 0;
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


void pass1( void ) {
    for( byte row = 0; row < 4; row++ )
        for( byte col = 1; col < 7; col++ )
            if( dieRoll( 1, 2 ) == 1 && getLevelMap( row, col - 1 ) == HORIZONTAL && getLevelMap( row, col ) == HORIZONTAL && getLevelMap( row, col + 1 ) == HORIZONTAL )
                setLevelMap( row, col, ROOM );
}


void pass2( void ) {
    for( byte row = 0; row < 4; row++ )
        for( byte col = 0; col < 7; col++ ) {
            if( getLevelMap( row, col ) == ROOM ) {
                for( byte rrow = row - 1; rrow != 0xff; rrow-- ) {                       // for a byte 0 - 1 == 0xff
                    if( getLevelMap( rrow, col ) == HORIZONTAL && rrow == 0 )            // top row gets UPPER-Ts
                        setLevelMap( rrow, col, UPPER_T );
                    else if( getLevelMap( rrow, col ) == HORIZONTAL && rrow == 3 )       // bottom row gets LOWER-Ts
                        setLevelMap(rrow, col, LOWER_T );
                    else if( getLevelMap( rrow, col ) == HORIZONTAL )                    // otherwise throw down a CROSS
                        setLevelMap( rrow, col, CROSS );
                }
            }
        }
}

CELL_TYPE convertCellSet( CELL_TYPE theCell, CONVERT_DIRECTION d ) {
    CELL_TYPE map[ 3 ][ 3 ] = {
        { UPPER_T, LEFT_UPPER_C, RIGHT_UPPER_C },
        { LOWER_T, LEFT_LOWER_C, RIGHT_LOWER_C },
        { CROSS,   LEFT_T,       RIGHT_T }
    };
    byte col = d == FROM_LEFT ? 1 : 2;
    byte row = theCell == UPPER_T ? 0 : theCell == LOWER_T ? 1 : 2;

    return map[ row ][ col ];
}


void convertCell( byte row, CELL_TYPE oldCell, CONVERT_DIRECTION d ) {
    byte startCol    = d == FROM_LEFT ? 0 : 7;
    sbyte adjustment = d == FROM_LEFT ? 1 : -1;

    for( sbyte col = startCol; ( d == FROM_LEFT ? col < 8 : col >= 0 ); col += adjustment ) {
        if( getLevelMap( row, col ) != CELL_EMPTY ) { 
            if( getLevelMap( row, col ) != oldCell ) {
                return;
            } else {
                setLevelMap( row, col, convertCellSet( oldCell, d ));
                return;            
            }
        }
    }
}


void pass3( void ) {
    byte col;
    byte row;

    for( row = 0; row < 4; row++ ) {
        col = 0;
        while( getLevelMap( row, col ) == HORIZONTAL ) {
            setLevelMap( row, col++, CELL_EMPTY );
            continue;
        }

        col = 7;
        while( getLevelMap(row, col ) == HORIZONTAL ) {
            setLevelMap( row, col--, CELL_EMPTY );
            continue;
        }
    }
}


void pass4( void ) {
    byte row;

    for( row = 0; row < 4; row++ ) {
        convertCell( row, CROSS, FROM_LEFT );
        convertCell( row, CROSS, FROM_RIGHT );        
    }

    convertCell( 0, UPPER_T, FROM_LEFT );
    convertCell( 0, UPPER_T, FROM_RIGHT );
    convertCell( 3, LOWER_T, FROM_LEFT );
    convertCell( 3, LOWER_T, FROM_RIGHT );
}


void addDoors( void ) {   
    for( byte row = 0; row < 4; row++ )
        for( byte col = 0; col < 8; col++ ) {
            if( getLevelMap( row, col ) == ROOM ) {
                byte x1 = col * 5;
                byte y1 = row * 5;
                byte x2 = x1 + 4;
                byte y2 = y1 + 4;

                // horizontal wall check
                for(  byte col = x1; col < x2; col++) {
                    if( mapGetData( col, y1 - 1 ) == TUNNEL )
                        mapSetData( col, y1, randomLockedDoor( HDOOR ));

                    if( mapGetData( col, y2 + 1 ) == TUNNEL )
                        mapSetData( col, y2, randomLockedDoor( HDOOR ));
                }

                for( byte row = y1; row < y2; row++ ) {
                    if( mapGetData( x1 - 1, row ) == TUNNEL )
                        mapSetData( x1, row, randomLockedDoor( VDOOR ));

                    if( mapGetData( x2 + 1, row ) == TUNNEL )
                        mapSetData( x2, row, randomLockedDoor( VDOOR ));
                }
            }
        }
}

void newDungeon( byte level ) {
    newMap();
    generateLevel();
    pass1();            // stuff more rooms in there
    pass2();            // do vertical tunnels, crosses, Ts
    pass3();            // clean horizontal tunnels at the edges
    pass4();            // convert crosses into left-t and right-t.  convert upper-t and lower-t to proper corners if need be.
    updateDungeon();    // convert levelMap to mapData

    addDoors();

    // add basic dungeon stuff
    addAtRandomLocation( STAIRS_UP );
    if( level != 100 )
        addAtRandomLocation( STAIRS_DOWN );
    if( dieRoll( 1, 2 ) == 1 )
        addAtRandomLocation( GWATER );
    if( dieRoll( 1, 2 ) == 1 )
        addAtRandomLocation( G_FOOD );

    // add monsters
    if( level < 100 )      
        populateDungeon( level );
    else {
        roamingCount = 0;    
        roamingMonsterList[ 0 ] = masterMonsterList[ monsterCount - 1 ];     // the main boss
        roamingMonsterList[ 0 ].loc = findRandomFloorSpace();
        roamingMonsterList[ 0 ].hitPoints = 1;
        roamingCount++;
    }

    // sprinkle artifacts
    if( level == 40 || level == 60 || level == 80 )
        addAtRandomLocation( G_ARTIFACT );
}
    