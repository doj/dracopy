/*
 * dir.h
 * Created on: 22.04.2018
 * Author: bader++
 * DraBrowse/Copy (dbc*) is a simple copy and file browser.
 * Since make use of kernal routines they shall be able to work with most file oriented IEC devices.
 * Created 2009-2018 by Sascha Bader and updated by Me
 * The code can be used freely as long as you retain a notice describing original source and author.
 * THE PROGRAMS ARE DISTRIBUTED IN THE HOPE THAT THEY WILL BE USEFUL, BUT WITHOUT ANY WARRANTY. USE THEM AT YOUR OWN RISK!
 * Newer versions might be available here: http://www.sascha-bader.de/html/code.html
 */

#ifndef DIR_H
#define DIR_H

#include <defines.h>

typedef struct{
	unsigned char *name;
	struct direlement *firstelement;
	struct direlement *selected;
	unsigned char flags;
	unsigned int pos;
	unsigned int free;
} Directory;

typedef struct direlement{
	struct cbm_dirent dirent;
	struct direlement *next;
	struct direlement *previous;
	unsigned char flags;
} DirElement;

void freeDir(Directory **dir);
unsigned char cbmReadDir(unsigned char lfn, register struct cbm_dirent *l_dirent);
Directory *readDir(Directory *dir, unsigned char lfn);
Directory *sortDir(Directory *dir);

#endif
