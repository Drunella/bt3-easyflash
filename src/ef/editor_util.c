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


#include <stdbool.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "util.h"
#include "editor.h"


text_entry_t race_names[7] = {
    { 0, "Human", 0 },
    { 1, "Elf", 0 },
    { 2, "Dwarf", 0 },
    { 3, "Hobbit", 0 },
    { 4, "Half-Elf", 0 },
    { 5, "Half-Orc", 0 },
    { 6, "Gnome", 0 },
};

text_entry_t class_names[13] = {
    { 0, "Warrior", 0 },
    { 1, "Wizard", 0 },
    { 2, "Sorcerer", 0 },
    { 3, "Conjurer", 0 },
    { 4, "Magician", 0 },
    { 5, "Rogue", 0 },
    { 6, "Bard", 0 },
    { 7, "Paladin", 0 },
    { 8, "Hunter", 0 },
    { 9, "Monk", 0 },
    {10, "Archmage", 0 },
    {11, "Chronomancer", 0 },
    {12, "Geomancer", 0 }
};



char* get_race_name(uint8_t rc)
{
    return race_names[rc].text;
}

char* get_class_name(uint8_t cl)
{
    return class_names[cl].text;
}


char convert_char_bd3_to_editor(char c)
{
    c = c & 0x7f;
    if (c >= 0x60) c = c - 0x20;
    else if (c>= 0x40 && c < 0x60) c = c + 0x20;
    return c;
}


uint8_t cprint_name(uint8_t x, uint8_t y, char* name)
{
    uint8_t o, n;
    char c;
    
    n = 0;
    gotoxy(x, y);
    for (o=0; o<15; o++) {
        c = name[o];
        if (c == 0xff) break;
        cputc(convert_char_bd3_to_editor(c));
        n++;
    }
    return n;
}

bool draw_status_confirmation(char* content)
{
    char c;
    uint8_t n;

    cclearxy(0,24,40);
    textcolor(COLOR_GRAY2);
    n = strlen(content);
    cputsxy(1,24, content);
    cputsxy(2+n, 24, "Are you sure? ");
    cursor(1);
    c = cgetc();
    cursor(0);
    cclearxy(0,24,40);
    return (c == 'y');
}


bool getunumberxy(uint8_t x, uint8_t y, uint8_t len, uint32_t* original)
{
    char c;
    uint8_t n;
    bool changed = false;
    char content[16];

    textcolor(COLOR_GRAY2);
    cclearxy(0,24,40);
    //              123456789012345678901234567890123456789
    cputsxy( 1,24, "( )cancel  (     )accept  (   )clear");
    textcolor(COLOR_WHITE);
    cputcxy(2,24, 0x5f);
    cputsxy(13,24, "Enter");
    cputsxy(28,24, "CLR");
    
    n = 0;
    content[0] = 0;

    for (;;) {
        gotoxy(x, y);
        cclearxy(x, y, len);
        cputsxy(x, y, content);

        cursor(1);
        c = cgetc();
        cursor(0);

        if (c == CH_ENTER) {
            // enter
            *original = atol(content);
            changed = true;
            break;
        
        } else if (c == CH_DEL) {
            // del
            if (n > 0) content[n-1] = 0;
            n--;

        } else if (c == CH_HOME || c == 0x93) {
            // clear
            content[0] = 0;
            n = 0;
        
        } else if (c == 0x5f) {
            // cancel
            break;

        } else if (c >= '0' && c <= '9') {
            if (n < len) {
                content[n] = c;
                content[n+1] = 0;
                n++;
            }
        }

    }

    textcolor(COLOR_GRAY2);
    return changed;
}

bool getsnumberxy(uint8_t x, uint8_t y, uint8_t len, int32_t* original)
{
    char c;
    uint8_t n;
    bool changed = false;
    char content[16];

    textcolor(COLOR_GRAY2);
    cclearxy(0,24,40);
    //              123456789012345678901234567890123456789
    cputsxy( 1,24, "( )cancel  (     )accept  (   )clear");
    textcolor(COLOR_WHITE);
    cputcxy(2,24, 0x5f);
    cputsxy(13,24, "Enter");
    cputsxy(28,24, "CLR");
    
    n = 0;
    content[0] = 0;

    for (;;) {
        gotoxy(x, y);
        cclearxy(x, y, len);
        cputsxy(x, y, content);

        cursor(1);
        c = cgetc();
        cursor(0);

        if (c == CH_ENTER) {
            // enter
            *original = atol(content);
            changed = true;
            break;
        
        } else if (c == CH_DEL) {
            // del
            if (n > 0) content[n-1] = 0;
            n--;

        } else if (c == CH_HOME || c == 0x93) {
            // clear
            content[0] = 0;
            n = 0;
        
        } else if (c == 0x5f) {
            // cancel
            break;

        } else if ((c >= '0' && c <= '9') || c == '-') {
            if (n < len) {
                content[n] = c;
                content[n+1] = 0;
                n++;
            }
        }

    }

    textcolor(COLOR_GRAY2);
    return changed;
}


uint8_t write_sectors_save(char* base)
{
    set_ef_diskid(1);
    write_ef_sector(0x010b, base + 0x0000);
    write_ef_sector(0x010c, base + 0x0200);
    return write_ef_sector(0x010d, base + 0x0400);
}

uint8_t write_sectors_storage(char* base)
{
    set_ef_diskid(1);
    write_ef_sector(0x010e, base + 0x0600);
    return write_ef_sector(0x010f, base + 0x0800);
}

uint8_t write_sectors_camp(char* base)
{
    set_ef_diskid(1);
    write_ef_sector(0x0110, base + 0x0a00);
    write_ef_sector(0x0111, base + 0x0c00);
    write_ef_sector(0x0112, base + 0x0e00);
    write_ef_sector(0x0113, base + 0x1000);
    write_ef_sector(0x0114, base + 0x1200);
    write_ef_sector(0x0115, base + 0x1400);
    write_ef_sector(0x0116, base + 0x1600);
    return write_ef_sector(0x0117, base + 0x1800);
}

void load_sectors(char* base)
{
    set_ef_diskid(1);
    read_ef_sector(0x010b, base + 0x0000);
    read_ef_sector(0x010c, base + 0x0200);
    read_ef_sector(0x010d, base + 0x0400);

    read_ef_sector(0x010e, base + 0x0600);
    read_ef_sector(0x010f, base + 0x0800);

    read_ef_sector(0x0110, base + 0x0a00);
    read_ef_sector(0x0111, base + 0x0c00);
    read_ef_sector(0x0112, base + 0x0e00);
    read_ef_sector(0x0113, base + 0x1000);
    read_ef_sector(0x0114, base + 0x1200);
    read_ef_sector(0x0115, base + 0x1400);
    read_ef_sector(0x0116, base + 0x1600);
    read_ef_sector(0x0117, base + 0x1800);
}
