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



struct character_info {
    unsigned char name[16];
    // 0x10
    uint8_t strength, intelligence, dexterity, constitution, luck;
    uint32_t experience;
    uint32_t gold;
    uint16_t level;
    // 0x1f
    uint16_t undrained_level;
    uint16_t current_hitpoints, maximum_hitpoints;
    uint16_t current_magepoints, maximum_magepoints;
    uint8_t playerclass;
    uint8_t playerrace;
    uint8_t dummy_2b;
    uint8_t dummy_2c;
    uint8_t status;
    uint8_t armorclass; // always recalculated
    // 0x30
    uint8_t items[36];
    // 0x50
    uint8_t classdata[12];
    uint8_t bardsongs;
    // 0x65
    uint8_t dummy_65[5];
    uint8_t unwithered_stats[5];
    uint8_t dummy_70[2];
    uint16_t gameflags;
    uint8_t dummy_74[12];
};


void cart_kill(void);
void cart_bankin(void);
void cart_bankout(void);
void cart_reset(void);


void __fastcall__ load_eapi(uint8_t high);

uint8_t __fastcall__ read_ef_sector(uint16_t sector, char* destination);
uint8_t __fastcall__ write_ef_sector(uint16_t sector, char* destination);


#endif
