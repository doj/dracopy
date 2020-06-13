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
| C64       | dc64, db64, dc64ieee | [dc6480](https://raw.githubusercontent.com/doj/dracopy/master/images/dc6480.png), dc64ieee80
| C128      | dc128, db128 | [dc1280](https://raw.githubusercontent.com/doj/dracopy/master/images/dc1280.png), db1280
| Plus 4    | dcp4, dbp4
| CBM 510   | dc510
| CBM 610   | | [dc610](https://raw.githubusercontent.com/doj/dracopy/master/images/dc610.png), db610
| PET 8296  | | dcpet8, dbpet8

Version Information and Download
---------------------------------

| Ver  | Date        | Feature | Author | Download |
| ---- | ------------| --------| ------ | -------- |
| 1.0  | 23 Jan 2009 | Initial Version | Sascha Bader |
| 1.0b | 23 May 2009 | Minor Bugfixes | Sascha Bader |
| 1.0c | 27 Dec 2009 | Files are now deleted using the scratch command | Sascha Bader | https://csdb.dk/release/?id=89910
| 1.0d | 12 Dec 2010 | implemented disk copy | Sascha Bader | https://csdb.dk/release/?id=98664
| 1.0e | 2018        | combined DraBrowse and DraCopy, single window, lots of other improvements | rbm | [freeforums.net](https://c-128.freeforums.net/thread/568/new-drabrowse-copy-file-browser) [csdb.dk](https://csdb.dk/release/?id=165305)
| 1.0doj | May 2020  | based on the source code of version 1.0c, reimplemented the disk copy, 80 column mode for C64, create d64 image | doj |
| 1.0doj | June 2020 | 80 column mode for C64, create d64 image, SFD-1001 support | doj | [dracopy-1.0doj.zip](http://www.cubic.org/~doj/c64/dracopy-1.0doj.zip)

You can also download dracopy in alternate color schemes:

| Scheme  | Download | Screenshot |
| ------- | -------- | ---------- |
| default | [dracopy-1.0doj.zip](http://www.cubic.org/~doj/c64/dracopy-1.0doj.zip) | [dc64](https://raw.githubusercontent.com/doj/dracopy/master/images/dc64.png)
| blue    | [dracopy-1.0doj-BLUE.zip](http://www.cubic.org/~doj/c64/dracopy-1.0doj-BLUE.zip) | [dc64](https://raw.githubusercontent.com/doj/dracopy/master/images/dc64-blue.png)
| SX-64   | [dracopy-1.0doj-SX.zip](http://www.cubic.org/~doj/c64/dracopy-1.0doj-SX.zip) | [dc64](https://raw.githubusercontent.com/doj/dracopy/master/images/dc64-sx.png)
| C128    | [dracopy-1.0doj-128.zip](http://www.cubic.org/~doj/c64/dracopy-1.0doj-128.zip) | [dc64](https://raw.githubusercontent.com/doj/dracopy/master/images/dc64-128.png)

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
- reduce memory use
  + see if printf() can be replaced with regular string operations
  + check which variables should be "register" https://cc65.github.io/doc/cc65.html#s8
  + check --codesize https://cc65.github.io/doc/cc65.html#option-codesize
- copy features from 1.0e
  + copy seq and rel
  + move file
- format 1571 double sided: https://groups.google.com/forum/#!topic/comp.sys.cbm/fALaIRfeNS0
- diskcopy
  + enable 1571 double sided mode for disk copy: https://www.pagetable.com/?p=1038
  + support 1571 disk copy with sd2iec
  + support 1581 disk copy with sd2iec
  + support tracks 36-42 for 1541 disk copy
  + graphical sector map for SFD-1001
- device id: does it work with 1541? does it work with sd2iec?
- pet
  + disk access doesn't work
  + remove stuff the pet won't use like 1581
- dc610
  + dc610 seems to be too big for memory, it won't even load
- dc510
  + execute keyboard buffer
- support Ultimate 1541 directly: https://github.com/xlar54/ultimateii-dos-lib
- support vice without true drive emulation?
- better program load for dc6480
- use reu to load file for copy. This will allow to copy between directories on the same device.
- order directory entries
- add write protection to disk

Platform Notes
---------------
The Commodore 128 has less memory available than the C64.
This results from the memory layout of the [cc65](https://cc65.github.io/) compiler.
If you use disks with many directory entries, you could run out of memory on the C128.

The [vice](https://vice-emu.sourceforge.io/) emulator needs to enable
the _True Drive Emulation_ to support all operations.

Diskcopy
---------
The program prints the status of reading/writing every sector during the diskcopy operation.

| Char | Description |
| ---- | ----------- |
| r    | reading an odd sector, if the track has more than 25 sectors
| R    | reading a sector, reading even sector if the track has more than 25 sectors
| w    | writing an odd sector, if the track has more than 25 sectors
| W    | writing a sector, writing even sector if the track has more than 25 sectors
| o (lowercase o) | the sector data contains only 0 bytes, the sector is written
| O (uppercase O) | the sector contains only 0 bytes, the sector is not written in optimized diskcopy
| E    | error while reading or writing the sector
| 0..9 | sector is partially used 0-99%
| !    | sector with an invalid link to the next sector (next sector track too large)

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

You can use pucrunch to compress the compiled dracopy program.
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
