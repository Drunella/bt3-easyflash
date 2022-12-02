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


#ifndef SAVEGAME_MAP_H
#define SAVEGAME_MAP_H


#include <stdio.h>


#define RESTORE_SECTORS 26
#define BACKUP_SECTORS 560


typedef struct sector_map_s {
    uint8_t track;
    uint8_t sector;
} sector_map_t;


extern sector_map_t sectors_save[];

extern sector_map_t sectors_backup[];


#endif
