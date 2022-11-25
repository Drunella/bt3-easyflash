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
;sector_map_bank = 35
;savegame_bank = 48


.export _init_io
;.export _load_file
;.export bd3_current_disk_index

.import __CLEARCLC_CALL_LOAD__
.import __CLEARCLC_CALL_RUN__
.import __CLEARCLC_CALL_SIZE__

.import __LOADFILE_CALL_LOAD__
.import __LOADFILE_CALL_RUN__
.import __LOADFILE_CALL_SIZE__

.import __LOADSECTOR_CALL_LOAD__
.import __LOADSECTOR_CALL_RUN__
.import __LOADSECTOR_CALL_SIZE__

.import __LOAD_DATA_LOAD__
.import __LOAD_DATA_RUN__
.import __LOAD_DATA_SIZE__

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

        ; load segment LOAD_DATA
        lda #<__LOAD_DATA_LOAD__
        sta source_address_low
        lda #>__LOAD_DATA_LOAD__
        sta source_address_high
        lda #<__LOAD_DATA_RUN__
        sta destination_address_low
        lda #>__LOAD_DATA_RUN__
        sta destination_address_high
        lda #<__LOAD_DATA_SIZE__
        sta bytes_to_copy_low
        lda #>__LOAD_DATA_SIZE__
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

        ; prepare save storage
;        jsr prepare_save_storage

        rts


    bytes_to_copy_low:
         .byte $ff
    bytes_to_copy_high:
         .byte $ff      

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
        rts


    prepare_save_storage:
        ; save mapping and bank in
        lda $01
        pha
        sei
        lda #$37   ; memory map, read from ef
        sta $01
        lda #EASYFLASH_LED | EASYFLASH_16K
        sta EASYFLASH_CONTROL
        lda #$00
        jsr EAPISetBank

        ; clear area
        ldx #$00
        lda #$00
    :   sta $df00, x
        inx
        cpx #$80
        bne :-

        ; initialise 
;CHARACTER_DISK_BANK = 13
;SAVE_10B_BANK = 40
;SAVE_10E_BANK = 48
;SAVE_110_BANK = 56
        lda #(SAVE_10B_BANK - CHARACTER_DISK_BANK)
        sta $df0b
        sta $df0c
        sta $df0d

        lda #(SAVE_10E_BANK - CHARACTER_DISK_BANK)
        sta $df0b
        sta $df0c

        lda #(SAVE_110_BANK - CHARACTER_DISK_BANK)
        sta $df10
        sta $df11
        sta $df12
        sta $df13
        sta $df14
        sta $df15
        sta $df16
        sta $df17

        ; bankout and return
        lda #$37   ; memory map, read from ef
        sta $01
        lda #EASYFLASH_KILL
        sta EASYFLASH_CONTROL
        pla
        sta $01    ; restore memory map
        cli
        rts

