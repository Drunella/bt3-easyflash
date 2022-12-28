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


#define SAVEGAME_ADDR ((char*)(0xa000))


typedef struct {
    uint8_t flags;
    uint8_t type;
    uint8_t uses;
} item_t;

typedef struct {
    unsigned char name[16];  // 16 bytes
    uint8_t strength, intelligence, dexterity, constitution, luck; // 5 bytes
    uint32_t experience, gold; // 8 bytes
    uint16_t level, undrained_level; // 4 bytes
    uint16_t current_hitpoints, maximum_hitpoints; // 4 bytes
    uint16_t current_spellpoints, maximum_spellpoints; // 4 bytes
    uint8_t playerclass, playerrace; // 2 bytes
    uint8_t dummy_2b, dummy_2c; // 2 bytes
    uint8_t status, armorclass; // 2 bytes
    uint8_t dummy_2f; // 1 byte
    // 0x30 (48)
    item_t items[12];  // 36 bytes
    // 0x54 (84)
    uint8_t spells[16];
    // 0x64 (100)
    uint8_t classdata[3]; // bard, rogue
    uint8_t dummy_65[3];
    // 0x6a (106)
    uint8_t unwithered_stats[5];
    uint8_t dummy_70[3];
    // 0x72 (115)
    uint8_t gameflags[2];
    // 0x75 (117)
    //uint8_t dummy_74[11];
} character_info_t;

typedef struct {
    uint8_t type;
    uint8_t selectable;
    character_info_t* content;
} character_entry_t;

typedef struct {
    uint8_t type;
    char* text;
    uint8_t flags;
} text_entry_t;

typedef struct {
    uint8_t id;
    char* text;
    uint8_t uses;
} item_name_t;


typedef struct {
    char code[5];
    uint8_t offset;
    uint8_t bit;
    uint8_t playerclass;
    uint8_t level;
    char* description;
} spellinfo_t;

/*typedef struct {
    uint8_t bits;
    uint8_t x, y;
    uint8_t length;
} editable_field_t;*/

typedef struct {
    uint8_t restriction;
    uint8_t up;
    uint8_t right;
    uint8_t down;
    uint8_t left;
} transition_t;

char* get_class_name(uint8_t cl);
char* get_race_name(uint8_t rc);
char* get_item_name(uint8_t id);
uint8_t get_item_uses(uint8_t id);
uint8_t get_index_itemid(uint8_t id);
uint8_t get_itemid_by_index(uint8_t index);
uint8_t get_item_count();
uint8_t get_alphabetical_index(uint8_t letter);

spellinfo_t* spells_list(uint8_t clss);
uint8_t spells_amount(uint8_t clss);
uint8_t count_spells(character_info_t* character, uint8_t clss);
bool has_spell(char* spelldata, spellinfo_t* spell);
void toggle_spell(char* spelldata, spellinfo_t* spell);

char convert_char_bt3_to_editor(char c);
uint8_t cprint_name(uint8_t x, uint8_t y, char* name);
bool getunumberxy(uint8_t x, uint8_t y, uint8_t len, uint32_t* original);
bool getsnumberxy(uint8_t x, uint8_t y, uint8_t len, int32_t* original);
bool draw_status_confirmation(char* content);

void character_main(character_entry_t* character);


uint8_t write_sectors_save(char* base);
uint8_t write_sectors_storage(char* base);
uint8_t write_sectors_camp(char* base);
void load_sectors(char* base);


bool process_character_list(char* data, uint8_t type);


#endif
