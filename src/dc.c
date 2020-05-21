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
void doFormat(void);
void doRename(void);
void doMakedir(void);
void doDelete(void);
void doToggleAll(void);
void doCopy(void);
void doCopySelected(void);
void doDeleteMulti(void);
int doDiskCopy(const BYTE deviceFrom, const BYTE deviceTo);
int copy(char * srcfile, BYTE srcdevice, char * destfile, BYTE destdevice, BYTE type);
void deleteSelected(void);

extern BYTE context;
extern BYTE devices[];
extern char linebuffer[];
extern Directory* dirs[];
extern const BYTE textc;

/* definitions */
char answer[40];
char menustatus[MENUW];

void
updateMenu(void)
{
	BYTE menuy=MENUY;

	revers(0);
	textcolor(textc);
	drawFrame(" " DRA_VERNUM " ",MENUX,MENUY,MENUW,MENUH,menustatus);

	menuy+=1;
	gotoxy(MENUX+1,menuy++);
	cputs("F1 READ DIR");
	gotoxy(MENUX+1,menuy++);
	cputs("F2 DEVICE");
	gotoxy(MENUX+1,menuy++);
	cputs("F3 VIEW HEX");
	gotoxy(MENUX+1,menuy++);
	cputs("F4 VIEW ASC");
	gotoxy(MENUX+1,menuy++);
	cputs("F5 COPY MUL");
	gotoxy(MENUX+1,menuy++);
	cputs("F6 DEL MUL");
	gotoxy(MENUX+1,menuy++);
	cputs("F7 RUN");
	gotoxy(MENUX+1,menuy++);
	cputs("F8 DISKCOPY");
	gotoxy(MENUX+1,menuy++);
	cputs("SP TAG");
	gotoxy(MENUX+1,menuy++);
#ifdef __PLUS4__
	cputs("ESC SWITCH");
#else
	cputc(' ');
	cputc(95); // arrow left
	cputs(" SWITCH W");
#endif
	gotoxy(MENUX+1,menuy++);
	cputs("CR CHG DIR");
	gotoxy(MENUX+1,menuy++);
	cputs("BS DIR UP");
	gotoxy(MENUX+1,menuy++);
	cputs(" T TOP");
	gotoxy(MENUX+1,menuy++);
	cputs(" B BOTTOM");
	gotoxy(MENUX+1,menuy++);
	cputs(" * INV SEL");
	gotoxy(MENUX+1,menuy++);
	cputs(" C COPY F");
	gotoxy(MENUX+1,menuy++);
	cputs(" D DEL F/D");
	gotoxy(MENUX+1,menuy++);
	cputs(" R REN F");
	gotoxy(MENUX+1,menuy++);
	cputs(" M MAKE DIR");
	gotoxy(MENUX+1,menuy++);
	cputs(" F FORMAT");
	gotoxy(MENUX+1,menuy++);
	cputs(" . ABOUT");
	gotoxy(MENUX+1,menuy++);
	cputs(" Q QUIT");
}

void
mainLoop(void)
{
	Directory * cwd = NULL;
	Directory * oldcwd;
	DirElement * current = NULL;
	unsigned int index = 0;
	unsigned int pos = 0;
	BYTE oldcontext;
	BYTE exitflag = 0;
	BYTE lfn = 8;
	BYTE lastpage = 0;
	BYTE nextpage = 0;

	context = 0;
	dirs[0]=NULL;
	dirs[1]=NULL;
	devices[0]=8;
	devices[1]=9;
	dirs[0]=readDir(dirs[0],devices[0],(BYTE)0);
	dirs[1]=readDir(dirs[1],devices[1],(BYTE)1);

	updateScreen(2);
	do
    {
      const BYTE c = cgetc();
    	switch (c)
      	{
    		case 10:

        case CH_F1:
					dirs[context]=readDir(dirs[context],devices[context],context);
					clrDir(context);
					showDir(dirs[context],context);
					break;

        case CH_F2:
					do
            {
              if (devices[context]++>12) devices[context]=8;
            }
					while(devices[context]==devices[1-context]);
					freeDir(&dirs[context]);
					dirs[context]=NULL;
					updateScreen(2);
					break;

        case CH_F3:
					cathex(devices[context],dirs[context]->selected->dirent.name);
					updateScreen(2);
					break;

        case CH_F4:
					catasc(devices[context],dirs[context]->selected->dirent.name);
					updateScreen(2);
					break;

        case CH_F5:
					doCopy();
					clrscr();
					// refresh destination dir
          {
            const BYTE other_context = 1-context;
            dirs[other_context] = readDir(dirs[other_context], devices[other_context], other_context);
          }
					updateScreen(2);
					break;

        case CH_F6:
					doDeleteMulti();
					updateScreen(2);
					break;

        case CH_F7:
					if (dirs[context]->selected)
            {
              execute(dirs[context]->selected->dirent.name,devices[context]);
            }
					break;

        case CH_F8:
          {
            const BYTE other_context = 1-context;
            int ret = doDiskCopy(devices[context], devices[other_context]);
            if (ret == 0)
              {
                dirs[other_context] = readDir(dirs[other_context], devices[other_context], other_context);
              }
            updateScreen(2);
          }
					break;

          // ----- switch context -----
        case 27:  // escape
		    case 95:  // arrow left
					oldcwd=GETCWD;
					oldcontext=context;
					context=context ^ 1;
					cwd=GETCWD;
					showDir(oldcwd,oldcontext); // clear cursor
					showDir(cwd,context);    // draw cursor
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
					exitflag = 1;
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
					showDir(cwd,context);
          break;

        case 'd':
					deleteSelected();
          break;

        case 'c':
					doCopySelected();
          break;

        case 'f':
					doFormat();
          break;

        case 'r':
					doRename();
          break;

        case 'm':
					doMakedir();
          break;

        case '*':
					doToggleAll();
          break;

        case '.':
					about("DraCopy");
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

          // --- enter directory
    		case 13:  // cr
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
					sprintf(linebuffer,"cd: \n");
					linebuffer[3]=95; // arrow left
					cmd(devices[context],linebuffer);
					refreshDir();
					break;
        }
    }
	while(exitflag==0);

  // nobody cares about freeing memory upon program exit.
#if 0
  if (dirs[0]!=NULL) freeDir(&dirs[0]);
  if (dirs[1]!=NULL) freeDir(&dirs[1]);
#endif
}

void
doCopySelected(void)
{
  int ret;
  Directory * cwd = GETCWD;

  if (cwd->selected!=NULL)
    {
      sprintf(linebuffer,"Filecopy from device %d to device %d",devices[context],devices[1-context]);
      newscreen(linebuffer);
      ret = copy(cwd->selected->dirent.name,
                 devices[context],
                 cwd->selected->dirent.name,
                 devices[1-context],
                 cwd->selected->dirent.type);
      if (ret == ERROR)
        {
          cputc(13);
          cputc(10);
          waitKey(0);
        }

      // refresh destination dir
      clrscr();
      dirs[1-context] = readDir(dirs[1-context],devices[1-context],(BYTE)(1-context));

      if (devices[0]==devices[1])
        {
          // refresh also source dir if it's the same drive
          dirs[context] = readDir(dirs[context],devices[context],context);
        }

      updateScreen(2);
    }
}

void
deleteSelected(void)
{
  DirElement * lastSel;
  Directory * cwd = GETCWD;

	if (cwd->selected!=NULL)
    {
      sprintf(linebuffer,"Delete file/directory on device %d",devices[context]);
      newscreen(linebuffer);
      if (really())
        {
          lastSel = cwd->selected;
          cprintf("%s.%s",cwd->selected->dirent.name,fileTypeToStr(cwd->selected->dirent.type));
          if (cwd->selected->dirent.type==DIRTYPE)
            {
              sprintf(linebuffer,"rd:%s",cwd->selected->dirent.name);
            }
          else
            {
              sprintf(linebuffer,"s:%s",cwd->selected->dirent.name);
            }
          if (cmd(devices[context],linebuffer)==OK)
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
      updateScreen(2);
    }
}

void
doCopy(void)
{
  BYTE cnt = 0;
	DirElement * current;

	Directory * srcdir;
	Directory * destdir;

	const BYTE srcdev = devices[context];
	const BYTE destdev = devices[1-context];

	srcdir=dirs[context];
	destdir=dirs[1-context];

	sprintf(linebuffer,"Filecopy from device %d to device %d",srcdev,destdev);
	if (srcdir==NULL || destdir==NULL)
    {
      cputs("no directory");
      return;
    }
	else
    {
      current = srcdir->firstelement;
      while (current!=NULL)
        {
          if (cnt == 0)
            {
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
          current=current->next;
          if (++cnt >= 24)
            {
              cnt = 0;
            }
        }
    }
}

void
doToggleAll(void)
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
      showDir(dirs[context],context);
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
doDeleteMulti(void)
{
	DirElement * current;
	int idx = 0;
	int selidx = 0;
	int x=0;
	int sx=0;
	Directory * cwd = GETCWD;

	if (dirs[context]==NULL)
    {
      return;
    }
	else
    {
      sprintf(linebuffer,"Delete files from device %d",devices[context]);
      newscreen(linebuffer);
      current = dirs[context]->firstelement;
      while (current!=NULL)
        {
          if (current->flags==1)
            {
              cprintf("%s.%s\n\r", current->dirent.name, fileTypeToStr(current->dirent.type));
            }
          current=current->next;
        }
      cputs("\n\r");
      if (really())
        {
          current = dirs[context]->firstelement;
          while (current!=NULL)
            {
              if (current->flags==1)
                {
                  cprintf("%s.%s", current->dirent.name, fileTypeToStr(current->dirent.type));
                  sprintf(linebuffer,"s:%s\n",current->dirent.name);
                  if (cmd(devices[context],linebuffer)==OK)
                    {
                      cputc(' ');
                      revers(1);
                      cputs("DELETED\n\r");
                      revers(0);
                    }
                  else
                    {
                      cputc(' ');
                      revers(1);
                      textcolor(COLOR_VIOLET);
                      puts("ERROR\n\r");
                      textcolor(textc);
                      revers(0);
                      break;
                    }
                }
              current=current->next;
            }

          //waitKey(0);

          // refresh directories
          clrscr();
          cwd = readDir(cwd, devices[context], context );
          dirs[context]=cwd;
          cwd->selected=cwd->firstelement;
          if (devices[0]==devices[1])
            {
              // refresh also other dir if it's the same drive
              clrDir(1-context);
              dirs[1-context] = readDir(dirs[1-context],devices[1-context],(BYTE)(1-context));
            }
        }
    }
}

void
doFormat(void)
{
	sprintf(linebuffer,"Format device %d",devices[context]);
	newscreen(linebuffer);
	if (really())
    {
      cputs("\n\rEnter new name: ");
      scanf ("%s",answer);
      cputs("\n\rWorking...");
      sprintf(linebuffer,"n:%s\n",answer);
      if(cmd(devices[context],linebuffer)==OK)
        {
          clrscr();
          dirs[context] = readDir(dirs[context],devices[context],context);
        }
      else
        {
          cputs("ERROR\n\r\n\r");
          waitKey(0);
        }
    }
	updateScreen(2);
}

void
doRename(void)
{
	int n;
	Directory * cwd = GETCWD;

	if (cwd->selected!=NULL)
    {
      sprintf(linebuffer,"Rename file %s on device %d",cwd->selected->dirent.name,devices[context]);
      newscreen(linebuffer);
      cputs("\n\rNew name (enter to skip): ");
      n=scanf ("%s",answer);
      if (n==1)
        {
          cputs("\n\rWorking...");
          sprintf(linebuffer,"r:%s=%s\n",answer,cwd->selected->dirent.name);
          if(cmd(devices[context],linebuffer)==OK)
            {
              clrscr();
              dirs[context] = readDir(dirs[context],devices[context],context);
            }
          else
            {
              cputs("ERROR\n\r\n\r");
              waitKey(0);
            }
        }
      updateScreen(2);
    }
}

void
doMakedir(void)
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
      sprintf(linebuffer,"md:%s\n",answer);
      if(cmd(devices[context],linebuffer)==OK)
        {
          clrscr();
          dirs[context] = readDir(dirs[context],devices[context],context);
        }
      else
        {
          cputs("ERROR\n\r\n\r");
          waitKey(0);
        }
    }
	updateScreen(2);
}

int
copy(char * srcfile, BYTE srcdevice, char * destfile, BYTE destdevice, BYTE type)
{
	int length=0;
	unsigned char * linebuffer;
	char deststring[30];
  char type_ch;
  int ret = OK;
  unsigned long total_length = 0;

  switch(type)
    {
    case _CBM_T_SEQ:
      type_ch = 's';
      break;
    case _CBM_T_PRG:
      type_ch = 'p';
      break;
    case _CBM_T_USR:
      type_ch = 'u';
      break;
    default:
      return ERROR;
    }

	if( cbm_open (6,srcdevice,CBM_READ,srcfile) != 0)
    {
      cputs("Can't open input file!\n");
      return ERROR;
    }

	// create destination string with filetype like "FILE,P"
	sprintf(deststring, "%s,%c", destfile, type_ch);
	if( cbm_open (7,destdevice,CBM_WRITE,deststring) != 0)
    {
      cputs("Can't open output file\n");
      cbm_close (6);
      return ERROR;
    }

	linebuffer = (unsigned char *) malloc(BUFFERSIZE);

	cprintf("%-16s:",srcfile);
	while(1)
    {
      if (kbhit())
        {
          char c = cgetc();
          if (c == 27 || c == 95)
            {
              ret = ABORT;
              break;
            }
        }

	  	cputs("R");
      length = cbm_read (6, linebuffer, BUFFERSIZE);
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
          if (c == 27 || c == 95)
            {
              ret = ABORT;
              ERRMSG(COLOR_YELLOW,"ABORT");
              break;
            }
        }

      if (length > 0)
        {
          cputs("W");
          if (cbm_write(7, linebuffer, length) != length)
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

	free(linebuffer);
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

const char sectors[42] = {
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
  19, 19, 19, 19, 19, 19, 19,
  18, 18, 18, 18, 18, 18,
  17, 17, 17, 17, 17,
  17, 17, 17, 17, 17, // track 36-40
  17, 17 // track 41-42
};

BYTE diskCopyBuf[256];

void
printSecStatus(BYTE t, BYTE s, BYTE st)
{
  if (t & 0x80)
    {
      textcolor(COLOR_RED);
      t &= 0x7f;
    }
  else
    {
      textcolor((t < 35) ? COLOR_GRAY3 : COLOR_GRAY1);
    }
  if (t >= 40)
    t = 39;
  gotoxy(t, 3+s);
  cputc(st);
}

int
doDiskCopy(const BYTE deviceFrom, const BYTE deviceTo)
{
  int ret;
  BYTE i, track;
  BYTE max_track = 35; // TODO: detect disk type

	sprintf(linebuffer, "Copy disk from device %d to %d? (Y/N)", deviceFrom, deviceTo);
  newscreen(linebuffer);
  while(1)
    {
      i = cgetc();
      if (i == 'y')
        break;
      if (i == 'n' ||
          i == 27 ||
          i == 95)
        return 1;
    }

  cputs("0000000001111111111222222222233333333334");
  gotoxy(0,2);
  cputs("1234567890123456789012345678901234567890");
  for(i = 0; i < 40; ++i)
    {
      const BYTE max_s = sectors[i];
      BYTE j;
      for(j = 0; j < max_s; ++j)
        {
          printSecStatus(i, j, '.');
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

  for(track = 34; track < 42; ++track)
    {
      const BYTE max_sector = sectors[track];
      BYTE sector;

      if (track >= 40)
        {
          gotoxy(39,2);
          textcolor(textc);
          cputc('1'+track-40);
        }

      for(sector = 0; sector < max_sector; ++sector)
        {
          // TODO: kbhit() doesn't work
          if (kbhit())
            {
              i = cgetc();
              if (i == 27 || i == 95)
                {
                  goto done;
                }
            }

          printSecStatus(track, sector, 'R');
          ret = cbm_write(6, linebuffer, sprintf(linebuffer, "u1:5 0 %d %d", track + 1, sector));
          if (ret < 0)
            {
              sprintf(diskCopyBuf, "read sector %i/%i failed: %i", track+1, sector, _oserror);
#define SECTOR_ERROR                                    \
              gotoxy(0,24);                             \
              textcolor(COLOR_LIGHTRED);                \
              cputs(diskCopyBuf);                       \
              printSecStatus(track|0x80, sector, 'E');
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
              if (track == max_track && sector == 0)
                {
                  ret = 0;
                  goto success;
                }
              sprintf(diskCopyBuf, "read %i/%i failed: %i", track+1, sector, _oserror);
              SECTOR_ERROR;
              continue;
            }

          printSecStatus(track, sector, 'W');
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
