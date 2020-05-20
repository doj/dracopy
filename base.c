/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */
/*
 * base.c
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
#include <conio.h>
#include <ctype.h>
#include "base.h"

unsigned char waitKey(unsigned char flag)
{
	revers(1);
	textcolor(COLOR_SIGNAL);
	cputs("    Press any key    ");
	if(flag) cputs(" <esc> to quit     ");
	revers(0);
	textcolor(COLOR_TEXT);
	return cgetc();
}

unsigned char really(void){

	char c;

	cputs(" (y/n)? ");
	c = cgetc();
	return (c == 'y');
}

unsigned char textInput(unsigned char xpos, unsigned char ypos, char *name){

	unsigned char idx;
	char c;

	idx = strlen(name);
	cursor(1);
	cputsxy(xpos, ypos, name);
	gotoxy(xpos + idx, ypos);

	while(1){
		c = cgetc();
		switch (c){

		case CH_ESC:
			cursor(0);
			return(0);

		case CH_ENTER:
			name[strlen(name)] = '\0';
			cursor(0);
			return(idx);

		case CH_DEL:
			if(idx){
				--idx;
				if(idx == (strlen(name) - 1)) name[idx] = '\0';
				else name[idx] = ' ';
				cputcxy(xpos + idx, ypos, ' ');
				gotoxy(xpos + idx, ypos);
			}
			break;

		case CH_CURS_LEFT:
			if(idx){
				--idx;
				gotoxy(xpos + idx, ypos);
			}
			break;

		case CH_CURS_RIGHT:
			if(idx < strlen(name)){
				++idx;
				gotoxy(xpos + idx, ypos);
			}
			break;

		default:
			if(idx == 16) continue;
			if(isprint(c) || isdigit(c)){
				name[idx] = c;
				cputc(c);
				++idx;
				break;
			}
			break;
		}
	}
}

void delay(unsigned int ti){

	while(--ti);
}
