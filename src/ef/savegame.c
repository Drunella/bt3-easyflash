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


sectors_save[6][2] = { {(uint8_t)28,(uint8_t)9}, 
                       {(uint8_t)28,(uint8_t)2}, 
                       {(uint8_t)28,(uint8_t)13}, 
                       {(uint8_t)28,(uint8_t)6}, 
                       {(uint8_t)28,(uint8_t)17}, 
                       {(uint8_t)28,(uint8_t)10}};


void main(void)
{
    // we assume eapi already installed at $c000
    // we assume the sector load/save functions are installed at $cxxx
    // we assume the wrapper are installed at $b7xx
    uint8_t retval, dev8, y, i;
//    dhandle_t handle;
//    unsigned sectors, size;

    cart_bankout();
    
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLACK);
    clrscr();
    textcolor(COLOR_GRAY2);

    
    cputs("    Bard's Tale III: Thief of Fate\r\n"
          "\r\n"
          "            Savegame editor\r\n\r\n");

    dev8 = device_present(8);
    cprintf("device 8 present: retval = %d\r\n", dev8);
    if (dev8 == 0) {
        cprintf("  %d: %s\n\r", device_last_statuscode(), device_last_status());
    } else {
        cprintf("  no device\n\r\n\r");
    }
    retval = device_present(9);

    cprintf("device 9 present: retval = %d\r\n", retval);
    if (retval == 0) {
        cprintf("  %d: %s\n\r", device_last_statuscode(), device_last_status());
    } else {
        cprintf("  no device\n\r\n\r");
    }
    
//    handle = dio_open(8);
//    sectors = dio_query_sectcount(handle);
//    size = dio_query_sectsize(handle);
//    vcprintf("Sectors: %d\n", sectors);
//    vcprintf("Size: %d\n", size);
    //read_sector(0xa000, 18, 0);
//    uint8_t result;
//    load_cbm_sector(8, 18, 0, (char*)0xa000);
//    load_cbm_sector(8, 18, 1, (char*)0xa100);
//    load_cbm_sector(8, 18, 2, (char*)0xa200);

    // block 267.0: track=28, sector=9
    // block 267.5: track=28, sector=2
    // block 268.0: track=28, sector=13
    // block 268.5: track=28, sector=6
    // block 269.0: track=28, sector=17
    // block 269.5: track=28, sector=10

    y = wherey();
    if (dev8 == 0) {
        for (i=0; i<6; i++) {
            gotoy(y);        
            retval = read_cbm_sector((char*)(0xa000 + i*0xff), 8, sectors_save[i][0], sectors_save[i][1]);
            cprintf("sector read. result = %d\r\n", retval);
            cprintf("status: %s\n\r", device_last_status());
        }
    } else {
        cprintf("device not present!\r\n");
    }
    
    
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
