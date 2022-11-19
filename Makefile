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
export LD65_LIB=/opt/cc65/share/cc65/lib

.SUFFIXES: .prg .s .c
.PHONY: clean subdirs all easyflash mrproper

EF_LOADER_FILES=build/ef/menu.o build/ef/menu_util.o build/ef/loadeapi.o build/ef/loadfile.o
#build/ef/loader.o
#build/ef/io-data.o
#build/ef/io-rw.o
#build/ef/io-code.o
#build/exo/exodecrunch.o
#build/ef/menu_savegame.o
#build/ef/menu_backup.o
#build/ef/music-base.o
#build/ef/music-disassemble.o


# all
all: easyflash

# easyflash
easyflash: subdirs build/bd3-easyflash.crt

# d81
#d81: subdirs build/u5remastered.d81

# backbit
#backbit: subdirs build/u5remastered-BackBit.d81

# assemble
build/%.o: src/%.s
	$(CA65) $(CA65FLAGS) -o $@ $<

# compile
build/%.s: src/%.c
	$(CC65) $(CC65FLAGS) -o $@ $<

# assemble2
build/%.o: build/%.s
	$(CA65) $(CA65FLAGS) -o $@ $<


# ------------------------------------------------------------------------
# easyflash

# exomizer for ef
#build/ef/exodecrunch.prg: build/exo/exodecrunch.o build/ef/io-rw.o build/ef/io-data.o
#	$(LD65) $(LD65FLAGS) -o $@ -C src/ef/exodecrunch.cfg $^

# easyflash init.prg
build/ef/init.prg: build/ef/init.o
	$(LD65) $(LD65FLAGS) -o $@ -C src/ef/init.cfg $^

# easyflash loader.prg
build/ef/loader.prg: $(EF_LOADER_FILES)
	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/loader.map -o $@ -C src/ef/loader.cfg c64.lib $(EF_LOADER_FILES)

# music
#build/ef/music.prg build/ef.f/music_rom.bin build/ef/music.map: $(EF_MUSIC_FILES)
#	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/music.map -o build/ef/music.prg -C src/ef/music.cfg $(EF_MUSIC_FILES)

# io-replacement
#build/ef/io-replacement.prg build/ef/io-replacement.map: build/ef/io-code.o build/ef/io-data.o build/ef/io-rw.o build/exo/exodecrunch.o
#	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/io-replacement.map -o build/ef/io-replacement.prg -C ./src/ef/io-replacement.cfg $^

# io-addendum
#build/ef/io-addendum.prg: build/ef/io-code.o build/ef/io-data.o build/ef/io-rw.o build/exo/exodecrunch.o
#	$(LD65) $(LD65FLAGS) -o $@ -C ./src/ef/io-addendum.cfg $^

# transfer-load
#build/ef/transfer-load.prg build/ef/transfer-load.map: build/ef/transfer-load.o
#	$(LD65) $(LD65FLAGS) -vm -m ./build/ef/transfer-load.map -o $@ -C ./src/ef/transfer-load.cfg $^

# disassemble m.prg 
#build/ef/music-disassemble.o: build/source/m.prg src/ef/music-disassemble.info ./src/ef/music-export.i
#	$(DA65) -i ./src/ef/music-disassemble.info -o ./build/temp/music-disassemble.s
#	cat ./src/ef/music-export.i >> ./build/temp/music-disassemble.s
#	$(CA65) $(CA65FLAGS) -o ./build/ef/music-disassemble.o ./build/temp/music-disassemble.s

# files with additional items
#build/ef.f/files.list: build/source/files.list build/ef/io-addendum.prg build/ef/music.prg
#	cp ./build/source/* ./build/ef.f/
#	cp build/ef/io-addendum.prg build/ef.f/io.add.prg
#	cp build/ef/music.prg build/ef.f/music.prg
#	echo "0x41/io.add io.add" >> build/ef.f/files.list
#	echo "0x41/music music" >> build/ef.f/files.list
	
# patch
#build/ef.f/patched.done: build/ef.f/files.list build/ef/io-replacement.map build/ef/transfer-load.map build/ef/music.map build/ef/transfer-load.prg build/ef.f/music_rom.bin
#	tools/u5patch.py -v -l ./build/ef.f/files.list -f ./build/ef.f -m build/ef/io-replacement.map -m build/ef/transfer-load.map -m build/ef/music.map ./patches/ef/*.patch ./patches/*.patch
#	cp build/ef.f/music_rom.bin build/ef/music_rom.aprg
#	touch ./build/ef.f/patched.done
	 
# crunch
#build/ef.f/crunched.done: build/ef.f/patched.done
#	tools/crunch.py -v -t level -b ./build/ef.f
#	touch build/ef.f/crunched.done

# build image dir and data
build/ef/files.dir.bin build/ef/files.data.bin: build/source/files.list
	cp src/ef/files.csv build/ef/files.csv
	tools/mkfiles.py -v -l build/ef/files.csv -f build/ef.f/ -o build/ef/files.data.bin -d build/ef/files.dir.bin -s 131072

# build blocks
#build/ef/crt.blocks.map: build/ef.f/files.list
#	tools/mkblocks.py -v -o ./src/disks.cfg -b ./src/ef/block.map -f ./build/ef.f -m ./build/ef/crt.blocks.map -d ./build/ef

# cartridge binary
build/ef/bd3-easyflash.bin: build/ef/init.prg build/ef/loader.prg src/ef/eapi-am29f040.prg build/ef/files.dir.bin build/ef/files.data.bin build/source/character.prodos build/source/dungeona.prodos build/source/dungeonb.prodos
	cp ./src/ef/crt.map ./build/ef/crt.map
	cp ./src/ef/eapi-am29f040.prg ./build/ef/eapi-am29f040.prg
	cp ./build/source/character.prodos build/ef/character.bin
	cp ./build/source/dungeona.prodos build/ef/dungeona.bin
	cp ./build/source/dungeonb.prodos build/ef/dungeonb.bin
	tools/mkbin.py -v -b ./build/ef -m ./build/ef/crt.map -o ./build/ef/bd3-easyflash.bin

# cartdridge crt
build/bd3-easyflash.crt: build/ef/bd3-easyflash.bin
	cartconv -b -t easy -o build/bd3-easyflash.crt -i build/ef/bd3-easyflash.bin -n "Bard's Tale III" -p


# ------------------------------------------------------------------------
# d81

subdirs:
	@mkdir -p ./build/temp 
	@mkdir -p ./build/source
	@mkdir -p ./build/ef.f
	@mkdir -p ./build/ef

clean:
	rm -rf build/ef
	rm -rf build/ef.f 
	rm -rf build/temp
	rm -rf build/source
	rm -f build/bd3-easyflash.crt

mrproper:
	rm -rf build


# external tool
build/prodos/prodos:
	git clone https://github.com/vandry/prodos ./build/prodos
	$(MAKE) -C ./build/prodos
	
# sanitized disks
build/source/boot.prodos: subdirs
	tools/sanitize.py -s ./disks/boot.d64 -d ./build/source/boot.prodos

build/source/character.prodos: subdirs
	tools/sanitize.py -s ./disks/character.d64 -d ./build/source/character.prodos

build/source/dungeona.prodos: subdirs
	tools/sanitize.py -s ./disks/dungeona.d64 -d ./build/source/dungeona.prodos

build/source/dungeonb.prodos: subdirs
	tools/sanitize.py -s ./disks/dungeonb.d64 -d ./build/source/dungeonb.prodos
	
# application files
build/source/files.list: subdirs build/source/boot.prodos build/prodos/prodos
	build/prodos/prodos -i ./build/source/boot.prodos ls > build/source/files.list
	tools/extract.sh build/source/files.list build/ef.f

# get m.prg	
#build/source/m.prg:
#	c1541 ./disks/osi.d64 -read m ./build/source/m.prg

# get meow.prg	
#build/source/meow.prg:
#	c1541 ./disks/osi.d64 -read meow ./build/source/meow.prg

# get subs.128.prg	
#build/source/subs.128.prg:
#	c1541 ./disks/osi.d64 -read subs.128 ./build/source/subs.128.prg

# get temp.subs.prg	
#build/source/temp.subs.prg:
#	c1541 ./disks/osi.d64 -read temp.subs ./build/source/temp.subs.prg
