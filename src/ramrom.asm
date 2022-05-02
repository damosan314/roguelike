
	; loader for the roguelikes - performs three functions:
	;
	;		1) Moves OS to RAM
	;		2) Overwrites system font with game font
	;		3) Loads and executes the roguelike
	;
	; warning: only tested under DOS 2.5.

	run _ramrom
        

	; Modified to act as a simple loader - thank you
	; Joe...where ever you are.

    ;Move XL OS ROM into RAM
    ;
    ;RAMROM--Installs the XL ROM-based
    ;  OS in RAM at the same address
    ;  space. This is useful for
    ;  making small patches to the
    ;  OS or for experimenting with
    ;  new design concepts, such as
    ;  multitasking, window
    ;  management, etc.
    ;
    ;    By Joe Miller.
    ;
    ;This version is configured
    ;as an AUTORUN.SYS file.
    ;

	; after running the following memory areas are useful
	; International Character Set 	CC00 - CFFF (1024 bytes)
	; floating point routines		d800 - dfff (2048 bytes)
	; ROM default font				e000 - e3ff (1024 bytes)
	; cassette handler				250ish
	; printer handler				200ish


	; start high
	org $b000

    SOURCE  EQU     $CB             	;zero page usage
    DEST    EQU     $CD
    OSROM   EQU     $C000           	;address of OS ROM start
    OSRAM   EQU     $4000           	;address of ROM destination
    NMIEN   EQU     $D40E           	;NMI enable register
    PORTB   EQU     $D301           	;memory mgt control latch
	SOUNDR	EQU		$41					; noisy IO flag
	CIOV	EQU		$e456				; good old CIO vector

	IOCB	EQU		$340					; we're using IOCB #1
	ICHID	EQU		IOCB + 0
	ICDNO	EQU		IOCB + 1
	ICCOM	EQU		IOCB + 2
	ICSTA	EQU		IOCB + 3
	ICBAL	EQU		IOCB + 4
	ICBAH	EQU		IOCB + 5
	ICBLL	EQU		IOCB + 8
	ICBLH	EQU		IOCB + 9
	ICAX1	EQU		IOCB + 10
	ICAX2	EQU		IOCB + 11

	OPEN	EQU		$03
	CLOSE	EQU		$0c
	GETCHAR	EQU		$07

	READ 	EQU		$04

	NOCLIK  equ 	$02db
	COLOR1  equ 	$02c5
	COLOR2  equ 	$02c6
	COLOR4  equ		$02c8
	SYSFONT equ 	$e400

.ifdef rl2
file:		.by 	'D1:RL2.XEX' $9b
.endif

.ifdef rl3
file:    	.by		'D1:RL3.XEX' $9b
.endif

.ifdef rl4
file:    	.by		'D1:RL4.XEX' $9b
.endif


fontFile:	.by		'D1:FONT.DAT' $9b

;;; atari binary header
;;;
header:		.wo		$0000				; should equal 0xffff
			.wo		$0000				; should equal 0x02e0
			.wo		$0000				; should equal 0x02e1
start:		.wo		$0000				; address of start routine
loadStart:	.wo		$0000				; load start address
loadEnd:	.wo		$0000				; load end address - 1


fileToWork: .wo		$ffff				; used by iocb routines - stores address of file we want to work on (used by IOCB routines)
bytesToWork .word	$0000				; used by iocb routines - the number of bytes to read/write in RAM
buffer		.word	$0000				; used by iocb routines - address of where to read/write data in RAM

;;; program start....

_ramrom:
	lda			#$08
	sta			NOCLIK
	sta			COLOR1
	lda			#$00
	sta			COLOR2
	sta			COLOR4
	sta			SOUNDR					; no need for disk noise thank you

	jsr	printf
	.by $9b $9b 'MOVING OS TO RAM...' $9b 0

	LDA			<OSROM
	STA     	SOURCE
    STA     	DEST            		;initialize copy addrs
	LDA     	>OSROM
    STA     	SOURCE+1
	LDA     	>OSRAM
	STA     	DEST+1
	LDY     	#0
                                     	;Repeat
Pass1:   	
	LDA     	(SOURCE),Y      		;copy ROM to RAM
	STA     	(DEST),Y
	INY
	BNE     	Pass1 
	INC     	DEST+1
	INC     	SOURCE+1
	BEQ     	Swap            		;If done
	LDA     	SOURCE+1
	CMP     	#$D0
	BNE     	Pass1           		;skip 2K block at $D000
	LDA     	#$D8
	STA     	SOURCE+1
	BNE     	Pass1           		;Until SOURCE = $0000

Swap:    	
	PHP                     			;save processor status
	SEI                     			;disable IRQs
	LDA     	NMIEN
	PHA                     			;save NMIEN
	LDA     	#0
	STA     	NMIEN           		;disable NMIs
	LDA     	PORTB
	AND     	#$FE            		;turn off ROMs
	STA     	PORTB           		;(leaving BASIC unchanged!)
	LDA     	>OSROM
	STA     	DEST+1          		;set up block copy
	LDA     	>OSRAM
	STA     	SOURCE+1
                                     	;Repeat
Pass2:   	
	LDA     	(SOURCE),Y      		;move RAM OS to proper address
	STA     	(DEST),Y
	INY
	BNE     	Pass2
	INC     	SOURCE+1        		;move to next page
	INC     	DEST+1
	BEQ     	Enable         			;If complete
	LDA     	DEST+1 
	CMP     	#$D0
	BNE     	Pass2           		;skip block at $D000
	LDA     	#$D8
	STA     	DEST+1
	BNE     	Pass2           		;Until DEST = $000

Enable:  	
	PLA
	STA     	NMIEN           		;reestablish NMI mask
	PLP                     			;reenable IRQs

loadFont:
	jsr	printf
				.by 'LOADING FONT...' $9b 0

	ldx			#$20
	jsr			iocbClose
	lda			#<fontFile
	sta			fileToWork
	lda			#>fontFile
	sta			fileToWork+1
	jsr			iocbOpenRead
	lda			#$00
	sta			buffer
	lda			#$e0
	sta			buffer+1
	lda			#$ff
	sta			bytesToWork
	lda			#$ff
	sta			bytesToWork+1
	jsr			iocbReadBytes
	ldx			#$20
	jsr			iocbClose

	jsr	printf
				.by 'LOADING ROGUELIKE...' $9b 0

openBinary:
	ldx			#$10
	jsr			iocbClose
	lda			#<file
	sta			fileToWork
	lda			#>file
	sta			fileToWork+1
	jsr			iocbOpenRead
readHeader:							
	lda			#<header
	sta			buffer
	lda			#>header
	sta			buffer+1
	lda			#12
	sta			bytesToWork
	lda			#0
	sta			bytesToWork+1
	jsr			iocbReadBytes
loadBinary:
	lda			loadStart
	sta			buffer
	lda			loadStart + 1
	sta			buffer + 1
	lda			#$ff					; read..
	sta			bytesToWork				; ...till...
	sta			bytesToWork+1			; ...end of file
	jsr			iocbReadBytes
	ldx			#$10
	jsr			iocbClose

runBinary:
	jsr	printf
	.by 'STARTING...' $9b $9b 0

	jmp			(start)          		; never returns....


; assumes X points to proper IOCB offset
iocbClose:
	lda			#CLOSE
	sta			ICCOM,x 
	jsr			CIOV
	rts

; assumes X points to proper IOCB offset
iocbOpenRead:
	lda			#OPEN					; open the file
	sta			ICCOM,x					; store the command
	lda			fileToWork				; file to load
	sta			ICBAL,x
	lda			fileToWork+1
	sta			ICBAH,x
	lda			#READ
	sta			ICAX1,x
	jsr			CIOV
	rts

; assumes X points to proper IOCB offset
iocbReadBytes
	lda			buffer					; write to address of header (above)
	sta			ICBAL,x
	lda			buffer+1
	sta			ICBAH,x
	lda			bytesToWork				; read entire header (12 bytes)
	sta			ICBLL,x
	lda			bytesToWork+1
	sta			ICBLH,x
	lda			#GETCHAR				; read characters
	sta			ICCOM,x
	jsr			CIOV
	rts

 .link '\Mad-Assembler-2.1.3\examples\LIBRARIES\stdio\lib\printf.obx'