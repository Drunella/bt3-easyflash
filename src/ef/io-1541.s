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


.feature c_comments


.export cbm_read_sector
.export cbm_write_sector
.export cbm_device_present
.export cbm_device_get_status
.export cbm_device_last_status
.export cbm_device_last_statuscode
.export cbm_device_clear_status
.export cbm_backup_zeropage
.export cbm_restore_zeropage

; ### fastloader ###
/*
.export cbm_write_memory_data
.export cbm_write_memory_begin
.export cbm_write_memory_end
.export cbm_execute_memory
*/


; -- 1541 block io -----------------------------------------------------------

.segment "DATA"

    blockio_buffer_filename:
        .byte $23  ; '#'

    blockio_u_command:
        .byte $55, "1 2 0 18 01", $0d
    blockio_u_command_end:
    blockio_u1_command_len = blockio_u_command_end - blockio_u_command - 1
    blockio_u2_command_len = blockio_u_command_end - blockio_u_command
    blockio_u_command_track = 7
    blockio_u_command_sector = 10
    blockio_u_command_id = blockio_u_command + 1

    blockio_bp_command:
        .byte $42, $2d, $50, " 2 0"
        ;.byte $0d
    blockio_bp_command_end:
    blockio_bp_command_len = blockio_bp_command_end - blockio_bp_command

    blockio_mw_command:
        .byte $4d, $2d, $57  ; "M-W"
    blockio_mw_command_data:
        .byte $00, $00, $00
    blockio_mw_command_end:
    blockio_mw_command_len = blockio_mw_command_end - blockio_mw_command

    blockio_me_command:
        .byte $4d, $2d, $45  ; "M-E"
    blockio_me_command_data:
        .byte $00, $00
    blockio_me_command_end:
    blockio_me_command_len = blockio_me_command_end - blockio_me_command

    blockio_drivenumber:
        .byte $08

    blockio_status:
        .byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        .byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        .byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        .byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
        .byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
    blockio_status_cursor:
        .byte $00
    blockio_status_code:
        .byte $00

    block_io_backup:
        .byte $00, $00, $00

    block_io_zeropage_backup:
        .byte $00, $00, $00, $00, $00, $00, $00, $00
        .byte $00, $00, $00, $00, $00, $00, $00, $00


.segment "CODE"

    cbm_backup_zeropage:
        ; no parameter
        ldx #$0f
    :   lda $a0, x
        sta block_io_zeropage_backup, x
        dex
        bpl :-
        rts


    cbm_restore_zeropage:
        ; parameter: none
        ; return: none
        ldx #$0f
    :   lda block_io_zeropage_backup, x
        sta $a0, x
        dex
        bpl :-
        rts


    cbm_device_clear_status:
        ; parameter: none
        ; return: none
        lda #$00
        sta blockio_status_cursor
        sta blockio_status
        rts


    device_writestatus:
        ldx blockio_status_cursor
        sta blockio_status, x
        inx
        lda #$00
        sta blockio_status, x
        stx blockio_status_cursor
        rts


    cbm_device_last_statuscode:
        ; parameter: none
        ; return: A:status code
        lda blockio_status
        bne :+  ; return 0 if string is empty
        sta blockio_status_code
        rts

     :  lda blockio_status
        sec
        sbc #$30
        tax
        lda #$00
     :  clc
        adc #$0a
        dex
        bne :-
        sta blockio_status_code
        
        lda blockio_status + 1
        sec
        sbc #$30
        clc
        adc blockio_status_code
        sta blockio_status_code
        rts
        

    cbm_device_last_status:
        ; parameter: none
        ; return: A/X (low/high) pointer to status text
        lda #<blockio_status
        ldx #>blockio_status
        rts


    cbm_device_get_status:
        ; parameter: A:device number
        pha
        jsr cbm_device_clear_status

        lda #$00      ; no filename
        ldx #$00
        ldy #$00
        jsr $ffbd     ; call SETNAM

        pla
        tax           ; device number
        lda #$0f      ; file number 15
        ldy #$0f      ; secondary address 15 (error channel)
        jsr $ffba     ; call SETLFS

        jsr $ffc0     ; call OPEN
        bcs :+++      ; if carry set, the file could not be opened

        ldx #$0f      ; filenumber 15
        jsr $ffc6     ; call CHKIN (file 15 now used as input)

    :   jsr $ffb7     ; call READST
        bne :+
        jsr $ffcf     ; call CHRIN
        jsr device_writestatus
        jmp :-
    :   jsr cbm_device_last_statuscode

    :   lda #$0f      ; filenumber 15
        jsr $ffc3     ; call CLOSE
        jsr $ffcc     ; call CLRCHN
        rts


    cbm_device_present:
        ; parameter: A:device number
        ; return: A:0=device present, 1=device not present
        tax
        lda $ba
        pha
        txa
        sta $ba

        jsr cbm_device_clear_status
        lda #$00
        sta $90       ; clear STATUS flags

        lda $ba       ; device number
        txa
        jsr $ffb1     ; call LISTEN
        lda #$6f      ; secondary address 15 (command channel)
        jsr $ff93     ; call SECLSN (SECOND)
        jsr $ffae     ; call UNLSN
        lda $90       ; get STATUS flags
        bne device_present_np  ; device not present

        lda $ba       ; device number
        jsr $ffb4     ; call TALK
        lda #$6f      ; secondary address 15 (error channel)
        jsr $ff96     ; call SECTLK (TKSA)

    :   lda $90       ; get STATUS flags
        bne :+        ; either EOF or error
        jsr $ffa5     ; call IECIN (get byte from IEC bus)
        jsr device_writestatus
        jmp :-        ; next byte
    :   jsr $ffab     ; call UNTLK
        pla
        sta $ba
        lda #$00
        rts

    device_present_np:
        pla
        sta $ba
        lda #$01
        rts


    write_u_command:
        ; x: offset in string, a: value
        stx write_u_command_decimal_value
        tax
        beq write_u_command_load
        sed
        lda #$00
        clc
    :   adc #$01
        dex
        bne :-
        cld
    write_u_command_load:
    write_u_command_decimal_value = write_u_command_load + 1
        ldx #$00
        pha
        lsr a
        lsr a
        lsr a
        lsr a
        ora #$30  ; digit '0'
        sta blockio_u_command, x
        pla
        and #$0f
        ora #$30  ; digit '0'
        inx
        sta blockio_u_command, x
        rts


    cbm_read_sector:
        ; protect zeropage outside
        ; parameter: 
        ;   $ab: track number
        ;   $ac: sector number
        ;   $ad: device number
        ;   $ae: low address of destination
        ;   $af: high address of destination
        ; return:
        ;   A: 0=success, x=error

        lda $ac   ; sector
        ldx #blockio_u_command_sector
        jsr write_u_command
        lda $ab   ; track
        ldx #blockio_u_command_track
        jsr write_u_command
        lda $ad   ; device
        sta blockio_drivenumber
        
        jsr cbm_device_clear_status
        lda #$31  ; U1 command
        sta blockio_u_command_id

        lda #$01
        ldx #<blockio_buffer_filename
        ldy #>blockio_buffer_filename
        jsr $ffbd     ; call SETNAM

        lda #$02      ; file number 2
        ldx blockio_drivenumber
        ldy #$02      ; secondary address 2
        jsr $ffba     ; call SETLFS

        jsr $ffc0     ; call OPEN
        bcs read_sector_close_fixerr  ; if carry set, the file could not be opened

        ; open the command channel
        lda #blockio_u1_command_len
        ldx #<blockio_u_command
        ldy #>blockio_u_command
        jsr $ffbd     ; call SETNAM
        lda #$0f      ; file number 15
        ldx blockio_drivenumber
        ldy #$0f      ; secondary address 15
        jsr $ffba     ; call SETLFS

        jsr $ffc0     ; call OPEN (open command channel and send U1 command)
        bcs read_sector_close_fixerr

        ; check drive error channel here to test for
        ldx #$0f
        jsr $ffc6     ; call CHKIN (file 15 now used as input)
    :   jsr $ffb7     ; call READST
        bne :+
        jsr $ffcf     ; call CHRIN
        jsr device_writestatus
        jmp :-
    :   jsr cbm_device_last_statuscode
        bne read_sector_close_error

        ; read data
        ldx #$02      ; filenumber 2
        jsr $ffc6     ; call CHKIN (file 2 now used as input)
        bcs read_sector_close_fixerr

        ldy #$00
    :   jsr $ffb7     ; call READST
        bne read_sector_close_fixerr
        jsr $ffcf     ; call CHRIN (get a byte from file)
        bcs read_sector_close_fixerr
        sta ($ae), y  ; write byte to memory
        iny
        bne :-        ; next byte, end when 256 bytes are read
        lda #$00
        pha

    read_sector_close:
        lda #$0f      ; filenumber 15
        jsr $ffc3     ; call CLOSE

        lda #$02      ; filenumber 2
        jsr $ffc3     ; call CLOSE

        jsr $ffcc     ; call CLRCHN

        pla
        rts

    read_sector_close_error:
        pha
        jmp read_sector_close

    read_sector_close_fixerr:
        lda $ff
        pha
        jmp read_sector_close


    cbm_write_sector:
        ; protect zeropage outside
        ; parameter:
        ;   $ab: track number
        ;   $ac: sector number
        ;   $ad: device number
        ;   $ae: low address of source
        ;   $af: high address of source
        ; return:
        ;   A: 0=success, x=error

        lda $ac
        ldx #blockio_u_command_sector
        jsr write_u_command
        lda $ab   ; track
        ldx #blockio_u_command_track
        jsr write_u_command
        lda $ad  ; device
        sta blockio_drivenumber
        jsr cbm_device_clear_status
        lda #$32  ; U2 command
        sta blockio_u_command_id

        ; open the drive memory channel
        lda #$01
        ldx #<blockio_buffer_filename
        ldy #>blockio_buffer_filename
        jsr $ffbd     ; call SETNAM

        lda #$02      ; file number 2
        ldx blockio_drivenumber
        ldy #$02      ; secondary address 2
        jsr $ffba     ; call SETLFS

        jsr $ffc0     ; call OPEN
        bcs write_sector_ext_fixerr

        ; open the command channel
        lda #blockio_bp_command_len
        ldx #<blockio_bp_command
        ldy #>blockio_bp_command
        jsr $ffbd     ; call SETNAM
        lda #$0f      ; file number 15
        ldx blockio_drivenumber
        ldy #$0f      ; secondary address 15
        jsr $ffba     ; call SETLFS

        jsr $ffc0     ; call OPEN (open command channel and send B-P command)
        bcs write_sector_ext_fixerr

        ldx #$02      ; filenumber 2
        jsr $ffc9     ; call CHKOUT (file 2 now used as output)
        bcs write_sector_ext_fixerr

        ldy #$00
    :   lda ($ae), y  ; read byte from memory
        jsr $ffd2     ; call CHROUT (write byte to channel buffer)
        iny
        bne :-        ; next byte, end when 256 bytes are read

        ldx #$0f      ; filenumber 15
        jsr $ffc9     ; call CHKOUT (file 15 now used as output)
        bcs write_sector_ext_fixerr

        ldy #$00
    :   lda blockio_u_command, y  ; read byte from command string
        jsr $ffd2     ; call CHROUT (write byte to command channel)
        iny
        cpy #blockio_u2_command_len
        bne :-

        ; check drive error channel here to test for
        jsr $ffcc     ; call CLRCHN
        ldx #$0f
        jsr $ffc6     ; call CHKIN (file 15 now used as input)
    :   jsr $ffb7     ; call READST
        bne :+
        jsr $ffcf     ; call CHRIN
        jsr device_writestatus
        jmp :-
    :   jsr cbm_device_last_statuscode
        bne write_sector_ext_error

        lda #$00
        pha

    write_sector_ext_close:
        lda #$0f      ; filenumber 15
        jsr $ffc3     ; call CLOSE

        lda #$02      ; filenumber 2
        jsr $ffc3     ; call CLOSE

        jsr $ffcc     ; call CLRCHN

        pla
        rts

    write_sector_ext_error:
        pha
        jmp write_sector_ext_close

    write_sector_ext_fixerr:
        lda $ff
        pha
        jmp write_sector_ext_close


; ------------------------------------------------------------------------
; ### fastloader ###
/*
.segment "CODE"

    cbm_write_memory_begin:
        ; protect zeropage outside
        ; parameter:
        ;   $ad: device number
        ; return:
        ;   A: 0=success, x=error
        lda #$00
        ldy #$00
        ldy #$00
        jsr $ffbd     ; call SETNAM (no filename)

        lda #$0f      ; file number 15
        ldx $ad
        stx blockio_drivenumber
        ldy #$0f      ; secondary address 15
        jsr $ffba     ; call SETLFS

        jsr $ffc0     ; call OPEN
        bcs :+        ; if carry set, the file could not be opened

;        ldx #$0f      ; filenumber 15
;        jsr $ffc9     ; call CHKOUT (file 15 now used as output)

        lda #$00
        rts

    :   bne :+        ; if a has value, accept as error code
        lda #$ff      ; error
    :   rts


    cbm_write_memory_end:
        lda #$0f      ; filenumber 15
        jsr $ffc3     ; call CLOSE

        jsr $ffcc     ; call CLRCHN

        rts


    cbm_write_memory_data:
        ; protect zeropage outside
        ; parameter:
        ;   $aa: size of bytes, max 32
        ;   $ab: low address in drive
        ;   $ac: high address in drive
        ;   $ae: low address of source
        ;   $af: high address of source
        ; return:
        ;   A: 0=success, x=error
        ldx #$0f      ; filenumber 15
        jsr $ffc9     ; call CHKOUT (file 15 now used as output)

        lda $ab
        sta blockio_mw_command_data      ; low address
        lda $ac
        sta blockio_mw_command_data + 1  ; high address
        lda $aa
        sta blockio_mw_command_data + 2  ; size
        lda $ae
        sta cbm_write_memory_data_source_low
        lda $af
        sta cbm_write_memory_data_source_high

        ; send command
        ldy #$00
    :   lda blockio_mw_command, y  ; get byte from command string
        jsr $ffd2     ; call CHROUT (send byte through command channel)
        iny
        cpy #blockio_mw_command_len
        bne :-

        ; send data
        ldy #$00
    cbm_write_memory_data_source:
    cbm_write_memory_data_source_low = cbm_write_memory_data_source + 1
    cbm_write_memory_data_source_high = cbm_write_memory_data_source + 2
        lda $ffff, y  ; get byte from data source
        jsr $ffd2     ; call CHROUT
        iny
        cpy $aa
        bne cbm_write_memory_data_source

        jsr $ffcc     ; call CLRCHN

        rts


    cbm_execute_memory:
        ; protect zeropage outside
        ; parameter:
        ;   $ad: device number
        ;   $ae: low address
        ;   $af: high address
        ; return:
        ;   A: 0=success, x=error
        lda $ae
        sta blockio_me_command_data
        lda $af
        sta blockio_me_command_data + 1

        lda #blockio_me_command_len
        ldx #<blockio_me_command
        ldy #>blockio_me_command
        jsr $ffbd     ; call SETNAM

        lda #$0f      ; file number 15
        ldx $ad       ; device
        ldy #$0f      ; secondary address 15
        jsr $ffba     ; call SETLFS

        jsr $ffc0     ; call OPEN
        jsr $ffe7

        lda #$00
        rts
*/


; -- attic ---------------------------------------------------------------
/*

    _write_cbm_sector_open:
        ; uint8_t __fastcall__ write_cbm_sector_open(uint8_t device);
        ; device in a
        sta blockio_drivenumber

        jsr _device_clear_status
        lda #$32  ; U2 command
        sta blockio_u_command_id

        ; open the drive memory channel
        lda #$01
        ldx #<blockio_buffer_filename
        ldy #>blockio_buffer_filename
        jsr $ffbd     ; call SETNAM

        lda #$02      ; file number 2
        ldx blockio_drivenumber
        ldy #$02      ; secondary address 2
        jsr $ffba     ; call SETLFS

        jsr $ffc0     ; call OPEN
        bcs write_sector_open_fixerr

        ; open the command channel
        lda #blockio_bp_command_len
        ldx #<blockio_bp_command
        ldy #>blockio_bp_command
        jsr $ffbd     ; call SETNAM
        lda #$0f      ; file number 15
        ldx blockio_drivenumber
        ldy #$0f      ; secondary address 15
        jsr $ffba     ; call SETLFS

        jsr $ffc0     ; call OPEN (open command channel and send B-P command)
        bcs write_sector_open_fixerr

        ; check drive error channel here to test for
        ldx #$0f
        jsr $ffc6     ; call CHKIN (file 15 now used as input)
    :   jsr $ffb7     ; call READST
        bne :+
        jsr $ffcf     ; call CHRIN
        jsr device_writestatus
        jmp :-
    :   jsr _device_last_statuscode
        bne write_sector_open_error

;        ldx #$02      ; filenumber 2
;        jsr $ffc9     ; call CHKOUT (file 2 now used as output)
;        bcs write_sector_open_fixerr

;        ldx #$0f      ; filenumber 15
;        jsr $ffc9     ; call CHKOUT (file 15 now used as output)
;        bcs write_sector_open_fixerr

        lda #$00      ; success
        rts

    write_sector_open_error:
        pha
        jsr _write_cbm_sector_close
        pla
        rts

    write_sector_open_fixerr:
        jsr _write_cbm_sector_close
        lda #$00
        rts


    _write_cbm_sector_close:
        ; void __fastcall__ write_cbm_sector_close();
        jsr $ffcc     ; call CLRCHN

        lda #$0f      ; filenumber 15
        jsr $ffc3     ; call CLOSE

        lda #$02      ; filenumber 2
        jsr $ffc3     ; call CLOSE

        rts


    _write_cbm_sector_data:
        ; uint8_t __fastcall__ write_cbm_sector(char* source, uint8_t track, uint8_t sector);
        tax ; sector in a
        lda $ae
        pha
        lda $af
        pha

        txa       ; sector
        ldx #blockio_u_command_sector
        jsr write_u_command
        jsr popa  ; track
        ldx #blockio_u_command_track
        jsr write_u_command
        jsr popa  ; address low
        sta $ae
        jsr popa  ; address high
        sta $af

        jsr _device_clear_status

        ldx #$02      ; filenumber 2
        jsr $ffc9     ; call CHKOUT (file 2 now used as output)
        bcs write_sector_data_fixerr

        ldx #$0f      ; filenumber 15
        jsr $ffc9     ; call CHKOUT (file 15 now used as output)
        bcs write_sector_data_fixerr

        ldy #$00
    :   lda ($ae), y  ; read byte from memory
        jsr $ffd2     ; call CHROUT (write byte to channel buffer)
        iny
        bne :-        ; next byte, end when 256 bytes are read

        ldy #$00
    :   lda blockio_u_command, y  ; read byte from command string
        jsr $ffd2     ; call CHROUT (write byte to command channel)
        iny
        cpy #blockio_u2_command_len
        bne :-

        ; check drive error channel here to test for
        jsr $ffcc     ; call CLRCHN
        ldx #$0f
        jsr $ffc6     ; call CHKIN (file 15 now used as input)
    :   jsr $ffb7     ; call READST
        bne :+
        jsr $ffcf     ; call CHRIN
        jsr device_writestatus
        jmp :-
    :   jsr _device_last_statuscode
        bne write_sector_data_error
        ldx #$00
        ;pha

    write_sector_data_finish:
        ;pla
        ;tax
        pla
        sta $af
        pla
        sta $ae
        txa
        rts

    write_sector_data_fixerr:
        jsr _write_cbm_sector_close
        ldx $ff
        ;pha
        jmp write_sector_data_finish

    write_sector_data_error:
        pha
        jsr _write_cbm_sector_close
        pla
        tax
        jmp write_sector_data_finish


*/
