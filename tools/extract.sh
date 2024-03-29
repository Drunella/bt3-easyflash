#!/bin/bash

# $1 files.list
# $2 destination directory

echo "extracting files from prodos disk"

IFS=$'\n'       # make newlines the only separator
set -f          # disable globbing
for i in $(cat < "$1"); do
  build/prodos/prodos -i build/ef/boot.prodos cat $i > $2/$i.bin
  #echo "build/prodos/prodos -i build/source/boot.prodos cat $i > $2/$i.bin"
  echo "extracting $i to $2/$i.bin"
done
