
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "atari.h"
#include "monster.h"
#include "player.h"
#include "item.h"
#include "screen.h"
#include "gd.h"
#include "die.h"
#include "item.h"
#include "map.h"
#include "glyphs.h"
#include "utils.h"
#include "inventory.h"
#include "ai.h"


bool canMoveTo( LOCATION l );           // defined in rl2.c

typedef enum {O_HORIZONTAL, O_VERTICAL, O_MOVEX, O_MOVEY } ORTHOGINAL;
typedef enum { MA_HEAL, MA_RUN, MA_FIGHT, MA_SPELL } MONSTER_ACTION;

void getDeltas( MONSTER *m, int *x, int *y ) {
    *x = abs( loc.x - m->loc.x );
    *y = abs( loc.y - m->loc.y );
}

ORTHOGINAL monsterOrthoganal( byte which ) {
    int dX, dY;
    
    getDeltas( &roamingMonsterList[ which ], &dX, &dY );

    if( dX == 1 && dY == 0 )
        return O_HORIZONTAL;

    if( dX == 0 && dY == 1 )
        return O_VERTICAL;

    if( dX >= dY)
        return O_MOVEX;
    else
        return O_MOVEY;
}

void monsterSpell( byte which, MONSTER_ACTION whichSpell ) { 
    MONSTER m;
    sbyte r;

    m = roamingMonsterList[ which ];

    if( m.cfPower > 0 )
        m.cfPower--;            // each spell uses one charge
    else {
        roamingMonsterList[ which ] = m;
        return;
    }
    switch( whichSpell ) {
        case MA_HEAL: 
            r = dieRoll( 1, 6 );    // the powah!
            roamingMonsterList[ which ].hitPoints += r;
            footerMessage( a_sprintf( msg, "%s HEALS ITSELF FOR %b", roamingMonsterList[ which ].name, r ));
            break;
        case MA_SPELL:

            r = dieRoll( 2, 6 );    // the powah!

            if( playerHas( 200 ) != 0xff ) {
                footerMessage( "LORATH ABSORBS SPELL" );
                magicPointsAbsorbed += r;
            } else {
                if( dieRoll( 1, 20 ) > dexterity ) {
                    r = atLeastOne( r - attributeModifier( dexterity ));
                    if( r > currentHitPoints )
                        currentHitPoints = 0;
                    else
                        currentHitPoints -= r;
                    footerMessage( a_sprintf( msg, "%s CASTS SPELL FOR %b DAMAGE", roamingMonsterList[ which ].name, r ));
                }
                    footerMessage("SPELL FIZZLES");
            }
            break;
        default:
            break;
    }
    roamingMonsterList[ which ] = m;
}

void monsterSwing( byte which ) {
    MONSTER m;
    sbyte requiredToHit = computePlayerAC();
    sbyte monsterSwing = dieRoll( 1, 20 );
    byte damage;
    byte i;
    EQUIPMENT weapon;

    memcpy( &m, &roamingMonsterList[ which ], sizeof( MONSTER ));

    // PLOT ARMOR! - players are hit only 1/2 the time
    if( requiredToHit < 10 )
        requiredToHit = 10;

    if( monsterSwing > ( 20 - requiredToHit )) {
        if( m.armedWith == 0 )
            damage = dieRoll( 1, 4 );
        else {
            weapon = findMasterEquipmentByID( m.armedWith );
            damage = dieRoll( weapon.hitDie, weapon.baseDie );            

            // PLOT ARMOR ROUND 2! - players level 0, 1, and 2 can only take 3 points of damage per hit
            if( level < 3 && damage > 3 )
                damage = 3;
        }

        if( damage >  currentHitPoints )
            currentHitPoints = 0;
        else
            currentHitPoints -= damage;

        if( findEquipped( EQ_ARMOR ) != 0xff ) {
            if( dieRoll( 1, 10 ) == 1 ) {
                byte index = findEquipped( EQ_ARMOR );
                inventory[ index ].status -= 5;
                footerMessage("ARMOR DAMAGED!");
                shortPause();
            }
        }

        // check if the has a special effect
        if( m.cf == CF_SICK ) {
            if( dieRoll( 1, 20 ) > constitution ) {
                isSick = 200;       // player is sick...
                footerMessage("YOU ARE SICK!");
            }
        }

        if( m.cf == CF_POISON ) {
            if( dieRoll( 1, 20) > constitution ) {
                isPoisoned = 10;   // player is poisoned
                footerMessage("YOU ARE POISONED!");
            }
        }

        if( m.cf == CF_STEAL )
            if( dieRoll( 1, 100 ) < 10 && dieRoll( 1, 20 ) > dexterity ) {  // steal a random unequipped item...it goes POOF!  Quest items cannot be taken...
                for( i = 0; i < 10; i++ )
                    if( slotUsed[ i ] && !equipped[ i ] && inventory[ i ].id < 200 ) {                 // can't steal quest items
                        footerMessage("THIEF TOOK AN ITEM!");
                        shortPause();
                        emptySlotAndCompress( i );
                        break;
                    }
            }

        footerMessage( a_sprintf( msg, "%s HIT FOR %b", m.name, damage ));
    }
}


bool canMonsterMoveTo( LOCATION newLoc, bool smartCreature ) {
    byte cell        = mapGetDataViaLoc( newLoc );
    bool playerThere = locEqual( loc, newLoc );
    bool monster     = monsterThere( newLoc );

    // smart creatures (sentient = true) are able to open doors if required.  Dumb creatures
    // will simply walk about floor and tunnels (and pass through doors already opened).
    //
    if( smartCreature )
        return  monster == false && playerThere == false && ( cell == STAIRS_UP || cell == STAIRS_DOWN || cell == TUNNEL || cell == FLOOR || cell == HDOOR || cell == VDOOR || cell == OVDOOR || cell == OHDOOR );
    else
        return  monster == false && playerThere == false && ( cell == STAIRS_UP || cell == STAIRS_DOWN || cell == TUNNEL || cell == FLOOR || cell == OVDOOR || cell == OHDOOR );
}


void moveMonster( byte which, ORTHOGINAL d ) {
    int dX, dY;
    MONSTER m = roamingMonsterList[ which ];
    bool moved = false;
    LOCATION temp, newHorizontalMove, newVerticalMove;
    byte cell;

    temp              = m.loc,
    newHorizontalMove = m.loc,
    newVerticalMove   = m.loc;
    dX                = (sbyte)loc.x - (sbyte)m.loc.x;     // distance between the player and monster
    dY                = (sbyte)loc.y - (sbyte)m.loc.y;     // ditto

    // normalize the deltas to be -1, 0, or +1
    if( dX != 0 ) {
        if(  dX > 0 )                       // player is to the right of the monster
            newHorizontalMove.x++;          // so monster moves east
        else
            newHorizontalMove.x--;
    }
    
    if( dY != 0 ) {
        if(  dY > 0  )                      // player is below the monster....
            newVerticalMove.y++;            // so monster moves south...
        else
            newVerticalMove.y--;
    }

    // try the selected move first...if it can't move there...go the other way
    if( d == O_MOVEX && canMonsterMoveTo( newHorizontalMove, m.sentient ))
        m.loc = newHorizontalMove;
    else if( d == O_MOVEX && canMonsterMoveTo( newVerticalMove, m.sentient ))
        m.loc = newVerticalMove;

    
    // repeat...
    if( d == O_MOVEY && canMonsterMoveTo( newVerticalMove, m.sentient ))
        m.loc = newVerticalMove;
    else if( d == O_MOVEY && canMonsterMoveTo( newHorizontalMove, m.sentient ))
        m.loc = newHorizontalMove;

    // check the cell the monster would be moving into, if it's an unlocked door and the
    // monster is sentient...open the darn door.  otherwise bail.
    cell = mapGetDataViaLoc( m.loc );
    if( cell == HDOOR || cell == VDOOR ) {
        mapSetDataViaLoc( m.loc, ( cell == HDOOR ? OHDOOR : OVDOOR ));   
        m.loc = temp;
    }
    
    // finlly have a good spot for the monster
    roamingMonsterList[ which ].loc = m.loc;
}


bool isLockable( LOCATION l ) {
    byte cell = mapGetDataViaLoc(  l );

    if( cell == OVDOOR || cell == OHDOOR || cell == VDOOR || cell == HDOOR )
        return true;
    else
        return false;
}


static void smartCreature( byte which ) {
    ORTHOGINAL     o      = monsterOrthoganal( which );
    MONSTER        m      = roamingMonsterList[ which ];
    MONSTER_ACTION ma     = MA_FIGHT; 
    word           r      = range( m.loc, loc );
    bool           canSee = drawLOS( m.loc, loc );
    //int dX, dY;

    //getDeltas( &m, &dX, &dY );

    ma = MA_FIGHT;

    // if the smart monster is almost dead elect to run.  if the monster is a 
    // priest of some sort with charges left cast a heal.  Otherwise the monster
    // will want to fight.
    if( m.hitPoints < 5 && m.cf == CF_PRAYERS && m.cfPower != 0 )
            ma = MA_HEAL;
    
    // a mage who is fighting will throw spells if it can...
    if( ma == MA_FIGHT && m.cf == CF_SPELLS && m.cfPower != 0 && canSee && r < 5 )
        ma = MA_SPELL;

    switch( o ) {
        case O_HORIZONTAL:
        case O_VERTICAL:
            switch( ma ) {
                case MA_HEAL:
                    break;
                case MA_SPELL:
                    monsterSpell( which, MA_HEAL );
                    break;
                case MA_FIGHT:
                    monsterSwing( which );
                    break;
                default:
                    break;
            }
        case O_MOVEX:
        case O_MOVEY:
            switch( ma ) {
                case MA_FIGHT:                      // move towards player instead of fighting
                    moveMonster( which, o );
                    break;
                case MA_SPELL:                      // throw spell at player
                    monsterSpell( which, MA_SPELL );
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}


// dumb creatures always attack if they can or move to an attack position
static void dumbCreature( byte which ) {
    ORTHOGINAL o = monsterOrthoganal( which );

    switch( o ) {
        case O_HORIZONTAL:
        case O_VERTICAL:
            monsterSwing( which );
            break;
        case O_MOVEX:
        case O_MOVEY:
            moveMonster( which, o );
            break;
    }
}

void monsterAI( void ) {
    for( byte i = 0; i < roamingCount; i++ )
        if( roamingMonsterList[ i ].hitPoints > 0 ) {
            if( roamingMonsterList[ i ].sentient == true )
                smartCreature( i );
            else
                dumbCreature( i );
        }
}