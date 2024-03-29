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

// ----------------------------------------------------------------------------
// Based on code from
// (C) 2006-2015 Per Olofsson, available under an Apache 2.0 license.
// https://github.com/MagerValp/u4remastered.git
// ----------------------------------------------------------------------------

#include <stdbool.h>
#include <conio.h>
#include <stdio.h>

#include "util.h"
//#include "menu_include.h"


#define MENU_START_Y 15


void savegame_main(void);


static void draw_startmenu(void) {
    clrscr();
    textcolor(COLOR_GRAY2);
    cputs("    Bard's Tale III: Thief of Fate\r\n"
          "\r\n"
          "             Designed by\r\n"
          "           Rebecca Heineman,\r\n"
          "          Bruce Schlickbernd,\r\n"
          "         Michael A. Stackpole\r\n"
          "\r\n");
    cputs("\r\n"
          "Copyright (c) 1988 InterPlay Productions\r\n"
          "\r\n"
          "     EasyFlash version by Drunella\r\n"
          "\r\n");
}

static void draw_version()
{
    char text[8];
    uint8_t n;
    
    n = sprintf(text, "v%d.%d", get_version_major(), get_version_minor());
    cputsxy(39-n, 24, text);
}


void main(void)
{
    static bool repaint;
    
    repaint = true;
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLACK);
    draw_startmenu();
    
    while (kbhit()) {
        cgetc();
    }
    
    for (;;) {
        
        if (repaint) {
            menu_clear(MENU_START_Y, 24);
            menu_option('G', "Start game");
            cputs("\r\n");
            menu_option('U', "Savegame utility");
            cputs("\r\n");
            menu_option('E', "Character editor");
            cputs("\r\n");
            menu_option('Q', "Quit to basic");
            draw_version();
        }
        
        repaint = false;
        
        switch (cgetc()) {
        case ' ':
        case 'g':
            menu_clear(MENU_START_Y,24);
            startup_game(); // does not return
            break;

        case 'u':
            savegame_main();
            draw_startmenu();
            repaint = true;
            break;

        case 'e':
            menu_clear(MENU_START_Y,24);
            cputsxy(30, 24, "loading...");
            startup_editor(); // does not return
            break;
        
        case 'q':
            cart_kill();
            __asm__("lda #$37");
            __asm__("sta $01");
            __asm__("ldx #$ff");
            __asm__("txs");
            __asm__("jmp $fcfb");
            break;
        }
    }
}
