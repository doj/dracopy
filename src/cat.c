/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */
/*
 * cat.c
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
 *
 */
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dirent.h>
#include <cbm.h>
#include <sys/types.h>
#include <stdlib.h>
#include "cat.h"
#include "base.h"
#include "defines.h"

#define x2(x) (buffer[x] < 10 ? "0" : ""), buffer[x]

int cathex(BYTE device, char * filename)
{
	unsigned int c;
	BYTE pchar = 0;
	BYTE cnt = 0;
	int offset = 0;
	unsigned int len;
	char buffer[8];

	if( cbm_open (6,device,CBM_READ,filename) != 0)
	{
		cputs("Can't open input file!\n");
	}
	else
	{
		offset=0;
	 	clrscr();
		do
		{
			if (cnt==0)
			{
			 	clrscr();
			}
			len = cbm_read (6, buffer, sizeof(buffer));
			cputhex16(offset);
			cputc(' ');
			for (c=0;c<len;c++)
			{
				cputhex8(buffer[c]);
				//cprintf("%02x",buffer[c]);
			}
			cputs(" : ");
			for (c=0;c<len;c++)
			{
				pchar=buffer[c];
				if (pchar==10 || pchar ==13)
				{
					pchar='.';
				}
				cputc(filterchar(pchar));
			}

			cputc(13);
			cputc(10);
			cnt++;
			offset+=sizeof(buffer);
			if (cnt==24 || (len!=sizeof(buffer)))
			{
				cnt=0;
				gotoxy(0,24);
				c=waitKey(1);
			}
	    }
	    while(len==sizeof(buffer) && c!=3 && c!='q');
		cbm_close (6);
	}

	return 0;
}

int catasc(BYTE device, char * filename)
{
	unsigned int c;
	BYTE pchar = 0;
	BYTE cnt = 0;
	int offset = 0;
	unsigned int len;
	char buffer[8];

	if( cbm_open (6,device,CBM_READ,filename) != 0)
	{
		cputs("Can't open input file!\n");
	}
	else
	{
		offset=0;
	 	clrscr();
		do
		{
			len = cbm_read (6, buffer, sizeof(buffer));
			for (c=0;c<len;c++)
			{
				pchar=buffer[c];
				if (pchar==13 || pchar==10 )
				{
					cputc(13);
					cputc(10);
				}
				else if (pchar==9) // tab
				{
					cputs("    ");
				}
				else
				{
					if (! (	pchar==13 ||
                  pchar==10 ||
                  (pchar >= 0x20 && pchar <= 0x7f) ||
                  (pchar >= 0xA0) ) )
					{
						pchar = '.';
					}
					if ( pchar>=65 && pchar<=90)
					{
						pchar = pchar+32;
					}
					else if (pchar>=97 && pchar <=122 )
					{
						pchar = pchar-32;
					}

					cputc(pchar);
				}
			}

			offset+=sizeof(buffer);
			if (wherey()>23 || (len!=sizeof(buffer)))
			{
				gotoxy(0,24);
				c=waitKey(1);
			 	clrscr();
			}
	    }
	    while(len==sizeof(buffer) && c!=3 && c!='q');
		cbm_close (6);
	}

	return 0;
}

BYTE filterchar(BYTE pchar)
{
	if (! (	pchar==13 ||
          pchar==10 ||
          (pchar >= 0x20 && pchar <= 0x7f) ||
          (pchar >= 0xA0) ) )
	{
		pchar = '.';
	}
	if ( pchar>=65 && pchar<=90)
	{
		pchar = pchar+32;
	}
	else if (pchar>=97 && pchar <=122 )
	{
		pchar = pchar-32;
	}
	return pchar;
}
