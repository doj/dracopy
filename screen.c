/*
 * screen.c
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
#include <conio.h>
#include <string.h>
#include <defines.h>
#include <base.h>

static unsigned char screen_bgc;
static unsigned char screen_borderc;
static unsigned char screen_textc;

void drawFrame(char *title, unsigned char xpos, unsigned char ypos, unsigned char xsize, unsigned char ysize){
	
	unsigned char h1 = 0;
	
	gotoxy(xpos, ypos);
	cputc(CH_ULCORNER);
	
	if(title) {
		h1 = (xsize - 2 - strlen(title)) / 2;
		chline(h1);
		
		revers(1);
		cputs(title);
		revers(0);
		
		chline(xsize - h1 - strlen(title) - 2);
	}
	else chline(xsize - 2);
	
	cputc(CH_URCORNER);
	cvlinexy(xpos, ypos + 1, ysize - 2);
	
	cputc(CH_LLCORNER);
	chline(xsize - 2);
	
	cputc(CH_LRCORNER);
	cvlinexy(xpos + xsize - 1, ypos + 1, ysize - 2);
}

void initScreen(unsigned char border, unsigned char bg, unsigned char text){
	
	screen_borderc = bordercolor(border);
	screen_bgc = bgcolor(bg);
	screen_textc = textcolor(text);
	
	clrscr();
}

void exitScreen(void){
	
	#ifdef __C128__
		if(*(char *)0x00d7 == 128){
			screen_bgc = COLOR_BACKGROUND;
			screen_textc = COLOR_TEXT;
		}
	#endif
	bordercolor(screen_borderc);
	bgcolor(screen_bgc);
	textcolor(screen_textc);
	clrscr();
}
