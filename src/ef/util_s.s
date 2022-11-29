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

.export _read_cbm_sector
.export _write_cbm_sector
.export _device_present
.export _device_last_status
.export _device_last_statuscode



; -- easyflash sector io -----------------------------------------------------

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


.segment "CODE"

    device_resetstatus:
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


    _device_last_statuscode:
        ; uint8_t __fastcall__ _device_last_statuscode();
        ; a: status code        
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
        

    _device_last_status:
        ; char* __fastcall__ _device_last_status();
        lda #<blockio_status
        ldx #>blockio_status
        rts


    _device_present:
        ; uint8_t __fastcall__ device_present(uint8_t device);
        tax
        lda $ba
        pha
        txa
        sta $ba

        jsr device_resetstatus
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


    _read_cbm_sector:
        ; uint8_t __fastcall__ read_cbm_sector(char* dest, uint8_t device, uint8_t track, uint8_t sector);
        ; sector in a
        ldx #blockio_u_command_sector
        jsr write_u_command
        jsr popa  ; track
        ldx #blockio_u_command_track
        jsr write_u_command
        jsr popa  ; device
        sta blockio_drivenumber
        jsr popa  ; address low
        sta $ae
        jsr popa  ; address high
        sta $af
        jsr device_resetstatus
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
    :   jsr _device_last_statuscode
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


    _write_cbm_sector:
        ; uint8_t __fastcall__ write_cbm_sector(char* source, uint8_t device, uint8_t track, uint8_t sector);
        ; sector in a
        ldx #blockio_u_command_sector
        jsr write_u_command
        jsr popa  ; track
        ldx #blockio_u_command_track
        jsr write_u_command
        jsr popa  ; device
        sta blockio_drivenumber
        jsr popa  ; address low
        sta $ae
        jsr popa  ; address high
        sta $af
        jsr device_resetstatus
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
        bcs write_sector_close_fixerr

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
        bcs write_sector_close_fixerr

        ; check drive error channel here to test for
        ldx #$0f
        jsr $ffc6     ; call CHKIN (file 15 now used as input)
    :   jsr $ffb7     ; call READST
        bne :+
        jsr $ffcf     ; call CHRIN
        jsr device_writestatus
        jmp :-
    :   jsr _device_last_statuscode
        bne write_sector_close_error

        ldx #$02      ; filenumber 2
        jsr $ffc9     ; call CHKOUT (file 2 now used as output)
        bcs write_sector_close_fixerr

        ldy #$00
    :   jsr $ffb7     ; call READST
        bne write_sector_close_fixerr
        lda ($ae), y  ; read byte from memory
        jsr $ffd2     ; call CHROUT (write byte to channel buffer)
        iny
        bne :-        ; next byte, end when 256 bytes are read

        ldx #$0f      ; filenumber 15
        jsr $ffc9     ; call CHKOUT (file 15 now used as output)
        bcs write_sector_close_fixerr

        ldy #$00
    :   jsr $ffb7     ; call READST
        bne write_sector_close_fixerr
        lda blockio_u2_command_len, y  ; read byte from command string
        jsr $ffd2     ; call CHROUT (write byte to command channel)
        iny
        cpy #blockio_u2_command_len
        bne :-

        pla
        rts

    write_sector_close:
        lda #$0f      ; filenumber 15
        jsr $ffc3     ; call CLOSE

        lda #$02      ; filenumber 2
        jsr $ffc3     ; call CLOSE

        jsr $ffcc     ; call CLRCHN

        pla
        rts

    write_sector_close_error:
        pha
        jmp write_sector_close

    write_sector_close_fixerr:
        lda $ff
        pha
        jmp write_sector_close
