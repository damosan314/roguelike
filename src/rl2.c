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

// forward decl
bool attack( LOCATION l );

const EQUIPMENT masterEquipmentList[] = {
	{  0, EQ_DEFAULT,   "NULL",             {0xff,0xff}, 0,   0, 00, 1,  4, 0xff,    0,  0,  0, EQUIP_NONE, 100 },
	{100, EQ_WEAPON,    "DAGGER", 			{0xff,0xff}, 1,   0, 00, 1,  4, 0xff,    0,  0,  0, EQUIP_HAND, 100 },
	{101, EQ_WEAPON,    "SHORT SWORD", 		{0xff,0xff}, 1,   0, 00, 1,  6, 0xff,    0,  0,  0, EQUIP_HAND, 100 },
	{102, EQ_WEAPON,    "LONG SWORD",  		{0xff,0xff}, 1,   0, 00, 1,  8, 0xff,    0,  0,  0, EQUIP_HAND, 100 },
	{103, EQ_WEAPON,    "2H SWORD",			{0xff,0xff}, 1,   0, 00, 1, 10, 0xff,    0,  0,  0, EQUIP_HANDS, 100 },
	{104, EQ_WEAPON,    "LONG BOW",			{0xff,0xff}, 1,   0, 00, 1,  8, 0xff,   10,  0,  0, EQUIP_HANDS, 100 },
	{105, EQ_WEAPON,    "CROSSBOW",			{0xff,0xff}, 1,   0, 00, 2,  4, 0xff,   10,  0,  0, EQUIP_HANDS, 100 },
	{106, EQ_WEAPON,    "SLING",			{0xff,0xff}, 1,   0, 00, 1,  6, 0xff,    5,  0,  0, EQUIP_HAND, 100 },
	{107, EQ_WEAPON,    "MACE",				{0xff,0xff}, 1,   0, 00, 1,  6, 0xff,    0,  0,  0, EQUIP_HAND, 100 },
	{108, EQ_WEAPON,    "SPEAR",			{0xff,0xff}, 1,   0, 00, 1,  6, 0xff,    0,  0,  0, EQUIP_HAND, 100 },
	{109, EQ_WEAPON,    "BASTARD SWORD",	{0xff,0xff}, 1,   0, 00, 2,  4, 0xff,    0,  0,  0, EQUIP_HAND, 100 },
	{110, EQ_WEAPON,    "CLUB", 			{0xff,0xff}, 1,   0, 00, 1,  6, 0xff,    0,  0,  0, EQUIP_HAND, 100 },
	{111, EQ_WEAPON,    "AXE",				{0xff,0xff}, 1,   0, 00, 1,  6, 0xff,    0,  0,  0, EQUIP_HAND, 100 },
	{112, EQ_WEAPON,    "BATTLE AXE",		{0xff,0xff}, 1,   0, 00, 1,  8, 0xff,    0,  0,  0, EQUIP_HANDS, 100 },
	{113, EQ_AMMO,      "ARROWS", 			{0xff,0xff}, 0,   0, 10, 0,  0,  104,    0,  0,  0, EQUIP_NONE, 100 },
	{114, EQ_AMMO,      "BOLTS", 			{0xff,0xff}, 0,   0, 10, 0,  0,  105,    0,  0,  0, EQUIP_NONE, 100 },
	{115, EQ_AMMO,      "STONES", 			{0xff,0xff}, 0,   0, 10, 0,  0,  106,    0,  0,  0, EQUIP_NONE, 100 },
	{116, EQ_ARMOR,     "CLOTHES",          {0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  10, 0, EQUIP_TORSO, 100 },
	{117, EQ_ARMOR,     "LEATHER",          {0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  8,  0, EQUIP_TORSO, 100 },
	{118, EQ_ARMOR,     "RING MAIL",        {0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  7,  0, EQUIP_TORSO, 100 },
	{119, EQ_ARMOR,     "CHAIN MAIL",       {0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  5,  0, EQUIP_TORSO, 100 },
	{120, EQ_ARMOR,     "PLATE MAIL",       {0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  4,  0, EQUIP_TORSO, 100 },
	{121, EQ_ARMOR,     "FULL PLATE",       {0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  1,  0, EQUIP_TORSO, 100 },
	{122, EQ_SHIELD,    "BUCKLER",          {0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  1,  0, EQUIP_HAND, 100 },
	{123, EQ_SHIELD,    "SHIELD",          	{0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  2,  0, EQUIP_HAND, 100 },
	{124, EQ_SHIELD,    "TOWER SHIELD",     {0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  3,  0, EQUIP_HAND, 100 },
	{125, EQ_GOLD,      "GOLD",             {0xff,0xff}, 0,   0, 00, 1,  6, 0xff,    0,  0,  0, EQUIP_NONE, 100 },
	{126, EQ_DEFAULT,   "LOCKPICKS",        {0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  0,  0, EQUIP_NONE, 100 },
	{127, EQ_DRINKABLE, "POTION",           {0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  0,  0, EQUIP_NONE, 100 },
	{128, EQ_EATABLE,   "RATIONS",          {0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  0,  0, EQUIP_NONE, 100 },
	{129, EQ_AMMO,      "ARROWS", 			{0xff,0xff}, 0,   0, 10, 0,  0,  104,    0,  0,  0, EQUIP_NONE, 100 },
	{130, EQ_AMMO,      "BOLTS", 			{0xff,0xff}, 0,   0, 10, 0,  0,  105,    0,  0,  0, EQUIP_NONE, 100 },
	{131, EQ_AMMO,      "STONES", 			{0xff,0xff}, 0,   0, 10, 0,  0,  106,    0,  0,  0, EQUIP_NONE, 100 },
	{132, EQ_AMMO,      "ARROWS", 			{0xff,0xff}, 0,   0, 10, 0,  0,  104,    0,  0,  0, EQUIP_NONE, 100 },
	{133, EQ_AMMO,      "BOLTS", 			{0xff,0xff}, 0,   0, 10, 0,  0,  105,    0,  0,  0, EQUIP_NONE, 100 },
	{134, EQ_AMMO,      "STONES", 			{0xff,0xff}, 0,   0, 10, 0,  0,  106,    0,  0,  0, EQUIP_NONE, 100 },
#ifdef CAN_DIG
	{199, EQ_WEAPON,    "PICKAXE",          {0xff,0xff}, 0,   0, 00, 1,  2, 0xff,    0,  0,  0, EQUIP_HANDS, 100 },
#endif
	{200, EQ_QUEST,     "THE LORATH",       {0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  0,  0, EQUIP_NONE, 100 },	// immune to magic - lvl 20+
	{201, EQ_QUEST,     "DREAD TORC",       {0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  0,  0, EQUIP_NONE, 100 },	// makes all attacks magical - lvl 60+
	{202, EQ_QUEST,     "THE KEY",        	{0xff,0xff}, 0,   0, 00, 0,  0, 0xff,    0,  0,  0, EQUIP_NONE, 100 },	// key to last level - lvl 80+
	{250, EQ_WEAPON,    "ANARAK",           {0xff,0xff}, 0,   0, 00, 4, 10, 0xff,    0,  0,  0, EQUIP_HAND, 100 }
};


#ifdef CAN_DIG
#define PICKAXE 199
#endif


const MONSTER masterMonsterList[] = {
//
// a monster is included in a level if hitDie <= (currentLevel % 10 )
//
//	ID		symbol			name				armedWith	hitDie	hitPoints	sentient	ac	creature_feature		cfPower	location
	{100, 	G_RATS,	   		"RATS",				000,  		1, 		0, 			false, 		10, CF_NONE,    			0, 		{0xff,0xff}},
	{101, 	G_SKELETON,  	"SKELETON", 		100,  		1, 		0, 			false, 		10, CF_UNDEAD,  			0, 		{0Xff,0Xff}},
	{102, 	G_SKELETON,  	"ARMORED SKELETON",	103,  		1, 		0, 			false,  	8, 	CF_NONE,    			0, 		{0xff,0xff}},
	{103, 	G_ZOMBIE,	   	"ZOMBIE",			000,  		1, 		0, 			false, 		10, CF_SICK,    			6, 		{0xff,0xff}},
	{104, 	G_CANINE,    	"WILD DOG",			000,  		1, 		0, 			false,  	9, 	CF_NONE,    			0, 		{0xff,0xff}},
	{105, 	G_CANINE,    	"RABID DOG",		000,  		2, 		0, 			false,  	8, 	CF_POISON, 				10, 	{0xff,0xff}},
	{106, 	G_BARBARIAN, 	"BARBARIAN",       	112,  		1, 		0, 			false, 		10, CF_NONE,    			0, 		{0xff,0xff}},
	{108, 	G_FIGHTER,   	"WARRIOR",       	103,  		1, 		0, 			true,   	8, 	CF_NONE,    			0, 		{0xff,0xff}},
	{109, 	G_THIEF,     	"THIEF",       		101,  		1, 		0, 			true,  		10, CF_STEAL,   			0, 		{0xff,0xff}},
	{110, 	G_MAGE,      	"MAGE",				100,  		3, 		0, 			true,  		10, CF_SPELLS,  			9, 		{0xff,0xff}},
	{111, 	G_CLERIC,    	"ACOLYTE",          100,  		1, 		0, 			true,  		10, CF_NONE,    			0, 		{0xff,0xff}},
	{112,   G_CLERIC, 		"CULTIST",			000,		1,		0,			true,		10, CF_NONE,				0,		{0xff,0xff}},
	{113,   G_CLERIC,  		"THRALL",			000,		1,		0,			false,		10, CF_NONE,				0,		{0xff,0xff}},
	{113, 	G_CLERIC,    	"CLERIC",           107, 		6, 		0, 			true,   	2, 	CF_PRAYERS, 			9, 		{0xff,0xff}},
	{114,	G_ZOMBIE,		"WHITE",			000,		7,		0,			true,		5,	CF_STEALPOWER,			1,		{0xff,0xff}},
	{200, 	G_BOSS,      	"ALDIRAK",          250, 		10, 	0, 			true,   	1, 	CF_ONLYMAGICALWEAPONS,	0, 		{0xff,0xff}}
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
MONSTER roamingMonsterList[ 10 ];			// saveable
EQUIPMENT droppedEquipmentList[ 10 ];		// saveable
MAP_DATA dungeon;							// saveable
LOCATION playerLocation;
byte cheat = 0;								// how cheaty is the player so far
char msg[ 41 ];								// buffer for use with a_sprintf()

// variables to manage lefty's shop
//
byte itemClass;
byte itemID;
EQUIPMENT item;
bool hasItem;

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

	centerText( 8, "CREATING NEW LEVEL" );

	droppedCount = 0;										// zap dropped items
	roamingCount = 0;										// zap roaming monsters

	newDungeon( currentLevel );								// create next dungeon level

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


void handleDoctor( void ) {
	byte ch;

	while( true ) {
		banner("DOCTOR");
		a_printStringAt( 4, 10, "1) HEAL ME");
		a_printStringAt( 5, 10, "2) CURE ME!" );				// removes poison / sick
		a_printStringAt( 7, 10, leaveOption );

		ch = getch();

		switch( ch ) {
			case KEY_9:
				return;
			case KEY_2:
				if( spendQuestion( 20, "CURE" )) {
					 isSick = 0;
					 isPoisoned = 0;
					 turns += 10 + dieRoll( 1, 10 );
				}
				break;
			case KEY_1:
				if( spendQuestion( 10, "HEAL" )) {
					 currentHitPoints =  hitPoints;
					 turns += 10 + dieRoll( 1, 10 );
				}
				break;
			default:
				break;
		}
	}
}

void handleInn( void ) {
	byte ch;

	while( true ) {
		banner("INN");
		a_printStringAt( 4, 10, "1) I NEED A ROOM");
		a_printStringAt( 5, 10, "2) I NEED FOOD!");
		a_printStringAt( 6, 10, "3) SELL GEAR");
		a_printStringAt( 8, 10, leaveOption);

		ch = getch();

		switch( ch ) {
			case KEY_9:
				return;
			case KEY_3:
				sellGear();
				break;
			case KEY_1:
				if( spendQuestion( 5, "ROOM" )) {		// takes 15 minutes per hitpoint cured
					 turns += (  hitPoints -  currentHitPoints ) * 15;
					 currentHitPoints =  hitPoints;
				}
				break;
			case KEY_2:
				if( spendQuestion( 5, "FEED" )) {
					 isFed = 250;
					 isWatered = 250;
					 turns += 60;				// spend an hour eating
				}
				break;
			default:
				break;
		}
	}
}


void handleChurch( void ) {
	byte ch;

	while( true ) {
		banner("PRIEST");

		if( cheat > 0 ) {
			a_printStringAt( 4, 10, "THE GODS ARE UNHAPPY!");
			a_printStringAt( 6, 10, "7) DONATE 50 GOLD" );
			a_printStringAt( 8, 10, "9) LEAVE IN SHAME" );
		} else {
			a_printStringAt( 4, 10, "1) I SEEK A BLESSING" );
			a_printStringAt( 5, 10, "2) I AM CURSED!" );
			a_printStringAt( 7, 10, leaveOption );
		}

		ch = getch();

		switch( ch ) { 
			case KEY_9:
				return;
			case KEY_7:
				if( cheat != 0 &&  gold > 10 ) {
					 gold -= 50;
					ch = dieRoll( 2, 6 );
					if( ch > cheat )
						cheat = 0;
					else
						cheat -= ch;
				}
				break;
			case KEY_1:
				if(  isBlessed != 0 ) {
					footerMessage("ALREADY BLESSED!");
					shortPause();
				} else {
					if( spendQuestion( 100, "BLESS")) {
						 isBlessed = 250;
						 turns += 5 + dieRoll( 1, 10 );
					}
				}
				break;
			case KEY_2:
				if( spendQuestion( 200, "CURE" )) {
					byte result = removeCursedInventory();
					if( result == 0 )
						footerMessage( "YOU HAD NO CURSES!" );
					else
						footerMessage( "CURSED ITEMS REMOVED!" );
					turns += 60;
					shortPause();
				}
				break;
		}
	}
}


bool purchaseLeftyItem( byte amount, byte equipmentID ) {
	EQUIPMENT e;
	
	if( findFirstEmptyInventorySlot() == 0xff ) {
		footerMessage("YOU CARRY TOO MUCH");
		return false;
	} else {
		e = findMasterEquipmentByID( equipmentID );
		footerMessage( a_sprintf( msg, "BUY %s FOR %b? (Y/N)", e.name, amount ));
		if( yesNo() == UC_KEY_Y ) {
			if( gold >= amount ) {
				addKarma( 20 );
				e.modifier = dieRoll( 1, 10 ) - 5;
				if( e.modifier == 0 )
					e.modifier = 1;
				if( e.modifier < 0 ) {
					footerMessage("IT'S CURSED!");
					shortPause();
				}
				addToPlayerInventory( e, false );
				gold -= amount;
			} else
				notEnoughGold();
		}
	}
	return true;
}


void handleLefty( void ) {

	// 25% of the time lefty will be selling armor...the rest of the time weapons
	
	while( true ) {
		banner( "LEFTY'S" );

		if( hasItem )
			a_printStringAt( 4, 10, a_sprintf(msg, "1) BUY %s", item.name ));

		a_printStringAt( 6, 10, leaveOption );
		a_printStringAt( 8, 10, helpOption );

		switch( getch() ) {
			case KEY_QUESTION:
				showFile("D1:LEFTY.DAT");
				break;
			case KEY_1:
				if( purchaseLeftyItem( 250, itemID ))
					hasItem = false;
				break;
			case KEY_9:
				return;
		}
	}
}


bool handleTown( void ) { 
	byte ch;

	footerMessage("BACK TO TOWN? (Y/N)" );

	if( yesNo() == UC_KEY_N ) {
		return true;
	}

	// set up lefty's for sale item for this visit....
	itemClass = dieRoll( 1, 2 );
	itemID = ( itemClass == 1 ? dieRoll( 1, 9 ) + 115 : dieRoll( 1, 13 ) + 99 );
	item = findMasterEquipmentByID( itemID );
	hasItem = true;

#ifdef CAN_DIG
	// insure a player always has a pickaxe in their inventory
	if( playerHas( PICKAXE ) == 0xff ) {
		addToPlayerInventory( findMasterEquipmentByID( PICKAXE ), false );       // pickaxe
		showFile( "D1:PICKAXE.DAT" );
	}
#endif

	if( currentTurn / 1440 > 8 )
		showFile("D1:WARN2.DAT");
	else if( currentTurn / 1440 > 5 )
		showFile("D1:WARN1.DAT");
	else
		showFile("D1:WARN0.DAT");

	while( true ) {
		banner("MAYOR - HELLO!");

		a_printStringAt( 4, 10, "1) VISIT DOCTOR" );
		a_printStringAt( 5, 10, "2) VISIT INN" );
		a_printStringAt( 6, 10, "3) VISIT THE CHURCH" );
		a_printStringAt( 8, 10, "4) VISIT...LEFTY" );
		a_printStringAt( 10, 10, leaveOption );
		a_printStringAt( 12, 10, helpOption );

		ch = getch();

		switch( ch ) {
			case KEY_QUESTION:
				showFile("D1:TOWN.DAT");
				break;

			case KEY_1:
				handleDoctor();
				break;

			case KEY_2:
				handleInn();
				break;

			case KEY_3:
				handleChurch();
				break;

			case KEY_4:
				handleLefty();
				break;

			case KEY_9:
				a_cls();
				return true;
			
			default:
				break;
		}
	}
}


bool handleNextLevel( void ) { 
	bool nope = false;
	byte i;
	word delta, average;
	word newLevel;
	byte levels;

#ifdef CAN_DIG
	if( currentLevel == 20 && playerHas( 200 ) == 0xff )		
		nope = true;
	else if( currentLevel == 30 && playerHas( 201 ) == 0xff)	
		nope = true;
	else if( currentLevel == 40 && playerHas( 202 ) == 0xff)	
		nope = true;
#else
	if( currentLevel == 40 && playerHas( 200 ) == 0xff )		
		nope = true;
	else if( currentLevel == 60 && playerHas( 201 ) == 0xff)	
		nope = true;
	else if( currentLevel == 80 && playerHas( 202 ) == 0xff)	
		nope = true;
#endif
	if( nope ) {
		footerMessage("KEEP EXPLORING!");
		shortPause();
		return false;
	}

	footerMessage("CLIMB DOWN? (Y/N)");
	if( yesNo() == UC_KEY_N )
		return true;

	a_cls();

	prepareLevel();

	delta = currentTurn - levelStartedAt;
	average = currentTurn / currentLevel;
	
	currentLevel += 1;

	centerText( 11, a_sprintf( msg, "TIME ON LAST LEVEL: %u", delta ));
	centerText( 12, a_sprintf( msg, "AVERAGE LEVEL TIME: %u", average ));
	
	if( average <= 144 ) 
		centerText( 14, "ON PACE!" );
	else
		centerText( 14, "MOVE FASTER HERO!");
	
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


bool handleLockedDoor( LOCATION l ) { 
	byte hasLockpicks = playerHas( 126 );	// does the player have lockpicks?
	byte operation  = 0;					// equals 1 then kick the door, 2 try lockpicks, 0 = run away!
	byte ch;
	bool unlocked = false;
	sbyte test;

	if( hasLockpicks != 0xff ) {				// player has lockpicks?
		if( inventory[ hasLockpicks ].status > 0 ) {
			unlockDoor( l );
			if( dieRoll( 1, 100 ) == 1 ) {
				inventory[ hasLockpicks ].status -= 25;
				footerMessage( "BROKE A PICK!" );
			}
			return true;
		}
	} else {
		footerMessage( "LOCKED!  KICK IT? (Y/N)" );
		if( yesNo() == UC_KEY_Y ) {
			if( dieRoll( 1, 20 ) + attributeModifier( (operation == 1 ?  strength :  dexterity ) ) - 10 > 0 ) {
				unlockDoor( l ); 
				return true;
			} else {
				--currentHitPoints;
				footerMessage( "YOU HURT YOURSELF" );
				return false;
			}
		}
	}
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

	if( droppedCount > 0 )
		for( byte i = lootIndex + 1; i < 10; i++ )			// compress dropped loot list
			droppedEquipmentList[ i - 1 ] = droppedEquipmentList[ i ];

	footerMessage( msg );
	shortPause();

	return true;
}


bool handleArtifact( LOCATION l ) {
	byte theID;
	EQUIPMENT theArtifact;

	if( findFirstEmptyInventorySlot() == 0xff ) {
		footerMessage("INVENTORY FULL");
		shortPause();
		return false;
	}

	switch( currentLevel ) {
#ifdef CAN_DIG
		case 20:
			theID = 200;
			break;
		case 30:
			theID = 201;
			break;
		case 40:
			theID = 202;
			break;
#else
		case 40:
			theID = 200;
			break;
		case 60:
			theID = 201;
			break;
		case 80:
			theID = 202;
			break;
#endif
	}

	theArtifact = findMasterEquipmentByID( theID );
	addToPlayerInventory( theArtifact, false );

	mapSetDataViaLoc( l, FLOOR );

	if( playerHas( 202 ) != 0xff )
		showFile( "D1:202.DAT" );
	else if( playerHas( 201 ) != 0xff )
		showFile( "D1:201.DAT" );
	else if( playerHas( 200 ) != 0xff )
		showFile( "D1:200.DAT" );

	a_cls();
	return true;
}


#ifdef CAN_DIG
bool handleDig( LOCATION l ) {
	byte newWall = 0;
	byte playerWeapon;
	EQUIPMENT digWith;

	// player has the pickaxe equipped?
	playerWeapon = findEquipped( EQ_WEAPON );
	if( playerWeapon != 0xff ) {
		digWith =  inventory[ playerWeapon ];
		if( digWith.id != PICKAXE ) {
			footerMessage( "WRONG TOOL!" );
			return false;
		}
	}

	if( l.x == 0 || l.x == 39 || l.y == 0 || l.y == 21 ) {
		footerMessage( "CAN'T DIG THAT WAY!" );
		return false;
	}

	footerMessage("DIGGING...");

	switch( mapGetDataViaLoc( l )) {
		case WALL:
			newWall = WALL1;
			break;
		case WALL1:
			newWall = WALL2;
			break;
		case WALL2:
			newWall = FLOOR;
			break;
	}

	mapSetDataViaLoc( l, newWall );

	return true;
}
#endif

bool performInteraction( LOCATION l ) {
	byte dungeonCell;

	if( monsterThere( l ))
		return attack( l );

	if( lootThere( l )) {
		handleLoot( l );
		return false;
	}

	dungeonCell = mapGetDataViaLoc( l );

	switch( dungeonCell ) {
#ifdef CAN_DIG
		case WALL:
		case WALL1:
		case WALL2:			return handleDig( l );
#endif
		case STAIRS_UP:		return handleTown();
		case STAIRS_DOWN:	return handleNextLevel();
		case VDOOR:
		case HDOOR:			return handleDoor( l );
		case LVDOOR:
		case LHDOOR:		return handleLockedDoor( l );
		case GWATER:		return handleWater( l );
		case G_FOOD:		return handleFood( l );
		case G_ARTIFACT:	return handleArtifact( l );
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

	memset( &buffer, 0, 24 * 40 );
	for( byte row = 0; row < 21; row++ )
		for( byte col = 0; col < 40; col++ ) {
			if( mapGetFlags( &dungeon, col, row ) == MAP_FLAG_VISIBLE )
				buffer[ yylookup_gr0[ row ] + col ] = mapGetData( col, row );
		}
				
	if( roamingCount != 0 )
		for( byte i = 0; i < roamingCount; i++ ) 
			if( roamingMonsterList[ i ].hitPoints > 0 && drawLOS( loc, roamingMonsterList[ i ].loc )) {
				LOCATION l = roamingMonsterList[ i ].loc;
				buffer[ yylookup_gr0[ l.y ] + l.x ] = roamingMonsterList[ i ].symbol;
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

	footerMessage( a_sprintf( msg, "LEVEL %b", currentLevel ));

	// every eight turns the player heals one point of damage
	if( currentTurn % 10 == 0 )
		addCurrentHitpoints( 1 );	

	// every four valid moves update player counters for hunger, thirst, poison, sickness, and blessing
	if( currentTurn % 4 == 0 )
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

	if( isSick > 0 || isPoisoned > 0 || isFed == 0 || isWatered == 0 ) 
		footerMessage("YOU AREN'T FEELING WELL");
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
		footerMessage("NO TARGET");

	// is the target in range?
	if( minRange <= weapon.range )
		attack( roamingMonsterList[ roamingIndex ].loc );
	else
		footerMessage("NO TARGET IN RANGE");
}


bool attack( LOCATION l ) {
	EQUIPMENT attackWith;
	byte which;
	byte playerWeapon;
	sbyte attackRoll;
	sbyte damage;
	bool rv = false;

 	attackWith = masterEquipmentList[ 0 ];

	attacksAttempted++;

	// fighting takes it's toll...
	if( isFed > 0 )		 --isFed;
	if( isWatered > 0 )	 --isWatered;

	for( byte i = 0; i < roamingCount; i++ )
		if( locEqual( l, roamingMonsterList[ i ].loc ) && roamingMonsterList[ i ].hitPoints != 0 ) {
			which = i;
			break;
		}

	playerWeapon = findEquipped( EQ_WEAPON );
	if( playerWeapon != 0xff ) 
		attackWith =  inventory[ playerWeapon ];


#ifdef CAN_DIG
	if( attackWith.id == 129 ) {
		footerMessage("USE A WEAPON!");
		return false;
	}
#endif

	if( attackWith.status == 0 ) {
		footerMessage("WEAPON BROKEN!");
		shortPause();
		attackRoll = 0;
	} else 
		attackRoll = dieRoll( 1, 20 ) + attributeModifier(  strength ) + attackWith.modifier + level;

	if( isBlessed )	           ++attackRoll;
	if( isSick || isPoisoned ) --attackRoll;
	if( cheat > 100 )	       --attackRoll;

	footerMessage( a_sprintf( msg, "ATTACK WITH %s", attackWith.name ));

	// compare attack roll against defenders armor
	//
	if( attackRoll > ( 20 - roamingMonsterList[ which ].ac )) {
		hitsLanded++;
		damage = atLeastOne( dieRoll( attackWith.hitDie, attackWith.baseDie ) + attackWith.modifier + attributeModifier(  strength ));
		if( roamingMonsterList[ which ].cf == CF_ONLYMAGICALWEAPONS && !(playerHas( 201 ) == 0xff || attackWith.modifier < 1 )) 
			damage = 0;
		
		footerMessage( a_sprintf( msg, "%s TAKES %d DMG", roamingMonsterList[ which ].name, damage ));

		if( damage > roamingMonsterList[ which ].hitPoints ) {
			monstersKilled++;
			roamingMonsterList[ which ].hitPoints = 0;
			dropLoot( roamingMonsterList[ which ].loc );
			addExperience();
			resultMessage( killed, roamingMonsterList[ which ].name );
		} else {
			roamingMonsterList[ which ].hitPoints -= damage;
		}
	} else
		resultMessage( missed, roamingMonsterList[ which ].name );

	if( dieRoll( 1, 100 ) == 1 ) {
		if( inventory[ playerWeapon ].status > 0 )
			inventory[ playerWeapon ].status -= 5;
		footerMessage("WEAPON DAMAGED!" );
		shortPause();
	}

	return rv;
}

void updatePlayerLocation( LOCATION newLocation ) {
	bool stairsDown = mapGetDataViaLoc( newLocation ) == STAIRS_DOWN;

	if( canInteractWith( newLocation ))
		if( !performInteraction( newLocation ))
			return;
			
	if( canMoveTo( newLocation )) {
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


int main( void ) {
	byte playerInput;
	byte d;

    COLOR1  = 0x08;
    COLOR2  = 0x00;
    COLOR4  = 0x00;		// border black - changes based on health
    NOCLIK  = true;

	atari_init();

playAgain:

    COLOR1  = 0x08;
    COLOR2  = 0x00;
    COLOR4  = 0x00;		// border black - changes based on health

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

	while( playerIsAlive() && playerHas( 250 ) == 0xff ) {
		updateLOS();
		paintLevel();

		if( turns++ == 14400 ) { // 10 days passed?
			showFile("D:APOC.DAT");
			goto playAgain;
		}

		playerLocation =  loc;

		currentTurn += 1;

		switch(( playerInput = getch() )) {
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
			case KEY_1:
#ifdef CAN_DIG
				currentLevel = 20;
#else
				currentLevel = 40;
#endif
				prepareLevel();
				a_cls();
				break;
			case KEY_2:
#ifdef CAN_DIG
				currentLevel = 30;
#else
				currentLevel = 60;
#endif
				prepareLevel();
				a_cls();
				break;
			case KEY_3:
#ifdef CAN_DIG
				currentLevel = 40;
#else
				currentLevel = 80;
#endif
				prepareLevel();
				a_cls();
				break;			
			case KEY_4:
				if( playerHas( 200 ) != 0xff && playerHas( 201 ) != 0xff && playerHas( 202 ) != 0xff ) {

#ifdef CAN_DIG
					currentLevel = 50;
#else
					currentLevel = 100 ;
#endif
					prepareLevel();
					a_cls();
				} else
					footerMessage("NOPE");
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
