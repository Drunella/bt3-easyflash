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

.export _init_io
.export _load_file

; uses zeropage variables
; fd: file id
; fe: directory pointer low
; ff: directory pointer high

.import __CLEARCLC_CALL_LOAD__
.import __CLEARCLC_CALL_RUN__
.import __CLEARCLC_CALL_SIZE__

.import __LOADFILE_CALL_LOAD__
.import __LOADFILE_CALL_RUN__
.import __LOADFILE_CALL_SIZE__

.import __LOADSECTOR_CALL_LOAD__
.import __LOADSECTOR_CALL_RUN__
.import __LOADSECTOR_CALL_SIZE__

.import __LOADFILE_BODY_LOAD__
.import __LOADFILE_BODY_RUN__
.import __LOADFILE_BODY_SIZE__

.import __LOADSECTOR_BODY_LOAD__
.import __LOADSECTOR_BODY_RUN__
.import __LOADSECTOR_BODY_SIZE__

.import __GAMELOADER_LOAD__
.import __GAMELOADER_RUN__
.import __GAMELOADER_SIZE__


.segment "IOLOADER"
    _init_io:
        ; load segment CLEARCLC_CALL
        lda #<__CLEARCLC_CALL_LOAD__
        sta source_address_low
        lda #>__CLEARCLC_CALL_LOAD__
        sta source_address_high
        lda #<__CLEARCLC_CALL_RUN__
        sta destination_address_low
        lda #>__CLEARCLC_CALL_RUN__
        sta destination_address_high
        lda #<__CLEARCLC_CALL_SIZE__
        sta bytes_to_copy_low
        lda #>__CLEARCLC_CALL_SIZE__
        sta bytes_to_copy_high
        jsr copy_segment

        ; load segment LOADFILE_CALL
        lda #<__LOADFILE_CALL_LOAD__
        sta source_address_low
        lda #>__LOADFILE_CALL_LOAD__
        sta source_address_high
        lda #<__LOADFILE_CALL_RUN__
        sta destination_address_low
        lda #>__LOADFILE_CALL_RUN__
        sta destination_address_high
        lda #<__LOADFILE_CALL_SIZE__
        sta bytes_to_copy_low
        lda #>__LOADFILE_CALL_SIZE__
        sta bytes_to_copy_high
        jsr copy_segment

        ; load segment LOADSECTOR_CALL
        lda #<__LOADSECTOR_CALL_LOAD__
        sta source_address_low
        lda #>__LOADSECTOR_CALL_LOAD__
        sta source_address_high
        lda #<__LOADSECTOR_CALL_RUN__
        sta destination_address_low
        lda #>__LOADSECTOR_CALL_RUN__
        sta destination_address_high
        lda #<__LOADSECTOR_CALL_SIZE__
        sta bytes_to_copy_low
        lda #>__LOADSECTOR_CALL_SIZE__
        sta bytes_to_copy_high
        jsr copy_segment

        ; load segment LOADFILE_BODY
        lda #<__LOADFILE_BODY_LOAD__
        sta source_address_low
        lda #>__LOADFILE_BODY_LOAD__
        sta source_address_high
        lda #<__LOADFILE_BODY_RUN__
        sta destination_address_low
        lda #>__LOADFILE_BODY_RUN__
        sta destination_address_high
        lda #<__LOADFILE_BODY_SIZE__
        sta bytes_to_copy_low
        lda #>__LOADFILE_BODY_SIZE__
        sta bytes_to_copy_high
        jsr copy_segment

        ; load segment LOADSECTOR_BODY
        lda #<__LOADSECTOR_BODY_LOAD__
        sta source_address_low
        lda #>__LOADSECTOR_BODY_LOAD__
        sta source_address_high
        lda #<__LOADSECTOR_BODY_RUN__
        sta destination_address_low
        lda #>__LOADSECTOR_BODY_RUN__
        sta destination_address_high
        lda #<__LOADSECTOR_BODY_SIZE__
        sta bytes_to_copy_low
        lda #>__LOADSECTOR_BODY_SIZE__
        sta bytes_to_copy_high
        jsr copy_segment

        ; load segment GAMELOADER
        lda #<__GAMELOADER_LOAD__
        sta source_address_low
        lda #>__GAMELOADER_LOAD__
        sta source_address_high
        lda #<__GAMELOADER_RUN__
        sta destination_address_low
        lda #>__GAMELOADER_RUN__
        sta destination_address_high
        lda #<__GAMELOADER_SIZE__
        sta bytes_to_copy_low
        lda #>__GAMELOADER_SIZE__
        sta bytes_to_copy_high
        jsr copy_segment

        rts


    copy_segment:
        lda bytes_to_copy_low
        beq copy_segment_loop
        inc bytes_to_copy_high
    copy_segment_loop:
    source_address_low = source_address + 1
    source_address_high = source_address + 2
    source_address:
        lda $ffff
    destination_address_low = destination_address + 1
    destination_address_high = destination_address + 2
    destination_address:
        sta $ffff
        ; increase source
        inc source_address_low
        bne :+
        inc source_address_high
    :   ; increase destination
        inc destination_address_low
        bne :+
        inc destination_address_high
    :   ; decrease size
        dec bytes_to_copy_low
        bne copy_segment_loop
        dec bytes_to_copy_high
        bne copy_segment_loop
    :   ; check size
;        lda #$ff
;        bit bytes_to_copy_low
;        bne copy_segment
;        bit bytes_to_copy_high
;        bne copy_segment

        rts

    bytes_to_copy_low:
         .byte $ff
    bytes_to_copy_high:
         .byte $ff      


.segment "CLEARCLC_CALL"

    ; clear_clc(void)
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

        php
        sei
        jmp load_file_body


.segment "LOADFILE_BODY"

    ; load_file(uint8_t filenumber)
    load_file_body:
        ; bank in
        lda $01
        pha
        lda #$37
        sta $01
        lda #EASYFLASH_LED | EASYFLASH_16K
        sta EASYFLASH_CONTROL
        lda #FILES_DIR_BANK
        jsr EAPISetBank

        ; find id
        ldy #data_directory::id
    :   lda ($fe),y           ; load id
        cmp $fd
        beq load_file_found   ; id matches
        lda #$10
        clc
        adc $fe
        sta $fe
        bcc :-                ; next file
        plp
        rts

    load_file_found:
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
        bcs :++
        inc load_destination_low
        bne :+
        inc load_destination_high
    :   bne data_loader

        ; bank out
    :   lda #EASYFLASH_KILL
        sta EASYFLASH_CONTROL
        pla
        sta $01
        plp
        rts

;    bank .byte
;    offset_low .byte
;    offset_high .byte
;    size_low .byte
;    size_high .byte
;    size_upper .byte
;    load_low .byte
;    load_high .byte
;    init_low .byte
;    init_high .byte
;    reserved .byte .byte .byte .byte .byte


    load_file_run:
    load_file_run_low = load_file_run + 1
    load_file_run_high = load_file_run + 2
        jsr $ffff
        rts


.segment "LOADSECTOR_CALL"

    ; load_sector() @ $be7a
    ;   $42 mode (1: read, 2: write, 3:?)
    ;   $43 ?
    ;   $44 low address destination buffer
    ;   $45 high address destination buffer
    ;   $46 low sector number
    ;   $47 high sector number
    _load_sector:
        ; branch if not load
        ldx #1
        cpx $42
        beq :+
        jmp save_sector_body
    :   jmp load_sector_body


.segment "LOADSECTOR_BODY"

    load_sector_body:
        lda $45              ; destination buffer
        sta transfer_dest_address_lower + 2
        sta transfer_dest_address_upper + 2
        inc transfer_dest_address_upper + 2
        lda $44
        sta transfer_dest_address_lower + 1
        sta transfer_dest_address_upper + 1

        ; calculate load address from sector
        clc                  ; low address 5 low bits -> high address
        lda $46
        asl a
        and #$3f
        adc #$80
        sta transfer_source_address_lower + 2
        sta transfer_source_address_upper + 2
        inc transfer_source_address_upper + 2

        lda $46              ; low address 3 high bits -> bank
        lsr a
        lsr a
        lsr a
        lsr a
        lsr a
        clc
        adc $47
        adc current_disk     ; 
        tax                  ; safe in x

;        lda #$80
;        adc transfer_source_address_lower + 2
;        sta transfer_source_address_lower + 2
;        lda #$81
;        adc transfer_source_address_upper + 2
;        sta transfer_source_address_upper + 2

        php
        sei

        ; bank in
        lda $01
        pha
        lda #$37
        sta $01
        lda #EASYFLASH_LED | EASYFLASH_16K
        sta EASYFLASH_CONTROL

        txa                  ; get back bank
        jsr EAPISetBank      ; set bank

        ldx #$00
    transfer_loop:
    transfer_source_address_lower:
        lda $8000, x
    transfer_dest_address_lower:
        sta $ba00, x
    transfer_source_address_upper:
        lda $8100, x
    transfer_dest_address_upper:
        sta $bb00, x
;        inc transfer_dest_address_lower + 1
;        bne :+
;        inc transfer_dest_address_lower + 2
;        inc transfer_dest_address_upper + 1
;        bne :+
;        inc transfer_dest_address_upper + 2
        inx
        bne transfer_loop

        ; bank out
        lda #EASYFLASH_KILL
        sta EASYFLASH_CONTROL
        pla
        sta $01
        cli

        plp
        rts


    save_sector_body:
        ; ###
        rts

    current_disk:
        .byte $08            ; character disk

