
#include <string.h>
#include "atari.h"
#include "input.h"
#include "screen.h"
#include "player.h"
#include "gd.h"
#include "die.h"
#include "item.h"
#include "player.h"
#include "utils.h"

void createCharacter( void ) {
    word cc = 0;
    byte ch;
    bool cheating = false;

    while( true ) {
        a_cls();

        banner( "PLAYER STATISTICS" );

        while( true ) {
cheat:
            if( cheating == true )
                strength = dexterity = constitution = charisma = 18;
            else {
                strength         = dieRoll( 3, 6 );
                dexterity        = dieRoll( 3, 6 );
                constitution     = dieRoll( 3, 6 );
                charisma         = dieRoll( 3, 6 );
            }

            hitPoints        = 10 + atLeastOne( (cheating == true ? 6 : dieRoll( 1, 6 )) + attributeModifier( constitution ));
            currentHitPoints = hitPoints;
            water            = dieRoll( 2, (cheating == true ? 6 : 3 ));
            potions          = dieRoll( 2, (cheating == true ? 6 : 3 ));
            rations          = dieRoll( 2, (cheating == true ? 6 : 3 ));
            isFed            = 250;
            isWatered        = 250;
            isBlessed        = 0;
            isPoisoned       = 0;
            isSick           = 0;

            slotUsed[ 0 ] = slotUsed[ 1 ] = slotUsed[ 2 ] = slotUsed[ 3 ] = false;

            addToPlayerInventory( findMasterEquipmentByID( 102 ), true );       // longsword
            addToPlayerInventory( findMasterEquipmentByID( 117 ), true );       // leather armor
            addToPlayerInventory( findMasterEquipmentByID( 122 ), true );       // buckler
#ifdef CAN_DIG
            addToPlayerInventory( findMasterEquipmentByID( 129 ), false );       // pick axe
#else
            addToPlayerInventory( findMasterEquipmentByID( 126 ), false );       // lockpicks
#endif
	        displayPlayerData();

            footerMessage("GOOD? (Y/N)");

            ch = yesNo();

            if( ch == UC_KEY_C && cheating == false ) {
                banner("CHEATING...");
                cheating = true;
                addKarma( 100 );
                goto cheat;
            }

            if( ch == UC_KEY_Y )
                return;

        }
    }
}
