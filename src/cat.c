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

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dirent.h>
#include <cbm.h>
#include <sys/types.h>
#include <stdlib.h>
#include "cat.h"
#include "base.h"
#include "defines.h"
#include "ops.h"

int
cathex(BYTE device, char *filename)
{
  unsigned int c;
  BYTE pchar = 0;
  BYTE cnt = 0;
  int offset = 0;
  unsigned int len;
  char buffer[8];

  if( cbm_open (6,device,CBM_READ,filename) != 0)
  {
    cputs("Can't open input file!\n");
    return -1;
  }

  clrscr();
  do
    {
      if (cnt==0)
        {
          clrscr();
        }
      len = cbm_read (6, buffer, sizeof(buffer));
      cputhex16(offset);
      cputc(' ');
      for (c=0; c<len; ++c)
        {
          cputhex8(buffer[c]);
          cputc(' ');
        }
      for (; c<len; ++c)
        {
          cputs("   ");
        }
      for (c=0; c<len; ++c)
        {
          textcolor(DC_COLOR_TEXT);
          pchar=buffer[c];
          if (pchar==10 || pchar ==13)
            {
              textcolor(DC_COLOR_DIM);
              pchar='.';
            }
          cputc(filterchar(pchar));
        }
      textcolor(DC_COLOR_TEXT);

      cputc(13);
      cputc(10);
      cnt++;
      offset+=sizeof(buffer);
      if (cnt==BOTTOM || (len!=sizeof(buffer)))
        {
          cnt=0;
          gotoxy(0,BOTTOM);
          c=waitKey(1);
          if (c == 'q' ||
              c == CH_LARROW ||
              c == CH_STOP)
            break;
        }
    }
  while(len == sizeof(buffer));

  cbm_close (6);
  return 0;
}

int
catasc(BYTE device, char *filename)
{
  unsigned int c;
  BYTE pchar = 0;
  BYTE cnt = 0;
  int offset = 0;
  unsigned int len;
  char buffer[8];

  if( cbm_open (6,device,CBM_READ,filename) != 0)
  {
    cputs("Can't open input file!\n");
    return -1;
  }

  clrscr();
  do
    {
      len = cbm_read (6, buffer, sizeof(buffer));
      for (c=0;c<len;c++)
        {
          textcolor(DC_COLOR_TEXT);
          pchar=buffer[c];
          if (pchar==13 || pchar==10 )
            {
              cputc(13);
              cputc(10);
            }
          else
            {
              cputc(filterchar(pchar));
            }
          textcolor(DC_COLOR_TEXT);
        }

      offset+=sizeof(buffer);
      if (wherey() >= BOTTOM || (len!=sizeof(buffer)))
        {
          gotoxy(0,BOTTOM);
          c=waitKey(1);
          if (c == 'q' ||
              c == CH_LARROW ||
              c == CH_STOP)
            break;
          clrscr();
        }
    }
  while(len == sizeof(buffer));

  cbm_close (6);
  return 0;
}

BYTE filterchar(BYTE pchar)
{
  if (pchar==9) // tab
    {
      textcolor(DC_COLOR_DIM);
      cputc(0xba);
    }
  else if (! ( (pchar==13) ||
               (pchar==10) ||
               (pchar >= 0x20 && pchar <= 0x7f) ||
               (pchar >= 0xA0) ) )
  {
    textcolor(DC_COLOR_DIM);
    pchar = '.';
  }
  if ( pchar>=65 && pchar<=90)
  {
    pchar = pchar+32;
  }
  else if (pchar>=97 && pchar <=122 )
  {
    pchar = pchar-32;
  }
  return pchar;
}
