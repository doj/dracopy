/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */
/*
 * dir.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <cbm.h>
#include "dir.h"
#include "defines.h"
#include "ops.h"

# define CBM_T_FREE 100

static const char progressBar[4] = { 0xA5, 0xA1, 0xA7, ' ' };
static const char progressRev[4] = { 0,    0,    1,    1 };

/**
 * read directory of device @p device and print information to window @p context.
 * @param[in,out] dir pointer to Directory object, if dir!=NULL it will be freed.
 * @param device CBM device number
 * @param context window ID, must be 0 or 1.
 * @return new allocated Directory object.
 */
Directory*
readDir(Directory *dir, const BYTE device, const BYTE context)
{
	DirElement * previous = NULL;

  BYTE cnt = 0xff;
  const BYTE y = DIRY;
  BYTE x = 0;

  const char *device_type;

  // todo: see if we can get the DOS status without re-opening the lfn
  // TODO: this seems to sometimes crash
  debugs("D1");
  device_type = getDeviceType(context);
  debugs("D2");

  freeDir(&dir);

	if (cbm_opendir(device, device) != 0)
    {
      cputs("could not open directory");
      cbm_closedir(device);
      return NULL;
    }

	while(1)
    {
      DirElement * current = (DirElement *) calloc(1, sizeof(DirElement));
      if (! current)
        break;
      //memset(&(current->dirent), 0x44, sizeof(current->dirent)); // TODO: is this required?

      if (myCbmReadDir(device, &(current->dirent)) != 0)
        {
          free(current);
          break;
        }

      // print progress bar
      if ((cnt>>2) >= DIRW)
        {
          x = DIRX + 1;
          revers(0);
          cnt = 0;
          gotoxy(x, y);
          cclear(DIRW);
          gotoxy(x+DIRW/2-2, y);
          cprintf("[%02i]", device);
        }
      else
        {
          gotoxy(x + (cnt>>2), y);
          revers(progressRev[cnt & 3]);
          cputc(progressBar[cnt & 3]);
          ++cnt;
        }

      if (dir==NULL)
        {
          // initialize directory
          dir = (Directory *) calloc(1, sizeof(Directory));
          if (! dir)
            break;
          if (current->dirent.type == _CBM_T_HEADER)
            {
              memcpy(dir->name, current->dirent.name, 16);
              dir->name[16] = ',';
              dir->name[17] = current->dirent.size & 255;
              dir->name[18] = current->dirent.size >> 8;
              dir->name[19] = 0;
            }
          else
            {
              strcpy(dir->name, "unknown type");
            }
          free(current);
        }
      else if (current->dirent.type==CBM_T_FREE)
        {
          // blocks free entry
          dir->free=current->dirent.size;
          free(current);
        }
      else if (dir->firstelement==NULL)
        {
          // first element
          dir->firstelement = current;
          dir->selected = current;
          previous=current;
        }
      else
        {
          // all other elements
          current->previous=previous;
          previous->next=current;
          previous=current;
        }
    }

	cbm_closedir(device);
	revers(0);

  dir->device_type = device_type;
	return dir;
}

/**
 * @return 0 upon success, @p l_dirent was set.
 * @return >0 upon error.
 */
unsigned char
myCbmReadDir(const BYTE device, struct cbm_dirent* l_dirent)
{
  unsigned char byte, i, byte2, byte3;

  if (cbm_k_chkin (device) != 0)
    {
      cbm_k_clrch ();
      return 1;
    }

  if (cbm_k_readst () == 0) {
    l_dirent->access = 0;

    // skip next basic line: 0x01, 0x01
    cbm_k_basin ();
    cbm_k_basin ();

    // read file size
    l_dirent->size = cbm_k_basin();
    l_dirent->size |= (cbm_k_basin()) << 8;

    i = 0;
    byte = cbm_k_basin();

    // handle "B" BLOCKS FREE
    if (byte == 'b') {
      l_dirent->type = CBM_T_FREE;
      l_dirent->name[i++] = byte;
      while ((byte = cbm_k_basin ()) != '\"'  )
        {
          if (cbm_k_readst () != 0)
            {     /* ### Included to prevent */
              cbm_k_clrch ();             /* ### Endless loop */
              l_dirent->name[i] = '\0';

              return 0;                   /* ### Should be probably removed */
            }
          if (i < sizeof (l_dirent->name) - 1) {
            l_dirent->name[i] = byte;
            ++i;
          }
        }
      l_dirent->name[i] = '\0';
      return 0;
    }

    // read file name
    if (byte != '\"')
      {
        while (cbm_k_basin() != '\"') {
          if (cbm_k_readst () != 0) {   /* ### Included to prevent */
            cbm_k_clrch ();           /* ### Endless loop */
            return 3;                 /* ### Should be probably removed */
          }                             /* ### */
        }
      }

    while ((byte = cbm_k_basin ()) != '\"'  )
      {
        if (cbm_k_readst () != 0) {     /* ### Included to prevent */
          cbm_k_clrch ();             /* ### Endless loop */
          return 4;                   /* ### Should be probably removed */
        }

        /* ### */

        if (i < sizeof (l_dirent->name) - 1) {
          l_dirent->name[i] = byte;
          ++i;
        }
      }
    l_dirent->name[i] = '\0';

    // read file type
    while ((byte=cbm_k_basin ()) == ' ') {
      if (cbm_k_readst ()) {          /* ### Included to prevent */
        cbm_k_clrch ();             /* ### Endless loop */
        return 5;                   /* ### Should be probably removed */
      }                               /* ### */
    }

    byte2 = cbm_k_basin();
    byte3 = cbm_k_basin();

#define X(a,b,c) byte==a && byte2==b && byte3==c

    if (X('p','r','g'))
      {
        l_dirent->type = CBM_T_PRG;
      }
    else if (X('s','e','q'))
      {
        l_dirent->type = CBM_T_SEQ;
      }
    else if (X('u','s','r'))
      {
        l_dirent->type = CBM_T_USR;
      }
    else if (X('d','e','l'))
      {
        l_dirent->type = CBM_T_DEL;
      }
    else if (X('r','e','l'))
      {
        l_dirent->type = CBM_T_REL;
      }
    else if (X('c','b','m'))
      {
        l_dirent->type = CBM_T_CBM;
      }
    else if (X('d','i','r'))
      {
        l_dirent->type = CBM_T_DIR;
      }
    else if (X('v','r','p'))
      {
        l_dirent->type = CBM_T_VRP;
      }
    else if (X('l','n','k'))
      {
        l_dirent->type = CBM_T_LNK;
      }
    else if (byte3 = ' ')
      {
        // reading the disk name line
        l_dirent->type = _CBM_T_HEADER;
        l_dirent->size = byte | (byte2 << 8);
        l_dirent->access = 0;

        while (cbm_k_basin() != 0) {
          if (cbm_k_readst () != 0) { /* ### Included to prevent */
            cbm_k_clrch ();         /* ### Endless loop */
            return 8;               /* ### Should be probably removed */
          }                           /* ### */
        }

        cbm_k_clrch ();
        return 0;                         /* Line read successfuly */
      }
    else
      {
        l_dirent->type = CBM_T_OTHER;
      }

    // read access
    byte = cbm_k_basin ();

    l_dirent->access = (byte == 0x3C) ? CBM_A_RO : CBM_A_RW;

    if (byte != 0) {
      while (cbm_k_basin() != 0) {
        if (cbm_k_readst () != 0) { /* ### Included to prevent */
          cbm_k_clrch ();         /* ### Endless loop */
          return 6;               /* ### Should be probably removed */
        }                           /* ### */
      }
    }

    cbm_k_clrch ();
    return 0;                         /* Line read successfuly */
  }

  return 7;
}

/*
 * free memory of directory structure
 */
void freeDir(Directory * * dir)
{
  DirElement * next;
	DirElement * acurrent;
	if (*dir==NULL)
    return;

  acurrent = (*dir)->firstelement;
  while (acurrent)
    {
      next = acurrent->next;
      free(acurrent);
      acurrent = next;
    }

  free(*dir);
	*dir=NULL;
}


/*
 * Remove an entry from its data structure (directory)
 */
void removeFromDir(DirElement * current)
{
 	if (current == NULL)
    return;

  if (current->previous)
    {
      current->previous->next = current->next;
    }
  if (current->next)
    {
      current->next->previous = current->previous;
    }
  free(current);
}
