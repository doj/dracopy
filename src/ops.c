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
#include <errno.h>
#include <ctype.h>
#if defined(__PET__)
#include <peekpoke.h>
#endif

const char *value2hex = "0123456789abcdef";

Directory* dirs[] = {NULL,NULL};
BYTE devices[] = {8,9};
char linebuffer[SCREENW+1];
char linebuffer2[SCREENW+1];
char answer[40];

BYTE DIR1H;
BYTE DIR2H;

void
initDirWindowHeight()
{
#ifdef CHAR80
  DIR1H = 23;
  DIR2H = 23;
#else
  DIR1H = 11;
  DIR2H = 10;
#endif
}

char DOSstatus[40];

/// string descriptions of enum drive_e
const char* drivetype[LAST_DRIVE_E] = {"", "1540", "1541", "1551", "1570", "1571", "1581", "1001", "sd2iec", "cmd", "vice"};
/// enum drive_e value for each device 0-11.
BYTE devicetype[12];

const char*
getDeviceType(BYTE context)
{
  const BYTE device = devices[context];
  BYTE idx;
  if (device > sizeof(devicetype))
    {
      return "!d";
    }
  idx = cmd(device, "ui");
  if (idx != 73)
    {
      linebuffer2[0] = 'Q';
      linebuffer2[1] = value2hex[idx >> 4];
      linebuffer2[2] = value2hex[idx & 15];
      linebuffer2[3] = 0;
      return linebuffer2;
    }
  for(idx = 1; idx < LAST_DRIVE_E; ++idx)
    {
      if(strstr(DOSstatus, drivetype[idx]))
        {
          devicetype[device] = idx;
          return drivetype[idx];
        }
    }
#if defined(__PLUS4__)
  if(strstr(DOSstatus, "tdisk"))
    {
      devicetype[device] = D1551;
      return drivetype[D1551];
    }
#endif
#if defined(SFD1001)
  if(strstr(DOSstatus, "cbm dos v2.7"))
    {
      devicetype[device] = D1001;
      return drivetype[D1001];
    }
#endif
  return "!n";
}

BYTE
dosCommand(const BYTE lfn, const BYTE drive, const BYTE sec_addr, const char *cmd)
{
  int res;
	if (cbm_open(lfn, drive, sec_addr, cmd) != 0)
    {
      return _oserror;
    }

	if (lfn != 15)
    {
      if (cbm_open(15, drive, 15, "") != 0)
        {
          cbm_close(lfn);
          return _oserror;
        }
    }

	DOSstatus[0] = 0;
	res = cbm_read(15, DOSstatus, sizeof(DOSstatus));

	if(lfn != 15)
    {
      cbm_close(15);
    }
	cbm_close(lfn);

  if (res < 1)
    {
      return _oserror;
    }

#if 0
  gotoxy(0,BOTTOM);
  cputs(DOSstatus);
#endif
	return (DOSstatus[0] - 48) * 10 + DOSstatus[1] - 48;
}

int
cmd(unsigned char device, const char *cmd)
{
  return dosCommand(15, device, 15, cmd);
}

void
execute(char * prg, BYTE device)
{
#if defined(__C64__) && defined(CHAR80)
  BYTE len = sprintf(KBCHARS, "lO\"%s\",%d", prg, device);
	*((unsigned char *)KBCHARS + len) = 13;  ++len;
	*((unsigned char *)KBCHARS + len) = 'r'; ++len;
	*((unsigned char *)KBCHARS + len) = 'U'; ++len;
	*((unsigned char *)KBCHARS + len) = 13;  ++len;
  *((unsigned char *)KBNUM) = len;
#else
  // prepare the screen with the basic command to load the next program
	exitScreen();

  gotoxy(0,2);
	cprintf("load\"%s\",%d,1", prg, device);
  gotoxy(0,7);
	cputs("run");

#if !defined(__PET__)
  gotoxy(14,BOTTOM-1);
  cputs("this program was loaded by");
  gotoxy(14,BOTTOM);
  cputs("DraCopy " DRA_VER);
#endif

  // put two CR in keyboard buffer
	*((unsigned char *)KBCHARS)=13;
	*((unsigned char *)KBCHARS+1)=13;
	*((unsigned char *)KBNUM)=2;

  // exit DraCopy, which will execute the BASIC LOAD above
	gotoxy(0,0);
#endif
  exit(0);
}

void
updateScreen(const BYTE context, BYTE num_dirs)
{
  clrscr();
	updateMenu();
	showDir(context, dirs[context], context);
  if (num_dirs > 1)
    {
      const BYTE other_context = context^1;
      showDir(other_context, dirs[other_context], context);
    }
}

int
main(void)
{
#ifdef __PET__
  // check for memory at 0x9000 and add to heap
  // https://cc65.github.io/doc/pet.html
  if (PEEK(0x9000) == POKE(0x9000, PEEK(0x9000)+1))
    {
      if (PEEK(0xAfff) == POKE(0xAfff, PEEK(0xAfff)+1))
        {
          _heapadd ((void *) 0x9000, 0x2000);
          //printf("9 8KB\r\n");
        }
      else if (PEEK(0x9fff) == POKE(0x9fff, PEEK(0x9fff)+1))
        {
          _heapadd ((void *) 0x9000, 0x1000);
          //printf("9 4KB\r\n");
        }
      else
        {
          //printf("9 fail\r\n");
        }
    }
  else if (PEEK(0xA000) == POKE(0xA000, PEEK(0xA000)+1))
    {
      if (PEEK(0xAfff) == POKE(0xAfff, PEEK(0xAfff)+1))
        {
          _heapadd ((void *) 0xA000, 0x1000);
          //printf("A 4KB\r\n");
        }
      else
        {
          //printf("A fail\r\n");
        }
    }
  else
    {
      //printf("no RAM\r\n");
    }
  //return 0;
#endif

#if defined(__C64__) && defined(CHAR80)
  _heapadd((void *) 0x0400, 0x0400);
#endif

  initScreen(DC_COLOR_BORDER, DC_COLOR_BG, DC_COLOR_TEXT);
  mainLoop();
  exitScreen();
  return 0;
}

#if !defined(__PET__)
#pragma charmap (0xff, 0x5f);
#pragma charmap (0xfc, 0x5c);

static const char* helpcontent[] = {
  "F1", "read dir",
  "F2", "change dev",
  "F3", "view hex",
  "F4", "view ASCII",
  "F5", "copy files",
  "F6", "delete file",
  "F7", "execute PRG",
  "F8", "disk copy",

  "0", "switch win",
  "\xff",  "switch win", // CH_LARROW
  "w", "window size",

  "CR", "change dir",
  "DL", "parent dir",
  "\x5e", "parent dir", // CH_UARROW
  "","",

  // middle column
  "m", "make dir",
  "f", "format disk",
  "l", "relabel dsk",
  "i", "disk image",
  "r", "rename file",
  "c", "copy on same device",

  "@", "DOS command",
  "\xfc", "change dev id", // CH_POUND

  "HO", "goto top",
  "t", "goto top",
  "b", "goto bottom",
  "n", "next page",
  "p", "prev page",

  "*", "invert selection",
  "SP", "select file",

  // right column
  "q", "quit",
  ".", "help",
  NULL
};

void
about(const char *progname)
{
  BYTE x = 0;
  BYTE y = 10;
  const char* *h = helpcontent;
  sprintf(linebuffer, " About %s " DRA_VER, progname);
  newscreen(linebuffer);

	textcolor(DC_COLOR_DIM);
	cputs("Copyright 2009 by Draco and others\n\r"
        "https://github.com/doj/dracopy\n\r"
        "\n\r"
        "THIS PROGRAM IS DISTRIBUTED IN THE HOPE\n\r"
        "THAT IT WILL BE USEFUL.\n\r"
        "IT IS PROVIDED WITH NO WARRANTY OF ANY\n\r"
        "KIND. USE IT AT YOUR OWN RISK!\n\r"
        );

  while(*h)
    {
      // print key
      textcolor(DC_COLOR_TEXT);
      cputsxy(x + 2 - strlen(*h), y, *h);
      ++h;

      // print description
      textcolor(DC_COLOR_DIM);
      cputsxy(x+3, y, *h);
      ++h;

      if (++y == SCREENH)
        {
#ifdef CHAR80
          x += 23;
#else
          x += 14;
#endif
          y = 10;
        }
    }

  cgetc();
}
#else
void
about(const char *progname)
{
  sprintf(linebuffer, " About %s " DRA_VER, progname);
  newscreen(linebuffer);
	cputs("Copyright 2009 by Draco and others\n\r"
        "https://github.com/doj/dracopy\n\r");
  cgetc();
}
#endif

void
clrDir(BYTE context)
{
	clearArea(DIRX+1, DIRY+1, DIRW, DIRH);
}

void
refreshDir(const BYTE context, const BYTE sorted, const BYTE mycontext)
{
	Directory * cwd = dirs[context];
  textcolor(DC_COLOR_HIGHLIGHT);
	cwd = readDir(cwd, devices[context], context, sorted);
	dirs[context]=cwd;
	cwd->selected=cwd->firstelement;
	showDir(context, cwd, mycontext);
#if 0
	if (devices[0]==devices[1])
    {
      // refresh also other dir if it's the same drive
      const BYTE other_context = context^1;
      dirs[other_context] = readDir(dirs[other_context], devices[other_context], other_context, sorted);
      showDir(other_context, cwd, mycontext);
    }
#endif
}

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
printElementPriv(const BYTE context, const Directory *dir, const DirElement *current, const BYTE xpos, const BYTE ypos)
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
      textcolor(DC_COLOR_HIGHLIGHT);
      cputc('>');
    }

  textcolor(DC_COLOR_TEXT);
  revers(0);
}

void
printDir(const BYTE context, const Directory *dir, const BYTE xpos, const BYTE ypos)
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
      clrDir(context);
      return;
    }

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
      printElementPriv(context, dir, current, xpos, ypos+idx+1);
      current = current->next;
    }

  // clear empty lines
  for (;idx < DIRH; ++idx)
    {
      gotoxy(xpos,ypos+idx+1);
      cputs("                         ");
    }
}

void
printElement(const BYTE context, const Directory *dir, const BYTE xpos, const BYTE ypos)
{
	const DirElement *current;

	int page = 0;
	int idx = 0;
	int pos = 0;
	int yoff=0;

	if (dir==NULL || dir->firstelement == NULL)
    {
      return;
    }

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

  printElementPriv(context, dir, current, xpos, ypos+yoff+1);
}

void
drawDirFrame(BYTE context, const Directory *dir, const BYTE mycontext)
{
  sprintf(linebuffer, " %02i:%s", (int)devices[context], dir ? dir->name : "");
	if(mycontext==context)
    {
      linebuffer[0] = '>';
      textcolor(DC_COLOR_HIGHLIGHT);
    }
	else
    {
      textcolor(DC_COLOR_TEXT);
    }

  if (dir)
    {
      sprintf(linebuffer2, "%s>%u bl free<", dir->device_type, dir->free);
    }
  else
    {
      linebuffer2[0] = 0;
    }
	drawFrame(linebuffer, DIRX, DIRY, DIRW+2, DIRH+2, linebuffer2);
	textcolor(DC_COLOR_TEXT);
}

void
showDir(BYTE context, const Directory * dir, const BYTE mycontext)
{
  drawDirFrame(context, dir, mycontext);
	printDir(context, dir, DIRX+1, DIRY);
}

void
changeDir(const BYTE context, const BYTE device, const char *dirname, const BYTE sorted)
{
  if (dirname)
    {
      BYTE mount = 0;
      register BYTE l = strlen(dirname);
      if (l > 4 && dirname[l-4] == '.')
        {
          if ((dirname[l-3] == 'd' || dirname[l-3] == 'D') &&
              (dirname[l-2] == '6') &&
              (dirname[l-1] == '4'))
            {
              mount = 1;
            }
          else if ((dirname[l-3] == 'd' || dirname[l-3] == 'D') &&
                   (dirname[l-2] == '7' || dirname[l-2] == '8') &&
                   (dirname[l-1] == '1'))
            {
              mount = 1;
            }
        }
      if (mount ||
          (l == 1 && dirname[0]==CH_LARROW))
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
  refreshDir(context, sorted, context);
}

void
changeDeviceID(BYTE device)
{
  int i;
  newscreen(" change device ID");
  cprintf("\n\rchange device ID %i to (0-255): ", device);
  sprintf(linebuffer, "%i", device);
  i = textInput(31, 2, linebuffer, 3);
  if (i <= 0)
    return;
  i = atoi(linebuffer);

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

void
debugs(const char *s)
{
  gotoxy(MENUXT,BOTTOM);
  cclear(SCREENW-MENUXT);
  gotoxy(MENUXT,BOTTOM);
  cputs(s);
}

void
debugu(const unsigned u)
{
  gotoxy(MENUXT,BOTTOM);
  cclear(SCREENW-MENUXT);
  gotoxy(MENUXT,BOTTOM);
  cprintf("%04x", u);
}

/**
 * input/modify a string.
 * based on version 1.0e, then modified.
 * @param[in] xpos screen x where input starts.
 * @param[in] ypos screen y where input starts.
 * @param[in,out] str string that is edited, it can have content and must have at least @p size + 1 bytes. Maximum size if 255 bytes.
 * @param[in] size maximum length of @p str in bytes.
 * @return -1 if input was aborted.
 * @return >= 0 length of edited string @p str.
 */
int
textInput(const BYTE xpos, const BYTE ypos, char *str, const BYTE size)
{
	register BYTE idx = strlen(str);
	register BYTE c;

	cursor(1);
	cputsxy(xpos, ypos, str);

	while(1)
    {
      c = cgetc();
      switch (c)
        {
      case CH_LARROW:
      case CH_ESC:
        cursor(0);
        return -1;

      case CH_ENTER:
        idx = strlen(str);
        str[idx] = 0;
        cursor(0);
        return idx;

      case CH_DEL:
        if (idx)
          {
            --idx;
            cputcxy(xpos + idx, ypos, ' ');
            for(c = idx; 1; ++c)
              {
                const BYTE b = str[c+1];
                str[c] = b;
                cputcxy(xpos + c, ypos, b ? b : ' ');
                if (b == 0)
                  break;
              }
            gotoxy(xpos + idx, ypos);
          }
        break;

        case CH_INS:
          c = strlen(str);
          if (c < size &&
              c > 0 &&
              idx < c)
            {
              ++c;
              while(c >= idx)
                {
                  str[c+1] = str[c];
                  if (c == 0)
                    break;
                  --c;
                }
              str[idx] = ' ';
              cputsxy(xpos, ypos, str);
              gotoxy(xpos + idx, ypos);
            }
          break;

      case CH_CURS_LEFT:
        if (idx)
          {
            --idx;
            gotoxy(xpos + idx, ypos);
          }
        break;

      case CH_CURS_RIGHT:
        if (idx < strlen(str) &&
            idx < size)
          {
            ++idx;
            gotoxy(xpos + idx, ypos);
          }
        break;

      default:
        if (isprint(c) &&
            idx < size)
          {
            const BYTE flag = (str[idx] == 0);
            str[idx] = c;
            cputc(c);
            ++idx;
            if (flag)
              str[idx+1] = 0;
            break;
          }
        break;
      }
    }
  return 0;
}


void
doDOScommand(const BYTE context, const BYTE sorted, const BYTE use_linebuffer)
{
  int i;
  const BYTE device = devices[context];
  newscreen(" DOS command");
#if !defined(__PET__)
  cprintf("\n\rsend DOS command to device %i:", device);
#endif
  linebuffer[use_linebuffer ? SCREENW : 0] = 0;
  i = textInput(0, 3, linebuffer, SCREENW);
  if (i > 0)
    {
      i = cmd(device, linebuffer);
      gotoxy(0,5);
      cprintf("result: %i\n\r", i);
      waitKey(0);
      dirs[context] = readDir(dirs[context], device, context, sorted);
    }
}
