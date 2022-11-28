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
#include <dio.h>

#include "util.h"


//uint8_t __cdecl__ read_sector(char* dest, uint8_t track, uint8_t sector);



void main(void)
{
    // we assume eapi already installed at $c000
    // we assume the sector load/save functions are installed at $cxxx
    // we assume the wrapper are installed at $b7xx
//    dhandle_t handle;
//    unsigned sectors, size;

    cart_bankout();
    
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLACK);
    clrscr();
    textcolor(COLOR_GRAY2);

    
    cputs("    Bard's Tale III: Thief of Fate\r\n"
          "\r\n"
          "            Savegame editor\r\n");

//    handle = dio_open(8);
//    sectors = dio_query_sectcount(handle);
//    size = dio_query_sectsize(handle);
//    vcprintf("Sectors: %d\n", sectors);
//    vcprintf("Size: %d\n", size);
    //read_sector(0xa000, 18, 0);
    
    
    while (kbhit()) { // clear all keys
        cgetc();
    }
    
    for (;;) {
        
        switch (cgetc()) {

            case 'r':
                cart_reset(); // does not return
                break;
        }
    }
}
