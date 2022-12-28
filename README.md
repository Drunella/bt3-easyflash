# Bard's Tale III EasyFlash
This is the source to build an EasyFlash version from original C64 
Bard's Tale III disks.

## Features
* Import and export of save games
* Character editor
* No codewheel necessary

## Required Tools
To build you need the following:
* cc65
* cartconv from VICE
* c1541 from VICE
* Python 3.6 or greater
* GNU Make
* C compiler

## Building
To build Bard's Tale III create the folder `disks/` and place the
original disks as d64 in it. Name the disks `boot.d64`(Boot disk), 
`character.d64`, `dungeona.d64`, `dungeonb.d64`.

Then build with

```
make
```

Find the crt image in the build sub-directory:
`build/bt3-easyflash.crt`.


# Bugs

I did not test the game thoroughly. A lot of functionality is
unknown to me and therefore I do not know if I missed important
parts of code to convert.


# License and Copyright

The code is © 2023 Drunella, available under an Apache 2.0 license.

The original Bard's Tale III game is © 1988 Interplay Productions.
No copy of the original game is included in this repository.
