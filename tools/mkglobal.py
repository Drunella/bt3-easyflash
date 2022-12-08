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

def load_map_file(filename):
    off_switch = "Exports list by value:"
    on_switch = "Exports list by name:"
    parser = False
    symbs = dict()
    with open(filename, "rt") as f:
        for l in f:
            line = l.strip()
            if line == on_switch:
                parser = True
                continue
            if line == off_switch:
                parser = False
                continue
            if parser == False:
                continue
            if len(line) == 0:
                continue
            # parse
            s = line.split()
            if len(s) < 3:
                continue
            k = s[0]
            symbs[k] = int(s[1], 16)
            if len(s) > 3:
                k = s[3]
                symbs[k] = int(s[4], 16)
    return symbs


def main(argv):
    p = argparse.ArgumentParser()
    p.add_argument("symbols", nargs='+', help="symbol to include in address list.")
    p.add_argument("-v", dest="verbose", action="store_true", help="Verbose output.")
    p.add_argument("-m", dest="mapfiles", action="append", required=True, help="map file with exported symbols.")
    p.add_argument("-o", dest="output", action="store", required=True, help="output address file.")
    args = p.parse_args()

    destination = args.output
    verbose = args.verbose

    maps = dict()
    if args.mapfiles is not None:
        for s in args.mapfiles:
            n = load_map_file(s)
            maps = {**maps, **n}


    f = open(destination, "w+")
    for p in args.symbols:
        try:
            if verbose is True:
                print('{0}_addr = ${1:02x}'.format(p, maps[p]), file=f)

        except Exception as e:
            print(e)
            traceback.print_exc()
            f.close()
            return 1
    f.close()
    return 0


if __name__ == '__main__':
    try:
        retval = main(sys.argv)
        sys.exit(retval)
    except Exception as e:
        print(e)
        traceback.print_exc()
        sys.exit(1)
