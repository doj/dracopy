/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */
/*
 * base.c
 *
 *  Created on: 3.01.2009
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

#include "base.h"
#include <stdio.h>
#include <conio.h>

BYTE waitKey(BYTE flag)
{
	revers(1);
#ifndef __cbm610__
	textcolor(COLOR_VIOLET);
#endif
	cputs("PRESS A KEY");
	if (flag)
	{
		cputs(" (RUNSTOP or Q to stop)");
	}
#ifndef __cbm610__
	textcolor(COLOR_LIGHTGREEN);
#endif
	revers(0);
	return cgetc();
}
