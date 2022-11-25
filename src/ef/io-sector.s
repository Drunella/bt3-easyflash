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


.export bd3_current_disk_index


.segment "LOADSECTOR_CALL"

    ; load_sector() @ $be7a
    ;   $42 mode (1: read, 2: write, 3:?)
    ;   $43 ?
    ;   $44 low address destination buffer
    ;   $45 high address destination buffer
    ;   $46 low sector number
    ;   $47 high sector number
    _load_sector:
        jsr loadsave_sector_body
        rts
;        lda $42
;        cmp #$03
;        beq do_3          ; branch if mode == 3
;        cmp #$02
;        bne do_load       ; branch if mode != 2
;        jmp save_sector_body
;    do_3:
;        sec  ;  for now set error
;        rts
;    do_load:
;        jmp load_sector_body


.segment "LOADSECTOR_BODY"

    save_mapping:
        .byte $37
    ef_bank:
        .byte $08
;    ef_bankmode:
;        .byte $d0        
;    ef_type:
;        .byte $00
    ef_address_high:
        .byte $80
    bd3_current_disk_index:
        .byte $01            ; 1: character, 2: dungeon a, 3: dungeon b
    bd3_current_disk:
        .byte 0              ; invalid
        .byte CHARACTER_DISK_BANK  ; start bank for character disk
        .byte DUNGEONA_DISK_BANK   ; start bank for dungeon a
        .byte DUNGEONB_DISK_BANK   ; start bank for dungeon b


    loadsave_sector_body:
        ; load, save or mode 3?
        lda $42
        cmp #$03        
        beq loadsave_mode3  ; branch if mode == 3
        cmp #$02
        bne loadsave_load   ; branch if mode != 2

        ; save sector
        jsr easyflash_bankin
        jsr calculate_sector_source
        jsr prepare_easyflash_storage
        bcc :+  ; branch if saving is possible
        sec
        rts
    :   jsr save_sector_data
        inc ef_address_high
        inc $45
        jsr save_sector_data
        dec $45
        jsr finish_easyflash_storage
        jsr easyflash_bankout
        clc
        rts
        
    loadsave_mode3:  ; mode 3
        sec
        rts                 ; for now return error on mode 3

    loadsave_load:  ; load sector
        jsr easyflash_bankin
        jsr calculate_sector_source
        jsr load_sector_data
        inc ef_address_high
        inc $45
        jsr load_sector_data
        dec $45
        jsr easyflash_bankout
        clc
        rts
 

    easyflash_bankin:
        ; a: bank to switch to
;        pha
        sei
        lda $01
        sta save_mapping
        lda #$37   ; memory map, read from ef
        sta $01
        lda #EASYFLASH_LED | EASYFLASH_16K
        sta EASYFLASH_CONTROL
;        pla
;        jsr EAPISetBank
        rts


    easyflash_bankout:
        lda #$37   ; memory map, read from ef
        sta $01
        lda #EASYFLASH_KILL
        sta EASYFLASH_CONTROL
        lda save_mapping
        sta $01    ; restore memory map
        cli
        rts


    load_sector_data:
        ; destination
        lda $44
        sta load_sector_destination_low
        lda $45
        sta load_sector_destination_high

        ; source
        lda ef_address_high
        sta load_sector_source_high

        ;jsr easyflash_bankin  ; bank in and set bank
        ldy #$37   ; memory map: read from ef
        sty $01
        lda ef_bank
        jsr EAPISetBank
        
        ldx #$00
        ; transfer loop
    :   ldy #$37   ; memory map: read from ef
        sty $01
    load_sector_source:
    load_sector_source_high = load_sector_source + 2
        lda $8000, x
        ldy #$34   ; memory map: all ram visible, no ef
        sty $01
    load_sector_destination:
    load_sector_destination_low = load_sector_destination + 1
    load_sector_destination_high = load_sector_destination + 2
        sta $ba00, x
        inx
        bne :-               ; loop 256 times

        ;jmp easyflash_bankout
        ldy #$37   ; memory map: read from ef
        sty $01
        rts
        

; saving ----------------------------------------

    save_sector_data:
        ; source
        lda $44
        sta save_sector_source_low
        lda $45
        sta save_sector_source_high

        ; prepare to save data
        ldy #$37   ; memory map: ef accessible
        sty $01
        lda ef_bank
        ;jsr easyflash_bankin  ; bank in and set bank
        jsr EAPISetBank

        ldx #$00
        ; transfer loop
    :   ldy #$34
        sty $01
    save_sector_source:
    save_sector_source_low = save_sector_source + 1
    save_sector_source_high = save_sector_source + 2
        lda $ba00, x
        ldy #$37   ; memory map: ef accessible
        sty $01
        ldy ef_address_high
        ;jsr EAPIWriteFlash ; ###
        clc  ; ### as test only
        nop  ; ### test only ###
        nop  ; ### test only ###
        bcs save_error
        inx
        bne :-               ; loop 256 times

        ;jsr easyflash_bankout
        clc
        rts
    save_error:
        ;jsr easyflash_bankout
        sec
        rts


;    save_sector_data:
;        ; source
;        lda $44
;        sta save_sector_source_low
;        lda $45
;        sta save_sector_source_high
;
;        ; prepare to save data
;        lda ef_bank
;        jsr easyflash_bankin  ; bank in and set bank
;
;        ; destination and length
;        lda ef_bankmode
;        ldy ef_address_high
;        ldx #$00
;        jsr EAPISetPtr
;        lda #$00
;        tax
;        ldy #$1  ; 256 bytes length
;        jsr EAPISetLen
;
;        ldx #$00
;        ; transfer loop
;    :   ldy #$34   ; read from memory: all ram visible, no ef
;        sty $01
;    save_sector_source:
;    save_sector_source_low = save_sector_source + 1
;    save_sector_source_high = save_sector_source + 2
;        lda $ba00, x
;        ldy #$37   ; memory map: ef accessible
;        sty $01
;; ###        jsr EAPIWriteFlashInc
;        clc  ; ###
;        bcs save_error
;        inx
;        bne :-               ; loop 256 times
;
;        jsr easyflash_bankout
;        clc
;        rts
;    save_error:
;        jsr easyflash_bankout
;        sec
;        rts


    prepare_easyflash_storage:
        ; $46 low sector number
        ; important: we assume that only correct sector numbers ask for saving
        ;lda #$01  ; correct bank
        ;jsr easyflash_bankin

        ; correct bank offset
        clc
        ldx $46
        lda $df00, x  ; load prepared bank offset to store data
        adc ef_bank
        sta ef_bank

        ; correct ef_address_high
        lda ef_address_high
        cmp $a0
        bcc :+  ; is in low area
        clc
        adc #$40
        sta ef_address_high
    :   

        ; correct ef_address_high from $a0 to $e0

        ; find next free slot, there will always be a free one
        
        ; erase previous area if we started in a new area
        
        ; must set correctly: 
        ; ef_bankmode
        ; ef_address_high
        ; ef_bank

        ;jmp easyflash_bankout
        rts


    finish_easyflash_storage:
        rts  ; ### debug ###
        ; $46 low sector number
        ; important: we assume that only correct sector numbers ask for saving
        ;lda #$00  ; correct bank
        ;jsr easyflash_bankin

        lda $46
        cmp #$0d   ; 010d: save game
        bne :+
        ; next area
        ; clean area
        jmp :+++

    :   lda $46
        cmp #$0f   ; 010f: unknown area
        bne :+
        ; next area
        ; clean area
        jmp :++

    :   lda $46
        cmp #$17   ; 0117: refugee camp
        bne :+
        ; next area
        ; clean area
        ;jmp :+

    :   ;jmp easyflash_bankout
        rts


; calculate sector sources ----------------------

    calculate_sector_source:
        clc
        lda $46              ; sector 0-5 low bits -> high address of ef
        rol a                ; 
        and #$3f
        clc
        adc #$80
        sta ef_address_high

        lda $46              ; low address 3 high bits -> bank
        lsr a
        lsr a
        lsr a
        lsr a
        lsr a
        ldx $47
        clc
        beq :+
        adc #$08             ; if bit 0 of sector_high is set, add value 8

    :   ldx bd3_current_disk_index     ; bank offset for disk
        clc
        adc bd3_current_disk, x
        sta ef_bank

        rts

        ; correct bank offset
;        clc
;        ldx $46
;        lda $df00, x  ; load prepared bank offset to store data
;        adc ef_bank
;        sta ef_bank
;        rts

        ; correct ef_address_high
;        lda ef_address_high
;        cmp $a0
;        bcc :+  ; is in low area
;        clc
;        adc #$40
;        sta ef_address_high
;    :
;
;        rts



;    get_value:
;        ldx $46              ; low byte of sector id is the table index
;    table_index:
;    table_index_low = table_index + 1
;    table_index_high = table_index + 2
;        lda $8000, x
;        rts
;
;
;    calculate_easyflash_parameters:
;        lda #sector_map_bank
;        jsr easyflash_bankin  ; bank in and set bank
;
;        ldx bd3_current_disk_index  ; offset for disk
;        lda bd3_current_disk, x
;        clc
;        adc $47              ; add sector id highest bit
;        sta table_index_high
;        jsr get_value
;        sta ef_type
;
;        inc table_index_high
;        inc table_index_high
;        jsr get_value
;        sta ef_bank
;
;        inc table_index_high  ; prepare and get address
;        inc table_index_high
;        jsr get_value
;        sta ef_address_high
;        
;        ;lda #$ff
;        bit ef_type
;        bvc :+
;        lda #$80
;        jsr calculate_easyflash_storage  ; recalculate for save area a
;        jmp easyflash_bankout
;    :   bpl :+
;        lda #$a0
;        jsr calculate_easyflash_storage  ; recalculate for save area b
;    :   jmp easyflash_bankout


;    calculate_easyflash_storage:
;        ; we are banked in
;        ; a: $80 for zone a, $a0 for zone b
;        ; header block
;        ; 0:    $ff area free; $00 block in use (could have free space)
;        ; 1-15: $ff block free, $80 block in use, $00 block deleted
;        ; check block header in a, if free increase to next area, increase bank by 8
;        ; loop over th 15 entries until we find the in use flag ($80)
;        sta storage_value_high
;        lda ef_bank
;        jsr EAPISetBank
;    
;        ldy #$00
;        ldx #$00
;    storage_value:
;        lda $8000, x
;    storage_value_high = table_index + 2
;        inx
;        cmp #$7f
;        bne :+
;        clc
;        txa
;        
;    :
;        cpx #$0f  ; jump to next block
;        bne :+
;        clc
        rts


; calculate sector sources (old) ----------------

;    calculate_easyflash_parameters:
;        lda $46              ; sector 0-5 low bits -> high address of ef
;        rol a                ; set carry for +1
;        and #$3f
;        clc
;        adc #$80
;        sta ef_address_high
;
;        lda $46              ; low address 3 high bits -> bank
;        lsr a
;        lsr a
;        lsr a
;        lsr a
;        lsr a
;        ldx $47
;        clc
;        beq :+
;        adc #$08             ; if bit 0 of sector_high is set, add value 8
;
;    :   ldx bd3_current_disk_index     ; bank offset for disk
;        clc
;        adc bd3_current_disk, x
;        sta ef_bank
;
;        ;lda #$00
;        ;sta ef_address_low
;
;        rts



; ------------------------------------------------------------------------
; attic

; ### calculate extended source for save zones
; ### find current save area in its area
; ### save zone a: low addresses only, save zone b: high addresses only

; ### saving of sectors
; ### find next save area
; ### if in new block erase old area
; ### save sector


    ;resolve_easyflash_parameters:
        ; ef_type
        ; ef_address_high
        ; ef_bank

        ; bank
;        get_value
;        sta ef_bank

        ; offset
;        inc $table_index_high
;        inc $table_index_high
;        jsr get_value
;        sta ef_address_high

        ; type
;        inc $table_index_high
;        inc $table_index_high
;        jsr get_value
;        sta ef_type
;        bit ef_type
        ; branch on negative flag
        ; branch on overflow flag

        ; no save allowed

;        ldx bd3_current_disk_index     ; bank offset for disk
;        clc
;        adc bd3_current_disk, x
;        sta ef_bank
        

;  ldy #[bankin]
;  sty ef_control
;  lda [source], x
;  ldy #$34     ; memory map
;  ldy #[bankout]
;  sty ef_control
;  sty $01
;  sta [dest], x
;  ldy #$35
;  sty
;  inx
;  bne loop 


;    load_sector_body:
;        lda $45              ; destination buffer
;        sta transfer_dest_address_lower + 2
;        sta transfer_dest_address_upper + 2
;        inc transfer_dest_address_upper + 2
;        lda $44
;        sta transfer_dest_address_lower + 1
;        sta transfer_dest_address_upper + 1

        ; calculate load address from sector
;        clc                  ; low address 5 low bits -> high address
;        lda $46
;        asl a
;        and #$3f
;        adc #$80
;        sta transfer_source_address_lower + 2
;        sta transfer_source_address_upper + 2
;        inc transfer_source_address_upper + 2

;        lda $46              ; low address 3 high bits -> bank
;        lsr a
;        lsr a
;        lsr a
;        lsr a
;        lsr a
;        clc
;        adc $47
;        adc current_disk     ; 
;        tax                  ; safe in x

;        lda #$80
;        adc transfer_source_address_lower + 2
;        sta transfer_source_address_lower + 2
;        lda #$81
;        adc transfer_source_address_upper + 2
;        sta transfer_source_address_upper + 2

;        php
;        sei

        ; bank in
;        lda $01
;        pha
;        lda #$37
;        sta $01
;        lda #EASYFLASH_LED | EASYFLASH_16K
;        sta EASYFLASH_CONTROL

;        txa                  ; get back bank
;        jsr EAPISetBank      ; set bank

;        ldx #$00
;    transfer_loop:
;    transfer_source_address_lower:
;        lda $8000, x
;    transfer_dest_address_lower:
;        sta $ba00, x
;    transfer_source_address_upper:
;        lda $8100, x
;    transfer_dest_address_upper:
;        sta $bb00, x
;        inc transfer_dest_address_lower + 1
;        bne :+
;        inc transfer_dest_address_lower + 2
;        inc transfer_dest_address_upper + 1
;        bne :+
;        inc transfer_dest_address_upper + 2
;        inx
;        bne transfer_loop

        ; bank out
;        lda #EASYFLASH_KILL
;        sta EASYFLASH_CONTROL
;        pla
;        sta $01
;        cli

;        plp
;        rts


;    save_sector_body:
        ; ###
;        rts


