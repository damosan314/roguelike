
#ifndef __PLAYER_H
#define __PLAYER_H

#include "common.h"
#include "item.h"
#include "die.h"

// player actions - drink a potion / water
//
#define A_POTION  0
#define A_WATER   1

extern    word 		  magicPointsAbsorbed;          // quest artifact absorbs spells energy
extern    char        name[ 21 ];
extern    LOCATION    loc;
extern    EQUIPMENT   inventory[ 10 ];
extern    bool        slotUsed[ 10 ];
extern    bool        equipped[ 10 ];
extern    word        experience;
extern    byte        strength;
extern    byte        dexterity;
extern    byte        constitution;
extern    byte        charisma;
extern    byte        hitPoints;
extern    byte        currentHitPoints;
extern    byte        level;
extern    byte        isSick;
extern    byte        isPoisoned;
extern    byte        isFed;
extern    byte        isWatered;
extern    word        gold;
extern    byte        isBlessed;
extern    byte        water;
extern    byte        potions;
extern    byte        rations;
extern    word        turns;

void        newPlayer( char *name );
void        addPlayerInventory( EQUIPMENT item );
sbyte       attributeModifier( byte attribute );
bool        playerIsAlive( void );
byte        findFirstEmptyInventorySlot( void );
void        compressEquipmentSlots( void );
sbyte       computePlayerAC( void );
byte        inventorySlotsUsed( void );
byte        findFirstUsedInventorySlotFrom( byte start  );
bool        verifyEquipStatus( byte slot );
byte        playerHas( ID itemID );
byte        addToPlayerInventory( EQUIPMENT e, bool equipped );
byte        findEquipped( EQUIPMENT_TYPE thing );
byte        playerHasAmmoFor( byte id );
void        addCurrentHitpoints( sbyte amount );
void        sip( byte item );
void        addHitpoints( sbyte amount );
void        addWater( sbyte amount );
void        addGold( word amount );
void        handleEating( void );
void        notEnoughGold( void );
byte        calculateCost( byte baseCost );
byte        removeCursedInventory( void );
byte        incrementStat( byte current );
void        addExperience( void );

#endif