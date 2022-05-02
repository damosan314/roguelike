
#include <stdbool.h>
#include "common.h"
#include "atari.h"
#include "input.h"
#include "inventory.h"
#include "gd.h"
#include "glyphs.h"
#include "screen.h"
#include "player.h"
#include "utils.h"
#include "log.h"

INCLUDE_ITEM_VARS_AS_EXTERN();

sbyte itemModifier( byte which )      { return inventory[ which ].modifier; }
EQUIPMENT_TYPE itemType( byte which ) { return inventory[ which ].eqType;   }

static void showInventory( void ) {
    byte i;
    sbyte m;
    byte low, high;

    for( i = 0; i < 10; i++ ) {
        a_printStringAt( 5 + i, 1, a_sprintf(msg, "%b)", i ));

        

        if( slotUsed[ i ] ) {

            LOAD_ITEM_VARS( inventory[ i ] );

            a_printStringAt( 5+ i, 4, a_sprintf( msg, "%s",  inventory[ i ].name ));

            a_gotoxy( 23, 5 + i );
            if( equipped[ i ] )
                a_putchar_raw( G_PLAYER );

            a_gotoxy( 24, 5 + i );
            if( ___status == 0 )
                a_putchar_raw( G_STATUS_000 );
            else if( ___status <= 25 )
                a_putchar_raw( G_STATUS_025 );
            else if( ___status <= 50 )
                a_putchar_raw( G_STATUS_050 );
            else if( ___status <= 75 )
                a_putchar_raw( G_STATUS_075 );
            else if( ___status <= 100 )
                a_putchar_raw( G_STATUS_100 );

            a_gotoxy( 25, 5 + i );
            
            m = ___modifier;

            if( m != 0 ) {
                if( m >= 1 ) 
                    a_printf("+%b", ___modifier );
                else
                    a_putchar_raw( G_CURSED );
            }

            // cursed items aren't usable...so don't show anything for them.
            if( m >= 0 ) {
                a_gotoxy( 27, 5 + i );
                if( itemType( i ) == EQ_WEAPON ) {
                    low = ___hitDie + ___modifier + attributeModifier( strength );
                    high = ( ___hitDie * ___baseDie ) + ___modifier + attributeModifier( strength );
                    a_printf( a_sprintf( msg, "D: %b-%b", low, high ));
                } else if( itemType( i ) == EQ_ARMOR ) {
                    a_printf( a_sprintf(msg,  "A: %d", ___ac - ( ___modifier + attributeModifier( dexterity ))));
                } else if( itemType( i ) == EQ_SHIELD ) {                
                    a_printf( a_sprintf( msg, "A: %d", ___ac + ___modifier ));
                } else if( itemType( i ) == EQ_AMMO ) {
                    a_printf(                 "S: %b",  ___ammo );
                }
            }
        } 
    }
}

byte inventoryPrompt( char *prompt ) { 
    byte ch;
    
    footerMessage( a_sprintf(msg, "%s (0-9)?", prompt ));

    while( true ) {
        ch = getch();
        switch( ch ) {
            case KEY_0:      return 0;
            case KEY_1:      return 1;
            case KEY_2:      return 2;
            case KEY_3:      return 3;
            case KEY_4:      return 4;
            case KEY_5:      return 5;
            case KEY_6:      return 6;
            case KEY_7:      return 7;
            case KEY_8:      return 8;
            case KEY_9:      return 9;
            case KEY_ENTER:  return 0xff;
        }
    }
}

void emptySlotAndCompress( byte which ) {
    slotUsed[ which ] = false;
    equipped[ which ] = false;
    compressEquipmentSlots();
}

word calculateGold( word multiplier, word base ) {
    return multiplier * (base + attributeModifier(  charisma ));
}

void sellGear( void ) {
    byte ch;
    word gold;
    EQUIPMENT e;

    while( true ) {
        a_cls();

        banner( "SELL" );
        
        showInventory();

        footerMessage( " CMD (S, F): " );
        switch( getch() ) {
            case UC_KEY_F:
            case LC_KEY_F:
                return;
            case UC_KEY_S:
            case LC_KEY_S:
                ch = inventoryPrompt( "SELL" );
                if( ch != 0xff ) {
                    e =  inventory[ ch ];

                    if( e.modifier >= 0 ) {
                        if( e.eqType == EQ_WEAPON )
                            gold = calculateGold( e.baseDie, 5 );
                        else if( e.eqType == EQ_ARMOR )
                            gold = calculateGold( 10 - e.ac, 50 );
                        else if( e.eqType == EQ_SHIELD )
                            gold = calculateGold( e.ac, 25 );

                        // magic stuff is worth more!
                        gold = calculateGold( gold, 1 + e.modifier );

                        footerMessage( a_sprintf( msg, "SELL %s FOR %u? (Y/N)", e.name, gold ));
                        if( yesNo() == UC_KEY_Y ) {
                             gold += gold;
                            emptySlotAndCompress( ch );
                            addLog("SOLD");
                        }
                    }
                }
                break;
        }
    }
}

void handleInventory( void ) {
    byte which;

    while( true ) {

        a_cls();
        banner( "INVENTORY" );

        showInventory();

        footerMessage( "CMD (E, U, D, F, ?):" );

        switch( getch() ) {
            case KEY_QUESTION:
                showFile( "D1:INV.DAT");
                break;

            case UC_KEY_F:                 // all done with inventory
            case LC_KEY_F:
                return;

            case UC_KEY_D:                 // drop what item?
            case LC_KEY_D:
                if((which = inventoryPrompt("DROP")) == 0xff )
                    break;

                if( slotUsed[ which ] ) {
                    if( inventory[ which ].eqType == EQ_QUEST ) {
                        footerMessage( "THAT'S AN ARTIFACT!" );
                        shortPause();
                    } else {
                        footerMessage( a_sprintf(msg, "DROP THE %s? (Y/N)",  inventory[ which ].name ));
                        if( yesNo() == UC_KEY_Y ) {
                            if( itemModifier( which ) >= 0 ) {
                                emptySlotAndCompress( which );
                                addLog("DROPPED");
                            } else {
                                footerMessage( "YOU ARE CURSED!" );
                                shortPause();
                            }
                        }
                    }
                }
                break;
            case UC_KEY_E:                 // equip what item?
            case LC_KEY_E:
                which = inventoryPrompt("EQUIP");
                if( inventory[ which ].modifier < 0  ) {
                    footerMessage("IT IS CURSED!");
                    shortPause();
                } else if( slotUsed[ which ] )
                    if(  inventory[ which ].where != EQUIP_NONE ) {
                        (void)verifyEquipStatus( which );
                    }
                break;
            case UC_KEY_U:                 // unequip what item?
            case LC_KEY_U:
                which = inventoryPrompt("UNEQUIP");
                if( slotUsed[ which ] )
                    if( equipped[ which ] == true )
                        equipped[ which ] = false;
                break;
            default:
                break;
        }
    }   
}