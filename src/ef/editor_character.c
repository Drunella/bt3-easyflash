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


//bool character_changed = false;


void draw_editor_characterdisplay(character_info_t* content)
{
    uint8_t n;
    textcolor(COLOR_GRAY2);
    chlinexy(1, 0, 38); // top
    cputcxy(19,0, 0xf2);
    cvlinexy(19, 1, 22);
    chlinexy(1, 23, 38);  // bottom
    cputcxy(19,0, 0xf2);  // middle top 
    cputcxy(19, 23, 0xf1); // middle bottom
    
    cputcxy(2, 0, '[');
    n = cprint_name(3, 0, content->name); // name
    cputcxy(3+n, 0, ']');

    cvlinexy(0, 1, 22);
    cvlinexy(39, 1, 22);
    cputcxy(0, 0, 0xf0);  // upper left
    cputcxy(39, 0, 0xee);  // upper right
    cputcxy(0, 23, 0xed);  // lower left
    cputcxy(39, 23, 0xfd);  // lower right
    
    cputsxy(21,  1, "(  )Inventory");

    cputsxy( 1,  1, "Race:"); cputsxy(6,1, get_race_name(content->playerrace));
    cputsxy( 1,  2, "Clss:"); cputsxy(6,2, get_class_name(content->playerclass));
    cputcxy( 0,3, 0xeb); chlinexy(1, 3, 18); cputcxy(19,3, 0xf3);
    //cputsxy( 1,  4, "\x12\x05S\x92\x9btr :");
    cputsxy( 1,  4, "Str :");
    cputsxy(10,  4, "Int :");
    cputsxy( 1,  5, "Dex :");
    cputsxy(10,  5, "Con :");
    cputsxy( 1,  6, "Luck:");
    cputsxy( 1,  7, "Exp :");
    cputsxy( 1,  8, "Gold:");
    cputsxy( 1,  9, "Lvl :");

    cputcxy( 0, 10, 0xeb); chlinexy(1,10, 18); cputcxy(19,10, 0xf3);
    cputsxy( 1, 11, "Disarm:");
    cputsxy( 1, 12, "Ident :");
    cputsxy( 1, 13, "Hide  :");
    cputcxy( 0, 10, 0xeb); chlinexy(1,10, 18); cputcxy(19,10, 0xf3);
    cputsxy( 1, 11, "Disarm:");
    cputsxy( 1, 12, "Ident :");
    cputsxy( 1, 13, "Hide  :");

    textcolor(COLOR_WHITE);
    cputsxy(22,1, "F1");
    cputcxy( 1,4, 'S');
    cputcxy(10,4, 'I');
    cputcxy( 1,5, 'D');
    cputcxy(10,5, 'C');
    cputcxy( 1,6, 'L');
    cputcxy( 2,7, 'x');
    cputcxy( 1,8, 'G');
    if (content->playerclass == 0x05) {
        cputcxy( 4,11, 'a');
        cputcxy( 3,12, 'e');
        cputcxy( 1,13, 'H');
    
    }
    textcolor(COLOR_GRAY2);

    cputsxy( 1, 14, "Armor :");
    cputsxy( 1, 15, "Status:");

    cputsxy( 1, 17, "Progress:");

}

void draw_editor_charactercontent(character_info_t* content)
{
    uint8_t i, f;
    item_t* item;
    
    textcolor(COLOR_GRAY2);

    gotoxy( 6, 4); cprintf("%3u", content->strength);
    gotoxy(15, 4); cprintf("%3u", content->intelligence);
    gotoxy( 6, 5); cprintf("%3u", content->dexterity);
    gotoxy(15, 5); cprintf("%3u", content->constitution);
    gotoxy( 6, 6); cprintf("%3u", content->luck);

    gotoxy( 7, 7); cprintf("%lu", content->experience);
    gotoxy( 7, 8); cprintf("%lu", content->gold);
    gotoxy( 7, 9); cprintf("%u", content->level);
    if (content->playerclass == 0x05) {
        gotoxy( 8, 11); cprintf("%3u", content->classdata[0]); // 0x54: disarm
        gotoxy( 8, 12); cprintf("%3u", content->classdata[1]); // 0x55: identify
        gotoxy( 8, 13); cprintf("%3u", content->classdata[2]); // 0x56: hide
    } else {
        cputcxy( 8, 11, '-');
        cputcxy( 8, 12, '-');
        cputcxy( 8, 13, '-');
    }

    for (i=0; i<12; i++) {
        item = &content->items[i];
        cclearxy(20, 3+i, 19);
        if (item->type != 0) {
            if (item->flags & 0x01) {
                cputcxy(20,3+i, 0xfa); // equipped
            }
            cputsxy(21, 3+i, get_item_name(item->type)); // name
            // uses is type dependant
            f = get_item_flags(item->type);
            if (f == 1) {
                if (item->uses<0xff) { // uses
                    gotoxy(35, 3+i);
                    cprintf("#%d", item->uses);
                }
            }
        }
    }

}

void draw_status_characterdisplay(bool changed)
{
    cclearxy(0,24,40);
    //             0123456789012345678901234567890123456789
    cputsxy(0,24, " ( )back");
    if (changed) cputsxy(31,24, "(  )save");
    
    textcolor(COLOR_WHITE);
    cputcxy(2,24, 0x5f);
    if (changed) cputsxy(32,24, "F7");
    textcolor(COLOR_GRAY2);
}


bool input_strength(character_info_t* content)
{
    bool changed = false;
    uint32_t value = (uint32_t)content->strength;
    changed = getnumberxy(6, 4, 3, &value);
    if (changed) content->strength = (uint8_t)value;
    return changed;
}

bool input_intelligence(character_info_t* content)
{
    bool changed = false;
    uint32_t value = (uint32_t)content->intelligence;
    changed = getnumberxy(15, 4, 3, &value);
    if (changed) content->intelligence = (uint8_t)value;
    return changed;
}

bool input_dexterity(character_info_t* content)
{
    bool changed = false;
    uint32_t value = (uint32_t)content->dexterity;
    changed = getnumberxy(6, 5, 3, &value);
    if (changed) content->dexterity = (uint8_t)value;
    return changed;
}

bool input_constitution(character_info_t* content)
{
    bool changed = false;
    uint32_t value = (uint32_t)content->constitution;
    changed = getnumberxy(15, 5, 3, &value);
    if (changed) content->constitution = (uint8_t)value;
    return changed;
}

bool input_luck(character_info_t* content)
{
    bool changed = false;
    uint32_t value = (uint32_t)content->luck;
    changed = getnumberxy(6, 6, 3, &value);
    if (changed) content->luck = (uint8_t)value;
    return changed;
}

bool input_experience(character_info_t* content)
{
    bool changed = false;
    uint32_t value = (uint32_t)content->experience;
    changed = getnumberxy(7, 7, 10, &value);
    if (changed) content->experience = (uint32_t)value;
    return changed;
}

bool input_gold(character_info_t* content)
{
    bool changed = false;
    uint32_t value = (uint32_t)content->gold;
    changed = getnumberxy(7, 8, 10, &value);
    if (changed) content->gold = (uint32_t)value;
    return changed;
}


bool save_character(character_entry_t* character)
{
    if (character->type == 1) {
        // savegame
        return write_sectors_save();
    } else if (character->type == 2) {
        // refugee camp
        return write_sectors_camp();
    }
    return false;
}


void character_main(character_entry_t* character)
{
    uint8_t repaint;
    bool changed;
    uint8_t retval;
    
    // we assume eapi already installed at $c000
    // we assume the sector load/save functions are installed at $cxxx
    // we assume the wrappers are installed at $b7xx

    // init
    //cart_bankout();
    //bgcolor(COLOR_BLACK);
    //bordercolor(COLOR_GRAY1);
    clrscr();
    textcolor(COLOR_GRAY2);
    repaint = 1;
    changed = false;

    // prepare
    draw_editor_characterdisplay(character->content);
    //draw_editor_characterlist_frame();
    //set_ef_diskid(1);
    //load_sectors();
    //prepare_characters();
    //index = previous_index(next_index(index));

    while (kbhit()) cgetc();
    
    for (;;) {
        if (repaint > 0) {
            draw_editor_charactercontent(character->content);
            if (repaint == 1) draw_status_characterdisplay(changed);
            repaint = 0;
        }

//        gotoxy(20,24); cprintf("%d,%d chars", amount_save, amount_camp); // debug

        retval = cgetc();
//        gotoxy(0,24); cprintf("cgetc: %x (%c)", retval, retval); // debug
        switch (retval) {
            case CH_F7: // F7
                if (changed) {
                    if (draw_status_confirmation("Saving character.")) {
                        if (save_character(character) == 0) {
                            changed = false;
                            repaint = 1;
                        } else {
                            // ### error
                            
                            repaint = 2;
                        }
                    } else {
                        repaint = 1;
                    }
                } 
                break;
            case 0x5f: // back arrow
                if (changed) {
                    if (!draw_status_confirmation("Unsaved changes.")) {
                        repaint = 1;
                        break;
                    }
                }
                return;
                
            case 's': // strength
                if (input_strength(character->content)) changed = true;
                repaint = 1;
                break;
            case 'i': // intelligence
                if (input_intelligence(character->content)) changed = true;
                repaint = 1;
                break;
            case 'd': // dexterity
                if (input_dexterity(character->content)) changed = true;
                repaint = 1;
                break;
            case 'c': // constitution
                if (input_constitution(character->content)) changed = true;
                repaint = 1;
                break;
            case 'l': // luck
                if (input_luck(character->content)) changed = true;
                repaint = 1;
                break;
            case 'x': // experience
                if (input_experience(character->content)) changed = true;
                repaint = 1;
                break;
            case 'g': // gold
                if (input_gold(character->content)) changed = true;
                repaint = 1;
                break;
        }
    }
}

/*

struct s_character_list {
    uint8_t type;
    uint8_t selectable;
    struct character_info* content;
};

static struct s_character_list character_list[MAX_CHARACTERS];
static uint8_t amount_save;
static uint8_t amount_camp;
static uint8_t amount_total;
static uint8_t character_index;


char convert_char_bd3_to_editor(char c)
{
    c = c & 0x7f;
    if (c >= 0x60) c = c - 0x20;
    else if (c>= 0x40 && c < 0x60) c = c + 0x20;
    return c;
}


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

uint8_t next_index(uint8_t index)
{
    uint8_t delta = 0;
    struct s_character_list* entry;
    
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
    struct s_character_list* entry;
    
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

void draw_editor_characterlist(uint8_t index)
{
    uint8_t i, n, pos;
    struct character_info* ch;
    struct s_character_list* entry;
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
    bordercolor(COLOR_GRAY1);
    clrscr();
    textcolor(COLOR_GRAY2);
    repaint = true;
    index = 0;

    // prepare
    draw_editor_characterlist_frame();
    set_ef_diskid(1);
    load_sectors();
    prepare_characters();
    index = previous_index(next_index(index));

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
                gotoxy(34,24); cprintf("up   ");
                index = next_index(index);
                //if (index < amount_total-1) index++;
                repaint = true;
                break;
            case 0x91: // up
            case 0x9d: // left
                gotoxy(34,24); cprintf("down ");
                //if (index > 0) index--;
                index = previous_index(index);
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
*/