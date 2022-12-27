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
import hashlib
import traceback
import pprint


def load_map(filename):
    m = []
    with open(filename) as f:
        for l in f:
            if l[0] == '#' or l[0] == ';':
                continue
            if len(l.strip()) == 0:
                continue
            m.append(l.split())
    return m


def load_file(filename):
    with open(filename, "rb") as f:
        return bytearray(f.read())


def remove_prg(data):
    low = data[0]
    high = data[1]
    del data[0:2]
    return high*256 + low


def bin_initialize():
    global binary_file
    binary_file = bytearray([0xff] * 64 * 16384) # all 64 banks


def bin_placedata(data, bank, address, size, start):
    global binary_file
    if address < 0x8000 or address >= 0xC000:
        raise Exception("address outside allowed range: 0x{0:04x}".format(address))
    address -= 0x8000
    address += bank * 16384
    if size == 0:
        binary_file[address:address+len(data)] = data[start:]
    else:
        if start+size > len(data):
            raise Exception("given data size is more than actual data size")
        binary_file[address:address+size] = data[start:start+size]


def bin_write(filename):
    global binary_file
    with open(filename, "wb") as f:
        f.write(binary_file)


# format map file
# bank f filename address [value] [length] [start]
# bank a address addr value
#
# f: writes filename at prg address (if *.prg) or given address
# a: writes addreass at given address in lo/hi format
#
# address: destination address in bin file (prg is ignored) (required for non prg)
#          all start addresses must be in range of 0x8000 to 0xbfff (lo-hi area)
#          or must be changed with addr
# length: the number of bytes to transfer, 0 for complete file
# start: start address in the file
# example
# 0 f eapi-am29f040.prg 0xb800 0 0
# 0 f directory.data.prg 0xa000 0 0
# 0 a 0x6ca8 addr 0x9601
#

def process(e):
    global build_path
    bank = int(e[0], 0)
    type = e[1]
    source = e[2]
    address = int(e[3], 0)
    if len(e) > 4:
        #address = int(e[3], 0)
        size = int(e[4], 0)
        start = int(e[5], 0)
        #if flag != "addr":
        #    raise Exception("unknown flag " + flag)
    else:
        #address = 0
        start = 0
        size = 0

    if type == "f":
        # set prg file with load address
        # addr must be given for a non prg file
        filename = os.path.join(build_path, source)
        data = load_file(filename)
        if source.endswith(".prg"):
            value = remove_prg(data)
            if address == 0:
                address = value
        else:
            if address == 0:
                raise Exception("must give address for non prg file")
        bin_placedata(data, bank, address, size, start)
        return address
    elif type == "a":
        # set data in low/high
        # address must be given
        #address = value
        value = int(source, 0)
        if address == 0:
            raise Exception("must give address for set data")
        data = bytearray(2);
        data[0] = value % 256
        data[1] = value // 256
        bin_placedata(data, bank, address, 0, 0)
        return address
    else:
        raise Exception("unknown type " + type)


def main(argv):
    global binary_file
    global build_path
    p = argparse.ArgumentParser()
    p.add_argument("-v", dest="verbose", action="store_true", help="Verbose output.")
#    p.add_argument("-s", dest="source", action="store", required=True, help="source directory.")
    p.add_argument("-b", dest="build", action="store", required=True, help="build directory.")
    p.add_argument("-m", dest="mapfiles", action="append", required=True, help="mapfile.")
    p.add_argument("-o", dest="outputfile", action="store", required=True, help="bin output file.")
    args = p.parse_args()
#    source_path = args.source
    temp_path = os.path.join(args.build, "temp")
    os.makedirs(temp_path, exist_ok=True)
    build_path = args.build
    os.makedirs(build_path, exist_ok=True)
    #obj_path = os.path.join(args.build, "obj")
    #os.makedirs(obj_path, exist_ok=True)

    if (args.verbose):
        print("creating binary cartridge image")

    bin_initialize()

    # add prg files
    for files in args.mapfiles:
        #map = load_map(files))
        map = load_map(files)
        for e in map:
            a = process(e)
            if args.verbose:
                print("included: {0} at {1:02x}:{2:04x}".format(e[2], int(e[0]), a))

    bin_write(args.outputfile)
    
    return 0

        
if __name__ == '__main__':
    try:
        retval = main(sys.argv)
        sys.exit(retval)
    except Exception as e:
        print(e)
        traceback.print_exc()
        sys.exit(1)
