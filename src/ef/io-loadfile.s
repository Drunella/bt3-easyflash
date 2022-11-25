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

; uses zeropage variables
; fd: file id
; fe: directory pointer low
; ff: directory pointer high


file_directory_entry = $10

.export _load_file


.segment "CLEARCLC_CALL"

    ; clear_clc(void)
    ; I do not know if this is necessary
    _clear_clc:
        clc
        rts


.segment "LOADFILE_CALL"

    ; load_file(uint8_t filenumber) @ $be02
    _load_file:
        ; save id
        sta $fd

        ; save directory location
        lda #>FILES_DIR_START
        sta $ff
        lda #$00
        sta $fe

        jmp loadfile_body


.segment "LOAD_DATA"

        ; meaning yet to be determined: data @ bd40
        .byte $00, $00, $00, $03, $7E, $BE, $00, $BA
        .byte $00, $04, $00, $00, $00, $00, $00, $00
        .byte $00, $01, $00, $02, $00, $00, $00, $00
        .byte $0A, $7E, $BE, $C3, $06, $00, $00, $00
        .byte $00, $00, $00, $00, $00, $00, $00, $00
        .byte $00, $00, $03, $60, $00, $BA, $00, $00
        .byte $01, $7E, $BE, $04, $00, $00, $00, $00
        .byte $00, $00


.segment "LOADFILE_BODY"

    ; loadfile_body
    loadfile_body:
        ; bank in
        lda #FILES_DIR_BANK
        jsr loadfile_bankin

        ; find id
        ; id(1), bank(1), offset(2), size(3), load(2), init(2), dummy(5)
        ldy #data_directory::id
    :   lda ($fe),y           ; load id
        cmp $fd
        beq loadfile_found    ; id matches
        cmp #$ff
        beq loadfile_finish   ; eof marker found
        clc
        lda #FILES_DIR_ENTRY
        adc $fe
        sta $fe               
        lda #$00
        adc $ff
        sta $ff
        jmp :-

    loadfile_found:
        ; destination
        ldy #data_directory::load_low
        lda ($fe), y
        sta load_destination_low
        ldy #data_directory::load_high
        lda ($fe), y
        sta load_destination_high

        ; call address
        ldy #data_directory::init_low
        lda ($fe), y
        sta load_file_run_low
        ldy #data_directory::init_high
        lda ($fe), y
        sta load_file_run_high

        ; EAPISetLen: XYA length, 24 bits (X = low, Y = med, A = high)
        ldy #data_directory::size_low
        lda ($fe), y
        tax
        ldy #data_directory::size_high
        lda ($fe), y
        tay
        lda #$00
        jsr EAPISetLen

        ; EAPISetPtr: A bank mode, XY address (X = low) address is in range $8000-$bfff
        ldy #data_directory::offset_low
        lda ($fe), y
        tax
        ldy #data_directory::offset_high
        lda ($fe), y
        tay
        lda #$D0
        jsr EAPISetPtr
        
        ; EAPISetBank: A bank
        ldy #data_directory::bank
        lda ($fe), y
        jsr EAPISetBank
        
        ldx #$34
        ldy #$37
    data_loader:
        jsr EAPIReadFlashInc
        stx $01
    load_destination_low = load_destination + 1
    load_destination_high = load_destination + 2
    load_destination:
        sta $ffff
        sty $01
        bcs loadfile_finish
        inc load_destination_low
        bne :+
        inc load_destination_high
    :   bne data_loader

        ; bank out
    loadfile_finish:
         jmp loadfile_bankout


    load_file_run:
    load_file_run_low = load_file_run + 1
    load_file_run_high = load_file_run + 2
        jsr $ffff
        rts


    loadfile_mapping:
        .byte $37

    loadfile_bankin:
        ; a: bank to switch to
        pha
        sei
        lda $01
        sta loadfile_mapping
        lda #$37   ; memory map, read from ef
        sta $01
        lda #EASYFLASH_LED | EASYFLASH_16K
        sta EASYFLASH_CONTROL
        pla
        jsr EAPISetBank
        rts


    loadfile_bankout:
        lda #$37   ; memory map, read from ef
        sta $01
        lda #EASYFLASH_KILL
        sta EASYFLASH_CONTROL
        lda loadfile_mapping
        sta $01    ; restore memory map
        cli
        rts

