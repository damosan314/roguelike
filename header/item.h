#ifndef __ITEM_H
#define __ITEM_H

#include <stdbool.h>
#include "common.h"

// access structures are expensive on the 6502...so the following two macros help out - the first defines "private" globals.  The second is 
// a wrapper around a function that sets them.
//
#define INCLUDE_ITEM_VARS()  byte           ___id; \
                             EQUIPMENT_TYPE ___eqType; \
                             char           *___name; \
                             LOCATION       ___l; \
                             sbyte          ___modifier; \
                             byte           ___ammo; \
                             byte           ___hitDie; \
                             byte           ___baseDie; \
                             byte           ___reloadsFor; \
                             byte           ___range; \
                             sbyte          ___ac; \
                             byte           ___amount; \
                             EQUIP_LOCATION ___where; \
                             byte           ___status

#define INCLUDE_ITEM_VARS_AS_EXTERN()  \
                            extern byte           ___id; \
                            extern EQUIPMENT_TYPE ___eqType; \
                            extern char           *___name; \
                            extern LOCATION       ___l; \
                            extern sbyte          ___modifier; \
                            extern byte           ___ammo; \
                            extern byte           ___hitDie; \
                            extern byte           ___baseDie; \
                            extern byte           ___reloadsFor; \
                            extern byte           ___range; \
                            extern sbyte          ___ac; \
                            extern byte           ___amount; \
                            extern EQUIP_LOCATION ___where; \
                            extern byte           ___status

#define LOAD_ITEM_VARS( e )  itemLoadVars( e, &___id, &___eqType, ___name, &___l, &___modifier, &___ammo, &___hitDie, &___baseDie, &___reloadsFor, &___range, &___ac, &___amount, &___where, &___status )

typedef enum { EQ_DEFAULT, EQ_QUEST, EQ_FUEL, EQ_EQUIPMENT, EQ_DRINKABLE, EQ_EATABLE, EQ_WEAPON, EQ_AMMO, EQ_ARMOR, EQ_SHIELD, EQ_GOLD } EQUIPMENT_TYPE;
typedef enum { EQUIP_NONE, EQUIP_TORSO, EQUIP_HAND, EQUIP_HANDS, EQUIP_LEFT, EQUIP_RIGHT, EQUIP_BOTH, EQUIP_HEAD } EQUIP_LOCATION;

typedef struct {
    byte x, y;
} LOCATION;

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

void        dropLoot( LOCATION l );
byte        lootIndexByLocation( LOCATION l );
void        itemLoadVars( EQUIPMENT e, 
                          byte *id, EQUIPMENT_TYPE *eqType, char *name, 
                          LOCATION *l, sbyte *modifier, byte *ammo, byte *hitDie,
                          byte *baseDie, byte *reloadsFor, byte *range, 
                          sbyte *ac, byte *amount, EQUIP_LOCATION *where, byte *status  );

#endif