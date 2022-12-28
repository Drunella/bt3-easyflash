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


.export _load_file
.export _io_load_sector
.export _set_current_disk


.import loadsave_sector_body
.import loadfile_body
.import bt3_current_disk_index

.segment "CLEARCLC_CALL"

    ; clear_clc(void) @ $be00
    ; I do not know if this is necessary
    _clear_clc:
        clc
        rts


.segment "LOADFILE_CALL"

    ; load_file(uint8_t filenumber) @ $be02
    _load_file:
        ; save id
        sta $fb

        ; save directory location
        lda #>FILES_DIR_START
        sta $ff
        lda #$00
        sta $fe

        ; result in $fb
        jmp loadfile_body


.segment "SETDISK_CALL"

    ; set disk @ $be38
    _set_current_disk:
        sta bt3_current_disk_index
        rts


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


.segment "LOADSECTOR_CALL"

    ; load_sector() @ $be7a
    ;   $42 mode (1: read, 2: write, 3:?)
    ;   $43 ?
    ;   $44 low address destination buffer
    ;   $45 high address destination buffer
    ;   $46 low sector number
    ;   $47 high sector number
    _io_load_sector:
        jsr loadsave_sector_body
        rts

