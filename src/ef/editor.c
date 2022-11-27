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

#include "util.h"


void main(void)
{
    // we assume eapi already installed at $c000
    // we assume the sector load/save functions are installed at $cxxx
    // we assume the wrapper are installed at $b7xx

    cart_bankout();
    
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLACK);
    clrscr();
    //gotoxy(0, 0);
    textcolor(COLOR_GRAY2);
    
    //     ****************************************
    cputs("    Bard's Tale III: Thief of Fate\r\n"
          "\r\n"
          "          Character Editor");
    
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