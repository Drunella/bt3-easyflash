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

# Settings
TARGET=c64
LD65=cl65
CA65=ca65
CC65=cc65
DA65=da65
#LD65=ld65
LD65FLAGS=-t $(TARGET)
CA65FLAGS=-t $(TARGET) -I . --debug-info
CC65FLAGS=-t $(TARGET) -O
#LD65FLAGS=
#export LD65_LIB=/opt/cc65/share/cc65/lib

.SUFFIXES: .prg .s .c
.PHONY: clean subdirs all easyflash mrproper

#EF_LOADER_FILES=build/ef/menu.o build/ef/util.o build/ef/loadeapi.o build/ef/io-loader.o build/ef/game-loader.o build/ef/io-sector.o build/ef/io-loadfile.o build/ef/io-caller.o build/ef/util_s.o build/ef/savegame.o build/ef/savegame_map.o
EF_LOADER_FILES=build/ef/loadeapi.o build/ef/io-loader.o build/ef/game-loader.o build/ef/io-sector.o build/ef/io-loadfile.o build/ef/io-caller.o
STARTMENU_FILES=build/ef/menu.o build/ef/util.o build/ef/util_s.o build/ef/savegame.o build/ef/savegame_map.o
EDITOR_FILES=build/ef/util.o build/ef/util_s.o build/ef/editor_main.o build/ef/editor_character.o build/ef/editor_util.o build/ef/editor_items.o build/ef/editor_spells.o

# all
all: easyflash

# easyflash
easyflash: subdirs build/bd3-easyflash.crt

# assemble
build/%.o: src/%.s
	$(CA65) $(CA65FLAGS) -g -o $@ $<

# compile
build/%.s: src/%.c
	$(CC65) $(CC65FLAGS) -g -o $@ $<

# assemble2
build/%.o: build/%.s
	$(CA65) $(CA65FLAGS) -g -o $@ $<

subdirs:
	@mkdir -p ./build/temp 
	@mkdir -p ./build/ef

clean:
	rm -rf build/ef
	rm -rf build/temp
	rm -f build/bd3-easyflash.crt

mrproper:
	rm -rf build

# external tool
build/prodos/prodos:
	git clone https://github.com/vandry/prodos ./build/prodos
	$(MAKE) -C ./build/prodos


# ------------------------------------------------------------------------
# easyflash

# easyflash init.prg
build/ef/init.prg: build/ef/init.o
	$(LD65) $(LD65FLAGS) -o $@ -C src/ef/init.cfg $^

# easyflash loader.prg
build/ef/loader.prg: $(EF_LOADER_FILES) subdirs
	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/loader.map -Ln ./build/ef/loader.info -o $@ -C src/ef/loader.cfg c64.lib $(EF_LOADER_FILES)

# sector-rom.prg
build/ef/sector-rom.bin: build/ef/io-sector.o subdirs
	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/sector-rom.map -Ln ./build/ef/sector-rom.info -o $@ -C src/ef/sector-rom.cfg build/ef/io-sector.o

# startmenu.prg subdirs
build/ef/startmenu.prg: subdirs $(STARTMENU_FILES)
	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/startmenu.map -Ln ./build/ef/startmenu.info -o $@ -C src/ef/startmenu.cfg c64.lib $(STARTMENU_FILES)

## savegame.prg subdirs
#build/ef/savegame.prg: subdirs $(SAVEGAME_FILES)
#	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/savegame.map -Ln ./build/ef/savegame.info -o $@ -C src/ef/savegame.cfg c64.lib $(SAVEGAME_FILES)

# editor.prg subdirs
build/ef/editor.prg: subdirs $(EDITOR_FILES)
	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/editor.map -Ln ./build/ef/editor.info -o $@ -C src/ef/editor.cfg c64.lib $(EDITOR_FILES)

# build image dir and data
build/ef/files.dir.bin build/ef/files.data.bin: build/ef/files.list build/ef/startmenu.prg build/ef/editor.prg build/ef/track18.bin build/ef/track09-sector14.bin build/ef/1541-fastloader.bin
	cp src/ef/files.csv build/ef/files.csv
	tools/mkfiles.py -v -l build/ef/files.csv -f build/ef/ -o build/ef/files.data.bin -d build/ef/files.dir.bin

# cartridge binary
build/ef/bd3-easyflash.bin: build/ef/patched.done build/ef/init.prg build/ef/loader.prg src/ef/eapi-am29f040.prg build/ef/files.dir.bin build/ef/files.data.bin build/ef/character.bin build/ef/dungeona.bin build/ef/dungeonb.bin build/ef/sector-rom.bin
	cp ./src/ef/crt.map ./build/ef/crt.map
	cp ./src/ef/eapi-am29f040.prg ./build/ef/eapi-am29f040.prg
	tools/mkbin.py -v -b ./build/ef -m ./build/ef/crt.map -o ./build/ef/bd3-easyflash.bin

# cartdridge crt
build/bd3-easyflash.crt: build/ef/bd3-easyflash.bin
	cartconv -b -t easy -o build/bd3-easyflash.crt -i build/ef/bd3-easyflash.bin -n "Bard's Tale III" -p

# apply patches
build/ef/patched.done: build/ef/character.bin
	tools/bd3patch.py -f ./build/ef/ -v ./src/ef/*.patch
	touch ./build/ef/patched.done
	
# sanitized disks
build/ef/boot.prodos: subdirs
	tools/sanitize.py -v -i 0 -s ./disks/boot.d64 -d ./build/ef/boot.prodos

build/ef/character.bin: subdirs
	tools/sanitize.py -v -i 1 -s ./disks/character.d64 -d ./build/ef/character.bin

build/ef/dungeona.bin: subdirs
	tools/sanitize.py -v -i 2 -s ./disks/dungeona.d64 -d ./build/ef/dungeona.bin

build/ef/dungeonb.bin: subdirs
	tools/sanitize.py -v -i 3 -s ./disks/dungeonb.d64 -d ./build/ef/dungeonb.bin
	
# application files
build/ef/files.list: subdirs build/ef/boot.prodos build/prodos/prodos
	build/prodos/prodos -i ./build/ef/boot.prodos ls > build/ef/files.list
	tools/extract.sh build/ef/files.list build/ef

# copy track 18 of character disk
build/ef/track18.bin: subdirs
	dd if=disks/character.d64 of=build/ef/track18.bin bs=256 skip=357 count=19

# copy prodos sector 87.1 at track 9 sector 14, summed 168 + 14
# to track09-sector14.bin
build/ef/track09-sector14.bin: subdirs
	dd if=disks/character.d64 of=build/ef/track09-sector14.bin bs=256 count=1 skip=182

# fastloader 1541 part, track 18 sector 14-17 (371 sectors in)
build/ef/1541-fastloader.bin: subdirs
	dd if=disks/boot.d64 of=build/ef/1541-fastloader.bin bs=256 count=4 skip=371
