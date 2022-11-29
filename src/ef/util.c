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


char cname[2] = "#";
char command[16];
//char bcommand;


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

uint8_t load_cbm_sector(uint8_t device, uint8_t track, uint8_t sector, char* dest)
{
    uint16_t i;
    uint8_t result;


    cbm_k_setnam(cname);
    cbm_k_setlfs(5, device, 5);
    result = cbm_k_open();
    if (result != 0) goto load_cbm_sector_finish;

    sprintf(command, "%c1 5 0 %d %d", 0x55, track, sector);
    cbm_k_setnam(command);
    cbm_k_setlfs(15, device, 15);
    result = cbm_k_open();
    if (result != 0) goto load_cbm_sector_finish;
    
    result = cbm_k_chkin(5);
    if (result != 0) goto load_cbm_sector_finish;
    
    for (i=0; i<256; i++) dest[i] = cbm_k_getin();
    result = 0;
    
  load_cbm_sector_finish:
    cbm_k_close(2);
    cbm_k_close(15);
    cbm_k_clrch();
    return result;
}


