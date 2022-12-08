#!/bin/bash
#
# $@ source files

cat $@ | \
    sed 's/^.*patch_line_remove//' | \
    sed 's/^.*patch_line_replace_1/jmp menu_transfer_28df/' | \
    sed 's/^.*patch_line_replace_2/jsr util64_rw_ef_sector/'

