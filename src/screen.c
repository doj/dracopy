/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */
/*
 * screen.c
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
#include <conio.h>
#include <string.h>
#include "defines.h"

static unsigned char screen_bgc;
static unsigned char screen_borderc;
static unsigned char screen_textc;

void
clearArea(BYTE xpos, BYTE ypos, BYTE xsize, BYTE ysize)
{
  int y = ypos;
  for (; y < (ypos+ysize); ++y)
    {
      cclearxy(xpos,y,xsize);
    }
}

void
newscreen(char * title)
{
	int i;
	clrscr();
	revers(1);
	for (i=0;i<SCREENW;i++)
    {
      cputc(' ');
    }
	gotoxy(0,0);
	cputs(title);
	revers(0);
	gotoxy(0,1);
}

void
drawFrame(char * title, BYTE xpos, BYTE ypos, BYTE xsize, BYTE ysize)
{
  BYTE h1 = 0;
  BYTE title_len = 0;

  // top
  gotoxy(xpos,ypos);
  cputc(CH_ULCORNER);

  if (title)
    {
      title_len = strlen(title);
      h1 = (xsize-2-title_len)/2;
      chline(h1);

      revers(1);
      cputs(title);
      revers(0);

      chline(xsize-h1-title_len-2);
    }
  else
    {
      chline(xsize-2);
    }
  cputc(CH_URCORNER);

  // left
  cvlinexy(xpos, ypos + 1, ysize - 2);

  // bottom
  cputc(CH_LLCORNER);
  chline(xsize - 2);
  cputc(CH_LRCORNER);

  // right
  cvlinexy(xpos + xsize - 1, ypos + 1, ysize - 2);
}

/* initialize screen mode */
void
initScreen(unsigned char border, unsigned char bg, unsigned char text)
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
