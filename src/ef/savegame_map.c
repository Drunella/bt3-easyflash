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
#include "savegame_map.h"


sector_map_t sectors_save[RESTORE_SECTORS] = { 
    {28,9}, {28,2},  // savegame
    {28,13}, {28,6}, 
    {28,17}, {28,10},
    {28,3}, {28,14}, // storage
    {28,7}, {29,0},
    {29,11}, {29,4}, // refugee camp
    {29,15}, {29,8},
    {29,1}, {29,12},
    {29,5}, {29,16},
    {29,9}, {29,2},
    {29,13}, {29,6},
    {29,17}, {29,10},
    {29,3}, {29,14}
};


sector_map_t sectors_backup[BACKUP_SECTORS] = {
    {1, 0  }, {1, 11 },  // sector 0
    {1, 1  }, {1, 12 }, 
    {1, 2  }, {1, 13 }, 
    {1, 3  }, {1, 14 }, 
    {1, 4  }, {1, 15 }, 
    {1, 5  }, {1, 16 }, 
    {1, 6  }, {1, 17 }, 
    {1, 7  }, {1, 18 }, 
    {1, 8  }, {1, 19 }, 
    {1, 9  }, {1, 20 }, 
    {1, 10 }, {2, 0  },  // sector 10
    {2, 11 }, {2, 1  }, 
    {2, 12 }, {2, 2  }, 
    {2, 13 }, {2, 3  }, 
    {2, 14 }, {2, 4  }, 
    {2, 15 }, {2, 5  }, 
    {2, 16 }, {2, 6  }, 
    {2, 17 }, {2, 7  }, 
    {2, 18 }, {2, 8  }, 
    {2, 19 }, {2, 9  }, 
    {2, 20 }, {2, 10 }, 
    {3, 0  }, {3, 11 }, 
    {3, 1  }, {3, 12 }, 
    {3, 2  }, {3, 13 }, 
    {3, 3  }, {3, 14 }, 
    {3, 4  }, {3, 15 }, 
    {3, 5  }, {3, 16 }, 
    {3, 6  }, {3, 17 }, 
    {3, 7  }, {3, 18 }, 
    {3, 8  }, {3, 19 }, 
    {3, 9  }, {3, 20 }, 
    {3, 10 }, {4, 0  }, 
    {4, 11 }, {4, 1  }, 
    {4, 12 }, {4, 2  }, 
    {4, 13 }, {4, 3  }, 
    {4, 14 }, {4, 4  }, 
    {4, 15 }, {4, 5  }, 
    {4, 16 }, {4, 6  }, 
    {4, 17 }, {4, 7  }, 
    {4, 18 }, {4, 8  }, 
    {4, 19 }, {4, 9  }, 
    {4, 20 }, {4, 10 }, 
    {5, 0  }, {5, 11 }, 
    {5, 1  }, {5, 12 }, 
    {5, 2  }, {5, 13 }, 
    {5, 3  }, {5, 14 }, 
    {5, 4  }, {5, 15 }, 
    {5, 5  }, {5, 16 }, 
    {5, 6  }, {5, 17 }, 
    {5, 7  }, {5, 18 }, 
    {5, 8  }, {5, 19 }, // sector 50
    {5, 9  }, {5, 20 }, 
    {5, 10 }, {6, 0  }, 
    {6, 11 }, {6, 1  }, 
    {6, 12 }, {6, 2  }, 
    {6, 13 }, {6, 3  }, 
    {6, 14 }, {6, 4  }, 
    {6, 15 }, {6, 5  }, 
    {6, 16 }, {6, 6  }, 
    {6, 17 }, {6, 7  }, 
    {6, 18 }, {6, 8  }, 
    {6, 19 }, {6, 9  }, 
    {6, 20 }, {6, 10 }, 
    {7, 0  }, {7, 11 }, 
    {7, 1  }, {7, 12 }, 
    {7, 2  }, {7, 13 }, 
    {7, 3  }, {7, 14 }, 
    {7, 4  }, {7, 15 }, 
    {7, 5  }, {7, 16 }, 
    {7, 6  }, {7, 17 }, 
    {7, 7  }, {7, 18 }, 
    {7, 8  }, {7, 19 }, 
    {7, 9  }, {7, 20 }, 
    {7, 10 }, {8, 0  }, 
    {8, 11 }, {8, 1  }, 
    {8, 12 }, {8, 2  }, 
    {8, 13 }, {8, 3  }, 
    {8, 14 }, {8, 4  }, 
    {8, 15 }, {8, 5  }, 
    {8, 16 }, {8, 6  }, 
    {8, 17 }, {8, 7  }, 
    {8, 18 }, {8, 8  }, 
    {8, 19 }, {8, 9  }, 
    {8, 20 }, {8, 10 }, 
    {9, 0  }, {9, 11 }, 
    {9, 1  }, {9, 12 }, 
    {9, 2  }, {9, 13 }, 
    {9, 3  }, {9, 14 }, 
    {9, 4  }, {9, 15 }, 
    {9, 5  }, {9, 16 }, 
    {9, 6  }, {9, 17 }, 
    {9, 7  }, {9, 18 }, 
    {9, 8  }, {9, 19 }, 
    {9, 9  }, {9, 20 }, 
    {9, 10 }, {10, 0 }, 
    {10, 11}, {10, 1 }, 
    {10, 12}, {10, 2 }, 
    {10, 13}, {10, 3 }, 
    {10, 14}, {10, 4 }, 
    {10, 15}, {10, 5 }, 
    {10, 16}, {10, 6 }, // sector 100
    {10, 17}, {10, 7 }, 
    {10, 18}, {10, 8 }, 
    {10, 19}, {10, 9 }, 
    {10, 20}, {10, 10}, 
    {11, 0 }, {11, 11}, 
    {11, 1 }, {11, 12}, 
    {11, 2 }, {11, 13}, 
    {11, 3 }, {11, 14}, 
    {11, 4 }, {11, 15}, 
    {11, 5 }, {11, 16}, 
    {11, 6 }, {11, 17}, 
    {11, 7 }, {11, 18}, 
    {11, 8 }, {11, 19}, 
    {11, 9 }, {11, 20}, 
    {11, 10}, {12, 0 }, 
    {12, 11}, {12, 1 }, 
    {12, 12}, {12, 2 }, 
    {12, 13}, {12, 3 }, 
    {12, 14}, {12, 4 }, 
    {12, 15}, {12, 5 }, 
    {12, 16}, {12, 6 }, 
    {12, 17}, {12, 7 }, 
    {12, 18}, {12, 8 }, 
    {12, 19}, {12, 9 }, 
    {12, 20}, {12, 10}, 
    {13, 0 }, {13, 11}, 
    {13, 1 }, {13, 12}, 
    {13, 2 }, {13, 13}, 
    {13, 3 }, {13, 14}, 
    {13, 4 }, {13, 15}, 
    {13, 5 }, {13, 16}, 
    {13, 6 }, {13, 17}, 
    {13, 7 }, {13, 18}, 
    {13, 8 }, {13, 19}, 
    {13, 9 }, {13, 20}, 
    {13, 10}, {14, 0 }, 
    {14, 11}, {14, 1 }, 
    {14, 12}, {14, 2 }, 
    {14, 13}, {14, 3 }, 
    {14, 14}, {14, 4 }, 
    {14, 15}, {14, 5 }, 
    {14, 16}, {14, 6 }, 
    {14, 17}, {14, 7 }, 
    {14, 18}, {14, 8 }, 
    {14, 19}, {14, 9 }, 
    {14, 20}, {14, 10}, 
    {15, 0 }, {15, 11}, 
    {15, 1 }, {15, 12}, 
    {15, 2 }, {15, 13}, 
    {15, 3 }, {15, 14}, // sector 150
    {15, 4 }, {15, 15}, 
    {15, 5 }, {15, 16}, 
    {15, 6 }, {15, 17}, 
    {15, 7 }, {15, 18}, 
    {15, 8 }, {15, 19}, 
    {15, 9 }, {15, 20}, 
    {15, 10}, {16, 0 }, 
    {16, 11}, {16, 1 }, 
    {16, 12}, {16, 2 }, 
    {16, 13}, {16, 3 }, 
    {16, 14}, {16, 4 }, 
    {16, 15}, {16, 5 }, 
    {16, 16}, {16, 6 }, 
    {16, 17}, {16, 7 }, 
    {16, 18}, {16, 8 }, 
    {16, 19}, {16, 9 }, 
    {16, 20}, {16, 10}, 
    {17, 0 }, {17, 11}, 
    {17, 1 }, {17, 12}, 
    {17, 2 }, {17, 13}, 
    {17, 3 }, {17, 14}, 
    {17, 4 }, {17, 15}, 
    {17, 5 }, {17, 16}, 
    {17, 6 }, {17, 17}, 
    {17, 7 }, {17, 18}, 
    {17, 8 }, {17, 19}, 
    {17, 9 }, {17, 20}, 
    {17, 10}, {19, 0 }, 
    {19, 11}, {19, 3 }, 
    {19, 14}, {19, 6 }, 
    {19, 17}, {19, 9 }, 
    {19, 1 }, {19, 12}, 
    {19, 4 }, {19, 15}, 
    {19, 7 }, {19, 18}, 
    {19, 10}, {19, 2 }, 
    {19, 13}, {19, 5 }, 
    {19, 16}, {19, 8 }, 
    {20, 0 }, {20, 11}, 
    {20, 3 }, {20, 14}, 
    {20, 6 }, {20, 17}, 
    {20, 9 }, {20, 1 }, 
    {20, 12}, {20, 4 }, 
    {20, 15}, {20, 7 }, 
    {20, 18}, {20, 10}, 
    {20, 2 }, {20, 13}, 
    {20, 5 }, {20, 16}, 
    {20, 8 }, {21, 0 }, 
    {21, 11}, {21, 3 }, 
    {21, 14}, {21, 6 }, 
    {21, 17}, {21, 9 }, // sector 200
    {21, 1 }, {21, 12}, 
    {21, 4 }, {21, 15}, 
    {21, 7 }, {21, 18}, 
    {21, 10}, {21, 2 }, 
    {21, 13}, {21, 5 }, 
    {21, 16}, {21, 8 }, 
    {22, 0 }, {22, 11}, 
    {22, 3 }, {22, 14}, 
    {22, 6 }, {22, 17}, 
    {22, 9 }, {22, 1 }, 
    {22, 12}, {22, 4 }, 
    {22, 15}, {22, 7 }, 
    {22, 18}, {22, 10}, 
    {22, 2 }, {22, 13}, 
    {22, 5 }, {22, 16}, 
    {22, 8 }, {23, 0 }, 
    {23, 11}, {23, 3 }, 
    {23, 14}, {23, 6 }, 
    {23, 17}, {23, 9 }, 
    {23, 1 }, {23, 12}, 
    {23, 4 }, {23, 15}, 
    {23, 7 }, {23, 18}, 
    {23, 10}, {23, 2 }, 
    {23, 13}, {23, 5 }, 
    {23, 16}, {23, 8 }, 
    {24, 0 }, {24, 11}, 
    {24, 3 }, {24, 14}, 
    {24, 6 }, {24, 17}, 
    {24, 9 }, {24, 1 }, 
    {24, 12}, {24, 4 }, 
    {24, 15}, {24, 7 }, 
    {24, 18}, {24, 10}, 
    {24, 2 }, {24, 13}, 
    {24, 5 }, {24, 16}, 
    {24, 8 }, {25, 0 }, 
    {25, 11}, {25, 4 }, 
    {25, 15}, {25, 8 }, 
    {25, 1 }, {25, 12}, 
    {25, 5 }, {25, 16}, 
    {25, 9 }, {25, 2 }, 
    {25, 13}, {25, 6 }, 
    {25, 17}, {25, 10}, 
    {25, 3 }, {25, 14}, 
    {25, 7 }, {26, 0 }, 
    {26, 11}, {26, 4 }, 
    {26, 15}, {26, 8 }, 
    {26, 1 }, {26, 12}, 
    {26, 5 }, {26, 16}, 
    {26, 9 }, {26, 2 }, 
    {26, 13}, {26, 6 }, // sector 250
    {26, 17}, {26, 10}, 
    {26, 3 }, {26, 14}, 
    {26, 7 }, {27, 0 }, 
    {27, 11}, {27, 4 }, 
    {27, 15}, {27, 8 }, 
    {27, 1 }, {27, 12}, 
    {27, 5 }, {27, 16}, 
    {27, 9 }, {27, 2 }, 
    {27, 13}, {27, 6 }, 
    {27, 17}, {27, 10}, 
    {27, 3 }, {27, 14}, 
    {27, 7 }, {28, 0 }, 
    {28, 11}, {28, 4 }, 
    {28, 15}, {28, 8 }, 
    {28, 1 }, {28, 12}, 
    {28, 5 }, {28, 16}, 
    {28, 9 }, {28, 2 }, 
    {28, 13}, {28, 6 }, 
    {28, 17}, {28, 10}, 
    {28, 3 }, {28, 14}, 
    {28, 7 }, {29, 0 }, 
    {29, 11}, {29, 4 }, 
    {29, 15}, {29, 8 }, 
    {29, 1 }, {29, 12}, 
    {29, 5 }, {29, 16}, 
    {29, 9 }, {29, 2 }, 
    {29, 13}, {29, 6 }, 
    {29, 17}, {29, 10}, 
    {29, 3 }, {29, 14}
};

