DraCopy and DraBrowsw
======================

DraCopy - a simple copy program.

![dracopy screenshot](https://raw.githubusercontent.com/doj/dracopy/master/images/dc64.png)

DraBrowse  - a simple file browser.

![drabro screenshot](https://raw.githubusercontent.com/doj/dracopy/master/images/db64.png)

Created 2009 by Sascha Bader.

Since both programs make use of kernal routines they shall
be able to work with most file oriented IEC devices.

Directories are supported on CMD compatible devices only.

| Platforms | Executables |
| --------- | ----------- |
| C64       | dc64, db64
| C128      | dc128, db128 (40 Columns), [dc1280](https://raw.githubusercontent.com/doj/dracopy/master/images/dc1280.gif), db1280 (80 Columns)
| Plus 4    | dcp4, dbp4
| CBM 610   | [dc610](https://raw.githubusercontent.com/doj/dracopy/master/images/dc610.gif), db610
| PET 8096  | dcpet8, dbpet8  (Note: use keys 1-8 instead of fkeys)

source code
------------
This repository contains the source code of the Commodore 64 copy program dracopy.
I have found two versions of the source code:
- 1.0c on https://csdb.dk/release/?id=89910
- 1.0e on https://c-128.freeforums.net/thread/568/new-drabrowse-copy-file-browser

The original program was written by Sascha Bader and is available at
http://www.mobilefx.de/html/dracopy.html
it currently has version 1.0d for download, but the source code is not available.

The 1.0e version was updated by an unknown developer.

The master branch is currently used to develop the 1.0doj version.

The code was tested to build on a unix system with GNU make.

You can use pucrunch to compress the compiled dracopy program.
See http://a1bert.kapsi.fi/Dev/pucrunch/ or https://github.com/mist64/pucrunch
for the source code.

TODO
-----
the following features or bugs should be fixed:
- [ ] support directories and sd2iec stuff
- [ ] format 1571 double sided: https://groups.google.com/forum/#!topic/comp.sys.cbm/fALaIRfeNS0
- [ ] find out why 1540 and 1570 don't work (?)

Version Information
--------------------

| Ver  | Date        | Feature | Author |
| ---- | ------------| --------| ------ |
| 1.0  | 23 Jan 2009 | Initial Version | Sascha Bader
| 1.0b | 23 May 2009 | Minor Bugfixes | Sascha Bader
| 1.0c | 27 Dec 2009 | Files are now deleted using the scratch command | Sascha Bader
| 1.0d | 12 Dec 2010 | implemented disk copy | Sascha Bader
| 1.0e | 2018        | combined DraBrowse and DraCopy, single window | unknown
| 1.0doj | May 2020  | based on the source code of version 1.0c, reimplemented the disk copy in DraCopy, currently d64 is supported. | doj

Copyright Notice
-----------------
The code can be used freely as long as you retain a notice describing original source and author.
THE PROGRAMS ARE DISTRIBUTED IN THE HOPE THAT THEY WILL BE USEFUL, BUT WITHOUT ANY WARRANTY.
USE THEM AT YOUR OWN RISK!

Contact
--------
The github repository https://github.com/doj/dracopy is maintained by
Dirk Jagdmann <doj@cubic.org>
