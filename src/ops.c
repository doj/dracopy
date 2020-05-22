/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */

#include "ops.h"
#include "screen.h"
#include "version.h"
#include "base.h"
#include <cbm.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef NOCOLOR
const BYTE textc = COLOR_WHITE;
#else
const BYTE textc = COLOR_LIGHTGREEN;
#endif

extern const BYTE textc;
Directory* dirs[] = {NULL,NULL};
BYTE context = 0;
BYTE devices[] = {8,9};
char linebuffer[SCREENW+1];
char linebuffer2[SCREENW+1];
char answer[40];

#ifdef CHAR80
BYTE DIRH = 23;
#else
BYTE DIRH = 10;
#endif

char DOSstatus[40];

/// string descriptions of enum drive_e
const char* drivetype[LAST_DRIVE_E] = {"\0", "1540", "1541", "1551", "1570", "1571", "1581", "sd2iec", "cmd", "vice"};
/// enum drive_e value for each device 0-11.
BYTE devicetype[12];

const char*
getDeviceType(BYTE context)
{
  BYTE device;
  BYTE idx;
  if (context > 1)
    {
      return "!ic";
    }
  device = devices[context];
  if (device > sizeof(devicetype))
    {
      return "!d";
    }
  if (dosCommand(15, device, 15, "ui") != 73)
    {
      return DOSstatus;
    }
  for(idx = 1; idx < LAST_DRIVE_E; ++idx)
    {
      if(strstr(DOSstatus, drivetype[idx]))
        {
          devicetype[device] = idx;
          return drivetype[idx];
        }
    }
  if(strstr(DOSstatus, "tdisk"))
    {
      devicetype[device] = D1551;
      return drivetype[D1551];
    }
  return DOSstatus;
  return "!nf";
}

unsigned char
dosCommand(unsigned char lfn, unsigned char drive, unsigned char sec_addr, char *cmd)
{
	cbm_open(lfn, drive, sec_addr, cmd);
	if (lfn != 15)
    cbm_open(15, drive, 15, "");

	memset(DOSstatus, 0, sizeof(DOSstatus));
	cbm_read(15, DOSstatus, sizeof DOSstatus);

	if(lfn != 15)
    cbm_close(15);
	cbm_close(lfn);

	DOSstatus[strcspn(DOSstatus, "\n")] = 0;
	return (DOSstatus[0] - 48) * 10 + DOSstatus[1] - 48;
}

int
cmd(unsigned char device, unsigned char * cmd)
{
  return dosCommand(15, device, 15, cmd);
}

void
execute(char * prg, BYTE device)
{
  // prepare the screen with the basic command to load the next program
	exitScreen();
	gotoxy(0,2);
	cprintf("load\"%s\",%d\n\r\n\r\n\r\n\r\n\r",prg,device);
	cputs("run\n\r");
  gotoxy(14,23);
  cputs("this program was loaded by");
  gotoxy(14,24);
  cputs("DraCopy " DRA_VER);

  // put two CR in keyboard buffer
	*((unsigned char *)KBCHARS)=13;
	*((unsigned char *)KBCHARS+1)=13;
	*((unsigned char *)KBNUM)=2;
  // exit DraCopy, which will execute the BASIC LOAD above
	gotoxy(0,0);
  exit(0);
}

void
updateScreen(BYTE num_dirs)
{
	clrscr();
	updateMenu();
	showDir(dirs[0],0);
  if (num_dirs > 1)
    showDir(dirs[1],1);
}

int
main(void)
{
  initScreen(COLOR_BLACK, COLOR_BLACK, textc);
  mainLoop();
  exitScreen();
  return 0;
}

void
about(const char *progname)
{
	BYTE idx=0;
	newscreen("About");
	textcolor(COLOR_YELLOW);
	idx=4;
	gotoxy(0,idx++);
  cputs(progname);
	cputs(" " DRA_VER);
#ifdef CHAR80
	cputs(" 80 Chars");
#endif
	textcolor(COLOR_GREEN);
	idx++;
	idx++;
	gotoxy(0,idx++);
	cputs("Copyright 2009 by Draco and others");
	idx++;
	gotoxy(0,idx++);
	cputs("https://github.com/doj/dracopy");
	idx++;
	idx++;
	gotoxy(0,idx++);
	cputs("THIS PROGRAM IS DISTRIBUTED IN THE HOPE");
	gotoxy(0,idx++);
	cputs("THAT IT WILL BE USEFUL.");
	idx++;
	gotoxy(0,idx++);
	cputs("IT IS PROVIDED WITH NO WARRANTY OF ANY ");
	gotoxy(0,idx++);
	cputs("KIND.");
	idx++;
	gotoxy(0,idx++);
	textcolor(COLOR_LIGHTRED);
	cputs("USE IT AT YOUR OWN RISK!");
	gotoxy(0,24);
	textcolor(COLOR_VIOLET);
	waitKey(0);
}

void
clrDir(BYTE context)
{
	clearArea((context==0)?DIR1X+1:DIR2X+1, (context==0)?DIR1Y:DIR2Y, DIRW, DIRH+1);
}

void
refreshDir(void)
{
	Directory * cwd = NULL;
	clrDir(context);
	cwd = readDir(cwd, devices[context], context);
	dirs[context]=cwd;
	cwd->selected=cwd->firstelement;
	showDir(cwd,context);
	if (devices[0]==devices[1])
    {
      // refresh also other dir if it's the same drive
      const BYTE other_context = 1-context;
      clrDir(other_context);
      dirs[other_context] = readDir(dirs[other_context], devices[other_context], other_context);
      showDir(cwd, other_context);
    }
}

const char *value2hex = "0123456789abcdef";

const char *reg_types[] = { "SEQ","PRG","URS","REL","VRP" };
const char *oth_types[] = { "DEL","CBM","DIR","LNK","OTH","HDR"};
char bad_type[4];
const char*
fileTypeToStr(BYTE ft)
{
  if (ft & _CBM_T_REG)
    {
      ft &= ~_CBM_T_REG;
      if (ft <= 4)
        return reg_types[ft];
    }
  else
    {
      if (ft <= 5)
        return oth_types[ft];
    }
  bad_type[0] = '?';
  bad_type[1] = value2hex[ft >> 4];
  bad_type[2] = value2hex[ft & 15];
  bad_type[3] = 0;
  return bad_type;
}

static void
printElementPriv(Directory * dir, DirElement *current, int xpos, int ypos)
{
	Directory * cwd = GETCWD;
  gotoxy(xpos,ypos);
  if ((current == dir->selected) && (cwd == dir))
    {
      revers(1);
    }

  cprintf("%4d %-16s %s", current->dirent.size, current->dirent.name, fileTypeToStr(current->dirent.type));

  if (current->flags!=0)
    {
      gotoxy(xpos,ypos);
      textcolor(COLOR_WHITE);
      cputc('>');
      textcolor(textc);
    }

  revers(0);
}

void
printDir(Directory * dir,int xpos, int ypos)
{
	DirElement * current;
	int selidx = 0;
	int page = 0;
	int skip = 0;
	int pos = 0;
	int idx = 0;
  const char *typestr = NULL;

	if (dir==NULL)
    {
      //cputs("no directory");
      return;
    }
	else
    {
      revers(0);
      current = dir->firstelement;
      idx=0;
      while (current!=NULL)
        {
          if (current==dir->selected)
            {
              break;
            }
          idx++;
          current=current->next;
        }

      page=idx/DIRH;
      skip=page*DIRH;

      current = dir->firstelement;

      // skip pages
      if (page>0)
        {
          for (idx=0; (idx < skip) && (current != NULL); ++idx)
            {
              current=current->next;
              pos++;
            }
        }

			for(idx=0; (current != NULL) && (idx < DIRH); ++idx)
        {
          printElementPriv(dir, current, xpos, ypos+idx+1);
          current = current->next;
        }

			// clear empty lines
			for (;idx < DIRH; ++idx)
        {
          gotoxy(xpos,ypos+idx+1);
          cputs("                         ");
        }
    }
}

void
printElement(Directory * dir,int xpos, int ypos)
{
	DirElement * current;

	int page = 0;
	int idx = 0;
	int pos = 0;
	int yoff=0;

	if (dir==NULL || dir->firstelement == NULL)
    {
      return;
    }
	else
    {
      revers(0);
      current = dir->firstelement;

      pos = dir->pos;

      idx=pos;
      while (current!=NULL && (idx--) >0)
        {
          current=current->next;
        }

      page=pos/DIRH;
      yoff=pos-(page*DIRH);

      printElementPriv(dir, current, xpos, ypos+yoff+1);
    }
}

void
showDir(Directory * dir, BYTE mycontext)
{
  const char *device_type;
	char *title = dir ? dir->name : "    no directory";
  if (mycontext > 1)
    return;
	if(mycontext==context)
    {
      sprintf(linebuffer, ">%02i:%s", (int)devices[mycontext], title);
      textcolor(COLOR_WHITE);
    }
	else
    {
      sprintf(linebuffer, " %02i:%s", (int)devices[mycontext], title);
      textcolor(textc);
    }

  device_type = getDeviceType(mycontext);

	sprintf(linebuffer2, "%s>%u blocks free", device_type, dir->free);
	drawFrame(linebuffer, (mycontext==0)?DIR1X:DIR2X, (mycontext==0)?DIR1Y:DIR2Y, DIRW+2, DIRH+2, linebuffer2);

	textcolor(textc);
	printDir(dir,(mycontext==0)?DIR1X+1:DIR2X+1,(mycontext==0)?DIR1Y:DIR2Y);
}

void
changeDir(BYTE device, const char *dirname)
{
  if (dirname)
    {
      BYTE mount = 0;
      BYTE l = strlen(dirname);
      if (l > 4 && dirname[l-4] == '.')
        {
          if (dirname[l-1] == '4' &&
              dirname[l-2] == '6' &&
              (dirname[l-3] == 'd' || dirname[l-3] == 'D'))
            {
              mount = 1;
            }
          else if (dirname[l-1] == '1' &&
                   (dirname[l-2] == '7' || dirname[l-2] == '8') &&
                   (dirname[l-3] == 'd' || dirname[l-3] == 'D'))
            {
              mount = 1;
            }
        }
      if (mount ||
          (l == 1 && dirname[0]==95)) // check for left arrow
        {
          sprintf(linebuffer, "cd:%s", dirname);
        }
      else
        {
          sprintf(linebuffer, "cd/%s/", dirname);
        }
    }
  else
    {
      strcpy(linebuffer, "cd//");
    }
  cmd(device, linebuffer);
  refreshDir();
}

void
changeDeviceID(BYTE device)
{
  int i;
  newscreen(" change device ID");
  cprintf("\n\rchange device ID %i to (0-255): ", device);
  scanf("%i", &i);

  if (devicetype[device] == SD2IEC)
    {
      sprintf(linebuffer, "U0>%c", i);
    }
  else
    {
      // TODO: doesn't work

      // Commodore drives:
      // OPEN 15,8,15:PRINT#15,"M-W";CHR$(119);CHR$(0);CHR$(2);CHR$(device number+32);CHR$(device number+64):CLOSE 15
      char *s = linebuffer;
      *s++ = 'm';
      *s++ = '-';
      *s++ = 'w';
      *s++ = 119; // addr lo
      *s++ = 0;   // addr hi
      *s++ = 2;   // number of bytes
      *s++ = 32+i;// device num + 0x20 for LISTEN
      *s++ = 64+i;// device num + 0x40 for TALK
      *s = 0;
    }

  cmd(device, linebuffer);
}
