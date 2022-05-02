
#include <stdlib.h>
#include <string.h>
#include "atari.h"
#include "screen.h"
#include "common.h"
#include "item.h"
#include "die.h"
#include "player.h"
#include "gd.h"
#include "inventory.h"
#include "utils.h"


void addExperience( void ) {
	experience += dieRoll( 100 - currentLevel, 2 );
	if( experience > (unsigned int)50000U )
		experience = (unsigned int)50000U;
}


byte incrementStat( byte current ) {
	if( current < 20 )
		return current+1;
	else
		return current;
}


byte removeCursedInventory( void ) {
	byte rv = 0;
	byte i;
nextCursedItem:
	for( i = 0; i < 10; i++ ) {
		if( slotUsed[ i ] ) {
			if( itemModifier( i ) < 0 ) {
				slotUsed[ i ] = false;
				compressEquipmentSlots();
				rv++;
				goto nextCursedItem;
			}
		}
	}
	return rv;
}


byte calculateCost( byte baseCost ) { return  baseCost - attributeModifier(  charisma ); }

void notEnoughGold( void ) {
	footerMessage("NOT ENOUGH GOLD!");
	shortPause();
}

// modify player's base hitpoints...set current to base (used for leveling up)
void addHitpoints( sbyte amount ) {
	hitPoints += amount;
	currentHitPoints =  hitPoints;
}


void addWater( sbyte amount ) {
	water += amount;
	if(  water > 10 )
		 water = 10;
}

void addGold( word amount ) {
	gold += amount;
	if( gold > 1000 )
		 gold = 1000;
}

void handleEating( void ) {
	footerMessage("EAT A RATION? (Y/N)" );
	if( yesNo() == UC_KEY_Y ) {
		if( rations != 0 ) {
			foodEaten++;
			rations--;
			isFed = 255;
			footerMessage("MMM...");
		} else {
			footerMessage("NO RATIONS!" );
		}
	}
}


void sip( byte item ) {
	char *thingToDrink = ( item == A_POTION ? "POTION" : "WATER" );
	byte quantity 	   = ( item == A_POTION ? potions :  water );
	byte points;

	if( quantity == 0 ) {
		footerMessage( a_sprintf( msg, "NO %s AVAILABLE", thingToDrink ));
	} else {
		footerMessage( a_sprintf( msg, "DRINK %s? (Y/N)", thingToDrink ));
		if( yesNo() == UC_KEY_Y ) {
			if( item == A_POTION ) {
				++potionsConsumed;
				points = dieRoll( 2, 6 );
				addCurrentHitpoints( points );
				--potions;
				footerMessage( a_sprintf(msg, "YOU HEALED FOR %b", points ));
			} else {
				++waterDrank;
				isWatered = 250;
				--water;
				footerMessage( "GULP!" );
			}
		}
	}
}


void addCurrentHitpoints( sbyte amount ) {
	 currentHitPoints += amount;
	if(  currentHitPoints >  hitPoints )
		 currentHitPoints =  hitPoints;
}


byte playerHasAmmoFor( byte id ) {
    for( byte i = 0; i < 10; i++ )
        if( slotUsed[ i ] && inventory[ i ].eqType == EQ_AMMO && inventory[ i ].reloadsFor == id )
            return i;
    
    return 0xff;
}

// the player would like to equip something so unequip other stuff as needed...
bool  verifyEquipStatus( byte slot ) {
    byte i;
    EQUIPMENT_TYPE eq;

    // selected item is not equipable so bail
    if( EQUIP_NONE ==  inventory[ slot ].where )
        return false;
    
    // determine the class of equipment we're equipping...
    eq =  inventory[ slot ].eqType;
    
    // scan inventory unequipping everything of that class (WEAPON, ARMOR, SHIELD)...
    for( i = 0; i < 10; i++ )
        if( slotUsed[ i ] )
            if( inventory[ i ].eqType == eq )
                equipped[ i ] = false;
    
    // marked the selected item as being equipped
    equipped[ slot ] = true;
    return true;
}

byte findEquipped( EQUIPMENT_TYPE thing ) {
    for( byte i = 0; i < 10; i++ )
        if( slotUsed[ i ] && equipped[ i ] && inventory[ i ].eqType == thing )
            return i;
    return 0xff;
}

sbyte computePlayerAC( void ) {
    sbyte acFromArmor = 10;
    byte i;

    // player wearing armor?
    i = findEquipped( EQ_ARMOR );
    if( i != 0xff )
        acFromArmor = inventory[ i ].ac - itemModifier( i );

    // player has a shield?
    i = findEquipped( EQ_SHIELD );
    if( i != 0xff ) 
        acFromArmor  -=  inventory[ i ].ac + itemModifier( i );

    // modify AC based on player's dexterity
    acFromArmor -= attributeModifier( dexterity );

    // finally if the player is blessed their armor is increased by one
    if( isBlessed )
        acFromArmor -= 1;

    return acFromArmor;
}

byte inventorySlotsUsed( void ) {
    byte rv = 0;
    sbyte i;

    for( i = 0; i < 10; i++ )
        if( slotUsed[ i ] == true )
            rv++;

    return rv;
}

byte findFirstEmptyInventorySlot( void ) {
    for( byte i = 0; i < 10; i++ )
        if( slotUsed[ i ] == false )
            return i;
    return 0xff;
}

byte findFirstUsedInventorySlotFrom( byte start  ) {
    for( byte i = start; i < 10; i++ )
        if( slotUsed[ i ] == true )
            return i;
    return 0xff;
}

void compressEquipmentSlots( void ) {
    sbyte slotsUsed = inventorySlotsUsed();
    byte freeSlot, nextUsedSlot;

    if( 0 == slotsUsed || 10 == slotsUsed ) // player inventory is full or empty?
        return;                             // no need to compress

    while( true ) {

        freeSlot = findFirstEmptyInventorySlot();
        if( freeSlot == 0xff )    // no free slots
            return;

        nextUsedSlot = findFirstUsedInventorySlotFrom( freeSlot );
        if( nextUsedSlot == 0xff )   // no used slots after freeSlot
            return;

        // overwrite this empty slot with the next availble used slot...
        inventory[ freeSlot ] = inventory[ nextUsedSlot ];
        slotUsed[ freeSlot ]  = slotUsed[ nextUsedSlot ];
        equipped[ freeSlot ]  = equipped[ nextUsedSlot ];

        // mark next used slot up...
        slotUsed[ nextUsedSlot ] = false;
    }

}

bool playerIsAlive( void ) {
    return currentHitPoints > 0;
}

void newPlayer( char *name ) {
    byte i;

    strcpy( name, "PLAYER" );
    loc.x       = 0xff;
    loc.y       = 0xff;
    experience  = 0;
    level       = 0;
    isBlessed   = 0;
    isSick      = 0;
    isPoisoned  = 0;
    isWatered   = 0;
    hitPoints   = 0;
    gold        = 0;
    water       = 0;
    potions     = 0;
    rations     = 0;

    magicPointsAbsorbed = 0;        // for use against final boss

    currentHitPoints = 0;
    
    for( i = 0; i < 10; i++ ) {
        equipped[ i ] = false;
        slotUsed[ i ] = false;
    }
}

sbyte attributeModifier( byte attribute ) {
    sbyte mods[] = { -3,-2,-1,-1,0,0,0,0,0,0,0,0,1,1,2,3,4,5 };
    return mods[ attribute - 3 ];
}


byte playerHas( ID itemID ) {
    for( byte i = 0; i < 10; i++ )
        if( slotUsed[ i ] && inventory[ i ].id == itemID )
            return i;

    return 0xff;
}


byte addToPlayerInventory( EQUIPMENT e, bool shouldBeEquipped ) {
	byte newSlot;
    
    newSlot = findFirstEmptyInventorySlot();
	
    if( newSlot != 0xff ) {
        inventory[ newSlot ] = e;
	    slotUsed[ newSlot ]  = true;
        equipped[ newSlot ]  = shouldBeEquipped;
    }

    return newSlot;
}
