#ifndef __SOUND_H
#define __SOUND_H

#include "atari.h"
#include "common.h"

#define SOUND_GUNSHOT       1
#define SOUND_STEP          2
#define SOUND_DROP          3
#define SOUND_SCALE         4

void _initVBI( void );
void _whichTrack( byte );
byte _isPlaying( void );
byte _vbiActive( void );

#endif