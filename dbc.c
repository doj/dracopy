/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */
/*
 * dbc.c
 * Created on: 22.04.2018
 * Author: bader++
 * DraBrowse/Copy (dbc*) is a simple copy and file browser.
 * Since make use of kernal routines they shall be able to work with most file oriented IEC devices.
 * Created 2009-2018 by Sascha Bader and updated by Me
 * The code can be used freely as long as you retain a notice describing original source and author.
 * THE PROGRAMS ARE DISTRIBUTED IN THE HOPE THAT THEY WILL BE USEFUL, BUT WITHOUT ANY WARRANTY. USE THEM AT YOUR OWN RISK!
 * Newer versions might be available here: http://www.sascha-bader.de/html/code.html
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <errno.h>
#include <cbm.h>
#include <base.h>
#include <dir.h>
#include <screen.h>
#include <cat.h>
#include <em.h>
#include <defines.h>
#include <dio.h>

void about(void);
void mainLoop(void);
void updateScreen(void);
void refreshDir(void);
void doDelete(void);
void doToggleAll(void);
void doRename(void);
void doRelabel(void);
void doMakeDir(void);
void doMakeImage(void);
void doFileCopy(void);
void doFormat(void);
void doDiskCopy(void);
void clearBar(void);
void updateMenu(void);
void doDown(void);
void doUp(void);
void doRight(void);
void doLeft(void);
void doDeviceUp(void);
void doDeviceDown(void);
void changeDir(unsigned char device, struct pwd *path);
void execute(char *prg, unsigned char drive);
void printMessage(unsigned char color, char *message);
void showDir(Directory *dir);
void printDir(Directory *dir);
void printElement(Directory *dir);
void* makePath(unsigned char device);
unsigned char diskType(unsigned char device);
unsigned char diskImage(char *currentfile);
unsigned char driveConnected(unsigned char drive);
unsigned char dosCommand(unsigned char lfn, unsigned char drive, unsigned char sec_addr, char *string);
unsigned char fileCopy(unsigned char srcdevice, struct pwd *srcpath, unsigned char destdevice, struct pwd *destpath, char *currentfile, unsigned char ftype);
unsigned char diskCopy(unsigned char srcdevice, unsigned char destdevice);

Directory *cwd = NULL;
DirElement *current = NULL;
unsigned int pos;
unsigned char nextpage;
unsigned char c;
unsigned char menu;
unsigned char reu;
unsigned char idx;
unsigned char lastpage;
signed char device;
const unsigned char drive[] = {8, 9, 10, 11};
unsigned char imgparent[sizeof drive];
unsigned char dirlevel[sizeof drive];
unsigned char imgtype[sizeof drive];
unsigned char drvtype[sizeof drive];
char *dirname[sizeof drive][MAXLEVEL];
char leftarrow[] = {95, 0};
char message[40];
char name[17];
char sbuffer[128];
char lbuffer[256];
extern char EMD;
struct pwd {char *bottom; char *top;};
enum {NONE, D1541, D1571, D1581, SD2IEC, CMD, VICE};
const char *imagetype[] = {"\0", ".d64", ".d71", ".d81", ".dnp"};
const char *drivetype[] = {"\0", "1541", "1571", "1581", "sd2iec", "cmd", "vice"};
const char *filetype[] = {"del", "cbm", "dir", "lnk", "---", "seq", "prg", "usr", "rel", "vrp"};
const char shorttype[] = {'s', 'p', 'u', 'r', 'v'};
const char sectors[] = {21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
						19, 19, 19, 19, 19, 19, 19,
						18, 18, 18, 18, 18, 18,
						17, 17, 17, 17, 17};

int main (void){
	#ifdef __C128__
		if(*(char *)0x00d7 == 0x80) *(char *)0xd030 = 1;
	#endif
	for(device = 0; device < sizeof drive; ++device){
		if(driveConnected(drive[device]) && dosCommand(15, drive[device], 15, "ui") == 73){
			for(idx = 0; idx < sizeof drivetype / sizeof drivetype[0]; ++idx){
				if(strstr(message, drivetype[idx])) drvtype[device] = idx;
			}
		}
	}
	device = 0;
	//reu = !em_install(&EMD);
  reu = em_load_driver ("c64-reu.emd");
	initScreen(COLOR_BORDER, COLOR_BACKGROUND, COLOR_TEXT);
	updateScreen();
	refreshDir();
	mainLoop();
	exitScreen();
	em_uninstall();

	return 0;
}

void updateScreen(void){

	clrscr();
	showDir(cwd);
	updateMenu();
	clearBar();
}

void updateMenu(void){

	revers(0);
	textcolor(COLOR_MENU);
	drawFrame(strupper(strcpy(name, drivetype[drvtype[device]])), MENUX, MENUY, MENUW, MENUH + 1);
	cputsxy(MENUX + 1, MENUY +  2, "F1 Dir");
	cputsxy(MENUX + 1, MENUY +  3, "F3 Dev[+]");
	cputsxy(MENUX + 1, MENUY +  4, "F5 Dev[-]");
	cputsxy(MENUX + 1, MENUY +  5, "F7 Asc");
	cputsxy(MENUX + 1, MENUY +  6, "F8 Hex");
	cputsxy(MENUX + 1, MENUY +  7, "CR Run");
	cputsxy(MENUX + 1, MENUY +  8, "SP Select");
	cputsxy(MENUX + 1, MENUY +  9, "CRSR Nav");
	cputsxy(MENUX + 1, MENUY + 10, menu ? "R Rename " : "L Load   ");
	cputsxy(MENUX + 1, MENUY + 11, menu ? "M Makedir" : "* Toogle ");
	cputsxy(MENUX + 1, MENUY + 12, menu ? "I Makeimg" : "T Top    ");
	cputsxy(MENUX + 1, MENUY + 13, menu ? "F Format " : "B Bottom ");
	cputsxy(MENUX + 1, MENUY + 14, menu ? "K Dskcopy" : "S Sort   ");
	cputsxy(MENUX + 1, MENUY + 15, menu ? "V Move   " : "C Copy   ");
	cputsxy(MENUX + 1, MENUY + 16, menu ? "N Relabel" : "D Delete ");
	cputsxy(MENUX + 1, MENUY + 17, "E Reu");
	cputsxy(MENUX + 1, MENUY + 18, "O Other");
	cputsxy(MENUX + 1, MENUY + 19, ". About");
	cputsxy(MENUX + 1, MENUY + 20, "Q Quit");
	cputsxy(MENUX + 3, MENUH -  2, reu ? "[Reu]" : "     ");
	cputsxy(MENUX + 2, MENUH -  1, "Drive ");
	cprintf("%-2d", drive[device]);
}

void clearBar(void){

	revers(1);
	textcolor(COLOR_MENU);
	cclearxy(0, BOTTOM, 40);
	cputsxy(10, BOTTOM, "DraBrowse/Copy V1.0e");
	textcolor(COLOR_TEXT);
	revers(0);
}

void refreshDir(void){

	if(!dirlevel[device]){
		changeDir(device, NULL);
		dirlevel[device] = 0;
	}
	cwd = readDir(cwd, drive[device]);
	showDir(cwd);
}

void mainLoop(void){

	unsigned char exitflag = 0;

	do{
		c = cgetc();
		switch(c){

			case CH_F1:
				refreshDir();
				break;

			case CH_F3:
			case '+':
				doDeviceUp();
				break;

			case CH_F5:
			case '-':
				doDeviceDown();
				break;

			case CH_F7:
				if(cwd->selected && cwd->selected->dirent.type != CBM_T_DIR){
					catasc(drive[device], cwd->selected->dirent.name, cwd->selected->dirent.type);
					updateScreen();
				}
				break;

			case CH_F8:
				if(cwd->selected && cwd->selected->dirent.type != CBM_T_DIR){
					cathex(drive[device], cwd->selected->dirent.name, cwd->selected->dirent.type);
					updateScreen();
				}
				break;

			case ' ':
				cwd->selected->flags = !cwd->selected->flags;
				/*if(cwd->selected->next){
					cwd->selected = cwd->selected->next;
					cwd->pos++;
				}*/
				showDir(cwd);
				break;

			case '*':
				doToggleAll();
				break;

			case 'd':
				if(cwd) doDelete();
				break;

			case 'r':
				if(cwd) doRename();
				break;

			case 'n':
				if(cwd) doRelabel();
				break;

			case 'm':
				if(cwd) doMakeDir();
				break;

			case 'i':
				if(cwd) doMakeImage();
				break;

			case 'k':
				if(cwd) doDiskCopy();
				break;

			case 'c':
			case 'v':
				if(cwd) doFileCopy();
				break;

			case 's':
				sortDir(cwd);
				showDir(cwd);
				break;

			case 'f':
				doFormat();
				break;

			case 'o':
				menu = !menu;
				updateMenu();
				break;

			case 'q':
				exitflag = 1;
				break;

			case 'e':
				if (! reu)
          {
            //reu = !em_install(&EMD);
            reu = em_load_driver ("c64-reu.emd");
          }
				else
          {
            reu = em_uninstall();
          }
				updateMenu();
				break;

			case 't':
				cwd->selected = cwd->firstelement;
				cwd->pos = 0;
				printDir(cwd);
				break;

			case 'b':
				current = cwd->firstelement;
				pos = 0;
				while(1){
					if(current->next){
						current = current->next;
						++pos;
					}
					else break;
				}
				cwd->selected = current;
				cwd->pos = pos;
				printDir(cwd);
				break;

			case '.':
				about();
				break;

			case CH_ENTER:
			case 'l':
				if(cwd->selected && cwd->selected->dirent.type == CBM_T_PRG){
					execute(cwd->selected->dirent.name, drive[device]);
					exit(0);
				}
			case CH_CURS_DOWN:
				doDown();
				break;

			case CH_CURS_UP:
				doUp();
				break;

			case CH_CURS_RIGHT:
				doRight();
				break;

			case CH_CURS_LEFT:
				doLeft();
				break;
		}
	}
	while(!exitflag);

	if(cwd) freeDir(&cwd);

	for(device = 0; device < sizeof drive; ++device){
		for(dirlevel[device] = 0; dirlevel[device] < MAXLEVEL; ++dirlevel[device]){
			if(dirname[device][dirlevel[device]]) free(dirname[device][dirlevel[device]]);
		}
	}
}

void doDeviceUp(void){

	do if(++device == sizeof drive) device = 0;
	while(!drvtype[device]);
	updateMenu();
	refreshDir();
}

void doDeviceDown(void){

	do if(--device < 0) device = sizeof drive - 1;
	while(!drvtype[device]);
	updateMenu();
	refreshDir();
}

void doDown(void){

	if(cwd->selected && cwd->selected->next){
		cwd->selected = cwd->selected->next;
		pos = cwd->pos;
		lastpage = pos / DIRH;
		nextpage = (pos + 1) / DIRH;

		if(lastpage != nextpage){
			++cwd->pos;
			printDir(cwd);
		}
		else{
			printElement(cwd);
			++cwd->pos;
			printElement(cwd);
		}
	}
}

void doUp(void){

	if(cwd->selected && cwd->selected->previous){
		cwd->selected = cwd->selected->previous;
		pos = cwd->pos;
		lastpage = pos / DIRH;
		nextpage = (pos - 1) / DIRH;

		if(lastpage != nextpage){
			--cwd->pos;
			printDir(cwd);
		}
		else{
			printElement(cwd);
			--cwd->pos;
			printElement(cwd);
		}
	}
}

void doRight(void){

	if(dirlevel[device] == MAXLEVEL || !strcmp(cwd->selected->dirent.name, ".") || !strcmp(cwd->selected->dirent.name, "..")) return;

	else if(cwd->selected->dirent.type == CBM_T_DIR || diskImage(cwd->selected->dirent.name)){
		sprintf(lbuffer, "cd:%s", cwd->selected->dirent.name);

		if(!dosCommand(15, drive[device], 15, lbuffer)){
			if(dirname[device][dirlevel[device]]) free(dirname[device][dirlevel[device]]);
			strcpy(dirname[device][dirlevel[device]] = (char *)malloc(strlen(cwd->selected->dirent.name) + 1), cwd->selected->dirent.name);
			++dirlevel[device];
			refreshDir();
		}
	}
}

void doLeft(void){

	if(dirlevel[device]){
		sprintf(lbuffer, "cd:%s", leftarrow);

		if(!dosCommand(15, drive[device], 15, lbuffer)){
			--dirlevel[device];

			if(diskImage(dirname[device][dirlevel[device]])){
				imgtype[device] = 0;
				imgparent[device] = 0;
			}
			refreshDir();
		}
	}
}

unsigned char diskImage(char *currentfile){

	for(idx = 1; idx < sizeof imagetype / sizeof imagetype[0]; ++idx){

		if(strstr(strlower(strcpy(name, currentfile)), imagetype[idx])){
			imgtype[device] = idx;
			imgparent[device] = dirlevel[device] + 1;
			return idx;
		}
	}
	return 0;
}

void* makePath(unsigned char device){

	struct pwd *fullpath = (struct pwd *)malloc(sizeof(struct pwd));

	fullpath->bottom = (char *)malloc(256);
	fullpath->top = (char *)malloc(256);

	strcpy(fullpath->bottom, drvtype[device] != VICE ? "/" : "\0");

	for(idx = 0; idx < imgparent[device]; ++idx){
		strcat(fullpath->bottom, "/");
		strcat(fullpath->bottom, dirname[device][idx]);
	}
	strcpy(fullpath->top, drvtype[device] != VICE ? "/" : "\0");

	for(idx = imgparent[device]; idx < dirlevel[device]; ++idx){
		strcat(fullpath->top, "/");
		strcat(fullpath->top, dirname[device][idx]);
	}
	strcat(fullpath->top, drvtype[device] != VICE ? "/:" : "/");

	return fullpath;
}

void changeDir(unsigned char device, struct pwd *path){

	sprintf(lbuffer, "cd%s", drvtype[device] != VICE ? "//" : ":/");
	sprintf(sbuffer, "cd:%s", leftarrow );

	dosCommand(15, drive[device], 15, lbuffer);
	dosCommand(15, drive[device], 15, sbuffer);
	dosCommand(15, drive[device], 15, lbuffer);

	sprintf(lbuffer, drvtype[device] != VICE ? "cd%s" : "cd:%s", path->bottom);
	dosCommand(15, drive[device], 15, lbuffer);

	sprintf(lbuffer, drvtype[device] != VICE ? "cd%s" : "cd:%s", path->top);
	dosCommand(15, drive[device], 15, lbuffer);
}

void doFileCopy(void){

	Directory *srcdir = cwd;

	struct pwd *fullpath = (struct pwd *)malloc(sizeof(struct pwd));
	struct pwd *srcpath = (struct pwd *)malloc(sizeof(struct pwd));
	struct pwd *destpath = (struct pwd *)malloc(sizeof(struct pwd));

	unsigned char k;
	unsigned char srcdevice = device;
	unsigned char srcimgtype = imgtype[device];
	unsigned char selectflag = 0;

	srcpath = makePath(device);
	strcpy(name, dirname[device][imgparent[device] - 1]);

	cwd = readDir(NULL, drive[device]);

	current = cwd->firstelement;
	pos = 0;
	while(pos < srcdir->pos){
		current = current->next;
		++pos;
	}
	cwd->selected = current;
	cwd->pos = pos;
	printDir(cwd);

	while(1){
	printMessage(COLOR_SIGNAL, "Select dest. drive/dir.   <return> <esc>");

		do{
			k = cgetc();
			switch(k){

				case CH_ESC:
					clearBar();
					return;

				case CH_F1:
					refreshDir();
					break;

				case CH_F3:
				case '+':
					doDeviceUp();
					break;

				case CH_F5:
				case '-':
					doDeviceDown();
					break;

				case CH_CURS_DOWN:
					doDown();
					break;

				case CH_CURS_UP:
					doUp();
					break;

				case CH_CURS_RIGHT:
					doRight();
					break;

				case CH_CURS_LEFT:
					doLeft();
					break;
				case 's':
					sortDir(cwd);
					showDir(cwd);
					break;
			}
		}
		while(k != CH_ENTER);

		if(srcdevice != device && c == 'v'){
			printMessage(COLOR_SIGNAL, "Can't move to other drives");
			continue;
		}
		else if(!reu && srcdevice == device && (srcimgtype ? 1 : 0) | (imgtype[device] ? 1 : 0) && strcmp(name, dirname[device][imgparent[device] - 1])){
			printMessage(COLOR_SIGNAL, "Can't copy without REU");
			continue;
		}
		else if(!cwd) continue;
		else break;
	}
	destpath = makePath(device);
	current = srcdir->firstelement;

	while(current){
		if(current->flags) selectflag = 1;
		current = current->next;
	}
	if(!selectflag) srcdir->selected->flags = 1;
	current = srcdir->firstelement;

	while(current){
		if(current->flags == 1){
			sprintf(lbuffer, (c == 'c') ? "Copying %-16s" : "Moving %-16s", current->dirent.name);
			printMessage(COLOR_TEXT, lbuffer);

			if(!fileCopy(srcdevice, srcpath, device, destpath, current->dirent.name, current->dirent.type)){
				if(c == 'v'){
					changeDir(srcdevice, srcpath);
					sprintf(lbuffer, "s:%s", current->dirent.name);
					if(dosCommand(15, drive[device],15, lbuffer) > 1) printMessage(COLOR_SIGNAL, message);
				}
			}
			else printMessage(COLOR_SIGNAL, message);
		}
		if(kbhit() && cgetc() == CH_ESC) break;
		current = current->next;
	}
	changeDir(device, makePath(device));
	refreshDir();
	clearBar();

	free(fullpath);
	free(srcpath);
	free(destpath);
}

void doDelete(void){

	unsigned char selectflag = 0;

	current = cwd->firstelement;
	while(current){
		if(current->flags) selectflag = 1;
		current = current->next;
	}
	sprintf(lbuffer, "Delete files/dirs on drive %d", drive[device]);
	printMessage(COLOR_TEXT, lbuffer);

	if(really()){
		if(!selectflag) cwd->selected->flags = 1;
		current = cwd->firstelement;
		while(current){
			if(current->flags == 1){
				sprintf(lbuffer, "Deleting %-16s", current->dirent.name);
				printMessage(COLOR_TEXT, lbuffer);
				sprintf(lbuffer, current->dirent.type != CBM_T_DIR ? "s:%s" : "rd:%s", current->dirent.name);

				if(dosCommand(15, drive[device], 15, lbuffer) > 1) printMessage(COLOR_SIGNAL, message);
			}
			if(kbhit() && cgetc() == CH_ESC) break;
			current = current->next;
		}
		refreshDir();
		clearBar();
	}
	else clearBar();
}

void doRename(void){

	printMessage(COLOR_TEXT, "Rename:                   <return> <esc>");
	strcpy(name, cwd->selected->dirent.name);

	if(textInput(8, BOTTOM, name)){
		if(dosCommand(2, drive[device], CBM_READ, name) != 62){
			printMessage(COLOR_SIGNAL, "File exists");
			clearBar();
		}
		else{
			sprintf(lbuffer, "r:%s=%s", name, cwd->selected->dirent.name);
			if(dosCommand(15, drive[device], 15, lbuffer)) printMessage(COLOR_SIGNAL, message);
			refreshDir();
			clearBar();
		}
	}
	else clearBar();
}

void doMakeDir(void){

	printMessage(COLOR_TEXT, "Makedir:                  <return> <esc>");
	memset(name, 0, sizeof name);

	if(textInput(9, BOTTOM, name)){
		sprintf(lbuffer, "md:%s", name);

		if(dosCommand(15, drive[device], 15, lbuffer)) printMessage(COLOR_SIGNAL, message);
		refreshDir();
		clearBar();
	}
	else clearBar();
}

void doFormat(void){

	sprintf(lbuffer, "Format drive %d", drive[device]);
	printMessage(COLOR_TEXT, lbuffer);

	if(really()){
		printMessage(COLOR_TEXT, "Name:                     <return> <esc>");
		memset(name, 0, sizeof name);

		if(textInput(6, BOTTOM, name)){
			printMessage(COLOR_TEXT, "Formating");
			sprintf(lbuffer, "n:%s", name);

			if(dosCommand(15, drive[device], 15, lbuffer)) printMessage(COLOR_SIGNAL, message);
		}
		refreshDir();
		clearBar();
	}
	else clearBar();
}

void doDiskCopy(void){

	Directory *srcdir = cwd;
	unsigned char srcdevice = device;

	cwd = NULL;
	while(1){
		printMessage(COLOR_SIGNAL, "Select dest. drive        <return> <esc>");

		do{
			c = cgetc();
			switch(c){

				case CH_ESC:
					clearBar();
					return;

				case CH_F1:
					refreshDir();
					break;

				case CH_F3:
				case '+':
					doDeviceUp();
					break;

				case CH_F5:
				case '-':
					doDeviceDown();
					break;

				case CH_CURS_DOWN:
					doDown();
					break;

				case CH_CURS_UP:
					doUp();
					break;

				case CH_CURS_RIGHT:
					doRight();
					break;

				case CH_CURS_LEFT:
					doLeft();
					break;
			}
		}
		while(c != CH_ENTER);

		if(srcdevice == device){
			printMessage(COLOR_SIGNAL, "Can't copy to the same drive");
			continue;
		}
		else if(!cwd) continue;
		else break;
	}
	sprintf(lbuffer, "Copy disk from drive %d to %d", drive[srcdevice], drive[device]);
	printMessage(COLOR_TEXT, lbuffer);

	if(really()){
		if(diskCopy(srcdevice, device)) printMessage(COLOR_SIGNAL, message);
		updateScreen();
		refreshDir();
	}
	else clearBar();
}

void doToggleAll(void){

	current = cwd->firstelement;
	while(current){
		current->flags = !(current->flags);
		current = current->next;
	}
	showDir(cwd);
}

void showDir(Directory *dir){

	char *title;

	revers(0);
	if(!dir){
		title = "No disk";
		dosCommand(15, drive[device], 15, "ui");
		for(idx = 1; idx < (DIRH + 2); ++idx) cclearxy(1, idx, DIRW);
	}
	else title = dir->name;
	sprintf(sbuffer, !imgtype[device] ? "\0" : "%s%3s%s", "[", strupper(strcpy(name, imagetype[imgtype[device]] + 1)), "]");
	sprintf(lbuffer, ">%-16s%5s<", title, sbuffer);
	textcolor(COLOR_FRAME);
	drawFrame(lbuffer, 0, 0, DIRW + 2, DIRH + 2);

	if(dir){
		gotoxy(2, DIRH + 1);
		cprintf(">%u blocks free<", dir->free);
		textcolor(COLOR_TEXT);
		printDir(dir);
	}
}

void about(void){

	clrscr();
	revers(0);
	cputsxy( 1, 6,  "Draco Browser / Copy V1.0e (Apr.2018)");
	cputsxy( 4, 8,  "Copyright 2009-2018 by Draco++");
	cputsxy( 0, 11, "This program is distributed in the hope");
	cputsxy( 8, 12, "that it will be useful.");
	cputsxy( 0, 14, "It is provided with no warranty of any");
	cputsxy(16, 15, "kind.");
	cputsxy( 7, 17, "Use it at your own risk!");
	gotoxy(8, 20);
	waitKey(0);
	updateScreen();
}

void execute(char *prg, unsigned char drive){

	exitScreen();
	em_uninstall();
	gotoxy(0, 2);
	cprintf("load\"%s\",%d", prg, drive);
	if(c == CH_ENTER) cputsxy(0, 7, "run");
	gotoxy(0, 0);
	*((char *) KBCHARS) = 13;
	*((char *) KBCHARS + 1) = 13;
	*((char *) KBLENGTH) = c == CH_ENTER ? 2 : 1;
}

void printDir(Directory *dir){

	unsigned char page;
	unsigned char skip;

	if(!dir) return;
	else{
		current = dir->firstelement;
		idx = 0;
		while(current){
			if(current == dir->selected) break;
			++idx;
			current = current->next;
		}
		page = idx / DIRH;
		skip = page * DIRH;

		current = dir->firstelement;
		if(page > 0){
			for(idx = 0; (idx < skip) && current; ++idx){
				current = current->next;
				++pos;
			}
		}
		idx = 0;
		while(current && idx < DIRH){
			gotoxy(1, idx + 1);
			if(current == dir->selected) revers(1);

			if(current->flags){
				textcolor(COLOR_FRAME);
				cputc('>');
			}
			else cputc(' ');

			sprintf(sbuffer, current->dirent.type == CBM_T_DIR ? leftarrow : "%5u", current->dirent.size);
			cprintf("%5s %-16.16s %3s", sbuffer, current->dirent.name, current->dirent.type & _CBM_T_REG ? filetype[current->dirent.type - 11] : filetype[current->dirent.type]);

			revers(0);
			textcolor(COLOR_TEXT);

			current = current->next;
			++idx;
		}
		for(; idx < DIRH; ++idx) cclearxy(1, idx + 1, DIRW);
	}
}

void printElement(Directory *dir){

	unsigned char page;
	unsigned char yoff;

	if(!dir) return;
	else{
		revers(0);
		textcolor(COLOR_TEXT);
		current = dir->firstelement;
		pos = dir->pos;
		idx = pos;

		while(current && idx--) current = current->next;

		page = pos / DIRH;
		yoff = pos - page * DIRH;
		gotoxy(1, yoff + 1);

		if(current == dir->selected) revers(1);

		if(current->flags){
			textcolor(COLOR_FRAME);
			cputc('>');
		}
		else cputc(' ');

		sprintf(sbuffer, current->dirent.type == CBM_T_DIR ? leftarrow : "%5u", current->dirent.size);
		cprintf("%5s %-16.16s %3s", sbuffer, current->dirent.name, current->dirent.type & _CBM_T_REG ? filetype[current->dirent.type - 11] : filetype[current->dirent.type]);

		revers(0);
	}
}

unsigned char dosCommand(unsigned char lfn, unsigned char drive, unsigned char sec_addr, char *string){

	memset(message, 0, sizeof message);
	cbm_open(lfn, drive, sec_addr, string);

	if(lfn != 15) cbm_open(15, drive, 15, "");
	cbm_read(15, message, sizeof message);

	if(lfn != 15) cbm_close(15);
	cbm_close(lfn);

	message[strcspn(message, "\n")] = 0;

	return (message[0] - 48) * 10 + message[1] - 48;
}

unsigned char driveConnected(unsigned char drive){

	cbm_k_setlfs(15, drive, 15);
	cbm_k_setnam(0);
	cbm_k_open();
	cbm_k_close(15);

	return !cbm_k_readst();
}

unsigned char diskType(unsigned char device){

	dosCommand(15, drive[device], CBM_SEQ, "$");

	if(message[0] == 0x41){
		if(drvtype[device] == SD2IEC && imgtype[device] == NONE) return 0;
		return message[1] & 0x80 ? 70 : 35;
	}
	else if(message[0] == 0x44) return 80;
	return 0;
}

void printMessage(unsigned char color, char *message){

	textcolor(color);
	revers(1);
	cclearxy(0, BOTTOM, 40);
	cputsxy(0, BOTTOM, message);
	delay(TIME);
}

unsigned char fileCopy(unsigned char srcdevice, struct pwd *srcpath, unsigned char destdevice, struct pwd *destpath, char *currentfile, unsigned char ftype){

	struct em_copy em;
	struct relative{char cmd; unsigned char channel; unsigned int recnumber; unsigned char position;};
	struct relative command = {'p', 96 + 2, 1, 0};
	signed int length;
	signed int lastlength;
	unsigned int emlastpage;
	unsigned char relsize;
	unsigned char errn = 0;
	unsigned char errc = 0;
	char *deststring = (char *)malloc(256);
	char *srcstring = (char *)malloc(256);
	char *buffer = (char *)malloc(BUFFERSIZE);

	changeDir(destdevice, destpath);
	sprintf(deststring, "%s", currentfile);
	sprintf(deststring, "%s,%c", currentfile, shorttype[ftype]);

	if(!(ftype & _CBM_T_REG)){
		sprintf(message, "Can't copy %s files", filetype[ftype]);
		errc = 99;
	}
	else if(dosCommand(2, drive[destdevice], CBM_READ, deststring) != 62){
		strcpy(message, "File exists");
		errc = 99;
	}
	else{
		if(reu){
			#ifdef __C128__
				if(*(char *)0x00d7 == 0x80) *(char *)0xd030 = 0;
			#endif

			if (ftype == CBM_T_REL){
				changeDir(srcdevice, srcpath);
				sprintf(srcstring, "%s,l", currentfile);
				cbm_open(2, drive[srcdevice], 2, srcstring);
				cbm_open(5, drive[srcdevice], 15, "");

				command.position = 254;
				do{
					cbm_write(5, &command, sizeof command);
					cbm_read(5, &errn, sizeof errn);
					if(!(errn - 48)) break;
				}
				while (--command.position > 0);

				cbm_close(2);
				cbm_close(5);

				relsize = command.position;

				if(!relsize){
					strcpy(message, "Record not present");
					errc = 99 ;
				}
				else{
					sprintf(sbuffer, "Record size %d", relsize);
					cprintf(sbuffer);

					delay(TIME);
					cclearxy(24, BOTTOM, 15);

					em.buf = lbuffer;
					em.count = sizeof lbuffer;
					em.offs = 0;
					em.page = 0;

					sprintf(srcstring, "%s,l,%c", currentfile, relsize);
					cbm_open(2, drive[srcdevice], 2, srcstring);
					cbm_open(5, drive[srcdevice], 15, "");

					gotoxy(32, BOTTOM);
					cputs("R ");

					command.recnumber = 1;
					command.position = 0;

					while(1){
						cbm_write(5, &command, sizeof command);
						memset(lbuffer, 0, sizeof lbuffer);
						cbm_read(2, lbuffer, relsize);
						cbm_read(5, &errn, sizeof errn);

						if(errn - 48) break;

						em_copyto(&em);
						em.page += (em.count >> 8);

						++command.recnumber;
					}

					changeDir(destdevice, destpath);
					sprintf(deststring, "%s,l,%c", currentfile, relsize);
					cbm_open(3, drive[destdevice], 3, deststring);

					emlastpage = em.page;
					em.page = 0;

					gotoxy(33, BOTTOM);
					cputs("W ");

					command.recnumber = 1;
					command.position = 0;

					do{
						em_copyfrom(&em);
						em.page += (em.count >> 8);

						cbm_write(5, &command, sizeof command);
						cbm_write(3, lbuffer, relsize);

						++command.recnumber;
					}
					while(em.page < emlastpage);
				}
			}
			else{
				changeDir(srcdevice, srcpath);
				cbm_open(2, drive[srcdevice], ftype ? CBM_SEQ : CBM_READ, currentfile);
				cbm_open(5, drive[srcdevice], 15, "");

				em.buf = buffer;
				em.count = BUFFERSIZE;
				em.offs = 0;
				em.page = 0;

				gotoxy(32, BOTTOM);
				cputs("R ");

				do{
					length = cbm_read(2, buffer, em.count);
					em_copyto(&em);
					em.page += (em.count >> 8);

					if (em.page > em_pagecount()){
						strcpy(message, "File too large");
						errc = 99;
						break;
					}
				}
				while(length == em.count);

				changeDir(destdevice, destpath);
				cbm_open(3, drive[destdevice], CBM_WRITE, deststring);

				lastlength = length;
				emlastpage = em.page;
				em.page = 0;

				gotoxy(33, BOTTOM);
				cputs("W ");

				do{
					em_copyfrom(&em);
					em.page += (em.count >> 8);
					em.page == emlastpage ? length = lastlength : length = em.count;

					if(cbm_write(3, buffer, length) != length){
						errc = 99;
						break;
					}
				}
				while(em.page < emlastpage);
			}
			#ifdef __C128__
				if(*(char *)0x00d7 == 0x80) *(char *)0xd030 = 1;
			#endif
		}
		else{
			if(srcdevice == destdevice) changeDir(destdevice, NULL);
			else{
				memset(srcpath->bottom, 0, sizeof(char));
				memset(srcpath->top, 0, sizeof(char));
				memset(destpath->bottom, 0, sizeof(char));
				memset(destpath->top, 0, sizeof(char));
			}
			if(ftype == CBM_T_REL){
				sprintf(srcstring, "%s%s,l", srcpath->top, currentfile);
				cbm_open(2, drive[srcdevice], 2, srcstring);
				cbm_open(5, drive[srcdevice], 15, "");

				command.position = 254;

				do{
					cbm_write(5, &command, sizeof command);
					cbm_read(5, &errn, sizeof errn);
					if(!(errn - 48)) break;
				}
				while (--command.position > 0);

				cbm_close(2);
				cbm_close(5);

				relsize = command.position;

				if(!relsize){
					strcpy(message, "Record not present");
					errc = 99;
				}
				else{
					sprintf(sbuffer, "Record Size %d", relsize);
					cprintf(sbuffer);

					delay(TIME);
					cclearxy(24, BOTTOM, 15);

					sprintf(srcstring, "%s%s,l,%c", srcpath->top, currentfile, relsize);
					sprintf(deststring, "%s%s,l,%c", destpath->top, currentfile, relsize);

					cbm_open(2, drive[srcdevice], 2, srcstring);
					cbm_open(3, drive[destdevice], 3, deststring);
					cbm_open(5, drive[srcdevice], 15, "");

					command.recnumber = 1;
					command.position = 0;

					while(1){
						gotoxy(32, BOTTOM);
						cputs("R ");

						cbm_write(5, &command, sizeof command);
						cbm_read(2, lbuffer, relsize);
						cbm_read(5, &errn, sizeof errn);

						if(errn - 48) break;

						gotoxy(33, BOTTOM);
						cputs("W ");

						cbm_write(3, lbuffer, relsize);
						++command.recnumber;
					}
				}
			}
			else{
				sprintf(srcstring, "%s%s", srcpath->top, currentfile);
				sprintf(deststring, "%s%s,%c", destpath->top, currentfile, shorttype[ftype]);

				cbm_open(2, drive[srcdevice], ftype ? CBM_SEQ : CBM_READ, srcstring);
				cbm_open(3, drive[destdevice], CBM_WRITE, deststring);
				cbm_open(5, drive[srcdevice], 15, "");

				do{
					gotoxy(32, BOTTOM);
					cputs("R  ");

					length = cbm_read(2, buffer, BUFFERSIZE);

					if(length >= 0){
						gotoxy(33, BOTTOM);
						cputs("W ");

						if(cbm_write(3, buffer, length) != length){
							errc = 99;
							break;
						}
					}
				}
				while(length == BUFFERSIZE);
			}
		}
		errc ? cputs("ERROR") : cputs("OK");
	}
	cbm_close(5);
	cbm_close(3);
	cbm_close(2);

	free(buffer);
	free(srcstring);
	free(deststring);

	return errc;
}

unsigned char diskCopy(unsigned char srcdevice, unsigned char destdevice){

	unsigned int br;
	unsigned char track;
	unsigned char sector;
	unsigned char itrack;
	unsigned char isector;
	char *buffer = (char *)malloc(257);
	struct errors{unsigned char track; unsigned char sector; struct errors *next;};
	struct errors *head = NULL;
	struct errors *current = NULL;

	track = diskType(srcdevice);
	itrack = diskType(destdevice);

	if(track & itrack){

		head = (struct errors *)malloc(sizeof(struct errors));
		current = head;

		sprintf(lbuffer, "Copying from drive %d to %d", drive[srcdevice], drive[destdevice]);
		printMessage(COLOR_TEXT, lbuffer);

		cbm_open(2, drive[srcdevice], 5, "#");
		cbm_open(3, drive[destdevice], 5, "#");
		cbm_open(4, drive[srcdevice], 15, "");
		cbm_open(5, drive[destdevice], 15, "");

		for(track = 0; track < itrack; ++track){

			itrack == 35 || itrack == 70 ? isector = sectors[track % 35] : isector = 40;
			for(sector = 0; sector < isector; ++sector){

				if(kbhit() && cgetc() == CH_ESC){
					track = itrack;
					break;
				}
				gotoxy(30, BOTTOM);
				cprintf("T:%02d S:%02d", track + 1, sector);

				cbm_write(4, lbuffer, sprintf(lbuffer, "u1:5 0 %d %d", track + 1, sector));
				cbm_write(5, "b-p:5 0", 7);
				cbm_read(2, buffer, 256);
				br = cbm_write(3, buffer, 256);
				cbm_write(5, lbuffer, sprintf(lbuffer, "u2:5 0 %d %d", track + 1, sector));

				if(br != 256){
					current->next = (struct errors *)malloc(sizeof(struct errors));
					current = current->next;
					current->track = track + 1;
					current->sector = sector;
				}
			}
		}
		cbm_close(5);
		cbm_close(4);
		cbm_close(3);
		cbm_close(2);

		if(head->next){
			current->next = NULL;
			printMessage(COLOR_SIGNAL, "Write errors occurred. Print");
			if(really()){
				current = head->next;
				clrscr();
				revers(0);
				textcolor(COLOR_TEXT);
				while(current){
					cprintf( "Track: %d Sector: %d\r\n", current->track, current->sector);
					current = current->next;
					if(wherey() == 24 || current == NULL){
						gotoxy(0, 24);
						if(waitKey(1) == CH_ESC) break;
						clrscr();
					}
				}
			}
		}
		while((current = head) != NULL){
			head = head->next;
			free(current);
		}
	}
	else{
		strcpy(message, "Incompatible drives/disks selected");
		return 99;
	}
	free(buffer);

	return 0;
}

void doMakeImage(void){

	unsigned char track;
	unsigned char sector;
	unsigned char isector;
	unsigned char itrack;
	char *pname = &name[0];

	printMessage(COLOR_TEXT, "Name:                     <return> <esc>");
	memset(name, 0, sizeof name);

	if(textInput(6, BOTTOM, name) && !imgtype[device]){

		for(idx = 1; idx < sizeof imagetype / sizeof imagetype[0]; ++idx){
			if(strstr(strlower(strcpy(sbuffer, pname)), imagetype[idx])) break;
		}
		switch(idx){

			case D1541:
				itrack = 35;
				break;

			case D1571:
				itrack = 70;
				break;

			case D1581:
				itrack = 80;
				break;

			default:
				printMessage(COLOR_SIGNAL, "Image type not supported");
				clearBar();
				return;
		}
		if(dosCommand(3, drive[device], CBM_READ, name) != 62){
			printMessage(COLOR_SIGNAL, "File exists");
			clearBar();
			return;
		}
		cbm_open(3, drive[device], CBM_WRITE, name);
		cclearxy(23, BOTTOM, 17);
		memset(lbuffer, 0, sizeof lbuffer);
		for(track = 0; track < itrack; ++track){
			itrack == 35 || itrack == 70 ? isector = sectors[track % 35] : isector = 40;

			for(sector = 0; sector < isector; ++sector){
				if(kbhit() && cgetc() == CH_ESC){
					track = itrack;
					break;
				}
				gotoxy(30, BOTTOM);
				cprintf("T:%02d S:%02d", track + 1, sector);
				cbm_write(3, lbuffer, 256);
			}
		}
		cbm_close(3);

		sprintf(sbuffer, "cd:%s", name);
		if(!dosCommand(15, drive[device], 15, sbuffer)){

			printMessage(COLOR_TEXT, "Name:                     <return> <esc>");
			memset(name, 0, sizeof name);

			sprintf(sbuffer, "n:%s", textInput(6, BOTTOM, name) ? name : "");
			dosCommand(15, drive[device], 15, sbuffer);

			sprintf(sbuffer, "cd:%s", leftarrow);
			dosCommand(15, drive[device], 15, sbuffer);
		}
		else printMessage(COLOR_SIGNAL, "Can't format");
		refreshDir();
		clearBar();
	}
	else clearBar();
}

void doRelabel(void){

	unsigned char track;
	unsigned char sector;
	unsigned char offset;
	unsigned int length = 0;
	char *buffer = (char *)malloc(257);

	if(idx = diskType(device)){

		switch(idx){

			case 70:
			case 35:
				track = 18;
				sector = 0;
				offset = 0x90;
				break;

			case 80:
				track = 40;
				sector = 0;
				offset = 0x04;
				break;

			default:
				track = 1;
				sector = 1;
				offset = 0x04;
		}

		cbm_open(2, drive[device], 5, "#");
		cbm_open(4, drive[device], 15, "");

		memset(name, 0, sizeof name);
		printMessage(COLOR_TEXT, "Label:                    <return> <esc>");
		cbm_write(4, lbuffer, sprintf(lbuffer, "u1:5 0 %d %d",track, sector));
		cbm_read(2, buffer, 256);

		for(length = 0; buffer[offset + length] != 0xa0 && length < 16; ++length) name[length] = buffer[offset + length];

		length = textInput(7, BOTTOM, name);

		if(length){

			memset(buffer + offset, 0xa0, 16);
			strncpy(buffer + offset, name, length);
			cbm_write(4, "b-p:5 0", 7);
			length = cbm_write(2, buffer, 256);
			cbm_write(4, lbuffer, sprintf(lbuffer, "u2:5 0 %d %d", track, sector));

			if(length != 256) printMessage(COLOR_SIGNAL, "Relabel error");

			refreshDir();
		}
		cbm_close(4);
		cbm_close(2);
	}
	else printMessage(COLOR_SIGNAL, "Can't relabel");

	clearBar();
	free(buffer);

	return;
}
