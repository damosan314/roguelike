
#ifndef __UTILS_H
#define __UTILS_H

#include <stdbool.h>
#include "common.h"
#include "item.h"

extern byte cheat;      // from rl2.c

void        centerText( byte row, char *str );
bool        showFile( char *filename );
void        pause( void );
void        shortPause( void );
ID          findMasterEquipmentByName( char *name );
EQUIPMENT   findMasterEquipmentByID( ID theID );
void        banner( char *msg );
byte        yesNo( void );
void        clearStatusLine( void );
void        footerMessage( char *msg );
void        addKarma( sbyte amount );
void        displayPlayerData( void );
byte        atLeastOne( sbyte value );
word        square_root( word n );
word        range( LOCATION from, LOCATION to );
bool        locEqual( LOCATION a, LOCATION b );
bool        drawLOS( LOCATION a, LOCATION b );
bool        monsterThere( LOCATION l );

#endif