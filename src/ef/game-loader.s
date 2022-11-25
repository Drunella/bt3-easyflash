; ----------------------------------------------------------------------------
; Copyright 2023 Drunella
;
; Licensed under the Apache License, Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
;     http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the specific language governing permissions and
; limitations under the License.
; ----------------------------------------------------------------------------


.include "easyflash.i"

.import _load_file

.export _startup_game
.export _startup_utility
.export _startup_editor

.import __STARTGAME_LOAD__
.import __STARTGAME_RUN__
.import __STARTGAME_SIZE__

diskswitcher_run = $67cb
.import bd3_current_disk_index


.segment "GAMELOADER"

    diskswitcher_load:
        ; we replace the following code in BARDSUBS1 (8 bytes)
        ; 67CB   20 70 6A   JSR L6A70   ; load sector 0
        ; 67CE   B0 2A      BCS L67FA   ; ? (on error)
        ; 67D0   AD FF BB   LDA $BBFF   ; load diskid
        lda $6ad8  ; variable of expected disk id
        sta bd3_current_disk_index
        nop
        nop
    diskswitcher_end:
    diskswitcher_size = diskswitcher_end - diskswitcher_load


    startup_init_game:
        lda #$20
        sta $034e                   ; unknown purpose
        sei
        lda #$35                    ; memory
        sta $01
        lda #$2f
        sta $00
        ldx #<_startup_game  ; set cold reset vector to 1800
        ldy #>_startup_game
        stx $fffc
        sty $fffd
        rts


    startup_init_utility:
        lda #$20
        sta $034e                   ; unknown purpose
        sei
        lda #$35                    ; memory
        sta $01
        lda #$2f
        sta $00
        ldx #<_startup_utility  ; set cold reset vector to 1800
        ldy #>_startup_utility
        stx $fffc
        sty $fffd
        rts


    startup_replacement:
        ; copy DISKSWITCHER
        ldx #<diskswitcher_size
    :   lda diskswitcher_load-1, x
        sta diskswitcher_run-1, x
        dex
        bne :-
        rts


    startup_init:
        ldx #<startup_dummy_vector
        ldy #>startup_dummy_vector
        stx $fffa
        sty $fffb
        stx $fffe
        sty $ffff
        lda #$7f                    ;   enable interrupts (?)
        sta $dc0d
        sta $dd0d
        lda $dc0d
        sta $dd0d
        ldx #$00
    label_19aa:
        lda #$01                    ; init color ram
        sta $d800,x
        sta $d900,x
        sta $da00,x
        sta $db00,x
        lda #$86                    ; init screen
        sta $0400,x
        sta $0500,x
        sta $0600,x
        sta $0700,x
        cpx #$2e
        bcs label_19d9
        lda data_19ee, x          ; write to vicii (from 19ee on, max $2e bytes)
        sta $d000,x
        cpx #$1d
        bcs label_19d9
        lda #$00
        sta $d400,x                 ; write 0 to sid, max $1d bytes
    label_19d9:
        inx
        bne label_19aa
        lda $dd00                   ; init cia2 (?)
        ora #$03
        sta $dd00
        lda #$3f
        sta $dd02
        lda #$34                    ; set port direction
        sta $01
        rts

    data_19ee:
        .byte $00, $00, $00, $00, $00, $00, $00, $00 
        .byte $00, $00, $00, $00, $00, $00, $00, $00 
        .byte $00, $07, $00, $00, $00, $00, $18, $00 
        .byte $18, $00, $00, $00, $00, $00, $00, $00 
        .byte $00, $00, $02, $03, $04, $00, $01, $07
        .byte $08, $09, $0a, $0b, $0c, $0d, $0e, $0f


    startup_memory:
        lda #$35             ; memory mapping, io visible
        sta $01
        ldx #$00

    label_1880:
        lda $3e00,x          ; copy to screen
        sta $0400,x          
        lda $3f00,x          ; copy to screen
        sta $0500,x
        lda $4000,x          ; copy to screen
        sta $0600,x
        lda $4100,x          ; copy to screen
        sta $0700,x

        lda $41c0,x          ; copy to color ram
        sta $d800,x
        lda $42c0,x          ; copy to color ram
        sta $d900,x
        lda $43c0,x          ; copy to color ram
        sta $da00,x
        lda $44c0,x          ; copy to color ram
        sta $db00,x

        lda $4800,x          ; copy MUSIC (overwrites load files)
        sta $c480,x
        lda $4900,x          ; copy MUSIC
        sta $c580,x
        lda $4a00,x          ; copy MUSIC
        sta $c680,x
        lda $4b00,x          ; copy MUSIC
        sta $c780,x
        lda $4c00,x          ; copy MUSIC
        sta $c880,x
        lda $4d00,x          ; copy MUSIC
        sta $c980,x
        lda $4e00,x          ; copy MUSIC
        sta $ca80,x
        lda $4f00,x          ; copy MUSIC
        sta $cb80,x
        lda $4f80,x          ; copy MUSIC
        sta $cc00,x

        inx
        bne label_1880
        lda #$34
        sta $01
        rts


    startup_dummy_vector:
        rti


    _startup_game:
        jsr startup_init_game
        jsr startup_init
        lda #11
        jsr _load_file       ; load MAINPIC64
        lda #12
        jsr _load_file       ; load DMUSIC64
        lda #13
        jsr _load_file       ; load SPLASH64

        ; splash
        cli
; ###        jsr $4000             ; we ignore the pressed key

        lda #$35 
        sta $01  
        lda #$07 
        sta $D011            ; screen off
        lda #$34 
        sta $01

        lda #21
        jsr _load_file       ; load BARD3.PIC64
        lda #22
        jsr _load_file       ; load MUSIC64
        lda #23
        jsr _load_file       ; load BARDSUBS2
        lda #24
        jsr _load_file       ; load BARDSUBS1

        jsr startup_replacement  ; replacements

        jsr startup_memory

        ldx #$00             ; continue to initialize
    label_18f0:
        txa
        lsr a
        lda #$ff
        bcs label_18f8
        lda #$fe
    label_18f8:
        sta $4000,x          ; clear memory
        sta $4100,x
        sta $4200,x
        cpx #$fe
        bcs label_1910
        lda #$00
        sta $02,x
        cpx #$e0
        bcs label_1910
        sta $0300,x          ; clear $0300 page with exceptions 
    label_1910:
        inx
        bne label_18f0

        lda #$41
        sta $034e

        ldx #$ff             ; reset stack
        txs
        cli
        cld                  ; no decimal

        lda #$35             ; memory mapping
        sta $01
        lda #$37
        sta $d011            ; set vicii mode (screen still off)
        lda #$34
        sta $01
        jmp $ff5b            ; start game


    _startup_utility:
        jsr startup_init_utility
        lda #31
        jsr _load_file       ; load UTIL64
        ; load own backup / restore code ###
        nop  ; todo
        rts


    _startup_editor:
        ; load editor ###
        ; editor will probably not use any bard's tale 3 code ###
        ; todo ###
        rts

