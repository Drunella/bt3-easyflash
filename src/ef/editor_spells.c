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
#include <ctype.h>
#include <stdlib.h>

#include "util.h"
#include "editor.h"


#define CONJURER_SPELLS_AMOUNT      19
#define MAGICIAN_SPELLS_AMOUNT      19
#define SORCERER_SPELLS_AMOUNT      19
#define WIZARD_SPELLS_AMOUNT        14
#define ARCHMAGE_SPELLS_AMOUNT       8
#define CHRONOMANCER_SPELLS_AMOUNT  27
#define GEOMANCER_SPELLS_AMOUNT     16
#define MISC_SPELLS_AMOUNT           3

#define SPELLS_AMOUNT CONJURER_SPELLS_AMOUNT+MAGICIAN_SPELLS_AMOUNT+SORCERER_SPELLS_AMOUNT+WIZARD_SPELLS_AMOUNT+ARCHMAGE_SPELLS_AMOUNT+CHRONOMANCER_SPELLS_AMOUNT+GEOMANCER_SPELLS_AMOUNT+MISC_SPELLS_AMOUNT


//spellinfo_t spells_list[SPELLS_AMOUNT] = {
spellinfo_t conjurer_spells_list[CONJURER_SPELLS_AMOUNT] = {
//     code ,  off,  bit, clss, lvl, desc
    { "MAFL", 0x54, 0x80,    3,   1, "Mage flame" },
    { "ARFI", 0x54, 0x40,    3,   1, "Arc Fire" },
    { "TRZP", 0x54, 0x20,    3,   1, "Trap Zap" },
    { "FRFO", 0x54, 0x10,    3,   2, "Freeze Foes" },
    { "MACO", 0x54, 0x08,    3,   2, "Kiel's Magic Compass" },
    { "WOHL", 0x54, 0x04,    3,   2, "Word of Healing" },
    { "LERE", 0x54, 0x02,    3,   3, "Lesser Revelation" },
    { "LEVI", 0x54, 0x01,    3,   3, "Levitation" },
    { "WAST", 0x55, 0x80,    3,   3, "Warstrike" },
    { "INWO", 0x55, 0x40,    3,   4, "Elik's Instant Wolf" },
    { "FLRE", 0x55, 0x20,    3,   4, "Flesh Restore" },
    { "GRRE", 0x55, 0x10,    3,   5, "Greater Revelation" },
    { "SHSP", 0x55, 0x08,    3,   5, "Shock Sphere" },
    { "FLAN", 0x55, 0x04,    3,   6, "Flesh Anew" },
    { "MALE", 0x55, 0x02,    3,   6, "Major Levitation" },
    { "REGN", 0x55, 0x01,    3,   7, "Regeneration" },
    { "APAR", 0x56, 0x80,    3,   7, "Apport Arcane" },
    { "FAFO", 0x56, 0x40,    3,   7, "Far Foe" },
    { "INSL", 0x56, 0x20,    3,   7, "Elik's Instant Slayer" },
};

spellinfo_t magician_spells_list[MAGICIAN_SPELLS_AMOUNT] = {
//     code ,  off,  bit, clss, lvl, desc
    { "VOPL", 0x56, 0x10,    4,   1, "Vorpal Plating" },
    { "QUFI", 0x56, 0x08,    4,   1, "Quick Fix" },
    { "SCSI", 0x56, 0x04,    4,   1, "Scry Site" },
    { "HOWA", 0x56, 0x02,    4,   2, "Holy Water" },
    { "MAGA", 0x56, 0x01,    4,   2, "Mage Gauntlets" },
    { "AREN", 0x57, 0x80,    4,   2, "Area Enchant" },
    { "MYSH", 0x57, 0x40,    4,   3, "Ybarra's Mystic Shield" },
    { "OGST", 0x57, 0x20,    4,   3, "Oscon's Ogre Strength" },
    { "STFL", 0x57, 0x10,    4,   3, "Star Flare" },
    { "SPTO", 0x57, 0x08,    4,   4, "Spectre Touch" },
    { "DRBR", 0x57, 0x04,    4,   4, "Dragon Breath" },
    { "ANMA", 0x57, 0x02,    4,   5, "Anti-Magic" },
    { "GIST", 0x57, 0x01,    4,   5, "Giant Strength" },
    { "PHDO", 0x58, 0x80,    4,   6, "Phase Door" },
    { "YMCA", 0x58, 0x40,    4,   6, "Ybarra's Mystical Coat" },
    { "REST", 0x58, 0x20,    4,   7, "Restoration" },
    { "DEST", 0x58, 0x10,    4,   7, "Death Strike" },
    { "ICES", 0x58, 0x08,    4,   7, "Ice Storm" },
    { "STON", 0x58, 0x04,    4,   7, "Stone to Flesh" },
};

spellinfo_t sorcerer_spells_list[SORCERER_SPELLS_AMOUNT] = {
//     code ,  off,  bit, clss, lvl, desc
    { "MIJA", 0x58, 0x02,    2,   1, "Mangar's Mind Jab" },
    { "PHBL", 0x58, 0x01,    2,   1, "Phase Blur" },
    { "LOTR", 0x59, 0x80,    2,   1, "Locate Traps" },
    { "DISB", 0x59, 0x40,    2,   2, "Disbelieve" },
    { "WIWA", 0x59, 0x20,    2,   2, "Wind Warrior" },
    { "FEAR", 0x59, 0x10,    2,   2, "Word of Fear" },
    { "WIOG", 0x59, 0x08,    2,   3, "Wind Ogre" },
    { "INVI", 0x59, 0x04,    2,   3, "Kylearan's Invisibility" },
    { "SESI", 0x59, 0x02,    2,   3, "Second Sight" },
    { "CAEY", 0x59, 0x01,    2,   4, "Cat Eyes" },
    { "WIDR", 0x5a, 0x80,    2,   4, "Wind Dragon" },
    { "DIIL", 0x5a, 0x40,    2,   5, "Disrupt Illusion" },
    { "MIBL", 0x5a, 0x20,    2,   5, "Mangar's Mind Blade" },
    { "WIGI", 0x5a, 0x10,    2,   6, "Wind Giant" },
    { "SOSI", 0x5a, 0x08,    2,   6, "Sorcerer Sight" },
    { "RIME", 0x5a, 0x04,    2,   7, "Rimefang" },
    { "WIHE", 0x5a, 0x02,    2,   7, "Wind Hero" },
    { "MAGM", 0x5a, 0x01,    2,   7, "Mage Maelstrom" },
    { "PREC", 0x5b, 0x80,    2,   7, "Preclusion" },
};

spellinfo_t wizard_spells_list[WIZARD_SPELLS_AMOUNT] = {
//     code ,  off,  bit, clss, lvl, desc
    { "SUEL", 0x5b, 0x40,    1,   1, "Summon Elemental" }, 
    { "FOFO", 0x5b, 0x20,    1,   1, "Fanskar's Force Focus" }, 
    { "PRSU", 0x5b, 0x10,    1,   2, "Prime Summoning" }, 
    { "DEBA", 0x5b, 0x08,    1,   2, "Demon Bane" }, 
    { "FLCO", 0x5b, 0x04,    1,   3, "Flame Column" }, 
    { "DISP", 0x5b, 0x02,    1,   3, "Dispossess" }, 
    { "HERB", 0x5b, 0x01,    1,   4, "Summon Herb" }, 
    { "ANDE", 0x5c, 0x80,    1,   4, "Animate Dead" }, 
    { "SPBI", 0x5c, 0x40,    1,   5, "Baylor's Spell Bind" }, 
    { "SOWH", 0x5c, 0x20,    1,   5, "Storal's Soul Whip" }, 
    { "GRSU", 0x5c, 0x10,    1,   6, "Greater Summoning" }, 
    { "BEDE", 0x5c, 0x08,    1,   6, "Beyond Death" }, 
    { "WIZW", 0x5c, 0x04,    1,   7, "Wacum's Wizard War" }, 
    { "DMST", 0x5c, 0x02,    1,   7, "Demon Strike" },
};

spellinfo_t archmage_spells_list[ARCHMAGE_SPELLS_AMOUNT] = {
//     code ,  off,  bit, clss, lvl, desc
    { "HAFO", 0x5c, 0x01,   10,   1, "Oscon's Haltfoe" }, 
    { "MEME", 0x5d, 0x80,   10,   1, "Melee Men" }, 
    { "BASP", 0x5d, 0x40,   10,   2, "Batch Spell" }, 
    { "CAMR", 0x5d, 0x20,   10,   3, "Camaraderie" }, 
    { "NILA", 0x5d, 0x10,   10,   4, "Fanskar's Night Lance" }, 
    { "HEAL", 0x5d, 0x08,   10,   5, "Heal All" }, 
    { "BRKR", 0x5d, 0x04,   10,   6, "The Brothers Kringle" }, 
    { "MAMA", 0x5d, 0x02,   10,   7, "Mangar's Mallet" },
};

spellinfo_t chronomancer_spells_list[CHRONOMANCER_SPELLS_AMOUNT] = {
//     code ,  off,  bit, clss, lvl, desc
    { "VITL", 0x5d, 0x01,   11,   1, "Vitality" },
    { "ARBO", 0x5e, 0x80,   11,   1, "Arboria Teleport" },
    { "ENIK", 0x5e, 0x40,   11,   1, "Arboria Return" },
    { "WIFI", 0x5e, 0x20,   11,   2, "Witherfist" },
    { "COLD", 0x5e, 0x10,   11,   2, "Frost Force" },
    { "GELI", 0x5e, 0x08,   11,   2, "Gelidia Teleport" },
    { "ECUL", 0x5e, 0x04,   11,   2, "Gelidia Return" },
    { "GOFI", 0x5e, 0x02,   11,   3, "God Fire" },
    { "STUN", 0x5e, 0x01,   11,   3, "Stun" },
    { "LUCE", 0x5f, 0x80,   11,   3, "Lucencia Teleport" },
    { "ILEG", 0x5f, 0x40,   11,   3, "Lucencia  Return" },
    { "LUCK", 0x5f, 0x20,   11,   4, "Luck Chant" },
    { "FADE", 0x5f, 0x10,   11,   4, "Far Death" },
    { "KINE", 0x5f, 0x08,   11,   4, "Kinestia Teleport" },
    { "OBRA", 0x5f, 0x04,   11,   4, "Kinestia Return" },
    { "WHAT", 0x5f, 0x02,   11,   5, "Identify" },
    { "OLAY", 0x5f, 0x01,   11,   5, "Youth" },
    { "OLUK", 0x60, 0x80,   11,   5, "Tenebrosia Teleport" },
    { "ECEA", 0x60, 0x40,   11,   5, "Tenebrosia Return" },
    { "GRRO", 0x60, 0x20,   11,   6, "Grave Robber" },
    { "FOTA", 0x60, 0x10,   11,   6, "Force of Tarjan" },
    { "AECE", 0x60, 0x08,   11,   6, "Tarmitia Teleport" },
    { "KULO", 0x60, 0x04,   11,   6, "Tarmitia Return" },
    { "SHSH", 0x60, 0x02,   11,   7, "Shadow Shield" },
    { "FAFI", 0x60, 0x01,   11,   7, "Fatal Fist" },
    { "EVIL", 0x61, 0x80,   11,   7, "Malefia Teleport" },
    { "LIVE", 0x61, 0x40,   11,   7, "Malefia Return" },
};

spellinfo_t geomancer_spells_list[GEOMANCER_SPELLS_AMOUNT] = {
//     code ,  off,  bit, clss, lvl, desc
    { "EADA", 0x61, 0x20,   12,   1, "Earth Dagger" },
    { "EASO", 0x61, 0x10,   12,   1, "Earth Song" },
    { "EAWA", 0x61, 0x08,   12,   1, "Earth Ward" },
    { "TREB", 0x61, 0x04,   12,   2, "Trebuchet" },
    { "EAEL", 0x61, 0x02,   12,   2, "Earth Elemental" },
    { "WAWA", 0x61, 0x01,   12,   2, "Wall Warp" },
    { "ROCK", 0x62, 0x80,   12,   3, "Petrify" },
    { "ROAL", 0x62, 0x40,   12,   3, "Roscoe's Alert" },
    { "SUSO", 0x62, 0x20,   12,   4, "Succor Song" },
    { "SAST", 0x62, 0x10,   12,   4, "Sandstorm" },
    { "SANT", 0x62, 0x08,   12,   5, "Sanctuary" },
    { "GLST", 0x62, 0x04,   12,   5, "Glacier Strike" },
    { "PATH", 0x62, 0x02,   12,   6, "Pathfinder" },
    { "MABA", 0x62, 0x01,   12,   6, "Magma Blast" },
    { "JOBO", 0x63, 0x80,   12,   7, "Jolt Bolt" },
    { "EAMA", 0x63, 0x40,   12,   7, "Earth Maw" },
};

spellinfo_t misc_spells_list[MISC_SPELLS_AMOUNT] = {
//     code ,  off,  bit, clss, lvl, desc
    { "GILL", 0x63, 0x20,    0,   0, "Gilles Gills" },
    { "DIVA", 0x63, 0x10,    0,   0, "Divine Intervention" },
    { "NUKE", 0x63, 0x08,    0,   0, "Gotterdamurung" },
};


uint8_t spells_amount(uint8_t clss)
{
    if (clss == 0xff) return SPELLS_AMOUNT;   
    if (clss == 0) return MISC_SPELLS_AMOUNT;
    if (clss == 3) return CONJURER_SPELLS_AMOUNT;
    if (clss == 4) return MAGICIAN_SPELLS_AMOUNT;
    if (clss == 2) return SORCERER_SPELLS_AMOUNT;
    if (clss == 1) return WIZARD_SPELLS_AMOUNT;
    if (clss == 10) return ARCHMAGE_SPELLS_AMOUNT;
    if (clss == 11) return CHRONOMANCER_SPELLS_AMOUNT;
    if (clss == 12) return GEOMANCER_SPELLS_AMOUNT;
    return 0;
}

spellinfo_t* spells_list(uint8_t clss)
{
    if (clss == 0) return misc_spells_list;
    if (clss == 3) return conjurer_spells_list;
    if (clss == 4) return magician_spells_list;
    if (clss == 2) return sorcerer_spells_list;
    if (clss == 1) return wizard_spells_list;
    if (clss == 10) return archmage_spells_list;
    if (clss == 11) return chronomancer_spells_list;
    if (clss == 12) return geomancer_spells_list;
    return NULL;
}


/*uint8_t count_spells(character_info_t* character, uint8_t clss)
{
    uint8_t i, amount, count;
    spellinfo_t* spell;  
    spellinfo_t* list;

    list = spells_list(clss);
    if (list == NULL) return 0;
    amount = spells_amount(clss);
    count = 0;
    for (i=0; i<amount; i++) {
        spell = &list[i];
        //if (clss != 0xff && spell->playerclass != clss) continue;
        if (character->spells[spell->offset - 0x54] & spell->bit) count++;
    }
    return count;
}*/

uint8_t count_ones(uint8_t byte)
{
    static const uint8_t NIBBLE_LOOKUP [16] =
    {
      0, 1, 1, 2, 1, 2, 2, 3,
      1, 2, 2, 3, 2, 3, 3, 4
    };


    return NIBBLE_LOOKUP[byte & 0x0F] + NIBBLE_LOOKUP[byte >> 4];
}

uint8_t count_spells(character_info_t* character, uint8_t clss)
{
    uint8_t count = 0;
    
    switch(clss) {
        case 0: // misc
            count += count_ones(character->spells[0x63 - 0x54]&0x38);
            break;
        case 3: // conjurer
            count += count_ones(character->spells[0x54 - 0x54]);
            count += count_ones(character->spells[0x55 - 0x54]);
            count += count_ones(character->spells[0x56 - 0x54]&0xe0);
            break;
        case 4: // magician
            count += count_ones(character->spells[0x56 - 0x54]&0x1f);
            count += count_ones(character->spells[0x57 - 0x54]);
            count += count_ones(character->spells[0x58 - 0x54]&0xfc);
            break;
        case 2: // sorcerer
            count += count_ones(character->spells[0x58 - 0x54]&0x03);
            count += count_ones(character->spells[0x59 - 0x54]);
            count += count_ones(character->spells[0x5a - 0x54]);
            count += count_ones(character->spells[0x5b - 0x54]&0x80);
            break;
        case 1: // wizard
            count += count_ones(character->spells[0x5b - 0x54]&0x7f);
            count += count_ones(character->spells[0x5c - 0x54]&0xfe);
            break;
        case 10: // archmage
            count += count_ones(character->spells[0x5c - 0x54]&0x01);
            count += count_ones(character->spells[0x5d - 0x54]&0xfe);
            break;
        case 11: // chronomancer
            count += count_ones(character->spells[0x5d - 0x54]&0x01);
            count += count_ones(character->spells[0x5e - 0x54]);
            count += count_ones(character->spells[0x5f - 0x54]);
            count += count_ones(character->spells[0x60 - 0x54]);
            count += count_ones(character->spells[0x61 - 0x54]&0xc0);
            break;
        case 12: // geomancer
            count += count_ones(character->spells[0x61 - 0x54]&0x3f);
            count += count_ones(character->spells[0x62 - 0x54]);
            count += count_ones(character->spells[0x63 - 0x54]&0xc0);
            break;
    }

    return count;
}



/*uint8_t count_conjurer_spells(character_info_t character)
{
    uint8_t i, amount;
    spellinfo_t* spell;

    amount = 0;
    for (i=0; i<CONJURER_SPELLS_AMOUNT; i++) {
        spell = &conjurer_spells_list[i];
        if (character->spells[spell->offset - 0x54] & spell->bit) amount++;
    }
    return amount;
}

uint8_t count_magician_spells(character_info_t character)
{
    uint8_t i, amount;
    spellinfo_t* spell;

    amount = 0;
    for (i=0; i<MAGICIAN_SPELLS_AMOUNT; i++) {
        spell = &magician_spells_list[i];
        if (character->spells[spell->offset - 0x54] & spell->bit) amount++;
    }
    return amount;
}*/


