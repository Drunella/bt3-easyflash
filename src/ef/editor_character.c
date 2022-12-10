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


// restriction:
// 0x10: spellcaster
// 0x20: rogue
// 0x40: hunter
transition_t common_transitions[] = {
//    restr,  up  right down  left
    { 0x00, 0x00, 0x00, 0x00, 0x00 },  // last position
    { 0x00, 0x00, 0x06, 0x02, 0x00 },  //  1, str
    { 0x00, 0x01, 0x07, 0x03, 0x00 },  //  2, dex
    { 0x00, 0x02, 0x07, 0x04, 0x00 },  //  3, luck
    { 0x00, 0x03, 0x07, 0x05, 0x00 },  //  4, exp
    { 0x00, 0x04, 0x08, 0x08, 0x00 },  //  5, gold
    { 0x00, 0x00, 0x0b, 0x07, 0x01 },  //  6, int
    { 0x00, 0x06, 0x0c, 0x08, 0x02 },  //  7, con
    { 0x00, 0x07, 0x11, 0x09, 0x05 },  //  8, max cond
    { 0x00, 0x08, 0x12, 0x1e, 0x05 },  //  9, max sppt
    { 0x00, 0x00, 0x00, 0x0b, 0x06 },  //  a, item 1
    { 0x00, 0x0a, 0x00, 0x0c, 0x06 },  //  b, item 2
    { 0x00, 0x0b, 0x00, 0x0d, 0x07 },  //  c, item 3
    { 0x00, 0x0c, 0x00, 0x0e, 0x07 },  //  d, item 4
    { 0x00, 0x0d, 0x00, 0x0f, 0x07 },  //  e, item 5
    { 0x00, 0x0e, 0x00, 0x10, 0x08 },  //  f, item 6
    { 0x00, 0x0f, 0x00, 0x11, 0x08 },  // 10, item 7
    { 0x00, 0x10, 0x00, 0x12, 0x08 },  // 11, item 8
    { 0x00, 0x11, 0x00, 0x13, 0x09 },  // 12, item 9
    { 0x00, 0x12, 0x00, 0x14, 0x09 },  // 13, item 10
    { 0x00, 0x13, 0x00, 0x15, 0x09 },  // 14, item 11
    { 0x00, 0x14, 0x00, 0x16, 0x1e },  // 15, item 12
    { 0x10, 0x15, 0x1b, 0x17, 0x20 },  // 16, conj spells
    { 0x10, 0x16, 0x1c, 0x18, 0x20 },  // 17, sorc spells
    { 0x10, 0x17, 0x1d, 0x19, 0x20 },  // 18, arch spells
    { 0x10, 0x18, 0x1d, 0x1a, 0x18 },  // 19, chrono spells
    { 0x10, 0x19, 0x00, 0x00, 0x00 },  // 1a, misc spells
    { 0x10, 0x15, 0x00, 0x1c, 0x16 },  // 1b, mag spells
    { 0x10, 0x1b, 0x00, 0x1d, 0x17 },  // 1c, wiz spells
    { 0x10, 0x1c, 0x00, 0x19, 0x18 },  // 1d, geo spells
    { 0x60, 0x09, 0x15, 0x1f, 0x00 },  // 1e, disarm, critical
    { 0x20, 0x1e, 0x00, 0x20, 0x00 },  // 1f, ident
    { 0x20, 0x1f, 0x00, 0x00, 0x16 },  // 20, hide
    
};



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
    
    cputsxy(21, 1, "Inventory");
    cputcxy(19, 2, 0xeb); chlinexy(20, 2, 19); cputcxy(39, 2, 0xf3);
    cputcxy(19,15, 0xeb); chlinexy(20,15, 19); cputcxy(39,15, 0xf3);

    cputsxy( 1, 1, "Race:"); cputsxy(6,1, get_race_name(content->playerrace));
    cputsxy( 1, 2, "Clss:"); cputsxy(6,2, get_class_name(content->playerclass));
    cputcxy( 0,3, 0xeb); chlinexy(1, 3, 18); cputcxy(19,3, 0xf3);
    cputsxy( 1, 4, "Str :");
    cputsxy(10, 4, "Int :");
    cputsxy( 1, 5, "Dex :");
    cputsxy(10, 5, "Con :");
    cputsxy( 1, 6, "Luck:");
    cputsxy( 1, 7, "Exp :");
    cputsxy( 1, 8, "Gold:");
    cputsxy( 1, 9, "Lvl :");
    cputsxy( 1,10, "Cond:");
    cputsxy( 1,11, "SpPt:");
    cputsxy( 1,12, "AC  :");
    cputsxy( 1,13, "Stat:");
    cputcxy( 0,14, 0xeb); chlinexy(1, 14, 18); cputcxy(19,14, 0xf3);

    // spells for spellcaster
    cputcxy(19,17, 0xeb); chlinexy(20,17, 19); cputcxy(39,17, 0xf3);
    cputsxy(21, 16, "Spells:");
    if (content->playerclass==1 || content->playerclass==2 || content->playerclass==3 ||
        content->playerclass==4 || content->playerclass==10 || content->playerclass==11 ||
        content->playerclass==12) {
        cputsxy(20, 18, "Con:      Mag:");
        cputsxy(20, 19, "Sor:      Wiz:");
        cputsxy(20, 20, "Arc:      Geo:");
        cputsxy(20, 21, "Chrono:");
        cputsxy(20, 22, "Misc:");
    }

    // skills for rogue    
    if (content->playerclass == 5) {
        cputsxy( 1, 15, "Disarm:"); 
        cputsxy( 1, 16, "Ident :");
        cputsxy( 1, 17, "Hide  :");

    }

    // skills for bard
    if (content->playerclass == 6) {
        cputsxy( 1, 15, "Songs :"); 

    }

    // skills for monk and hunter
    if (content->playerclass==8) {
        cputsxy( 1, 15, "Critic:"); 

    }
}

bool process_field_8u(uint8_t* data, uint8_t x, uint8_t y, uint8_t len, bool selected, bool edit)
{
    bool changed = 0;
    uint32_t v = (uint32_t)*data;
    if (edit && selected) {
        changed = getunumberxy(x, y, len, &v);
        if (changed) *data = (uint8_t)v;
    }
    if (selected) revers(1);
    textcolor(COLOR_WHITE);
    cclearxy(x, y, len);
    gotoxy(x, y); cprintf("%u", *data);
    textcolor(COLOR_GRAY2);
    revers(0);
    return changed;
}

bool process_field_8s(int8_t* data, uint8_t x, uint8_t y, uint8_t len, bool selected, bool edit)
{
    bool changed = false;
    int32_t v = (int32_t)*data;
    if (edit && selected) {
        changed = getsnumberxy(x, y, len, &v);
        if (changed) *data = (int8_t)v;
    }
    if (selected) revers(1);
    textcolor(COLOR_WHITE);
    cclearxy(x, y, len);
    gotoxy(x, y); cprintf("%d", *data);
    textcolor(COLOR_GRAY2);
    revers(0);
    return changed;
}

bool process_field_16u(uint16_t* data, uint8_t x, uint8_t y, uint8_t len, bool selected, bool edit)
{
    bool changed = false;
    uint32_t v = (uint32_t)*data;
    if (edit && selected) {
        changed = getunumberxy(x, y, len, &v);
        if (changed) *data = (uint16_t)v;
    }
    if (selected) revers(1);
    textcolor(COLOR_WHITE);
    cclearxy(x, y, len);
    gotoxy(x, y); cprintf("%u", *data);
    textcolor(COLOR_GRAY2);
    revers(0);
    return changed;
}

bool process_field_32u(uint32_t* data, uint8_t x, uint8_t y, uint8_t len, bool selected, bool edit)
{
    bool changed = false;
    uint32_t v = *data;
    if (edit && selected) {
        changed = getunumberxy(x, y, len, &v);
        if (changed) *data = v;
    }
    if (selected) revers(1);
    textcolor(COLOR_WHITE);
    cclearxy(x, y, len);
    gotoxy(x, y); cprintf("%lu", *data);
    textcolor(COLOR_GRAY2);
    revers(0);
    return changed;
}

bool process_field_item(item_t* item, uint8_t x, uint8_t y, uint8_t len, bool selected, bool edit)
{
    uint8_t data;
    uint8_t n, f;
    bool changed = false;
    if (edit && selected) {
        changed = process_character_list(&data, 0xff);
        if (changed) {
            item->flags = 0; // ### is this allowed? ###
            item->type = data;
            item->uses = get_item_uses(data);
            if (item->uses == 0) {
                item->uses = 0xff;
                item->flags |= 0x02;
            }
        }
    }
    if (selected) revers(1);
    textcolor(COLOR_WHITE);
    if (item->type != 0) {
        cclearxy(x, y, len);
        if (item->flags & 0x01) {
            textcolor(COLOR_GRAY2);
            cputcxy(x, y, 0xfa); // equipped
        } else if (item->flags & 0xc0) {
            textcolor(COLOR_GRAY2);
            cputcxy(x, y, 0x3f); // unidentified
        }
        textcolor(COLOR_WHITE);
        gotoxy(x+1, y);
        n = cprintf("%s", get_item_name(item->type)); // name
        cclearxy(x+n+1, y, len-n-1);
        // uses is type dependant
        f = get_item_uses(item->type);
        if (f > 0) {
            if (item->uses<0xff) { // uses
                gotoxy(x+15, y);
                cprintf("#%d", item->uses);
            }
        }
    } else {
        cclearxy(x, y, len);
    }

    textcolor(COLOR_GRAY2);
    revers(0);
    return changed;
}


uint8_t all_spells, max_all_spells;
uint8_t conjurer_spells, max_conjurer_spells;
uint8_t magician_spells, max_magician_spells;
uint8_t sorcerer_spells, max_sorcerer_spells;
uint8_t wizard_spells, max_wizard_spells;
uint8_t archmage_spells, max_archmage_spells;
uint8_t chronomancer_spells, max_chronomancer_spells;
uint8_t geomancer_spells, max_geomancer_spells;
uint8_t misc_spells, max_misc_spells;

void recalculate_intermediates(character_info_t* content)
{
    //all_spells = count_spells(content, 0xff); // all
    //max_all_spells = spells_amount(0xff);
    conjurer_spells = count_spells(content, 3); // conj: 3
    max_conjurer_spells = spells_amount(3);
    magician_spells = count_spells(content, 4); // mag: 4
    max_magician_spells = spells_amount(4);
    sorcerer_spells = count_spells(content, 2); // sorc: 2
    max_sorcerer_spells = spells_amount(2);
    wizard_spells = count_spells(content, 1); // wiz: 1
    max_wizard_spells = spells_amount(1);
    archmage_spells = count_spells(content, 10); //  archmage 10
    max_archmage_spells = spells_amount(10);
    chronomancer_spells = count_spells(content, 11); // chrono 11
    max_chronomancer_spells = spells_amount(11);
    geomancer_spells = count_spells(content, 12); // geo 12
    max_geomancer_spells = spells_amount(12);
    misc_spells = count_spells(content, 0); // misc
    max_misc_spells = spells_amount(0);
    all_spells = conjurer_spells + magician_spells + sorcerer_spells + wizard_spells + 
                 archmage_spells + chronomancer_spells + geomancer_spells + misc_spells;
    max_all_spells = max_conjurer_spells + max_magician_spells + max_sorcerer_spells + max_wizard_spells + 
                     max_archmage_spells + max_chronomancer_spells + max_geomancer_spells + max_misc_spells;
}    

bool process_field_spells(uint8_t x, uint8_t y, uint8_t clss, bool selected, character_info_t* content, bool edit)
{
    bool changed = false;
    uint8_t count, max;
    //spellinfo_t* list = spells_list(clss);
    
    if (edit && selected) {
        changed = process_character_list(content->spells, clss);
        if (changed) recalculate_intermediates(content);
    }
    
    switch(clss) {
        case 3:
            count = conjurer_spells;
            max = max_conjurer_spells;
            break;
        case 4:
            count = magician_spells;
            max = max_magician_spells;
            break;
        case 2:
            count = sorcerer_spells;
            max = max_sorcerer_spells;
            break;
        case 1:
            count = wizard_spells;
            max = max_wizard_spells;
            break;
        case 10:
            count = archmage_spells;
            max = max_archmage_spells;
            break;
        case 11:
            count = chronomancer_spells;
            max = max_chronomancer_spells;
            break;
        case 12:
            count = geomancer_spells;
            max = max_geomancer_spells;
            break;
        case 0:
            count = misc_spells;
            max = max_misc_spells;
            break;
        default:
            count = 0; max = 0;
            break;
    }
    
    if (selected) revers(1);
    textcolor(COLOR_WHITE);
    gotoxy(x,y);
    cprintf("%2d/%2d", count, max);
    textcolor(COLOR_GRAY2);
    revers(0);
    return changed;
}


bool draw_editor_charactercontent(character_info_t* content, uint8_t position, bool edit)
{
    uint8_t i, value, retval = 0;
    uint16_t percent, v16;
    bool changed;
    
    textcolor(COLOR_GRAY2);
    changed = 0;

    changed |= process_field_8u(&content->strength,      6, 4, 4, (position == 1), edit);
    changed |= process_field_8u(&content->intelligence, 15, 4, 4, (position == 6), edit);
    changed |= process_field_8u(&content->dexterity,     6, 5, 4, (position == 2), edit);
    changed |= process_field_8u(&content->constitution, 15, 5, 4, (position == 7), edit);
    changed |= process_field_8u(&content->luck,          6, 6, 4, (position == 3), edit);
    
    changed |= process_field_32u(&content->experience,   7, 7,10, (position == 4), edit);
    changed |= process_field_32u(&content->gold,         7, 8,10, (position == 5), edit);

    gotoxy(7, 10); cprintf("%u", content->current_hitpoints);
    cputcxy(12,10, '/');
    changed |= process_field_16u(&content->maximum_hitpoints,   14,10, 5, (position == 8), edit);
    
    gotoxy(7, 11); cprintf("%u", content->current_spellpoints);
    cputcxy(12,11, '/');
    changed |= process_field_16u(&content->maximum_spellpoints, 14,11, 5, (position == 9), edit);

    gotoxy(7, 12); cprintf("%d", content->armorclass);

    value = content->status;
    if (value & 0x01) cputcxy( 7, 13, 'P'); else cputcxy( 7, 13, ' '); // poisoned
    if (value & 0x02) cputcxy( 8, 13, 'O'); else cputcxy( 8, 13, ' '); // old
    if (value & 0x04) cputcxy( 9, 13, 'D'); else cputcxy( 9, 13, ' '); // dead
    if (value & 0x08) cputcxy(10, 13, 'S'); else cputcxy(10, 13, ' '); // stoned
    if (value & 0x10) cputsxy(11, 13, "Pa"); else cputsxy(11, 13, "  "); // paralyzed
    if (value & 0x20) cputsxy(13, 13, "Ps"); else cputsxy(12, 13, "  "); // possessed
    if (value & 0x40) cputsxy(15, 13, "In"); else cputsxy(13, 13, "  "); // insane
    //if (value & 0x01) cputcxy(14, 13, '?'); else cputcxy(14, 13, ' '); // ? withered ?

    // items
    for (i=0; i<12; i++) {
        // item position start at 0x0a;
        changed |= process_field_item(&(content->items[i]), 20, 3+i, 19, (position == 0x0a+i), edit);
    }

    // spells for spellcaster
    if (content->playerclass==1 || content->playerclass==2 || content->playerclass==3 ||
        content->playerclass==4 || content->playerclass==10 || content->playerclass==11 ||
        content->playerclass==12) {
        changed |= process_field_spells(24, 18, 3, (position==0x16), content, edit); // conjurer 3
        changed |= process_field_spells(34, 18, 4, (position==0x1b), content, edit); // magician 4
        changed |= process_field_spells(24, 19, 2, (position==0x17), content, edit); // sorcerer 2
        changed |= process_field_spells(34, 19, 1, (position==0x1c), content, edit); // wizard 1
        changed |= process_field_spells(24, 20, 10,(position==0x18), content, edit); // archmage 10
        changed |= process_field_spells(34, 20, 12,(position==0x1d), content, edit); // geomancer 12
        changed |= process_field_spells(28, 21, 11,(position==0x19), content, edit); // chronomancer 11
        changed |= process_field_spells(28, 22, 0, (position==0x1a), content, edit); // misc 0
        //if (changed) recalculate_intermediates(content);
        gotoxy(30,16); cprintf("%3d/%3d", all_spells, max_all_spells);
    }
        
    // skills for rogue
    if (content->playerclass == 5) {
        changed |= process_field_8u(&(content->classdata[0]), 8, 15, 3, (position==0x1e), edit);
        v16 = (uint16_t)content->classdata[0];
        percent = (v16*100) / 256;
        gotoxy( 12, 15); cprintf("(%d%%)", percent);

        changed |= process_field_8u(&(content->classdata[1]), 8, 16, 3, (position==0x1f), edit);
        v16 = (uint16_t)content->classdata[1];
        percent = (v16*100) / 256;
        gotoxy( 12, 16); cprintf("(%d%%)", percent);

        changed |= process_field_8u(&(content->classdata[2]), 8, 17, 3, (position==0x20), edit);
        v16 = (uint16_t)content->classdata[2];
        percent = (v16*100) / 256;
        gotoxy( 12, 17); cprintf("(%d%%)", percent);
    }

    // skills for bard
    if (content->playerclass == 6) {
        gotoxy( 8, 15); cprintf("%3d", content->classdata[0]);
    }

    // skills for hunter
    if (content->playerclass == 8) {
        changed |= process_field_8u(&(content->classdata[0]), 8, 15, 3, (position==0x1e), edit);
        v16 = (uint16_t)content->classdata[0];
        percent = (v16*100) / 256;
        gotoxy( 12, 15); cprintf("(%d%%)", percent);
    }
    
    //if (changed) retval |= 0x01;
    return changed;
}

void draw_status_characterdisplay(bool changed)
{
    //             0123456789012345678901234567890123456789
    cputsxy(0,24, "( )back (     )edit            (    )nav");
    if (changed) cputsxy( 22,24,        "(  )save");
    
    textcolor(COLOR_WHITE);
    cputcxy(1,24, 0x5f);
    if (changed) cputsxy(23,24, "F7");
    cputsxy(9,24, "Enter");
    cputsxy(32,24, "CRSR");
    textcolor(COLOR_GRAY2);
}


bool is_move_allowed(character_info_t* content, uint8_t restriction)
{
    uint8_t playerclass;

    if (restriction == 0) return true;
    playerclass = content->playerclass;    
    if (restriction & 0x10) {
        // only for spellcaster
        if (content->playerclass==1 || content->playerclass==2 || content->playerclass==3 ||
            content->playerclass==4 || content->playerclass==10 || content->playerclass==11 ||
            content->playerclass==12) return true;
    }
    if (restriction & 0x20) {
        // only for rogue
        if (content->playerclass==5) return true;
    }
    if (restriction & 0x40) {
        // only for hunter
        if (content->playerclass==8) return true;
    }
    return false;
}

uint8_t move_down(character_info_t* content, uint8_t position)
{
    uint8_t p = common_transitions[position].down;

    if (p == 0) return position;
    if (common_transitions[0].down != 0) p = common_transitions[0].down;
    if (!is_move_allowed(content, common_transitions[p].restriction)) return position;
    memset(&common_transitions[0], 0, sizeof(transition_t));
    common_transitions[0].up = position;
    return p;
}

uint8_t move_right(character_info_t* content, uint8_t position)
{
    uint8_t p = common_transitions[position].right;

    if (p == 0) return position;
    if (common_transitions[0].right != 0) p = common_transitions[0].right;
    if (!is_move_allowed(content, common_transitions[p].restriction)) return position;
    memset(&common_transitions[0], 0, sizeof(transition_t));
    common_transitions[0].left = position;
    return p;
}

uint8_t move_up(character_info_t* content, uint8_t position)
{
    uint8_t p = common_transitions[position].up;

    if (p == 0) return position;
    if (common_transitions[0].up != 0) p = common_transitions[0].up;
    if (!is_move_allowed(content, common_transitions[p].restriction)) return position;
    memset(&common_transitions[0], 0, sizeof(transition_t));
    common_transitions[0].down = position;
    return p;
}

uint8_t move_left(character_info_t* content, uint8_t position)
{
    uint8_t p = common_transitions[position].left;

    if (p == 0) return position;
    if (common_transitions[0].left != 0) p = common_transitions[0].left;
    if (!is_move_allowed(content, common_transitions[p].restriction)) return position;
    memset(&common_transitions[0], 0, sizeof(transition_t));
    common_transitions[0].right = position;
    return p;
}


bool save_character(character_entry_t* character)
{
    if (character->type == 1) {
        // savegame
        return write_sectors_save(SAVEGAME_ADDR);
    } else if (character->type == 2) {
        // refugee camp
        return write_sectors_camp(SAVEGAME_ADDR);
    }
    return false;
}


void character_main(character_entry_t* character)
{
    uint8_t repaint;
    bool changed;
    uint8_t retval;
    uint16_t position;
    bool edit;
    
    // we assume eapi already installed at $c000
    // we assume the sector load/save functions are installed at $cxxx
    // we assume the wrappers are installed at $b7xx

    // init
    textcolor(COLOR_GRAY2);
    repaint = 1;
    changed = false;
    position = 1;
    memset(&common_transitions[0], 0, sizeof(transition_t));
    edit = false;

    clrscr();
    draw_editor_characterdisplay(character->content);
    recalculate_intermediates(character->content);

    while (kbhit()) cgetc();
    
    for (;;) {
        if (repaint > 0) {
            changed |= draw_editor_charactercontent(character->content, position, edit);
            edit = false;
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

            case 0x11: // down
                position = move_down(character->content, position);
                repaint = 1;
                break;
            case 0x1d: // right
                position = move_right(character->content, position);
                repaint = 1;
                break;
            case 0x91: // up
                position = move_up(character->content, position);
                repaint = 1;
                break;
            case 0x9d: // left
                position = move_left(character->content, position);
                repaint = 1;
                break;
            case 0x0d: // enter
                edit = true;
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
                // editor menu
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