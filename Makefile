##############################################################################
# building

D64=dracopy10doj.d64
TARGETS=dc64 db64 dc128 db128 dc1280 db1280 dcp4 dbp4 dbpet8 db610 dc610

all:	$(TARGETS)
	sh d64.sh 'dracopy 1.0doj,dc' $(D64) $^

COMMON_SRC=src/screen.c src/cat.c src/dir.c src/base.c src/ops.c
DC_SRC=src/dc.c $(COMMON_SRC)

dc64:	$(DC_SRC)
	cl65 -I include -t c64 $^ -o $@.prg
	-pucrunch -c64 $@.prg $@

dc128:	$(DC_SRC)
	cl65 -I include -t c128 $^ -o $@.prg
	-pucrunch -fc128 $@.prg $@

dc1280:	$(DC_SRC)
	cl65 -I include -t c128 -DCHAR80 $^ -o $@.prg
	-pucrunch -fc128 $@.prg $@

dcp4:	$(DC_SRC)
	cl65 -I include -t plus4 $^ -o $@.prg
	-pucrunch -fc16 $@.prg $@

dc610:	$(DC_SRC)
	cl65 -I include -t cbm610 -DCHAR80 -DNOCOLOR $^ -o $@

# code too big
dcpet8:	$(DC_SRC)
	cl65 -I include -t pet -DCHAR80 -DNOCOLOR $^ -o $@

DB_SRC=src/db.c $(COMMON_SRC)

db64:	$(DB_SRC)
	cl65 -I include -t c64 $^ -o $@.prg
	-pucrunch -c64 $@.prg $@

db128:	$(DB_SRC)
	cl65 -I include -t c128 $^ -o $@.prg
	-pucrunch -fc128 $@.prg $@

db1280:	$(DB_SRC)
	cl65 -I include -t c128 -DCHAR80 $^ -o $@.prg
	-pucrunch -fc128 $@.prg $@

dbp4:	$(DB_SRC)
	cl65 -I include -t plus4 $^ -o $@.prg
	-pucrunch -fc16 $@.prg $@

db610:	$(DB_SRC)
	cl65 -I include -t cbm610 -DCHAR80 -DNOCOLOR $^ -o $@

dbpet8:	$(DB_SRC)
	cl65 -I include -t pet -DCHAR80 -DNOCOLOR $^ -o $@

clean:
	$(RM) -rf d src/*.o src/*.s *.prg *.seq *.d64 *.d71 *.d81 \
	dc64 dc128 dc1280 dcpet8 dcp4 dc610 \
	db64 db128 db1280 dbpet8 dbp4 db610
	find . -name '*~' -delete

##############################################################################
# testing

D64_9=9.d64
$(D64_9):
	for i in `perl -e 'for(1..143){print "$$_ "}'` ; do echo $$i > $$i.seq ; done
	sh d64.sh 'number nine,n9' $@ README.md *.seq
	$(RM) *.seq

D71=10.d71
$(D71):	dc128 dc1280
	for i in `perl -e 'for(1..66){print "$$_ "}'` ; do echo $$i > $$i.seq ; done
	TYPE=d71 sh d64.sh 'test,71' $@ *.seq dc128.prg dc1280.prg
	$(RM) *.seq

D71_2=10_2.d71
$(D71_2):
	c1541 -format 'test2,71' d71 $@

D81=11.d81
$(D81):
	for i in `perl -e 'for(1..66){print "$$_ "}'` ; do echo $$i > $$i.seq ; done
	TYPE=d81 sh d64.sh 'test,81' $@ *.seq
	$(RM) *.seq

D81_2=11_2.d81
$(D81_2):
	c1541 -format 'test2,81' d81 $@

X64?=x64sc
x64:	all $(D64_9) $(D71) $(D81)
#	$(X64) -autostart dc64.prg -drive8type 1541 -8 $(D64) -drive9type 1542 -9 $(D64_9) -drive10type 1571 -10 $(D71) -drive11type 1581 -11 $(D81)
#	$(X64) -autostart dc64.prg -drive8type 1541 -8 $(D64) -drive9type 1542 -9 $(D64_9) -drive10type 0 -drive11type 0
	$(X64) -autostart dc64.prg -drive8type 1541 -8 $(D64) -drive9type 0 -drive10type 0 -drive11type 0

x64_71:	all $(D71) $(D71_2)
	$(X64) -autostart dc64.prg -drive8type 1571 -8 $(D71) -drive9type 1571 -9 $(D71_2)

x64_81:	all $(D81) $(D81_2)
	$(X64) -autostart dc64.prg -drive8type 1581 -8 $(D81) -drive9type 1581 -9 $(D81_2)

X128?=x128
x128:	all $(D71) $(D71_2) $(D64) $(D64_9)
	$(X128) -autostart dc128.prg -drive8type 1541 -8 $(D64) -drive9type 1542 -9 $(D64_9) -drive10type 1570 -10 $(D64) -drive11type 1570 -11 $(D64_9)

x1280:	all $(D64_9)
	$(X128) -80col -autostart dc1280.prg -drive8type 1571 -8 $(D71) -drive9type 1571 -9 $(D71_2)

XPLUS4?=xplus4
xplus4:	all $(D64_9)
	$(XPLUS4) -autostart dcp4.prg -drive8type 1551 -8 $(D64) -drive9type 1551 -9 $(D64_9)

XPET?=xpet
xpet:	all $(D64_9)
	$(XPET) -autostart dbpet8.prg -8 $(D64) -9 $(D64_9)
