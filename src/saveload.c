
#include <stdbool.h>
#include "common.h"
#include "atari.h"
#include "screen.h"
#include "gd.h"
#include "utils.h"

bool loadGame( void ) {
	FILE f;
	
	a_cls();

	f = a_fopen( "D1:RLSAVE.DAT", "r" );
	if( f == 0xff )
		return false;

	footerMessage("LOADING...");

	a_fread( f, &roamingCount, sizeof( byte ));
	a_fread( f, &droppedCount, sizeof( byte ));
	a_fread( f, &currentLevel, sizeof( byte ));
	a_fread( f, &currentTurn, sizeof( word ));
	a_fread( f, &levelStartedAt, sizeof( word ));
	a_fread( f, &roamingMonsterList, 10 * sizeof( MONSTER ));
	a_fread( f, &droppedEquipmentList, 10 * sizeof( EQUIPMENT ));
	a_fread( f, &dungeon, sizeof( MAP_DATA ));
	a_fread( f, &cheat, sizeof( byte ));
	a_fread( f, &magicPointsAbsorbed, sizeof( word ));
	a_fread( f, &name, 21);
	a_fread( f, &loc, sizeof( LOCATION ));
	a_fread( f, &inventory, 10 * sizeof( EQUIPMENT ));
	a_fread( f, &slotUsed, 10 * sizeof( bool ));
	a_fread( f, &equipped, 10 * sizeof( bool ));	
	a_fread( f, &experience, sizeof( word ));
	a_fread( f, &strength, sizeof( byte ));
	a_fread( f, &dexterity, sizeof( byte ));
	a_fread( f, &constitution, sizeof( byte ));
	a_fread( f, &charisma, sizeof( byte ));
	a_fread( f, &hitPoints, sizeof( byte ));
	a_fread( f, &currentHitPoints, sizeof( byte ));
	a_fread( f, &level, sizeof( byte ));
	a_fread( f, &isSick, sizeof( byte ));
	a_fread( f, &isPoisoned, sizeof( byte ));
	a_fread( f, &isFed, sizeof( byte ));
	a_fread( f, &isWatered, sizeof( byte ));
	a_fread( f, &gold, sizeof( word ));
	a_fread( f, &isBlessed, sizeof( byte ));
	a_fread( f, &water, sizeof( byte ));
	a_fread( f, &potions, sizeof( byte ));
	a_fread( f, &rations, sizeof( byte ));
	a_fread( f, &turns, sizeof( word ));
	a_fclose( f );
	a_fdelete( "D1:RLSAVE.DAT" );

	a_cls();

	return true;
}

void saveGame( void ) {
	FILE f; 
	
	banner("SAVE AND EXIT? (Y/N)");
	
	if( yesNo() == UC_KEY_N )
		return;

	footerMessage("SAVING GAME...");
	f = a_fopen("D1:RLSAVE.DAT", "w");

	if( f == 0xff ) {
		footerMessage("UNABLE TO OPEN FILE" );
		shortPause();
		return;
	}

	a_fwrite( f, &roamingCount, sizeof( byte ));
	a_fwrite( f, &droppedCount, sizeof( byte ));
	a_fwrite( f, &currentLevel, sizeof( byte ));
	a_fwrite( f, &currentTurn, sizeof( word ));
	a_fwrite( f, &levelStartedAt, sizeof( word ));
	a_fwrite( f, &roamingMonsterList, 10 * sizeof( MONSTER ));
	a_fwrite( f, &droppedEquipmentList, 10 * sizeof( EQUIPMENT ));
	a_fwrite( f, &dungeon, sizeof( MAP_DATA ));
	a_fwrite( f, &cheat, sizeof( byte ));
	a_fwrite( f, &magicPointsAbsorbed, sizeof( word ));
	a_fwrite( f, &name, 21 );
	a_fwrite( f, &loc, sizeof( LOCATION ));
	a_fwrite( f, &inventory, 10 * sizeof( EQUIPMENT ));
	a_fwrite( f, &slotUsed, 10 * sizeof( bool ));
	a_fwrite( f, &equipped, 10 * sizeof( bool ));	
	a_fwrite( f, &experience, sizeof( word ));
	a_fwrite( f, &strength, sizeof( byte ));
	a_fwrite( f, &dexterity, sizeof( byte ));
	a_fwrite( f, &constitution, sizeof( byte ));
	a_fwrite( f, &charisma, sizeof( byte ));
	a_fwrite( f, &hitPoints, sizeof( byte ));
	a_fwrite( f, &currentHitPoints, sizeof( byte ));
	a_fwrite( f, &level, sizeof( byte ));
	a_fwrite( f, &isSick, sizeof( byte ));
	a_fwrite( f, &isPoisoned, sizeof( byte ));
	a_fwrite( f, &isFed, sizeof( byte ));
	a_fwrite( f, &isWatered, sizeof( byte ));
	a_fwrite( f, &gold, sizeof( word ));
	a_fwrite( f, &isBlessed, sizeof( byte ));
	a_fwrite( f, &water, sizeof( byte ));
	a_fwrite( f, &potions, sizeof( byte ));
	a_fwrite( f, &rations, sizeof( byte ));
	a_fwrite( f, &turns, sizeof( word ));

	a_fclose( f );

	banner( "GAME SAVED...");
	footerMessage("REMOVE DISK");
	while( true )
		;
}
