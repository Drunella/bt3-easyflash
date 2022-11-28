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

.import sp, popa

.export _read_ef_sector
.export _write_ef_sector 


.segment "CODE"

    ; uint8_t __fastcall__ read_pd_sector(uint16_t sector, char* destination)
    _read_ef_sector:
        ; $42 mode (1: read, 2: write, 3:?)
        ; $43 ?
        ; $44 low address destination buffer
        ; $45 high address destination buffer
        ; $46 low sector number
        ; $47 high sector number
        sta $44    ; a: destination low byte
        stx $45    ; x: destination high byte
        jsr popa   ; sector low byte
        sta $46
        jsr popa   ; sector high byte
        sta $47
        lda #$01   ; read mode
        sta $42
        sta io_set_current_disk_addr
    
        lda $01
        pha
        jsr io_loadsave_sector_addr
        pla
        sta $01
        bcs :+     ; carry set is error
        ldx #$00
        lda #$00
        rts
    :   ldx #$00   ; error
        lda #$01
        rts

    ; uint8_t __fastcall__ write_pd_sector(uint16_t sector, char* source)
    _write_ef_sector:
        ; $42 mode (1: read, 2: write, 3:?)
        ; $43 ?
        ; $44 low address destination buffer
        ; $45 high address destination buffer
        ; $46 low sector number
        ; $47 high sector number
        sta $44    ; a: destination low byte
        stx $45    ; x: destination high byte
        jsr popa   ; sector low byte
        sta $46
        jsr popa   ; sector high byte
        sta $47
        lda #$02   ; writemode
        sta $42
        lda #$01
        sta io_set_current_disk_addr
    
        lda $01
        pha
        jsr io_loadsave_sector_addr
        pla
        sta $01
        bcs :+     ; carry set is error
        ldx #$00
        lda #$00
        rts
    :   ldx #$00   ; error
        lda #$01
        rts

