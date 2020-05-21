#src/%.o: src/%.c include/%.h
#	cc65 -O -I include -t c64 $<
#	ca65 -I include -t c64 src/$$(basename $< .c).s

D64=dracopy10doj.d64
all: dc64 dc128 dc1280 dcp4 dcpet8 db64 db128 db1280 dbp4 dbpet8 db610 dc610
	$(RM) $(D64)
	c1541 -format 'dracopy 1.0doj,dc' d64 $(D64)
	for i in $^ ; do c1541 -attach $(D64) -write $$i ; done

D64_9=9.d64

$(D64_9):
	c1541 -format 'number nine,dc' d64 $@
	for i in `perl -e 'for(1..60){print "$$_ "}'` ; do echo $$i > $$i.seq ; c1541 -attach $(D64_9) -write $$i.seq ; done
	$(RM) *.seq

X64?=x64sc
x64:	all $(D64_9)
	$(X64) -8 $(D64) -autostart dc64.prg -9 $(D64_9)

x128:	all $(D64_9)
	$@ -8 $(D64) -autostart dc128.prg -9 $(D64_9)

xplus4:	all $(D64_9)
	$@ -8 $(D64) -autostart dcp4.prg -9 $(D64_9)

xpet:	all $(D64_9)
	$@ -8 $(D64) -autostart dcpet8.prg -9 $(D64_9)

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

# todo: doesn't build
dbv:	$(DB_SRC)
	cl65 -I include -t vic20 $^ -o $@

clean:
	$(RM) -rf d src/*.o src/*.s *.prg $(D64) 9.d64 \
	dc64 dc128 dc1280 dcpet8 dcp4 dc610 \
	db64 db128 db1280 dbpet8 dbp4 db610 \
	dbv
	find . -name '*~' -delete
