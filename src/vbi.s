.global _initVBI
.global _vbiRoutine
.global _whichTrack
.global _vbiActive
.global _isPlaying
.global _soundTest

AUDCTL  =   $D208
AUDF1   =   $D200
AUDC1   =   $D201
AUDF2   =   $D202
AUDC2   =   $D203
SKCTL   =   $D20F
XITVBV  =   $E462           ; Exit deferred VBI O.S. vector
SETVBV  =   $E45C           ; Set VBI O.S. vector

ACTIVE_TRACK = $f0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; track format:
;;;
;;; string of bytes (notes).  a value of 0xff is the end of the track.  
;;;     if the next byte is 0 the track is restarted, otherwise sound is killed
;;;
;;; format for tracks is freq, dur, freq, dur, 0xff, 0 | 0xff
;;;
track_NULL:     .byte   0xff, 0xff

track_scale:    .byte   0, 5,   0b00001111
                .byte   5, 5,   0b00001111
                .byte   10, 5,  0b00001111
                .byte   15, 5,  0b00001111
                .byte   20, 5,  0b00001111
                .byte   25, 5,  0b00001111
                .byte   30, 5,  0b00001111
                .byte   35, 5,  0b00001111
                .byte   40, 5,  0b00001111
                .byte   45, 5,  0b00001111
                .byte   50, 5,  0b00001111
                .byte   55, 5,  0b00001111
                .byte   60, 5,  0b00001111
                .byte   65, 5,  0b00001111
                .byte   70, 5,  0b00001111
                .byte   75, 5,  0b00001111
                .byte   80, 5,  0b00001111
                .byte   85, 5,  0b00001111
                .byte   90, 5,  0b00001111
                .byte   95, 5,  0b11001111
                .byte   100, 5, 0b11001111
                .byte   105, 5, 0b11001111
                .byte   110, 5, 0b11001111
                .byte   115, 5, 0b11001111
                .byte   120, 5, 0b11001111
                .byte   125, 5, 0b11001111
                .byte   130, 5, 0b11001111
                .byte   135, 5, 0b11001111
                .byte   140, 5, 0b11001111
                .byte   145, 5, 0b11001111
                .byte   150, 5, 0b11001111
                .byte   155, 5, 0b11001111
                .byte   160, 5, 0b11001111
                .byte   165, 5, 0b11001111
                .byte   170, 5, 0b11001111
                .byte   175, 5, 0b11001111
                .byte   180, 5, 0b11001111
                .byte   185, 5, 0b11001111
                .byte   190, 5, 0b11001111
                .byte   195, 5, 0b11001111
                .byte   200, 5, 0b11001111
                .byte   205, 5, 0b11001111
                .byte   210, 5, 0b11001111
                .byte   215, 5, 0b11001111
                .byte   220, 5, 0b11001111
                .byte   225, 5, 0b11001111
                .byte   230, 5, 0b11001111
                .byte   235, 5, 0b11001111
                .byte   240, 5, 0b11001111
                .byte   245, 5, 0b11001111
                .byte   250, 5, 0b11001111
                .byte   255


track_sneak:    .byte   120,    2, 4
                .byte   0,      5, 0
                .byte   120,    2, 4
                .byte   255

track_step:     .byte   120,    2, 8
                .byte   0,      5, 0
                .byte   120,    2, 8
                .byte   255

track_gunshot:  .byte   50,    2, 15
                .byte   100,   2, 0b10001111
                .byte   0,     1, 0
                .byte   255

track_drip:     .byte   0,0,0xff,0xff

track_wind:     .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   128, 1
                .byte   255

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; is the VBI currently running - 0 = no, 0xff = yes
;;;
playing:        .byte   0
ready:          .byte   0               ;; 0 = playing, 0xff = ready to be set
volume:         .byte   8               ;; 15 is regular volume, 0 is off

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; the current track that is playing and the current index into that track - up to 128
;;;     tracks can be defined.
;;;
active_index:   .byte   0xff

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; the current note being played and the duration (number of jiffies) for that note
;;;
note:           .byte   0xff
noteHigh:       .byte   0xff
duration:       .byte   0xff

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; the sound effects
;;;
track_table:    .short  track_gunshot
                .short  track_step
                .short  track_drip
                .short  track_scale
                .short  track_NULL

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; ensures our vbi routine is called each vertical blank
;;;
_initVBI:
    lda         #0
    sta         note
    sta         duration
    sta         active_index
    lda         #0xff
    sta         ready
    ldy         #<_vbiRoutine
    ldx         #>_vbiRoutine
    lda         #7
    jsr         SETVBV
    rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; which sound effect track should we start playing now?  called from C.
;;;
;;; prototype: _whichTrack( trackNumber );
;;; 
_whichTrack:
    sta         ACTIVE_TRACK    ;; track is 1 .. n
    tax                         ;; make it track - 1
    dex
    txa
    asl         a               ;; point to the proper offset into address table
    tax                         ;; we now have the proper offset
    lda         #0xff           ;; wait till the DLI is done
    cmp         playing
    beq         _whichTrack
    jsr         punchPokey   
    lda         track_table,x
    sta         ACTIVE_TRACK
    lda         track_table+1,x
    sta         ACTIVE_TRACK+1
    lda         #0
    sta         note
    sta         duration
    sta         active_index
    sta         ready
    rts


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; returns current track that's playing (0xff if nothing is playing)
;;;
;;; prototype: byte playing = _isPlaying();
;;; 
_isPlaying:
    lda         ready       ;; 0 = playing, 0xff = ready to be set
    rts


_vbiActive:
    lda         playing
    rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; our deferred VBI code - it's a simple music player
;;;
_vbiRoutine:
    lda         ready               ; track selected?
    cmp         #0
    beq         _vbiRoutineTrackSelected
    jmp         _vbiExit

_vbiRoutineTrackSelected:
    lda         #0xff               ; set VBI running flag
    sta         playing
    ldx         duration            ; keep playing current note?
    cpx         #0         
    beq         _vbiLoadNewNote     ; nope, load the next one
    dex
    stx         duration
    jmp         _vbiExit            ; exit VBI if we're still playing the current note
_vbiLoadNewNote:
    ldy         active_index        ; get current index into the track
    lda         (ACTIVE_TRACK),y    ; get new note and duration
    cmp         #0xff               ; end of track?
    bne         _vbiLoadNewSound
    jsr         killSound           ; end of track so kill sound
    lda         #0
    sta         note
    sta         duration
    sta         volume
    lda         #0xff               ; tell the world we're ready for a new track
    sta         ready
    jmp         _vbiExit
_vbiLoadNewSound:
    sta         note
    iny
    lda         (ACTIVE_TRACK),y
    sta         duration
    iny
    lda         (ACTIVE_TRACK),y
    sta         volume
    iny
    sty         active_index        ; save index for next LOOP
    jsr         setSound            ; play the frequency / volume
_vbiExit:
    lda         #0                  ; clear VBI running flag
    sta         playing
    jmp         XITVBV              ; hand the VBI back to the system

setSound:
    lda         note
    sta         AUDF1
    lda         note
    sta         AUDF2
    lda         volume
    sta         AUDC1
    sta         AUDC2
    rts

killSound:
    lda         #0
    sta         AUDF1
    sta         AUDC1
    rts


punchPokey:
    lda         #0x00               ; punch pokey
    sta         AUDCTL
    lda         #0x03
    sta         SKCTL   
    rts


_soundTest:
    jsr punchPokey
    ldx #0x00
_x: stx note
    ldy #0x00
_y: sty noteHigh
    lda #0b10000011
    sta volume

    stx note
    sty noteHigh

    lda noteHigh
    sta AUDF1
    lda note
    sta AUDF2
    lda volume
    sta AUDC1
    sta AUDC2

    lda #0
    sta AUDF1
    sta AUDF2
    
    iny
    bne _y
    inx
    bne _x

    lda #0
    sta AUDF1
    sta AUDF2
    sta AUDC1
    sta AUDC2
    rts