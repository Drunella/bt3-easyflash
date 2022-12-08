; ----------------------------------------------------------------------------
; Copyright 2023 Drunella
;
; Licensed under the Apache License, Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
;     http://www.apache.org/licen~ses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the specific language governing permissions and
; limitations under the License.
; ----------------------------------------------------------------------------

.include "easyflash.i"
.include "global.i"


.import var_io_buffer_low
.import var_io_buffer_high
.import var_io_sector_low
.import var_io_sector_high

.import backup_zeropage_2328
.import restore_zeropage_2328

.import calculate_track_sector, calc_track_number, calc_sector_number

.import cbm_read_sector, cbm_backup_zeropage, cbm_restore_zeropage

.export util64_rw_ef_sector
.export util64_r_cbm_sector
.export util64_r_prodos_sector


.segment "CODE"

    util64_rw_ef_sector:
        ; read or write prodos sector from easyflash
        pha
        jsr backup_zeropage_2328
        pla            
        sta $42        
        lda var_io_buffer_low
        sta $44
        lda var_io_buffer_high
        sta $45
        lda var_io_sector_low
        sta $46
        lda var_io_sector_high
        sta $47
        jsr loadsave_sector_body_addr
        pha            
        jsr restore_zeropage_2328
        pla
        cmp #$01
        rts



    util64_r_cbm_sector:
        ; read cbm sector from floppy
        jsr cbm_backup_zeropage
        lda $44
        sta $ae
        lda $45
        sta $af
        lda calc_track_number
        sta $ab
        lda calc_sector_number
        sta $ac
        lda #$08
        sta $ad
        lda $01
        pha
        lda #$36
        sta $01
        jsr cbm_read_sector
        bne :+
        pla
        sta $01
        jsr cbm_restore_zeropage
        clc
        rts

    :   pla
        sta $01
        jsr cbm_restore_zeropage
        sec
        rts
        

    util64_r_prodos_sector:
        ; read prodos sector from disk
        clc
        jsr calculate_track_sector
        jsr util64_r_cbm_sector
        bcs :+
        sec  ; this adds one to the prodos sector number
        jsr calculate_track_sector
        inc $45
        jsr util64_r_cbm_sector
        dec $45
    :   rts

