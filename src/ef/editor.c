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



void load_sectors()
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
}



void editor_draw_charselect()
{
    //memcpy(0x0400, screen_select_character, 1000);
    cputsxy(0,0,"    Bard's Tale III: Thief of Fate\r\n"
                "\r\n"
                "           Character Editor");
    //gotoxy(0,5);
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
    cputsxy(2,5, " Save Game ");
    cputsxy(2,13, " Refugee Camp ");
}

void editor_draw_characters(uint8_t highlighted)
{
    uint8_t i, o;
    struct character_info* ch;
    char c;
    for (i=0; i<7; i++) {
        ch = (struct character_info*)(0xa000 + 0x80*i);
        if (ch->name[0] == 0xff) continue;
        for (o=0; o<15; o++) {
            c = ch->name[o];
            if (c == 0xff) continue;
            c = c & 0x7f;
            if (o == 0) c = toupper(c); else c = tolower(c);
            cputcxy(2+o, 6+i, c);
        }
    }
    
}


void main(void)
{
//    uint8_t retval;
    
    // we assume eapi already installed at $c000
    // we assume the sector load/save functions are installed at $cxxx
    // we assume the wrapper are installed at $b7xx

    cart_bankout();
    
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_GRAY1);
    clrscr();
    //gotoxy(0, 0);
    textcolor(COLOR_GRAY2);
    
    //     ****************************************
//    cputs("    Bard's Tale III: Thief of Fate\r\n"
//          "\r\n"
//          "          Character Editor");
    editor_draw_charselect();
    
    load_sectors();
    
    editor_draw_characters(0);
    
    while (kbhit()) {
        cgetc();
    }
    
    for (;;) {
        
        switch (cgetc()) {
        case 'r':
            cart_reset();
            break;
        }
    }
}
