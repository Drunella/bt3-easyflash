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
# processing

amount_sectors = 683
usable_sectors = 664
track_borders = [0, 21, 42, 63, 84, 105, 126, 147, 168, 189, 210, 231, 252, 273, 294, 315, 336, 357, 376, 395, 414, 433, 452, 471, 490, 508, 526, 544, 562, 580, 598, 615, 632, 649, 666, 683]
tracks_zone3 = [0x00, 0x0B, 0x01, 0x0C, 0x02, 0x0D, 0x03, 0x0E, 0x04, 0x0F, 0x05, 0x10, 0x06, 0x11, 0x07, 0x12, 0x08, 0x13, 0x09, 0x14, 0x0A]
tracks_zone2 = [0x00, 0x0B, 0x03, 0x0E, 0x06, 0x11, 0x09, 0x01, 0x0C, 0x04, 0x0F, 0x07, 0x12, 0x0A, 0x02, 0x0D, 0x05, 0x10, 0x08]
tracks_zone1 = [0x00, 0x0B, 0x04, 0x0F, 0x08, 0x01, 0x0C, 0x05, 0x10, 0x09, 0x02, 0x0D, 0x06, 0x11, 0x0A, 0x03, 0x0E, 0x07]
tracks_zone0 = [0x00, 0x0B, 0x05, 0x10, 0x0A, 0x04, 0x0F, 0x09, 0x03, 0x0E, 0x08, 0x02, 0x0D, 0x07, 0x01, 0x0C, 0x06]


def validate(verbose, data, ident):
  if data[0]!=0x43 and data[1]!=0x42 and data[2]!=0x4d:
      raise Exception("unknown disk content")
      
  if (data[ 7]!=0x42 and data[ 8]!=0x41 and data[ 9]!=0x52 and data[10]!=0x44 and 
      data[11]!=0x27 and data[12]!=0x53 and data[13]!=0x20 and data[14]!=0x54 and
      data[15]!=0x41 and data[16]!=0x4c and data[17]!=0x45 and data[18]!=0x20 and
      data[19]!=0x33):
    raise Exception("not a Bards' Tale 3 disk")

  ident = int(ident)
  diskid = data[11*256 + 255]
  if verbose is True:
      print("requested disk identifier: " + str(ident) + ", found identifier: " + str(diskid));
  
  if (diskid == ident):
    return

  if (ident == 0):
    raise Exception("not a Bard's Tale 3 Boot disk")
  if (ident == 1):
    raise Exception("not a Bard's Tale 3 Character disk")
  if (ident == 2):
    raise Exception("not a Bard's Tale 3 Dungeon A disk")
  if (ident == 3):
    raise Exception("not a Bard's Tale 3 Dungeon B disk")
  raise Exception("unknown identifier")


# ----------------------------------------------------------------------------
# main block

def main():
  parser = argparse.ArgumentParser(description="Sanitize c64 prodos images.")
  parser.add_argument("-v", action="store_true", dest="verbose", required=False, help="verbose")
  parser.add_argument("-s", action="store", dest="source", required=True, help="source file")
  parser.add_argument("-d", action="store", dest="dest", required=True, help="destination file")
  parser.add_argument("-i", action="store", dest="identifier", required=True, help="identifier for the disk")
  arguments = parser.parse_args()
  source_filename = arguments.source
  dest_filename = arguments.dest
  identifier = arguments.identifier

  if (arguments.verbose):
    print("sanitizing d64 disk to prodos sector disk")

  # check files
  if not os.path.exists(source_filename):
    raise FileNotFoundError(source_filename)

  #print("source: " + source_filename)
  with open(source_filename, "rb") as f:
    source_data = f.read()
    
  validate(arguments.verbose, source_data, identifier)
  
  #print("destination: " + dest_filename)
  dest_data = [0] * usable_sectors * 256
  
  track = 0
  counter = 0
  for block in range(0, amount_sectors):
    if block >= track_borders[track]:
      track = track + 1
    pos = block - track_borders[track - 1]
    if track < 18:
      sector = tracks_zone3[pos]
    elif track == 18:
      sector = 0xff
    elif track < 25:
      sector = tracks_zone2[pos]
    elif track < 31:
      sector = tracks_zone1[pos]
    elif track < 36:
      sector = tracks_zone0[pos]
    else:
      raise Exception("track number too large")
    if sector == 0xff:
      continue
      
    # copy data in the "classic way"
    address = (track_borders[track - 1] + sector)
    for i in range(0,256):
      dest_data[counter*256+i] = source_data[address*256 + i]
        
    #print("block " + str(counter/2) + ": track="+ str(track) + ", sector=" + str(sector) )
    counter = counter + 1

  binary_format = bytearray(dest_data)
  with open(dest_filename, "wb") as f:
    f.write(binary_format)
  if (arguments.verbose):
    print(source_filename + " sanitized to " + dest_filename)
  return 0

if __name__ == "__main__":
  main()
