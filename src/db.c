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

	++menuy;
	gotoxy(MENUXT,++menuy);
	cputs(" F1 DIR");
	gotoxy(MENUXT,++menuy);
	cputs(" F2 DEVICE");
	gotoxy(MENUXT,++menuy);
	cputs(" F3 HEX");
	gotoxy(MENUXT,++menuy);
	cputs(" F4 ASC");
	gotoxy(MENUXT,++menuy);
	cputs(" CR RUN/CD");
	gotoxy(MENUXT,++menuy);
	cputs(" BS DIR UP");
	gotoxy(MENUXT,++menuy);
	cputs("  T TOP");
	gotoxy(MENUXT,++menuy);
	cputs("  B BOTTOM");
	gotoxy(MENUXT,++menuy);
	cputs("  . ABOUT");
	gotoxy(MENUXT,++menuy);
	cputs("  Q QUIT");
	++menuy;
	gotoxy(MENUXT,++menuy);
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
  BYTE context = 0;

  DIR1H = 23;
  DIR2H = 23;
	devices[0]=8;
	devices[1]=9;
	dirs[0]=readDir(dirs[0],devices[0],(BYTE)0);
	dirs[1]=NULL;

	updateScreen(context, 1);
	while(1)
    {
      c = cgetc();
    	switch (c)
      	{
        case '1':
        case CH_F1:
          textcolor(COLOR_WHITE);
					dirs[context]=readDir(dirs[context],devices[context],context);
					showDir(context, dirs[context], context);
					break;

        case '2':
        case CH_F2:
					if (++devices[context] > 11)
            devices[context]=8;
					freeDir(&dirs[context]);
					showDir(context, dirs[context], context);
					break;

        case '3':
        case CH_F3:
					cathex(devices[context],dirs[context]->selected->dirent.name);
					updateScreen(context, 1);
					break;

        case '4':
        case CH_F4:
					catasc(devices[context],dirs[context]->selected->dirent.name);
					updateScreen(context, 1);
					break;

		    case 't':
        case CH_HOME:
					cwd=GETCWD;
					cwd->selected=cwd->firstelement;
					cwd->pos=0;
					printDir(context, cwd, DIRX+1, DIRY);
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
					printDir(context, cwd, DIRX+1, DIRY);
					break;

		    case 'q':
          goto done;

        case '.':
					about("DraBrowse");
          updateScreen(context, 1);
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
                  printDir(context, cwd, DIRX+1, DIRY);
                }
              else
                {
                  printElement(context, cwd, DIRX+1, DIRY);
                  cwd->pos++;
                  printElement(context, cwd, DIRX+1, DIRY);
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
                  printDir(context, cwd, DIRX+1, DIRY);
                }
              else
                {
                  printElement(context, cwd, DIRX+1, DIRY);
                  cwd->pos--;
                  printElement(context, cwd, DIRX+1, DIRY);
                }
            }
          break;

          // --- start / enter directory
		    case CH_ENTER:
					cwd=GETCWD;
					if (cwd->selected && cwd->selected->dirent.type==CBM_T_PRG)
            {
              execute(dirs[context]->selected->dirent.name,devices[context]);
            }
					// else fallthrough to CURS_RIGHT

    		case CH_CURS_RIGHT:
					cwd=GETCWD;
					if (cwd->selected)
            {
              changeDir(context, devices[context], cwd->selected->dirent.name);
            }
					break;

          // --- leave directory
        case CH_DEL:
    		case CH_CURS_LEFT:
          {
            char buf[2];
            buf[0] = CH_LARROW;
            buf[1] = 0;
            changeDir(context, devices[context], buf);
          }
					break;

        case CH_UARROW:
          changeDir(context, devices[context], NULL);
          break;
        }
    }

 done:;
  // nobody cares about freeing memory upon program exit.
#if 0
  freeDir(&dirs[0]);
#endif
}
