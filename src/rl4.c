//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// simple roguelike for A8
//
// binaries start to blow up at around 36,000 bytes
//
// to build:
//
// 		mos-a800xl-clang -Os -o rl2.xex rl2.c die.c screen.c item.c monster.c player.c map.c dungeon.c input.c cchar.c atari.c cio.s rand.s inventory.c utils.c
//

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "atari.h"
#include "common.h"
#include "screen.h"
#include "item.h"
#include "player.h"
#include "gd.h"
#include "dungeon.h"
#include "monster.h"
#include "cchar.h"
#include "input.h"
#include "glyphs.h"
#include "map.h"
#include "inventory.h"
#include "utils.h"
#include "ai.h"
#include "log.h"
#include "saveload.h"
#include "sound.h"


const EQUIPMENT masterEquipmentList[] = {
	{  0, EQ_DEFAULT,   "NULL",             {0xff,0xff}, 0,   0, 00, 2,  4, 0xff,    0,  0,  0, EQUIP_NONE, 100 },
	{100, EQ_WEAPON,    "M-4", 				{0xff,0xff}, 1,   0, 00, 2,  8, 0xff,   10,  0,  0, EQUIP_HANDS, 100 },
	{101, EQ_WEAPON,    "M9A1", 			{0xff,0xff}, 1,   0, 00, 1,  6, 0xff,    5,  0,  0, EQUIP_HAND, 100 },
	{102, EQ_AMMO,      "5.56 MM", 			{0xff,0xff}, 0,   0, 30, 0,  0,  100,    0,  0,  0, EQUIP_NONE, 100 },
	{103, EQ_AMMO,      "9 MM", 			{0xff,0xff}, 0,   0, 15, 0,  0,  101,    0,  0,  0, EQUIP_NONE, 100 },
	{104, EQ_WEAPON,    "KNIFE", 			{0xff,0xff}, 1,   0, 00, 1,  4, 0xff,    0,  0,  0, EQUIP_HAND, 100 },
	{105, EQ_WEAPON,    "AR-10", 			{0xff,0xff}, 1,   0, 00, 4,  8, 0xff,   15,  0,  0, EQUIP_HANDS, 100 },
	{106, EQ_AMMO,      "7.62 NATO",    	{0xff,0xff}, 0,   0, 20, 0,  0,  105,    0,  0,  0, EQUIP_NONE, 100 },
};


const MONSTER masterMonsterList[] = {
//
// a monster is included in a level if hitDie <= (currentLevel % 10 )
//
//	ID		symbol			name				armedWith	hitDie	hitPoints	sentient	ac	creature_feature		cfPower	location
	{100, 	G_ZOMBIE,	   	"ZOMBIE",			000,  		5, 		0, 			false, 		10, CF_SICK,    			6, 		{0xff,0xff}}
};


// byte offsets for rows on a graphics 0 screen
//
const word yylookup_gr0[] = { 
    0, 40, 80, 120, 160, 200, 240, 280, 320, 360, 400, 440, 480, 520, 560, 600, 640, 680, 720, 760, 800, 840, 880, 920 
};


byte equipmentCount;
byte roamingCount;							// saveable
byte monsterCount;
byte droppedCount;							// saveable
byte currentLevel;							// saveable
word currentTurn;							// saveable
word levelStartedAt;						// saveable
MONSTER roamingMonsterList[ 20 ];			// saveable
EQUIPMENT droppedEquipmentList[ 10 ];		// saveable
MAP_DATA dungeon;							// saveable
LOCATION playerLocation;
byte cheat = 0;								// how cheaty is the player so far
char msg[ 41 ];								// buffer for use with a_sprintf()


// player data (saveable)
//
word 		magicPointsAbsorbed;
char        name[ 21 ];
LOCATION    loc;
EQUIPMENT   inventory[ 10 ];
bool        slotUsed[ 10 ];
bool        equipped[ 10 ];
word        experience;
byte        strength;
byte        dexterity;
byte        constitution;
byte        charisma;
byte        hitPoints;
byte        currentHitPoints;
byte        level;
byte        isSick;
byte        isPoisoned;
byte        isFed;
byte        isWatered;
word        gold;
byte        isBlessed;
byte        water;
byte        potions;
byte        rations;
word        turns;

word		stepsTaken;
word		attacksAttempted;
word		hitsLanded;
word		foodEaten;
word		potionsConsumed;
word		waterDrank;
word		monstersKilled;

const char *leaveOption 	  = "9) LEAVE";
const char *helpOption  	  = "?) HELP";
const char *s_potion    	  = "POTION";
const char *s_water       	  = "WATER";
const char *killed        	  = "KILLED";
const char *hit               = "HIT";
const char *missed            = "MISSED";
const char *cannot_move_there = "YOU CAN'T MOVE THERE";

byte playerdata[ 100 ];
byte *chp, *mhp;
byte test1, test2;


INCLUDE_ITEM_VARS();

void prepareLevel( void ) {
	a_cls();

	centerText( 8, "CREATING NEW TOWN" );

	droppedCount = 0;										// zap dropped items
	roamingCount = 0;										// zap roaming monsters

	newDungeon( currentLevel );								// create next dungeon level
	memset( &dungeon.mapFlags, MAP_FLAG_VISIBLE, sizeof( dungeon.mapFlags ));

	playerLocation   = mapGetStartLocation();					// locate starting point
	loc              = playerLocation;
}


void resultMessage( char *action, char *thing ) {
	footerMessage( a_sprintf( msg, "%s THE %s", action, thing ));
}


void gdInit( void ) {
    currentLevel 	    = 1;        // game starts at level 1...ends at level 100
	equipmentCount      = sizeof( masterEquipmentList ) / sizeof( EQUIPMENT );
	monsterCount		= sizeof( masterMonsterList ) / sizeof( MONSTER );
	roamingCount		= 0;
	droppedCount		= 0;
	currentTurn			= 0;
	newPlayer("PLAYER");
    newMap();
}


bool lootThere( LOCATION l ) {
	byte lootIndex = lootIndexByLocation( l );

	if( lootIndex == 0xff )
		return false;
	else if( droppedEquipmentList[ lootIndex ].id > 0 )
		return true;
	else
		return false;
}



// something can only move to a location if it's a floor or tunnel tile
bool canMoveTo( LOCATION l ) {
	byte dungeonCell = mapGetDataViaLoc( l );

	if( monsterThere( l ))
		return false;

	if( lootThere( l ))
		return true;

	switch( dungeonCell ) {
#ifdef CAN_DIG
		case WALL:
		case WALL1:
		case WALL2:
#endif
		case FLOOR:
		case TUNNEL:
		case STAIRS_DOWN:
		case STAIRS_UP:
		case VDOOR:
		case HDOOR:
		case OHDOOR:
		case OVDOOR:
		case G_FOOD:
		case G_ARTIFACT:
			return true;
			break;
		default:
			return false;
			break;
	}
}


bool canInteractWith( LOCATION l ) {
	byte dungeonCell;

	// is a monster there?
	if( monsterThere( l ))
		return true;

	// loot?
	if( lootThere( l ))
		return true;

	dungeonCell = mapGetDataViaLoc( l );

	switch( dungeonCell ) {

#ifdef CAN_DIG
		case WALL:
		case WALL1:
		case WALL2:
#endif
		case STAIRS_DOWN:
		case STAIRS_UP:
		case VDOOR:
		case HDOOR:
		case LVDOOR:
		case LHDOOR:
		case GWATER:
		case G_FOOD:
		case G_ARTIFACT:
			return true;
			break;
		default:
			return false;
			break;
	}
}


bool spendQuestion( byte amount, char *action ) { 
	byte ch;

	amount += cheat;		// cheaters lose in the end...  right?  right?!?

	footerMessage( a_sprintf(msg, "%s YOU FOR %b GOLD? (Y/N)", action, amount ));
	
	ch = yesNo();

	if( ch == UC_KEY_Y ) {
		if( amount <=  gold ) {
			 gold -= amount;
			return true;
		} else {
			notEnoughGold();
			return false;
		}
	}

	return false;
}


bool handleSquad( void ) { 
	byte ch;

	footerMessage("BACK TO SQUAD? (Y/N)" );

	if( yesNo() == UC_KEY_N ) {
		return true;
	}



	while( true ) {
		banner("SQUAD - WELCOME BACK!");

		a_printStringAt( 4, 10, "1) REPORT IN" );
		a_printStringAt( 5, 10, "2) SEE MEDIT" );
		a_printStringAt( 6, 10, "3) LOAD UP" );
		a_printStringAt( 8, 10, "4) GET BACK TO IT" );
		a_printStringAt( 10, 10, helpOption );

		ch = getch();

		switch( ch ) {
			case KEY_QUESTION:
				showFile("D1:SQUAD.DAT");
				break;

			case KEY_1:
				break;

			case KEY_2:
				break;

			case KEY_3:
				break;

			case KEY_4:
				a_cls();
				return true;
				break;

			
			default:
				break;
		}
	}
}


bool handleNextLevel( void ) { 
	bool nope = false;
	byte i;
	word newLevel;
	byte levels;

	footerMessage("CLIMB DOWN? (Y/N)");
	if( yesNo() == UC_KEY_N )
		return true;

	a_cls();

	prepareLevel();


	currentLevel += 1;
	
	levelStartedAt = currentTurn;

	newLevel =  experience / 1000;
	if( newLevel >  level ) {
		levels =  newLevel -  level;

		centerText( 16, "LEVEL UP!" );

		for( i = 0; i < levels; i++ ) {
			 level += 1;

			// increment a random statistic by 1.  stat range is 1-20.
			switch( dieRoll( 1, 4 )) {
				case 1:	 strength     = incrementStat(  strength ); 	break;
				case 2:	 dexterity    = incrementStat(  dexterity );	break;
				case 3:	 constitution = incrementStat(  constitution );	break;
				case 4:	 charisma     = incrementStat(  charisma );		break;
			}

			// levels 1-10 get new hitpoints
			if(  level <= 10 )
				 hitPoints += atLeastOne( dieRoll( 1, 8 ) + attributeModifier(  constitution ));
		}

		// heals on level up though
		 currentHitPoints =  hitPoints;
		
	}
	pause();
	a_cls();
	return false;
}


bool handleWater( LOCATION l ) { 
	footerMessage("FILL WATERSKIN? (Y/N)" );
	if( yesNo() == UC_KEY_Y ) {

		if( water != 10 )
			addWater( 1 );

		mapSetDataViaLoc( l, FLOOR );
	}
	return true; 
}


bool handleFood( LOCATION l ) {
	footerMessage("PICK UP SUPPLIES? (Y/N)" );
	if( yesNo() == UC_KEY_Y ) {
		if( dieRoll( 1, 2 ) == 1 ) {			// picked up a potion
			footerMessage("A POTION!");
			if( potions != 10 )
				potions++;
		} else {
			footerMessage("A RATION!");
			if( rations != 10 )
				rations++;
		}
		shortPause();
		mapSetDataViaLoc( l, FLOOR );
	}
	return true;
}

bool handleLoot( LOCATION l ) {
	byte itemsFound;
	byte haveThatAmmo;
	bool willAdd = false;
	byte lootIndex = lootIndexByLocation( l );

	if( lootIndex == 0xff ) 
		return false;
	
	LOAD_ITEM_VARS( droppedEquipmentList[ lootIndex ] );

	switch( ___eqType ) {
		case EQ_GOLD:
			addGold( ___amount );
			a_sprintf( msg, "%u GOLD", ___amount );		
			break;
		case EQ_EATABLE:
			if( rations != 10 )
				 rations++;
			a_sprintf( msg, "RATIONS" );
			break;
		case EQ_DRINKABLE:
			itemsFound = dieRoll( 2, 2 );
			potions += itemsFound;
			if( potions > 10 )
				potions = 10;
			a_sprintf( msg, "%u POTIONS", itemsFound );
			break;
		case EQ_WEAPON:
		case EQ_ARMOR:
		case EQ_SHIELD:
			if( ___modifier ) {
				footerMessage( "A MAGICAL ITEM!  PICK IT UP? (Y/N)" );
				if( yesNo() == UC_KEY_N )
					return false;			// let that effer stay on the floor
			}
			willAdd = true;			
			a_sprintf(msg, "A %s", droppedEquipmentList[ lootIndex ].name );
			break;
		case EQ_AMMO:
			haveThatAmmo = playerHas( ___id );
			if( haveThatAmmo == 0xff )
				willAdd = true;
			else
				inventory[ haveThatAmmo ].ammo += ___ammo;
			a_sprintf( msg, "GOT %s", droppedEquipmentList[ lootIndex ].name );
			break;
		default:
			willAdd = true;
			a_sprintf( msg, "GOT %s", name );
			break;
	}


	if( willAdd ) 
		addToPlayerInventory( droppedEquipmentList[ lootIndex ], false );

	droppedEquipmentList[ lootIndex ].id = 0;			// mark item as picked up
	--droppedCount;

	mapSetDataViaLoc( l, FLOOR );

	if( droppedCount > 0 )
		for( byte i = lootIndex + 1; i < 10; i++ )			// compress dropped loot list
			droppedEquipmentList[ i - 1 ] = droppedEquipmentList[ i ];

	footerMessage( msg );
	shortPause();

	return true;
}


bool performInteraction( LOCATION l ) {
	byte dungeonCell;

	if( lootThere( l )) {
		handleLoot( l );
		return false;
	}

	dungeonCell = mapGetDataViaLoc( l );

	switch( dungeonCell ) {
		case STAIRS_UP:		return handleSquad();
		case STAIRS_DOWN:	return handleNextLevel();
		case VDOOR:
		case HDOOR:			return handleDoor( l );
		case GWATER:		return handleWater( l );
		case G_FOOD:		return handleFood( l );
	}
	return false;
}



void showPlayer( void ) {
	word endOfTheWorld = 14400 - currentTurn;
	byte hoursLeft = endOfTheWorld / 60;
	byte minutesLeft = endOfTheWorld % 60;

	a_cls();
	banner( a_sprintf( msg, "STATUS - %b HOURS, %b MINUTES REMAIN", hoursLeft, minutesLeft ));
	addLog( msg + 9);
	displayPlayerData();
	pause();
}


static void paintRawSymbolAtLoc( LOCATION loc, byte symbol ) {
	a_gotoxy( loc.x, loc.y );
	a_putchar_raw( symbol );
}


static void paintRawSymbolAtXY( byte x, byte y, byte symbol ) {
	a_gotoxy( x, y );
	a_putchar_raw( symbol );
}


byte buffer[ 24 * 40 ];

void paintLevel( void ) {

	memcpy( &buffer, &dungeon.mapData, sizeof( dungeon.mapData ));
			
	if( roamingCount != 0 )
		for( byte i = 0; i < roamingCount; i++ ) {
			MONSTER m = roamingMonsterList[ i ];
			if( m.hitPoints > 0 && drawLOS( loc, m.loc )) {
				buffer[ yylookup_gr0[ m.loc.y ] + m.loc.x ] = m.symbol;
			}
		}

	buffer[ yylookup_gr0[ loc.y] + loc.x ] = G_PLAYER;

	memcpy( (byte *)SAVMSC, buffer, sizeof( buffer ));
}


void endOfTurn( void ) {
	if( currentHitPoints < ( hitPoints >> 2 ))
		COLOR4 = 32;
	else if( currentHitPoints <= ( hitPoints >> 1 ))
		COLOR4 = 224;		
	else
		COLOR4 = 0x00;

	
	if( currentTurn % 60 != 0 )
		return;
	
	// handle a cheater's karma coming back into balance  ;)
	if( cheat > 0 )
		addKarma( -1 );

	if( isBlessed > 0 )
		 isBlessed -= 1;
	
	if( isFed > 0 )
		 isFed -= 1;
	
	if( isWatered > 0 )
		 isWatered--;
	
	if( isPoisoned > 0 ) { 
		 isPoisoned -= 1;
		 currentHitPoints -= 1;
	} 

	if( isSick > 0 ) 
		 isSick -= 1;
}


void fire( void ) {
	byte playerWeapon = findEquipped( EQ_WEAPON );
	EQUIPMENT weapon = inventory[ playerWeapon ];
	byte roamingIndex;
	byte minRange = 0xff;

	if( weapon.range == 0 ) {
		footerMessage( "CAN'T FIRE THAT!" );
		return;
	}

	// get inventory slot where the ammo resides
	byte ammoInInventory = playerHasAmmoFor( weapon.id );
	if( ammoInInventory == 0xff ) {
		footerMessage("NO AMMO!");
		return;
	}

	// do we have ammo left?
	if( inventory[ ammoInInventory ].ammo == 0 ) {
		footerMessage("NO AMMO!");
		return;
	} else
		--inventory[ ammoInInventory ].ammo;

	// find closest visible target
	roamingIndex = 0xff;
	for( byte i = 0; i < roamingCount; i++ ) {

		// if the monster is alive...
		if( roamingMonsterList[ i ].hitPoints > 0 ) {
			// can we see it?
			if( drawLOS( loc, roamingMonsterList[ i ].loc )) {
				// get range ... is it shorter than the current minimum range?
				byte r = range( loc, roamingMonsterList[ i ].loc );
				if( r < minRange ) {
					minRange = r;
					roamingIndex = i;			// yup - store target
				}
			}
		}
	}

	if( roamingIndex == 0xff )
		return;

	// is the target in range?
	if( minRange <= weapon.range ) {
		_whichTrack( SOUND_GUNSHOT );
		byte attackRoll = dieRoll( 1, 20 ) + attributeModifier(  dexterity ) + level;
		if( attackRoll > ( 20 - roamingMonsterList[ roamingIndex ].ac )) {
			byte damage = dieRoll( weapon.hitDie, weapon.baseDie );
			hitsLanded++;
			paintRawSymbolAtLoc( roamingMonsterList[ roamingIndex ].loc, 0x4c );
			
			if( damage > roamingMonsterList[ roamingIndex ].hitPoints ) {
				monstersKilled++;
				roamingMonsterList[ roamingIndex ].hitPoints = 0;
				dropLoot( roamingMonsterList[ roamingIndex ].loc );
				addExperience();
			} else {
				roamingMonsterList[ roamingIndex ].hitPoints -= damage;
			}
		}
	} 
}



void updatePlayerLocation( LOCATION newLocation ) {
	bool stairsDown = mapGetDataViaLoc( newLocation ) == STAIRS_DOWN;

	if( canInteractWith( newLocation ))
		if( !performInteraction( newLocation ))
			return;
			
	if( canMoveTo( newLocation )) {
		_whichTrack( SOUND_STEP );
		loc = newLocation;
		experience += 1;
	} 
}


void clearAndPaint( void ) {
	a_cls();
	paintLevel();
}


void updatePlayer() {
	updatePlayerLocation( playerLocation );
	stepsTaken++;
}

void _soundTest( void );

int main( void ) {
	byte playerInput;
	byte d;
	bool monstersAct = true;

    COLOR1  = 0x08;
    COLOR2  = 0x00;
    COLOR4  = 0x00;		// border black - changes based on health

	atari_init();
	_initVBI();

	while( true ) {
		_soundTest();
	}

playAgain:

    COLOR1  = 0x08;
    COLOR2  = 0x00;
    COLOR4  = 0x00;		// border black - changes based on health
    NOCLIK  = true;

    a_cls();

	gdInit();
	
	showFile( "D1:INTRO.DAT" );

    if( !loadGame() ) {
		showFile( "D1:STORY.DAT" );
		createCharacter();
		prepareLevel();
		levelStartedAt = 0;
	} else
		playerLocation =  loc;
	
	a_cls();

	while( playerIsAlive() ) {
		paintLevel();

		playerLocation =  loc;

		currentTurn += 1;

		playerInput = 0xff;

		if( kbhit() )
			playerInput = getch();
		else {
			byte stick = STICK0;
			byte button = STRIG0;

			if( button == STICK_TRIGGER_PRESSED )
				playerInput = UC_KEY_F;
			else {
				switch( stick ) {
					case STICK_LEFT:
						playerInput = UC_KEY_A;
						break;
					case STICK_RIGHT:
						playerInput = UC_KEY_D;
						break;
					case STICK_UP:
						playerInput = UC_KEY_W;
						break;
					case STICK_DOWN:
						playerInput = UC_KEY_S;
						break;
				}
			}
		}		

		switch( playerInput ) {
			case 0xff:
				break;
			case UC_KEY_W:
			case LC_KEY_W:
				if( playerLocation.y != 0 ) {
					--playerLocation.y;
					updatePlayer();
				}
				break;
			case UC_KEY_A:
			case LC_KEY_A:
				if( playerLocation.x != 0 ) {
					--playerLocation.x;
					updatePlayer();
				}
				break;
			case UC_KEY_S:
			case LC_KEY_S:
				if( playerLocation.y < 21 ) {
					++playerLocation.y;
					updatePlayer();
				}
				break;
			case UC_KEY_D:
			case LC_KEY_D:
				if( playerLocation.x < 40 ) {
					++playerLocation.x;
					updatePlayer();
				}
				break;
			case UC_KEY_E:
			case LC_KEY_E:
				handleEating();
				break;
			case UC_KEY_Q:
			case LC_KEY_Q:
				saveGame();
				break;
			case UC_KEY_I:
			case LC_KEY_I:
				handleInventory();
				clearAndPaint();
				break;
			case KEY_QUESTION:
				showFile( "D1:LEVEL.DAT" );
				a_cls();
				paintLevel();
				break;
			case UC_KEY_F:
			case LC_KEY_F:
				fire();
				break;
			case UC_KEY_C:
			case LC_KEY_C:
				addKarma( 100 );				// bad bad player...
				addGold( 1000 );				// gimme dat gold!
				footerMessage( "CHEATER!" );
				break;
			case UC_KEY_L:
			case LC_KEY_L:
				showLog();
				clearAndPaint();				
				break;
			case UC_KEY_P:
			case LC_KEY_P:
				showPlayer();
				clearAndPaint();
				break;
			case KEY_8:
				sip( A_WATER );
				break;
			case KEY_9:
				sip( A_POTION );
				break;
			default:
				// default ignore everything else
				break;
		}
		if( currentTurn % 4 == 0 )
			monsterAI();

		endOfTurn();
	}

	if(playerHas( 250 ) == 0xff )
		showFile("D1:FAIL.DAT");
	else
		showFile("D1:GAMEWIN.DAT");

	goto playAgain;

    return 0;
}
