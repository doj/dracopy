/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */
/*
 * dc.c
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
 */
/*
 * this code is now maintained on https://github.com/doj/dracopy
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <screen.h>
#include <conio.h>
#include <cbm.h>
#include <errno.h>
#include "cat.h"
#include "dir.h"
#include "base.h"
#include "defines.h"
#include "version.h"
#include "ops.h"

/* declarations */
BYTE really(void);
void doFormat(const BYTE context);
void doRename(const BYTE context);
void doMakedir(const BYTE context);
void doToggleAll(const BYTE context);
void doCopyMulti(const BYTE context);
void doCopySelected(const BYTE context);
void doDeleteMulti(const BYTE context);
int doDiskCopy(const BYTE deviceFrom, const BYTE deviceTo);
int copy(const char *srcfile, const BYTE srcdevice, const char *destfile, const BYTE destdevice, BYTE type);
void deleteSelected(const BYTE context);

extern BYTE devices[];
extern char linebuffer[];
extern char answer[];
extern Directory* dirs[];
extern const BYTE textc;

/* definitions */
char menustatus[MENUW];
static BYTE windowState = 0;

void
updateMenu(void)
{
	BYTE menuy=MENUY;

	revers(0);
	textcolor(textc);
	drawFrame(" " DRA_VERNUM " ",MENUX,MENUY,MENUW,MENUH,menustatus);

	gotoxy(MENUX+1,++menuy);
	cputs("F1 READ DIR");
	gotoxy(MENUX+1,++menuy);
	cputs("F2 DEVICE");
	gotoxy(MENUX+1,++menuy);
	cputs("F3 VIEW HEX");
	gotoxy(MENUX+1,++menuy);
	cputs("F4 VIEW ASC");
	gotoxy(MENUX+1,++menuy);
	cputs("F5 COPY MUL");
	gotoxy(MENUX+1,++menuy);
	cputs("F6 DEL MUL");
	gotoxy(MENUX+1,++menuy);
	cputs("F7 RUN");
	gotoxy(MENUX+1,++menuy);
	cputs("F8 DISKCOPY");
	gotoxy(MENUX+1,++menuy);
	cputs("SP TAG");
	gotoxy(MENUX+1,++menuy);
#ifdef __PLUS4__
	cputs("ESC SWITCH");
#else
	cputc(' ');
	cputc(CH_LARROW); // arrow left
	cputs(" SWITCH W");
#endif
	gotoxy(MENUX+1,++menuy);
	cputs("CR CHG DIR");
	gotoxy(MENUX+1,++menuy);
	cputs("BS DIR UP");
	gotoxy(MENUX+1,++menuy);
	cputs(" T TOP");
	gotoxy(MENUX+1,++menuy);
	cputs(" B BOTTOM");
	gotoxy(MENUX+1,++menuy);
	cputs(" * INV SEL");
	gotoxy(MENUX+1,++menuy);
	cputs(" C COPY F");
	gotoxy(MENUX+1,++menuy);
	cputs(" D DEL F/D");
	gotoxy(MENUX+1,++menuy);
	cputs(" R RENAME");
	gotoxy(MENUX+1,++menuy);
	cputs(" M MAKE DIR");
	gotoxy(MENUX+1,++menuy);
	cputs(" F FORMAT");
	gotoxy(MENUX+1,++menuy);
	cputs(" . ABOUT");
	gotoxy(MENUX+2,++menuy);
	cputc(0x5c); // pound
	cputs(" DEV ID");
	gotoxy(MENUX+1,++menuy);
	cputs(" @ DOS CMD");
}

void
mainLoop(void)
{
	Directory * cwd = NULL;
	DirElement * current = NULL;
  unsigned int pos = 0;
	BYTE lastpage = 0;
	BYTE nextpage = 0;
  BYTE context = 0;

  initDirWindowHeight();

  dirs[0] = dirs[1] = NULL;
	updateScreen(context, 2);

  textcolor(COLOR_WHITE);
	devices[context] = 8;
	dirs[context] = readDir(NULL, devices[context], context);
  showDir(context, dirs[context], context);

  textcolor(textc);
	devices[1] = 9;
	dirs[1] = readDir(NULL, devices[1], 1);
  showDir(context, dirs[1], 1);

	while(1)
    {
      const BYTE c = cgetc();
    	switch (c)
      	{
        case '1':
        case CH_F1:
          textcolor(COLOR_WHITE);
					dirs[context]=readDir(dirs[context],devices[context],context);
          debugs("D F1 1");
					showDir(context, dirs[context], context);
          debugs("D F1 2");
					break;

        case '2':
        case CH_F2:
          // cycle through devices until we found the next one, not
          // used by the other context.
					do
            {
              if (++devices[context] > 11)
                devices[context] = 8;
            }
					while(devices[context]==devices[context^1]);
					freeDir(&dirs[context]);
					showDir(context, dirs[context], context);
					break;

        case '3':
        case CH_F3:
					cathex(devices[context],dirs[context]->selected->dirent.name);
					updateScreen(context, 2);
					break;

        case '4':
        case CH_F4:
					catasc(devices[context],dirs[context]->selected->dirent.name);
					updateScreen(context, 2);
					break;

        case '5':
        case CH_F5:
					doCopyMulti(context);
          debugs("D12");
					updateScreen(context, 2);
					// refresh destination dir
          {
            const BYTE other_context = context^1;
            debugs("D14");
            dirs[other_context] = readDir(dirs[other_context], devices[other_context], other_context);
            debugs("D14b");
            showDir(context, dirs[other_context], other_context);
          }
          debugs("D15");
					break;

        case '6':
        case CH_F6:
					doDeleteMulti(context);
          debugs("D16");
					break;

        case '7':
        case CH_F7:
					if (dirs[context]->selected)
            {
              execute(dirs[context]->selected->dirent.name,devices[context]);
            }
					break;

        case '8':
        case CH_F8:
          {
            const BYTE other_context = context^1;
            int ret = doDiskCopy(devices[context], devices[other_context]);
            updateScreen(context, 2);
            dirs[other_context] = readDir(dirs[other_context], devices[other_context], other_context);
          }
					break;

          // ----- switch context -----
        case '0':
        case CH_ESC:
		    case CH_LARROW:  // arrow left
          {
            Directory *oldcwd = GETCWD;
            context = context ^ 1;
            drawDirFrame(context, oldcwd, context^1);
            drawDirFrame(context, GETCWD, context);
          }
					break;

        case 't':
		    case CH_HOME:
					cwd=GETCWD;
					cwd->selected=cwd->firstelement;
					cwd->pos=0;
					printDir(context, cwd, DIRX+1, DIRY);
          break;

		    case 'b':
          cwd = GETCWD;
          current = cwd->firstelement;
          while (1)
            {
              if (current->next!=NULL)
                {
                  current=current->next;
                  ++pos;
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

        case 'n':
          // todo next page
          break;

        case 'p':
          // todo prev page
          break;

		    case 'q':
					goto done;
          break;

        case ' ':
					cwd=GETCWD;
					cwd->selected->flags=!cwd->selected->flags;

					// go to next entry
					if (cwd->selected->next!=NULL)
            {
              cwd->selected=cwd->selected->next;
              cwd->pos++;
            }
					showDir(context, cwd, context);
          break;

        case 'd':
					deleteSelected(context);
          break;

        case 'c':
					doCopySelected(context);
          break;

        case 'f':
					doFormat(context);
          break;

        case 'r':
					doRename(context);
          break;

        case 'm':
					doMakedir(context);
          break;

        case '*':
					doToggleAll(context);
          break;

        case '.':
					about("DraCopy");
          updateScreen(context, 2);
          break;

        case '@':
          // TODO: DOS command
          break;

        case CH_POUND:
          changeDeviceID(devices[context]);
          updateScreen(context, 2);
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

          // --- enter directory
    		case CH_ENTER:
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
            buf[0] = CH_LARROW; // arrow left
            buf[1] = 0;
            changeDir(context, devices[context], buf);
          }
					break;

        case CH_UARROW:
          changeDir(context, devices[context], NULL);
          break;

        case 'w':
          switch(++windowState)
            {
            default:
            case 0:
              windowState = 0;
              initDirWindowHeight();
              break;
            case 1:
              DIR1H += DIR2H - 2;
              DIR2H = 2;
              break;
            case 2:
              {
                const BYTE tmp = DIR1H;
                DIR1H = DIR2H;
                DIR2H = tmp;
              }
              break;
            }
          showDir(context, dirs[0], 0);
          showDir(context, dirs[1], 1);
          break;
        }
    }

 done:;
  // nobody cares about freeing memory upon program exit.
#if 0
  freeDir(&dirs[0]);
  freeDir(&dirs[1]);
#endif
}

void
doCopySelected(const BYTE context)
{
  const BYTE other_context = 1 - context;
  int ret;
  Directory * cwd = GETCWD;

  if (cwd->selected == NULL)
    return;

  sprintf(linebuffer ,"Filecopy from device %d to device %d", devices[context], devices[other_context]);
  newscreen(linebuffer);
  ret = copy(cwd->selected->dirent.name,
             devices[context],
             cwd->selected->dirent.name,
             devices[other_context],
             cwd->selected->dirent.type);
  if (ret == ERROR)
    {
      cputc(CH_ENTER);
      cputc(10);
      waitKey(0);
    }

  // refresh destination dir
  updateScreen(context, 2);
  dirs[other_context] = readDir(dirs[other_context], devices[other_context], other_context);

  if (devices[0]==devices[1])
    {
      // refresh also source dir if it's the same drive
      dirs[context] = readDir(dirs[context],devices[context],context);
    }
}

void
deleteSelected(const BYTE context)
{
  DirElement * lastSel;
  Directory * cwd = GETCWD;

	if (cwd->selected == NULL)
    return;

  sprintf(linebuffer, " Delete file/directory on device %d", devices[context]);
  newscreen(linebuffer);
  lastSel = cwd->selected;
  cprintf("%s.%s\n\r", cwd->selected->dirent.name, fileTypeToStr(cwd->selected->dirent.type));
  if (really())
    {
      if (cwd->selected->dirent.type==DIRTYPE)
        {
          sprintf(linebuffer,"rd:%s",cwd->selected->dirent.name);
        }
      else
        {
          sprintf(linebuffer,"s:%s",cwd->selected->dirent.name);
        }
      if (cmd(devices[context],linebuffer)==1)
        {
          removeFromDir(cwd->selected);
          cputc(' ');
          revers(1);
          cputs("DELETED\n\r");
          revers(0);

          // select next / prior entry
          if (cwd->selected->next!=NULL)
            {
              cwd->selected=cwd->selected->next;
              //cwd->pos++;
            }
          else if (cwd->selected->previous!=NULL)
            {
              cwd->selected=cwd->selected->previous;
              cwd->pos--;
            }
          else
            {
              cwd->selected=NULL;
              cwd->pos=0;
            }

          // update first element if needed
          if (cwd->firstelement==lastSel)
            {
              cwd->firstelement=cwd->firstelement->next;
            }
        }
      else
        {
          cputc(' ');
          revers(1);
          textcolor(COLOR_SIGNAL);
          cputs("ERROR\n\r");
          textcolor(textc);
          revers(0);
          waitKey(0);
        }
    }
  updateScreen(context, 2);
}

void
doCopyMulti(const BYTE context)
{
  BYTE cnt = 0xf0;
	DirElement * current;

	Directory *srcdir = dirs[context];
	Directory *destdir = dirs[context^1];

	const BYTE srcdev = devices[context];
	const BYTE destdev = devices[context^1];

	if (srcdir==NULL || destdir==NULL)
    return;

	sprintf(linebuffer,"Filecopy from device %d to device %d",srcdev,destdev);
  for(current = srcdir->firstelement; current; current=current->next)
    {
      if (++cnt >= 24)
        {
          cnt = 0;
          newscreen(linebuffer);
        }
      if (current->flags==1)
        {
          int ret = copy(current->dirent.name, srcdev, current->dirent.name, destdev, current->dirent.type);
          if (ret == OK)
            {
              // deselect
              current->flags=0;
            }
          else if (ret == ABORT)
            {
              return;
            }
        }
    }
}

void
doToggleAll(const BYTE context)
{
	DirElement * current;
	if (dirs[context]==NULL)
    {
      //cputs("no directory");
      return;
    }
	else
    {
      current = dirs[context]->firstelement;
      while (current!=NULL)
        {
          current->flags=1-(current->flags);
          current=current->next;
        }
      showDir(context, dirs[context], context);
    }
}

BYTE
really(void)
{
	char c;
	cputs("Really (Y/N)? ");
	c = cgetc();
	cputc(c);
	cputs("\n\r");
	return (c=='y' || c=='Y');
}

void
doDeleteMulti(const BYTE context)
{
	DirElement * current;
	int idx = 0;
	Directory * cwd = GETCWD;

	if (dirs[context]==NULL)
    return;

  for(current = dirs[context]->firstelement; current; current=current->next)
    {
      if (current->flags)
        ++idx;
    }

  if (idx == 0)
    return;

  sprintf(linebuffer, "Delete %i files from device %d", idx, devices[context]);
  newscreen(linebuffer);
  if (! really())
    {
      updateScreen(context, 2);
      return;
    }

  idx = 3;
  for(current = dirs[context]->firstelement; current; current=current->next)
    {
      if (! current->flags)
        continue;

      gotoxy(0,idx);
      cprintf("%s.%s ", current->dirent.name, fileTypeToStr(current->dirent.type));

      sprintf(linebuffer,"s:%s",current->dirent.name);
      if (cmd(devices[context],linebuffer)==1)
        {
          revers(1);
          cputs("DELETED");
          revers(0);
        }
      else
        {
          revers(1);
          textcolor(COLOR_VIOLET);
          puts("ERROR");
          textcolor(textc);
          revers(0);
          break;
        }

      if (++idx > 24)
        {
          newscreen(linebuffer);
          idx = 1;
        }
    }

  updateScreen(context, 2);
  // refresh directories
  debugs("D9");
  cwd = readDir(cwd, devices[context], context);
  dirs[context]=cwd;
  cwd->selected=cwd->firstelement;
  debugs("D10");
  showDir(context, cwd, context);
  if (devices[0] == devices[1])
    {
      const BYTE other_context = context^1;
      dirs[other_context] = cwd;
      showDir(context, cwd, other_context);
    }
}

void
doFormat(const BYTE context)
{
  BYTE flag = 0;
	sprintf(linebuffer,"Format device %d",devices[context]);
	newscreen(linebuffer);
	if (really())
    {
      cputs("\n\rEnter new name: ");
      scanf ("%s",answer);
      if (strlen(answer) == 0)
        goto done;
      flag = 1;
      cputs("\n\rWorking...");
      sprintf(linebuffer,"n:%s",answer);
      if(cmd(devices[context],linebuffer) != OK)
        {
          cputs("ERROR\n\r");
          waitKey(0);
        }
    }
 done:
	updateScreen(context, 2);
  if (flag)
    {
      dirs[context] = readDir(dirs[context], devices[context], context);
      showDir(context, dirs[context], context);
    }
}

void
doRename(const BYTE context)
{
	int n;
	Directory * cwd = GETCWD;

	if (cwd->selected == NULL)
    return;

  sprintf(linebuffer,"Rename file %s on device %d",cwd->selected->dirent.name,devices[context]);
  newscreen(linebuffer);
  cputs("\n\rNew name (enter to skip): ");
  n=scanf ("%s",answer);
  if (n==1)
    {
      cputs("\n\rWorking...");
      sprintf(linebuffer,"r:%s=%s",answer,cwd->selected->dirent.name);
      if(cmd(devices[context],linebuffer)==OK)
        {
          updateScreen(context, 2);
          dirs[context] = readDir(dirs[context],devices[context],context);
          return;
        }
      else
        {
          cputs("ERROR\n\r\n\r");
          waitKey(0);
        }
    }
  updateScreen(context, 2);
}

void
doMakedir(const BYTE context)
{
	int n;
	Directory * cwd = GETCWD;

	sprintf(linebuffer,"Make directory on device %d",cwd->selected->dirent.name,devices[context]);
	newscreen(linebuffer);
	cputs("\n\rName (enter to skip): ");
	n=scanf ("%s",answer);
	if (n==1)
    {
      cputs("\n\rWorking...");
      sprintf(linebuffer,"md:%s",answer);
      if(cmd(devices[context],linebuffer)==OK)
        {
          updateScreen(context, 2);
          dirs[context] = readDir(dirs[context],devices[context],context);
          return;
        }
      else
        {
          cputs("ERROR\n\r\n\r");
          waitKey(0);
        }
    }
	updateScreen(context, 2);
}

int
copy(const char *srcfile, const BYTE srcdevice, const char *destfile, const BYTE destdevice, BYTE type)
{
	int length=0;
	BYTE *buf;
  int ret = OK;
  unsigned long total_length = 0;

  switch(type)
    {
    case _CBM_T_SEQ:
      type = 's';
      break;
    case _CBM_T_PRG:
      type = 'p';
      break;
    case _CBM_T_USR:
      type = 'u';
      break;
    default:
      return ERROR;
    }

	if (cbm_open(6,srcdevice,CBM_READ,srcfile) != 0)
    {
      cputs("Can't open input file!\n");
      return ERROR;
    }

	// create destination string with filetype like "FILE,P"
	sprintf(linebuffer, "%s,%c", destfile, type);
	if (cbm_open(7, destdevice, CBM_WRITE, linebuffer) != 0)
    {
      cputs("Can't open output file\n");
      cbm_close (6);
      return ERROR;
    }

	buf = (unsigned char *) malloc(BUFFERSIZE);

	cprintf("%-16s:",srcfile);
	while(1)
    {
      if (kbhit())
        {
          char c = cgetc();
          if (c == CH_ESC || c == CH_LARROW)
            {
              ret = ABORT;
              break;
            }
        }

	  	cputs("R");
      length = cbm_read (6, buf, BUFFERSIZE);
      if (length < 0)
        {
#define ERRMSG(color,msg) \
          cputc(' ');     \
          revers(1);            \
          textcolor(color);     \
          cputs(msg);           \
          textcolor(textc);     \
          revers(0);            \
          cputs("\r\n");

          ERRMSG(COLOR_YELLOW,"READ ERROR");
          ret = ERROR;
          break;
        }

      if (kbhit())
        {
          char c = cgetc();
          if (c == CH_ESC || c == CH_LARROW)
            {
              ret = ABORT;
              ERRMSG(COLOR_YELLOW,"ABORT");
              break;
            }
        }

      if (length > 0)
        {
          cputs("W");
          if (cbm_write(7, buf, length) != length)
            {
              ERRMSG(COLOR_YELLOW,"WRITE ERROR");
              break;
            }
          total_length += length;
        }

      if (length < BUFFERSIZE)
        {
          cprintf(" %lu bytes", total_length);
          ERRMSG(textc,"OK");
          break;
        }

    }

	free(buf);
	cbm_close (6);
	cbm_close (7);
	return ret;
}

/*
  int printErrorChannel(BYTE device)
  {
  unsigned char buf[128];
  unsigned char msg[100]; // should be large enough for all messages
  unsigned char e, t, s;

  if (cbm_open(15, device, 15, "") == 0)
  {
  if (cbm_read(1, buf, sizeof(buf)) < 0) {
  puts("can't read error channel");
  return(1);
  }
  cbm_close(15);
  }
  if (	sscanf(buf, "%hhu, %[^,], %hhu, %hhu", &e, msg, &t, &s) != 4) {
  puts("parse error");
  puts(buf);
  return(2);
  }
  printf("%hhu,%s,%hhu,%hhu\n", (int) e, msg, (int) t, (int) s);
  return(0);
  }
*/

const char sectors1541[42] = {
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
  19, 19, 19, 19, 19, 19, 19,
  18, 18, 18, 18, 18, 18,
  17, 17, 17, 17, 17,
  17, 17, 17, 17, 17, // track 36-40
  17, 17 // track 41-42
};

const char sectors1571[70] = {
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
  19, 19, 19, 19, 19, 19, 19,
  18, 18, 18, 18, 18, 18,
  17, 17, 17, 17, 17,
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
  19, 19, 19, 19, 19, 19, 19,
  18, 18, 18, 18, 18, 18,
  17, 17, 17, 17, 17
};

BYTE diskCopyBuf[256];

#define IS_1541(dt) (dt == D1540 || dt == D1541 || dt == D1551 || dt == D1570 || dt == SD2IEC)

BYTE
maxTrack(BYTE dt)
{
  if (IS_1541(dt))
    return 42;
  else if (dt == D1571)
    return 70;
  else if (dt == D1581)
    return 80;
  return 0;
}

BYTE
maxSector(BYTE dt, BYTE t)
{
  if (IS_1541(dt))
    return sectors1541[t];
  if (dt == D1571)
    return sectors1571[t];
  if (dt == D1581)
    return 40;
  return 0;
}

void
printSecStatus(BYTE dt, BYTE t, BYTE s, BYTE st)
{
  if (t & 0x80)
    {
      textcolor(COLOR_RED);
      t &= 0x7f;
    }
  else
    {
      textcolor(COLOR_GRAY3);
    }
  if (IS_1541(dt))
    {
      if (t >= 35)
        textcolor(COLOR_GRAY1);
      if (t >= 40)
        t = 39;
    }
  else if (dt == D1571 && t >= 35)
    {
      t -= 35;
      textcolor(COLOR_WHITE);
    }
  else if (dt == D1581)
    {
      if (t >= 40)
        {
          textcolor(COLOR_WHITE);
          t -= 40;
        }
      if ((s&1) == 0)
        {
          if (st == 'R') st = 'r';
          else if (st == 'W') st = 'w';
        }
      s >>= 1;
    }
  gotoxy(t, 3+s);
  cputc(st);
}

int
doDiskCopy(const BYTE deviceFrom, const BYTE deviceTo)
{
  int ret;
  const BYTE dt = devicetype[deviceFrom];
  const BYTE max_track = maxTrack(dt);
  BYTE i = devicetype[deviceTo];
  BYTE track = maxTrack(i);

	sprintf(linebuffer, "Copy disk from device %d to %d? (Y/N)", deviceFrom, deviceTo);
  newscreen(linebuffer);

  if (max_track != track)
    {
      cprintf("\n\rcan't copy from %s (%i tracks)\n\rto %s (%i tracks)", drivetype[dt], max_track, drivetype[i], track);
      cgetc();
      return ERROR;
    }

  if (max_track == 0)
    {
      cprintf("\n\rcan't copy drive type %s", drivetype[dt]);
      cgetc();
      return ERROR;
    }

  while(1)
    {
      i = cgetc();
      if (i == 'y')
        break;
      if (i == 'n' ||
          i == CH_ESC ||
          i == CH_LARROW)
        return 1;
    }

  cputs("0000000001111111111222222222233333333334");
  gotoxy(0,2);
  cputs("1234567890123456789012345678901234567890");
  for(track = 0; track < 40; ++track)
    {
      BYTE max_s = 40;
      BYTE sector;
      if (IS_1541(dt))
        max_s = sectors1541[track];
      else if (dt == D1571)
        {
          if (track == 35)
            break;
          max_s = sectors1571[track];
        }
      for(sector = 0; sector < max_s; ++sector)
        {
          printSecStatus(dt, track, sector, '.');
        }
    }

  if ((i = cbm_open(9, deviceFrom, 5, "#")) != 0)
    {
      sprintf(diskCopyBuf, "device %i: open data failed: %i", deviceFrom, i);
      goto error;
    }
  if ((i = cbm_open(6, deviceFrom, 15, "")) != 0)
    {
      sprintf(diskCopyBuf, "device %i: open cmd failed: %i", deviceFrom, i);
      goto error;
    }
  if ((i = cbm_open(7, deviceTo,   5, "#")) != 0)
    {
      sprintf(diskCopyBuf, "device %i: open data failed: %i", deviceTo, i);
      goto error;
    }
  if ((i = cbm_open(8, deviceTo,   15, "")) != 0)
    {
      sprintf(diskCopyBuf, "device %i: open cmd failed: %i", deviceTo, i);
      goto error;
    }

  for(track = 0; track < max_track; ++track)
    {
      const BYTE max_sector = maxSector(dt, track);
      BYTE sector;

      // check if 1541 writes to extra tracks
      if (IS_1541(dt)
          && track >= 40)
        {
          gotoxy(39,2);
          textcolor(textc);
          cputc('1'+track-40);
        }
      // check if 1571 writes on back side
      if (dt == D1571 && track == 35)
        {
          textcolor(textc);
          gotoxy(0,1);
          cputs("33334444444444555555555566666666667     ");
          gotoxy(0,1);
          cputs("67890123456789012345678901234567890     ");
        }
      // check if 1581 writes on back side
      if (dt == D1581 && track == 40)
        {
          textcolor(textc);
          gotoxy(0,1);
          cputs("4444444445555555555666666666677777777778");
        }

      for(sector = 0; sector < max_sector; ++sector)
        {
          if (kbhit())
            {
              i = cgetc();
              if (i == CH_ESC || i == CH_LARROW)
                {
                  goto done;
                }
            }

          printSecStatus(dt, track, sector, 'R');
          ret = cbm_write(6, linebuffer, sprintf(linebuffer, "u1:5 0 %d %d", track + 1, sector));
          if (ret < 0)
            {
              sprintf(diskCopyBuf, "read sector %i/%i failed: %i", track+1, sector, _oserror);
#define SECTOR_ERROR                                    \
              gotoxy(0,24);                             \
              textcolor(COLOR_LIGHTRED);                \
              cputs(diskCopyBuf);                       \
              printSecStatus(dt, track|0x80, sector, 'E');
              SECTOR_ERROR;
              continue;
            }

          ret = cbm_write(8, "b-p:5 0", 7);
          if (ret < 0)
            {
              sprintf(diskCopyBuf, "setup buffer failed: %i", track+1, sector, _oserror);
              SECTOR_ERROR;
              continue;
            }

          ret = cbm_read(9, diskCopyBuf, 256);
          if (ret != 256)
            {
              // check for expected failures at the end of a disk
              if (IS_1541(dt)
                  && track >= 35
                  && sector == 0)
                {
                  ret = 0;
                  goto success;
                }
              if (dt == D1571 && track == 35 && sector == 0)
                {
                  ret = 0;
                  goto success;
                }
              sprintf(diskCopyBuf, "read %i/%i failed: %i", track+1, sector, _oserror);
              SECTOR_ERROR;
              continue;
            }

          printSecStatus(dt, track, sector, 'W');
          ret = cbm_write(7, diskCopyBuf, 256);
          if (ret != 256)
            {
              sprintf(diskCopyBuf, "write %i/%i failed: %i", track+1, sector, _oserror);
              SECTOR_ERROR;
              continue;
            }

          ret = cbm_write(8, linebuffer, sprintf(linebuffer, "u2:5 0 %d %d", track + 1, sector));
          if (ret < 0)
            {
              sprintf(diskCopyBuf, "write cmd %i/%i failed: %i", track+1, sector, _oserror);
              SECTOR_ERROR;
              continue;
            }
        }

      if (IS_1541(dt))
        {
          gotoxy(0,24);
          textcolor(COLOR_LIGHTBLUE);
          switch(track)
            {
            case 5: cputs("this will take a while"); break;
            case 6: cputs("                      "); break;
            case 17: cputs("halftime"); break;
            case 18: cputs("        "); break;
            case 31: cputs("almost there"); break;
            case 32: cputs("any minute now"); break;
            case 33: cputs("writing last track!!!"); break;
            case 35: cputs("this disk is oversized"); break;
            }
        }
      else if (dt == D1571)
        {
          gotoxy(0,24);
          textcolor(COLOR_LIGHTBLUE);
          switch(track)
            {
            case 35: cputs("copy the back side"); break;
            case 69: cputs("writing last track!!!"); break;
            }
        }
      else if (dt == D1581)
        {
          gotoxy(0,24);
          textcolor(COLOR_LIGHTBLUE);
          switch(track)
            {
            case 40: cputs("copy the back side"); break;
            case 79: cputs("writing last track!!!"); break;
            }
        }
    }

 success:
  gotoxy(0,24);
  textcolor(COLOR_LIGHTBLUE);
  cputs("disk copy success          ");
  ret = 0;
  goto done;

 error:
  ret = -1;
  gotoxy(0,24);
  textcolor(COLOR_LIGHTRED);
  cputs(diskCopyBuf);
  cgetc();

 done:
  cbm_close(6);
  cbm_close(7);
  cbm_close(8);
  cbm_close(9);

  textcolor(textc);
  return ret;
}
