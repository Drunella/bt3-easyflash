// ----------------------------------------------------------------------------
// Copyright 2023 Drunella
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------------------------------------------------------

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>


#define SAVE_ADDRESS 0xa000
#define TEMP_ADDRESS 0x1800

// startup addresses, also in easyflash.i
#define JT_STARTUP_STARTMENU  0x1800
#define JT_STARTUP_EDITOR  0x1803
#define JT_STARTUP_GAME  0x1806


uint8_t get_version_major();
uint8_t get_version_minor();

    
void cart_kill(void);
void cart_bankin(void);
void cart_bankout(void);
void cart_reset(void);

void startup_game_ext();
void startup_editor_ext();
void startup_startmenu_ext();
void __fastcall__ startup_game();
void __fastcall__ startup_editor();
void __fastcall__ startup_startmenu();

void __fastcall__ set_ef_diskid(uint8_t diskid);
uint8_t __fastcall__ read_ef_sector(uint16_t sector, char* destination);
uint8_t __fastcall__ write_ef_sector(uint16_t sector, char* destination);

void __fastcall__ load_ef_file(uint8_t fileid);

uint8_t __fastcall__ read_cbm_sector(char* dest, uint8_t device, uint8_t track, uint8_t sector);
uint8_t __fastcall__ write_cbm_sector_open(uint8_t device);
uint8_t __fastcall__ write_cbm_sector_data(char* source, uint8_t track, uint8_t sector);
void __fastcall__ write_cbm_sector_close();
uint8_t __fastcall__ write_cbm_sector_ext(char* source, uint8_t device, uint8_t track, uint8_t sector);
uint8_t __fastcall__ device_present(uint8_t device);
char* __fastcall__ device_last_status();
uint8_t __fastcall__ device_last_statuscode();
void __fastcall__ device_clear_status();

void load_sectors();
uint8_t write_sectors_save();
uint8_t write_sectors_storage();
uint8_t write_sectors_camp();


void menu_clear(uint8_t start, uint8_t stop);
void menu_option(char key, char *desc);


#endif
