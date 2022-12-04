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
#include "editor.h"


#define MAX_CHARACTERS 39
#define CAMP_HEIGHT 9


static character_entry_t character_list[MAX_CHARACTERS];
static uint8_t amount_save;
static uint8_t amount_camp;
static uint8_t amount_total;
static uint8_t character_index;



void prepare_single_character(character_entry_t* entry, character_info_t* ch, uint8_t type)
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
    character_info_t* ch;
    
    memset(character_list, 0, sizeof(character_list));   

    amount_total = 0;
    amount_save = 0;
    amount_camp = 0;
    for (i=0; i<7; i++) {
        ch = (character_info_t*)(0xa000 + 0x80*i);        
        if (ch->name[0] == 0xff) continue;  // empty
        if (ch->name[0] == 0x00) continue;  // empty
        prepare_single_character(&character_list[amount_total], ch, 1);
        amount_total++;
        amount_save++;        
        if (amount_total >= MAX_CHARACTERS) return;
    }    

    for (i=0; i<32; i++) {
        ch = (character_info_t*)(0xaa00 + 0x80*i);
        if (ch->name[0] == 0xff) continue;  // empty
        if (ch->name[0] == 0x00) continue;  // empty
        prepare_single_character(&character_list[amount_total], ch, 2);
        amount_total++;
        amount_camp++;
        if (amount_total >= MAX_CHARACTERS) return;
    }
}

uint8_t next_index(uint8_t index)
{
    uint8_t delta = 0;
    character_entry_t* entry;
    
    while (true) {
        if (index+delta >= amount_total) break;
        delta++;
        entry = &character_list[index+delta];
        if (entry->selectable) {
            index += delta;
            break;
        }
    }
    
    return index;
}

uint8_t previous_index(uint8_t index)
{ 
    uint8_t delta = 0;
    character_entry_t* entry;
    
    while (true) {    
        if (delta >= index) return 0;
        delta++;
        entry = &character_list[index-delta];
        if (entry->selectable) {
            index -= delta;
            break;
        }
    }
    
    return index;
}

void draw_editor_characterlist_frame()
{
    clrscr();
    textcolor(COLOR_GRAY2);
    cputsxy(0,0,"    Bard's Tale III: Thief of Fate\r\n"
                "\r\n"
                "           Character Editor");
    chlinexy(1, 5, 38);
    chlinexy(1,13,38);  // middle
    chlinexy(1, 14+CAMP_HEIGHT, 38);  // bottom
    cvlinexy(0,6,9+CAMP_HEIGHT);
    cvlinexy(39,6,9+CAMP_HEIGHT);
    cputcxy(0, 13, 0xeb);  // middle left
    cputcxy(39,13, 0xf3);  // middle right
    cputcxy(0, 5, 0xf0);  // upper left
    cputcxy(39,5, 0xee);  // upper right
    cputcxy(0, 14+CAMP_HEIGHT,0xed);  // lower left
    cputcxy(39,14+CAMP_HEIGHT,0xfd);  // lower right
    cputsxy(2,5, "[Save Game]");
    cputsxy(2,13, "[Refugee Camp]");
    cputsxy(1,24, "( )back  (     )select   (    )up/down");
    textcolor(COLOR_WHITE);
    cputcxy(2,24, 0x5f);
    cputsxy(11,24, "Enter");
    cputsxy(27,24, "CRSR");
    textcolor(COLOR_GRAY2);
}


void draw_editor_characterlist(uint8_t index)
{
    uint8_t i, n, pos;
    character_info_t* ch;
    character_entry_t* entry;
    static uint8_t camp_offset = 0;
    
    textcolor(COLOR_GRAY2);
    
    for (i=0; i<amount_save; i++) {
        entry = &character_list[i];
        if (entry->type != 1) continue;
        ch = entry->content;
        if (index == i) revers(1); else revers(0);
        if (entry->selectable) textcolor(COLOR_WHITE);
        n = cprint_name(1, 6+i, ch->name);
        cclearxy(1+n, 6+i, 38-n);
        textcolor(COLOR_GRAY2);
        revers(0);
    }
    
    // start offset
    if (index >= amount_save+camp_offset+CAMP_HEIGHT) camp_offset += index - (amount_save+camp_offset+CAMP_HEIGHT) + 1;
    if (index < amount_save+camp_offset) {
        if (amount_save+camp_offset - index >= camp_offset) camp_offset = 0;
        else camp_offset -= amount_save+camp_offset - index;
    }
    for (i=0; i<CAMP_HEIGHT; i++) {
        if (i+camp_offset < amount_total) {
            pos = amount_save+i+camp_offset;
            entry = &character_list[pos];
            if (entry->type != 2) continue;
            ch = entry->content;
            if (index == pos) revers(1); else revers(0);
            if (entry->selectable) textcolor(COLOR_WHITE);
            n = cprint_name(1, 14+i, ch->name);
        } else {
            n = 0;
        }
        cclearxy(1+n, 14+i, 38-n);
        textcolor(COLOR_GRAY2);
        revers(0);
    }
    
    if (camp_offset > 0) cputcxy(39, 14, 0xf1); else cputcxy(39, 14, 0xdd);
    if (amount_save+camp_offset+CAMP_HEIGHT < amount_total) cputcxy(39, 15+CAMP_HEIGHT-2, 0xf2); else cputcxy(39, 15+CAMP_HEIGHT-2, 0xdd);
    
//    gotoxy(0,24); cprintf("off=%d idx=%d save=%d all=%d   ", camp_offset, index, amount_save, amount_total); // debug
}


void main(void)
{
    bool repaint;
    uint8_t retval, index;
    
    // we assume eapi already installed at $c000
    // we assume the sector load/save functions are installed at $cxxx
    // we assume the wrappers are installed at $b7xx

    // init
    //cart_bankout();
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLACK);
    //clrscr();
    textcolor(COLOR_GRAY2);
    repaint = true;
    index = 0;

    // prepare
    set_ef_diskid(1);
    load_sectors();
    prepare_characters();
    index = previous_index(next_index(index));

    // show menu
    draw_editor_characterlist_frame();

    while (kbhit()) cgetc();
    
    for (;;) {
        if (repaint) {
            draw_editor_characterlist(index);
            repaint = false;
        }

//        gotoxy(20,24); cprintf("%d,%d chars", amount_save, amount_camp); // debug

        retval = cgetc();
//        gotoxy(0,24); cprintf("cgetc: %x (%c)", retval, retval); // debug
        switch (retval) {
            case 0x11: // down
            case 0x1d: // right
                //gotoxy(34,24); cprintf("up   ");
                index = next_index(index);
                //if (index < amount_total-1) index++;
                repaint = true;
                break;
            case 0x91: // up
            case 0x9d: // left
                //gotoxy(34,24); cprintf("down ");
                //if (index > 0) index--;
                index = previous_index(index);
                repaint = true;
                break;
            case 0x0d: // enter
                //gotoxy(34,24); cprintf("enter");
                character_main(&character_list[index]);
                draw_editor_characterlist_frame();
                while (kbhit()) cgetc();
                repaint = true;
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
