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

#include <assert.h>
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
void doRenameOrCopy(const BYTE context, const BYTE mode);
void doToggleAll(const BYTE context);
void doCopy(const BYTE context);
void doDelete(const BYTE context);
int doDiskCopy(const BYTE deviceFrom, const BYTE deviceTo);
int copy(const char *srcfile, const BYTE srcdevice, const char *destfile, const BYTE destdevice, BYTE type);
void doMakeImage(const BYTE device);

extern BYTE devices[];
extern char linebuffer[];
extern char answer[];
extern Directory* dirs[];
extern const BYTE textc;

/* definitions */
char menustatus[MENUW];
static BYTE windowState = 0;
static BYTE sorted = 0;

void
nextWindowState(const BYTE context)
{
#if !defined(CHAR80)
  if (context == 1 &&
      windowState == 0)
    {
      windowState = 1;
    }

  initDirWindowHeight();
  switch(++windowState)
    {
    default:
    case 0:
      windowState = 0;
      break;
    case 1:
      DIR1H += DIR2H - 2;
      DIR2H = 2;
      break;
    case 2:
      DIR2H += DIR1H - 2;
      DIR1H = 2;
      break;
    }

  showDir(0, dirs[0], context);
  showDir(1, dirs[1], context);
#endif
}

void
updateMenu(void)
{
	BYTE menuy=MENUY;

	revers(0);
	textcolor(textc);
	drawFrame(" " DRA_VERNUM " ",MENUX,MENUY,MENUW,MENUH,menustatus);

	cputsxy(MENUXT,++menuy,"F1 READ DIR");
	cputsxy(MENUXT,++menuy,"F2 DEVICE");
	cputsxy(MENUXT,++menuy,"F3 VIEW HEX");
	cputsxy(MENUXT,++menuy,"F4 VIEW ASC");
	cputsxy(MENUXT,++menuy,"F5 COPY");
	cputsxy(MENUXT,++menuy,"F6 DELETE");
	cputsxy(MENUXT,++menuy,"F7 RUN");
	cputsxy(MENUXT,++menuy,"F8 DISKCOPY");
	cputsxy(MENUXT,++menuy,"SP TAG");
#ifdef __PLUS4__
	cputsxy(MENUXT,++menuy,"EC SWITCH");
#else
	cputcxy(MENUXT+1,++menuy,CH_LARROW); cputs(" SWITCH W");
#endif
	cputsxy(MENUXT,++menuy,"CR CHG DIR");
	cputsxy(MENUXT,++menuy,"BS DIR UP");
	cputsxy(MENUXT,++menuy," T TOP");
	cputsxy(MENUXT,++menuy," B BOTTOM");
	cputsxy(MENUXT,++menuy," * INV SEL");
	cputsxy(MENUXT,++menuy," R RENAME");
	cputsxy(MENUXT,++menuy," M MAKE DIR");
	cputsxy(MENUXT,++menuy," F FORMAT");
	cputcxy(MENUXT+1,++menuy,CH_POUND); cputs(" DEV ID");
	cputsxy(MENUXT,++menuy," @ DOS CMD");
	cputsxy(MENUXT,++menuy," S SORT DIR");
	cputsxy(MENUXT,++menuy," I MAKE IMG");
#if defined(CHAR80)
	cputsxy(MENUXT,++menuy," Q QUIT");
#else
	cputsxy(MENUXT,++menuy," W WIN SIZE");
#endif
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

  {
    BYTE i = 7;
    textcolor(COLOR_WHITE);
    while(++i < 12)
      {
        devices[context] = i;
        dirs[context] = readDir(NULL, devices[context], context, sorted);
        if (dirs[context])
          {
            showDir(context, dirs[context], context);
            goto found_upper_drive;
          }
      }

  found_upper_drive:
    textcolor(textc);
    while(++i < 12)
      {
        devices[1] = i;
        dirs[1] = readDir(NULL, devices[1], 1, sorted);
        if (dirs[1])
          {
            showDir(1, dirs[1], context);
            goto found_lower_drive;
          }
      }

    // if no drive was found for the lower window,
    // enlarge the upper window.
    nextWindowState(context);
  }

 found_lower_drive:
	while(1)
    {
      {
        // TODO: find out where a file is left open, then remove this workaround block
        BYTE i;
        for(i = 0; i < 16; ++i)
          {
            cbm_close(i);
            cbm_closedir(i);
          }
      }
      {
        const size_t s = _heapmemavail();
        if (s < 0x1000)
          {
            gotoxy(MENUXT,BOTTOM);
            textcolor(COLOR_WHITE);
            cprintf("lowmem:%04x",s);
            textcolor(textc);
          }
      }

    	switch (cgetc())
      	{
        case 's':
          sorted = ! sorted;
          // fallthrough
        case '1':
        case CH_F1:
          textcolor(COLOR_WHITE);
					dirs[context] = readDir(dirs[context], devices[context], context, sorted);
					showDir(context, dirs[context], context);
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
          {
            const BYTE other_context = context^1;
            freeDir(&dirs[other_context]);
            doCopy(context);
            updateScreen(context, 2);
            // refresh destination dir
            dirs[other_context] = readDir(dirs[other_context], devices[other_context], other_context, sorted);
            showDir(other_context, dirs[other_context], context);
          }
					break;

        case '6':
        case CH_F6:
					doDelete(context);
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
            freeDir(&dirs[other_context]);
            doDiskCopy(devices[context], devices[other_context]);
            updateScreen(context, 2);
            refreshDir(other_context, sorted, context);
          }
					break;

          // ----- switch context -----
        case '0':
        case CH_ESC:
		    case CH_LARROW:  // arrow left
          {
            const BYTE prev_context = context;
            context = context ^ 1;
            drawDirFrame(context, dirs[context], context);
            drawDirFrame(prev_context, dirs[prev_context], context);
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
          cwd = GETCWD;
          current = cwd->selected;
          for(pos = 0; pos < (context ? DIR2H : DIR1H) && current->next; ++pos)
            {
              current = current->next;
            }
          cwd->pos += pos;
          cwd->selected = current;
					printDir(context, cwd, DIRX+1, DIRY);
          break;

        case 'p':
          cwd = GETCWD;
          current = cwd->selected;
          for(pos = 0; pos < (context ? DIR2H : DIR1H) && current->prev; ++pos)
            {
              current = current->prev;
            }
          assert(cwd->pos >= pos);
          cwd->pos -= pos;
          cwd->selected = current;
					printDir(context, cwd, DIRX+1, DIRY);
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

        case 'f':
          strcpy(linebuffer, "n:");
          doDOScommand(context, sorted, 1);
          updateScreen(context, 2);
          break;

        case 'r':
					doRenameOrCopy(context, 0);
          break;

        case 'm':
          strcpy(linebuffer, "md:");
          doDOScommand(context, sorted, 1);
          updateScreen(context, 2);
          break;

        case '*':
					doToggleAll(context);
          break;

        case '.':
					about("DraCopy");
          updateScreen(context, 2);
          break;

        case '@':
          doDOScommand(context, sorted, 0);
          updateScreen(context, 2);
          break;

        case 'c':
					doRenameOrCopy(context, 1);
          break;

        case 'i':
          doMakeImage(devices[context]);
          updateScreen(context, 2);
          refreshDir(context, sorted, context);
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
					if (cwd->selected!=NULL && cwd->selected->prev!=NULL)
            {
              cwd->selected=cwd->selected->prev;
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
              changeDir(context, devices[context], cwd->selected->dirent.name, sorted);
            }
					break;

          // --- leave directory
    		case CH_DEL:
    		case CH_CURS_LEFT:
          {
            char buf[2];
            buf[0] = CH_LARROW; // arrow left
            buf[1] = 0;
            changeDir(context, devices[context], buf, sorted);
          }
					break;

        case CH_UARROW:
          changeDir(context, devices[context], NULL, sorted);
          break;

        case 'w':
          nextWindowState(context);
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
doCopy(const BYTE context)
{
  BYTE flag = 0;
  BYTE cnt = 0xf0;
	DirElement * current;
	const BYTE srcdev = devices[context];
	const BYTE destdev = devices[context^1];
	Directory * cwd = GETCWD;

	sprintf(linebuffer," Filecopy from device %d to device %d",srcdev,destdev);
  for(current = cwd->firstelement; current; current=current->next)
    {
      if (++cnt >= BOTTOM)
        {
          cnt = 0;
          newscreen(linebuffer);
        }
      if (current->flags==1)
        {
          int ret;
          flag = 1;
          ret = copy(current->dirent.name, srcdev, current->dirent.name, destdev, current->dirent.type);
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

  if (! flag)
    {
      current = cwd->selected;
      if (current)
        {
          int ret = copy(current->dirent.name, srcdev, current->dirent.name, destdev, current->dirent.type);
          if (ret == ERROR)
            {
              cputc(CH_ENTER);
              cputc(10);
              waitKey(0);
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
doDelete(const BYTE context)
{
	DirElement * current;
	int idx = 0;
	Directory * cwd = GETCWD;
  int ret;

	if (dirs[context]==NULL)
    return;

  for(current = dirs[context]->firstelement; current; current=current->next)
    {
      if (current->flags)
        ++idx;
    }

  if (idx == 0)
    {
      cwd->selected->flags = 1;
      idx = 1;
    }

  sprintf(linebuffer, " Delete %i files from device %d", idx, devices[context]);
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

      sprintf(linebuffer,
              "%s:%s",
              (current->dirent.type==DIRTYPE) ? "rd" : "s",
              current->dirent.name);
      ret = cmd(devices[context],linebuffer);
      cputc(' ');
      revers(1);
      if (ret == 1)
        {
          cputs("DELETED");
        }
      else
        {
          textcolor(COLOR_VIOLET);
          cputs("ERROR");
          break;
        }
      textcolor(textc);
      revers(0);

      if (++idx > BOTTOM)
        {
          newscreen(linebuffer);
          idx = 1;
        }
    }

  updateScreen(context, 2);
  // refresh directories
  cwd = readDir(cwd, devices[context], context, sorted);
  dirs[context]=cwd;
  cwd->selected=cwd->firstelement;
  showDir(context, cwd, context);
  if (devices[0] == devices[1])
    {
      const BYTE other_context = context^1;
      dirs[other_context] = cwd;
      showDir(other_context, cwd, context);
    }
}

void
doRenameOrCopy(const BYTE context, const BYTE mode)
{
	int n;
	Directory * cwd = GETCWD;

	if (cwd->selected == NULL)
    return;

  sprintf(linebuffer,
          " %s file %s on device %d",
          mode ? "Copy" : "Rename",
          cwd->selected->dirent.name,devices[context]);
  newscreen(linebuffer);
  cputs("\n\rNew name: ");
  strcpy(answer, cwd->selected->dirent.name);
  n = textInput(10,2,answer,16+3);
  if (n > 0 && strcmp(answer,cwd->selected->dirent.name))
    {
      cputs("\n\rWorking...");
      sprintf(linebuffer,
              "%c:%s=%s",
              mode ? 'c' : 'r',
              answer,
              cwd->selected->dirent.name);
      if(cmd(devices[context],linebuffer)==OK)
        {
          dirs[context] = readDir(dirs[context], devices[context], context, sorted);
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
  int ret = OK;
  unsigned long total_length = 0;
  BYTE xpos, ypos;

	BYTE *buf = (BYTE*) malloc(BUFFERSIZE);
  if (! buf)
    {
      cputs("Can't alloc\n\r");
      return ERROR;
    }

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
      cputs("unsupported file type\n\r");
      ret = ERROR;
      goto done;
    }

	sprintf(linebuffer, "%s,%c", srcfile, type);
	if (cbm_open(6, srcdevice, CBM_READ, linebuffer) != 0)
    {
      cputs("Can't open input file!\n\r");
      ret = ERROR;
      goto done;
    }

	// create destination string with filetype like "FILE,P"
	sprintf(linebuffer, "%s,%c", destfile, type);
	if (cbm_open(7, destdevice, CBM_WRITE, linebuffer) != 0)
    {
      cputs("Can't open output file\n\r");
      ret = ERROR;
      goto done;
    }

	cprintf("%-16s:",srcfile);
  xpos = wherex();
  ypos = wherey();
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

	  	cputsxy(xpos, ypos, "R");
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
          cputsxy(xpos, ypos, "W");
          if (cbm_write(7, buf, length) != length)
            {
              ERRMSG(COLOR_YELLOW,"WRITE ERROR");
              ret = ERROR;
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

 done:
	free(buf);
	cbm_close(6);
	cbm_close(7);
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
    return 35;//42;
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
#if !defined(CHAR80)
  else if (dt == D1571 && t >= 35)
    {
      t -= 35;
      textcolor(COLOR_WHITE);
    }
#endif
  else if (dt == D1581)
    {
#if !defined(CHAR80)
      if (t >= 40)
        {
          textcolor(COLOR_WHITE);
          t -= 40;
        }
#endif
      if ((s&1) == 0)
        {
          if (st == 'R') st = 'r';
          else if (st == 'W') st = 'w';
        }
      s >>= 1;
    }
  cputcxy(t, 3+s, st);
}

/**
 * disk sector copy from device @p deviceFrom to @p deviceTo.
 * based on version 1.0e, then heavily modified.
 * @return OK if copy was successful.
 * @return ERROR if copy failed or devices are incompatible.
 * @return ABORT if copy was aborted.
 */
int
doDiskCopy(const BYTE deviceFrom, const BYTE deviceTo)
{
  int ret = OK;
  const BYTE dt = devicetype[deviceFrom];
  const BYTE max_track = maxTrack(dt);
  BYTE i = devicetype[deviceTo];
  BYTE track = maxTrack(i);

	sprintf(linebuffer, " Copy disk from device %d to %d? (Y/N)", deviceFrom, deviceTo);
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
        return ABORT;
    }

  cputs("0000000001111111111222222222233333333334");
#if defined(CHAR80)
  cputs("4444444445555555555666666666677777777778");
#endif
  cputsxy(0,2, "1234567890123456789012345678901234567890");
#if defined(CHAR80)
  cputs("1234567890123456789012345678901234567890");
#endif
  for(track = 0; track < 80; ++track)
    {
      BYTE max_s = 40;
      BYTE sector;
      if (IS_1541(dt))
        {
          if (track == 40)
            {
              break;
            }
          max_s = sectors1541[track];
        }
      else if (dt == D1571)
        {
#if defined(CHAR80)
          if (track == 70)
            break;
#else
          if (track == 35)
            break;
#endif
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

#if !defined(CHAR80)
      // check if 1541 writes to extra tracks
      if (IS_1541(dt)
          && track >= 40)
        {
          textcolor(textc);
          cputcxy(39,2, '1'+track-40);
        }
      // check if 1571 writes on back side
      if (dt == D1571 && track == 35)
        {
          textcolor(textc);
          cputsxy(0,1,"33334444444444555555555566666666667     ");
          cputsxy(0,2,"67890123456789012345678901234567890     ");
        }
      // check if 1581 writes on back side
      if (dt == D1581 && track == 40)
        {
          textcolor(textc);
          cputsxy(0,1,"4444444445555555555666666666677777777778");
        }
#endif

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
              textcolor(COLOR_LIGHTRED);                \
              cputsxy(0,BOTTOM,diskCopyBuf);                \
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
                  ret = OK;
                  goto success;
                }
              if (dt == D1571 && track == 35 && sector == 0)
                {
                  ret = OK;
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
          textcolor(COLOR_LIGHTBLUE);
          gotoxy(0,BOTTOM);
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
          textcolor(COLOR_LIGHTBLUE);
          gotoxy(0,BOTTOM);
          switch(track)
            {
            case 35: cputs("copy the back side"); break;
            case 69: cputs("writing last track!!!"); break;
            }
        }
      else if (dt == D1581)
        {
          textcolor(COLOR_LIGHTBLUE);
          gotoxy(0,BOTTOM);
          switch(track)
            {
            case 40: cputs("copy the back side"); break;
            case 79: cputs("writing last track!!!"); break;
            }
        }
    }

 success:
  textcolor(COLOR_LIGHTBLUE);
  cputsxy(0,BOTTOM,"disk copy success          ");
  ret = OK;
  goto done;

 error:
  ret = ERROR;
  textcolor(COLOR_LIGHTRED);
  cputsxy(0,BOTTOM,diskCopyBuf);
  cgetc();

 done:
  cbm_close(6);
  cbm_close(7);
  cbm_close(8);
  cbm_close(9);

  textcolor(textc);
  return ret;
}

void
doMakeImage(const BYTE device)
{
  BYTE dt = 0;
  int bam;
  int n;
  int i;
  BYTE *buf;
  int answer_len;

	sprintf(linebuffer, " Make Image on device %d", device);
  newscreen(linebuffer);

  cputs("\n\rValid Image extensions: .d64 .d71 .d81\n\rImage name: ");
  answer_len = n = textInput(12,3,answer,16);
  if (n < 0)
    return;
  if (n > 0 && n < 5)
    {
      cputsxy(0,6,"invalid image name\n\r");
      waitKey(0);
      return;
    }

  if (answer[n-3] == 'd' || answer[n-3] == 'D')
    {
      if (answer[n-2] == '6' &&
          answer[n-1] == '4')
        {
          dt = D1541;
          n = 683;
          bam = 357;
        }
      else if (answer[n-2] == '7' &&
               answer[n-1] == '1')
        {
          dt = D1571;
          n = 1366;
          bam = 357;
        }
      else if (answer[n-2] == '8' &&
               answer[n-1] == '1')
        {
          dt = D1581;
          n = 80*40;
          bam = 39*40;
        }
    }
  if (dt == 0)
    {
      cputsxy(0,6,"invalid image type\n\r");
      waitKey(0);
      return;
    }

  buf = (BYTE*) malloc(256);
  if (! buf)
    {
      cputsxy(0,6,"Can't alloc\n\r");
      waitKey(0);
      return;
    }
  memset(buf, 0, 256);

  sprintf(linebuffer, "%s,p", answer);
  if (cbm_open(7, device, CBM_WRITE, linebuffer) != 0)
    {
      cputsxy(0,6,"Can't open output file\n\r");
      waitKey(0);
      goto done;
    }

  for(i = 0; i < n; ++i)
    {
      gotoxy(0,4);
      cprintf("write block %i/%i", i, n);

      memset(buf, 0, 256);
      if (dt == D1541 || dt == D1571)
        {
          if (i == bam ||
              (dt == D1571 && i == 1040))
            {
              int j;
              // http://unusedino.de/ec64/technical/formats/d64.html

              // track/sector of first directory block
              buf[0] = 18;
              buf[1] = 1;

              buf[2] = 0x41; // DOS version

              // http://unusedino.de/ec64/technical/formats/d71.html
              if (dt == D1571)
                {
                  buf[3] = 0x80;
                  for(j = 0; j < 35; ++j)
                    {
                      buf[0xDD + j] = maxSector(dt, j+35);
                    }
                }

              // write BAM,
              // 35 objects of 4 bytes.
              for(j = 0; j < 35; ++j)
                {
                  // first byte, sectors per track
                  BYTE max_sector = maxSector(dt, j);
                  buf[4 + j*4] = max_sector;

                  // 21, 19, 18, 17 bits set for each free block per track
                  if (j == bam)
                    {
                      buf[5 + j*4] = 0x3f;
                    }
                  else
                    {
                      buf[5 + j*4] = 0xff;
                    }

                  buf[6 + j*4] = 0xff;

                  switch(max_sector)
                    {
                    case 21: max_sector = 0x1f; break;
                    case 19: max_sector = 0x07; break;
                    case 18: max_sector = 0x03; break;
                    case 17: max_sector = 0x01; break;
                    }
                  buf[7 + j*4] = max_sector;
                }

              // disk name
              memset(&buf[0x90], 0xA0, 0xAB - 0x90);
              memcpy(&buf[0x90], answer, answer_len-4);
              // disk ID
              buf[0xA2] = 'd';
              buf[0xA3] = 'c';
              // DOS type
              buf[0xA5] = '2';
              buf[0xA6] = 'a';
            }
          else if (i == bam+1)
            {
              // first directory sector
              buf[1] = 0xff;
            }
        }

      if (dt == D1581)
        {
          // http://unusedino.de/ec64/technical/formats/d81.html

          // header sector at 40/0
          if (i == bam)
            {
              // track/sector of first directory sector
              buf[0] = 40;
              buf[1] = 3;
              // DOS type
              buf[2] = 0x44;
              // disk name
              memset(&buf[4], 0xA0, 0x1D-4);
              memcpy(&buf[4], answer, answer_len - 4);
              // disk ID
              buf[0x16] = 'd';
              buf[0x17] = 'c';
              // DOS version
              buf[0x19] = '3';
              // disk version
              buf[0x1a] = 'd';
            }
          // first BAM at 40/1
          // second BAM at 40/2
          else if (i == bam+1 || i == bam+2)
            {
              BYTE *b;
              int t;
              // next track/sector
              if (i == bam+1)
                {
                  buf[0] = 40;
                  buf[1] = 2;
                }
              else
                {
                  buf[1] = 0xff;
                }
              // version
              buf[2] = 'd';
              buf[3] = 0xBB;
              // disk ID
              buf[4] = 'd';
              buf[5] = 'c';
              // I/O byte
              buf[6] = 0xC0;

              b = &buf[0x10];
              for(t = 1; t <= 40; ++t)
                {
                  if (i == bam+2 && t == 40)
                    {
                      *b++ = 36;
                      *b++ = 0xf0;
                    }
                  else
                    {
                      *b++ = 40;
                      *b++ = 0xff;
                    }
                  *b++ = 0xff;
                  *b++ = 0xff;
                  *b++ = 0xff;
                  *b++ = 0xff;
                }
            }
          // first directory sector at 40/3
          else if (i == bam+3)
            {
              buf[1] = 0xff;
            }
        }

      if (i == n-1)
        {
          strcpy(buf+220, "image created by dracopy " DRA_VERNUM);
        }

      if (cbm_write(7, buf, 256) != 256)
        {
          cputsxy(0,6,"write error\n\r");
          waitKey(0);
          goto done;
        }

      if (kbhit())
        {
          char c = cgetc();
          if (c == CH_ESC || c == CH_LARROW)
            {
              cputsxy(0,6,"abort");
              cbm_close(7);
              sprintf(linebuffer, "s:%s", answer);
              cmd(device, linebuffer);
              goto done;
            }
        }
    }

 done:
  cbm_close(7);
  free(buf);
}
