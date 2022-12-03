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
#include <stdio.h>
#include <string.h>
#include <cbm.h>

#include "util.h"



void cart_kill()
{
    __asm__("lda #$37"); // default
    __asm__("sta $01");
    __asm__("lda #$04");
    __asm__("sta $de02");
}


void cart_bankin()
{
    __asm__("lda #$37");
    __asm__("sta $01");
    __asm__("lda #$87"); // led & 16k
    __asm__("sta $DE02");
}


void cart_bankout()
{
    __asm__("lda #$36");
    __asm__("sta $01");
    __asm__("lda #$04"); // none
    __asm__("sta $DE02");
}

void cart_reset()
{
    __asm__("lda #$a9");  // lda #$35
    __asm__("sta $0100");
    __asm__("lda #$75");
    __asm__("sta $0101");

    __asm__("lda #$8d");  // sta $de02
    __asm__("sta $0102");
    __asm__("lda #$02");
    __asm__("sta $0103");
    __asm__("lda #$de");
    __asm__("sta $0104");

    __asm__("lda #$6c");  // jmp ($fffc)
    __asm__("sta $0105");
    __asm__("lda #$fc");
    __asm__("sta $0106");
    __asm__("lda #$ff");
    __asm__("sta $0107");
    
    __asm__("lda #$00");  // set bank 0
    __asm__("sta $de00");

    __asm__("lda #$37");  // set memory mapping
    __asm__("sta $01");

    __asm__("jmp $0100"); // execute mini reset code
    
    // code to copy
    // lda #$75     $a9 $35
    // sta $de02    $8d $02 $de
    // jmp ($fffc)  $6c $fc $ff
    
}


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

uint8_t write_sectors_storage()
{
    write_ef_sector(0x010e, (char*)0xa600);
    return write_ef_sector(0x010f, (char*)0xa800);
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


void menu_clear(uint8_t start, uint8_t stop)
{
    uint8_t y;

    for (y = start; y < stop; ++y) {
        cclearxy(0, y, 40);
    }
    gotoxy(0, start);
}


void menu_option(char key, char *desc)
{
    textcolor(COLOR_GRAY2);
    cputs("       (");
    textcolor(COLOR_WHITE);
    cputc(key);
    textcolor(COLOR_GRAY2);
    cputs(")  ");
    //textcolor(COLOR_GRAY2);
    //cprintf("%s\r\n", desc);
    cputs(desc);
    cputs("\r\n");
}
