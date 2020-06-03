DraCopy and DraBrowse
======================

DraCopy - a simple copy program.

![dracopy screenshot](https://raw.githubusercontent.com/doj/dracopy/master/images/dc64.png)

DraBrowse  - a simple file browser.

![drabro screenshot](https://raw.githubusercontent.com/doj/dracopy/master/images/db64.png)

Created 2009 by Sascha Bader.

Both programs use the kernal routines and are
able to work with most file oriented IEC devices.

Directories are supported on CMD compatible devices and [sd2iec](https://www.c64-wiki.com/wiki/SD2IEC).

| Platforms | Executables |
| --------- | ----------- |
| C64       | dc64, db64
| C128      | dc128, db128 (40 Columns), [dc1280](https://raw.githubusercontent.com/doj/dracopy/master/images/dc1280.png), db1280 (80 Columns)
| Plus 4    | dcp4, dbp4
| CBM 610   | [dc610](https://raw.githubusercontent.com/doj/dracopy/master/images/dc610.gif), db610
| PET 8096  | dcpet8, dbpet8

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
| F7, 7 | execute selected program
| F8, 8 | copy disk from current window to the other device
| ←, ESC, 0 | switch window
| Return, right | enter directory
| DEL, left | go to parent directory
| ↑ | go to root directory
| space | select current file
| * | invert selection
| HOME, t | move cursor to top row of first page in current window
| n | go to the next page in current window
| p | go to the previous page in current window
| b | go to bottom (last) page in current window
| c | copy current file on the same device
| r | rename current file
| f | format device in current window
| @ | send DOS command to device in current window
| . | show about information
| q | quit program to BASIC
| £ | change device ID of current device
| w | enlarge or shrink current window
| s | show directory entries sorted

source code
------------
This repository contains the source code of the Commodore 64 copy program dracopy.
I have found two versions of the source code:
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
- copy some more features from 1.0e
  + better dir handling (vice)
  + make image 'i'
  + copy seq and rel
- format 1571 double sided: https://groups.google.com/forum/#!topic/comp.sys.cbm/fALaIRfeNS0
- diskcopy
  + enable 1571 double sided mode for disk copy: https://www.pagetable.com/?p=1038
  + support 1571 disk copy with sd2iec
  + support 1581 disk copy with sd2iec
  + support tracks 36-42 for 1541 disk copy
- help screen 'h'

Version Information and Download
---------------------------------

| Ver  | Date        | Feature | Author | Download |
| ---- | ------------| --------| ------ | -------- |
| 1.0  | 23 Jan 2009 | Initial Version | Sascha Bader |
| 1.0b | 23 May 2009 | Minor Bugfixes | Sascha Bader |
| 1.0c | 27 Dec 2009 | Files are now deleted using the scratch command | Sascha Bader | https://csdb.dk/release/?id=89910
| 1.0d | 12 Dec 2010 | implemented disk copy | Sascha Bader | https://csdb.dk/release/?id=98664
| 1.0e | 2018        | combined DraBrowse and DraCopy, single window, lots of other improvements | rbm | https://c-128.freeforums.net/thread/568/new-drabrowse-copy-file-browser
| 1.0doj | May 2020  | based on the source code of version 1.0c, reimplemented the disk copy | doj | [dracopy-1.0doj.zip](http://www.cubic.org/~doj/c64/dracopy-1.0doj.zip)

Platform Notes
---------------
The Commodore 128 has less memory available than the C64.
This results from the memory layout of the [cc65](https://cc65.github.io/) compiler.
If you use disks with many directory entries, you could run out of memory on the C128.

Copyright Notice
-----------------
The code can be used freely as long as you retain a notice describing original source and author.
THE PROGRAMS ARE DISTRIBUTED IN THE HOPE THAT THEY WILL BE USEFUL, BUT WITHOUT ANY WARRANTY.
USE THEM AT YOUR OWN RISK!

Contact
--------
The github repository https://github.com/doj/dracopy is maintained by
Dirk Jagdmann <doj@cubic.org>
