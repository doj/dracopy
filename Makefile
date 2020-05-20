# These sources were developed with cc65 2.15

SYS?=c64
NAME=dracopy10e
CSRC=base.c cat.c dbc.c dir.c screen.c
ASRC=
X64?=x64sc

############## you should hardly need to configure beyond this point

CC   = cc65
AS   = ca65
LD   = ld65

LDLIBS+=--lib $(SYS).lib
LDFLAGS+=-t $(SYS) -m $(basename $@).map -o $@

CFLAGS+=-T --standard cc65 --codesize 500 -t $(SYS) -DVERSION=\"$(VERSION)\"
#CFLAGS+=-g --check-stack
CFLAGS+=-O -Oi -Or -Os
CFLAGS+=-DNDEBUG -I.

AFLAGS+=-t $(SYS)
#AFLAGS+=-g

EXE=$(NAME).prg
D64=$(NAME).d64

OBJ=$(CSRC:%.c=%.o) $(ASRC:%.s=%.o)

all:	$(EXE)
	if [ ! -f Makefile.dep ];then $(MAKE) dep;fi

$(EXE):	$(OBJ)
	$(LD) $(LDFLAGS) $^ $(LDLIBS)
	-pucrunch -c64 $@ > $@.new
	mv $@.new $@

$(D64):	$(EXE)
	rm -f $(D64)
	c1541 -format 'doj,dj' d64 $(D64)
	c1541 $(D64) -write $(EXE)

x64:	$(D64)
	$(X64) -autostartprgmode 1 -autostart $(EXE)

%.o:	%.c
	$(CC) $(CFLAGS) $<
	$(AS) $(AFLAGS) $*.s
#	$(RM) $*.s

%.o:	%.s
	$(AS) $(AFLAGS) $<

clean:
	-rm -f $(EXE) *.map $(CSRC:%.c=%.s) *.o *~ Makefile.dep $(D64)

dep:
	gcc -E -M -nostdinc -I$(CC65_HOME)/include -D__CBM__ *.c > Makefile.dep

-include Makefile.dep
