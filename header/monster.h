
#ifndef __MONSTER_H
#define __MONSTER_H

#include "common.h"
#include "item.h"

typedef enum { CF_NONE, CF_UNDEAD, CF_ONLYMAGICALWEAPONS, CF_AUTOHEAL, CF_SPELLS, CF_STEAL, CF_PRAYERS, CF_POISON, CF_SICK, CF_STEALPOWER } CREATURE_FEATURE;

typedef struct {
    byte                id;
    byte                symbol;
    char                name[ 21 ];
    ID                  armedWith;      // ID of primary weapon this monster is armed with
    byte                hitDie;         // for definition the number of d6 for hitpoints, for instantiation the number of hitpoints
    byte                hitPoints;
    bool                sentient;       // is this a smarter creature?
    sbyte               ac;             // armor class of this create
    CREATURE_FEATURE    cf;             // creature special power
    sbyte               cfPower;        // some creature features require an argument
    LOCATION            loc;
} MONSTER;

#endif