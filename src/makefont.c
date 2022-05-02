
#include "atari.h"

#define dta ,

byte fontData[] = {
   	    0,0,0,0,0,0,0,0
	dta 0,24,24,24,24,0,24,0
	dta 0,102,102,102,0,0,0,0
	dta 0,102,255,102,102,255,102,0
	dta 24,62,96,60,6,124,24,0
	dta 0,102,108,24,48,102,70,0
	dta 28,54,28,56,111,102,59,0
	dta 0,24,24,24,0,0,0,0
	dta 0,14,28,24,24,28,14,0
	dta 0,112,56,24,24,56,112,0
	dta 0,102,60,255,60,102,0,0
	dta 0,24,24,126,24,24,0,0
	dta 0,0,0,0,0,24,24,48
	dta 0,0,0,126,0,0,0,0
	dta 0,0,0,0,0,24,24,0
	dta 0,6,12,24,48,96,64,0
	dta 0,60,102,110,118,102,60,0
	dta 0,24,56,24,24,24,126,0
	dta 0,60,102,12,24,48,126,0
	dta 0,126,12,24,12,102,60,0
	dta 0,12,28,60,108,126,12,0
	dta 0,126,96,124,6,102,60,0
	dta 0,60,96,124,102,102,60,0
	dta 0,126,6,12,24,48,48,0
	dta 0,60,102,60,102,102,60,0
	dta 0,60,102,62,6,12,56,0
	dta 0,0,24,24,0,24,24,0
	dta 0,0,24,24,0,24,24,48
	dta 2,12,52,84,84,52,12,2
	dta 0,0,126,0,0,126,0,0
	dta 64,48,44,42,42,44,48,64
	dta 0,60,102,12,24,0,24,0
	dta 0,60,102,110,110,96,62,0
	dta 0,24,60,102,102,126,102,0
	dta 0,124,102,124,102,102,124,0
	dta 0,60,102,96,96,102,60,0
	dta 0,120,108,102,102,108,120,0
	dta 0,126,96,124,96,96,126,0
	dta 0,126,96,124,96,96,96,0
	dta 0,62,96,96,110,102,62,0
	dta 0,102,102,126,102,102,102,0
	dta 0,126,24,24,24,24,126,0
	dta 0,6,6,6,6,102,60,0
	dta 0,102,108,120,120,108,102,0
	dta 0,96,96,96,96,96,126,0
	dta 0,99,119,127,107,99,99,0
	dta 0,102,118,126,126,110,102,0
	dta 0,60,102,102,102,102,60,0
	dta 0,124,102,102,124,96,96,0
	dta 0,60,102,102,102,108,54,0
	dta 0,124,102,102,124,108,102,0
	dta 0,60,96,60,6,6,60,0
	dta 0,126,24,24,24,24,24,0
	dta 0,102,102,102,102,102,126,0
	dta 0,102,102,102,102,60,24,0
	dta 0,99,99,107,127,119,99,0
	dta 0,102,102,60,60,102,102,0
	dta 0,102,102,60,24,24,24,0
	dta 0,126,12,24,48,96,126,0
	dta 0,30,24,24,24,24,30,0
	dta 0,64,96,48,24,12,6,0
	dta 0,120,24,24,24,24,120,0
	dta 0,0,60,126,66,66,126,0
	dta 120,72,72,72,75,75,72,120
	dta 193,0,108,0,180,0,219,0
	dta 131,0,54,0,45,0,219,0
	dta 195,0,195,0,97,0,182,0
	dta 54,0,219,0,109,0,176,0
	dta 219,0,182,0,109,0,219,0
	dta 0,126,66,66,66,66,126,0
	dta 36,36,231,0,0,231,36,36
	dta 0,56,84,108,56,68,56,0
	dta 128,88,0,102,153,0,36,66
	dta 0,84,124,56,56,124,84,0
	dta 0,0,0,2,67,60,0,37
	dta 74,129,60,102,126,102,36,24
	dta 0,66,36,8,16,36,66,0
	dta 195,195,0,48,48,6,198,192
	dta 109,0,127,64,64,127,0,109
	dta 182,0,254,2,2,254,0,182
	dta 2,26,130,162,90,128,164,36
	dta 0,153,129,102,24,24,0,36
	dta 1,25,0,39,88,25,1,37
	dta 4,50,128,55,48,2,84,144
	dta 0,0,60,102,66,102,60,0
	dta 60,36,60,36,36,36,36,60
	dta 135,133,231,0,0,231,162,226
	dta 0,0,0,0,255,36,36,36
	dta 24,24,0,60,102,153,195,0
	dta 0,14,107,56,0,102,60,0
	dta 56,84,0,58,68,16,40,0
	dta 0,0,0,0,0,0,0,0
	dta 0,0,0,0,0,0,0,0
	dta 0,0,0,0,0,0,0,0
	dta 255,24,24,24,24,24,24,255
	dta 129,129,129,255,255,129,129,129
	dta 0,126,126,126,126,126,126,0
	dta 85,170,85,170,85,170,85,170
	dta 0,126,66,126,126,126,126,0
	dta 34,136,34,136,34,136,34,136
	dta 0,36,72,18,36,72,18,0
	dta 0,18,72,36,18,72,36,0
	dta 0,18,72,36,18,72,36,0
	dta 0,16,0,16,84,56,16,108
	dta 0,0,32,16,20,56,16,108
	dta 0,0,8,16,80,56,16,108
	dta 0,126,66,66,126,126,126,0
	dta 0,126,66,66,66,126,126,0
	dta 255,24,24,24,24,24,24,255
	dta 129,129,129,255,255,129,129,129
	dta 0,0,60,36,36,60,0,0
	dta 214,40,128,184,214,145,169,170
	dta 170,84,56,78,78,56,84,170
	dta 0,16,68,186,146,146,170,68
	dta 0,80,80,56,20,16,40,40
	dta 0,84,84,56,16,16,40,40
	dta 0,20,18,50,30,50,84,0
	dta 0,72,72,124,76,72,72,84
	dta 32,32,38,20,10,18,40,72
	dta 96,0,12,0,48,0,102,0
	dta 0,2,16,68,1,20,64,8
	dta 224,144,84,60,20,16,40,72
	dta 255,0,0,0,0,0,0,255
	dta 129,129,129,129,129,129,129,129
	dta 24,24,24,24,24,24,24,24
	dta 85,170,85,170,85,170,85,170
	dta 255,24,24,24,24,24,24,255
	dta 129,129,129,255,255,129,129,129
};

int main( void ) {
    FILE f = a_fopen("D1:FONT.DAT", "w" );
    a_fwrite( f, (byte *)fontData, sizeof( fontData ));
    a_fclose( f );
    return 0;
}