src/%.o: src/%.c include/%.h
	cc65 -O -I include -t c64 $<
	ca65 -I include -t c64 src/$$(basename $< .c).s

all: dc610 dc64 dc128 dc1280 dcp4 dcpet8 db64 db128 db1280 dbp4 dbpet8 db610 package 


dc64: src/screen.c src/dc.c src/cat.c src/dir.c
	cl65 -I include -t c64 src/screen.c src/dc.c src/dir.c src/cat.c src/base.c -o dc64.prg
	pucrunch -c64 dc64.prg dc64 

dc128: src/screen.c src/dc.c src/cat.c src/dir.c
	cl65 -I include -t c128 src/screen.c src/dc.c src/dir.c src/cat.c src/base.c -o dc128.prg
	pucrunch -fc128 dc128.prg dc128 

dc1280: src/screen.c src/dc.c src/cat.c src/dir.c
	cl65 -I include -t c128 -DCHAR80 src/screen.c src/dc.c src/dir.c src/cat.c src/base.c -o dc1280.prg
	pucrunch -fc128 dc1280.prg dc1280 

dcp4: src/screen.c src/dc.c src/cat.c src/dir.c
	cl65 -I include -t plus4 src/screen.c src/dc.c src/dir.c src/cat.c src/base.c -o dcp4.prg
	pucrunch -fc16 dcp4.prg dcp4

dc610: src/screen.c src/dc.c src/cat.c src/dir.c
	cl65 -I include -t cbm610 -DCHAR80 -DNOCOLOR src/screen.c src/dc.c src/dir.c src/cat.c src/base.c -o dc610

dcpet8: src/screen.c src/dc.c src/cat.c src/dir.c
	cl65 -I include -t pet -DCHAR80 -DNOCOLOR src/screen.c src/dc.c src/dir.c src/cat.c src/base.c -o dcpet8


db64: src/screen.c src/db.c src/cat.c src/dir.c
	cl65 -I include -t c64 src/screen.c src/db.c src/dir.c src/cat.c src/base.c -o db64.prg
	pucrunch -c64 db64.prg db64 

db128: src/screen.c src/db.c src/cat.c src/dir.c
	cl65 -I include -t c128 src/screen.c src/db.c src/dir.c src/cat.c src/base.c -o db128.prg
	pucrunch -fc128 db128.prg db128 

db1280: src/screen.c src/db.c src/cat.c src/dir.c
	cl65 -I include -t c128 -DCHAR80 src/screen.c src/db.c src/dir.c src/cat.c src/base.c -o db1280.prg
	pucrunch -fc128 db1280.prg db1280 

dbp4: src/screen.c src/db.c src/cat.c src/dir.c
	cl65 -I include -t plus4 src/screen.c src/db.c src/dir.c src/cat.c src/base.c -o dbp4.prg
	pucrunch -fc16 dbp4.prg dbp4

db610: src/screen.c src/db.c src/cat.c src/dir.c
	cl65 -I include -t cbm610 -DCHAR80 -DNOCOLOR src/screen.c src/db.c src/dir.c src/cat.c src/base.c -o db610

dbpet8: src/screen.c src/db.c src/cat.c src/dir.c
	cl65 -I include -t pet -DCHAR80 -DNOCOLOR src/screen.c src/db.c src/dir.c src/cat.c src/base.c -o dbpet8

dbv: src/screen.c src/db.c src/cat.c src/dir.c
	cl65 -I include -t vic20 src/screen.c src/db.c src/dir.c src/cat.c src/base.c -o dbv


test: src/test.c
	cl65 -t c64 src/test.c -o test

test128: src/test.c
	cl65 -t c128 src/test.c -o test128

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
	rm -rf d src/*.o src/*.s *.prg dc64 dc128 dc1280 dcpet8 dcp4 
	rm -rf d src/*.o src/*.s *.prg db64 db128 db1280 dbpet8 dbp4 
	
