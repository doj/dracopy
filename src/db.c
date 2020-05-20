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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <screen.h>
#include <conio.h>
#include <cbm.h>
#include <errno.h>
#include <cat.h>
#include <dir.h>
#include <base.h>
#include <defines.h>


/* declarations */
void about(void);
void mainLoop(void);
void updateScreen(void);
void updateMenu(void);

void execute(char * prg, BYTE device);
int cmd(unsigned char lfn, unsigned char * cmd);
void refreshDir(void);
void clrDir(BYTE context);
void showDir(Directory * cwd, BYTE context);
void printDir(Directory * dir,int xpos, int ypos);
void printElement(Directory * dir,int xpos, int ypos);

/* definitions */
BYTE context = 0;
BYTE devices[] = {8,9};
char linebuffer[SCREENW+1];
char answer[40];

Directory * dirs[] = {NULL,NULL};
char * types [8] = {"DEL","SEQ","PRG","USR","REL","CBM","VRP","---"};

// some filetypes are not yet supported, using u instead
// char shorttypes [8] = {'d','s','p','u','l','c','v','-' };
char shorttypes [8] = {'u','s','p','u','u','u','u','u' };


#ifdef NOCOLOR
  BYTE textc = COLOR_WHITE;
#else
  BYTE textc = COLOR_LIGHTGREEN;
#endif

#ifdef DIRH
#undef DIRH
#define DIRH 22
#endif

#ifdef MENUX
#undef MENUX
#define MENUX 27
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
#ifndef __VIC20__
	  updateMenu();
#endif
	showDir(dirs[0],0);
}


void updateMenu(void)
{
	BYTE menuy=MENUY;

	revers(0);
	menuy+=2;
	gotoxy(MENUX+1,menuy++);
	cprintf(" F1 DIR");
	gotoxy(MENUX+1,menuy++);
	cprintf(" F2 DEVICE");
	gotoxy(MENUX+1,menuy++);
	cprintf(" F3 HEX");
	gotoxy(MENUX+1,menuy++);
	cprintf(" F4 ASC");
	gotoxy(MENUX+1,menuy++);
#ifdef __PLUS4__
	cprintf("ESC SWITCH");
#else
	cputc(' ');
	cputc(95); // arrow left
	cprintf("  SWITCH");
#endif
	gotoxy(MENUX+1,menuy++);
	cprintf(" CR RUN / CD");
	gotoxy(MENUX+1,menuy++);
	cprintf(" BS DIR UP");
	gotoxy(MENUX+1,menuy++);
	cprintf(" T TOP");
	gotoxy(MENUX+1,menuy++);
	cprintf(" B BOTTOM");
	gotoxy(MENUX+1,menuy++);
	cprintf(" . ABOUT");
	gotoxy(MENUX+1,menuy++);
	cprintf(" Q QUIT");
	menuy++;
	gotoxy(MENUX+1,menuy++);
	cprintf("Device:%02d",devices[0]);
	drawFrame("B64 V1.0",MENUX,MENUY,MENUW,MENUH+1);
	revers(1);
	textcolor(COLOR_GREEN);
	gotoxy(0,BOTTOM);

#ifdef CHAR80
	  cprintf("               DraBrowse V1.0C 8 Bit (27.12.2009) 80 Characters                 ");
#else
	  cprintf("   DraBrowse V1.0C 8 Bit (27.12.2009)   ");
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

	updateScreen();
	do
	{
		c = cgetc();
    	switch (c)
      	{
			case CH_F1:
					dirs[context]=readDir(dirs[context],devices[context],context);
					clrDir(context);
					showDir(dirs[context],context);
					break;

			case CH_F2:
					if (devices[context]++>12) devices[context]=8;
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
					while (1==1)
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
					sprintf(linebuffer,"cd: \n");
					linebuffer[3]=95; // arrow left
					cmd(devices[context],linebuffer);
					refreshDir();
					break;
	    }
	}
	while(exitflag==0);

    if (dirs[0]!=NULL) freeDir(&dirs[0]);
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

void showDir(Directory * dir, BYTE mycontext)
{
	char * title;
	if (dir!=NULL)
	{
		title=dir->name;
	}
	else
	{
		title="no directory";
	}
	if(mycontext==context)
	{
		sprintf(linebuffer,"> %16s <",title);
		textcolor(COLOR_WHITE);
	}
	else
	{
		sprintf(linebuffer,"%16s",title);
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
	clrscr();
	textcolor(COLOR_YELLOW);
	idx=4;
	gotoxy(0,idx++);
#ifdef CHAR80
	cprintf("Draco Browser V1.0C 8 Bit (27.12.2009) 80 Chars");
#else
	cprintf("Draco Browser V1.0C 8 Bit (27.12.2009)");
#endif
	textcolor(COLOR_GREEN);
	idx++;
	idx++;
	gotoxy(0,idx++);
	cprintf("       Copyright 2009 by Draco");
	idx++;
	gotoxy(0,idx++);
	cprintf("    Mail: DracoSan@googlemail.com");
	idx++;
	idx++;
	idx++;
	gotoxy(0,idx++);
	cprintf("THIS PROGRAM IS DISTRIBUTED IN THE HOPE");
	gotoxy(0,idx++);
	cprintf("THAT IT WILL BE USEFUL.");
	idx++;
	gotoxy(0,idx++);
	cprintf("IT IS PROVIDED WITH NO WARRANTY OF ANY ");
	gotoxy(0,idx++);
	cprintf("KIND.\n");
	idx++;
	gotoxy(0,idx++);
	textcolor(COLOR_LIGHTRED);
	cprintf("USE IT AT YOUR OWN RISK!");
	gotoxy(0,BOTTOM);
	textcolor(COLOR_VIOLET);
	waitKey(0);
	updateScreen();
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
	Directory * cwd = GETCWD;


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

	if (dir==NULL)
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

