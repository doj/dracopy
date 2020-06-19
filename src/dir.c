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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <cbm.h>
#include "dir.h"
#include "defines.h"
#include "ops.h"

#define CBM_T_FREE 100

#define DISK_ID_LEN 5
#define disk_id_buf linebuffer2

static const char progressBar[4] = { 0xA5, 0xA1, 0xA7, ' ' };
static const char progressRev[4] = { 0,    0,    1,    1 };

/**
 * read directory of device @p device.
 * @param[in,out] dir pointer to Directory object, if dir!=NULL it will be freed.
 * @param device CBM device number
 * @param sorted if true, return directory entries sorted.
 * @param context window context.
 * @return new allocated Directory object.
 */
Directory*
readDir(Directory *dir, const BYTE device, const BYTE context, const BYTE sorted)
{
  DirElement * previous = NULL;

  BYTE cnt = 0xff;
  const BYTE y = DIRY;
  BYTE x = 0;

  freeDir(&dir);
  memset(disk_id_buf, 0, DISK_ID_LEN);

  if (cbm_opendir(device, device) != 0)
    {
      cbm_closedir(device);
      return NULL;
    }

  while(1)
    {
      BYTE ret;
      DirElement * de = (DirElement *) calloc(1, sizeof(DirElement));
      if (! de)
        goto stop;

      ret = myCbmReadDir(device, &(de->dirent));
      if (ret != 0)
        {
          debugu(ret);
          free(de);
          goto stop;
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
            goto stop;
          if (de->dirent.type == _CBM_T_HEADER)
            {
              BYTE i;
              for(i = 0; de->dirent.name[i]; ++i)
                {
                  dir->name[i] = de->dirent.name[i];
                }
              dir->name[i++] = ',';
              memcpy(&dir->name[i], disk_id_buf, DISK_ID_LEN);
            }
          else
            {
              strcpy(dir->name, "unknown type");
            }
          free(de);
        }
      else if (de->dirent.type==CBM_T_FREE)
        {
          // blocks free entry
          dir->free=de->dirent.size;
          free(de);
          goto stop;
        }
      else if (dir->firstelement==NULL)
        {
          // first element
          dir->firstelement = de;
          previous=de;
        }
      else
        {
          // all other elements
          if (sorted)
            {
              // iterate the sorted list
              DirElement *e;
              for(e = dir->firstelement; e->next; e = e->next)
                {
                  // if the new name is greater than the current list item,
                  // it needs to be inserted in the previous position.
                  if (strncmp(e->dirent.name, de->dirent.name, 16) > 0)
                    {
                      // if the previous position is NULL, insert at the front of the list
                      if (! e->prev)
                        {
                          de->next = e;
                          e->prev = de;
                          dir->firstelement = de;
                        }
                      else
                        {
                          // insert somewhere in the middle
                          DirElement *p = e->prev;
                          assert(p->next == e);
                          p->next = de;
                          de->next = e;

                          de->prev = p;
                          e->prev = de;
                        }
                      goto inserted;
                    }
                }
              assert(e->next == NULL);
              e->next = de;
              de->prev = e;
            inserted:;
            }
          else
            {
              de->prev = previous;
              previous->next = de;
              previous = de;
            }
        }
    }

 stop:
  cbm_closedir(device);
  revers(0);

  if (dir)
    {
      dir->selected = dir->firstelement;
    }
  return dir;
}

/**
 * @param l_dirent pointer to cbm_dirent object, must be memset to 0.
 * @return 0 upon success, @p l_dirent was set.
 * @return >0 upon error.
 */
unsigned char
myCbmReadDir(const BYTE device, struct cbm_dirent* l_dirent)
{
  BYTE b, len;
  BYTE i = 0;

  // check that device is ready
  if (cbm_k_chkin (device) != 0)
    {
      cbm_k_clrch();
      return 1;
    }
  if (cbm_k_readst() != 0)
    {
      return 7;
    }

  // skip next basic line: 0x01, 0x01
  cbm_k_basin();
  cbm_k_basin();

  // read file size
  l_dirent->size = cbm_k_basin();
  l_dirent->size |= (cbm_k_basin()) << 8;

  // read line into linebuffer
  memset(linebuffer, 0, sizeof(linebuffer));
  //cclearxy(0,BOTTOM,SCREENW);//debug
  while(1)
    {
      // read byte
      b = cbm_k_basin();
      // EOL?
      if (b == 0)
        {
          break;
        }
      // append to linebuffer
      if (i < sizeof(linebuffer))
        {
          linebuffer[i++] = b;
          //cputcxy(i,BOTTOM,b);//debug
        }
      // return if reading had error
      if (cbm_k_readst() != 0)
        {
          cbm_k_clrch();
          return 2;
        }
    }
  cbm_k_clrch();
  //cputcxy(i,BOTTOM,'?');//debug

  // handle "B" BLOCKS FREE
  if (linebuffer[0] == 'b')
    {
      l_dirent->type = CBM_T_FREE;
      return 0;
    }

  // check that we have a minimum amount of characters to work with
  if (i < 5)
    {
      return 3;
    }

  // strip whitespace from right part of line
  for(len = i; len > 0; --len)
    {
      b = linebuffer[len];
      if (b == 0 ||
          b == ' ' ||
          b == 0xA0)
        {
          linebuffer[len] = 0;
          continue;
        }
      ++len;
      break;
    }

  //cputcxy(len,BOTTOM,'!');//debug
  //cgetc();//debug

  // parse file name

  // skip until first "
  for(i = 0; i < sizeof(linebuffer) && linebuffer[i] != '"'; ++i)
    {
      // do nothing
    }

  // copy filename, until " or max size
  b = 0;
  for(++i; i < sizeof(linebuffer) && linebuffer[i] != '"' && b < 16; ++i)
    {
      l_dirent->name[b++] = linebuffer[i];
    }

  // check file type
#define X(a,b,c) linebuffer[len-3]==a && linebuffer[len-2]==b && linebuffer[len-1]==c

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
  else
    {
      // parse header
      l_dirent->type = _CBM_T_HEADER;

      // skip one character which should be "
      if (linebuffer[i] == '"')
        {
          ++i;
        }
      // skip one character which should be space
      if (linebuffer[i] == ' ')
        {
          ++i;
        }

      // copy disk ID
      for(b = 0; i < sizeof(linebuffer) && b < DISK_ID_LEN; ++i, ++b)
        {
          disk_id_buf[b] = linebuffer[i];
        }

      // strip disk name
      for(b = 15; b > 0; --b)
        {
          if (l_dirent->name[b] == 0 ||
              l_dirent->name[b] == ' ' ||
              l_dirent->name[b] == 0xA0)
            {
              l_dirent->name[b] = 0;
              continue;
            }
          break;
        }

      return 0;
    }

  // parse read-only
  l_dirent->access = (linebuffer[i-4] == 0x3C) ? CBM_A_RO : CBM_A_RW;

  return 0;
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

  if (current->prev)
    {
      current->prev->next = current->next;
    }
  if (current->next)
    {
      current->next->prev = current->prev;
    }
  free(current);
}
