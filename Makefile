src/%.o: src/%.c include/%.h
	cc65 -O -I include -t c64 $<
	ca65 -I include -t c64 src/$$(basename $< .c).s

all: dc610 dc64 dc128 dc1280 dcp4 dcpet8 db64 db128 db1280 dbp4 dbpet8 db610 package

dc64: src/screen.c src/dc.c src/cat.c src/dir.c src/base.c
	cl65 -I include -t c64 $^ -o $@.prg
	-pucrunch -c64 $@.prg $@

dc128: src/screen.c src/dc.c src/cat.c src/dir.c src/base.c
	cl65 -I include -t c128 $^ -o $@.prg
	-pucrunch -fc128 $@.prg $@

dc1280: src/screen.c src/dc.c src/cat.c src/dir.c src/base.c
	cl65 -I include -t c128 -DCHAR80 $^ -o $@.prg
	-pucrunch -fc128 $@.prg $@

dcp4: src/screen.c src/dc.c src/cat.c src/dir.c src/base.c
	cl65 -I include -t plus4 $^ -o $@.prg
	-pucrunch -fc16 $@.prg $@

dc610: src/screen.c src/dc.c src/cat.c src/dir.c src/base.c
	cl65 -I include -t cbm610 -DCHAR80 -DNOCOLOR $^ -o $@

dcpet8: src/screen.c src/dc.c src/cat.c src/dir.c src/base.c
	cl65 -I include -t pet -DCHAR80 -DNOCOLOR $^ -o $@

db64: src/screen.c src/db.c src/cat.c src/dir.c src/base.c
	cl65 -I include -t c64 $^ -o $@.prg
	-pucrunch -c64 $@.prg $@

db128: src/screen.c src/db.c src/cat.c src/dir.c src/base.c
	cl65 -I include -t c128 $^ -o $@.prg
	-pucrunch -fc128 $@.prg $@

db1280: src/screen.c src/db.c src/cat.c src/dir.c src/base.c
	cl65 -I include -t c128 -DCHAR80 $^ -o $@.prg
	-pucrunch -fc128 $@.prg $@

dbp4: src/screen.c src/db.c src/cat.c src/dir.c src/base.c
	cl65 -I include -t plus4 $^ -o $@.prg
	-pucrunch -fc16 $@.prg $@

db610: src/screen.c src/db.c src/cat.c src/dir.c src/base.c
	cl65 -I include -t cbm610 -DCHAR80 -DNOCOLOR $^ -o $@

dbpet8: src/screen.c src/db.c src/cat.c src/dir.c src/base.c
	cl65 -I include -t pet -DCHAR80 -DNOCOLOR $^ -o $@

dbv: src/screen.c src/db.c src/cat.c src/dir.c src/base.c
	cl65 -I include -t vic20 $^ -o $@

test: src/test.c
	cl65 -t c64 $^ -o $@

test128: src/test.c
	cl65 -t c128 $^ -o $@

package: all
	c1541 -attach dc10c.d64 -delete dc64 || exit 0
	c1541 -attach dc10c.d64 -delete dc128 || exit 0
	c1541 -attach dc10c.d64 -delete dc1280 || exit 0
	c1541 -attach dc10c.d64 -delete dc610 || exit 0
	c1541 -attach dc10c.d64 -delete dcp4 || exit 0
	c1541 -attach dc10c.d64 -delete dcpet8 || exit 0

	c1541 -attach dc10c.d64 -write dc64 || exit 0
	c1541 -attach dc10c.d64 -write dc128 || exit 0
	c1541 -attach dc10c.d64 -write dc1280 || exit 0
	c1541 -attach dc10c.d64 -write dc610 || exit 0
	c1541 -attach dc10c.d64 -write dcp4 || exit 0
	c1541 -attach dc10c.d64 -write dcpet8 || exit 0

	c1541 -attach dc10c.d64 -delete db64 || exit 0
	c1541 -attach dc10c.d64 -delete db128 || exit 0
	c1541 -attach dc10c.d64 -delete db1280 || exit 0
	c1541 -attach dc10c.d64 -delete db610 || exit 0
	c1541 -attach dc10c.d64 -delete dbp4 || exit 0
	c1541 -attach dc10c.d64 -delete dbpet8 || exit 0

	c1541 -attach dc10c.d64 -write db64 || exit 0
	c1541 -attach dc10c.d64 -write db128 || exit 0
	c1541 -attach dc10c.d64 -write db1280 || exit 0
	c1541 -attach dc10c.d64 -write db610 || exit 0
	c1541 -attach dc10c.d64 -write dbp4 || exit 0
	c1541 -attach dc10c.d64 -write dbpet8 || exit 0

clean:
	$(RM) -rf d src/*.o src/*.s *.prg \
	dc64 dc128 dc1280 dcpet8 dcp4 dc610 \
	db64 db128 db1280 dbpet8 dbp4 db610
	find . -name '*~' -delete
