/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */
/** @file
 * \date 10.01.2009
 * \author bader
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
 * https://github.com/doj/dracopy
 */
#ifndef DIR_H_
#define DIR_H_

#include "defines.h"
#include <cbm.h>

typedef struct direlement {
  struct cbm_dirent dirent;
  struct direlement * next;
  struct direlement * prev;
  unsigned char flags;
} DirElement;


typedef struct {
  /// 16 characters name
  /// 1 comma
  /// 5 characters ID
  /// NUL
  char name[16+1+5+1];
  struct direlement * firstelement;
  struct direlement * selected;
  /// current cursor position
  unsigned int pos;
  /// number of free blocks
  unsigned int free;
} Directory;

void freeDir(Directory* *dir);
void removeFromDir(DirElement *current);
Directory* readDir(Directory *dir, const BYTE device, const BYTE context, const BYTE sorted);
unsigned char myCbmReadDir(const BYTE device, struct cbm_dirent* l_dirent);

#endif /* DIR_H_ */
