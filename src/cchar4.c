
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

            addToPlayerInventory( findMasterEquipmentByID( 100 ), true );       // m4
            addToPlayerInventory( findMasterEquipmentByID( 101 ), false );      // m9a1
            
            addToPlayerInventory( findMasterEquipmentByID( 102 ), false );      // 556 ammo
            inventory[ 2 ].ammo = 180;

            addToPlayerInventory( findMasterEquipmentByID( 103 ), false );      // 9mm ammo
            inventory[ 3 ].ammo = 45;

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
