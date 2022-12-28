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

; important variables in yx
; y
; 0 : current bank, absolute value
; 1 : current offset, absolute value
; 2 : next bank (for writing), absolute value
; 3 : next offset (for writing), absolute value
;  x 
;  0: game
;  1: game
;  2: game
;  3: storage
;  4: storage
;  5: camp
;  6: camp
;  7: camp
;  8: camp
;  9: camp
;  a: camp
;  b: camp
;  c: camp
;  d:  0
;  e:  0
;  f:  0
;
; df40: erase command (=0: do not erase, >0: erase)
; df41: erase offset high value
; df42: erase bank
;
; df50-df5f: variables
; df60-df6f: variables
; df70-df7f: variables


.include "easyflash.i"


.export bt3_current_disk_index
.export init_save_storage
.export loadsave_sector_body

.export _compare_data

erase_command = $df40
erase_bank = $df42
erase_offset  = $df41



.segment "IOLOADER"

    init_save_storage:
        ; save mapping and bank
        sei
        lda $01
        pha
        lda #$37   ; memory map, read from ef
        sta $01
        lda #EASYFLASH_LED | EASYFLASH_16K
        sta EASYFLASH_CONTROL
        ;lda #$00
        ;jsr EAPISetBank

        ; clear variable area
        ldx #$00
        lda #$00
    :   sta $df00, x
        inx
        cpx #$80
        bne :-

        ; initialise save game area
    init_10b:
        lda #SAVE_10B_BANK
        jsr EAPISetBank

        ldx #$00
        ldy #$80
        lda #$ff
        jsr _compare_data    ; yx: address (x low), a: data
        bcc set_10b_a0       ; branch if no data in $8000
    set_10b_80:
        lda #SAVE_10B_BANK   ; set save game is in low chip
        sta $df00
        sta $df01
        sta $df02
        sta $df20
        sta $df21
        sta $df22
        lda #$80             ; current offset
        sta $df10
        lda #$82
        sta $df11
        lda #$84
        sta $df12
        lda #$e0             ; next offset
        sta $df30
        lda #$e2
        sta $df31
        lda #$e4
        sta $df32
        jmp init_10e
    set_10b_a0:
        lda #SAVE_10B_BANK   ; set save game is in high chip
        sta $df00
        sta $df01
        sta $df02
        sta $df20
        sta $df21
        sta $df22
        lda #$a0             ; current offset
        sta $df10
        lda #$a2
        sta $df11
        lda #$a4
        sta $df12
        lda #$80             ; next offset
        sta $df30
        lda #$82
        sta $df31
        lda #$84
        sta $df32

        ; initialise storage save area
    init_10e:
        lda #SAVE_10E_BANK
        jsr EAPISetBank

        ldx #$00
        ldy #$80
        lda #$ff
        jsr _compare_data    ; yx: address (x low), a: data
        bcc set_10f_a0       ; branch if no data in $8000
    set_10f_80:
        lda #SAVE_10E_BANK   ; set save game is in low chip
        sta $df03
        sta $df04
        sta $df23
        sta $df24
        lda #$80             ; current offset
        sta $df13
        lda #$82
        sta $df14
        lda #$e0             ; next offset
        sta $df33
        lda #$e2
        sta $df34
        jmp init_110
    set_10f_a0:
        lda #SAVE_10E_BANK   ; set save game is in low chip
        sta $df03
        sta $df04
        sta $df23
        sta $df24
        lda #$a0             ; current offset
        sta $df13
        lda #$a2
        sta $df14
        lda #$80             ; next offset
        sta $df33
        lda #$82
        sta $df34

        ; initialize camp save area
    init_110:
        lda #SAVE_110_BANK
        jsr EAPISetBank

        ldx #$00
        ldy #$80
        lda #$ff
        jsr _compare_data    ; yx: address (x low), a: data
        bcs set_110_80       ; branch if data in $8000
        jmp set_110_a0
    set_110_80:
        lda #SAVE_110_BANK   ; set save game is in low chip
        sta $df05
        sta $df06
        sta $df07
        sta $df08
        sta $df09
        sta $df0a
        sta $df0b
        sta $df0c
        sta $df25
        sta $df26
        sta $df27
        sta $df28
        sta $df29
        sta $df2a
        sta $df2b
        sta $df2c
        lda #$80             ; current offset
        sta $df15
        lda #$82
        sta $df16
        lda #$84
        sta $df17
        lda #$86
        sta $df18
        lda #$88
        sta $df19
        lda #$8a
        sta $df1a
        lda #$8c
        sta $df1b
        lda #$8e
        sta $df1c
        lda #$e0             ; next offset
        sta $df35
        lda #$e2
        sta $df36
        lda #$e4
        sta $df37
        lda #$e6
        sta $df38
        lda #$e8
        sta $df39
        lda #$ea
        sta $df3a
        lda #$ec
        sta $df3b
        lda #$ee
        sta $df3c
        jmp init_done
    set_110_a0:
        lda #SAVE_110_BANK   ; set save game is in low chip
        sta $df05
        sta $df06
        sta $df07
        sta $df08
        sta $df09
        sta $df0a
        sta $df0b
        sta $df0c
        sta $df25
        sta $df26
        sta $df27
        sta $df28
        sta $df29
        sta $df2a
        sta $df2b
        sta $df2c
        lda #$a0             ; current offset
        sta $df15
        lda #$a2
        sta $df16
        lda #$a4
        sta $df17
        lda #$a6
        sta $df18
        lda #$a8
        sta $df19
        lda #$aa
        sta $df1a
        lda #$ac
        sta $df1b
        lda #$ae
        sta $df1c
        lda #$80             ; next offset
        sta $df35
        lda #$82
        sta $df36
        lda #$84
        sta $df37
        lda #$86
        sta $df38
        lda #$88
        sta $df39
        lda #$8a
        sta $df3a
        lda #$8c
        sta $df3b
        lda #$8e
        sta $df3c

    init_done:
        ; bankout and return
        lda #$00
        jsr EAPISetBank
        lda #$37   ; memory map, read from ef
        sta $01
        lda #EASYFLASH_KILL
        sta EASYFLASH_CONTROL
        pla
        sta $01    ; restore memory map
        cli
        rts



.segment "SECTOR_ROM"

    calculate_next_storage:
        ; A: the sector if (010d, 010f, 0117)
        cmp #$0d   ; 010d: save game last sector
        bne :+
        jmp calculate_next_storage_10d

;    :   lda $46
    :   cmp #$0f   ; 010f: unknown area last sector
        bne :+
        jmp calculate_next_storage_10f

;    :   lda $46
    :   cmp #$17   ; 0117: refugee camp last sector
        bne :+
        jmp calculate_next_storage_117

    :
        rts


    calculate_next_storage_10d:
        lda #$ff
        sta $df40
        lda $df10  ; current offset
        sta $df41
        lda #SAVE_10B_BANK
        sta $df42

        ldx $df10  ; current offset
        ldy $df30  ; next offset
        stx $df30
        sty $df10
        ldx $df11  ; current offset
        ldy $df31  ; next offset
        stx $df31
        sty $df11
        ldx $df12  ; current offset
        ldy $df32  ; next offset
        stx $df32
        sty $df12

        jsr correct_ultimax_addresses 
        rts


    calculate_next_storage_10f:
        lda #$ff   ; delete command
        sta $df40
        lda $df13  ; current offset
        sta $df41
        lda #SAVE_10E_BANK
        sta $df42

        ldx $df13  ; current offset
        ldy $df33  ; next offset
        stx $df33
        sty $df13
        ldx $df14  ; current offset
        ldy $df34  ; next offset
        stx $df34
        sty $df14

        jsr correct_ultimax_addresses 
        rts


    calculate_next_storage_117:
        lda #$ff   ; delete command
        sta $df40
        lda $df15  ; current offset
        sta $df41
        lda #SAVE_110_BANK
        sta $df42

        ldx $df15  ; current offset
        ldy $df35  ; next offset
        stx $df35
        sty $df15
        ldx $df16  ; current offset
        ldy $df36  ; next offset
        stx $df36
        sty $df16
        ldx $df17  ; current offset
        ldy $df37  ; next offset
        stx $df37
        sty $df17
        ldx $df18  ; current offset
        ldy $df38  ; next offset
        stx $df38
        sty $df18
        ldx $df19  ; current offset
        ldy $df39  ; next offset
        stx $df39
        sty $df19
        ldx $df1a  ; current offset
        ldy $df3a  ; next offset
        stx $df3a
        sty $df1a
        ldx $df1b  ; current offset
        ldy $df3b  ; next offset
        stx $df3b
        sty $df1b
        ldx $df1c  ; current offset
        ldy $df3c  ; next offset
        stx $df3c
        sty $df1c

        jsr correct_ultimax_addresses
        rts


    correct_ultimax_addresses:
        ; decrease all offsets in the current offset area if >= $e0
        ldx #$00
    :   lda $df10, x
        cmp #$e0
        bcc :+
        lda $df10, x
        sec
        sbc #$40
        sta $df10, x
    :   inx
        cpx #$0d
        bne :--

        ; increase all offsets in the next offset area if >= $a0 and < $e0
        ldx #$00
    :   lda $df30, x
        cmp #$a0
        bcc :+
        cmp #$e0
        bcs :+
        lda $df30, x
        clc
        adc #$40
        sta $df30, x
    :   inx
        cpx #$0d
        bne :--


        rts


.segment "LOADSECTOR_BODY"

    save_mapping:
        .byte $37
    ef_bank:
        .byte $08
    ef_address_high:
        .byte $80
    bt3_current_disk_index:
        .byte $01            ; 1: character, 2: dungeon a, 3: dungeon b
    bt3_current_disk:
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
        jsr loadsave_bankin
        jsr calculate_sector_source
        bcs loadsave_mode3  ; branch if saving is not possible
        jsr save_sector_data
        inc ef_address_high
        inc $45
        jsr save_sector_data
        dec $45
        jsr finish_easyflash_storage
        jsr loadsave_bankout
        clc
        rts
        
    loadsave_load:  ; load sector
        jsr loadsave_bankin
        jsr calculate_sector_source
        jsr load_sector_data
        inc ef_address_high
        inc $45
        jsr load_sector_data
        dec $45
        jsr loadsave_bankout
        clc
        rts

    loadsave_mode3:  ; mode 3
        sec
        rts                 ; for now return error on mode 3
 

    loadsave_bankin:
        sei
        lda $01
        sta save_mapping
        lda #$37   ; memory map, read from ef
        sta $01
        lda #EASYFLASH_LED | EASYFLASH_16K
        sta EASYFLASH_CONTROL
        rts


    loadsave_bankout:
        lda #$37   ; memory map, read from ef
        sta $01
        lda #EASYFLASH_KILL
        sta EASYFLASH_CONTROL
        lda save_mapping
        sta $01    ; restore memory map
        cli
        rts


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
        ldy $47
        clc
        beq :+
        adc #$08             ; if bit 0 of sector_high is set, add value 8

    :   ldx bt3_current_disk_index     ; bank offset for disk
        clc
        adc bt3_current_disk, x
        sta ef_bank          ; no we see the first bank of the specified save area

        ; correct bank and offset for saveable banks
        lda bt3_current_disk_index  ; if disk != 1, then no save
        cmp #$01
        bne prepare_nosave

        lda $47    ; if sector < 0x0100, then no save
        cmp #$1
        bne prepare_nosave

        sec
        lda $46    ; if sector < 0x010b, then no save
        sbc #$0b
        bmi prepare_nosave

        ; now correct the bank
        tax
        lda $df00, x
        sta ef_bank
        lda $df10, x   ; and the offset
        sta ef_address_high

        lda $42    ; if mode != 2, then no save
        cmp #$02
        bne prepare_nosave

        ; now correct for the next area
        lda $df20, x
        sta ef_bank
        lda $df30, x   ; and the offset
        sta ef_address_high

        clc
        rts

    prepare_nosave:
        ; check for modified sectors ### ??
        sec
        rts


; -- loading ------------------------------------

    load_sector_data:
        ; destination
        lda $44
        sta load_sector_destination_low
        lda $45
        sta load_sector_destination_high

        ; source
        lda ef_address_high
        sta load_sector_source_high

        ; bank in and set bank
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

        ldy #$37   ; memory map: read from ef
        sty $01
        rts
        

; --- saving ------------------------------------

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
        jsr EAPIWriteFlash
        bcs save_error
        inx
        bne :-               ; loop 256 times

        ldy #$37   ; memory map: read from ef
        sty $01
        clc
        rts
    save_error:
        ldy #$37   ; memory map: read from ef
        sty $01
        sec
        rts


    prepare_easyflash_storage:
        ; prepare next area to store
        ; erase if necessary
        ; A contains the low value of the sector
        pha        ; save offset
        lda #$00
        jsr EAPISetBank
        pla        ; restore sector
        jsr calculate_next_storage  ; read only code, a is sector

        ; check for delete
        lda erase_command
        beq :+
        lda erase_bank
        jsr EAPISetBank
        ldy erase_offset
        jsr EAPIEraseSector
        lda #$00
        sta erase_command
    :   rts


    finish_easyflash_storage:
        ; $46 low sector number
        ; important: we assume that only correct sector numbers ask for saving
        lda $46

        cmp #$0d   ; 010d: save game last sector
        bne :+
        ;jsr prepare_easyflash_storage ; next area, a is sector 
        jmp finish_prepare

;    :   lda $46
    :   cmp #$0f   ; 010f: unknown area last sector
        bne :+
        ;jsr prepare_easyflash_storage ; next area, a is sector
        jmp finish_prepare

;    :   lda $46
    :    cmp #$17   ; 0117: refugee camp last sector
        bne finish_return
        ;jsr prepare_easyflash_storage ; next area, a is sector

    finish_prepare:
        jsr prepare_easyflash_storage ; next area, a is sector

    finish_return:
        rts


; --- utility -----------------------------------

    _compare_data:
        ; yx: address (x low)
        ; a: data
        ; return: clc if all bytes are eqal
        stx compare_data_cmp_low
        sty compare_data_cmp_high
        ldx #$00
    compare_data_cmp:
    compare_data_cmp_low = compare_data_cmp + 1
    compare_data_cmp_high = compare_data_cmp + 2
        cmp $8000, x
        beq :+
        sec
        rts
    :   inx
        bne compare_data_cmp
        clc
        rts





; ------------------------------------------------------------------------
; attic

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
;        ldx bt3_current_disk_index  ; offset for disk
;        lda bt3_current_disk, x
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
;        rts


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
;    :   ldx bt3_current_disk_index     ; bank offset for disk
;        clc
;        adc bt3_current_disk, x
;        sta ef_bank
;
;        ;lda #$00
;        ;sta ef_address_low
;
;        rts



;  calculate extended source for save zones
;  find current save area in its area
;  save zone a: low addresses only, save zone b: high addresses only

;  saving of sectors
;  find next save area
;  if in new block erase old area
;  save sector


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

;        ldx bt3_current_disk_index     ; bank offset for disk
;        clc
;        adc bt3_current_disk, x
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
        ; 
;        rts




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
;;         jsr EAPIWriteFlashInc
;        clc  ; 
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


;    prepare_easyflash_storage:
        ; $46 low sector number
        ; important: we assume that only correct sector numbers ask for saving
        ;lda #$01  ; correct bank
        ;jsr easyflash_bankin

        ; correct bank offset
;        clc
;        ldx $46
;        lda $df00, x  ; load prepared bank offset to store data
;        adc ef_bank
;        sta ef_bank

        ; correct ef_address_high
;        lda ef_address_high
;        cmp $a0
;        bcc :+  ; is in low area
;        clc
;        adc #$40
;        sta ef_address_high
;    :   

        ; correct ef_address_high from $a0 to $e0

        ; find next free slot, there will always be a free one
        
        ; erase previous area if we started in a new area
        
        ; must set correctly: 
        ; ef_bankmode
        ; ef_address_high
        ; ef_bank

        ;jmp easyflash_bankout
;        rts
