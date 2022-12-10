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
#include <stdio.h>

#include "util.h"
#include "editor.h"


#define LIST_HEIGHT 22


char headline_ff[] = "Inventory";
char headline_3[] = "Conjurer Spells";
char headline_4[] = "Magician Spells";
char headline_2[] = "Sorcerer Spells";
char headline_1[] = "Wizard Spells";
char headline_10[] = "Archmage Spells";
char headline_11[] = "Chronomancer Spells";
char headline_12[] = "Geomancer Spells";
char headline_0[] = "Miscellaneous Spells";


uint8_t get_index_max(uint8_t type)
{
    switch (type) {
        case 0xff: return get_item_count(); break;
        case 1: return spells_amount(1); break;
        case 2: return spells_amount(2); break;
        case 3: return spells_amount(3); break;
        case 4: return spells_amount(4); break;
        case 10: return spells_amount(10); break;
        case 11: return spells_amount(11); break;
        case 12: return spells_amount(12); break;
        case 0: return spells_amount(0); break;
        default: break;
    }
    return 0;
}

char* get_headline(uint8_t type)
{
    switch (type) {
        case 0xff: return headline_ff; break;
        case 1: return headline_1; break;
        case 2: return headline_2; break;
        case 3: return headline_3; break;
        case 4: return headline_4; break;
        case 10: return headline_10; break;
        case 11: return headline_11; break;
        case 12: return headline_12; break;
        case 0: return headline_0; break;
        default: break;
    }
    return NULL;
}

char* get_index_text(uint8_t type, uint8_t index, char* data)
{
    static char itemline[40];
    uint8_t id, value, n;
    spellinfo_t* spells;
    char known;
    
    switch (type) {
        case 0xff: 
            id = get_itemid_by_index(index);
            value = get_item_uses(id);
            memset(itemline, 32, sizeof(itemline));
            n = sprintf(itemline, "%s", get_item_name(id));
            if (value != 0) {
                itemline[n] = 32;
                n += sprintf(&itemline[20], "(#%d)", value);
            }
            return itemline;
        case 1:
        case 2:
        case 3:
        case 4:
        case 10:
        case 11:
        case 12:
        case 0:
            spells = spells_list(type);
            if (has_spell(data, &spells[index])) known = 0xfa; else known = ' ';
            n = sprintf(itemline, "%c%s (%s)", known, spells[index].code, spells[index].description);
            return itemline;
            
        default: break;
    }
    return NULL;
}

uint8_t get_index_selection(uint8_t type, uint8_t index)
{
    if (type == 0xff) {
        return get_itemid_by_index(index);
    }
    return 0;
}


uint8_t next_item(uint8_t type, uint8_t index, uint8_t step)
{
    if (index+step >= get_index_max(type) - 1) return get_index_max(type) - 1;
    index += step;
    return index;
}

uint8_t previous_item(uint8_t /*type*/, uint8_t index, uint8_t step)
{
    if (step >= index) return 0;
    index -= step;
    return index;
}


void draw_editor_listdisplay(char* headline)
{
    uint8_t n;
    
    textcolor(COLOR_GRAY2);
    chlinexy(1, 0, 38); // top
    chlinexy(1, 23, 38);  // bottom
    
    if (headline != NULL) {
        cputcxy(2, 0, '[');
        gotoxy(3,0);
        n = cprintf("%s", headline);
        cputcxy(3+n, 0, ']');
    }

    cvlinexy(0, 1, 22);
    cvlinexy(39, 1, 22);
    cputcxy(0, 0, 0xf0);  // upper left
    cputcxy(39, 0, 0xee);  // upper right
    cputcxy(0, 23, 0xed);  // lower left
    cputcxy(39, 23, 0xfd);  // lower right
}


void draw_editor_listcontent(uint8_t type, uint8_t index, char* data)
{
    static uint8_t index_offset = 0;
    uint8_t i, pos, n;
    uint8_t max = get_index_max(type);
    char* content;
    
    // start offset
    if (index >= index_offset+LIST_HEIGHT) index_offset += index - (index_offset+LIST_HEIGHT) + 1;
    if (index < index_offset) {
        if (index_offset - index >= index_offset) index_offset = 0;
        else index_offset -= index_offset - index;
    }
    for (i=0; i<LIST_HEIGHT; i++) {
        if (i+index_offset < max) {
            pos = i+index_offset;
            content = get_index_text(type, pos, data);
            if (index == pos) revers(1); else revers(0);
            textcolor(COLOR_WHITE);
            gotoxy(1,1+i);
            n = cprintf("%s", content);
        } else {
            n = 0;
        }
        cclearxy(1+n, 1+i, 38-n);
        textcolor(COLOR_GRAY2);
        revers(0);
    }

    if (index_offset > 0) cputcxy(39, 1, 0xf1); else cputcxy(39, 1, 0xdd);
    if (index_offset+LIST_HEIGHT < max) cputcxy(39, 1+LIST_HEIGHT-2, 0xf2); else cputcxy(39, 1+LIST_HEIGHT-2, 0xdd);

}

void draw_status_listdisplay(uint8_t type)
{
    if (type == 0xff) {
        //             0123456789012345678901234567890123456789
        cputsxy(0,24, "( )back (     )select          (    )nav");
        
        textcolor(COLOR_WHITE);
        cputcxy(1,24, 0x5f);
        cputsxy(9,24, "Enter");
        cputsxy(32,24, "CRSR");
        textcolor(COLOR_GRAY2);
    } else {
        //             0123456789012345678901234567890123456789
        cputsxy(0,24, "( )back          (     )toggle (    )nav");
        
        textcolor(COLOR_WHITE);
        cputcxy(1,24, 0x5f);
        cputsxy(18,24, "Space");
        cputsxy(32,24, "CRSR");
        textcolor(COLOR_GRAY2);
    }
}


char backup_screen_data[1000];
char backup_color_data[1000];

void backup_screen()
{
  memcpy(backup_screen_data, (char*)(0x0400), 1000);
  memcpy(backup_color_data, (char*)(0xd800), 1000);
}

void restore_screen()
{
  memcpy((char*)(0x0400), backup_screen_data, 1000);
  memcpy((char*)(0xd800), backup_color_data, 1000);
}

bool process_character_list(char* data, uint8_t type)
{
    uint8_t repaint;
    uint8_t index;
    bool changed;
    uint8_t retval;
    uint16_t position;
    spellinfo_t* spells;
    
    // we assume eapi already installed at $c000
    // we assume the sector load/save functions are installed at $cxxx
    // we assume the wrappers are installed at $b7xx

    // init
    backup_screen();
    clrscr();
    textcolor(COLOR_GRAY2);
    repaint = 1;
    position = 1;
    changed = false;
    
    // prepare
    draw_editor_listdisplay(get_headline(type));
    if (type == 0xff) {
        index = get_index_itemid((uint8_t)(*data));
    } else {
        index = 0;
    }
        

    while (kbhit()) cgetc();
    
    for (;;) {
        if (repaint > 0) {
            draw_editor_listcontent(type, index, data);
            draw_status_listdisplay(type);
            repaint = 0;
        }

        retval = cgetc();
        if (type == 0xff && retval >= 'a' && retval <= 'z') {
            index = get_alphabetical_index(retval);
            repaint = 1;
        } else switch (retval) {
            case 0x5f: // back arrow
                if (type == 0xff) {
                    restore_screen();
                    return false;
                } else {
                    restore_screen();
                    return changed;
                }

            case 0x11: // down
                index = next_item(type, index, 1);
                repaint = 1;
                break;
            case 0x1d: // right
                index = next_item(type, index, 10);
                repaint = 1;
                break;
            case 0x91: // up
                index = previous_item(type, index, 1);
                repaint = 1;
                break;
            case 0x9d: // left
                index = previous_item(type, index, 10);
                repaint = 1;
                break;
            case 0x20: // space
                if (type != 0xff) {
                    spells = spells_list(type);
                    toggle_spell(data, &spells[index]);
                    changed = true;
                    repaint = 1;
                }
                break;
            case 0x0d: // enter
                if (type == 0xff) {
                    // item list
                    (uint8_t)(*data) = get_index_selection(type, index);
                    restore_screen();
                    return true;
                }
                break;
                
        }
    }
}
