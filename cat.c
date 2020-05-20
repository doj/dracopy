/*
 * cat.c
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
#include <base.h>

void cathex(unsigned char drive, char *currentfile, unsigned char ftype){
	
	unsigned char c;
	unsigned char pchar;
	unsigned char length;
	unsigned int offset = 0;
	char cbuffer[8];
	
	cbm_open(6, drive, ftype ? CBM_SEQ : CBM_READ, currentfile);
	clrscr();
	do{
		length = cbm_read(6, cbuffer, sizeof cbuffer);
		cputhex16(offset);
		cputc(0x20);
		cputc(0x20);
		for(c = 0; c < length; ++c){
			cputhex8(cbuffer[c]);
			cputc(0x20);
		}
		cputcxy(30, wherey(), 0x3a);
		cputc(0x20);
		revers(1);
		for(c = 0; c < length; ++c){
			pchar = cbuffer[c];
			if(pchar == 0x00 || pchar == 0x0a || pchar == 0x0d || (pchar <= 0x20 && pchar >= 0x7f) || (pchar <= 0xa0 && pchar >= 0xff)) pchar = 0x2e;
			if(pchar >= 0x41 && pchar <= 0x5a) pchar = pchar + 0x20;
			else if(pchar >= 0x61 && pchar <= 0x7a) pchar = pchar - 0x20;
			cputc(pchar);
		}
		revers(0);
		if(wherex() == 40) cputs("\n\r");
		offset += sizeof cbuffer;
		if(wherey() == 24 || length != sizeof cbuffer){
			gotoxy(0, 24);
			if(waitKey(1) == CH_ESC){
				cbm_close(6);
				return;
			}
			clrscr();
		}
	}
	while(length == sizeof cbuffer);
	cbm_close(6);
	return;
}

void catasc(unsigned char drive, char *currentfile, unsigned char ftype){
	
	unsigned char pchar;
	unsigned char length;
	unsigned char c;
	char last;
	char cbuffer[40];
	
	cbm_open(6, drive, ftype ? CBM_SEQ : CBM_READ, currentfile);
	clrscr();
	do{
		length = cbm_read(6, cbuffer, sizeof cbuffer);
		for(c = 0; c < length; ++c){
			last = pchar;
			pchar = cbuffer[c];
			if((pchar == 0x0a && last != 0x0d) || (pchar == 0x0d && last != 0x0a)) cputs("\n\r");
			else if(pchar == 0x09) cputs("    ");
			else{
				if(pchar >= 0x41 && pchar <= 0x5a) pchar = pchar + 0x20;
				else if(pchar >= 0x61 && pchar <= 0x7a) pchar = pchar - 0x20;
				cputc(pchar);
			}
			if(wherey() == 24){
				gotoxy(0, 24);
				if(waitKey(1) == CH_ESC){
					cbm_close(6);
					return;
				}
				clrscr();
			}
		}
		if(length != sizeof cbuffer){
			gotoxy(0, 24);
			waitKey(1);
		}
	}
	while(length == sizeof cbuffer);
	cbm_close(6);
	return;
}
