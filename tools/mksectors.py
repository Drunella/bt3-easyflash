#!/usr/bin/env python3

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

import os
import sys
import glob
import subprocess
import argparse
import traceback
import pprint
import csv



def data_writebyte(data, position, value):
    data[position] = value


def data_writeword(data, position, value):
    data[position] = (value & 0x00ff)
    data[position+1] = (value & 0xff00) >> 8


def data_write24(data, position, value):
    data[position] = (value & 0x0000ff)
    data[position+1] = (value & 0x00ff00) >> 8
    data[position+2] = (value & 0xff0000) >> 16


def decomment(csvfile):
    for row in csvfile:
        raw = row.split('#')[0].strip()
        if raw: yield raw


def main(argv):

    p = argparse.ArgumentParser()
    p.add_argument("-v", dest="verbose", action="store_true", help="Verbose output.")
    p.add_argument("-i", dest="sector_map", action="store", required=True, help="sector map as csv")
    p.add_argument("-o", dest="binary_map", action="store", required=True, help="sector map as binary.")

    args = p.parse_args()
    verbose = args.verbose
    sector_map = args.sector_map
    binary_map = args.binary_map
    
    image_size = 4096
    image = bytearray([255] * image_size)
    counter = 0

    with open(sector_map, newline='') as csvfile:
        csvdata = csv.reader(decomment(csvfile), delimiter=',', quotechar='|')
        for row in csvdata:
            try:
                sector_id = int(row[0].strip(), 0)
                sector_type = int(row[1].strip(), 0)
                sector_bank = int(row[2].strip(), 0)
                sector_offset = int(row[3].strip(), 0)
            except:
                continue
            position = sector_id;
            image[position +    0] = sector_type
            image[position +  512] = sector_bank
            image[position + 1024] = sector_offset
            counter = counter + 1
            

    # write file
    with open(binary_map, "wb") as f:
        f.write(image)

    if verbose is True:
        print("written " + str(counter) + " sector entries")
    
    return 0

        
if __name__ == '__main__':
    try:
        retval = main(sys.argv)
        sys.exit(retval)
    except Exception as e:
        print(e)
        traceback.print_exc()
        sys.exit(1)
