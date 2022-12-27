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


.include "easyflash.i"

.import sp, popax, popa

.export _startup_game
.export _startup_editor
.export _startup_import

.export _set_ef_diskid
.export _read_ef_sector
.export _write_ef_sector 

.export _load_ef_file
.export _load_ef_file_ext

.export _read_cbm_sector
.export _write_cbm_sector_ext
;.export _write_cbm_sector_open
;.export _write_cbm_sector_data
;.export _write_cbm_sector_close
.export _device_present
.export _device_last_status
.export _device_last_statuscode
.export _device_clear_status

.export _write_cbm_memory_begin
.export _write_cbm_memory_end
.export _write_cbm_memory_data
.export _execute_cbm_memory
.export _kill_cbm_fastload
.export _write_cbm_sector_fastload

.import cbm_read_sector
.import cbm_write_sector
.import cbm_device_present
.import cbm_device_last_status
.import cbm_device_last_statuscode
.import cbm_device_clear_status
.import cbm_backup_zeropage
.import cbm_restore_zeropage
.import cbm_write_memory_data
.import cbm_write_memory_begin
.import cbm_write_memory_end
.import cbm_execute_memory

.import send_iec_byte_fastload
.import init_iec_fastload
.import recv_iec_byte_fastload
.import loadsave_cbm_sector_fastload
.import calc_return_value
.import calc_track_number
.import calc_sector_number
.import modetracksector_iec_fastload


; -- easyflash startup -------------------------------------------------------

.segment "CODE"

    _startup_game:
        jmp jt_startup_game

    _startup_editor:
        jmp jt_startup_editor

    _startup_startmenu:
        jmp jt_startup_startmenu

    _startup_import:
        jmp jt_startup_import


; -- easyflash sector io -----------------------------------------------------

.segment "CODE"

    ; void __fastcall__ set_ef_diskid(uint8_t diskid)
    _set_ef_diskid:
        ; diskid in a
        jmp io_set_current_disk_addr


    ; uint8_t __fastcall__ read_ef_sector(uint16_t sector, char* destination)
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
        ;jsr io_set_current_disk_addr
    
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


    ; uint8_t __fastcall__ write_ef_sector(uint16_t sector, char* source)
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
        ;lda #$01
        ;jsr io_set_current_disk_addr
    
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



; -- easyflash load file -----------------------------------------------------

.segment "CODE"

    _load_ef_file:
        ; uint8_t __fastcall__ load_ef_file(uint8_t fileid);
        tax
        lda $ff
        pha
        lda $fe
        pha
        lda $fd
        pha
        lda $fc
        pha
        lda $fb
        pha
        txa
        jsr io_load_file_addr
        tax
        pla
        sta $fb
        pla
        sta $fc
        pla
        sta $fd
        pla
        sta $fe
        pla
        sta $ff
        txa
        ldx #$00
        rts


    _load_ef_file_ext:
        ; uint8_t __fastcall__ load_ef_file_ext(char* destination, uint8_t fileid)
        tax
        lda $ff
        pha
        lda $fe
        pha
        lda $fd
        pha
        lda $fc
        pha
        lda $fb
        pha
        txa
        pha
        jsr popax  ; address
        sta $fc
        stx $fd
        pla
        jsr io_load_file_addr
        lda $fb
        tax
        pla
        sta $fb
        pla
        sta $fc
        pla
        sta $fd
        pla
        sta $fe
        pla
        sta $ff
        txa
        ldx #$00
        rts



; -- 1541 block io -----------------------------------------------------------

; protect zeropage here
; parameter:
;   $ab: track number
;   $ac: sector number
;   $ad: device number
;   $ae: low address of source/destination
;   $af: high address of source/destination
; return:
;   A: 0=success, x=error

.segment "CODE"

    _device_clear_status:
        ; void __fastcall__ device_clear_status();
        jmp cbm_device_clear_status


    _device_last_statuscode:
        ; uint8_t __fastcall__ _device_last_statuscode();
        ; a: status code
        jmp cbm_device_last_statuscode


    _device_last_status:
        ; char* __fastcall__ _device_last_status();
        jmp cbm_device_last_status


    _device_present:
        ; uint8_t __fastcall__ device_present(uint8_t device);
        jmp cbm_device_present


    _read_cbm_sector:
        ; uint8_t __fastcall__ read_cbm_sector(char* dest, uint8_t device, uint8_t track, uint8_t sector);
        pha
        jsr cbm_backup_zeropage
        pla
        sta $ac   ; sector in a
        jsr popa  ; track
        sta $ab
        jsr popa  ; device
        sta $ad
        jsr popa  ; address low
        sta $ae
        jsr popa  ; address high
        sta $af

        jsr cbm_read_sector

        pha 
        jsr cbm_restore_zeropage
        pla
        ldx #$00
        rts


    _write_cbm_sector_ext:
        ; uint8_t __fastcall__ write_cbm_sector(char* source, uint8_t device, uint8_t track, uint8_t sector);
        pha
        jsr cbm_backup_zeropage
        pla
        sta $ac   ; sector in a
        jsr popa  ; track
        sta $ab
        jsr popa  ; device
        sta $ad
        jsr popa  ; address low
        sta $ae
        jsr popa  ; address high
        sta $af

        jsr cbm_write_sector

        pha
        jsr cbm_restore_zeropage
        pla
        ldx #$00
        rts


    _write_cbm_memory_begin:
        ; uint8_t __fastcall__ _write_cbm_memory_begin(uint8_t device);
        pha
        jsr cbm_backup_zeropage
        pla

        sta $ad  ; device number in a
        jsr cbm_write_memory_begin

        pha
        jsr cbm_restore_zeropage
        pla
        ldx #$00
        rts


    _write_cbm_memory_end:
        ; void __fastcall__ _write_cbm_memory_end();
        jmp cbm_write_memory_end
        

    _write_cbm_memory_data:
        ; uint8_t __fastcall__ write_cbm_memory_data(char* source, char*, destination, uint8_t size);
        pha
        jsr cbm_backup_zeropage
        pla
        sta $aa  ; size in a
        jsr popax  ; dest address
        sta $ab
        stx $ac
        jsr popax  ; source address
        sta $ae
        stx $af

        jsr cbm_write_memory_data

        pha
        jsr cbm_restore_zeropage
        pla
        ldx #$00
        rts


    _execute_cbm_memory:
        ; uint8_t __fastcall__ execute_cbm_memory(char* address, uint8_t device);
        ; address in a/x
        pha
        jsr cbm_backup_zeropage
        pla
        sta $ad    ; device
        jsr popax  ; address
        sta $ae
        stx $af

        jsr cbm_execute_memory

        pha
        jsr cbm_restore_zeropage
        pla
        ldx #$00
        rts



; -- 1541 fastloader -----------------------------------------------------------

.segment "CODE"

    _kill_cbm_fastload:
        ; void __fastcall__ kill_cbm_fastload();
        lda $01    ; save memory configuration
        pha
        lda $d015  ; save sprite enable register
        pha
        lda #$00
        sta $d015

        sta calc_sector_number
        lda #18
        sta calc_track_number        
        lda #$04  ; additional mode: kill fastloader
        sta $42

        jsr init_iec_fastload
        jsr modetracksector_iec_fastload

;        lda #$c7   ; reset bus to default
;        sta $dd00

        pla
        sta $d015
        pla
        sta $01
        rts


    dummy_vector:
        rti

    disable_interrupts:
        sei
        ; set dummy interrupt
        ldx #<dummy_vector
        ldy #>dummy_vector
        stx $fffa
        sty $fffb
        stx $fffe
        sty $ffff
        lda #$7f   ; disable interrupts (?)
        sta $dc0d
        sta $dd0d
        lda $dc0d
        sta $dd0d
        cli
        rts

    enable_interrupts:
        sei
        lda #$81   ; start timer for interrupts
        sta $dc0d
        cli
        rts


    _write_cbm_sector_fastload:
        ; uint8_t __fastcall__ write_cbm_sector_fastload(char* source, uint8_t track, uint8_t sector)
        sta calc_sector_number  ; sector in a
        jsr popa  ; track (a)
        sta calc_track_number
        jsr popax  ; address low(a), high(x)
        sta $44
        stx $45
       
        lda #$02  ; mode 2
        sta $42

        jsr disable_interrupts

        jsr loadsave_cbm_sector_fastload
        bcs :+

        jsr enable_interrupts
        lda calc_return_value  ; return value
        ldx #$00
        rts

    :   jsr enable_interrupts
        lda #$ff  ; generic error
        ldx #$00
        rts


/*

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

    block_io_backup:
        .byte $00, $00, $00


.segment "CODE"

    _device_clear_status:
        ; void __fastcall__ device_clear_status();
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

        jsr _device_clear_status
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
        jsr _device_clear_status
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


    _write_cbm_sector_ext:
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

        ; check drive error channel here to test for
;        ldx #$0f
;        jsr $ffc6     ; call CHKIN (file 15 now used as input)
;    :   jsr $ffb7     ; call READST
;        bne :+
;        jsr $ffcf     ; call CHRIN
;        jsr device_writestatus
;        jmp :-
;    :   jsr _device_last_statuscode
;        bne write_sector_ext_error

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
    :   jsr _device_last_statuscode
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

*/