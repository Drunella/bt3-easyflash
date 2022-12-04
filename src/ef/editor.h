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

#ifndef EDITOR_H
#define EDITOR_H

#include <stdint.h>


typedef struct {
    uint8_t flags;
    uint8_t type;
    uint8_t uses;
} item_t;

typedef struct {
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
    //uint8_t items[36];
    item_t items[12];
    // 0x50
    uint8_t classdata[12];
    uint8_t bardsongs;
    // 0x65
    uint8_t dummy_65[5];
    uint8_t unwithered_stats[5];
    uint8_t dummy_70[2];
    uint16_t gameflags;
    uint8_t dummy_74[12];
} character_info_t;

typedef struct {
    uint8_t type;
    uint8_t selectable;
    character_info_t* content;
} character_entry_t;

typedef struct {
    uint8_t type;
    char text[19];
    uint8_t flags;
} text_entry_t;


char* get_class_name(uint8_t cl);
char* get_race_name(uint8_t rc);
char* get_item_name(uint8_t id);
uint8_t get_item_flags(uint8_t id);

char convert_char_bd3_to_editor(char c);
uint8_t cprint_name(uint8_t x, uint8_t y, char* name);
bool getnumberxy(uint8_t x, uint8_t y, uint8_t len, uint32_t* original);
bool draw_status_confirmation(char* content);

void character_main(character_entry_t* character);



#endif
