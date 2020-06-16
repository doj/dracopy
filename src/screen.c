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
#include "defines.h"

static unsigned char screen_bgc;
static unsigned char screen_borderc;
static unsigned char screen_textc;

void
clearArea(const BYTE xpos, const BYTE ypos, const BYTE xsize, const BYTE ysize)
{
  BYTE y = ypos;
  for (; y < (ypos+ysize); ++y)
    {
      cclearxy(xpos,y,xsize);
    }
}

void
newscreen(const char *title)
{
  BYTE i;

  BYTE len = SCREENW;
  len -= strlen(title);
  len /= 2;

  clrscr();
  revers(1);
  for(i = 0; i < len; ++i)
    cputc(' ');
  for (; *title && i < SCREENW; ++i)
    cputc(*title++);
  for (; i < SCREENW; ++i)
    cputc(' ');
  revers(0);
  gotoxy(0,1);
}

void
drawFrame(const char *title, const BYTE xpos, const BYTE ypos, const BYTE xsize, const BYTE ysize, const char *subtitle)
{
  // top
  gotoxy(xpos,ypos);
  cputc(CH_ULCORNER);
  chline(xsize-2);
  cputc(CH_URCORNER);

  if (title)
    {
      BYTE title_len = strlen(title);
      gotoxy(xpos + (xsize-title_len)/2 ,ypos);
      revers(1);
      cputs(title);
      revers(0);
    }

  // left
  cvlinexy(xpos, ypos + 1, ysize - 2);

  // bottom
  cputc(CH_LLCORNER);
  chline(xsize - 2);
  cputc(CH_LRCORNER);

  if (subtitle)
    {
      gotoxy(xpos+2, ypos+ysize-1);
      //revers(1);
      cputs(subtitle);
      //revers(0);
    }

  // right
  cvlinexy(xpos + xsize - 1, ypos + 1, ysize - 2);
}

/* initialize screen mode */
void
initScreen(const BYTE border, const BYTE bg, const BYTE text)
{
  screen_borderc = bordercolor(border);
  screen_bgc = bgcolor(bg);
  screen_textc = textcolor(text);
  clrscr();
}

/* restore basic screen mode */
void
exitScreen(void)
{
  bordercolor(screen_borderc);
  bgcolor(screen_bgc);
  textcolor(screen_textc);
  clrscr();
}
