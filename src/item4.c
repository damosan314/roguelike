
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"
#include "item.h"
#include "gd.h"
#include "glyphs.h"
#include "screen.h"
#include "utils.h"

/*
typedef struct {
    byte                    id;                     // the id of this element
    EQUIPMENT_TYPE          eqType;
    char                    name[ 18 ];             //
    LOCATION                loc;                    // when instantiated via a loot drop, save the location...
    byte                    weight;                 
    sbyte                   modifier;
    byte                    ammo;                   // number of shots.
    byte                    hitDie;                 // number of dice to roll for damage
    byte                    baseDie;                // base die type (4, 6, 8, ...) used for damage rolls
    byte                    reloadsFor;             // the id of the item this ammo reloads for
    byte                    range;
    sbyte                   ac;
    byte                    amount;                 // when instantiated, amount of ammo, number of potions...or amount of gold....
    EQUIP_LOCATION          where;
    byte                    status;
} EQUIPMENT;
*/

void itemLoadVars( EQUIPMENT e, byte *id, EQUIPMENT_TYPE *eqType, char *name, LOCATION *l, sbyte *modifier, byte *ammo, byte *hitDie, byte *baseDie, byte *reloadsFor, byte *range, sbyte *ac, byte *amount, EQUIP_LOCATION *where, byte *status  ) {
    *id = e.id;
    *eqType = e.eqType;
    name = (char *)&e.name;
    *l = e.loc;
    *modifier = e.modifier;
    *ammo = e.ammo;
    *hitDie = e.hitDie;
    *baseDie = e.baseDie;
    *reloadsFor = e.reloadsFor;
    *range = e.range;
    *ammo = e.ammo;
    *ac = e.ac;
    *amount = e.amount;
    *where = e.where;
    *status = e.status;
}


void dropLoot( LOCATION l ) {
    EQUIPMENT e;
    byte r;

    if( droppedCount == 10 )
        return;

    if( dieRoll( 1, 4 ) != 1 )
        return;

tryAgain:
    r = dieRoll( 1, equipmentCount ) - 1;
    e = masterEquipmentList[ r ];
    if( e.eqType != EQ_AMMO )
        goto tryAgain;
    
    e.ammo = dieRoll( 6, 6 );
    e.loc = l;

    mapSetDataViaLoc( e.loc, G_LOOT );

    droppedEquipmentList[ droppedCount++ ] = e;
}

byte lootIndexByLocation( LOCATION l ) {
    for( byte i = 0; i < 10; i++ )
        if( locEqual( l, droppedEquipmentList[ i ].loc ))
            return i;

    return 0xff;
}