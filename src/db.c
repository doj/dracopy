/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */
/*
 * db.c
 *
 *  Created on: 10.01.2009
 *      Author: bader
 *
 * DraCopy (dc*) is a simple copy program.
 * DraBrowser (db*) is a simple file browser.
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <screen.h>
#include <conio.h>
#include "cat.h"
#include "dir.h"
#include "base.h"
#include "defines.h"
#include "ops.h"
#include "version.h"

/* definitions */
extern BYTE context;
extern BYTE devices[];
extern char linebuffer[];
extern Directory* dirs[];
extern const BYTE textc;

void
updateMenu(void)
{
	BYTE menuy=MENUY;

	revers(0);
	textcolor(textc);
	drawFrame(" " DRA_VERNUM " ",MENUX,MENUY,MENUW,MENUH,NULL);

	menuy+=2;
	gotoxy(MENUX+1,menuy++);
	cputs(" F1 DIR");
	gotoxy(MENUX+1,menuy++);
	cputs(" F2 DEVICE");
	gotoxy(MENUX+1,menuy++);
	cputs(" F3 HEX");
	gotoxy(MENUX+1,menuy++);
	cputs(" F4 ASC");
	gotoxy(MENUX+1,menuy++);
#ifdef __PLUS4__
	cputs("ESC SWITCH");
#else
	cputc(' ');
	cputc(95); // arrow left
	cputs("  SWITCH");
#endif
	gotoxy(MENUX+1,menuy++);
	cputs(" CR RUN/CD");
	gotoxy(MENUX+1,menuy++);
	cputs(" BS DIR UP");
	gotoxy(MENUX+1,menuy++);
	cputs("  T TOP");
	gotoxy(MENUX+1,menuy++);
	cputs("  B BOTTOM");
	gotoxy(MENUX+1,menuy++);
	cputs("  . ABOUT");
	gotoxy(MENUX+1,menuy++);
	cputs("  Q QUIT");
	menuy++;
	gotoxy(MENUX+1,menuy++);
	cprintf(" Device:%02d",devices[0]);
}

void
mainLoop(void)
{
	Directory * cwd = NULL;
	DirElement * current = NULL;
	unsigned int pos = 0;
	BYTE c;
	BYTE lastpage = 0;
	BYTE nextpage = 0;

  DIRH = 23;
	context = 0;
	devices[0]=8;
	devices[1]=9;
	dirs[0]=readDir(dirs[0],devices[0],(BYTE)0);
	dirs[1]=NULL;

	updateScreen(1);
	while(1)
    {
      c = cgetc();
    	switch (c)
      	{
        case '1':
        case CH_F1:
					dirs[context]=readDir(dirs[context],devices[context],context);
					clrDir(context);
					showDir(dirs[context],context);
					break;

        case '2':
        case CH_F2:
					if (++devices[context] >= 12)
            devices[context]=8;
					freeDir(&dirs[context]);
					dirs[context]=NULL;
					updateScreen(1);
					break;

        case '3':
        case CH_F3:
					cathex(devices[context],dirs[context]->selected->dirent.name);
					updateScreen(1);
					break;

        case '4':
        case CH_F4:
					catasc(devices[context],dirs[context]->selected->dirent.name);
					updateScreen(1);
					break;

		    case 't':
					cwd=GETCWD;
					cwd->selected=cwd->firstelement;
					cwd->pos=0;
					printDir(cwd,(context==0)?DIR1X+1:DIR2X+1,(context==0)?DIR1Y:DIR2Y);
          break;
		    case 'b':
					cwd=GETCWD;
					current = cwd->firstelement;
					pos=0;
					while (1)
            {
              if (current->next!=NULL)
                {
                  current=current->next;
                  pos++;
                }
              else
                {
                  break;
                }
            }
					cwd->selected=current;
					cwd->pos=pos;
					printDir(cwd,(context==0)?DIR1X+1:DIR2X+1,(context==0)?DIR1Y:DIR2Y);
					break;

		    case 'q':
          goto done;

        case '.':
					about("DraBrowse");
          break;

    		case CH_CURS_DOWN:
					cwd=GETCWD;
					if (cwd->selected!=NULL && cwd->selected->next!=NULL)
            {
              cwd->selected=cwd->selected->next;
              pos=cwd->pos;
              lastpage=pos/DIRH;
              nextpage=(pos+1)/DIRH;
              if (lastpage!=nextpage)
                {
                  cwd->pos++;
                  printDir(cwd,(context==0)?DIR1X+1:DIR2X+1,(context==0)?DIR1Y:DIR2Y);
                }
              else
                {
                  printElement(cwd,(context==0)?DIR1X+1:DIR2X+1,(context==0)?DIR1Y:DIR2Y);
                  cwd->pos++;
                  printElement(cwd,(context==0)?DIR1X+1:DIR2X+1,(context==0)?DIR1Y:DIR2Y);
                }

            }
          break;

    		case CH_CURS_UP:
					cwd=GETCWD;
					if (cwd->selected!=NULL && cwd->selected->previous!=NULL)
            {
              cwd->selected=cwd->selected->previous;
              pos=cwd->pos;
              lastpage=pos/DIRH;
              nextpage=(pos-1)/DIRH;
              if (lastpage!=nextpage)
                {
                  cwd->pos--;
                  printDir(cwd,(context==0)?DIR1X+1:DIR2X+1,(context==0)?DIR1Y:DIR2Y);
                }
              else
                {
                  printElement(cwd,(context==0)?DIR1X+1:DIR2X+1,(context==0)?DIR1Y:DIR2Y);
                  cwd->pos--;
                  printElement(cwd,(context==0)?DIR1X+1:DIR2X+1,(context==0)?DIR1Y:DIR2Y);
                }
            }
          break;

          // --- start / enter directory
		    case 13:
					cwd=GETCWD;
					if (cwd->selected!=NULL && !(cwd->selected->dirent.type==DIRTYPE))
            {
              execute(dirs[context]->selected->dirent.name,devices[context]);
              exit(0);
            }
					// else fallthrough to CURS_RIGHT

    		case CH_CURS_RIGHT:
					cwd=GETCWD;
					if (cwd->selected!=NULL)
            {
              sprintf(linebuffer,"cd:%s\n",cwd->selected->dirent.name);
              cmd(devices[context],linebuffer);
              refreshDir();
            }
					break;

          // --- leave directory
        case 20:  // backspace
    		case CH_CURS_LEFT:
					strcpy(linebuffer,"cd: \n");
					linebuffer[3]=95; // arrow left
					cmd(devices[context],linebuffer);
					refreshDir();
					break;
        }
    }

 done:;
  // nobody cares about freeing memory upon program exit.
#if 0
  if (dirs[0]!=NULL)
    freeDir(&dirs[0]);
#endif
}
