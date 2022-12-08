#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# ----------------------------------------------------------------------------
# Copyright 2023 Drunella
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ----------------------------------------------------------------------------
 
# ----------------------------------------------------------------------------
# import and definitions
import sys
import os
import json
import pprint
import argparse



# ----------------------------------------------------------------------------
# main block

def main(argc, argv):
    for i in range(1, argc):
        c = int(argv[i], 16);
        if (c >= 0x80 and c <=0xff):
            c -= 0x80
        
        if (c == 32):
            print(" ", end='');
        elif (c == 13 or c == 10):
            print()
        else:
            print(chr(c), end='');
        
    print()

if __name__ == "__main__":
  main(len(sys.argv), sys.argv)
