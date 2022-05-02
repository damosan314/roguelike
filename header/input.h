#ifndef __INPUT_H
#define __INPUT_H

#include <stdbool.h>
#include "common.h"
#include "atari.h"

bool kbhit( void );             // returns true if a character is waiting
byte getch( void );             // returns last character from user

#endif