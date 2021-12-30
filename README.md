DraCopy and DraBrowse
======================

DraCopy - a simple copy program.

![dracopy screenshot](https://raw.githubusercontent.com/doj/dracopy/master/images/dc64.png)

DraBrowse  - a simple file browser.

![drabrowse screenshot](https://raw.githubusercontent.com/doj/dracopy/master/images/db64.png)

Created 2009 by Sascha Bader.

Both programs use the kernal routines and are
able to work with most file oriented IEC devices.

Directories are supported on CMD compatible devices and [sd2iec](https://www.c64-wiki.com/wiki/SD2IEC).
The [SFD-1001](https://www.c64-wiki.com/wiki/SFD-1001) floppy is supported with the
[IEEE-488](https://www.pagetable.com/?p=1303) interface and the C64 version, use the dc64ieee program.

| Platforms | 40 columns | 80 columns |
| --------- | ---------- | ---------- |
| C64       | dc64, db64, dc64ieee | [dc6480](https://raw.githubusercontent.com/doj/dracopy/master/images/dc6480.png), db6480, dc64ieee80
| C128      | dc128, db128 | [dc1280](https://raw.githubusercontent.com/doj/dracopy/master/images/dc1280.png), db1280
| Plus 4    | dcp4, dbp4
| CBM 510   | dc510, db510
| CBM 610   | | [dc610](https://raw.githubusercontent.com/doj/dracopy/master/images/dc610.png), db610
| PET 3032B, 4032 | [dcpet40](https://raw.githubusercontent.com/doj/dracopy/master/images/dcpet40.png), dbpet40
| PET 8032, 8096, 8296 | | [dcpet80](https://raw.githubusercontent.com/doj/dracopy/master/images/dcpet80.png), dbpet80

Version Information and Download
---------------------------------

| Ver  | Date        | Feature | Author | Download |
| ---- | ------------| --------| ------ | -------- |
| 1.0  | 23 Jan 2009 | Initial Version | Sascha Bader |
| 1.0b | 23 May 2009 | Minor Bugfixes | Sascha Bader |
| 1.0c | 27 Dec 2009 | Files are now deleted using the scratch command | Sascha Bader | https://csdb.dk/release/?id=89910
| 1.0d | 12 Dec 2010 | implemented disk copy | Sascha Bader | https://csdb.dk/release/?id=98664
| 1.0e | 2018        | single window, REU, many other improvements | rbm | [freeforums.net](https://c-128.freeforums.net/thread/568/new-drabrowse-copy-file-browser) [csdb.dk](https://csdb.dk/release/?id=165305)
| 1.0doj | May 2020  | based on the source code of version 1.0c, reimplemented the disk copy | doj |
| 1.0doj | June 2020 | 80 column mode for C64, create d64 image, SFD-1001 support, REU | doj | [dracopy-1.0doj.zip](http://www.cubic.org/~doj/c64/dracopy-1.0doj.zip)

DraCopy builds in alternate color schemes:

| Scheme  | Download | Screenshot |
| ------- | -------- | ---------- |
| default | [dracopy-1.0doj.zip](http://www.cubic.org/~doj/c64/dracopy-1.0doj.zip) | [dc64](https://raw.githubusercontent.com/doj/dracopy/master/images/dc64.png)
| blue    | [dracopy-1.0doj-BLUE.zip](http://www.cubic.org/~doj/c64/dracopy-1.0doj-BLUE.zip) | [dc64-blue](https://raw.githubusercontent.com/doj/dracopy/master/images/dc64-blue.png)
| SX-64   | [dracopy-1.0doj-SX.zip](http://www.cubic.org/~doj/c64/dracopy-1.0doj-SX.zip) | [dc64-sx](https://raw.githubusercontent.com/doj/dracopy/master/images/dc64-sx.png)
| C128    | [dracopy-1.0doj-128.zip](http://www.cubic.org/~doj/c64/dracopy-1.0doj-128.zip) | [dc64-128](https://raw.githubusercontent.com/doj/dracopy/master/images/dc64-128.png)

The following special builds of dracopy are available for download:

| build | Download |
| ----- | -------- |
| Commodore REU for dc64 | [dracopy-1.0doj-reu.zip](http://www.cubic.org/~doj/c64/dracopy-1.0doj-reu.zip)
| Kerberos MIDI for dc64 | [dracopy-1.0doj-kerberos.zip](http://www.cubic.org/~doj/c64/dracopy-1.0doj-kerberos.zip)

Keys
-----
The following key can be pressed to select a function.
The DraBrowse program only supports a subset of these functions.
See the TODO section below for some functions that may have issues.

| Key | Function |
| --- | -------- |
| F1, 1 | read directory in current window
| F2, 2 | select the next device for the current window
| F3, 3 | view current file as hex dump
| F4, 4 | view current file as ASCII text
| F5, 5 | copy all selected files from current window
| F6, 6 | delete all selected file in current window
| F7, 7 | run the selected program
| F8, 8 | [copy disk](https://raw.githubusercontent.com/doj/dracopy/master/images/dc64-diskcopy.png) from current window to the other device
| d | optimized disk copy, only write sectors which are not all 0 bytes.
| ←, ESC, 0 | switch window
| w | [enlarge or shrink](https://raw.githubusercontent.com/doj/dracopy/master/images/dc64-windowsize.png) current window
| Return    | enter directory or run the selected program
| right     | enter directory
| DEL, left | go to parent directory
| ↑ | go to root directory
| s | show directory entries sorted
| space | select current file
| * | invert selection
| HOME, t | move cursor to top row of first page in current window
| n | go to the next page in current window
| p | go to the previous page in current window
| b | go to bottom page in current window
| c | copy current file on the same device
| r | rename current file
| z | read current file into REU
| x | write file in REU to disk
| f | format device in current window
| l | change disk name, relabel
| i | create a D64, D71, D81 image file
| @ | send a DOS command to the device in current window
| £ | change the device ID of the current device
| . | show about/help information
| q | quit program to BASIC

TODO
-----
the following features can be implemented or bugs should be fixed:
- sd2iec [documentation](https://www.sd2iec.de/gitweb/?p=sd2iec.git;a=blob;f=README;hb=HEAD)
  + rmdir https://wpguru.co.uk/2014/08/how-to-use-sd2iec-a-quick-command-reference/
    however regular scratch works on directories as well.
  + in diskcopy get size of currently mounted image and check if it
    matches the size of the target device.
- reduce memory use
  + check which variables should be "register" https://cc65.github.io/doc/cc65.html#s8
  + check --codesize https://cc65.github.io/doc/cc65.html#option-codesize
- copy features from 1.0e
  + copy seq and rel
  + move file
- format 1571 double sided: https://groups.google.com/forum/#!topic/comp.sys.cbm/fALaIRfeNS0
- diskcopy
  + enable 1571 double sided mode for disk copy: https://www.pagetable.com/?p=1038
  + support tracks 36-42 for 1541 disk copy
- device id: does it work with 1541? does it work with sd2iec?
- dc510
  + run PRG
- support Ultimate 1541 directly: https://github.com/xlar54/ultimateii-dos-lib
- support vice without true drive emulation?
- dc6480 is crashing when running a PRG with filename > 6 or 7 characters.
  Maybe setup a small assembly program which is started after the reset to BASIC?
- order directory entries
- add write protection to disk
- use SDOS https://csdb.dk/release/?id=184797
- for C128 use https://sites.google.com/site/h2obsession/CBM/C128/JiffySoft128
- fix directory read in Pi1541 browse mode

Platform Notes
---------------
The Commodore 128 has less memory available than the C64.
This results from the memory layout of the [cc65](https://cc65.github.io/) compiler.
Disks with many directory entries could run out of memory on the C128.

The PET needs to have at least 32KB of RAM to run DraCopy. There is
not much memory left, disks with many directory entries may run out of
memory.

The [vice](https://vice-emu.sourceforge.io/) emulator needs to enable
the _True Drive Emulation_ to support all operations.

Feature Description
--------------------

### Diskcopy
The program prints the status of reading/writing every sector during the diskcopy operation.

| Char | Description |
| ---- | ----------- |
| r    | reading an odd sector, if the track has more than 25 sectors
| R    | reading a sector or reading even sector if the track has more than 25 sectors
| w    | writing an odd sector, if the track has more than 25 sectors
| W    | writing a sector or writing even sector if the track has more than 25 sectors
| o (lowercase o) | the sector data (254 bytes) contains only 0 bytes
| O (uppercase O) | the whole sector (256 bytes) contains only 0 bytes
| e    | error while reading the sector
| E    | error while writing the sector
| 0..9 | sector was written and is only partially used, 0% - 99%
| !    | sector was written and contains an invalid link to the next sector (track too large)

When the optimized diskcopy is used (key 'D') sectors which are all
zero bytes (status char uppercase 'O') will not be written to the target disk. This will decrease
the copy time by only writing sectors with data.

### Format

Use a disk name of up to 16 characters. Only giving a disk name will
quick format the disk, only writing the BAM and and empty
directory. Add a 2 character disk ID separated by a comma character
for a full format, writing all disk sectors with 0 bytes.

### Relabel

Use a disk name of up to 16 characters. The 2 character disk ID can be
changed, by appending it with a comma character to the disk name.

### Disk Image

Give the disk image file name and the disk image type. Valid disk
image types are ".d64", ".d71", ".d81". The image file name + image
type name must be <= 16 characters.

### REU support

A number of RAM expansion cards are supported with the cc65 compiler
and DraCopy. If the RAM expansion is detected a single file can be
loaded into the REU with the 'z' key. The file can then be written to
disk multiple times with the 'x' key.
This feature is useful to either write a file multiple times, or to
copy a file between directories on the same device.

If the RAM expansion is larger than a disk image size, the diskcopy
feature will write the image into the RAM expansion upon the first
disk copy. A subsequent disk copy will then use the RAM expansion disk
image and doesn't need to read the source drive any more.

The Download section contains a link to a pre-built version of DraCopy
for the [Commodore REU](https://www.c64-wiki.com/wiki/Commodore_REU)
cartridges. Support for other RAM expansion hardware needs to be
custom compiled, by uncommenting the name of the REU driver in the
Makefile. All REU versions of DraCopy require the corresponding *.emd
driver file to be present on the device from where DraCopy is started.

When a REU is used the amount of used and total memory in KB is
displayed on the bottom of the menu window. Note that ~4 disk blocks
are 1KB.

Contact
--------
The github repository https://github.com/doj/dracopy is maintained by
Dirk Jagdmann <doj@cubic.org>

source code
------------
This repository contains the source code of the Commodore 64 copy program dracopy.
Two versions of the source code were released previously:
- 1.0c on https://csdb.dk/release/?id=89910 , [github branch](https://github.com/doj/dracopy/tree/1.0c-import)
- 1.0e on https://c-128.freeforums.net/thread/568/new-drabrowse-copy-file-browser , [github branch](https://github.com/doj/dracopy/tree/1.0e-import)

The original program was written by Sascha Bader and is available at
http://www.mobilefx.de/html/dracopy.html
it currently has version 1.0d for download, but the source code is not available.

The 1.0e version was updated by rbm.

The master branch in this repository is currently used to develop
the 1.0doj version which was branched from the 1.0c version. It is
re-implementing some features from the independently branched 1.0e
version.

The code was tested to build on a unix system (Linux and OsX) with GNU
make and the [cc65](https://cc65.github.io/) C compiler for Commodore
8 bit computers.

pucrunch can be used to compress the compiled dracopy program.
See http://a1bert.kapsi.fi/Dev/pucrunch/ or https://github.com/mist64/pucrunch
for the source code.

Different color schemes can be compiled, see the top of the Makefile how to enable them.
To create additional color schemes, add a new define to the Makefile and configure
the scheme in defines.h

Copyright Notice
-----------------
The code can be used freely as long as you retain a notice describing original source and author.
THE PROGRAMS ARE DISTRIBUTED IN THE HOPE THAT THEY WILL BE USEFUL, BUT WITHOUT ANY WARRANTY.
USE THEM AT YOUR OWN RISK!

Links
------
IEEE-488 drives like SFD-1001:
http://www.zimmers.net/anonftp/pub/cbm/schematics/cartridges/c64/ieee-488/index.html
https://www.lemon64.com/forum/viewtopic.php?t=65199&sid=720fd316d9a3227e0820c1465f4e042b

[C64 utilities](https://sta.c64.org/c64utils.html) has 2 d64 images
with various disk, file and development utilities for the
Commodore 64.

[CBM-Command](https://github.com/CINJ/CBM-Command) an orthodox file
manager, similar to DraCopy. See also
https://www.c64-wiki.com/wiki/CBM-Command

[Advanced optimizations in CC65](https://github.com/ilmenit/CC65-Advanced-Optimizations)
a good article how to write efficient cc65 C code.

1581 Partitions
----------------

Information from https://www.youtube.com/watch?v=Fgtl8g1Gfog

### create a 1581 partition

OPEN 1,8,15
PRINT#15,"/0:PARTNAME,"+CHR$(start track)+CHR$(0)+CHR$(number of
blocks LO)+CHR$(number of blocks HI)+",C"
CLOSE 1

number of blocks should be a multiple of 40 (40 sectors per track).

### changing into a 1581 partition

@/0:PARTNAME

You would also need to format the partition after changing into it for
the first time:

@N0:PARTNAME,ID

To change to the root directory:

@/
