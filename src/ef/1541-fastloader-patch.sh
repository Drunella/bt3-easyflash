#!/bin/bash
#
# $@ source files

cat $@ | \
    sed 's/^.*patch_line_replace_1/cmp #$04\nbne :+\njmp $eaa0\n:\n/'

