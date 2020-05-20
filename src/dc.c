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

/* declarations */
void mainLoop(void);
BYTE really(void);
void updateScreen(void);
void updateMenu(void);
void doFormat(void);
void doRename(void);
void doMakedir(void);
void doDelete(void);
void doToggleAll(void);
void doCopy(void);
void doCopySelected(void);
void doDeleteMulti(void);
void doDiskCopy(const BYTE deviceFrom, const BYTE deviceTo);
void execute(char * prg, BYTE device);
int copy(char * srcfile, BYTE srcdevice, char * destfile, BYTE destdevice, BYTE type);
int cmd(unsigned char lfn, unsigned char * cmd);
void deleteSelected(void);
void refreshDir(void);
void clrDir(BYTE context);
void showDir(Directory * cwd, BYTE context);
void printDir(Directory * dir,int xpos, int ypos);
void printElement(Directory * dir,int xpos, int ypos);
//int printErrorChannel(BYTE device);
void about(void);

/* definitions */
BYTE context = 0;
BYTE devices[] = {8,9};
char linebuffer[SCREENW+1];
char answer[40];

Directory * dirs[] = {NULL,NULL};
const char * types [8] = {"DEL","SEQ","PRG","USR","REL","CBM","DIR","---"};

// some filetypes are not yet supported, using u instead
// char shorttypes [8] = {'d','s','p','u','l','c','v','-' };
const char shorttypes [8] = {'u','s','p','u','u','u','u','u' };

#ifdef NOCOLOR
const BYTE textc = COLOR_WHITE;
#else
const BYTE textc = COLOR_LIGHTGREEN;
#endif

int main(void)
{
  initScreen(COLOR_BLACK, COLOR_BLACK, textc);
  mainLoop();
  exitScreen();
  return 0;
}

void updateScreen(void)
{
	BYTE menuy=MENUY;
	clrscr();
	textcolor(textc);
	revers(0);
	updateMenu();
	showDir(dirs[0],0);
	showDir(dirs[1],1);
}

void updateMenu(void)
{
	BYTE menuy=MENUY;

	revers(0);
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
	cputs(" SWITCH");
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
	cputs(" * SEL");
	gotoxy(MENUX+1,menuy++);
	cputs(" C COPY F");
	gotoxy(MENUX+1,menuy++);
	cputs(" D DEL F/D");
	gotoxy(MENUX+1,menuy++);
	cputs(" R REN F");
	gotoxy(MENUX+1,menuy++);
	cputs(" M MAKE D");
	gotoxy(MENUX+1,menuy++);
	cputs(" F FORMAT");
	gotoxy(MENUX+1,menuy++);
	cputs(" . ABOUT");
	gotoxy(MENUX+1,menuy++);
	cputs(" Q QUIT");
	drawFrame(NULL,MENUX,MENUY,MENUW,MENUH+1);
	revers(1);
	textcolor(COLOR_GREEN);
	gotoxy(0,24);

#ifdef CHAR80
  cputs("               DraCopy " DRA_VER "       80 Characters                   ");
#else
  cputs("       DraCopy " DRA_VER "       ");
#endif

	textcolor(textc);
	revers(0);
}

void mainLoop(void)
{
	Directory * cwd = NULL;
	Directory * oldcwd;
	DirElement * current = NULL;
	unsigned int index = 0;
	unsigned int pos = 0;
	BYTE oldcontext;
	BYTE exitflag = 0;
	BYTE c;
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

	updateScreen();
	do
    {
      c = cgetc();
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
					updateScreen();
					break;

        case CH_F3:
					cathex(devices[context],dirs[context]->selected->dirent.name);
					updateScreen();
					break;

        case CH_F4:
					catasc(devices[context],dirs[context]->selected->dirent.name);
					updateScreen();
					break;

        case CH_F5:
					doCopy();
					clrscr();
					// refresh destination dir
					dirs[1-context] = readDir(dirs[1-context],devices[1-context],context);
					updateScreen();
					break;

        case CH_F6:
					doDeleteMulti();
					updateScreen();
					break;

        case CH_F7:
					if (dirs[context]->selected!=NULL)
            {
              execute(dirs[context]->selected->dirent.name,devices[context]);
            }
					exit(0);
					break;

        case CH_F8:
          doDiskCopy(devices[context], devices[1-context]);
					updateScreen();
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
					about();
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

  if (dirs[0]!=NULL) freeDir(&dirs[0]);
  if (dirs[1]!=NULL) freeDir(&dirs[1]);
}


void refreshDir(void)
{
	Directory * cwd = NULL;
	clrDir(context);
	cwd = readDir(cwd, devices[context], context );
	dirs[context]=cwd;
	cwd->selected=cwd->firstelement;
	showDir(cwd,context);
	if (devices[0]==devices[1])
    {
      // refresh also other dir if it's the same drive
      clrDir(1-context);
      dirs[1-context] = readDir(dirs[1-context],devices[1-context],(BYTE)(1-context));
      showDir(cwd,1-context);
    }
}

void doCopySelected(void)
{
  Directory * cwd = GETCWD;

  if (cwd->selected!=NULL)
    {
      sprintf(linebuffer,"Filecopy from device %d to device %d\n",devices[context],devices[1-context]);
      newscreen(linebuffer);
      if (copy(cwd->selected->dirent.name,
               devices[context],
               cwd->selected->dirent.name,
               devices[1-context],
               cwd->selected->dirent.type)==ERROR)
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

      updateScreen();
    }
}

void deleteSelected(void)
{
  DirElement * lastSel;
  Directory * cwd = GETCWD;

	if (cwd->selected!=NULL)
    {
      sprintf(linebuffer,"Delete file/directory on device %d\n",devices[context]);
      newscreen(linebuffer);
      if (really())
        {
          lastSel = cwd->selected;
          cprintf("%s.%s",cwd->selected->dirent.name,types[cwd->selected->dirent.type]);
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
      updateScreen();
    }
}

void showDir(Directory * dir, BYTE mycontext)
{
	char * title;
	if (dir!=NULL)
    {
      title=dir->name;
    }
	else
    {
      title="    no directory";
    }
  if (mycontext > 1)
    return;
	if(mycontext==context)
    {
      sprintf(linebuffer, ">S%02i: %s", (int)devices[mycontext], title);
      textcolor(COLOR_WHITE);
    }
	else
    {
      sprintf(linebuffer, " D%02i: %s", (int)devices[mycontext], title);
      textcolor(textc);
    }

	drawFrame(linebuffer,(mycontext==0)?DIR1X:DIR2X,(mycontext==0)?DIR1Y:DIR2Y,DIRW+2,DIRH+2);
	gotoxy((mycontext==0)?DIR1X+1:DIR2X+1,(mycontext==0)?(DIR1Y+DIRH+1):(DIR2Y+DIRH+1));
	cprintf(">%u blocks free<",dir->free);
	textcolor(textc);
	printDir(dir,(mycontext==0)?DIR1X+1:DIR2X+1,(mycontext==0)?DIR1Y:DIR2Y);
}

void clrDir(BYTE context)
{
	clearArea((context==0)?DIR1X+1:DIR2X+1,((context==0)?DIR1Y:DIR2Y ) ,DIRW,DIRH+1);
}

void about(void)
{
	BYTE idx=0;
	newscreen("About");
	textcolor(COLOR_YELLOW);
	idx=4;
	gotoxy(0,idx++);
	cputs("DraCopy " DRA_VER);
#ifdef CHAR80
	cputs(" 80 Chars");
#endif
	textcolor(COLOR_GREEN);
	idx++;
	idx++;
	gotoxy(0,idx++);
	cputs("      Copyright 2009 by Draco");
	idx++;
	gotoxy(0,idx++);
	cputs("     https://github.com/doj/dracopy");
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
	cputs("KIND.\n");
	idx++;
	gotoxy(0,idx++);
	textcolor(COLOR_LIGHTRED);
	cputs("USE IT AT YOUR OWN RISK!");
	gotoxy(0,24);
	textcolor(COLOR_VIOLET);
	waitKey(0);
	updateScreen();
}

void doCopy(void)
{
	DirElement * current;

	Directory * srcdir;
	Directory * destdir;

	BYTE srcdev = devices[context];
	BYTE destdev = devices[1-context];

	int idx = 0;
	int selidx = 0;
	int x=0;
	int sx=0;

	srcdir=dirs[context];
	destdir=dirs[1-context];

	sprintf(linebuffer,"Filecopy from device %d to device %d\n",srcdev,destdev);
	newscreen(linebuffer);
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
          if (current->flags==1)
            {
              if (copy(current->dirent.name,srcdev,current->dirent.name,destdev,current->dirent.type)==OK)
                {
                  // deselect
                  current->flags=0;
                }
            }
          current=current->next;
        }
      cputs("\n\r");
      waitKey(0);
    }
}


void doToggleAll(void)
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

BYTE really(void)
{
	char c;
	cputs("Really (Y/N)? ");
	c = cgetc();
	cputc(c);
	cputs("\n\r");
	return (c=='y' || c=='Y');
}

void doDeleteMulti(void)
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
      sprintf(linebuffer,"Delete files from device %d\n",devices[context]);
      newscreen(linebuffer);
      current = dirs[context]->firstelement;
      while (current!=NULL)
        {
          if (current->flags==1)
            {
              cprintf("%s.%s\n\r",current->dirent.name,types[current->dirent.type]);
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
                  cprintf("%s.%s",current->dirent.name,types[current->dirent.type]);
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

          waitKey(0);

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

void execute(char * prg, BYTE device)
{
	clrscr();
	gotoxy(0,2);
	cprintf("load\"%s\",%d\n\r\n\r\n\r\n\r\n\r",prg,device);
	cputs("run\n\r");
	gotoxy(0,0);
	*((unsigned char *)KBCHARS)=13;
	*((unsigned char *)KBCHARS+1)=13;
	*((unsigned char *)KBNUM)=2;
}

void printDir(Directory * dir,int xpos, int ypos)
{
	DirElement * current;
	int selidx = 0;
	int page = 0;
	int skip = 0;
	int pos = 0;
	int idx = 0;
  const char *typestr = NULL;
	Directory * cwd = GETCWD;

	//clr(xpos,ypos+1,DIRW,DIRH);

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
          for (idx=0;(idx<skip) && current!=NULL;idx++)
            {
              current=current->next;
              pos++;
            }
        }

      idx=0;

			while (current!=NULL && idx<DIRH)
        {
          gotoxy(xpos,ypos+idx+1);
          if ((current == dir->selected) && (cwd == dir))
            {
              revers(1);
            }
          if (current->flags!=0)
            {
              textcolor(COLOR_WHITE);
              cputc('>');
            }
          else
            {
              cputc(' ');
            }

          if (current->dirent.type < 8)
            {
              typestr = types[current->dirent.type];
            }
          else
            {
              if (current->dirent.type == CBM_T_OTHER)
                typestr = "oth";
              else
                typestr = "doj";
            }
          cprintf("%-4d%-16s %2s", current->dirent.size, current->dirent.name, typestr);
          revers(0);
          textcolor(textc);

          current=current->next;
          ++idx;
        }

			// clear empty lines
			for (;idx<DIRH;idx++)
        {
          gotoxy(xpos,ypos+idx+1);
          cputs("                         ");
        }
    }
}

void printElement(Directory * dir,int xpos, int ypos)
{
	DirElement * current;

  //	char line[MENUX];

	int page = 0;
	int idx = 0;
	int pos = 0;
	int yoff=0;
	Directory * cwd = GETCWD;

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

      gotoxy(xpos,ypos+yoff+1);
      if ((current== dir->selected) && (cwd==dir))
        {
          revers(1);
        }
      if (current->flags!=0)
        {
          textcolor(COLOR_WHITE);
          cputc('>');
        }
      else
        {
          cputc(' ');
        }

      cprintf("%-4d%-16s %2s",current->dirent.size,current->dirent.name,types[current->dirent.type]);
      revers(0);
      textcolor(textc);
    }
}

void doFormat(void)
{
	sprintf(linebuffer,"Format device %d\n",devices[context]);
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
	updateScreen();
}

void doRename(void)
{
	int n;
	Directory * cwd = GETCWD;

	if (cwd->selected!=NULL)
    {
      sprintf(linebuffer,"Rename file %s on device %d\n",cwd->selected->dirent.name,devices[context]);
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
      updateScreen();
    }
}

void doMakedir(void)
{
	int n;
	Directory * cwd = GETCWD;

	sprintf(linebuffer,"Make directory on device %d\n",cwd->selected->dirent.name,devices[context]);
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
	updateScreen();
}

int cmd(unsigned char device, unsigned char * cmd)
{
	int length=0;
	unsigned char f;

	if ((f = cbm_open(15, device, 15, cmd)) != 0)
    {
      cputs("ERROR");
      return(ERROR);
    }
  cbm_close(15);
	return OK;

}

int copy(char * srcfile, BYTE srcdevice, char * destfile, BYTE destdevice, BYTE type)
{
	int length=0;
	unsigned char * linebuffer;
	char deststring[30];

	if( cbm_open (6,srcdevice,CBM_READ,srcfile) != 0)
    {
      cputs("Can't open input file!\n");
      return ERROR;
    }

	// create destination string with filetype like "FILE,P"
	sprintf(deststring,"%s,%c",destfile,shorttypes[type]);
	if( cbm_open (7,destdevice,CBM_WRITE,deststring) != 0)
    {
      cputs("Can't open output file\n");
      cbm_close (6);
      return ERROR;
    }

	linebuffer = (unsigned char *) malloc(BUFFERSIZE);

	cprintf("%-16s:",srcfile);
	do
    {
	  	cputs("R");
      length = cbm_read (6, linebuffer, BUFFERSIZE);

      if (length>=0)
        {
          //cprintf("%d",length);
          cputs("W");
          if (cbm_write(7, linebuffer, length) != length)
            {
              cputc(' ');
              revers(1);
              textcolor(COLOR_VIOLET);
              cputs("ERROR");
              textcolor(textc);
              revers(0);
              cputc(13);
              cputc(10);
              free(linebuffer);
              cbm_close (6);
              cbm_close (7);
              return ERROR;
            }
          //cprintf("%d",length);

        }
    }
  while(length==BUFFERSIZE);
	free(linebuffer);
	cbm_close (6);
	cbm_close (7);
	cputc(' ');
	revers(1);
	cputs("OK");
	revers(0);
	cputc(13);
	cputc(10);

	return OK;
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

void
doDiskCopy(const BYTE deviceFrom, const BYTE deviceTo)
{
	clrscr();
	gotoxy(0,0);
	cprintf("copy %i %i", deviceFrom,deviceTo);
	waitKey(0);
  
}
