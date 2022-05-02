
#ifndef __GD_H
#define __GD_H

#include "common.h"
#include "player.h"
#include "monster.h"
#include "map.h"

extern const EQUIPMENT masterEquipmentList[];             // defined in rl2.c
extern const MONSTER masterMonsterList[];
extern MAP_DATA dungeon;
extern byte equipmentCount;
extern byte roamingCount;
extern byte monsterCount;
extern byte droppedCount;
extern byte currentLevel;
extern word currentTurn;
extern word levelStartedAt;
extern MONSTER roamingMonsterList[];        // monsters moving around the map
extern EQUIPMENT droppedEquipmentList[];      // contains copy of loot dropped by monsters

extern char msg[];

extern byte cheat;

// stats
extern word	stepsTaken;
extern word	attacksAttempted;
extern word	hitsLanded;
extern word	foodEaten;
extern word	potionsConsumed;
extern word	waterDrank;
extern word	monstersKilled;

#endif