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

FILEENTRY_SIZE = 0x10
FILEDIRECTORY_SIZE = 0x0200
FILEDATA_SIZE = 0x30000
FILEDATA_STARTBANK = 1
FILEDIRECTOTY_ENTRIES = (FILEDIRECTORY_SIZE / FILEENTRY_SIZE) - 1


def decomment(csvfile):
    for row in csvfile:
        raw = row.split('#')[0].strip()
        if raw: yield raw


def data_writebyte(data, position, value):
    data[position] = value


def data_writeword(data, position, value):
    data[position] = (value & 0x00ff)
    data[position+1] = (value & 0xff00) >> 8


def data_write24(data, position, value):
    data[position] = (value & 0x0000ff)
    data[position+1] = (value & 0x00ff00) >> 8
    data[position+2] = (value & 0xff0000) >> 16


def add_file(path, id, loadaddress, initaddress, prg):
    global image_data
    global image_dir
    global image_data_cursor
    global image_dir_cursor
    
    with open(path, "rb") as file:
        contents = file.read()
        size = len(contents) - prg

    if (image_dir_cursor + 1 >= FILEDIRECTORY_SIZE):
        raise Exception("too many files")
    if (image_data_cursor + size > FILEDATA_SIZE):
        raise Exception("files too large")

    banknum = FILEDATA_STARTBANK + image_data_cursor // 16384
    
    bankaddress = image_data_cursor % 16384
    if bankaddress < 8192:
        bankaddress = bankaddress + 0x8000
    else:
        bankaddress = bankaddress + 0xA000 - 0x2000
    
    if prg > 0:
        loadaddress = contents[0] + contents[1]*256

    data_writebyte(image_dir, image_dir_cursor, id)
    #image_dir_cursor = image_dir_cursor + 16
    data_writeword(image_dir, image_dir_cursor+1, banknum)
    data_writeword(image_dir, image_dir_cursor+2, bankaddress)
    data_write24(image_dir, image_dir_cursor+4, size)
    data_writeword(image_dir, image_dir_cursor+7, loadaddress)
    data_writeword(image_dir, image_dir_cursor+9, initaddress)
    
    # write data
    for i in range(0, size):
        image_data[image_data_cursor+i] = contents[prg + i]

    # next entry
    image_dir_cursor = image_dir_cursor + FILEENTRY_SIZE;
    image_data_cursor = image_data_cursor + size
    
#    dc = image_dir_cursor * FILEENTRY_SIZE;
#    if (dc+1 >= FILEDIRECTORY_SIZE):
#        raise Exception("too many files")
#    banknum = FILEDATA_STARTBANK + image_data_cursor // 16384
#    bankaddress = image_data_cursor % 16384
#    if bankaddress < 8192:
#        bankaddress = bankaddress + 0x8000
#    else:
#        bankaddress = bankaddress + 0xA000 - 0x2000
#    
#    if prg > 0:
#        loadaddress = contents[0] + contents[1]*256
#
#    # write directory entry
#    dc = image_dir_cursor;
#    data_writebyte(image_dir, dc, id)
#    image_dir_cursor = image_dir_cursor + 16
#    data_writeword(image_dir, dc+1, banknum)
#    data_writeword(image_dir, dc+2, bankaddress)
#    data_write24(image_dir, dc+4, size)
#    data_writeword(image_dir, dc+7, loadaddress)
#    data_writeword(image_dir, dc+9, initaddress)
#    
#    # write data
#    for i in range(0, size):
#        image_data[image_data_cursor+i] = contents[prg + i]
#    image_data_cursor = image_data_cursor + size    
    #print(path + ": id="+str(id)+" size="+str(size)+" load="+str(loadaddress)+" init="+str(initaddress))


def main(argv):
    #global image_data_max
    #global image_dir_max
    global image_data
    global image_dir
    global image_data_cursor
    global image_dir_cursor
    #global start_bank

    p = argparse.ArgumentParser()
    p.add_argument("-v", dest="verbose", action="store_true", help="Verbose output.")
    p.add_argument("-l", dest="files_list", action="store", required=True, help="files configuration csv.")
    p.add_argument("-f", dest="files_dir", action="store", required=True, help="files directory.")
    p.add_argument("-o", dest="dest_image", action="store", required=True, help="destination binary image.")
    p.add_argument("-d", dest="dest_dir", action="store", required=True, help="destination binary directory.")
    #p.add_argument("-s", dest="image_size", action="store", required=True, help="destination binary image size in bytes.")
    #p.add_argument("-b", dest="start_bank", action="store", required=True, help="start bank.")

    args = p.parse_args()
    verbose = args.verbose
    files_list = args.files_list
    files_dir = args.files_dir
    dest_image = args.dest_image
    os.makedirs(os.path.dirname(dest_image), exist_ok=True)
    dest_dir = args.dest_dir
    os.makedirs(os.path.dirname(dest_dir), exist_ok=True)
    #image_size = args.image_size
    #start_bank = int(args.start_bank, 0)

    #image_data_max = int(image_size, 0)
    #image_dir_max = 512
    image_data = bytearray([255] * FILEDATA_SIZE)
    image_dir = bytearray([255] * FILEDIRECTORY_SIZE)
    image_data_cursor = 0
    image_dir_cursor = 0

    with open(files_list, newline='') as csvfile:
        csvdata = csv.reader(decomment(csvfile), delimiter=',', quotechar='|')
        for row in csvdata:
            name = row[0].strip()
            try:
                id = int(row[1].strip(), 0)
            except:
                continue
            if id == 0:
                continue
            loadaddress = int(row[2].strip(), 0)
            initaddress = int(row[3].strip(), 0)
            prg = int(row[4].strip(), 0)
            
            path = os.path.join(files_dir, name)
            if not os.path.isfile(path):
                if verbose:
                    print("WARNING: file " + path + " does not exist")
                continue
            add_file(os.path.join(files_dir, name), id, loadaddress, initaddress, prg)
            if verbose:
                print("file " + path + " included into file image")

    # write files
    with open(dest_image, "wb") as f:
        f.write(image_data)
    with open(dest_dir, "wb") as f:
        f.write(image_dir)

    return 0

        
if __name__ == '__main__':
    try:
        retval = main(sys.argv)
        sys.exit(retval)
    except Exception as e:
        print(e)
        traceback.print_exc()
        sys.exit(1)
