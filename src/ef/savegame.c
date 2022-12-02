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
#include <stdio.h>
#include <conio.h>
#include <string.h>

#include "util.h"
#include "savegame_map.h"


#define MENU_START_Y 6
#define MENU_START_X 12
#define OUTPUT_START_Y 18


static char temp_line[41];


bool sure(uint8_t x, uint8_t y)
{
    char c;
    uint8_t ox, oy;
    
    ox = wherex();
    oy = wherey();
    textcolor(COLOR_GRAY2);
    cputsxy(x, y, "Are you sure? ");
    cursor(1);
    c = cgetc();
    cursor(0);
    cclearxy(x, y, 16);
    gotoxy(ox, oy);
    return c == 'y';
}

void anykey(uint8_t x, uint8_t y)
{
    uint8_t ox, oy;
    
    ox = wherex();
    oy = wherey();
    textcolor(COLOR_GRAY2);
    cputsxy(x, y, "Please press any key.");
    while (!kbhit());
    while (kbhit()) cgetc(); // clear all keys
    cclearxy(x, y, 22);

    gotoxy(ox, oy);
    return;
}

static void print_device(uint8_t x, uint8_t y, uint8_t device)
{
    if (device == 0) return;
    cclearxy(x, y, 3);
    gotoxy(x, y);
    textcolor(COLOR_WHITE);
    cprintf("%d", device); // ### crashes, stack related?
    textcolor(COLOR_GRAY2);
}

static void print_error(char* text)
{
    uint8_t len;
    
    textcolor(COLOR_GRAY2);
    len = strlen(text);
    if (len == 0) return;
    cputsxy(0, 24, "Error: ");
    cputsxy(7, 24, text);
    if (len < 35) {
        cclearxy(7+len, 24, 35-len);
    }
}

void clear_error()
{
    cclearxy(0, 24, 40);
}


static uint8_t select_device(uint8_t x, uint8_t y)
{
    uint8_t device = 0;
    uint8_t input_len = 0;
    char c;

    for (;;) {
        if (input_len) {
            print_device(x, y, device);
        } else {
            cclearxy(x, y, 3);
            gotoxy(x, y);
        }

        cursor(1);
        c = cgetc();
        cursor(0);

        if (c == CH_ENTER) {
            if (device >= 4 && device <= 30) {
                return device;
            }
        } else if (c == CH_DEL && input_len > 0) {
            device /= 10;
            --input_len;
        } else if (c >= '0' && c <= '9' && input_len < 2) {
            device = device * 10 + c - '0';
            ++input_len;
        }
    }
}

void clear_menu()
{
    uint8_t i;

    for (i = MENU_START_Y; i < OUTPUT_START_Y; ++i) {
        cclearxy(0, i, 40);
    }
    gotoxy(0, MENU_START_Y);
}

void clear_output()
{
    uint8_t i;

    for (i = OUTPUT_START_Y; i < 25; ++i) {
        cclearxy(0, i, 40);
    }
    gotoxy(0, OUTPUT_START_Y);
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

void draw_menu()
{
    clrscr();
    textcolor(COLOR_GRAY2);
    
    cputs("    Bard's Tale III: Thief of Fate\r\n"
          "\r\n"
          "            Savegame utility\r\n\r\n");

}

void draw_progress(int progress) 
{
    cprintf("writing sector %d of %d...  ", progress, BACKUP_SECTORS + 19 + 1);
    gotox(0);
}


uint8_t check_bd3_character_disk(uint8_t device)
{
    uint8_t retval;
    char* work = (char*)TEMP_ADDRESS;
    
    retval = read_cbm_sector(work, device, 1, 0);
    if (retval != 0) return retval;

    retval = read_cbm_sector(work+0x0100, device, 1, 11);
    if (retval != 0) return retval;
    
    if (work[0]!=0x43 && work[1]!=0x42 && work[2]!=0x4d) return 0xff;
    
    if (work[7]!=0x42 && work[8]!=0x41 && work[9]!=0x52 && 
        work[10]!=0x44 && work[11]!=0x27 && work[12]!=0x53) return 0xff;
        
    if (work[0x1ff] != 0x01) return 0xff;
    return 0;
}


uint8_t backup_to_disk(uint8_t device)
{
    int i;
    uint8_t retval;
    char* source;
    int progress = 0;

    cprintf("Please insert a blank disk into #%d.\n\r", device);
    cprintf("The disk will be overwritten and a\n\r"
            "valid Bard's Tale III character disk\n\r"
            "will be created.\n\r");
    anykey(0, wherey());

    set_ef_diskid(1);
    source = (char*)SAVE_ADDRESS;
    
    // open
    //retval = write_cbm_sector_open(device);
    //if (retval != 0) return 0x40;
    
    // load original track 18
    load_ef_file(41); // track 18
    for (i=0; i<19; i++) {
        retval = write_cbm_sector_ext(source, device, 18, (uint8_t)i);
        if (retval != 0) return 0x40;
        draw_progress(++progress);
        source += 0x0100;
    }
    
    // write character disk && savegame
    source = (char*)SAVE_ADDRESS;
    for (i=0; i<BACKUP_SECTORS/2; i++) {
        retval = read_ef_sector(i, source);
        if (retval != 0) {
            sprintf(temp_line, "backup failed");
            print_error(temp_line);
            return 1;
        }
        retval = write_cbm_sector_ext(source, device, sectors_backup[i*2].track, sectors_backup[i*2].sector);
        if (retval != 0) return 0x40;
        draw_progress(++progress);
        retval = write_cbm_sector_ext(source+0x0100, device, sectors_backup[i*2+1].track, sectors_backup[i*2+1].sector);
        if (retval != 0) return 0x40;
        draw_progress(++progress);
    }
    
    
    
    // write original sector with codewheel code
    load_ef_file(42); // codewheel sector
    retval = write_cbm_sector_ext(source, device, 18, i);
    if (retval != 0) return 0x40;
    draw_progress(++progress);

    // close
    //write_cbm_sector_close();
    
    cprintf("\n\r");
    cprintf("finished.");

    return 1;

}

uint8_t restore_from_disk(uint8_t device)
{
    uint8_t i, retval;
    char* dest;
    bool really;

    cprintf("Your save game and all character in\n\r"
            "the refugee camp will be overwritten.\n\r");
    really = sure(0, wherey());
    if (!really) return 0xb0;

    set_ef_diskid(1);
    
    // test if valid character disk
    retval = check_bd3_character_disk(device);
    if (retval == 0xff) {
        print_error("not a valid character disk");
        return 1;
    } else if (retval > 0) {
        return 0x40;
    }

    // load 26 sectors from disk
    dest = (char*)SAVE_ADDRESS;
    for (i=0; i<RESTORE_SECTORS; i++) {
        retval = read_cbm_sector(dest, device, sectors_save[i].track, sectors_save[i].sector);
        if (retval != 0) return 0x40;
        cprintf("loading sector %d of %d...  ", i, RESTORE_SECTORS);
        gotox(0);
        dest += 0x0100;
    }
    cprintf("\n\r");

    // saving to easyflash
    cprintf("saving savegame ...");
    write_sectors_save();
    cprintf(" ...");
    write_sectors_storage();
    cprintf(" ...");
    write_sectors_camp();
    cprintf(" done.");
        
    return 1;
}


void main(void)
{
    // we assume eapi already installed at $c000
    // we assume the sector load/save functions are installed at $cxxx
    // we assume the wrapper are installed at $b7xx
    static uint8_t repaint = 1;
    static uint8_t device = 8;
    uint8_t retval, newdevice;

    cart_bankout();
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLACK);
    draw_menu();
    retval = device_present(device);
    if (retval != 0) device = 0;
    device_clear_status();

    while (kbhit()) { // clear all keys
        cgetc();
    }
    
    for (;;) {
        if (repaint > 0) {
            clear_menu();
            menu_option('D', "Device #");
            cputs("\r\n");
            menu_option('B', "Backup to floppy disk");
            cputs("\r\n");
            menu_option('R', "Restore from floppy disk");
            cputs("\r\n");
            menu_option('1', "Import from Bard's Tale I");
            cputs("\r\n");
            menu_option('2', "Import from Bard's Tale II");
            cputs("\r\n");
            menu_option(0x5f, "Return to main menu");
            print_device(MENU_START_X + 8, MENU_START_Y, device);
            if (repaint & 0x80) clear_output();
            if (repaint & 0x40) print_error(device_last_status());
        }
        
        switch (cgetc()) {

            case 0x5f:
                clear_output();
                cart_reset(); // does not return
                break;
            case 'd':
                clear_output();
                newdevice = select_device(MENU_START_X, MENU_START_Y + 1);
                retval = device_present(newdevice);
                if (retval == 0) {
                    repaint = 1;
                    device = newdevice;
                } else {
                    sprintf(temp_line, "device %d not present", newdevice);
                    print_error(temp_line);
                    repaint = 1;
                }
                break;
            case 'b':
                clear_output();
                repaint = backup_to_disk(device);
                break;
            case 'r':
                clear_output();
                repaint = restore_from_disk(device);
                break;
            case '1':
                clear_output();
                repaint = 0x80;
                break;
            case '2':
                clear_output();
                repaint = 0x80;
                break;
        }
    }


/*
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
        if (repaint) {
            clear_menu();
            menu_option('G', "Start game");
            cputs("\r\n");
            menu_option('J', "Journey Onward");
            cputs("\r\n");
            menu_option('S', "Manage savegames");
            cputs("\r\n");
            menu_option('Q', "Quit to basic");
        }
        
        switch (cgetc()) {

            case 'r':
                cart_reset(); // does not return
                break;
        }
    }*/
}
