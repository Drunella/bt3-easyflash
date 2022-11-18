# Bard's Tale III EasyFlash
This is the source to build an EasyFlash version from original C64 
Bard's Tale III disks.

## Features
* Import and export save games

## Required Tools
To build you need the following:
* cc65
* cartconv from VICE
* Python 3.6 or greater
* GNU Make

## Building
To build Bard's Tale III create the folder `disks/` and place the
original disks in it. Name the disks `boot.d64`(Boot disk), `character.d64`, 
`dungeona.d64`, `dungeonb.d64`.

Then build with

```
make
```

Find the crt image in the build sub-directory:
`build/bd3-easyflash.crt`.


# Bugs

I wanted to have a working version with music as fast as possible. I did not
test it thoroughly.
