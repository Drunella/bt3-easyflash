#!/bin/bash
#
# $@ source files

cat $@ | \
    sed 's/^.*patch_line_replace_1/lda #>data_table_c100/'

