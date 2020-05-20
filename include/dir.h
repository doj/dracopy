/*
 * dir.h
 *
 *  Created on: 10.01.2009
 *      Author: bader
 *
 * DraCopy (dc*) is a simple copy program.
 * DraBrowse (db*) is a simple file browser.
 *
 * Since both programs make use of kernal routines they shall
 * be able to work with most file oriented IEC devices.
 *
 * Created 2009 by Sascha Bader
 *
 * The code can be used freely as long as you retain
 * a notice describing original source and author.
 *
 * THE PROGRAMS ARE DISTRIBUTED IN THE HOPE THAT THEY WILL BE USEFUL,
 * BUT WITHOUT ANY WARRANTY. USE THEM AT YOUR OWN RISK!
 *
 * Newer versions might be available here: http://www.sascha-bader.de/html/code.html
 *
 */
#ifndef DIR_H_
#define DIR_H_

#include <defines.h>

typedef struct direlement {
	struct cbm_dirent dirent;
	struct direlement * next;
	struct direlement * previous;
	unsigned char flags;
} DirElement;


typedef struct {
  unsigned char *name;
  struct direlement * firstelement;
  struct direlement * selected;
  unsigned char flags;
  unsigned int pos;
  unsigned int free;
} Directory;

# define CBM_T_FREE 100

void freeDir(Directory * * dir);
void removeFromDir(DirElement * current);
Directory * readDir(Directory * dir, BYTE lfn, BYTE context);
unsigned char myCbmReadDir (unsigned char lfn, register struct cbm_dirent* l_dirent);


#endif /* DIR_H_ */
