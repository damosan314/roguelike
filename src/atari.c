#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "atari.h"
#include "screen.h"
#include "utils.h"

IOCB *iocbp[ 8 ]   = {  (IOCB *)0x0340, 
                        (IOCB *)0x0350, 
                        (IOCB *)0x0360,  
                        (IOCB *)0x0370, 
                        (IOCB *)0x0380, 
                        (IOCB *)0x0390,
                        (IOCB *)0x03a0,
                        (IOCB *)0x03b0 };

// iocb0 = screen
// iocb6 and iocb7 = graphics and printer
bool iocbFree[ 8 ] = { false, true, true, true, true, true, false, false };
bool atEOF[ 8 ]    = { false, false, false, false, false, false, false, false };
byte a_error = 0;

void callCIO( byte iocb, byte command, byte aux1, byte aux2, void *data, word bytes )  {
    IOCB *p = iocbp[ iocb ];

    iocb = iocb << 4;

    p->ICCOM = command;
    
    if( command == ICCOM_OPEN ) {
        p->ICAX1 = aux1;
        p->ICAX2 = aux2;
    }

    p->ICBAL = data;
    p->ICBLL = bytes;

    a_error = _cio( iocb );
}


void atari_init( void ) {
    a_error = 0;
    a_init();                   // init screen handler
}


static byte findFreeIOCB( void ) {
    byte i;

    for( i = 0; i < 8; i++ )
        if( iocbFree[ i ] == true )
            return i;
    return 0xff;
}

word a_fwrite( FILE f, void *data, word size ) {   
    callCIO( f, ICCOM_PUT_CHAR, 0, 0, data, size );
    if( a_error == CIO_SUCCESS ) 
        return iocbp[ f ]->ICBLL;
    return 0;
}


word a_fread( FILE f, void *data, word size ) {

    callCIO( f, ICCOM_GET_CHAR, 0, 0, data, size );

    if( a_error == CIO_END_OF_FILE ) {
        atEOF[ f ] = true;
        return 0;
    }

    if( a_error == CIO_SUCCESS ) 
        return iocbp[ f ]->ICBLL;

    return 0;
}


bool a_feof( FILE f ) { return atEOF[ f ]; }


void a_fclose( FILE f ) { 
    callCIO( f, ICCOM_CLOSE, 0, 0, NULL, 0 );
    if( a_error == CIO_SUCCESS ) {
        iocbFree[ f ] = true;
        atEOF[ f ] = false;
    }
}

//
// simple fopen - only supports R and W (overwrite) modes at the moment
//
FILE a_fopen( char *filename, char *mode ) {   
    byte openMode;

    byte iocbToUse = findFreeIOCB();
    if( iocbToUse == 0xff )
        return 0xff;

    switch( *mode ) {
        case 'W':
        case 'w':
            openMode = ICAX_DISK_WRITE_NEW;
            break;
        case 'R':
        case 'r':
            openMode = ICAX_DISK_OPEN_FILE;
            break;
    }

    callCIO( iocbToUse, ICCOM_OPEN, openMode, 0, filename, 0);
    if( a_error == CIO_SUCCESS ) {
        iocbFree[ iocbToUse ] = false;    
        atEOF[ iocbToUse ] = false;
        return iocbToUse;
    } else
        callCIO( iocbToUse, ICCOM_CLOSE, 0, 0, NULL, 0 );       // releae IOCB

    return 0xff;
}


bool a_fdelete( char *filename ) {
    char localFilename[ 20 ];
    bool rv = false;
    byte iocbToUse;

    //strcpy( localFilename, filename );
    //localFilename[ strlen( filename ) + 1 ] = EOL;      // convert to be CIO friendly

    iocbToUse = findFreeIOCB();
    if( iocbToUse == 0xff )
        return NULL;
    
    callCIO( iocbToUse, ICCOM_DOS_DELETE, 0, 0, &localFilename, 0 );
    if( a_error == CIO_SUCCESS )
        rv = true;
    callCIO( iocbToUse, ICCOM_CLOSE, 0, 0, NULL, 0 );       // releae IOCB

    return rv;
}