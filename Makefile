##############################################################################
# building

CFLAGS=-I include -O -Or -Os -r

D64=dracopy10doj.d64
TARGETS=dc64 db64 dc128 db128 dc1280 db1280 dcp4 dbp4 db610 dc610 dbpet8 dcpet8

all:	$(TARGETS)
	sh d64.sh 'dracopy 1.0doj,dj' $(D64) dc64 db64 dc128 db128 dc1280 db1280 dcp4 dbp4

COMMON_SRC=src/screen.c src/cat.c src/dir.c src/base.c src/ops.c
DC_SRC=src/dc.c $(COMMON_SRC)

dc64:	$(DC_SRC)
	cl65 $(CFLAGS) -t c64 -m $@.map $^ -o $@.prg
	-pucrunch -c64 $@.prg $@

dc128:	$(DC_SRC)
	cl65 $(CFLAGS) -t c128 $^ -o $@.prg
	-pucrunch -fc128 $@.prg $@

dc1280:	$(DC_SRC)
	cl65 $(CFLAGS) -t c128 -DCHAR80 $^ -o $@.prg
	-pucrunch -fc128 $@.prg $@

dcp4:	$(DC_SRC)
	cl65 $(CFLAGS) -t plus4 $^ -o $@.prg
	-pucrunch -fc16 $@.prg $@

dc610:	$(DC_SRC)
	cl65 $(CFLAGS) -t cbm610 -DCHAR80 -DNOCOLOR $^ -o $@

# code too big
dcpet8:	$(DC_SRC)
	cl65 $(CFLAGS) -t pet -DCHAR80 -DNOCOLOR $^ -o $@

DB_SRC=src/db.c $(COMMON_SRC)

db64:	$(DB_SRC)
	cl65 $(CFLAGS) -t c64 $^ -o $@.prg
	-pucrunch -c64 $@.prg $@

db128:	$(DB_SRC)
	cl65 $(CFLAGS) -t c128 $^ -o $@.prg
	-pucrunch -fc128 $@.prg $@

db1280:	$(DB_SRC)
	cl65 $(CFLAGS) -t c128 -DCHAR80 $^ -o $@.prg
	-pucrunch -fc128 $@.prg $@

dbp4:	$(DB_SRC)
	cl65 $(CFLAGS) -t plus4 $^ -o $@.prg
	-pucrunch -fc16 $@.prg $@

db610:	$(DB_SRC)
	cl65 $(CFLAGS) -t cbm610 -DCHAR80 -DNOCOLOR $^ -o $@

dbpet8:	$(DB_SRC)
	cl65 $(CFLAGS) -t pet -DCHAR80 -DNOCOLOR $^ -o $@

clean:
	$(RM) -rf d src/*.o src/*.s *.prg *.map *.seq *.d64 *.d71 *.d81 $(TARGETS) dracopy-1.0doj.zip
	find . -name '*~' -delete

zip:	dracopy-1.0doj.zip
	[ -d ../doj/c64/ ] && cp $< ../doj/c64/

dracopy-1.0doj.zip:	$(TARGETS)
	zip -9 $@ $^ README.md

##############################################################################
# testing

D64_9=9.d64
$(D64_9):
	for i in `perl -e 'for(1..143){print "$$_ "}'` ; do echo $$i > $$i.seq ; done
	sh d64.sh 'number nine,n9' $@ README.md *.seq
	$(RM) *.seq

D64PET=pet.d64
$(D64PET):
	sh d64.sh 'pet,aa' $@ dbpet8 dcpet8 README.md
	$(RM) *.seq

D71=10.d71
$(D71):	dc128 dc1280
	for i in `perl -e 'for(1..140){print "$$_ "}'` ; do echo $$i > $$i.seq ; done
	TYPE=d71 sh d64.sh 'test,71' $@ dc128 dc1280 *.seq
	$(RM) *.seq

D71_2=10_2.d71
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

X64?=x64sc
x64:	all $(D64_9) $(D71) $(D81) $(D81_2)
#	$(X64) -autostart dc64.prg -drive8type 1541 -8 $(D64) -drive9type 1542 -9 $(D64_9) -drive10type 1571 -10 $(D71) -drive11type 1581 -11 $(D81)
#	$(X64) -autostart dc64.prg -drive8type 1541 -8 $(D64) -drive9type 1542 -9 $(D64_9) -drive10type 0 -drive11type 0
#	$(X64) -autostart dc64.prg -drive8type 1541 -8 $(D64) -drive9type 0 -drive10type 0 -drive11type 0
	$(X64) -autostart dc64.prg -drive8type 1541 -8 $(D64) -drive9type 1542 -9 $(D64_9) -drive10type 1581 -10 $(D81_2) -drive11type 1581 -11 $(D81)

x64_71:	all $(D71) $(D71_2)
	$(X64) -autostart dc64.prg -drive8type 1571 -8 $(D71) -drive9type 1571 -9 $(D71_2)

x64_81:	all $(D81) $(D81_2)
	$(X64) -autostart dc64.prg -drive8type 1581 -8 $(D81) -drive9type 1581 -9 $(D81_2)

X128?=x128
x128:	all $(D71) $(D71_2) $(D64) $(D64_9)
	$(X128) -autostart dc128 -drive8type 1541 -8 $(D64) -drive9type 1542 -9 $(D64_9) -drive10type 1571 -10 $(D71) -drive11type 1571 -11 $(D71_2)

x1280:	all $(D71) $(D71_2)
	$(X128) -80col -autostart dc1280 -drive8type 1571 -8 $(D71) -drive9type 1571 -9 $(D71_2)

XPLUS4?=xplus4
xplus4:	all $(D64_9)
	$(XPLUS4) -autostart dcp4.prg -drive8type 1551 -8 $(D64) -drive9type 1551 -9 $(D64_9)

XPET?=xpet
xpet:	all $(D64_9) $(D64PET)
	$(XPET) -autostart dcpet8 -8 $(D64PET) -9 $(D64_9)
