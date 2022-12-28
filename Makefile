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
CA65FLAGS=-t $(TARGET) -I . -I build/ef --debug-info
CC65FLAGS=-t $(TARGET) -O
#LD65FLAGS=

.SUFFIXES: .prg .s .c
.PHONY: clean all easyflash mrproper

EF_LOADER_FILES=build/ef/loadeapi.o build/ef/io-loader.o build/ef/game-loader.o build/ef/io-sector.o build/ef/io-loadfile.o build/ef/io-caller.o 
STARTMENU_FILES=build/ef/menu.o build/ef/util.o build/ef/util_s.o build/ef/savegame.o build/ef/savegame_map.o build/ef/io-1541.o build/ef/io-sectortable-da.o
EDITOR_FILES=build/ef/util.o build/ef/util_s.o build/ef/editor_main.o build/ef/editor_character.o build/ef/editor_util.o build/ef/editor_items.o build/ef/editor_spells.o build/ef/io-1541.o build/ef/editor_list.o build/ef/io-sectortable-da.o
IMPORT_UTIL64_FILES=build/ef/util64-da.o build/ef/io-sectortable-da.o build/ef/util64-additional.o build/ef/io-1541.o

# all
all: easyflash

# easyflash
easyflash: build/bt3-easyflash.crt

# assemble
build/%.o: src/%.s
	@mkdir -p ./build/ef
	$(CA65) $(CA65FLAGS) -g -o $@ $<

# compile
build/%.s: src/%.c
	@mkdir -p ./build/ef
	$(CC65) $(CC65FLAGS) -g -o $@ $<

# assemble2
build/%.o: build/%.s
	@mkdir -p ./build/ef
	$(CA65) $(CA65FLAGS) -g -o $@ $<

clean:
	rm -rf build/ef
	rm -f build/bt3-easyflash.crt

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

# loader.prg
build/ef/loader.prg build/ef/loader.map: $(EF_LOADER_FILES)
	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/loader.map -Ln ./build/ef/loader.lst -o $@ -C src/ef/loader.cfg c64.lib $(EF_LOADER_FILES)

# sector-rom.prg
build/ef/sector-rom.bin: build/ef/io-sector.o
	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/sector-rom.map -Ln ./build/ef/sector-rom.lst -o $@ -C src/ef/sector-rom.cfg build/ef/io-sector.o

# startmenu.prg
build/ef/startmenu.prg: $(STARTMENU_FILES)
	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/startmenu.map -Ln ./build/ef/startmenu.lst -o $@ -C src/ef/startmenu.cfg c64.lib $(STARTMENU_FILES)

## savegame.prg
#build/ef/savegame.prg: $(SAVEGAME_FILES)
#	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/savegame.map -Ln ./build/ef/savegame.lst -o $@ -C src/ef/savegame.cfg c64.lib $(SAVEGAME_FILES)

# editor.prg
build/ef/editor.prg: $(EDITOR_FILES)
	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/editor.map -Ln ./build/ef/editor.lst -o $@ -C src/ef/editor.cfg c64.lib $(EDITOR_FILES)

# import-util64.prg
build/ef/import-util64.prg: $(IMPORT_UTIL64_FILES)
	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/import-util64.map -Ln ./build/ef/import-util64.lst -o $@ -C src/ef/import-util64.cfg c64.lib $(IMPORT_UTIL64_FILES)

# build image dir and data
build/ef/files.dir.bin build/ef/files.data.bin: src/ef/files.csv build/ef/files.list build/ef/startmenu.prg build/ef/editor.prg build/ef/track18.bin build/ef/track09-sector14.bin build/ef/1541-fastloader-recomp.bin build/ef/track01-sector00.bin build/ef/track01-sector11.bin build/ef/import-util64.prg build/ef/savegame-orig.bin
	cp src/ef/files.csv build/ef/files.csv
	tools/mkfiles.py -v -l build/ef/files.csv -f build/ef/ -o build/ef/files.data.bin -d build/ef/files.dir.bin

# cartridge binary
build/ef/bt3-easyflash.bin: build/ef/patched.done build/ef/init.prg build/ef/loader.prg src/ef/eapi-am29f040.prg build/ef/files.dir.bin build/ef/files.data.bin build/ef/character.bin build/ef/dungeona.bin build/ef/dungeonb.bin build/ef/sector-rom.bin
	cp ./src/ef/crt.map ./build/ef/crt.map
	cp ./src/ef/eapi-am29f040.prg ./build/ef/eapi-am29f040.prg
	cp ./src/ef/ef-name.bin ./build/ef/ef-name.bin
	tools/mkbin.py -v -b ./build/ef -m ./build/ef/crt.map -o ./build/ef/bt3-easyflash.bin

# cartdridge crt
build/bt3-easyflash.crt: build/ef/bt3-easyflash.bin
	cartconv -b -t easy -o build/bt3-easyflash.crt -i build/ef/bt3-easyflash.bin -n "Bard's Tale III" -p

# apply patches
build/ef/patched.done: build/ef/character.bin
	tools/bt3patch.py -f ./build/ef/ -v ./src/ef/*.patch
	touch ./build/ef/patched.done
	
# sanitized disks
build/ef/boot.prodos: disks/boot.d64
	@mkdir -p ./build/ef
	tools/sanitize.py -v -i 0 -s ./disks/boot.d64 -d ./build/ef/boot.prodos

build/ef/character.bin: disks/character.d64
	@mkdir -p ./build/ef
	tools/sanitize.py -v -i 1 -s ./disks/character.d64 -d ./build/ef/character.bin

build/ef/dungeona.bin: disks/dungeona.d64
	@mkdir -p ./build/ef
	tools/sanitize.py -v -i 2 -s ./disks/dungeona.d64 -d ./build/ef/dungeona.bin

build/ef/dungeonb.bin: disks/dungeonb.d64
	@mkdir -p ./build/ef
	tools/sanitize.py -v -i 3 -s ./disks/dungeonb.d64 -d ./build/ef/dungeonb.bin
	
# application files
build/ef/files.list: build/ef/boot.prodos build/prodos/prodos
	build/prodos/prodos -i ./build/ef/boot.prodos ls > build/ef/files.list
	tools/extract.sh build/ef/files.list build/ef

# copy track 18 of character disk
build/ef/track18.bin: disks/character.d64
	@mkdir -p ./build/ef
	dd if=disks/character.d64 of=build/ef/track18.bin bs=256 skip=357 count=19

# copy prodos sector 87.1 at track 9 sector 14, summed 168 + 14
# to track09-sector14.bin
build/ef/track09-sector14.bin: disks/character.d64
	@mkdir -p ./build/ef
	dd if=disks/character.d64 of=build/ef/track09-sector14.bin bs=256 count=1 skip=182

build/ef/track01-sector00.bin: disks/character.d64
	@mkdir -p ./build/ef
	dd if=disks/character.d64 of=build/ef/track01-sector00.bin bs=256 count=1 skip=0

build/ef/track01-sector11.bin: disks/character.d64
	@mkdir -p ./build/ef
	dd if=disks/character.d64 of=build/ef/track01-sector11.bin bs=256 count=1 skip=11

build/ef/savegame-orig.bin: build/ef/character.bin
	@mkdir -p ./build/ef
	dd if=build/ef/character.bin of=build/ef/savegame-orig.bin bs=512 count=13 skip=267


# get 2.0.prg
build/ef/2.0.prg: disks/boot.d64
	SDL_VIDEODRIVER=dummy c1541 -attach disks/boot.d64 -read 2.0 ./build/ef/2.0.prg

# disassemble of prodos 2.0
build/ef/io-sectortable-da.s: build/ef/files.list src/ef/io-sectortable-da.info src/ef/io-sectortable-exp.inc build/ef/2.0.prg src/ef/io-sectortable-patch.sh
	$(DA65) -i ./src/ef/io-sectortable-da.info -o build/ef/temp1.s
#	cat src/ef/io-sectortable-exp.inc build/ef/temp1.s > build/ef/io-sectortable-da.s
	src/ef/io-sectortable-patch.sh src/ef/io-sectortable-exp.inc build/ef/temp1.s > build/ef/io-sectortable-da.s
	rm -f build/ef/temp1.s


# disassemble of util64
build/ef/util64-da.s: build/ef/global.i src/ef/util64-da.info build/ef/2.0.prg src/ef/util64-patch.sh src/ef/util64-exp.inc
	$(DA65) -i ./src/ef/util64-da.info -o build/ef/temp2.s
	src/ef/util64-patch.sh src/ef/util64-exp.inc build/ef/temp2.s > build/ef/util64-da.s
	rm -f build/ef/temp2.s

# global addresses
build/ef/global.i: build/ef/loader.map
	tools/mkglobal.py -v -m ./build/ef/loader.map -o ./build/ef/global.i loadsave_sector_body


# fastloader 1541 part, track 18 sector 14-17 (371 sectors in)
build/ef/1541-fastloader.bin: disks/boot.d64
	@mkdir -p ./build/ef
	dd if=disks/boot.d64 of=build/ef/1541-fastloader.bin bs=256 count=4 skip=371

# disassemble of 1541 fastloader
build/ef/1541-fastloader-da.s: src/ef/1541-fastloader-da.info build/ef/1541-fastloader.bin src/ef/1541-fastloader-patch.sh
	$(DA65) -i ./src/ef/1541-fastloader-da.info -o build/ef/temp3.s
	src/ef/1541-fastloader-patch.sh build/ef/temp3.s > build/ef/1541-fastloader-da.s
	rm -f build/ef/temp3.s

# recompiled fastloader
build/ef/1541-fastloader-recomp.bin: build/ef/1541-fastloader-da.s
	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/1541-fastloader-recomp.map -Ln ./build/ef/1541-fastloader-recomp.lst -o $@ -C src/ef/1541-fastloader.cfg c64.lib $^
