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
#include <conio.h>
#include <string.h>
#include <ctype.h>

#include "util.h"


#define MAX_CHARACTERS 39


struct s_character_list {
    uint8_t type;
    uint8_t selectable;
    struct character_info* content;
};

static struct s_character_list character_list[MAX_CHARACTERS];
static uint8_t amount_save;
static uint8_t amount_camp;
static uint8_t amount_total;


/*void load_sectors()
{
    read_ef_sector(0x010b, (char*)0xa000);
    read_ef_sector(0x010c, (char*)0xa200);
    read_ef_sector(0x010d, (char*)0xa400);

    read_ef_sector(0x010e, (char*)0xa600);
    read_ef_sector(0x010f, (char*)0xa800);

    read_ef_sector(0x0110, (char*)0xaa00);
    read_ef_sector(0x0111, (char*)0xac00);
    read_ef_sector(0x0112, (char*)0xae00);
    read_ef_sector(0x0113, (char*)0xb000);
    read_ef_sector(0x0114, (char*)0xb200);
    read_ef_sector(0x0115, (char*)0xb400);
    read_ef_sector(0x0116, (char*)0xb600);
    read_ef_sector(0x0117, (char*)0xb800);
}

uint8_t write_sectors_save()
{
    write_ef_sector(0x010b, (char*)0xa000);
    write_ef_sector(0x010c, (char*)0xa200);
    return write_ef_sector(0x010d, (char*)0xa400);
}

uint8_t write_sectors_camp()
{
    write_ef_sector(0x0110, (char*)0xaa00);
    write_ef_sector(0x0111, (char*)0xac00);
    write_ef_sector(0x0112, (char*)0xae00);
    write_ef_sector(0x0113, (char*)0xb000);
    write_ef_sector(0x0114, (char*)0xb200);
    write_ef_sector(0x0115, (char*)0xb400);
    write_ef_sector(0x0116, (char*)0xb600);
    return write_ef_sector(0x0117, (char*)0xb800);
}*/


void prepare_single_character(struct s_character_list* entry, struct character_info* ch, uint8_t type)
{
    entry->selectable = 1;
    if (ch->playerclass >= 0x0d) entry->selectable = 0;
    if (ch->name[0] == 0xaa) entry->selectable = 0;
    entry->type = type;
    entry->content = ch;
}

void prepare_characters()
{
    uint8_t i;
    struct character_info* ch;
    
    memset(character_list, 0, sizeof(character_list));   

    amount_total = 0;
    amount_save = 0;
    amount_camp = 0;
    for (i=0; i<7; i++) {
        ch = (struct character_info*)(0xa000 + 0x80*i);        
        if (ch->name[0] == 0xff) continue;  // empty
        if (ch->name[0] == 0x00) continue;  // empty
        prepare_single_character(&character_list[amount_total], ch, 1);
        amount_total++;
        amount_save++;        
        if (amount_total >= MAX_CHARACTERS) return;
    }    

    for (i=0; i<32; i++) {
        ch = (struct character_info*)(0xaa00 + 0x80*i);
        if (ch->name[0] == 0xff) continue;  // empty
        if (ch->name[0] == 0x00) continue;  // empty
        prepare_single_character(&character_list[amount_total], ch, 2);
        amount_total++;
        amount_camp++;
        if (amount_total >= MAX_CHARACTERS) return;
    }
}


void editor_draw_charselect()
{
    textcolor(COLOR_GRAY2);
    cputsxy(0,0,"    Bard's Tale III: Thief of Fate\r\n"
                "\r\n"
                "           Character Editor");
    chlinexy(1, 5, 38);
    chlinexy(1, 23, 38);
    chlinexy(1,13,38);
    cvlinexy(0,6,18);
    cvlinexy(39,6,18);
    cputcxy(0, 13, 0xeb);  // middle left
    cputcxy(39,13, 0xf3);  // middle right
    cputcxy(0, 5, 0xf0);  // upper left
    cputcxy(39,5, 0xee);  // upper right
    cputcxy(0, 23,0xed);  // lower left
    cputcxy(39,23,0xfd);  // lower right
    cputsxy(2,5, "[Save Game]");
    cputsxy(2,13, "[Refugee Camp]");
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

void editor_draw_characters(uint8_t index)
{
    uint8_t i, n, x;
    struct character_info* ch;
    struct s_character_list* entry;
    
    textcolor(COLOR_GRAY2);
    
    for (i=0; i<amount_save; i++) {
        entry = &character_list[i];
        if (entry->type != 1) continue;
        ch = entry->content;
        if (index == i) revers(1); else revers(0);
        n = cprint_name(1, 6+i, ch->name);
        cclearxy(1+n, 6+i, 38-n);
        revers(0);
    }
    
    // start index
    for (i=0; i<8; i++) {
        entry = &character_list[amount_save+i];
        if (entry->type != 2) continue;
        ch = entry->content;
        if (index == amount_save+i) revers(1); else revers(0);
        n = cprint_name(1, 14+i, ch->name);
        cclearxy(1+n, 14+i, 38-n);
        revers(0);
    }

    
    
}


void main(void)
{
    bool repaint;
    uint8_t retval, index;
    
    // we assume eapi already installed at $c000
    // we assume the sector load/save functions are installed at $cxxx
    // we assume the wrappers are installed at $b7xx

    // init
    cart_bankout();
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_GRAY1);
    clrscr();
    textcolor(COLOR_GRAY2);

    // prepare
    editor_draw_charselect();
    set_ef_diskid(1);
    load_sectors();
    prepare_characters();

    while (kbhit()) cgetc();
    
    repaint = true;
    index = 0;
    
    for (;;) {
        if (repaint) {
            editor_draw_characters(index);
            repaint = false;
        }

        gotoxy(20,24); cprintf("%d,%d chars", amount_save, amount_camp); // debug

        retval = cgetc();
        gotoxy(0,24); cprintf("cgetc: %x (%c)", retval, retval); // debug
        switch (retval) {
            case 0x11: // down
                gotoxy(34,24); cprintf("up   ");
                if (index < amount_total-1) index++;
                repaint = true;
                break;
            case 0x91: // up
                gotoxy(34,24); cprintf("down ");
                if (index > 0) index--;
                repaint = true;
                break;
            case 0x13: // enter
                gotoxy(34,24); cprintf("enter");
                // ### editor menu
                break;
            case 0x5f: // back arrow
                cart_reset(); // does not return
                break;
            case 'S': // debug 's'
                write_sectors_save();
                break;
        }
    }
}
