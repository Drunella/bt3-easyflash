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

; easyflash bard's tale iii parameter

EAPI_SOURCE  = $b800  ; $a000 (hirom) + 1800
EAPI_DESTINATION = $c000

FILES_DIR_BANK = 0
FILES_DIR_START = $bc00
FILES_DATA_BANK = 1
FILES_DATA_START = $8000
FILES_DIR_ENTRY = $10

CHARACTER_DISK_BANK = 13
DUNGEONA_DISK_BANK = 22
DUNGEONB_DISK_BANK = 31

; as this is the bank offset on the disk
SAVE_10B_BANK = 40
SAVE_10E_BANK = 48
SAVE_110_BANK = 56
SAVE_SECTOR_OFFSET = 8


; io function
io_loadsave_sector_addr = $be7a
io_set_current_disk_addr = $be38
io_load_file_addr = $be02


; startup functions
jt_startup_startmenu = $1800
jt_startup_editor = $1803
jt_startup_game = $1806


; eapi functions

EASYFLASH_BANK    = $de00
EASYFLASH_CONTROL = $de02
EASYFLASH_LED     = $80
EASYFLASH_16K     = $07
EASYFLASH_KILL    = $04

EAPIInit          = EAPI_DESTINATION + $14
EAPIWriteFlash    = $df80
EAPIEraseSector   = $df83
EAPISetBank       = $df86
EAPIGetBank       = $df89
EAPISetPtr        = $df8c
EAPISetLen        = $df8f
EAPIReadFlashInc  = $df92
EAPIWriteFlashInc = $df95
EAPISetSlot       = $df98
EAPIGetSlot       = $df9b


; data struct

.struct data_directory
    id .byte
    bank .byte
    offset_low .byte
    offset_high .byte
    size_low .byte
    size_high .byte
    size_upper .byte
    load_low .byte
    load_high .byte
    init_low .byte
    init_high .byte
    reserved1 .byte 
    reserved2 .byte 
    reserved3 .byte 
    reserved4 .byte 
    reserved5 .byte 
.endstruct
        