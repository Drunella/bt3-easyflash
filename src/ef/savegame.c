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
#define OUTPUT_START_Y 17

#define SAVEGAME_ADDR  ((char*)(0x8000))
#define TEMPMEM_ADDR   ((char*)(0x1000))


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
    cprintf("%d", device); // crashes, stack related?
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


void clear_output()
{
    uint8_t i;

    for (i = OUTPUT_START_Y; i < 25; ++i) {
        cclearxy(0, i, 40);
    }
    gotoxy(0, OUTPUT_START_Y);
}


void draw_menu()
{
    clrscr();
    textcolor(COLOR_GRAY2);
    
    cputs("    Bard's Tale III: Thief of Fate\r\n"
          "\r\n"
          "            Savegame utility\r\n\r\n");

}

void draw_progress_write(int progress, int max)
{
    cprintf("writing sector %d of %d ...  ", progress, max);
    gotox(0);
}

void draw_progress_read(int progress, int max)
{
    cprintf("reading sector %d of %d ...  ", progress, max);
    gotox(0);
}


uint8_t check_bd3_character_disk(uint8_t device)
{
    uint8_t retval;
    char* work = TEMPMEM_ADDR;
    
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


uint8_t create_character_disk(uint8_t device)
{
    int i;
    uint8_t retval;
    char* source;
    int progress = 0;

    cprintf("Please insert a blank disk into #%d.\n\r", device);
    cprintf("The disk will be overwritten and a\n\r"
            "valid Bard's Tale III character disk\n\r"
            "will be created.\n\r\n\r");
    anykey(0, wherey());
    clear_output();

    set_ef_diskid(1);
    
    // open
    //retval = write_cbm_sector_open(device);
    //if (retval != 0) return 0x40;
    
    // load original track 18
    source = SAVEGAME_ADDR;
    load_ef_file_ext(source, 41); // track 18
    for (i=0; i<19; i++) {
        retval = write_cbm_sector_ext(source, device, 18, (uint8_t)i);
        if (retval != 0) return 0x40;
        draw_progress_write(++progress, ALL_SECTORS + 19 + 1);
        source += 0x0100;
    }
    
    // write character disk && savegame
    source = SAVEGAME_ADDR;
    for (i=0; i<ALL_SECTORS/2; i++) {
        retval = read_ef_sector(i, source);
        if (retval != 0) {
            sprintf(temp_line, "backup failed");
            print_error(temp_line);
            return 1;
        }
        retval = write_cbm_sector_ext(source, device, sectors_all[i*2].track, sectors_all[i*2].sector);
        if (retval != 0) return 0x40;
        draw_progress_write(++progress, ALL_SECTORS + 19 + 1);
        retval = write_cbm_sector_ext(source+0x0100, device, sectors_all[i*2+1].track, sectors_all[i*2+1].sector);
        if (retval != 0) return 0x40;
        draw_progress_write(++progress, ALL_SECTORS + 19 + 1);
    }
    
    // write original sector with codewheel code
    source = TEMPMEM_ADDR;
    load_ef_file_ext(source, 42); // codewheel sector
    retval = write_cbm_sector_ext(source, device, 9, 14);
    if (retval != 0) return 0x40;
    draw_progress_write(++progress, ALL_SECTORS + 19 + 1);

    // close
    //write_cbm_sector_close();
    
    cprintf("\n\r");
    cprintf("finished.");

    return 1;
}


uint8_t backup_to_character_disk(uint8_t device)
{
    int i;
    uint8_t retval;
    char* source;
    int progress = 0;
    int maxprogress = SAVE_SECTORS + 2;

    cprintf("Please insert a disk into #%d.\n\r", device);
    cprintf("You can use a valid\n\r"
            "Bard's Tale III character disk.\n\r\n\r");
    anykey(0, wherey());

    set_ef_diskid(1);

    // write disk identification
    // track01-sector00.bin: 45; track01-sector11.bin: 46
    source = TEMPMEM_ADDR;
    load_ef_file_ext(source, 45);
    load_ef_file_ext(source + 0x0100, 46);

    retval = write_cbm_sector_ext(source, device, 1, 0);
    if (retval != 0) return 0x40;
    draw_progress_write(++progress, maxprogress);

    retval = write_cbm_sector_ext(source + 0x0100, device, 1, 11);
    if (retval != 0) return 0x40;
    draw_progress_write(++progress, maxprogress);
    
    // open
    //retval = write_cbm_sector_open(device);
    //if (retval != 0) return 0x40;
    
    // write savegame
    source = SAVEGAME_ADDR;
    for (i=0; i<SAVE_SECTORS/2; i++) {
        retval = read_ef_sector(0x10b + i, source);
        if (retval != 0) {
            sprintf(temp_line, "backup failed");
            print_error(temp_line);
            return 1;
        }
        retval = write_cbm_sector_ext(source, device, sectors_save[i*2].track, sectors_save[i*2].sector);
        if (retval != 0) return 0x40;
        draw_progress_write(++progress, maxprogress);
        source += 0x0100;
        retval = write_cbm_sector_ext(source, device, sectors_save[i*2+1].track, sectors_save[i*2+1].sector);
        if (retval != 0) return 0x40;
        draw_progress_write(++progress, maxprogress);
        source += 0x0100;
    }
    
    // close
    //write_cbm_sector_close();
    
    cprintf("\n\r");
    cprintf("finished.");

    return 1;
}


uint8_t restore_from_character_disk(uint8_t device)
{
    uint8_t i, retval;
    char* dest;
    bool really;

    cprintf("Your save game and all character in\n\r"
            "the refugee camp will be overwritten.\n\r\n\r");
    really = sure(0, wherey());
    if (!really) return 0xb0;
    clear_output();

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
    dest = SAVEGAME_ADDR;
    for (i=0; i<SAVE_SECTORS; i++) {
        retval = read_cbm_sector(dest, device, sectors_save[i].track, sectors_save[i].sector);
        if (retval != 0) return 0x40;
        dest += 0x0100;
        draw_progress_read(i+1, SAVE_SECTORS);
    }
    cprintf("\n\r");

    // saving to easyflash
    cprintf("restoring savegame ...");
    dest = SAVEGAME_ADDR;
    for (i=0; i<SAVE_SECTORS/2; i++) {
        write_ef_sector(i + 0x010b, dest);
        dest += 0x0200;
    }
    
    cprintf(" done.");
        
    return 1;
}


uint8_t restore_from_disk_file(uint8_t device)
{
    uint8_t i;
    char* dest;
    bool really;

    cprintf("Your save game and all character in\n\r"
            "the refugee camp will be overwritten.\n\r\n\r");
    really = sure(0, wherey());
    if (!really) return 0xb0;
    clear_output();

    set_ef_diskid(1);

    cprintf("reading savegame ...");
    // load file from disk
    if (!read_cbm_file(device, "bd3save", (void*)(SAVEGAME_ADDR))) {
        cprintf(" failed.\n\r");
        return 1;
    }
    cprintf(" done.\n\r");

    // saving to easyflash
    cprintf("restoring savegame ...");
    dest = SAVEGAME_ADDR;
    for (i=0; i<SAVE_SECTORS/2; i++) {
        write_ef_sector(i + 0x010b, dest);
        dest += 0x0200;
    }
    
    cprintf(" done.");
        
    return 1;
}

uint8_t backup_to_disk_file(uint8_t device)
{
    int i;
    uint8_t retval;
    char* source;
    int progress = 0;
    int maxprogress = SAVE_SECTORS + 2;

    cprintf("Please insert a disk into #%d.\n\r", device);
    cprintf("Do not use your Bard's Tale III\n\r"
            "character disk.\n\r\n\r");
    anykey(0, wherey());
    clear_output();

    set_ef_diskid(1);
    source = SAVEGAME_ADDR;
    for (i=0; i<SAVE_SECTORS/2; i++) {
        retval = read_ef_sector(0x10b + i, source);
        if (retval != 0) {
            sprintf(temp_line, "backup failed");
            print_error(temp_line);
            return 1;
        }
        source += 0x0200;
    }

    cprintf("writing savegame ...");
    if (write_cbm_file(device, "bd3save", (uint16_t)SAVEGAME_ADDR, (void*)(SAVEGAME_ADDR), 0x1a00)) {
        cprintf(" done.");
    } else {
        cprintf(" failed.");
    }

    return 1;
}


uint8_t format_flash()
{
    int i;
    uint8_t retval;
    char* source;
    bool really;

    cprintf("This will delete your save game\n\r"
            "and all characters.\n\r\n\r");
    really = sure(0, wherey());
    if (!really) return 0xb0;
    clear_output();

    // load original savegame
    source = SAVEGAME_ADDR;
    retval = load_ef_file_ext(source, 47);
    
    // saving to easyflash
    set_ef_diskid(1);
    cprintf("formatting savegame ...");
    for (i=0; i<SAVE_SECTORS/2; i++) {
        write_ef_sector(i + 0x010b, source);
        source += 0x0200;
    }
    cprintf(" done.");

    return 1;
}


void savegame_restore()
{
    // we assume eapi already installed at $c000
    // we assume the sector load/save functions are installed at $cxxx
    // we assume the wrapper are installed at $b7xx
    static uint8_t repaint = 1;
    static uint8_t device = 8;
    uint8_t retval, newdevice;

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
            menu_clear(MENU_START_Y, OUTPUT_START_Y);
            menu_option('D', "Device #");
            cputs("\r\n");
            menu_option('R', "Restore from file");
            cputs("\r\n");
            menu_option('E', "Restore from character disk");
            cputs("\r\n");
            menu_option(0x5f, "Return to main menu");
            print_device(MENU_START_X + 8, MENU_START_Y, device);
            if (repaint & 0x80) clear_output();
            if (repaint & 0x40) print_error(device_last_status());
            gotoxy(0, OUTPUT_START_Y);
        }
        
        switch (cgetc()) {

            case 0x5f:
                return;
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
            case 'r':
                clear_output();
                repaint = restore_from_disk_file(device);
                break;
            case 'e':
                clear_output();
                repaint = restore_from_character_disk(device);
                break;
        }
    }

}

void savegame_backup()
{
    // we assume eapi already installed at $c000
    // we assume the sector load/save functions are installed at $cxxx
    // we assume the wrapper are installed at $b7xx
    static uint8_t repaint = 1;
    static uint8_t device = 8;
    uint8_t retval, newdevice;

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
            menu_clear(MENU_START_Y, OUTPUT_START_Y);
            menu_option('D', "Device #");
            cputs("\r\n");
            menu_option('B', "Backup to file");
            cputs("\r\n");
            menu_option('E', "Backup to character disk");
            cputs("\r\n");
            menu_option('C', "Create character disk");
            cputs("\r\n");
            menu_option(0x5f, "Return to main menu");
            print_device(MENU_START_X + 8, MENU_START_Y, device);
            if (repaint & 0x80) clear_output();
            if (repaint & 0x40) print_error(device_last_status());
            gotoxy(0, OUTPUT_START_Y);
        }
        
        switch (cgetc()) {

            case 0x5f:
                return;
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
                repaint = backup_to_disk_file(device);
                break;
            case 'e':
                clear_output();
                repaint = backup_to_character_disk(device);
                break;
            case 'c':
                clear_output();
                repaint = create_character_disk(device);
                break;
        }
    }

}

void savegame_main()
{
    // we assume eapi already installed at $c000
    // we assume the sector load/save functions are installed at $cxxx
    // we assume the wrapper are installed at $b7xx
    static uint8_t repaint = 1;
    static uint8_t device = 8;

    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLACK);
    draw_menu();

    while (kbhit()) { // clear all keys
        cgetc();
    }
    
    for (;;) {
        if (repaint > 0) {
            menu_clear(MENU_START_Y, OUTPUT_START_Y);
            menu_option('B', "Backup to disk");
            cputs("\r\n");
            menu_option('R', "Restore from disk");
            cputs("\r\n");
            menu_option('I', "Import from Bard's Tale I/II");
            cputs("\r");
            menu_option('F', "Format save game");
            cputs("\r\n");
            menu_option(0x5f, "Return to main menu");
            if (repaint & 0x80) clear_output();
            if (repaint & 0x40) print_error(device_last_status());
            gotoxy(0, OUTPUT_START_Y);
        }
        
        switch (cgetc()) {

            case 0x5f:
                return;
            case 'b':
                clear_output();
                savegame_backup();
                repaint = 1;
                break;
            case 'r':
                clear_output();
                savegame_restore();
                repaint = 1;
                break;
            case 'i':
                clear_output();
                repaint = 1;
                startup_import(); // does not return
                break;
            case 'f':
                clear_output();
                repaint = 1;
                format_flash();
                break;
        }
    }

}

