##############################################################################
# configuration

# the CC65_HOME environment variable should be set.
# PATH should include $CC65_HOME/bin

# color schemes.
# uncomment one of these CFLAGS to enable a color scheme
#CFLAGS+=-DCOLOR_SCHEME_BLUE
#CFLAGS+=-DCOLOR_SCHEME_SX
#CFLAGS+=-DCOLOR_SCHEME_128

# C64 256K memory expansion
#REU=c64-c256k.emd
# Berkeley Softworks GeoRam
#REU=c64-georam.emd
# RamCart 64/128
#REU=c64-ramcart.emd
# CBM REU
#REU=c64-reu.emd
# C64 D2TV
#REU=dtv-himem.emd
# Kerberos MIDI interface
#REU=c64-kerberos.emd

# enable this flags to build DraCopy with built in support for the Kerberos MIDI interface SRAM.
#CFLAGS+=-DKERBEROS

##############################################################################
# building

VER=1.3doj

CFLAGS+=-I include -O -Or -Os -r -D DRA_VERNUM=\"$(VER)\" -D DRA_VERDATE=\"2022-05-07\"

ifneq ($(REU),)
C64CFLAGS+=-DREU=\"$(REU)\"
endif

C64CFLAGS+=$(CFLAGS)

D64=dracopy-$(VER).d64
TARGETS_C64=dc64 db64 dc6480 db6480 dc64ieee dc64ieee80 dc128 db128 dc1280 db1280
TARGETS=$(TARGETS_C64) dcp4 dbp4 dc510 db510 dc610 db610 dcpet80 dbpet80 dcpet40 dbpet40

all:	$(TARGETS) $(D64)

$(D64):	$(TARGETS) $(REU)
	sh d64.sh 'dracopy $(VER),dj' $(D64) $(REU) dc64 db64 dc6480 dc128 db128 dc1280 db1280 dcp4 dbp4

COMMON_SRC=src/screen.c src/cat.c src/dir.c src/base.c src/ops.c
DC_SRC=src/dc.c $(COMMON_SRC)

dc64:	$(DC_SRC)
	cl65 $(C64CFLAGS) -t c64 -m $@.map $^ -o $@.prg
	#pucrunch -c64 $@.prg $@
	exomizer sfx systrim -o $@ $@.prg

dc6480:	$(DC_SRC)
	cl65 $(C64CFLAGS) -DCHAR80 -t c64 -m $@.map $^ c64-soft80.o -o $@.prg
	#pucrunch -c64 $@.prg $@
	exomizer sfx systrim -o $@ $@.prg

dc64ieee:	$(DC_SRC)
	cl65 $(C64CFLAGS) -DSFD1001 -t c64 -C dc64ieee.cfg -m $@.map $^ -o $@.prg
	#pucrunch -c64 $@.prg $@
	exomizer sfx systrim -o $@ $@.prg

dc64ieee80:	$(DC_SRC)
	cl65 $(C64CFLAGS) -DSFD1001 -DCHAR80 -t c64 -C dc64ieee.cfg -m $@.map $^ c64-soft80.o -o $@.prg
	#pucrunch -c64 $@.prg $@
	exomizer sfx systrim -o $@ $@.prg

dc128:	$(DC_SRC)
	cl65 $(CFLAGS) -t c128 $^ -o $@.prg
	#pucrunch -fc128 $@.prg $@
	exomizer sfx systrim -t 128 -o $@ $@.prg

dc1280:	$(DC_SRC)
	cl65 $(CFLAGS) -t c128 -DCHAR80 $^ -o $@.prg
	#pucrunch -fc128 $@.prg $@
	exomizer sfx systrim -t 128 -o $@ $@.prg

dcp4:	$(DC_SRC)
	cl65 $(CFLAGS) -t plus4 $^ -o $@.prg
	#pucrunch -fc16 $@.prg $@
	exomizer sfx systrim -t 4 -o $@ $@.prg

dc510:	$(DC_SRC)
	cl65 $(CFLAGS) -t cbm510 -DSFD1001 $^ -o $@

dc610:	$(DC_SRC)
	cl65 $(CFLAGS) -t cbm610 -DSFD1001 -DCHAR80 $^ -o $@

dcpet40:	$(DC_SRC)
	cl65 $(CFLAGS) -t pet -DSFD1001 $^ -o $@.prg
	cp $@.prg $@
	#exomizer sfx systrim -t 4032 -o $@ $@.prg

dcpet80:	$(DC_SRC)
	cl65 $(CFLAGS) -t pet -DSFD1001 -DCHAR80 $^ -o $@.prg
	cp $@.prg $@
	#exomizer sfx systrim -t 4032 -o $@ $@.prg

DB_SRC=src/db.c $(COMMON_SRC)

db64:	$(DB_SRC)
	cl65 $(C64CFLAGS) -t c64 $^ -o $@.prg
	#pucrunch -c64 $@.prg $@
	exomizer sfx systrim -o $@ $@.prg

db6480:	$(DB_SRC)
	cl65 $(C64CFLAGS) -DCHAR80 -t c64 -m $@.map $^ c64-soft80.o -o $@.prg
	#pucrunch -c64 $@.prg $@
	exomizer sfx systrim -o $@ $@.prg

db128:	$(DB_SRC)
	cl65 $(CFLAGS) -t c128 $^ -o $@.prg
	#pucrunch -fc128 $@.prg $@
	exomizer sfx systrim -t 128 -o $@ $@.prg

db1280:	$(DB_SRC)
	cl65 $(CFLAGS) -t c128 -DCHAR80 $^ -o $@.prg
	#pucrunch -fc128 $@.prg $@
	exomizer sfx systrim -t 128 -o $@ $@.prg

dbp4:	$(DB_SRC)
	cl65 $(CFLAGS) -t plus4 $^ -o $@.prg
	#pucrunch -fc16 $@.prg $@
	exomizer sfx systrim -t 4 -o $@ $@.prg

db510:	$(DB_SRC)
	cl65 $(CFLAGS) -t cbm510 -DSFD1001 $^ -o $@

db610:	$(DB_SRC)
	cl65 $(CFLAGS) -t cbm610 -DSFD1001 -DCHAR80 $^ -o $@

dbpet40:	$(DB_SRC)
	cl65 $(CFLAGS) -t pet -DSFD1001 $^ -o $@.prg
	cp $@.prg $@
	#exomizer sfx systrim -t 4032 -o $@ $@.prg

dbpet80:	$(DB_SRC)
	cl65 $(CFLAGS) -t pet -DSFD1001 -DCHAR80 $^ -o $@.prg
	cp $@.prg $@
	#exomizer sfx systrim -t 4032 -o $@ $@.prg

clean:
	$(RM) -rf d src/*.o src/*.s *.prg *.map *.seq *.d64 *.d71 *.d8[012] $(TARGETS) *.zip *.emd
	find . -name '*~' -delete

zip:	dracopy-$(VER).zip

dracopy-$(VER).zip:	$(TARGETS) $(REU)
	zip -9 $@ $^ README.md

dc64.zip:	$(TARGETS_C64) $(REU)
	zip -9 $@ $^ README.md

test.prg:	src/test.c
	cl65 $(CFLAGS) -t pet $^ -o $@

$(REU):
	cp -f $(CC65_HOME)/target/c64/drv/emd/$(REU) $@

printversion:
	@echo $(VER)

##############################################################################
# testing

D64_9=9.d64
$(D64_9):	dc510 test.prg
	for i in `perl -e 'for(1..140){print "$$_ "}'` ; do echo $$i > $$i.seq ; done
	cp -f test.prg test
	dd if=/dev/zero of=x1 bs=2048 count=1
	sh d64.sh 'number nine,n9' $@ dc510 test README.md x1 *.seq
	$(RM) *.seq test
	dd if=/dev/urandom of=x1 bs=1 count=10000
	dd if=$@ of=x2 bs=1 skip=10000
	cat x1 x2 > $@
	rm x1 x2

D71=10.d71
$(D71):	dc128 dc1280 db128 db1280
	for i in `perl -e 'for(1..140){print "$$_ "}'` ; do echo $$i > $$i.seq ; done
	TYPE=d71 sh d64.sh 'test,71' $@ dc128 dc1280 db128 db1280 *.seq
	$(RM) *.seq

D71_2=11.d71
$(D71_2):
	for i in `perl -e 'for(1..140){print "$$_ "}'` ; do echo $$i > $$i.seq ; done
	TYPE=d71 sh d64.sh 'test2,71' $@ dc128 dc1280 *.seq
	$(RM) *.seq

D81=11.d81
$(D81):
	for i in `perl -e 'for(1..200){print "$$_ "}'` ; do echo $$i > $$i.seq ; done
	TYPE=d81 sh d64.sh 'test,81' $@ $(TARGETS) *.seq
	$(RM) *.seq

D81_2=11_2.d81
$(D81_2):
	c1541 -format 'test2,81' d81 $@
	for i in `perl -e 'for(1..200){print "$$_ "}'` ; do echo $$i > $$i.seq ; done
	TYPE=d81 sh d64.sh 'test2,81' $@ *.seq
	$(RM) *.seq

X64?=x64sc -autostartprgmode 1 -reu -reusize 2048 # -drive8truedrive -drive9truedrive -drive10truedrive -drive11truedrive
x64:	$(D64) $(D81) $(D81_2) $(D64_9)
	#$(X64) -autostart dc64.prg -drive8type 1541 -8 $(D64) -drive9type 1541 -9 $(D64_9) -drive10type 1581 -10 $(D81_2) -drive11type 1581 -11 $(D81)
	$(X64) -autostart dc64.prg -drive8type 1541 -8 $(D64) -drive9type 1541 -iecdevice9 -device9 1 -fs9 $(PWD) -drive10type 1581 -10 $(D81_2) -drive11type 1581 -11 $(D81)

x6480:	$(D64) $(D81) $(D81_2) $(D64_9)
	$(X64) -autostart dc6480.prg -drive8type 1541 -8 $(D64) -drive9type 1541 -iecdevice9 -device9 1 -fs9 $(PWD) -drive10type 1581 -10 $(D81_2) -drive11type 1581 -11 $(D81)

D82=8.d82
$(D82):	dc64ieee dcpet80 dcpet40
	TYPE=d82 sh d64.sh 'dc64ieee,dc' $@ $^

D82_2=8_2.d82
$(D82_2):	dc64ieee80 dbpet80 dbpet40
	TYPE=d82 sh d64.sh 'dc64ieee80,dc' $@ $^

x64ieee:	$(D82) $(D82_2)
	$(X64) -autostart dc64ieee -ieee488 -cartieee images/ieee-488-cartridge.bin -drive8type 1001 -8 $(D82) -drive9type 1001 -9 $(D82_2)

x64ieee80:	$(D82) $(D82_2)
	$(X64) -autostart dc64ieee80 -ieee488 -cartieee images/ieee-488-cartridge.bin -drive8type 1001 -8 $(D82) -drive9type 1001 -9 $(D82_2)

x64_71:	dc64 $(D71) $(D71_2)
	$(X64) -autostart dc64 -drive8type 1571 -8 $(D71) -drive9type 1571 -9 $(D71_2)

x64_81:	dc64 $(D81) $(D81_2)
	$(X64) -autostart dc64 -drive8type 1581 -8 $(D81) -drive9type 1581 -9 $(D81_2)

X128?=x128  -drive8truedrive -drive9truedrive -drive10truedrive -drive11truedrive # -autostartprgmode 1
x128:	$(D71) $(D71_2) $(D64) $(D64_9)
	$(X128) -autostart dc128 -drive8type 1541 -8 $(D64) -drive9type 1542 -9 $(D64_9) -drive10type 1571 -10 $(D71) -drive11type 1571 -11 $(D71_2)

x1280:	$(D71) $(D71_2)
	#$(X128) -80col -autostart dc1280 -drive8type 1571 -8 $(D71) -drive9type 1571 -9 $(D71_2) -drive10type 1581 -10 $(D81_2) -drive11type 1581 -11 $(D81)
	$(X128) -80col -autostart dc1280 -drive8type 1571 -8 $(D71) -drive9type 1541 -iecdevice9 -device9 1 -fs9 $(PWD) -drive10type 1581 -10 $(D81_2) -drive11type 1581 -11 $(D81)

XPLUS4?=xplus4 -autostartprgmode 1 -autostart dcp4 -drive8truedrive -drive9truedrive -drive10truedrive -drive11truedrive
xplus4:	dcp4 $(D64_9)
	$(XPLUS4) -drive8type 1551 -8 $(D64) -drive9type 1551 -9 $(D64_9)

D64PET=pet.d64
$(D64PET):	dbpet80 dcpet80 dbpet40 dcpet40 test.prg
	cp -f test.prg test
	sh d64.sh 'dbpet,dc' $@ $^ test
	rm test

D80PET=pet.d80
$(D80PET):	dcpet80 dbpet80 dbpet40 dcpet40
	TYPE=d80 sh d64.sh 'd80,dc' $@ $^

XPET?=xpet -autostartprgmode 1 -drive8truedrive -drive9truedrive -drive10truedrive -drive11truedrive
xpet:	$(D64PET) $(D82) $(D64) $(D82_2)
	#$(XPET) -model 3032B -ramsize 32 -petram9 -petramA -autostart dcpet40 -drive8type 3040 -8 $(D64PET) -drive10type 3040 -10 $(D64) -CRTCdsize
	#$(XPET) -model 4032 -ramsize 32 -petram9 -petramA -autostart dcpet40 -drive8type 3040 -8 $(D64PET) -drive10type 3040 -10 $(D64) -CRTCdsize
	#$(XPET) -model 8032 -ramsize 32 -petram9 -petramA -autostart dcpet80 -drive8type 4040 -8 $(D64PET) -drive10type 4040 -10 $(D64)
	$(XPET) -model 8096 -ramsize 96 -petram9 -petramA -autostart dcpet80 -drive8type 8250 -8 $(D82) -drive10type 8250 -10 $(D82_2)
	#$(XPET) -model 8296 -ramsize 32 -petram9 -petramA -autostart dcpet80 -drive8type 1001 -8 $(D82) -drive10type 1001 -10 $(D82_2)

D80CBM2=cbm2.d64
$(D80CBM2):	dc610 db610
	TYPE=d80 sh d64.sh 'cbm2,dc' $@ $^

XCBM2?=xcbm2 -autostartprgmode 1 -autostart dc610 -drive8truedrive -drive9truedrive -drive10truedrive -drive11truedrive
xcbm2:	$(D80CBM2) $(D82) $(D80PET) $(D82_2)
	$(XCBM2) -drive8type 8050 -8 $(D80CBM2) -drive9type 8050 -9 $(D80PET) -drive10type 1001 -10 $(D82) -drive11type 1001 -11 $(D82_2)

XCBM510?=xcbm5x0 -autostartprgmode 1 -autostart dc510 -drive8truedrive -drive9truedrive -drive10truedrive -drive11truedrive
xcbm510:	$(D64) $(D64_9)
	$(XCBM510) -drive8type 2031 -8 $(D64_9) -drive9type 2031 -9 $(D64)
