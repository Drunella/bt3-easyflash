#!/usr/bin/python3
# -*- coding: utf-8 -*-
 
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



# ----------------------------------------------------------------------------
# main block

def main():
  parser = argparse.ArgumentParser(description="Sanitize c64 prodos images.")
  parser.add_argument("-s", action="store", dest="source", required=True, help="source file")
  parser.add_argument("-d", action="store", dest="dest", required=True, help="destination file")
  arguments = parser.parse_args()
  source_filename = arguments.source
  dest_filename = arguments.dest

  # check files
  if not os.path.exists(source_filename):
    raise FileNotFoundError(source_filename)

  #print("source: " + source_filename)
  with open(source_filename, "rb") as f:
    source_data = f.read()
    
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
  print(source_filename + "sanitized to " + dest_filename)
  

if __name__ == "__main__":
  main()

